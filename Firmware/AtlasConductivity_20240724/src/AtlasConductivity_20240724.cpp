#include "Particle.h"
#include "SdFat.h"
#include <Ezo_i2c.h> // Include the EZO I2C library
#include <Wire.h>    // Include Arduino's I2C library
#include <sequencer2.h> // Imports a 2-function sequencer
#include <Ezo_i2c_util.h> // Brings in common print statements

//Initialize I2C addresses for the probes 
Ezo_board RTD = Ezo_board(102, "RTD");       //create a RTD circuit object, who's address is 102 and name is "RTD"
Ezo_board EC = Ezo_board(100, "EC");      //create an EC circuit object who's address is 100 and name is "EC"

//forward declarations of functions to use them in the sequencer before defining them
void step1(); 
void step2();
int secondsUntilNextEvent();

Sequencer2 Seq(&step1, 1000, &step2, 0);  //calls the steps in sequence with time in between them

SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

// SD SPI Configuration Details
const int SD_CHIP_SELECT = D5;
SdFat sd;
File myFile;

// Various timing constants
const unsigned long MAX_TIME_TO_PUBLISH_MS = 20000; // Only stay awake for this time trying to connect to the cloud and publish
//const unsigned long TIME_AFTER_PUBLISH_MS = 4000; // After publish, wait 4 seconds for data to go out
const unsigned long SECONDS_BETWEEN_MEASUREMENTS = 360; // 360 for deployments

// State variables
enum State {
  DATALOG_STATE,
  PUBLISH_STATE,
  SLEEP_STATE
};
State state = DATALOG_STATE;

// Define whether to publish, 1, or not, 0 
#define PUBLISHING 1

//Other definitions
unsigned long stateTime = 0;
long real_time;
int millis_now;
double temp; 
double cond; 
char data[120];
float rtd;
float ec_float; 

// Global objects
SerialLogHandler logHandler;
SystemSleepConfiguration config;
const char *eventName = "cond";
FuelGauge batteryMonitor;

void setup() {
  if (PUBLISHING == 1) {
    Particle.connect();
  } else {
    Cellular.off(); // Turn off cellular for preliminary testing
  }
  delay(3000);
  Wire.begin();                           //start the I2C
  Serial.begin(9600);                     //start the serial communication to the computer
  Seq.reset();                            //initialize the sequencer

  //Initialize sd card
  if (!sd.begin(SD_CHIP_SELECT))
  {
    sd.initErrorHalt();
  }
  // open the file for write at end like the "Native SD library"
  if (!myFile.open("conductivity.csv", O_RDWR | O_CREAT | O_AT_END))
  {
    Serial.println("opening test.csv for write failed");
  } else {
    Serial.println("Writing to sd card"); 
    myFile.println("Real_Time,Temp_C,Cond_uScm-1"); //printing headers 
    myFile.close(); 
  }
}

void loop() {
  switch (state) {
    case DATALOG_STATE: {
      //Seq.run(); // Run the sequencer
      delay(1000);
      step1(); 
      delay(1000); 
      step2(); 
      delay(300);
      temp = RTD.get_last_received_reading(); 
      cond = EC.get_last_received_reading(); 
    
      real_time = Time.now(); // Real time for logging
      millis_now = millis();

      float cellVoltage = batteryMonitor.getVCell();
      float stateOfCharge = batteryMonitor.getSoC();

      snprintf(data, sizeof(data), "%li,%.2f,%.2f,%.2f,%.2f", real_time, temp, cond, cellVoltage, stateOfCharge); 
      delay(1000); 
      Serial.println(data);
      
      //Save data to SD card
      if (!sd.begin(SD_CHIP_SELECT, SPI_FULL_SPEED)) {
        Serial.println("failed to open card");
      }
      delay(1000);
      if (!myFile.open("conductivity.csv", O_RDWR | O_CREAT | O_AT_END)) {
        Serial.println("opening conductivity.csv for write failed");
      } else {
        // Save to SD card
        myFile.print(data);
        myFile.print("\n"); // Put next data on a new line
        myFile.close();
      }

      if (PUBLISHING == 1) { //determine next state
        state = PUBLISH_STATE;
       } else {
        state = SLEEP_STATE;}
    }
    break;

    case PUBLISH_STATE: {
      // Prep for cellular transmission
      bool isMaxTime = false;
      stateTime = millis();
      while (!isMaxTime) {
        //connect particle to the cloud
        if (Particle.connected() == false) {
          Particle.connect();
          Serial.print("Trying to connect");
        }
        // If connected, publish data buffer
        if (Particle.connected()) {
          Serial.println("publishing data");
          // bool (or Future) below requires acknowledgment to proceed
          bool success = Particle.publish(eventName, data, 60, PRIVATE, WITH_ACK);
          Serial.printlnf("publish result %d", success);
          isMaxTime = true;
          state = SLEEP_STATE;
        }
        // If not connected after certain amount of time, go to sleep to save battery
        else {
          // Took too long to publish, just go to sleep
          if (millis() - stateTime >= MAX_TIME_TO_PUBLISH_MS) {
            isMaxTime = true;
            state = SLEEP_STATE;
            Serial.println("max time for publishing reached without success; go to sleep");
          }
          Serial.println("Not max time, try again to connect and publish");
          delay(500);
        }
      }
    }
    break;
    
    case SLEEP_STATE:{
      Serial.println("going to sleep");
      delay(500);
  
      // Sleep time determination and configuration
      int wakeInSeconds = secondsUntilNextEvent(); // Calculate how long to sleep 
  
      config.mode(SystemSleepMode::ULTRA_LOW_POWER)
        .gpio(D2, FALLING)
        .duration(wakeInSeconds * 1000L) // Set seconds until wake
        .network(NETWORK_INTERFACE_CELLULAR, SystemSleepNetworkFlag::INACTIVE_STANDBY); // keeps the cellular modem powered, but does not wake the MCU for received data
  
      // Ready to sleep
      SystemSleepResult result = System.sleep(config); // Device sleeps here
  
      // It'll only make it here if the sleep call doesn't work for some reason (UPDATE: only true for hibernate. ULP will wake here.)
      Serial.print("Feeling restless");
      stateTime = millis();
      state = DATALOG_STATE;
    }
    break;
  }                             
}

void step1(){
  //send a read command. we use this command instead of ETD.send_cmd("R"); 
  //to let the library know to parse the reading
  RTD.send_read_cmd();    
  Serial.print("test1 ");                   
  EC.send_read_cmd();
  Serial.println("test2"); 
}

void step2(){
  receive_and_print_reading(RTD);             //get the reading from the PH circuit
  Serial.print("test3 ");
  receive_and_print_reading(EC);             //get the reading from the EC circuit
  Serial.println("test4");
}

// Function to calculate seconds until the next event
int secondsUntilNextEvent() {
  int current_seconds = Time.now();
  int seconds_to_sleep = SECONDS_BETWEEN_MEASUREMENTS - (current_seconds % SECONDS_BETWEEN_MEASUREMENTS);
  Serial.print("Sleeping for");
  Serial.println(seconds_to_sleep);
  return seconds_to_sleep;
}
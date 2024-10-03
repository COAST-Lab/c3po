#include "Particle.h"
#include "SdFat.h"
#include <Ezo_i2c.h> // Include the EZO I2C library
#include <Wire.h>    // Include Arduino's I2C library
#include <sequencer2.h> // Imports a 2-function sequencer
#include <Ezo_i2c_util.h> // Brings in common print statements

// SD SPI Configuration Details
const int SD_CHIP_SELECT = D5;
SdFat sd;

// Forward declarations of functions
void step1();
void step2();
int secondsUntilNextEvent();

// Project-specific
Ezo_board EC = Ezo_board(100, "EC");       //create a PH circuit object, who's address is 99 and name is "PH"
Ezo_board RTD = Ezo_board(102, "RTD"); 

// Sequencer setup
Sequencer2 Seq(&step1, 1000, &step2, 0);  // Calls the steps in sequence with time in between them

// Various timing constants
const unsigned long MAX_TIME_TO_PUBLISH_MS = 20000; // Only stay awake for this time trying to connect to the cloud and publish
// const unsigned long TIME_AFTER_PUBLISH_MS = 4000; // After publish, wait 4 seconds for data to go out

// ***** IMPORTANT!!!
// If SECONDS_BETWEEN_MEASUREMENTS < 600, must use 
//.network(NETWORK_INTERFACE_CELLULAR, SystemSleepNetworkFlag::INACTIVE_STANDBY);
// in sleep configuration to avoid reconnection penalty
const unsigned long SECONDS_BETWEEN_MEASUREMENTS = 360; // Assuming same as TinyCamML?
// ***** IMPORTANT!!! See note above this const.

// State variables
enum State {
  DATALOG_STATE,
  PUBLISH_STATE,
  SLEEP_STATE
};
State state = DATALOG_STATE;

// Define whether to publish
#define PUBLISHING 1


unsigned long stateTime = 0;
char data[120];
char rtd[24];
float ec_float; // Float variable to hold the float value

// Turn off cellular for preliminary testing; turn on for deployment
//SYSTEM_MODE(MANUAL); // Uncomment for preliminary testing
SYSTEM_MODE(SEMI_AUTOMATIC); // Uncomment for deployment
SYSTEM_THREAD(ENABLED);

// Global objects
SerialLogHandler logHandler;
SystemSleepConfiguration config;
const char *eventName = "cond";

long real_time;
int millis_now;
double temp; 
double cond; 

// Setup function
void setup() {
  if (PUBLISHING == 1) {
    Particle.connect();
  } else {
    Cellular.off(); // Turn off cellular for preliminary testing
  }

  Serial.begin(9600); // Enable serial port
  Wire.begin();       // Enable I2C port
  Log.info("Conductivity and Temperature Sensor");
}

// Serial event function
void serialEvent() {
  char computerdata[20];
  byte received_from_computer = Serial.readBytesUntil(13, computerdata, 20); // Read data until <CR>
  computerdata[received_from_computer] = 0; // Null-terminate the string
}

// Step 1 function
void step1() {
  RTD.send_read_cmd(); // Send read command to RTD
  EC.send_read_cmd();  // Send read command to EC
}

// Step 2 function
void step2() {
  receive_and_print_reading(RTD); // Get reading from RTD circuit
  Log.info(" ");
  receive_and_print_reading(EC);  // Get reading from EC circuit
  Log.info(" ");
  temp = RTD.get_last_received_reading(); 
  cond = EC.get_last_received_reading(); 

  real_time = Time.now(); // Real time for logging
  millis_now = millis();

  // Start SD stuff
  File myFile;

  // Initialize the library
  if (!sd.begin(SD_CHIP_SELECT, SPI_FULL_SPEED)) {
    Log.info("failed to open card");
  }

  delay(2000);

  // Open the file for write
  if (!myFile.open("conductivity.csv", O_RDWR | O_CREAT | O_AT_END)) {
    Log.info("opening conductivity.csv for write failed");
  } else {
    // Save to SD card
   // myFile.print(data);
    myFile.print(real_time);
    myFile.print(",");
    myFile.print(temp);
    myFile.print(",");
    myFile.print(cond);
    myFile.print("\n"); // Put next data on a new line
    myFile.close();
  }

  delay(2000);

  // Determine next state
  if (PUBLISHING == 1) {
    state = PUBLISH_STATE;
  } else {
    state = SLEEP_STATE;
  }
}

// Publish State
void publishState() {
  bool isMaxTime = false;
  stateTime = millis();

  while (!isMaxTime) {
    if (!Particle.connected()) {
      Particle.connect();
      Log.info("Trying to connect");
    }

    if (Particle.connected()) {
      Log.info("publishing data");
      snprintf(data, sizeof(data), "%li,%.5f,%.02f", 
      real_time, // if it takes a while to connect, this time could be offset from sensor recording
      temp,
      cond
    );

    delay(2000);

      bool success = Particle.publish(eventName, data, PRIVATE, WITH_ACK); // infor that will be publish, "data" defined earlier
      Log.info("publish result %d", success);

    delay(2000);
    
      isMaxTime = true;
      state = SLEEP_STATE;
    } else {
      if (millis() - stateTime >= MAX_TIME_TO_PUBLISH_MS) {
        isMaxTime = true;
        state = SLEEP_STATE;
        Log.info("max time for publishing reached without success; go to sleep");
      }
      Log.info("Not max time, try again to connect and publish");
      delay(500);
    }
  }
}

// Sleep State
void sleepState() {
  Log.info("going to sleep");
  delay(500);

  int wakeInSeconds = secondsUntilNextEvent(); // Calculate how long to sleep

  config.mode(SystemSleepMode::ULTRA_LOW_POWER)
    .gpio(D2, FALLING)
    .duration(wakeInSeconds * 1000L); // Set seconds until wake

  RTD.send_cmd("sleep");  // This will likely send the correct I2C sleep command.
  EC.send_cmd("sleep"); //Decrease energy output from 16.85 mA to 0.5 mA
  
  SystemSleepResult result = System.sleep(config); // Device sleeps here

  Log.info("Feeling restless");
  stateTime = millis();
  state = DATALOG_STATE;
}

// Main loop function
void loop() {
  switch (state) {
    case DATALOG_STATE:
      // Execute data logging tasks
      Seq.run(); // Run the sequencer
      break;

    case PUBLISH_STATE:
      publishState(); // Handle publishing
      break;

    case SLEEP_STATE:
      sleepState(); // Handle sleeping
      break;
  }
}

// Function to calculate seconds until the next event
int secondsUntilNextEvent() {
  int current_seconds = Time.now();
  int seconds_to_sleep = SECONDS_BETWEEN_MEASUREMENTS - (current_seconds % SECONDS_BETWEEN_MEASUREMENTS);
  Log.info("Sleeping for %i", seconds_to_sleep);
  return seconds_to_sleep;
}







/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "SdFat.h"


 //------------------SD SPI Configuration Details--------------------------------
const int SD_CHIP_SELECT = D5;
SdFat sd;

// function prototype` <br />
 int secondsUntilNextEvent();

// Project specific
#define address 100              //default I2C ID number for EZO pH Circuit.
#define Ezo_board EC = Ezo_board(100, "EC");

//------------------State variables
//not yet used but placeholders in case of additional states
enum State {
  DATALOG_STATE,
  PUBLISH_STATE,
  SLEEP_STATE
};
State state = DATALOG_STATE;

// Define whether (1) or not (0) to publish
#define PUBLISHING 0

unsigned long stateTime = 0;
char data[120];

//------------------Turn off cellular for prelim testing; turn on for deployment
SYSTEM_MODE(MANUAL); // uncomment for prelim testing
//SYSTEM_MODE(SEMI_AUTOMATIC); // uncomment for deployment
SYSTEM_THREAD(ENABLED);

// Global objects
SerialLogHandler logHandler;
SystemSleepConfiguration config;
const char * eventName = "ec";

void setup(void);
void loop(void);
void string_pars();
long real_time;
int millis_now;


// Various timing constants
const unsigned long MAX_TIME_TO_PUBLISH_MS = 20000; // Only stay awake for this time trying to connect to the cloud and publish
// const unsigned long TIME_AFTER_PUBLISH_MS = 4000; // After publish, wait 4 seconds for data to go out

// ***** IMPORTANT!!!
// If SECONDS_BETWEEN_MEASUREMENTS < 600, must use 
//.network(NETWORK_INTERFACE_CELLULAR, SystemSleepNetworkFlag::INACTIVE_STANDBY);
// in sleep configuration to avoid reconnection penalty
const unsigned long SECONDS_BETWEEN_MEASUREMENTS = 360; // Assuming same as TinyCamML?
// ***** IMPORTANT!!! See note above this const.


void setup(void) {
  if (PUBLISHING==1) {
    Particle.connect();
  }
  else{
    Cellular.off(); // turn off cellular for prelim testing (uncomment)
  }

  // delay(5000); // to see response from begin command

  Serial.begin(9600);            //enable serial port.
  Wire.begin();                  //enable I2C port.
  Log.info("Conductivity Sensor");

}
<<<<<<< HEAD

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
=======
>>>>>>> parent of acf5ca1 (rtd/publish changes)
    
    char computerdata[20];           //we make a 20 byte character array to hold incoming data from a pc/mac/other.
    byte received_from_computer = 0; //we need to know how many characters have been received.
    bool serial_event = false;       //a flag to signal when data has been received from the pc/mac/other.
    byte code = 0;                   //used to hold the I2C response code.
    char ec_data[24];                //we make a 24 byte character array to hold incoming data from the pH circuit.
    byte in_char = 0;                //used as a 1 byte buffer to store in bound bytes from the EC Circuit.
    byte i = 0;                      //counter used for ec_data array.
    int delay_time = 1400;           //used to change the delay needed depending on the command sent to the EZO Class pH Circuit.

    char *ec;                        //char pointer used in string parsing.

    float ec_float;                  //float var used to hold the float value of the pH.

    void serialEvent() {           //this interrupt will trigger when the data coming from the serial monitor(pc/mac/other) is received.
      received_from_computer = Serial.readBytesUntil(13, computerdata, 20); //we read the data sent from the serial monitor(pc/mac/other) until we see a <CR>. We also count how many characters have been received.
      computerdata[received_from_computer] = 0; //stop the buffer from transmitting leftovers or garbage.
      serial_event = true;           //set the serial event flag.
    } 

void loop(void) {
  // Enter state machine
  switch (state) {


/////////////////////////DATALOG STATE////////////////////////
  case DATALOG_STATE: {
    void loop(); {                   //the main loop will take a reading every 5 seconds.

//    computerdata[0] = 'r'; //Take a reading
    computerdata[0] = 'R';
    delay_time=1800; // Need to set the delay to 1.8 seconds when taking a reading

    delay(5000);

    Wire.beginTransmission(address);            //call the circuit by its ID number.
    Wire.write(computerdata);                   //transmit the command that was sent through the serial port.
    Wire.endTransmission();                     //end the I2C data transmission.

    delay(delay_time);                        //wait the correct amount of time for the circuit to complete its instruction.

    Wire.requestFrom(address, 24, 1);          //call the circuit and request 24 bytes (this is more then we need).
    code = Wire.read();                        //the first byte is the response code, we read this separately.
    delay(6000);

  Wire.write("RTD sleep"); // Send sleep command to RTD
  Wire.write("EC sleep");  // Send sleep command to EC                       //Decrease energy output from 16.85 mA to 0.5 mA

    while (Wire.available()) {                  //are there bytes to receive.
      in_char = Wire.read();                    //receive a byte.
      ec_data[i] = in_char;                     //load this byte into our array.
      i += 1;                                   //incur the counter for the array element.
      if (in_char == 0) {                       //if we see that we have been sent a null command.
        i = 0;                                  //reset the counter i to 0.
        Wire.endTransmission();                 //end the I2C data transmission.
        break;                                  //exit the while loop.
      }
    }
       
    delay(1000);

    switch (code) {                           //switch case based on what the response code is.
      case 1:                                 //decimal 1.
        Log.info("Success");            //means the command was successful.
        break;                                //exits the switch case.

      case 2:                                 //decimal 2.
        Log.info("Failed");             //means the command has failed.
        break;                                //exits the switch case.

      case 254:                               //decimal 254.
        Log.info("Pending");            //means the command has not yet been finished calculating.
        break;                                //exits the switch case.

      case 255:                               //decimal 255.
        Log.info("No Data");            //means there is no further data to send.
        break;                                //exits the switch case.
      }

       delay(1000); 

    Log.info(ec_data);                  //print the data.
    serial_event = false;                     //reset the serial event flag.

    //if(computerdata[0]=='R') string_pars(); //Call the String_Pars() function to break up the comma separated ec_data string into its individual parts.
    
    delay(5000-delay_time); //this will pause until 5 seconds have elapsed from previous reading
  }

    void string_pars(); {                  //this function will break up the CSV string into its 4 individual parts. EC|TDS|SAL|SG.
                                      //this is done using the C command “strtok”.

      ec = strtok(ec_data, ",");          //let's pars the string at each comma.

      Log.info("ec: %s", ec);

  //uncomment this section if you want to take the values and convert them into floating point number.
  /*
  ec_float=atof(ec);
  tds_float=atof(tds);
  sal_float=atof(sal);
  sg_float=atof(sg);
  */

  real_time = Time.now(); // "Real" time and current millis for logging
  millis_now = millis();

   snprintf(data, sizeof(data), "%li,%s", 
   real_time, 
   ec
   ); 

    delay(2000);

 // Start SD stuff
    File myFile;

    // Initialize the library
    if (!sd.begin(SD_CHIP_SELECT, SPI_FULL_SPEED)) {
      Log.info("failed to open card");
    }

    delay(2000);

    // open the file for write at end like the "Native SD library"
    if (!myFile.open("conductivity.csv", O_RDWR | O_CREAT | O_AT_END)) {
      Log.info("opening conductivity.csv for write failed");
    }
    else{ // if file does open, save to SD; otherwise, proceed to publish
      // Save to SD card
      myFile.print(data);
      myFile.print("\n"); // put next data on new line
      myFile.close();
    }

    delay(2000);

    if (PUBLISHING==1) {
      state = PUBLISH_STATE;
    }
    else{
      state = SLEEP_STATE;
    }
    }

    break;

/////////////////////// PUBLISH STATE ////////////////////////

  case PUBLISH_STATE: {

    // Prep for cellular transmission
    bool isMaxTime = false;
    stateTime = millis();

    while (!isMaxTime) {
      //connect particle to the cloud
      if (Particle.connected() == false) {
        Particle.connect();
        Log.info("Trying to connect");
      }

      // If connected, publish data buffer
      if (Particle.connected()) {

        Log.info("publishing data");

        // bool (or Future) below requires acknowledgment to proceed
        bool success = Particle.publish(eventName, ec_data, 60, PRIVATE, WITH_ACK);
        Log.info("publish result %d", success); 

        isMaxTime = true;
        state = SLEEP_STATE;
      }
      // If not connected after certain amount of time, go to sleep to save battery
      else {
        // Took too long to publish, just go to sleep
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
  
  break;

  //////////////////////////////////////**SLEEP_STATE ***//////////////////////////////////////////
  /*** Get here from PUBLISH_STATE after attempted publish or DATALOG_STATE if PUBLISHING==0
  ***/

  case SLEEP_STATE: {
    Log.info("going to sleep");
    delay(500);

    // Sleep time determination and configuration
    int wakeInSeconds = secondsUntilNextEvent(); // Calculate how long to sleep 

    config.mode(SystemSleepMode::ULTRA_LOW_POWER)
      .gpio(D2, FALLING)
      .duration(wakeInSeconds* 1000L); // Set seconds until wake
      // .network(NETWORK_INTERFACE_CELLULAR, SystemSleepNetworkFlag::INACTIVE_STANDBY); // keeps the cellular modem powered, but does not wake the MCU for received data

    // Ready to sleep
    SystemSleepResult result = System.sleep(config); // Device sleeps here

    // It'll only make it here if the sleep call doesn't work for some reason (UPDATE: only true for hibernate. ULP will wake here.)
    Log.info("Feeling restless");
    stateTime = millis();
    state = DATALOG_STATE;

  }
    break;  
  }
}
}

int secondsUntilNextEvent() {

  int current_seconds = Time.now();
  int seconds_to_sleep = SECONDS_BETWEEN_MEASUREMENTS - (current_seconds % SECONDS_BETWEEN_MEASUREMENTS);

  Log.info("Sleeping for %i", seconds_to_sleep);

  return seconds_to_sleep;
  }




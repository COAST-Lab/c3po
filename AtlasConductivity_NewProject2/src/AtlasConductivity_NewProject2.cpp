/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"

void setup(void);
void loop(void);
void string_pars();

SYSTEM_MODE(SEMI_AUTOMATIC); // uncomment for deployment
SYSTEM_THREAD(ENABLED);

// Project specific
#define address 100              //default I2C ID number for EZO pH Circuit.

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


void setup()                     //hardware initialization.
{
  Serial.begin(9600);            //enable serial port.
  Wire.begin();                  //enable I2C port.
}


void serialEvent() {           //this interrupt will trigger when the data coming from the serial monitor(pc/mac/other) is received.
  received_from_computer = Serial.readBytesUntil(13, computerdata, 20); //we read the data sent from the serial monitor(pc/mac/other) until we see a <CR>. We also count how many characters have been received.
  computerdata[received_from_computer] = 0; //stop the buffer from transmitting leftovers or garbage.
  serial_event = true;           //set the serial event flag.
}

void loop() {                   //the main loop will take a reading every 5 seconds.

//    computerdata[0] = 'r'; //Take a reading
    computerdata[0] = 'R';
    delay_time=1800; // Need to set the delay to 1.8 seconds when taking a reading

    Wire.beginTransmission(address);            //call the circuit by its ID number.
    Wire.write(computerdata);                   //transmit the command that was sent through the serial port.
    Wire.endTransmission();                     //end the I2C data transmission.

    delay(delay_time);                        //wait the correct amount of time for the circuit to complete its instruction.

    Wire.requestFrom(address, 24, 1);          //call the circuit and request 24 bytes (this is more then we need).
    code = Wire.read();                        //the first byte is the response code, we read this separately.
    
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
       
    switch (code) {                           //switch case based on what the response code is.
      case 1:                                 //decimal 1.
        Serial.println("Success");            //means the command was successful.
        break;                                //exits the switch case.

      case 2:                                 //decimal 2.
        Serial.println("Failed");             //means the command has failed.
        break;                                //exits the switch case.

      case 254:                               //decimal 254.
        Serial.println("Pending");            //means the command has not yet been finished calculating.
        break;                                //exits the switch case.

      case 255:                               //decimal 255.
        Serial.println("No Data");            //means there is no further data to send.
        break;                                //exits the switch case.
    }
  
    Serial.println(ec_data);                  //print the data.
    serial_event = false;                      //reset the serial event flag.

    if(computerdata[0]=='R') string_pars(); //Call the Strign_Pars() function to break up the comma separated ec_data string into its individual parts.
    
    delay(5000-delay_time); //this will pause until 5 seconds have elapsed from previous reading
  }

void string_pars() {                  //this function will break up the CSV string into its 4 individual parts. EC|TDS|SAL|SG.
                                      //this is done using the C command “strtok”.

  ec = strtok(ec_data, ",");          //let's pars the string at each comma.

  Serial.print("ec:");                //we now print each value we parsed separately.
  Serial.println(ec);                 //this is the pH value.
  //Spark.publish("ec", String(ec));
  

  //uncomment this section if you want to take the values and convert them into floating point number.
  /*
  ec_float=atof(ec);
  tds_float=atof(tds);
  sal_float=atof(sal);
  sg_float=atof(sg);
  */
}
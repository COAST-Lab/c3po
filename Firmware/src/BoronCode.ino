#include <Wire.h>

#define address 100  // I2C address of the EZO EC Circuit

char computerdata[32];           // Buffer for incoming serial data
int received_from_computer = 0;  // Number of characters received
bool serial_event = false;       // Flag for serial data received
byte code = 0;                   // Response code from I2C communication
char ec_data[32];                // Buffer for data from EC circuit
byte in_char = 0;                // Temporary storage for incoming bytes
int i = 0;                       // Counter for ec_data array
int time_ = 570;                 // Delay time depending on command

void setup() {
  Serial.begin(9600);  // Start serial communication
  Wire.begin();        // Start I2C communication
}

void serialEvent() {
  received_from_computer = Serial.readBytesUntil('\r', computerdata, 32); // Read until carriage return
  computerdata[received_from_computer] = '\0';  // Null terminate the string
  serial_event = true;  // Set serial event flag
}

void loop() {
  if (serial_event) {
    for (i = 0; i <= received_from_computer; i++) {
      computerdata[i] = tolower(computerdata[i]);
    }
    i = 0;

    if (computerdata[0] == 'c' || computerdata[0] == 'r')
      time_ = 570;
    else
      time_ = 250;

    Wire.beginTransmission(address);
    Wire.write((uint8_t*)computerdata, received_from_computer);  // Cast to uint8_t* for Particle compatibility
    Wire.endTransmission();

    if (strcmp(computerdata, "sleep") != 0) {
      delay(time_);

      Wire.requestFrom(address, 32, true);  // Request 32 bytes with stop bit
      code = Wire.read();

      switch (code) {
        case 1:
          Serial.println("Success");
          break;
        case 2:
          Serial.println("Failed");
          break;
        case 254:
          Serial.println("Pending");
          break;
        case 255:
          Serial.println("No Data");
          break;
      }

      i = 0;
      while (Wire.available()) {
        in_char = Wire.read();
        ec_data[i] = in_char;
        i++;
        if (in_char == 0) {
          i = 0;
          break;
        }
      }

      ec_data[i] = '\0';  // Null terminate ec_data
      Serial.println(ec_data);
      Serial.println();
    }

    serial_event = false;
  }
}

void string_pars() {
  char *ec = strtok(ec_data, ",");
  char *tds = strtok(NULL, ",");
  char *sal = strtok(NULL, ",");
  char *sg = strtok(NULL, ",");

  if (ec != NULL) {
    Serial.print("EC: ");
    Serial.println(ec);
  }
  if (tds != NULL) {
    Serial.print("TDS: ");
    Serial.println(tds);
  }
  if (sal != NULL) {
    Serial.print("Salinity: ");
    Serial.println(sal);
  }
  if (sg != NULL) {
    Serial.print("Specific Gravity: ");
    Serial.println(sg);
  }

  Serial.println();
}
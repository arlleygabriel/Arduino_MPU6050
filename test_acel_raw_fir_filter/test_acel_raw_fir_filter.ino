#include <Wire.h>
#include <I2Cdev.h>
int16_t acc_x, acc_y, acc_z;
float Oldacc_x=0, Oldacc_y=0, Oldacc_z=0;
float Nacc_x, Nacc_y, Nacc_z;


void setup() {
  Wire.begin();                                                        //Start I2C as master
  Serial.begin(57600);                                                 //Use only for debugging
  pinMode(13, OUTPUT);                                                 //Set output 13 (LED) as output
  setup_mpu_6050_registers();                                          //Setup the registers of the MPU-6050 (500dfs / +/-8g) and start the gyro
  digitalWrite(13, HIGH);                                              //Set digital output 13 high to indicate startup
  setXAccelOffset(-5251);
  setYAccelOffset(-3049);
  setZAccelOffset(1517);
}

void loop() {

  read_mpu_6050_data();
  Nacc_x = 0.90*Oldacc_x + 0.1*acc_x;
  //   y[n] = a*x[n-1] + b*x[n]    a + b = 1
  Oldacc_x = Nacc_x;
  Serial.print(acc_x);
  Serial.print(" ");
  Serial.println(Nacc_x);
  delay(5);
  
}


void read_mpu_6050_data(){                                             //Subroutine for reading the raw gyro and accelerometer data
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x3B);                                                    //Send the requested starting register
  Wire.endTransmission();                                              //End the transmission
  Wire.requestFrom(0x68,14);                                           //Request 14 bytes from the MPU-6050
  while(Wire.available() < 14);                                        //Wait until all the bytes are received
  acc_x = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_x variable
  acc_y = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_y variable
  acc_z = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_z variable
}

char tmp_str[7]; // temporary variable used in convert function
char* convert_int16_to_str(int16_t i) { // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}

void setup_mpu_6050_registers(){
  //Activate the MPU-6050
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x6B);                                                    //Send the requested starting register
  Wire.write(0x00);                                                    //Set the requested starting register
  Wire.endTransmission();                                              //End the transmission
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  
  Wire.write(0x1C);                                                    //Send the requested starting register
  Wire.write(0x18);                                                    //Set the requested starting register +-16g
  //Wire.write(0x11);                                                    //Set the requested starting register +- 8g
  //Wire.write(0x8);                                                    //Set the requested starting register +- 4g
  //Wire.write(0x0);                                                    //Set the requested starting register +- 2g
  Wire.endTransmission();                                               //End the transmission
  
  Wire.beginTransmission(0x68);
  Wire.write(0x1A);                                                     // Reg_Map page 6 DLPF (3 Register Map Add(Dec.)26 (26=1A)
  Wire.write(0x06);                                               // Setting the filter to 6 - Register Map last 3 bits (or 0b00000010 for 2)
  Wire.endTransmission();
}

void setXAccelOffset(int16_t offset) {
    I2Cdev::writeWord(0x68, 0x06, offset);
}

void setYAccelOffset(int16_t offset) {
    I2Cdev::writeWord(0x68, 0x08, offset);
}

void setZAccelOffset(int16_t offset) {
    I2Cdev::writeWord(0x68, 0x0A, offset);
}

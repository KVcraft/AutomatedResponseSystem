/*
  MPU6050 Raw

  A code for obtaining raw data from the MPU6050 module with the option to
  modify the data output format.

  Find the full MPU6050 library documentation here:
  https://github.com/ElectronicCats/mpu6050/wiki
*/
#include "I2Cdev.h"
#include "MPU6050.h"

/* MPU6050 default I2C address is 0x68*/
MPU6050 mpu;
//MPU6050 mpu(0x69);         //Use for AD0 high
//MPU6050 mpu(0x68, &Wire1); //Use for AD0 low, but 2nd Wire (TWI/I2C) object.

/* OUTPUT FORMAT DEFINITION----------------------------------------------------------------------------------
- Use "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated list of the accel 
X/Y/Z and gyro X/Y/Z values in decimal. Easy to read, but not so easy to parse, and slower over UART.

- Use "OUTPUT_BINARY_ACCELGYRO" to send all 6 axes of data as 16-bit binary, one right after the other. 
As fast as possible without compression or data loss, easy to parse, but impossible to read for a human. 
This output format is used as an output.
--------------------------------------------------------------------------------------------------------------*/ 
#define OUTPUT_READABLE_ACCELGYRO
//#define OUTPUT_BINARY_ACCELGYRO

int16_t ax, ay, az;
int16_t gx, gy, gz;
bool blinkState;

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver srituhobby = Adafruit_PWMServoDriver();

// Define servo motors
#define mainGate 0
#define hillGate 1
#define bunkDoorL 2
#define bunkDoorR 3
#define backDoor 4
#define frontDoor 5


// Function to convert degrees to PWM values
int angleToPWM(int angle) {
  return map(angle, 0, 180, 150, 600);
}

void setup() {
  /*--Start I2C interface--*/
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin(); 
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif

  Serial.begin(9600); //Initializate Serial wo work well at 8MHz/16MHz

  /*Initialize device and check connection*/ 
  Serial.println("Initializing MPU...");
  mpu.initialize();
  Serial.println("Testing MPU6050 connection...");
  if(mpu.testConnection() ==  false){
    Serial.println("MPU6050 connection failed");
    while(true);
  }
  else{
    Serial.println("MPU6050 connection successful");
  }

  /* Use the code below to change accel/gyro offset values. Use MPU6050_Zero to obtain the recommended offsets */ 
  Serial.println("Updating internal sensor offsets...\n");
  mpu.setXAccelOffset(0); //Set your accelerometer offset for axis X
  mpu.setYAccelOffset(0); //Set your accelerometer offset for axis Y
  mpu.setZAccelOffset(0); //Set your accelerometer offset for axis Z
  mpu.setXGyroOffset(0);  //Set your gyro offset for axis X
  mpu.setYGyroOffset(0);  //Set your gyro offset for axis Y
  mpu.setZGyroOffset(0);  //Set your gyro offset for axis Z
  /*Print the defined offsets*/
  Serial.print("\t");
  Serial.print(mpu.getXAccelOffset());
  Serial.print("\t");
  Serial.print(mpu.getYAccelOffset()); 
  Serial.print("\t");
  Serial.print(mpu.getZAccelOffset());
  Serial.print("\t");
  Serial.print(mpu.getXGyroOffset()); 
  Serial.print("\t");
  Serial.print(mpu.getYGyroOffset());
  Serial.print("\t");
  Serial.print(mpu.getZGyroOffset());
  Serial.print("\n");

  /*Configure board LED pin for output*/ 
  pinMode(LED_BUILTIN, OUTPUT);

    srituhobby.begin();
  srituhobby.setPWMFreq(60);

  // Set servos to initial positions
  srituhobby.setPWM(hillGate, 0, angleToPWM(90));     
  srituhobby.setPWM(mainGate, 0, angleToPWM(90)); 
  srituhobby.setPWM(bunkDoorL, 0, angleToPWM(90));   
  srituhobby.setPWM(bunkDoorR, 0, angleToPWM(90));
  srituhobby.setPWM(frontDoor, 0, angleToPWM(90));     
  srituhobby.setPWM(backDoor, 0, angleToPWM(90));

  pinMode(A0, INPUT);

}

void loop() {
  /* Read raw accel/gyro data from the module. Other methods commented*/
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  //mpu.getAcceleration(&ax, &ay, &az);
  //mpu.getRotation(&gx, &gy, &gz);

  /*Print the obtained data on the defined format*/
  #ifdef OUTPUT_READABLE_ACCELGYRO
    //Serial.println(ax); Serial.print("\t");

  #endif

  #ifdef OUTPUT_BINARY_ACCELGYRO
    Serial.write((uint8_t)(ax >> 8)); Serial.write((uint8_t)(ax & 0xFF));
    Serial.write((uint8_t)(ay >> 8)); Serial.write((uint8_t)(ay & 0xFF));
    Serial.write((uint8_t)(az >> 8)); Serial.write((uint8_t)(az & 0xFF));
    Serial.write((uint8_t)(gx >> 8)); Serial.write((uint8_t)(gx & 0xFF));
    Serial.write((uint8_t)(gy >> 8)); Serial.write((uint8_t)(gy & 0xFF));
    Serial.write((uint8_t)(gz >> 8)); Serial.write((uint8_t)(gz & 0xFF));
  #endif

  /*Blink LED to indicate activity*/
  blinkState = !blinkState;
  digitalWrite(LED_BUILTIN, blinkState);

  if(ax<5000){
    Serial.println("Risk of Earthquake!");

  //mainGate Open
  for (int pos = 90; pos <= 180; pos++) {
    srituhobby.setPWM(mainGate, 0, angleToPWM(pos));
    delay(20);
  }

  //frontDoor open
  for (int pos = 90; pos >= 0; pos--) {
    srituhobby.setPWM(frontDoor, 0, angleToPWM(pos));
    delay(20);
  }

  }

  if(int waterLevel=analogRead(A0)>700 ){
    Serial.println("Risk of Flood");

  //hillGate open
  for (int pos = 90; pos >= 0; pos--) {
    srituhobby.setPWM(hillGate, 0, angleToPWM(pos));
    delay(20);
  }

  //frontDoor open
  for (int pos = 90; pos >= 0; pos--) {
    srituhobby.setPWM(frontDoor, 0, angleToPWM(pos));
    delay(20);
  }
}

}



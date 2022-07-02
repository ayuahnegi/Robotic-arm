#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SoftwareSerial.h>

#define MIN_PULSE_WIDTH       650
#define MAX_PULSE_WIDTH       2350
#define FREQUENCY             50
 
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
SoftwareSerial Bluetooth(0, 1); // Arduino(RX, TX) - HC-05 Bluetooth (TX, RX)

int servoWaist = 0;
int servoShoulder = 1;
int servoElbow = 2;
int servoWrist = 3;
int servoGrip = 4;

int servo1Pos;  // current position
int servo2Pos;
int servo3Pos;
int servo4Pos;
int servo5Pos; 

int servo01P[50]; // for storing positions/steps
int servo02P[50];
int servo03P[50];
int servo04P[50];
int servo05P[50];

int index = 0;
String dataIn = "";
int speed = 20;
int delayTime = 20;


void setup() 
{
  // Setup PWM Controller object
  pwm.begin();
  pwm.setPWMFreq(FREQUENCY);
  Bluetooth.begin(38400); // Default baud rate of the Bluetooth module
  Bluetooth.setTimeout(1);
  delay(20);

  // Robot arm initial position
  servo1PPos = 90;
  moveMotorDeg(servo1Pos, servoWaist)
  servo2PPos = 150;
  moveMotorDeg(servo2Pos, servoShoulder)
  servo3PPos = 35;
  moveMotor(servo3Pos, servoElbow)
  servo4PPos = 85;
  moveMotor(servo4Pos, servoWrist)
  servo5PPos = 80;
  moveMotor(servo5Pos, servoGrip)

  }

// Function to move motor to specific position
void moveMotorDeg(int moveDegree, int motorOut)
{
  int pulse_wide, pulse_width;
 
  // Convert to pulse width
  pulse_wide = map(moveDegree, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  pulse_width = int(float(pulse_wide) / 1000000 * FREQUENCY * 4096);
  
  //Control Motor
  pwm.setPWM(motorOut, 0, pulse_width);

  // Add short delay
  delay(delayTime);
}


void loop() {  

  if (Bluetooth.available() > 0){
    dataIn = Bluetooth.readString();  // Read the data as string
    
    
    // If "Waist" slider has changed value - Move Servo 1 to position
    if (dataIn.startsWith("s1")) {
      String dataInS = dataIn.substring(2, dataIn.length()); // Extract only the number. E.g. from "s1120" to "120"
      servo1Pos = dataInS.toInt();  // Convert the string into integer
    
      moveMotorDeg(servo1Pos, servoWaist)
        
    }

    // Move Servo 2 - Shoulder Servo
    if (dataIn.startsWith("s2")) {
      String dataInS = dataIn.substring(2, dataIn.length());
      servo2Pos = dataInS.toInt();

      moveMotorDeg(servo2Pos, servoShoulder)
    }

    // Move Servo 3 - Elbow Servo
    if (dataIn.startsWith("s3")) {
      String dataInS = dataIn.substring(2, dataIn.length());
      servo3Pos = dataInS.toInt();
      
      moveMotor(servo3Pos, servoElbow)
    }

    // Move Servo 4 - Wrist Servo
    if (dataIn.startsWith("s4")) {
      String dataInS = dataIn.substring(2, dataIn.length());
      servo4Pos = dataInS.toInt();
      
      moveMotor(servo4Pos, servoWrist)
    }

    // Move Servo 5 - Gripper Servo
    if (dataIn.startsWith("s5")) {
      String dataInS = dataIn.substring(2, dataIn.length());
      servo5Pos = dataInS.toInt();
      
      moveMotor(servo5Pos, servoGrip)
    }    

    // If button "SAVE" is pressed
    if (dataIn.startsWith("SAVE")) {
      servo01P[index] = servo1Pos;  // save position into the array
      servo02P[index] = servo2Pos;
      servo03P[index] = servo3Pos;
      servo04P[index] = servo4Pos;
      servo05P[index] = servo5Pos;
      index++;                        // Increase the array index
    }

      // If button "RUN" is pressed
    if (dataIn.startsWith("RUN")) {
      runservo();  // Automatic mode - run the saved steps 
    }

    // If button "RESET" is pressed
    if ( dataIn == "RESET") {
      memset(servo01P, 0, sizeof(servo01P)); // Clear the array data to 0
      memset(servo02P, 0, sizeof(servo02P));
      memset(servo03P, 0, sizeof(servo03P));
      memset(servo04P, 0, sizeof(servo04P));
      memset(servo05P, 0, sizeof(servo05P));
      index = 0;  // Index to 0
    }

  }  
 
}

// Automatic mode custom function - run the saved steps
void runservo() {
  while (dataIn != "RESET") {
    for (int i = 0; i <= index - 2; i++) {  // Run through all steps(index)
      if (Bluetooth.available() > 0) {      // Check for incomding data
        dataIn = Bluetooth.readString();
        if ( dataIn == "PAUSE") {           // If button "PAUSE" is pressed
          while (dataIn != "RUN") {         // Wait until "RUN" is pressed again
            if (Bluetooth.available() > 0) {
              dataIn = Bluetooth.readString();
              if ( dataIn == "RESET") {     
                break;
              }
            }
          }
        }

        // If speed slider is changed
        if (dataIn.startsWith("ss")) {
          String dataInS = dataIn.substring(2, dataIn.length());
          speed = dataInS.toInt(); // Change servo speed (delay time)
        }

      }

      // Servo Waist
      moveMotorDeg(servo01P[i], servoWaist)

      // Servo Shoulder
      moveMotorDeg(servo02P[i], servoShoulder)

      // Servo Elbow
      moveMotorDeg(servo03P[i], servoElbow)

      // Servo Wrist
      moveMotorDeg(servo04P[i], servoWrist)

      // Servo Grip
      moveMotorDeg(servo05P[i], servoGrip)

    }

  }

}

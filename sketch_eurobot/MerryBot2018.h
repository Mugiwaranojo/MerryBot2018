#ifndef MerryBot2018_h
#define MerryBot2018_h

#include "Arduino.h"
#include <Wire.h>
#include <Servo.h>
#include <ArduinoJson.h>

class MerryBot2018
{
  public:
    MerryBot2018();
	void setup();	 
	
	void forward(int time);
	void backward(int time);
	void turnLeft(int time);
	void turnRight(int time);
	void stop();
	
	void openBackDoor();
	void closeBackDoor();
	void openBottomDoor();
	void closeBottomDoor();
	void openFrontDoor();
	void closeFrontDoor();
	
	void launchBall();
	void openTapLeft();
	void openTapRight();
	void takeCubeLeft();
	void takeCubeRight();
	void takeBallLeft();
	void takeBallRight();
	
	void setServoPositions(JsonObject* data);
	JsonObject* getStatus();
	
  private:
    int _pinServoLeftElbow;
	int _pinServoLeftShoulder;
	int _pinServoLeftWrist;
	int _pinServoLeftHand;
	int _pinServoRightElbow;
	int _pinServoRightShoulder;
	int _pinServoRightWrist;
	int _pinServoRightHand;
	int _pinServoLeftBackDoor;
	int _pinServoRightBackDoor;
	int _pinServoLeftBottomDoor;
	int _pinServoRightBottomDoor;
	int _pinServoFrontDoor;
	int _pinMotorLauncher;
	int _pinTelemeterFront;
	int _pinTelemeterBack;
	int _pinTelemeterLeftHand;
	int _pinTelemeterRightHand;
	int _gyro_address;
	int _pinMotorLeftEN;
	int _pinMotorLeftIN1;
	int _pinMotorLeftIN2;
	int _pinMotorRightEN;
	int _pinMotorRightIN1;
	int _pinMotorRightIN2;
	
	Servo _servos[15];
	int _servosValues[15];
};

#endif
#include "Arduino.h"
#include <Servo.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include "MerryBot2018.h"

//Constructor 
MerryBot2018::MerryBot2018()
{
    _pinServoLeftElbow 		= 2;
	_pinServoLeftShoulder 	= 3;
	_pinServoLeftWrist 		= 4;
	_pinServoLeftHand 		= 5;
	_pinServoRightElbow 	= 6;
	_pinServoRightShoulder 	= 7;
	_pinServoRightWrist 	= 8;
	_pinServoRightHand 		= 9;
	_pinServoLeftBackDoor 	= 10;
	_pinServoRightBackDoor 	= 11;
	_pinServoLeftBottomDoor = 12;
	_pinServoRightBottomDoor= 13;
	_pinServoFrontDoor 		= 14;
	
	_servosValues[_pinServoLeftElbow]= 0;
	_servosValues[_pinServoLeftShoulder]= 0;
	_servosValues[_pinServoLeftWrist]= 0;
	_servosValues[_pinServoLeftHand]= 0;
	_servosValues[_pinServoRightElbow]= 0;
	_servosValues[_pinServoRightShoulder]= 0;
	_servosValues[_pinServoRightWrist]= 0;
	_servosValues[_pinServoRightHand]= 0;
	_servosValues[_pinServoLeftBackDoor]= 0;
	_servosValues[_pinServoRightBackDoor]= 0;
	_servosValues[_pinServoLeftBottomDoor]= 0;
	_servosValues[_pinServoRightBottomDoor]= 0;
	_servosValues[_pinServoFrontDoor]= 0;
	
	_pinMotorLauncher 		= 26;
	
	
	_pinTelemeterFront 		= 8;
	_pinTelemeterBack 		= 9;
	_pinTelemeterLeftHand 	= 10;
	_pinTelemeterRightHand 	= 11;
	
	_gyro_address 			= 0x68;
	
	_pinMotorLeftEN 		= 0;
	_pinMotorLeftIN1 		= 22;
	_pinMotorLeftIN2 		= 23;
	_pinMotorRightEN 		= 1;
	_pinMotorRightIN1 		= 24;
	_pinMotorRightIN2 		= 25;
	
}

//Setup pins
void MerryBot2018::setup(){
	
	Wire.begin();
	Wire.beginTransmission(_gyro_address);
	Wire.write(0x6B);  // PWR_MGMT_1 register
	Wire.write(0);     // set to zero (wakes up the MPU-6050)
	Wire.endTransmission(true);
	
	for(int i=2; i<15;i++){
		_servos[i].attach(i);
		this->_servos[i].write(_servosValues[i]);
	}
	
	pinMode(_pinMotorLeftEN, OUTPUT);
	pinMode(_pinMotorRightEN, OUTPUT);
	pinMode(_pinMotorLeftIN1, OUTPUT);
	pinMode(_pinMotorLeftIN2, OUTPUT);
	pinMode(_pinMotorRightIN1, OUTPUT);
	pinMode(_pinMotorRightIN2, OUTPUT);
	
	pinMode(_pinTelemeterFront, OUTPUT);
	pinMode(_pinTelemeterBack, OUTPUT);
	pinMode(_pinTelemeterLeftHand, OUTPUT);
	pinMode(_pinTelemeterRightHand, OUTPUT);
	
	pinMode(_pinMotorLauncher, OUTPUT);
}

//GetStatus : get sensors informations and servo positions
JsonObject* MerryBot2018::getStatus(){
	DynamicJsonBuffer  jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	root["mesureFront"] = 13*pow(analogRead(_pinTelemeterFront)*(5/1024), -1);
	root["mesureBack"]	= 13*pow(analogRead(_pinTelemeterBack)*(5/1024), -1);
	root["mesureLeft"] 	= 13*pow(analogRead(_pinTelemeterLeftHand)*(5/1024), -1);
	root["mesureRight"] = 13*pow(analogRead(_pinTelemeterRightHand)*(5/1024), -1);
	JsonArray& data = root.createNestedArray("dataGyro");
	Wire.beginTransmission(_gyro_address);
	Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
	Wire.endTransmission(false);
	Wire.requestFrom(_gyro_address,14,true);  // request a total of 14 registers
	data.add(Wire.read()<<8|Wire.read());  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
	data.add(Wire.read()<<8|Wire.read());  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
	data.add(Wire.read()<<8|Wire.read());  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
	data.add(Wire.read()<<8|Wire.read());  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
	data.add(Wire.read()<<8|Wire.read());  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
	data.add(Wire.read()<<8|Wire.read());  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
	data.add(Wire.read()<<8|Wire.read());  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
	JsonObject& servoPositions = jsonBuffer.createObject();
	for(int i=2; i<10;i++){
		const char servoNum = static_cast<char>(i);
		servoPositions[&servoNum] = _servosValues[i];
	}
	root["servoPositions"] 	= servoPositions;
	root["leftBackDoor"] 	= _servosValues[_pinServoLeftBackDoor]==0 		? "close" : "open";
	root["rightBackDoor"]	= _servosValues[_pinServoRightBackDoor]==180 	? "close" : "open";
	root["leftBottomDoor"] 	= _servosValues[_pinServoLeftBottomDoor]==0 	? "close" : "open";
	root["rightBottomDoor"] = _servosValues[_pinServoRightBottomDoor]==180 	? "close" : "open";
	root["frontDoor"]		= _servosValues[_pinServoFrontDoor]==0 			? "close" : "open";
	return &root;
}

//setServoPositions
void MerryBot2018::setServoPositions(JsonObject* data){
	for(JsonObject::iterator it=data->begin(); it!=data->end(); ++it) 
	{
		int servoNum = atoi(it->key);
		int servoValue = atoi(it->value);
		_servosValues[servoNum] = servoValue;
	}
	for(int i=2; i<15;i++){
		_servos[i].write(_servosValues[i]);
	}
}

//////////////////
//WEELS MOVES/////
//////////////////
void MerryBot2018::stop(){
	digitalWrite(_pinMotorLeftIN1, LOW);
	digitalWrite(_pinMotorLeftIN2, LOW);  
	digitalWrite(_pinMotorRightIN1, LOW);
	digitalWrite(_pinMotorRightIN2, LOW);
}

void MerryBot2018::forward(int time){
	analogWrite(_pinMotorLeftEN, 200);
	digitalWrite(_pinMotorLeftIN1, HIGH);
	digitalWrite(_pinMotorLeftIN2, LOW);  
	analogWrite(_pinMotorRightEN, 200);
	digitalWrite(_pinMotorRightIN1, HIGH);
	digitalWrite(_pinMotorRightIN2, LOW);
	delay(time);
	this->stop();
}

void MerryBot2018::backward(int time){
	analogWrite(_pinMotorLeftEN, 200);
	digitalWrite(_pinMotorLeftIN1, LOW);
	digitalWrite(_pinMotorLeftIN2, HIGH);  
	analogWrite(_pinMotorRightEN, 200);
	digitalWrite(_pinMotorRightIN1, LOW);
	digitalWrite(_pinMotorRightIN2, HIGH);
	delay(time);
	this->stop();
}

void MerryBot2018::turnLeft(int time){
	analogWrite(_pinMotorLeftEN, 200);
	digitalWrite(_pinMotorLeftIN1, HIGH);
	digitalWrite(_pinMotorLeftIN2, LOW);  
	digitalWrite(_pinMotorRightIN1, LOW);
	digitalWrite(_pinMotorRightIN2, LOW);
	delay(time);
	this->stop();
}

void MerryBot2018::turnRight(int time){
	analogWrite(_pinMotorRightEN, 200);
	digitalWrite(_pinMotorLeftIN1, LOW);
	digitalWrite(_pinMotorLeftIN2, LOW);  
	digitalWrite(_pinMotorRightIN1, HIGH);
	digitalWrite(_pinMotorRightIN2, LOW);
	delay(time);
	this->stop();
}

//////////////////////
//OPEN/CLOSE DOORS////
//////////////////////
void MerryBot2018::openBackDoor(){
	DynamicJsonBuffer  jsonBuffer;
	JsonObject& servoPosition = jsonBuffer.createObject();
	const char servoLeft = static_cast<char>(_pinServoLeftBackDoor);
	const char servoRight = static_cast<char>(_pinServoRightBackDoor);
	servoPosition[&servoLeft] = 90;
	servoPosition[&servoRight] = 90;
	this->setServoPositions(&servoPosition);
}

void MerryBot2018::closeBackDoor(){
	DynamicJsonBuffer  jsonBuffer;
	JsonObject& servoPosition = jsonBuffer.createObject();
	const char servoLeft = static_cast<char>(_pinServoLeftBackDoor);
	const char servoRight = static_cast<char>(_pinServoRightBackDoor);
	servoPosition[&servoLeft] = 0;
	servoPosition[&servoRight] = 180;
	this->setServoPositions(&servoPosition);
}

void MerryBot2018::openBottomDoor(){
	DynamicJsonBuffer  jsonBuffer;
	JsonObject& servoPosition = jsonBuffer.createObject();
	const char servoLeft = static_cast<char>(_pinServoLeftBottomDoor);
	const char servoRight = static_cast<char>(_pinServoRightBottomDoor);
	servoPosition[&servoLeft] = 180;
	servoPosition[&servoRight] = 0;
	this->setServoPositions(&servoPosition);
}

void MerryBot2018::closeBottomDoor(){
	DynamicJsonBuffer  jsonBuffer;
	JsonObject& servoPosition = jsonBuffer.createObject();
	const char servoLeft = static_cast<char>(_pinServoLeftBottomDoor);
	const char servoRight = static_cast<char>(_pinServoRightBottomDoor);
	servoPosition[&servoLeft] = 0;
	servoPosition[&servoRight] = 180;
	this->setServoPositions(&servoPosition);
}

void MerryBot2018::openFrontDoor(){
	DynamicJsonBuffer  jsonBuffer;
	JsonObject& servoPosition = jsonBuffer.createObject();
	const char servo= static_cast<char>(_pinServoFrontDoor);
	servoPosition[&servo] = 120;
	this->setServoPositions(&servoPosition);
}
void MerryBot2018::closeFrontDoor(){
	DynamicJsonBuffer  jsonBuffer;
	JsonObject& servoPosition = jsonBuffer.createObject();
	const char servo= static_cast<char>(_pinServoFrontDoor);
	servoPosition[&servo] = 0;
	this->setServoPositions(&servoPosition);
}

//////////////////
//Actions    /////
//////////////////
void MerryBot2018::launchBall(){
	digitalWrite(_pinMotorLauncher, HIGH);
	delay(500);
	digitalWrite(_pinMotorLauncher, LOW);
}

/*
void MerryBot2018::openTapLeft();
void MerryBot2018::openTapRight();
void MerryBot2018::takeCubeLeft(int position);
void MerryBot2018::takeCubeRight(int position);
void MerryBot2018::takeBallLeft(int position);
void MerryBot2018::takeBallRight(int position);*/
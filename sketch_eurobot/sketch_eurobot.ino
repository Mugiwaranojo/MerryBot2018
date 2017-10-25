#include "MerryBot2018.h"

MerryBot2018 robot;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // wait serial port initialization
  }
  Serial.println("Robot init ...");
  robot.setup();
  Serial.println("Robot init end");
}

void loop() {
  // put your main code here, to run repeatedly:
  receiveEvent();
}

String data="";
void receiveEvent(){
  if (Serial.available() > 0) {
    // read the incoming byte:
    data = Serial.readString();
    delay(100);
    parseData();
  }
}

void parseData(){
  DynamicJsonBuffer  jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(data);
  // Test if parsing succeeds.
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  const char* action= root["action"];
  if(action=="forward"){
    robot.forward(root["time"]);
  }else if(action=="backward"){
    robot.backward(root["time"]);
  }else if(action=="turnLeft"){
    robot.turnLeft(root["time"]);
  }else if(action=="turnRight"){
    robot.turnRight(root["time"]);
  }else{
    robot.stop();
  }

  JsonObject* object=  robot.getStatus();
  object->printTo(Serial);
}

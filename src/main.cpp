#include "config.h"
#include "tempSensors.h"
#include "relaysControl.h"
#include "connections.h"


void setup() {
    beginCycle();
}

void operationLoop(){
    if(progLen > 0){
        if(currentProgram < progLen){
            String inc = income["program"][currentProgram]["mode"];
            shokerMode = inc;
            stageTime = income["program"][currentProgram]["time"];
            operation(stageTime, targetRelayPosition);
        }else{
            operation(relayAwaitPosition);    
        }
    }
}
void loop() {
    client.poll();
    sendData();
    operationLoop();
    if(lostConnection){
        client.connect(websockets_server_host, websockets_server_port, websockets_path);
        client.ping();
    }
}
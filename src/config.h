#include <Arduino.h>
#define door  14 // объявление пина для отслеживания двери
#define fanRelay 12 // relay 1
#define compRelay 13 // relay 4
#define doorDefrostRelay 15 //relay 2
#define defrostRelay 2 // relay 3
// #define configPin 21
bool lostConnection = false;
String shokerMode = "await";
int currentRelayPosition[4] = {0, 0, 0, 0};
int targetRelayPosition[4] = {0, 0, 0, 0};
int relayAwaitPosition[4] = {0, 0, 0, 0};
int relayFreezePosition[4] = {1, 1, 0, 1};
int relayDefrostPosition[4] = {1, 1, 1, 1};
int currentProgram = 0;
long stageTime = 0;
int progLen = 0;
long globalTime = 0;
void clearAll(){
    stageTime = 0;
    currentProgram = 0;
    shokerMode = "await";
    progLen = 0;
    globalTime = 0;
}
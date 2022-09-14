long preTime = 0;
long preGlobalTime = 0;
int delayRelaySwitch = 0;
bool flagMaintainStart = false;
bool flagMaintainStop = true;
void modeConvertation(String shokerMode)
{
    if (shokerMode == "freeze")
    {
        for (int i = 0; i < 4; ++i)
        {
            targetRelayPosition[i] = relayFreezePosition[i];
        }
    }
    else if (shokerMode = "defrost")
    {
        for (int i = 0; i < 4; ++i)
        {
            targetRelayPosition[i] = relayDefrostPosition[i];
        }
    }
    else if (shokerMode = "await")
    {
        for (int i = 0; i < 4; ++i)
        {
            targetRelayPosition[i] = relayAwaitPosition[i];
        }
    }
}

void relayConfigCycle(int relayCondition[4], long currentTime)
{

    if (currentTime > delayRelaySwitch && currentTime < delayRelaySwitch + 2000)
    {
        if (relayCondition[0] != currentRelayPosition[0])
        {
            Serial.println("suka1");
            pinMode(fanRelay, OUTPUT);
            digitalWrite(fanRelay, relayCondition[0]);
            delayRelaySwitch += 2000;
            currentRelayPosition[0] = relayCondition[0];
        }
        else if (relayCondition[1] != currentRelayPosition[1])
        {
            Serial.println("suka2");
            pinMode(doorDefrostRelay, OUTPUT);
            digitalWrite(doorDefrostRelay, relayCondition[1]);
            delayRelaySwitch += 2000;
            currentRelayPosition[1] = relayCondition[1];
        }
        else if (relayCondition[2] != currentRelayPosition[2])
        {
            Serial.println("suka3");
            pinMode(defrostRelay, OUTPUT);
            digitalWrite(defrostRelay, relayCondition[2]);
            delayRelaySwitch += 2000;
            currentRelayPosition[2] = relayCondition[2];
        }
        else if (relayCondition[3] != currentRelayPosition[3])
        {
            Serial.println("suka4");
            pinMode(compRelay, OUTPUT);
            digitalWrite(compRelay, relayCondition[3]);
            delayRelaySwitch += 2000;
            currentRelayPosition[3] = relayCondition[3];
        }
    }
}

void operation(int time, int relayCondition[4])
{
    if (preTime == 0)
    {
        preTime = millis();
        modeConvertation(shokerMode);
        delayRelaySwitch = 0;
    }
    globalTime = (millis() - preGlobalTime) / 1000;
    if ((millis() - preTime) < (time * 1000))
    {
        if (shokerMode != "maintaining")
        {
            relayConfigCycle(relayCondition, (millis() - preTime));
        }
        else
        {
            if (temperature6 > -19)
            {
                if(!flagMaintainStart){
                    preTime = millis();
                    flagMaintainStart = true;
                    flagMaintainStop = true;
                }
                relayConfigCycle(relayFreezePosition, (millis() - preTime));
            }
            if (temperature6 < -25)
            {
                if(flagMaintainStop){
                    preTime = millis();
                    flagMaintainStart = false;
                    flagMaintainStop = false;
                }
                relayConfigCycle(relayAwaitPosition, (millis() - preTime));
                
            }
        }
    }
    else
    {
        preTime = 0;
        ++currentProgram;
    }
}

void operation(int relayCondition[4])
{
    if (preTime == 0)
    {
        preTime = millis();
    }
    if ((millis() - preTime) < (10 * 1000))
    {
        relayConfigCycle(relayCondition, (millis() - preTime));
    }
    else
    {
        clearAll();
    }
}

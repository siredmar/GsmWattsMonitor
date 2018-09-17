#pragma once
#include <Arduino.h>

class TriggerSwitch
{
public:
    TriggerSwitch(double trigger, int delay)
        : trigger(trigger)
        , delay(delay)
        , captureEnabled(true)
        , lastTrigger(0)
    {
    }

    bool Process(double input)
    {
    }

    bool checkThreshold(double input)
    {
        if (input > trigger)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    double trigger;
    int delay;
    bool captureEnabled;
    long lastTrigger;
    float accu;
};
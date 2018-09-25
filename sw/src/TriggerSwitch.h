#pragma once
#include <Arduino.h>

// #define DEBUG

enum class TriggerSwitchState
{
    idle,
    capturing,
    trigger,
    release
};

template <class T> class TriggerSwitch
{
public:
    TriggerSwitch(T trigger, T release, long hysteresis, void (*callback)(void))
        : newTrigger(false)
        , trigger(trigger)
        , release(release)
        , hysteresisMs(hysteresis * 1000)
        , startTime(-1)
        , timeDelta(0)
        , callback(callback)
        , state(TriggerSwitchState::idle)
    {
    }

    void setTrigger(T trig, T release, int hysteresis)
    {
        this.trigger = trigger;
        this.release = release;
        this.hysteresisMs = hysteresis * 1000;
    }

    void reset()
    {
        state = TriggerSwitchState::idle;
        timeDelta = 0;
        startTime = -1;
    }

    void process(T input)
    {
        switch (state)
        {
            case TriggerSwitchState::idle:
            #ifdef DEBUG
            Serial.println("State: idle");
            #endif
                if (input >= trigger)
                {
                    #ifdef DEBUG
                    Serial.println("idle: idle -> trigger");
                    #endif
                    state = TriggerSwitchState::trigger;
                    newTrigger = false;
                }
                break;

            case TriggerSwitchState::trigger:
                timeDelta = 0;
                startTime = -1;
                state = TriggerSwitchState::capturing;
                #ifdef DEBUG
                Serial.println("trigger: trigger -> capturing");
                #endif
                break;

            case TriggerSwitchState::capturing:
                if (input < release)
                {
                    #ifdef DEBUG
                    Serial.println("Capturing: input < release");
                    #endif
                    if (startTime == -1)
                    {
                        startTime = millis();
                    }
                    #ifdef DEBUG
                    Serial.print("startTime: ");
                    Serial.println(startTime);
                    #endif
                    timeDelta = millis() - startTime;
                    #ifdef DEBUG
                    Serial.print("timeDelta: ");
                    Serial.println(timeDelta);
                    #endif
                    if (timeDelta >= hysteresisMs)
                    {
                        #ifdef DEBUG
                        Serial.println("capturing: capturing -> release");
                        #endif
                        state = TriggerSwitchState::release;
                        newTrigger = true;
                    }
                }
                else
                {
                    #ifdef DEBUG
                    Serial.println("capturing: capturing -> trigger");
                    #endif
                    state = TriggerSwitchState::trigger;
                }
                break;

            case TriggerSwitchState::release:
                #ifdef DEBUG
                Serial.println("release: release -> idle");
                #endif
                (*callback)();
                reset();
                state = TriggerSwitchState::idle;
                break;
        }
    }

    bool getState()
    {
        return newTrigger;
    }

    TriggerSwitchState getStatemachine()
    {
        return state;
    }

private:
    bool newTrigger;
    T trigger;
    T release;
    long hysteresisMs;
    long startTime;
    long timeDelta;
    void (*callback)(void);
    TriggerSwitchState state;
};
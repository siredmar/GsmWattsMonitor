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
        , verbose(false)
    {
    }

    void setVerbose(bool mode)
    {
        verbose = mode;
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
                if (verbose == true)
                {
                    Serial.println("State: idle");
                }
                if (input >= trigger)
                {
                    if (verbose == true)
                    {
                        Serial.println("idle: idle -> trigger");
                    }
                    state = TriggerSwitchState::trigger;
                    newTrigger = false;
                }
                break;

            case TriggerSwitchState::trigger:
                timeDelta = 0;
                startTime = -1;
                state = TriggerSwitchState::capturing;
                if (verbose == true)
                {
                    Serial.println("trigger: trigger -> capturing");
                }
                break;

            case TriggerSwitchState::capturing:
                if (input < release)
                {
                    if (verbose == true)
                    {
                        Serial.println("Capturing: input < release");
                    }
                    if (startTime == -1)
                    {
                        startTime = millis();
                    }
                    if (verbose == true)
                    {
                        Serial.print("startTime: ");
                        Serial.println(startTime);
                    }
                    timeDelta = millis() - startTime;
                    if (verbose == true)
                    {
                        Serial.print("timeDelta: ");
                        Serial.println(timeDelta);
                    }
                    if (timeDelta >= hysteresisMs)
                    {
                        if (verbose == true)
                        {
                            Serial.println("capturing: capturing -> release");
                        }
                        state = TriggerSwitchState::release;
                        newTrigger = true;
                    }
                }
                else
                {
                    if (verbose == true)
                    {
                        Serial.println("capturing: capturing -> trigger");
                    }
                    state = TriggerSwitchState::trigger;
                }
                break;

            case TriggerSwitchState::release:
                if (verbose == true)
                {
                    Serial.println("release: release -> idle");
                }
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
    bool verbose;
};
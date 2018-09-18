#pragma once
#include <Arduino.h>

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
    TriggerSwitch(T trigger, T release, int hysteresis, void (*callback)(void))
        : trigger(trigger)
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
                if (input >= trigger)
                {
                    state = TriggerSwitchState::trigger;
                }
                break;

            case TriggerSwitchState::trigger:
                timeDelta = 0;
                startTime = -1;
                state = TriggerSwitchState::capturing;
                break;

            case TriggerSwitchState::capturing:
                if (input < release)
                {
                    if (startTime == -1)
                    {
                        startTime = millis();
                    }

                    timeDelta += millis() - startTime;
                    if (timeDelta >= hysteresisMs)
                    {
                        state = TriggerSwitchState::release;
                    }
                }
                else if (input >= trigger)
                {
                    state = TriggerSwitchState::trigger;
                }
                break;

            case TriggerSwitchState::release:
                (*callback)();
                reset();
                state = TriggerSwitchState::idle;
                break;
        }
    }

private:
    T trigger;
    T release;
    long hysteresisMs;
    long startTime;
    long timeDelta;
    void (*callback)(void);
    TriggerSwitchState state;
};
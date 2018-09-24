
#pragma once
#include <Arduino.h>
#include <SoftwareSerial.h>

#define SIM800_DEBUG

#define SIM800_DEFAULT_RX 8
#define SIM800_DEFAULT_TX 7

#define TIME_OUT_READ_SERIAL 5000
class Sim800
{
public:
    Sim800();
    Sim800(int rx, int tx);
    bool init(const String& pin);
    bool testAT();
    bool networkStatus();
    bool functionMode(bool enabled);
    bool sendSms(const String& number, const String& text);
    bool callNumber(const String& number);
    bool hangupCall();
    bool setPin(const String& pin);
    bool checkPin();
    enum class CallState
    {
        ready,
        unavailable,
        unknown,
        ringing,
        callInProgress,
        asleep
    };
    CallState callState();

private:
    SoftwareSerial* mySerial;
    bool parseInput(const String& input, String pattern);
    String readSerial();
    String readSerial(uint32_t timeout);
    bool readResponse(const String& str);
    bool sendCmd(char c);
    bool sendCmd(const String& text, bool newline);
    int rxPin;
    int txPin;
};
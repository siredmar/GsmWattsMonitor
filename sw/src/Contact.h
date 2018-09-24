#pragma once
#include "Configuration.h"
#include "Sim800.h"
#include <Arduino.h>

class Contact
{
public:
    Contact(Sim800* m, Configuration* configuration);
    bool init();
    bool sendSms(const String& number, const String& text);
    bool callNumber(const String& number, int seconds);
    void process();
    bool registerNumber(const String& number, bool call = true, int seconds = 0, bool sms = false);
    bool deleteRegistration(const String& number);
    void status(int index);
    void status();

private:
    Sim800* modem;
    Configuration* configuration;
    int findRegistrationIndex(const String& number);
};
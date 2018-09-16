#include "Configuration.h"
#include "Contact.h"
#include "Sim800.h"
#include <Arduino.h>
#include <Cmd.h>
#include <EmonLib.h>

#define EMON_PIN 1

Sim800* modem;
Contact* contact;
Configuration* configuration;
EnergyMonitor emon1;

void PrintHelp()
{
    Serial.println(F("Available commands"));
    Serial.println(F("help\t\tget this help"));
    Serial.println(F("sim_pin\t\tsim_pin <pin>"));
    Serial.println(F("status\t\tstatus information"));
    Serial.println(F("eeprom_reset\tresets the eeprom defaults"));
    Serial.println(F("register\tregister <number> <call> <seconds> <sms>"));
    Serial.println(F("delete\tdelete <number>"));
    Serial.println(F("text\ttext <text for SMS>"));
    Serial.println(F("calib\tcalib <value>"));
}

void Register(int arg_cnt, char** args)
{
    bool call = (bool)cmdStr2Num(args[2], 10);
    int seconds = (int)cmdStr2Num(args[3], 10);
    bool sms = (bool)cmdStr2Num(args[4], 10);
    contact->registerNumber(String(args[1]), call, seconds, sms);
}

void Delete(int arg_cnt, char** args)
{
    contact->deleteRegistration(String(args[1]));
}

void Help(int arg_cnt, char** args)
{
    PrintHelp();
}

void Text(int arg_cnt, char** args)
{
    String text;
    for (int i = 1; i < arg_cnt; i++)
    {
        text += String(args[i]);
        text += " ";
    }
    text += String(args[arg_cnt]);

    Serial.print(F("Setting SMS text to: "));
    Serial.println(text);
    configuration->text(text.c_str());
}

void Status(int arg_cnt, char** args)
{
    Serial.print(F("EEPROM Init: "));
    Serial.println(configuration->initialized());
    Serial.print(F("SIM Pin: "));
    Serial.println(configuration->simPin());
    Serial.print(F("Current sensor calibration: "));
    Serial.println(configuration->energyEmonCalibration());
    contact->status();
}

void SimPin(int arg_cnt, char** args)
{
    if (arg_cnt == 2)
    {
        Serial.println(args[1]);
        configuration->simPin(args[1]);
    }
    else
    {
        Serial.println(F("invalid arguments"));
    }
}

void Calibration(int arg_cnt, char** args)
{
    if (arg_cnt == 2)
    {
        Serial.println(args[1]);
        configuration->energyEmonCalibration((double)cmdStr2float(args[1]));
    }
    else
    {
        Serial.println(F("invalid arguments"));
    }
}

void Process(int arg_cnt, char** args)
{
    contact->process();
}

void EepromFactory(int arg_cnt, char** args)
{
    configuration->resetFactoryEeprom(1024);
}

void EepromReset(int arg_cnt, char** args)
{
    configuration->reset();
}

void setup()
{
    Serial.begin(115200);
    configuration = new Configuration();

    if (configuration->initialized() == 0xFF)
    {
        Serial.println(F("EEPROM is fresh -> initializing"));
        configuration->reset();
    }
    else
    {
        Serial.println(F("EEPROM properly initialized"));
        Serial.print(F("Sim Pin: "));
        Serial.println(configuration->simPin());
    }

    modem = new Sim800(8, 9);
    contact = new Contact(modem, configuration);
    contact->init();

    emon1.current(EMON_PIN, configuration->energyEmonCalibration());

    cmdInit(&Serial);
    cmdAdd("help", Help);
    cmdAdd("status", Status);
    cmdAdd("sim_pin", SimPin);
    cmdAdd("eeprom_reset", EepromReset);
    cmdAdd("register", Register);
    cmdAdd("delete", Delete);
    cmdAdd("process", Process);
    cmdAdd("text", Text);
    cmdAdd("calib", Calibration);
    PrintHelp();
    Serial.print(F("CMD >> "));
}

void loop()
{
    cmdPoll();
    delay(100);
    double Irms = emon1.calcIrms(1480);  // Calculate Irms only
    emon1.serialprint();
}
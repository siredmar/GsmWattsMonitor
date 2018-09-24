#include "Sim800.h"

Sim800::Sim800()
    : rxPin(SIM800_DEFAULT_RX)
    , txPin(SIM800_DEFAULT_TX)
{
    mySerial = new SoftwareSerial(rxPin, txPin);
    mySerial->begin(57600);
}

Sim800::Sim800(int rx, int tx)
    : rxPin(rx)
    , txPin(tx)
{
    mySerial = new SoftwareSerial(rxPin, txPin);
    mySerial->begin(57600);
}

bool Sim800::init(const String& pin)
{
    Serial.print(F("Trying to find Sim800..."));
    sendCmd("AT+CMEE=1");
    while (!testAT())
    {
        delay(100);
        Serial.print(F("."));
    }
    Serial.println(F(" found"));

    Serial.print(F("Enabling network..."));
    functionMode(true);
    Serial.println(F(" done"));
    Serial.print(F("Unlocking SIM..."));
    setPin(pin);
    delay(5000);
    if (!checkPin())
    {
        Serial.println(F(" failed"));
    }
    Serial.println(F(" done"));

    Serial.print(F("Trying to find network..."));
    while (!networkStatus())
    {
        delay(1000);
        Serial.print(F("."));
    }
    Serial.println(F(" connected"));
    return true;
}

bool Sim800::testAT()
{
    return sendCmd("AT");
}

bool Sim800::networkStatus()
{
    sendCmd("AT+CREG?");
    return readResponse(",1");
}

bool Sim800::functionMode(bool enabled)
{
    String command;
    command = "AT+CFUN=";
    if (enabled)
    {
        command += "1";
    }
    else
    {
        command += "4";
    }

    mySerial->println(command);
    return readResponse("OK");
}

Sim800::CallState Sim800::callState()
{
    Sim800::CallState ret = Sim800::CallState::unknown;
    sendCmd("AT+CPAS");
    String str = readSerial();
    if (parseInput(str, ": 0"))
    {
        ret = Sim800::CallState::ready;
    }
    else if (parseInput(str, ": 1"))
    {
        ret = Sim800::CallState::unavailable;
    }
    else if (parseInput(str, ": 2"))
    {
        ret = Sim800::CallState::unknown;
    }
    else if (parseInput(str, ": 3"))
    {
        ret = Sim800::CallState::ringing;
    }
    else if (parseInput(str, ": 4"))
    {
        ret = Sim800::CallState::callInProgress;
    }
    else if (parseInput(str, ": 5"))
    {
        ret = Sim800::CallState::asleep;
    }
    else
    {
        ret = Sim800::CallState::unknown;
    }
    return ret;
}

bool Sim800::sendSms(const String& number, const String& text)
{
    sendCmd("AT+CMGF=1\r", false);
    if (!readResponse("OK"))
    {
        Serial.println(F("AT+CMGF failed"));
        return false;
    }
    String command;
    command = "AT+CMGS=\"";
    command += number;
    command += "\"\r";
    sendCmd(command, false);
    if (!readResponse(">"))
    {
        Serial.println(F("AT+CMGS failed"));
        return false;
    }
    sendCmd(text, false);
    sendCmd(26);
    String resp = readSerial();
    if (!parseInput(resp, "OK"))
    {
        Serial.println(F("text failed"));
        return false;
    }

    // String cmd = "AT+CMGS=\"";
    // cmd += number;
    // cmd += "\"\r";
    // Serial.println(cmd);
    // cmd += text;
    // cmd += "\r";
    // Serial.println(cmd);
    // sendCmd(cmd);
    // if(!readResponse("OK"))
    // {
    //     Serial.println(F("AT+CMGS failed"));
    //     return false;
    // }

    // mySerial->print((char)26);
    // if(!readResponse("OK"))
    // {
    //     return false;
    // }
    return true;
}

bool Sim800::callNumber(const String& number)
{
    String command;
    command = "ATD";
    command += number;
    command += ";";

    mySerial->println(command);
    return readResponse("OK");
}

bool Sim800::hangupCall()
{
    sendCmd("ATH");
    return readResponse("OK");
}

bool Sim800::setPin(const String& pin)
{
    String command;
    command = "AT+CPIN=";
    command += pin;

    mySerial->println(command);
    return readResponse("OK");
}

bool Sim800::checkPin()
{
    String command;
    command = "AT+CPIN?";

    mySerial->println(command);
    return readResponse("READY");
}

String Sim800::readSerial()
{
    return readSerial(TIME_OUT_READ_SERIAL);
}

String Sim800::readSerial(uint32_t timeout)
{
    uint64_t timeOld = millis();

    while (!mySerial->available() && !(millis() > timeOld + timeout))
    {
        delay(13);
    }

    String str;

    while (mySerial->available())
    {
        if (mySerial->available() > 0)
        {
            str += (char)mySerial->read();
        }
    }

    return str;
}

bool Sim800::readResponse(const String& pattern)
{
    String str;
    str = readSerial();
#ifdef SIM800_DEBUG
    Serial.println(str);
#endif
    if (str.indexOf(pattern) == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool Sim800::sendCmd(const String& text, bool newline = true)
{
#ifdef SIM800_DEBUG
    if (newline)
        Serial.println(text);
    else
        Serial.print(text);
#endif
    if (newline)
        mySerial->println(text);
    else
        mySerial->print(text);
    delay(100);
    return true;
}

bool Sim800::sendCmd(char c)
{
#ifdef SIM800_DEBUG
    Serial.print(c);
#endif
    mySerial->print(c);
    delay(100);
    return true;
}

bool Sim800::parseInput(const String& input, String pattern)
{
#ifdef SIM800_DEBUG
    Serial.println(input);
#endif
    if (input.indexOf(pattern) == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}
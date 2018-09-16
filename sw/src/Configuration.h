#pragma once

enum class NumberOfContacts
{
    CONTACT_0 = 0,
    CONTACT_1 = 1,
    CONTACT_2 = 2,
    MAX_NUMBER_OF_CONTACTS = 3
};

class Configuration
{
public:
    typedef struct
    {
        bool enabled;
        char pin[9];
        char apn[30];
        char user[20];
        char pass[20];
    } simConfigurationType;

    typedef struct
    {
        bool registered;
        char phoneNumber[20];
        bool sendSms;
        bool call;
        int seconds;
    } contactType;

    typedef struct
    {
        char text[50];
        contactType contact[(int)NumberOfContacts::MAX_NUMBER_OF_CONTACTS];
        int numberOfRegisteredContacts;
    } contactsConfigurationType;

    typedef struct
    {
        double wattsTrigger;
        long wattsTimeout;
        double emonCalibration;
    } energyConfigurationType;

    typedef struct
    {
        char initialized;
        energyConfigurationType energy;
        contactsConfigurationType contacts;
        simConfigurationType sim;
    } configurationType;

    void reset();
    void resetFactoryEeprom(int size);

    char initialized();
    const char* simPin();
    void simPin(const char* pin);
    bool simEnabled();
    void simEnabled(bool en);
    contactType readContact(int index);
    void text(const char* text);
    const char* text();
    int numberOfRegisteredContacts();
    void numberOfRegisteredContacts(int number);
    void contactRegistered(int index, bool flag);
    bool contactRegistered(int index);
    void contactSmsFlag(int index, bool flag);
    bool contactSmsFlag(int index);
    void contactCallFlag(int index, bool flag);
    bool contactCallFlag(int index);
    void contactCallSeconds(int index, int seconds);
    int contactCallSeconds(int index);
    void contactNumber(int index, const char* number);
    const char* contactNumber(int index);

private:
    configurationType configuration;
};

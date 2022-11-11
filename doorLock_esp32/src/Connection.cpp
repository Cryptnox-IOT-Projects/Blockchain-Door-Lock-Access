#include <Arduino.h>
#include <Card.h>
#include <Connection.h>
#include <Web3.h>
#include <Util.h>
#include <Contract.h>
#include <WiFi.h>

// const char *ssid = "V1A7C";
// const char *password = "00341877";
const char *ssid = "V9273";
const char *password = "sithuaung2019";
int wificounter = 0;
void Connection::setUpNFC()
{

    nfc.begin();

    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata)
    {
        while (1)
            ; // halt
    }
    nfc.setPassiveActivationRetries(0xFF);
    nfc.SAMConfig();
}

void Connection::setUpWifi()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return;
    }

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.persistent(false);
        WiFi.mode(WIFI_OFF);
        WiFi.mode(WIFI_STA);

        WiFi.begin(ssid, password);
    }

    wificounter = 0;
    while (WiFi.status() != WL_CONNECTED && wificounter < 10)
    {
        for (int i = 0; i < 500; i++)
        {
            delay(1);
        }
        Serial.print(".");
        wificounter++;
    }

    if (wificounter >= 10)
    {
        Serial.println("Restarting ...");
        ESP.restart(); // targetting 8266 & Esp32 - you may need to replace this
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

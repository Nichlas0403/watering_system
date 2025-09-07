#ifndef SmarthomeService_h
#define SmarthomeService_h
#include "Arduino.h"
#include <ESP8266HTTPClient.h>
#include "ArduinoJson.h"

class SmarthomeService
{
    private:
        HTTPClient _client;
        WiFiClient _wifiClient;
        String _url;

    public:
        SmarthomeService();
        bool UpdateIpAddress(String ipAddress);
};

#endif
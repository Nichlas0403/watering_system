#ifndef SmarthomeService_h
#define SmarthomeService_h
#include "Arduino.h"
#include "FlashService.h"
#include <ESP8266HttpClient.h>
#include "ArduinoJson.h"

class SmarthomeService
{
    private:
        FlashService _flashService;
        HTTPClient _client;
        WiFiClient _wifiClient;
        String _url;

    public:
        SmarthomeService();
        void UpdateSmarthome(int soilReading);
};

#endif
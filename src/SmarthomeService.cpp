#include "SmarthomeService.h"

SmarthomeService::SmarthomeService() : _client(), _wifiClient()
{
    _url = "http://homeassistant.local:8123/api"; // Replace with your Home Assistant IP and port
}

bool SmarthomeService::UpdateIpAddress(String ipAddress)
{
    String url = _url + "/events/general_update_microcontroller_ip_address";
    _client.begin(_wifiClient, url);

    String authHeader = "Bearer myToken";
    _client.addHeader("Authorization", authHeader);
    _client.addHeader("Content-Type", "application/json");
    String jsonBody = "{\"staticName\": \"self_watering_plant_microcontroller\", \"baseIpAddress\": \"" + ipAddress + "\", \"healthCheckRoute\": \"/health\"}";

    int httpResponseCode = _client.POST(jsonBody);

    _client.end();

    return (httpResponseCode == 200);
}
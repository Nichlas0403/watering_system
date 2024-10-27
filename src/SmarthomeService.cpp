#include "SmarthomeService.h"

SmarthomeService::SmarthomeService() : _flashService(), _client(), _wifiClient()
{
    _tokenFlashKey = "smarthomeBearerTokenFlash";
    _token = _flashService.ReadFromFlash(_tokenFlashKey);
    _url = "http://homeassistant.local:8123/api/services"; // Replace with your Home Assistant IP and port
}

void SmarthomeService::UpdateIsOnline()
{
    String url = _url + "/input_boolean/turn_on";
    _client.begin(_wifiClient, url);

    String authHeader = "Bearer " + _token;
    _client.addHeader("Authorization", authHeader);
    _client.addHeader("Content-Type", "application/json");

    String jsonBody = "{\"entity_id\": \"input_boolean.general_self_watering_plant_is_online\"}";

    _client.POST(jsonBody);

    _client.end();
}

void SmarthomeService::UpdateIsOffline()
{
    String url = _url + "/input_boolean/turn_off";
    _client.begin(_wifiClient, url);

    String authHeader = "Bearer " + _token;
    _client.addHeader("Authorization", authHeader);
    _client.addHeader("Content-Type", "application/json");

    String jsonBody = "{\"entity_id\": \"input_boolean.general_self_watering_plant_is_online\"}";

    _client.POST(jsonBody);

    _client.end();
}
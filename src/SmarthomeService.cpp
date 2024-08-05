#include "SmarthomeService.h"

SmarthomeService::SmarthomeService() : _flashService(), _client(), _wifiClient()
{
    _tokenFlashKey = "smarthomeBearerTokenFlash";
    _token = _flashService.ReadFromFlash(_tokenFlashKey);
}

void SmarthomeService::UpdateSmarthome(int soilReading)
{
    // _client.begin(_wifiClient, _url);

    // _client.addHeader("Authorization", "Bearer");
    // _client.addHeader("Content-Type", "application/json");

    // _client.POST("{\"entity_id\": \"someresource\"}");

    // _client.end();
}
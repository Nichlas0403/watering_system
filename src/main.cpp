#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "SmarthomeService.h"

#define soilSensorInput A0 //han hun
#define soilSensorOutput D2 //d1 //han hun
#define pumpOutput D1 //d2 //han hun

//WIFI varibles
String _wifiName = "ssid";
String _wifiPassword = "password";

//Services
ESP8266WebServer _server(80);
SmarthomeService _smarthomeService;

//Core variables
unsigned long previousMillis = 0;
const long interval = 10000; // Check every 10 seconds
bool smarthomeUpdated = false;

unsigned long _currentMillis;
unsigned long _wateringTimeEnd;
unsigned int _wateringTime;
bool _isWatering;


void connectToWiFi();

void setup() {
  Serial.begin(9600);

  pinMode(soilSensorInput, INPUT);
  pinMode(soilSensorOutput, OUTPUT);
  pinMode(pumpOutput, OUTPUT);

  connectToWiFi(); 
  _smarthomeService.UpdateIpAddress(WiFi.localIP().toString());
}

void loop() {

  _server.handleClient();
  _currentMillis = millis();

  
  if(!smarthomeUpdated)
  {
    smarthomeUpdated = _smarthomeService.UpdateIpAddress(WiFi.localIP().toString());
  }

  // Check Wi-Fi status periodically
  if ((_currentMillis - previousMillis >= interval) && WiFi.status() != WL_CONNECTED) 
  {
    smarthomeUpdated = false;

    Serial.println("WiFi disconnected, attempting to reconnect...");
    previousMillis = _currentMillis;  // Reset the timer

    // Only attempt to reconnect if not already reconnecting
    if (!WiFi.isConnected()) 
    {
      WiFi.reconnect(); // Let the ESP32 handle reconnection automatically
      
      // Wait until connected, with a timeout
      unsigned long startAttemptTime = millis();
      while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) 
      {
        delay(500); // Small delay to allow for network stability
        Serial.println("Waiting for WiFi to reconnect...");
      }
    }

    // Check if reconnection was successful
    if (WiFi.status() == WL_CONNECTED) 
    {
      Serial.println("Reconnected to WiFi successfully!");
    } 
    else 
    {
      Serial.println("Failed to reconnect to WiFi.");
    }
  }




  //Watering logic
  if(_currentMillis < _wateringTimeEnd && !_isWatering)
  {
    _isWatering = true;
    digitalWrite(pumpOutput, HIGH);
  }
  else if(_currentMillis > _wateringTimeEnd && _isWatering)
  {
    _isWatering = false;
    digitalWrite(pumpOutput, LOW);
  }
}

// ------------------- API -----------------------  

void Water()
{
  // Check if the request has a body
  if (!_server.hasArg("plain"))
  {
    _server.send(400, "application/json", "{\"error\":\"No JSON body provided\"}");
    return;
  }

  // Get the JSON body
  String jsonBody = _server.arg("plain");

  // Create a JSON document
  DynamicJsonDocument doc(1024); // Adjust size according to your needs

  // Parse the JSON body
  DeserializationError error = deserializeJson(doc, jsonBody);

  if (error)
  {
    _server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }

  // Check for the key and get the value
  if (!doc.containsKey("wateringTimeMilliseconds"))
  {
    _server.send(400, "application/json", "{\"error\":\"Key: 'wateringTimeMilliseconds' is missing\"}");
    return;
  }

  int wateringTime = doc["wateringTimeMilliseconds"].as<int>();

  if (wateringTime <= 0)
  {
    _server.send(400, "application/json", "{\"error\":\"Watering time must be above 0\"}");
    return;
  }

  // Set the watering time end
  _wateringTimeEnd = millis() + wateringTime;

  // Send success response
  _server.send(200);
}

void healthCheck()
{
  _server.send(200);
}

void getSoilReading()
{
  digitalWrite(soilSensorOutput, HIGH);
  delay(125); //Delay so the HIGH voltage can be send from the GPIO before the sensors starts reading

  int value = analogRead(soilSensorInput);

  digitalWrite(soilSensorOutput, LOW);

  String jsonResponse = "{\"reading\":" + String(value) + "}";
  _server.send(200, "text/json", jsonResponse);
}

// Core server functionality
void restServerRouting() 
{
  _server.on(F("/water"), HTTP_POST, Water);
  _server.on(F("/health"), HTTP_GET, healthCheck);
  _server.on(F("/soil-reading"), HTTP_GET, getSoilReading);
}

void handleNotFound() 
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += _server.uri();
  message += "_server: ";
  message += (_server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += _server.args();
  message += "\n";

  for (uint8_t i = 0; i < _server.args(); i++) 
  {
    message += " " + _server.argName(i) + ": " + _server.arg(i) + "\n";
  }

  _server.send(404, "text/plain", message);
}

void connectToWiFi()
{
  WiFi.mode(WIFI_STA);

  if (WiFi.SSID() != _wifiName) 
  {
    Serial.println("Creating new connection to wifi");
    WiFi.begin(_wifiName, _wifiPassword);
    WiFi.persistent(true);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
  }
  else
  {
    Serial.println("Using existing wifi settings...");
  }

 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(_wifiName);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
 
  // Set server routing
  restServerRouting();
  // Set not found response
  _server.onNotFound(handleNotFound);
  // Start server
  _server.begin();

  Serial.println("HTTP server started");
}


//192.168.1.40
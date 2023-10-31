#include <Arduino.h>

#include "dd_dht.h"

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include "../lib_cond/lib_cond.h"
#include "../lib_cond/lib_cond.h"

#define DHTTYPE DHT11 // DHT 11

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

float temperature = 0;
int temperature_error = 0;

float humidity = 0;
int humidity_error = 0;

float dd_dht_GetTemperature(void)
{
  return temperature;
}
float dd_dht_GetHumidity(void)
{
  return humidity;
}


int dd_dht_GetTemperatureError(void){
  return temperature_error;
}
int dd_dht_GetHumidityError(void){
  return humidity_error;
}

void dd_dht_setup()
{
  // Serial.begin(9600);
  //  Initialize device.
  dht.begin();
  // Serial.println(F("DHTxx Unified Sensor Example"));
  //  Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  // Serial.println(F("------------------------------------"));
  // Serial.println(F("Temperature Sensor"));
 
  dht.humidity().getSensor(&sensor);
  // Serial.println(F("Humidity Sensor"));

  delayMS = sensor.min_delay / 1000;
}

void dd_dht_loop()
{
  // Delay between measurements.
  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature))
  {
    // Serial.println(F("Error reading temperature!"));
    temperature_error = 1;
  }
  else
  {
    temperature_error = 0;
    temperature = event.temperature;
    // Serial.print(F("Temperature: "));
    // Serial.print(temperature);
    // Serial.println(F("°C"));
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity))
  {
    // Serial.println(F("Error reading humidity!"));
    humidity_error = 1;
  }
  else
  {
    humidity_error = 0;

    humidity = event.relative_humidity;
    // Serial.print(F("Humidity: "));
    // Serial.print(humidity);
    // Serial.println(F("%"));
  }
}
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
#define INPUT_BUFF_SIZE 8
float temp_buff_in[INPUT_BUFF_SIZE];
#define MEDIAN_BUFF_SIZE 5
float temp_buff_med[MEDIAN_BUFF_SIZE];

#define LPF_BUFF_SIZE 4
float temp_buff_lpf[LPF_BUFF_SIZE];

float temp_weights_lpf[LPF_BUFF_SIZE] =
    {50, 25, 15, 10};
    
void dd_dht_filter_temp(){
  if (dd_dht_GetTemperatureError() == 0)
  {
    float temp = dd_dht_GetTemperature();
    Serial.print(F("Temperature: "));
    Serial.print(temp);
    Serial.println(F("°C"));

    // 1. FILTRU MEDIAN
    // 1.1. coletam fluxul de intrare in bufer FIFO
    fifo_push(temp, temp_buff_in, INPUT_BUFF_SIZE);
    print_buff(temp_buff_in, INPUT_BUFF_SIZE);

    // 1.2. luam o copie din buferul de intrare
    buf_copy(temp_buff_in, temp_buff_med, MEDIAN_BUFF_SIZE);
    print_buff(temp_buff_med, MEDIAN_BUFF_SIZE);

    // 1.3. sortam copia
    buf_sort(temp_buff_med, MEDIAN_BUFF_SIZE);
    print_buff(temp_buff_med, MEDIAN_BUFF_SIZE);

    // 1.4. extragem mediana
    float temp_median = temp_buff_med[MEDIAN_BUFF_SIZE / 2];

    // raportam valoarea mediana
    Serial.print(F("Temperature MEDIAN: "));
    Serial.print(temp_median);
    Serial.println(F("°C"));

    // 1. FILTRU TRECE JOS (media ponderata)
    // 1.1. coletam fluxul de intrare in bufer FIFO
    fifo_push(temp_median, temp_buff_lpf, LPF_BUFF_SIZE);
    print_buff(temp_buff_lpf, LPF_BUFF_SIZE);

    // 1.2 evaluam media ponderata
    float temp_flt = buf_wavg(temp_buff_lpf, temp_weights_lpf, LPF_BUFF_SIZE);

    // raportam valoarea filtrata
    Serial.print(F("Temperature FTJ: "));
    Serial.print(temp_flt);
    Serial.println(F("°C"));
  }
  else
  {
    // Serial.println(F("Error reading temperature!"));
  }
}
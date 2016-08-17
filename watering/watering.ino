
#include "Timer.h"
#include <EEPROM.h>


int sensorPin = A4;
int ledPin = 13;      

char in_byte = 0;
char out_byte = 0;
char* command;
char* value;
char string[30];

int i = 0;
int low_pass = 50;
bool pump_state = true;
int humidity = 0;
int on_time = 1500;
int low_treshold = 800;
bool state = true;
bool get_value = false;

// eeprom addresses
int addr_treshold = 0;
int addr_on_time = 4;

Timer t;
Timer pump;

void read_sensor(int* value);
void stop_pump();


void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);

  analogReference(DEFAULT); 

  read_eeprom_values();

  Serial.begin(9600);

  t.every(10000, run_water_algorithm);


  
}

void loop() {

  if (Serial.available() > 0) {

    int i = 0;
    
    while(Serial.available() > 0)
    {
      in_byte = Serial.read();
      string[i] = in_byte;
      ++i;
      string[i] = '\0';
    } 
    

    command = strtok(string," ");
  
    if (command == 0)
    {
      Serial.println("nothing received");
    }
    else
    {      
      value = strtok(NULL, " ");
  
      if(value == 0)
      {
        get_value = true;
      }
      else
      {
        get_value = false;
      }
      
    }



    if( strcmp(command, "pump") == 0)
    {
      pump_state = atoi(value);

      // switch pump on or off
      digitalWrite(5, pump_state);

      Serial.println("pump cmd received");
    }
    else if ( !strcmp(command, "sensor"))
    {
      read_sensor(&humidity);
      Serial.println(humidity);
    }
    else if ( !strcmp(command, "ontime"))
    {
      if(get_value == true)
      {
        Serial.print("ontime: ");
        Serial.println(on_time);
      }
      else
      {
        on_time = atoi(value);
        EEPROM.put(addr_on_time, on_time);
      }
    }
    else if ( !strcmp(command, "treshold"))
    {
      if(get_value == true)
      {
        Serial.print("treshold: ");
        Serial.println(low_treshold);
      }
      else
      {
        low_treshold = atoi(value);
        EEPROM.put(addr_treshold, low_treshold);
      }
    }
    else if (strcmp(command, "state") == 0)
    {
      if(get_value == true)
      {
        Serial.print("state: ");
        Serial.println(state);
      }
      else
      {
        state = atoi(value);
      }
    }
    else
    {
      Serial.println("unknown command");
    }

  }

  t.update();
  pump.update();
  
}

void read_sensor(int* value)
{
    unsigned long mean_adc_value = 0;
    
    // switch on sensor 
    digitalWrite(2, HIGH);

    // wait for things to stabilize
    delay(2000);

    int i = 0;
    
    for(i = 0; i < low_pass; i++)
    {
      mean_adc_value += analogRead(sensorPin);
      delay(2);
    }
    mean_adc_value = mean_adc_value / low_pass;

    *value = mean_adc_value;

    // turn off sensor
    digitalWrite(2, LOW);

}


void run_water_algorithm()
{
  if (state == true)
  {
    read_sensor(&humidity);

    if (humidity > low_treshold)
    {
      // start pump
      digitalWrite(5, HIGH);
      pump.every(on_time, stop_pump, 1);
      Serial.println("pump started");
    }
  } 

}

void stop_pump()
{
  digitalWrite(5, LOW);
  Serial.println("pump stopped");
}

void read_eeprom_values()
{
  EEPROM.get(addr_on_time, on_time);
  EEPROM.get(addr_treshold, low_treshold);
}



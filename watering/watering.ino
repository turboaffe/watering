
#include "Timer.h"


int sensorPin = A4;    // select the input pin for the potentiometer
int ledPin = 13;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor

char in_byte = 0;
char out_byte = 0;
char* command;
char* value;
char string[30];
int i = 0;
bool pump_state = false;
int humidity = 0;
int on_time = 2000;
int low_treshold = 500;
bool state = false;

Timer t;
Timer pump;

void read_sensor(int* value);
void stop_pump();


void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(5, OUTPUT);

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  t.every(10000, run_water_algorithm);
  
}

void loop() {

  if (Serial.available() > 0) {

    while ((in_byte != '\n') && (i < 30))
    {
      in_byte = Serial.read();
      string[i] = in_byte;
      ++i;
    }
    string[i] = '\n';
      
    in_byte = 0;
    i = 0;

    
    command = strtok(string," ");
    value = strtok(NULL," ");

    // write received command
    //    Serial.print(command);
    //    Serial.print(value);

    if( !strcmp(command, "pump"))
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
      on_time = atoi(value);
      Serial.println("ontime ok");
    }
    else if ( !strcmp(command, "treshold"))
    {
      low_treshold = atoi(value);
      Serial.println("treshold ok");
    }
    else if ( !strcmp(command, "state"))
    {
      state = atoi(value);
      Serial.println("state ok");
    }
    else
    {
      Serial.println("unknown command");
    }

  }

  t.update();
  pump.update();
  

  // do the algorithm

  // check the sensor value with a low pass filter

//  // collect 5 values, add them and mean
//  for(i = 0, i < 5, ++i)
//  {
//    
//  }



  // if the sensor is below a certain value for a while, give a short burst of water
  
  
  

}

void read_sensor(int* value)
{
    // switch on sensor 
    digitalWrite(2, HIGH);

    // wait for things to stabilize
    delay(200);
    
    // read the value from the sensor:
    *value = analogRead(sensorPin);

    delay(20);
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




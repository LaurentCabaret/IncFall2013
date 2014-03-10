/************************************************************************
 * INC courses : 2014 - Spring - Test module for lamp electronics       *
 *                                                                      *
 * Copyright : (c) 2014 Xavier Lagorce <Xavier.Lagorce@crans.org>       *
 ************************************************************************/

//**** Global variables used for global parameters
// Inputs :
int poten = A0; // Potentiometer
int sw = 2; // User button
// Ouputs :
int led_ind = 13;   // Arduino LED
int led_blue = 9;   // RGB LEDs
int led_green = 10; //    "
int led_red = 11;   //    "
// Shared variables
int blink_state = 0;
boolean alive_state = false;
int sensor_value = 0;
boolean light_up = false;
// Program parameters
unsigned long alive_period = 500;
unsigned long serial_period = 50;
unsigned long read_sensor_period = 200;
#define SERIAL_BUF_SIZE 10

// This function is called one time when the arduino starts
void setup() {
  // Set output pins as outputs
  pinMode(led_ind, OUTPUT);
  pinMode(led_blue, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_red, OUTPUT);
  
  // Set default values for outputs (all LEDs OFF)
  digitalWrite(led_ind, LOW);
  digitalWrite(led_blue, HIGH);
  digitalWrite(led_red, HIGH);
  digitalWrite(led_green, HIGH);
  
  // Open the serial link at 115200 bps
  Serial.begin(9600);
}

// This function is called continuously when the arduino runs
void loop() {
  i_m_alive_loop();
  read_sensor_loop();
  serial_loop();
}

// Simple task to indicate that the code is not stuck somewhere
void i_m_alive_loop() {
  // This static variable will keep its value between different calls to this function
  //  but like all local variables won't be accessible from the outside of this function.
  static unsigned long last_time = 0;
  
  // Get the number of milliseconds ellapsed since the start of the arduino
  unsigned long cur_time = millis();
  
  // Only do something if a sufficient amount of time has passed by
  //  (and don't block the program to wait if not)
  if (cur_time - last_time > alive_period) {
    // Things to do periodically
    if (alive_state) {
      digitalWrite(led_ind, HIGH);
    } else {
      digitalWrite(led_ind, LOW);
    }
    alive_state = !alive_state;
    
    // Keep track of the last time the task was executed
    last_time = cur_time;
  }
}

// Manage the serial communication with the central system
void serial_loop() {
  // This static variable will keep its value between different calls to this function
  //  but like all local variables won't be accessible from the outside of this function.
  static unsigned long last_time = 0;
  static unsigned char buffer[SERIAL_BUF_SIZE];
  static unsigned char buffer_ind = 0;
  
  // Get the number of milliseconds ellapsed since the start of the arduino
  unsigned long cur_time = millis();

  // Only do something if a sufficient amount of time has passed by
  //  (and don't block the program to wait if not)
  if (cur_time - last_time > serial_period) {
    while (Serial.available() > 0) {
      buffer[buffer_ind] = Serial.read();
      if (buffer[buffer_ind] == '\r') {
        // process command
        if (buffer_ind == 4) {
          // correct packet size
          if (buffer[0] == 'B' && buffer[1] == 'T') {
            switch (buffer[2] - '0') {
              case 0:
                digitalWrite(led_blue, buffer[3] == 'D' ? LOW : HIGH);
                if (buffer[3] == 'D') {
                  light_up = !light_up;
                }
                break;
              case 1:
                digitalWrite(led_red, buffer[3] == 'D' ? LOW : HIGH);
                break;
              case 2:
                digitalWrite(led_green, buffer[3] == 'D' ? LOW : HIGH);
                break;
               default:
                 // shouldn't happen
                 break;
            }
          } else if (buffer[0] == 'L' && buffer[1] == 'I' && buffer[2] == '1' && buffer[3] == '?') {
            if (light_up) {
              Serial.print("LI1U\r");
            } else {
              Serial.print("LI1D\r");
            }
          }
        }
        buffer_ind = 0;
      } else {
        buffer_ind++;
        if (buffer_ind >= SERIAL_BUF_SIZE) {
          buffer_ind = 0;
        }
      }
    }    
    last_time = cur_time;
  }
}

// This task reads the sensor input and relays the value to the computer
void read_sensor_loop() {
  // This static variable will keep its value between different calls to this function
  //  but like all local variables won't be accessible from the outside of this function.
  static unsigned long last_time = 0;
  
  // Get the number of milliseconds ellapsed since the start of the arduino
  unsigned long cur_time = millis();
  
  // Only do something if a sufficient amount of time has passed by
  //  (and don't block the program to wait if not)
  if (cur_time - last_time > read_sensor_period) {
    // Read the potentiometer input
    sensor_value = analogRead(poten);
    
    last_time = cur_time;
  }
}

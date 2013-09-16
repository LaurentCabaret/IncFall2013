/************************************************************************
 * Electronics tutorials for INC courses : 03 - Tasks                   *
 *                                                                      *
 *   This program demonstrates how to have several tasks running        *
 * "simultaneously "on the arduino.                                     *
 *                                                                      *
 * Copyright : (c) 2013 Xavier Lagorce <Xavier.Lagorce@crans.org>       *
 ************************************************************************/

//**** Global variables used for global parameters
// Inputs :
int sw = 2; // User button
// Ouputs :
int led_ind = 13;   // Arduino LED
int led_blue = 9;   // RGB LEDs
int led_green = 10; //    "
int led_red = 11;   //    "
// Shared variables
int blink_state = 0;
boolean alive_state = false;
// Program parameters
unsigned long alive_period = 500;
unsigned long kikoo_period = 200;

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
}

// This function is called continuously when the arduino runs
void loop() {
  i_m_alive_loop();
  kikoo_loop();
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

// Some task executing what we want the arduino to do
void kikoo_loop() {
  // This static variable will keep its value between different calls to this function
  //  but like all local variables won't be accessible from the outside of this function.
  static unsigned long last_time = 0;
  
  // Get the number of milliseconds ellapsed since the start of the arduino
  unsigned long cur_time = millis();
  
  // Only do something if a sufficient amount of time has passed by
  //  (and don't block the program to wait if not)
  if (cur_time - last_time > kikoo_period) {
    // Stops all LEDs
    digitalWrite(led_blue, HIGH);  // OFF
    digitalWrite(led_red, HIGH);   // OFF
    digitalWrite(led_green, HIGH); // OFF
    // Light the right one in the sequence
    switch(blink_state) {
      case 0:
        digitalWrite(led_red, LOW);
        break;
      case 1:
        digitalWrite(led_blue, LOW);
        break;
      case 2:
        digitalWrite(led_green, LOW);
        break;
    }
    // Only change the system state if the button is pressed
    if (digitalRead(sw) == LOW) {
      blink_state = (blink_state + 1) % 3;
    }
    
    last_time = cur_time;
  }
}

/************************************************************************
 * Electronics tutorials for INC courses : 02 - Timings                 *
 *                                                                      *
 *   This program demonstrates the use of some timing functions of the  *
 *  arduino library.                                                    *
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
  // Wait for 400 ms
  delay(400);
}

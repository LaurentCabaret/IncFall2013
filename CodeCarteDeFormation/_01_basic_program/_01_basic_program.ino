/************************************************************************
 * Electronics tutorials for INC courses : 01 - Basic program           *
 *                                                                      *
 *   This program demonstrates the minimal contents of an arduino       *
 *  program and use of digital inputs/outputs.                          *
 *                                                                      *
 * Copyright : (c) 2013 Xavier Lagorce <Xavier.Lagorce@crans.org>       *
 ************************************************************************/

// Global variables used for global parameters
// Inputs :
int sw = 2; // User button

// Ouputs :
int led_ind = 13;   // Arduino LED
int led_blue = 9;   // RGB LEDs
int led_green = 10; //    "
int led_red = 11;   //    "

// This function is called one time when the arduino starts
void setup() {
  // Set output pins as outputs
  pinMode(led_ind, OUTPUT);
  pinMode(led_blue, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_red, OUTPUT);
  
  // Set default values for outputs
  digitalWrite(led_ind, LOW);
  digitalWrite(led_blue, HIGH);
  digitalWrite(led_red, HIGH);
  digitalWrite(led_green, HIGH);
}

// This function is called continuously when the arduino runs
void loop() {
  // Let's read the button state and light the LEDs when it's pressed
  if (digitalRead(sw) == LOW) {
    digitalWrite(led_ind, LOW);    // OFF
    digitalWrite(led_blue, LOW);   // ON
    digitalWrite(led_red, LOW);    // ON
    digitalWrite(led_green, LOW);  // ON
  } else {
    digitalWrite(led_ind, HIGH);   // ON
    digitalWrite(led_blue, HIGH);  // OFF
    digitalWrite(led_red, HIGH);   // OFF
    digitalWrite(led_green, HIGH); // OFF
  }
}

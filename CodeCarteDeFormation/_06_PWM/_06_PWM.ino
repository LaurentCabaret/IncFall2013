/************************************************************************
 * Electronics tutorials for INC courses : 06 - PWM                     *
 *                                                                      *
 *   This program demonstrates the Pulse Width Modulation (PWM)         *
 * functionnality of some of the arduino pins.                          *
 *                                                                      *
 * Copyright : (c) 2013 Xavier Lagorce <Xavier.Lagorce@crans.org>       *
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
boolean is_blinking = false;
boolean alive_state = false;
int sensor_value = 0;
volatile boolean button_pressed = false;  // These variables need to be volatile
volatile boolean button_released = false; // to be changed in the interrupt handle
// Program parameters
unsigned long alive_period = 500;
unsigned long kikoo_period = 200;
unsigned long read_sensor_period = 200;

// Interrupts handle
void button_interrupt_handle() {
  // This function is called whenever the button changes state it should be as quick as possible.
  
  // Detect button pressed == falling edge on the pin
  if (digitalRead(sw) == LOW) {
    button_pressed = true;
  } else {
    // Detect button released == rising edge on the pin
    button_released = true;
  }
}

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
  
  // Attach handles to interrupt sources
  attachInterrupt(0, button_interrupt_handle, CHANGE);
  
  // Open the serial link at 115200 bps
  Serial.begin(115200);
  // Be polite
  Serial.println("Hello, I'm waking up... and you ?");
}

// This function is called continuously when the arduino runs
void loop() {
  i_m_alive_loop();
  read_sensor_loop();
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

// Blinken Lichten
void kikoo_loop() {
  // This static variable will keep its value between different calls to this function
  //  but like all local variables won't be accessible from the outside of this function.
  static unsigned long last_time = 0;
  
  // Get the number of milliseconds ellapsed since the start of the arduino
  unsigned long cur_time = millis();
  
  // Change if we are blinking or not at each press of the button
  if (button_pressed) {
    is_blinking = !is_blinking;
    button_pressed = false;
  }
  
  // Blink if we should, else stops all LEDs
  if (!is_blinking) {
    analogWrite(led_blue, 255);  // OFF
    analogWrite(led_red, 255);   // OFF
    analogWrite(led_green, 255); // OFF
  } else {
    // Only do something if a sufficient amount of time has passed by
    //  (and don't block the program to wait if not)
    if (cur_time - last_time > kikoo_period) {
      // Stops all LEDs
      analogWrite(led_blue, 255);  // OFF
      analogWrite(led_red, 255);   // OFF
      analogWrite(led_green, 255); // OFF
      // Determine the intensity value
      int intensity = 255-((long)sensor_value)*255/710;
      // Light the right one in the sequence
      switch(blink_state) {
        case 0:
          analogWrite(led_red, intensity);
          break;
        case 1:
          analogWrite(led_blue, intensity);
          break;
        case 2:
          analogWrite(led_green, intensity);
          break;
      }
      
      // Go to next blinking state
      blink_state = (blink_state + 1) % 3;
      
      last_time = cur_time;
    }
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
    
    // Send the value to the computer
    Serial.print("New analog reading : ");
    Serial.print(sensor_value);
    // Display a simple graph of the value
    Serial.print(". \t|");
    for (int i=0; i < 20; i++) {
      if (i == sensor_value*20/1024) {
        Serial.print("*");
      } else {
        Serial.print(" ");
      }
    }
    Serial.println("|");
    
    last_time = cur_time;
  }
}

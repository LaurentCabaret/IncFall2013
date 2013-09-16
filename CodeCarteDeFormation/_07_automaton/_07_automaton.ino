/************************************************************************
 * Electronics tutorials for INC courses : 07 - Automaton               *
 *                                                                      *
 *   This program demonstrates the use of an automaton to control       *
 * everything easily.                                                   *
 *                                                                      *
 * Copyright : (c) 2013 Xavier Lagorce <Xavier.Lagorce@crans.org>       *
 ************************************************************************/

// States of the control automaton
typedef enum {
  state_idle,
  state_set_color,
  state_enter_config,
  state_config_red,
  state_config_blue,
  state_config_green,
} control_state;


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
boolean alive_state = false;
volatile boolean button_pressed = false;  // These variables need to be volatile
volatile boolean button_released = false; // to be changed in the interrupt handle
control_state state;
int red_int = 255;
int blue_int = 255;
int green_int = 255;
// Program parameters
unsigned long control_period = 100;
unsigned long config_press_time = 2000;
unsigned long alive_period = 500;

#define DEBOUNCE_TIME 10

// Interrupts handle
void button_interrupt_handle() {
  // This function is called whenever the button changes state it should be as quick as possible.
  static unsigned long last_event = 0;

  // Detect button pressed == falling edge on the pin
  if (millis() - last_event > DEBOUNCE_TIME && digitalRead(sw) == LOW) {
    button_pressed = true;
  } else {
    if (millis() - last_event > DEBOUNCE_TIME) {
      // Detect button released == rising edge on the pin
      button_released = true;
    }
  }
  last_event = millis();
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
  
  // Set initial state
  state = state_set_color;
}

// This function is called continuously when the arduino runs
void loop() {
  i_m_alive_loop();
  control_loop();
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

// Main task
void control_loop() {
  // This static variable will keep its value between different calls to this function
  //  but like all local variables won't be accessible from the outside of this function.
  static unsigned long last_time = 0;
  
  // Get the number of milliseconds ellapsed since the start of the arduino
  unsigned long cur_time = millis();

  // To keep track of timings
  static unsigned long btn_pressed_at;
  
  // Only do something if a sufficient amount of time has passed by
  //  (and don't block the program to wait if not)
  if (cur_time - last_time > control_period) {
    switch(state) {
      case state_idle:
          // If the button is pressed, keep note of the current
          // time and wait for the release
          if (button_pressed) {
            state = state_enter_config;
            button_pressed = false;
            button_released = false;
            btn_pressed_at = cur_time;
          } else {
            // Ack a previous release
            if (button_released) {
              button_released = false;
            }
          }
        break;
      case state_set_color:
        // Set correct LED color and go to idle state
        analogWrite(led_red, red_int);
        analogWrite(led_blue, blue_int);
        analogWrite(led_green, green_int);
        state = state_idle;
        break;
      case state_enter_config:
        // Go to config mode if the button stayed pressed for long enough
        if (cur_time - btn_pressed_at >= config_press_time) {
          // Stop every LED before going to config mode
          analogWrite(led_red, 255);
          analogWrite(led_blue, 255);
          analogWrite(led_green, 255);
          state = state_config_red;
          alive_period = 100;
        } else {
          if (button_released) {
            state = state_idle;
            button_released = false;
          }
        }
        break;
      case state_config_red:
        // Set the red intensity according to the potentiometer
        red_int = 255-((long)analogRead(poten))*255/710;
        analogWrite(led_red, red_int);
        // Go to next config if the button is pressed
        if (button_pressed) {
          button_pressed = false;
          analogWrite(led_red, 255);
          state = state_config_blue;
        }
        break;
      case state_config_blue:
        // Set the blue intensity according to the potentiometer
        blue_int = 255-((long)analogRead(poten))*255/710;
        analogWrite(led_blue, blue_int);
        // Go to next config if the button is pressed
        if (button_pressed) {
          button_pressed = false;
          analogWrite(led_blue, 255);
          state = state_config_green;
        }
        break;
      case state_config_green:
        // Set the green intensity according to the potentiometer
        green_int = 255-((long)analogRead(poten))*255/710;
        analogWrite(led_green, green_int);
        // Exit config mode if button is pressed
        if (button_pressed) {
          button_pressed = false;
          analogWrite(led_green, 255);
          state = state_set_color;
          alive_period = 500;
        }
        break;
      default:
        state = state_set_color;
        break;
    }
    
    last_time = cur_time;
  }  
}


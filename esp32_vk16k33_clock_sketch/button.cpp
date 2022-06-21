#include "HT16K33.h"
#include "button.h"

const int DEBOUNCE_DELAY = 40;   // the debounce time; increase if the output flickers

Button::Button(int pin) 
{
  buttonPin   = pin;
  pinMode(buttonPin, INPUT_PULLUP);

  // Variables will change:
  lastSteadyState = LOW;       // the previous steady state from the input pin
  lastFlickerableState = LOW;  // the previous flickerable state from the input pin
  currentState;                // the current reading from the input pin

  // the following variables are unsigned longs because the time, measured in
  // milliseconds, will quickly become a bigger number than can be stored in an int.
  lastDebounceTime = 0;  // the last time the output pin was toggled
}

int Button::checkState() 
{
  // read the state of the switch/button:
  currentState = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch/button changed, due to noise or pressing:
  if (currentState != lastFlickerableState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
    // save the the last flickerable state
    lastFlickerableState = currentState;
  }

  int ret_val = NO_CHANGE;
  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (lastSteadyState == HIGH && currentState == LOW)
    {
      //The button is pressed
      ret_val = BUTTON_DOWN; 
    }
    else if (lastSteadyState == LOW && currentState == HIGH)
    {
      //The button is released
      ret_val = BUTTON_UP;
    }
    
    // save the the last steady state
    lastSteadyState = currentState;
  } 
  return ret_val; 
}

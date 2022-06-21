//#include "action.h"

#define NO_CHANGE 0
#define BUTTON_DOWN 1
#define BUTTON_UP 2

class Button 
{
  private:
    int buttonPin;
    
  public:
    Button(int pin);
    int checkState();
    int currentState;                // the current reading from the input pin
    int lastFlickerableState;
    unsigned long lastDebounceTime;
    int lastSteadyState;
};

#define debounce 100
#define DCgap 100
#define holdTime 2000
#define longHoldTime 5000

class MyButton
{
  public: 
  uint8_t buttonPin = -1;
  boolean buttonVal = HIGH;   // value read from button
  boolean buttonLast = HIGH;  // buffered value of the button's previous state
  boolean DCwaiting = false;  // whether we're waiting for a double click (down)
  boolean DConUp = false;     // whether to register a double click on next release, or whether to wait and click
  boolean singleOK = true;    // whether it's OK to do a single click
  long downTime = -1;         // time the button was pressed down
  long upTime = -1;           // time the button was released
  boolean ignoreUp = false;   // whether to ignore the button release because the click+hold was triggered
  boolean waitForUp = false;        // when held, whether to wait for the up event
  boolean holdEventPast = false;    // whether or not the hold event happened already
  boolean longHoldEventPast = false;// whether or not the long hold event happened already

public:
  MyButton(int pin) {
    buttonPin = pin;
  }

  int checkButton() {    

   int event = 0;
   buttonVal = digitalRead(buttonPin);
   // Button pressed down
   if (buttonVal == LOW && buttonLast == HIGH && (millis() - upTime) > debounce)
   {
       downTime = millis();
       ignoreUp = false;
       waitForUp = false;
       singleOK = true;
       holdEventPast = false;
       longHoldEventPast = false;
       if ((millis()-upTime) < DCgap && DConUp == false && DCwaiting == true)  DConUp = true;
       else  DConUp = false;
       DCwaiting = false;
   }
   // Button released
   else if (buttonVal == HIGH && buttonLast == LOW && (millis() - downTime) > debounce)
   {        
       if (not ignoreUp)
       {
           upTime = millis();
           if (DConUp == false) DCwaiting = true;
           else
           {
               event = 2;
               DConUp = false;
               DCwaiting = false;
               singleOK = false;
           }
       }
   }
   // Test for normal click event: DCgap expired
   if ( buttonVal == HIGH && (millis()-upTime) >= DCgap && DCwaiting == true && DConUp == false && singleOK == true && event != 2)
   {
       event = 1;
       DCwaiting = false;
   }
   // Test for hold
   if (buttonVal == LOW && (millis() - downTime) >= holdTime) {
       // Trigger "normal" hold
       if (not holdEventPast)
       {
           event = 3;
           waitForUp = true;
           ignoreUp = true;
           DConUp = false;
           DCwaiting = false;
           //downTime = millis();
           holdEventPast = true;
       }
       // Trigger "long" hold
       if ((millis() - downTime) >= longHoldTime)
       {
           if (not longHoldEventPast)
           {
               event = 4;
               longHoldEventPast = true;
           }
       }
   }
   buttonLast = buttonVal;
   return event;
  }
};
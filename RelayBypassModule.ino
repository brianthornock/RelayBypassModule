int relay = 0; //Relay out on PB0/Pin 5
int button = 4; //Button in on PB4/Pin 3

uint8_t relayState = LOW;
uint8_t buttonState;
uint8_t lastButtonState = LOW;

unsigned long lastDebounceTime; //Time stamp of last time we had a button press candidate
unsigned long debounceDelay = 10; //Number of ms to ensure a properly debounced button
unsigned long lastButtonDownTime; //Time stamp of last time the button was pressed down
const unsigned int buttonPressTime = 500; // How long the button has to be pressed to be counted as a press and hold
uint8_t flag; //Used for indicating if we have a button press interrupt
uint8_t tempDebounceState = 0; //A place holder for debounce state during interrupt routine

void setup() {
  //Configure pin modes and states
  pinMode(relay, OUTPUT);
  pinMode(button, INPUT);
  digitalWrite(relay, LOW);

  //Configure the pin change interrupt settings for fast response to button presses
  GIMSK = 0b00100000; // Enable pin change interrupts
  PCMSK = 0b00010000; // Enable PCIE on PB4
  sei(); //Start interrupt routine
}


//Interrupt service routine to be run when our button is pressed
ISR (PCINT0_vect){
  
  int reading = digitalRead(button);

  // If the status of the interrupt is not the same as the last time we dealt with the button, set flag high to deal with it again
  if (reading != lastButtonState){
    tempDebounceState = reading;
    flag = 1;
  }
}

void loop() {
  // Don't do anything until a button is pressed
  if (flag!=0){
    checkButton();
  }
}



void checkButton(){
  if ((millis()-lastDebounceTime) > debounceDelay){ 
        if (digitalRead(button)==tempDebounceState){
          //The switch has been confirmed as debounced
          buttonState = tempDebounceState;
        
          if (buttonState == HIGH && lastButtonState == LOW){
            // We have a high to low transition. Mark the time and toggle the relay state.
            relayState = !relayState;
            lastButtonDownTime = millis();
          }
          //Check to see if we had an on/off transition. If so, see if it has been at least buttonPressTime long
          if (buttonState == LOW && lastButtonState == HIGH && (millis()-lastButtonDownTime>buttonPressTime)){
            //We have a low to high transition, so we turn the relay off
            relayState = LOW;
          }
          digitalWrite(relay,relayState);
          lastButtonState = buttonState;
        }
        flag = 0; //Now that we have waited long enough to know if it is debounced and have taken any necessary action, reset flag to low
      }
}

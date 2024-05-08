#ifndef UPS_h
#define UPS_h

class UPS{
  private:
    int condition;

  public:
    //int State(int state, int alarm);
    int State(int pin_state_UPS);
    //int State(int pin_state_LV);
    int Alarm(int alarm);   //Function used to detect if there ara alarms signal in the LV
};

int UPS::State(int pin_state_UPS){
    Serial.print("- State UPS:         ");Serial.println(digitalRead(pin_state_UPS));
    if(digitalRead(pin_state_UPS) == 1){
      return(1);
    }
    else{return(0);}
}

#endif
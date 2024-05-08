/* Functions for the switch on/off procedure, here we have the new funcition that avoid the use 
 * dealy() fun in the code.
 */
#ifndef InterLock_h
#define InterLock_h

int pin_interlock_MF_LV = CONTROLLINO_R0;
int pin_interlock_MF_HV = CONTROLLINO_R1;

//Interolock single board LV.
int pin_interlock_board1_HV = CONTROLLINO_R2;


float dt_LV;
float T_start_Ramp_down;


class InterLock {
  private:

  public:                                 //Note: var is the state (can be 1 or 0).
    void configInterLockPIN(void);
    void interlock_HV(int var);           //General InterLock signals for HV and LV.
    void interlock_MF_HV(int var);        //Interlock signal for HV Main frame.
    void interlock_MF_LV(int var);        //Interlock signal for LV Main frame.
    void interlock_board1_HV(int var);    //InterLock signals for single board.
    void switch_OFF(int ramp_down);       //Safe switch off.
    void switch_OFF1(float ramp_down, bool Start_Ramp_down);       //Safe switch off.
    void General_INTERLOCK(int var);      //switch off everything.
};

void InterLock::configInterLockPIN(){
  // //The main frame interlocks.
  // int pin_interlock_MF_LV = CONTROLLINO_R0;
  // int pin_interlock_MF_HV = CONTROLLINO_R1;
  // //Interolock single board LV.
  // int pin_interlock_board1_LV = CONTROLLINO_R2;
  // int pin_interlock_board1_HV = CONTROLLINO_R3;
  //Define pin modes
  pinMode(pin_interlock_MF_LV,OUTPUT);
  pinMode(pin_interlock_MF_HV,OUTPUT);
  // pinMode(pin_interlock_board1_LV,OUTPUT);
  pinMode(pin_interlock_board1_HV,OUTPUT);
}

void InterLock::interlock_HV(int var){
  if(var==1){digitalWrite(pin_interlock_MF_HV,HIGH);
             digitalWrite(pin_interlock_board1_HV,HIGH);}
  if(var==0){digitalWrite(pin_interlock_MF_HV,LOW);
             digitalWrite(pin_interlock_board1_HV,LOW);}
  else{}
}
void InterLock::interlock_MF_LV(int var){
  if(var==1){digitalWrite(pin_interlock_MF_LV,HIGH);}
  if(var==0){digitalWrite(pin_interlock_MF_LV,LOW);}
  else{}
}
void InterLock::interlock_MF_HV(int var){
  if(var==1){digitalWrite(pin_interlock_MF_HV,HIGH);}
  if(var==0){digitalWrite(pin_interlock_MF_HV,LOW);}
  else{}
}
void InterLock::interlock_board1_HV(int var){
  if(var==1){digitalWrite(pin_interlock_board1_HV,HIGH);}
  if(var==0){digitalWrite(pin_interlock_board1_HV,LOW);}
}

//Function to switch of the system in a Safe way.
void InterLock::switch_OFF(int ramp_down){
  interlock_HV(0);
  Serial.println("Ramp Down");
  delay(ramp_down);   //for the moment this may be enough to ensure that the LV is
                      //on during the switch off of HV.
  interlock_MF_LV(0);
  Serial.println("All OFF");
}

//Function to switch of the system in a Safe way.
void InterLock::switch_OFF1(float ramp_down, bool Start_Ramp_down){
  //Serial.print("  "); Serial.println(Start_Ramp_down);
  if(Start_Ramp_down == true){
    Serial.println("------------------------------------------------------------START RAMP DOWN");
    Serial.print(dt_LV);Serial.print("    ");Serial.println(ramp_down);
    T_start_Ramp_down = millis();
    //Start_Ramp_down = false;
  }
  Serial.print("STARTTTT RAMP DOWN:  "); Serial.print(T_start_Ramp_down); Serial.print("  "); Serial.println(Start_Ramp_down);
  interlock_HV(0);
  dt_LV = millis() - T_start_Ramp_down;
  if(dt_LV > ramp_down){
    interlock_MF_LV(0);
    Serial.println("-----------------------------------------------------------------All IS OFF");
    
    T_start_Ramp_down = millis();
  }
}

//Set all the interlock signals at a given value (HIGH or LOW).
void InterLock::General_INTERLOCK(int var){
  interlock_HV(var);
  interlock_MF_LV(var);
}


#endif

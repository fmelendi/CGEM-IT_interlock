#ifndef cooling_h
#define cooling_h

//cooling calibration
// 4.4 l/min 1.85V 0l/min 0.88V

int pin_cooling_flowmeter_1 = CONTROLLINO_A14;

// int pin_chiller_alarm       = 17;
// int pin_chiller_run         = 19;

float cooling_flow_threshold = 3.9;
int N_cooling_measure        = 10;

float Cooling_flow_1  = 0.00;         

class cooling {
  
  private:
    float flux_1      = 0.00;
    float flux_sm_1   = 0.00;
    float ref_voltage = 5.0;
    float N_digits    = 1023.0;
    int sp = 1;

  public:
    void configuration();
    float cooling_measure_flux();
    int chiller_status();
};

void cooling::configuration(){
  pinMode(pin_cooling_flowmeter_1, INPUT);
  // pinMode(pin_chiller_alarm, INPUT);
  // pinMode(pin_chiller_run, INPUT);
}

// int cooling::chiller_status(){
//     if((digitalRead(pin_chiller_alarm) && digitalRead(pin_chiller_run)) == 0){
//         return(0);
//     }
//     else{return(1);}
// }

float cooling::cooling_measure_flux(){
    
    flux_sm_1 = 0.00;
    for(int i = 0;i < N_cooling_measure; i++){
       //flux_sm_1 = flux_sm_1 + (((analogRead(pin_cooling_flowmeter_1)/N_digits)*ref_voltage));
        //Serial.print("chiller voltage:  "); Serial.println((analogRead(pin_cooling_flowmeter_1)/N_digits)*ref_voltage);
        flux_sm_1 = flux_sm_1 + (((analogRead(pin_cooling_flowmeter_1)/N_digits)*ref_voltage)-0.89)/0.227;
    }

    flux_1 = flux_sm_1/N_cooling_measure; 
    if(sp == 1){
      Serial.print("  - Cooling measure : ");
      Serial.println(flux_1);
    }
    // Serial.print("voltage FLOWMETER 1: --------->    "); Serial.println(flux_1);

    return(flux_1);
}


#endif

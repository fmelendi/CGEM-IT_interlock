/*
 * Interlock Logic, 3 states logic for the interlock, the software gets the input parameters
 * from the libraries: environment.h, gas.h, cooling.h and LV.h (interlock.h manage the
 * interlock signals); It determines if the the conditions are met and through the interlock
 * signal control the high and low voltage (through the interLock.h library).
 * The leds ON in the controllino mean that this stage is ready to work (regarding the parameter)
 * while regarding the interlock signals mean that we have the possibility to switch on the
 * LV and HV.
 * - There is the new ramp down procedure
 * - New procedure for the initialization of Chipcap2.
 * - There are two theshold values avoid....
 * - sp variable in enviroment.h and gas.h to serial print the measures.
 * - Inster the alarms (towards the laptop);
 * - integrate again the flow meter.
 * - Introduction of the flowmeter for the gas sys, the number in the name identifies the flowmter number.
 * - Super User Mode, (SUM).
 * - Lower threshold, for both environment and gas.
 * - UDP temporization, 2 different speed of sampling.

 * - ADDED LV FIFO! (11/12/2023)
 * // memo 67, 1819 a 1!
 * 
 * 17-01-2024 
 * added TESTMODE
 * 
 * 08/04/2024
 * added chargeNewprogramm (CNP)
 */

 // --------------------------------------  TO TEST -----------------------------------------

//--- Library needed for the code.
#include <Controllino.h>
#include "gas.h"
#include "InterLock.h"
#include "cooling.h"
#include "environment.h"
#include "LV.h"

#include <Ethernet.h>
#include <EthernetUdp.h>

//----- Creation of the object needed (for our libraries) -----
cooling c     = cooling();
InterLock IL  = InterLock();
gas g         = gas();
environment e = environment();
LV lv         = LV();

//----- Test Mode -----
String activateTESTMODE = "activateTESTMODE";
String disableTESTMODE  = "disableTESTMODE";
int TESTMODE = 0;

//----- Super User Mode -----
String activateSUM        = "activateSUM";
String disableSUM         = "disableSUM";
bool SUM                  = false;                           //Activate the Super User Mode.
bool SUM_gas_skip         = false;
bool SUM_cooling_skip     = false;
bool SUM_environment_skip = false;
bool SUM_LV_skip          = false;
bool SUM_HV_skip          = false;

int LED_state_environment;
int LED_state_cooling;
int LED_state_gas;


//----- Logic parameters and variables -----

byte state_sys   = 0B0000000;                 //Bin. Number to rappresent the state of the system.
byte state_CHECK = 0;                       //Bin. Number used to keep track of the evolution of the sys.

//Variable used to write the binary number.
int state_E  = 0;
int state_G  = 0;
int state_C  = 0;
int state_L  = 0;


//Admissible states.
byte state0 = 0B00000000; //0
byte state1 = 0B00000001; //1
byte state2 = 0B00000011; //3
byte state3 = 0B00000111; //7 (for the moment this state is not possible!).

unsigned int environment_buffer_state = 0;
unsigned int gas_buffer_state         = 0;
unsigned int cooling_buffer_state     = 0;
unsigned int LV_buffer_state          = 0;

byte adm_state_sys = 0;

//---State LED---
//Each led tell us if the single stage is fine.
int led_LV  = A0;
int led_HV  = A1;
int led_env = A2;
int led_gas = A3;
int led_coo = A4;

//Length of the ramp_down of HV.
unsigned long ramp_down      = 240000;           //Length of the ramp down (may i can try with unsigned long)

bool Start_Ramp_down = true;       //If true ramp down procedure is enable, (if false vice versa). (NO TO BE SET!)

//----- SERIAL communication -----

String msg    = "";
String reset  = "RESET";

//08/04/2024
unsigned long timeinterval_CNP = 240000;

String chargeNewprogram = "CNP";
int pin_resetControl = CONTROLLINO_R4; 
unsigned long time_CNP;
bool state_CNP = false;

//----- UDP communication -----

bool UDP_state = true;             //Variable to activate the UDP comunication (for all the quantities).

unsigned long UDP_time_interval    = 5000;                                   //Time interval between UDP messages.
unsigned long UDP_time_post_alarm  = 300000;

//Enter the MAC address and IP address for your controller below.
//The IP address will be dependent on your local network:
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 2, 177);
unsigned int localPort = 8888;         // local port to listen on.

IPAddress remoteip(192, 168, 2, 176);
unsigned int remotePort = 8888;

//An EthernetUDP instance to let us send and receive packets over UDP protocol.

EthernetUDP Udp;
                                            
String env_sensors        = String("/");                          //String with the env. parameters.
String gas_sensors        = String("/");                          //String with the gas. parameters.
String Cooling_flow_1_str = String("/");                          //String with the cooling parameters.
String LV_states          = String("/");                          //String with the LV states.
String GEMROC_states      = String("/");

unsigned long time_UDP, dt_UDP;
unsigned long time_UDP_post_switchOFF, dt_UDP_post_switchOFF;     //Istant and time interval wrt when the system is again in a safe condition,
bool  UDP_transiotion      = false;
bool UDP_start_alarm_timer = true;

int N_gas_alarm_messages = 1;                                     // it used just N_coo_alarm_messages for the moment, -> to be implemented!
int N_coo_alarm_messages = 1;                                     
int N_env_alarm_messages = 1;


//----- Sensor parameters -----

//---Cooling sensors---

//COOLING-Floweter, YS-F401.
// bool sp_coolingFlow = true;             //Variable to activate the Serial comminication
// float cooling_flow_threshold = 0.0;     //Threshold for the cooling flow, (L/min).
// int pin = 19;                           //Interrupt pin for the cooling flowmeter.

// volatile int count = 0;                 //Counter for the correspondet flowmeter, (volatile ==> varialble changed by something beyond the control of the code).
// unsigned long t1,dt;                    //Variables to evaluate the time interval.
// float freq,flux,Cooling_flow_1;         //Outputs of flowmeter 1.

// int N_flow_measure = 0;


//----- Redundancy system -----

int redu_sig = 49;                      //Pin used to communicate with the redundacy system.

//----- System reset function -----
void (* resetFunc) (void) = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////

void setup(){

  
  pinMode(pin_resetControl,OUTPUT);
  digitalWrite(pin_resetControl,HIGH);

  //Open Serial communication.
  Serial.begin(9600);
  Serial.println("  ");
  Serial.println("--- Initialization of the Interlock ---");
  Serial.println("1- Initialization of the Serial communication: OK");

  // //Start the Ethernet
  Ethernet.begin(mac, ip);
  //Start UDP
  Udp.begin(localPort);
  Serial.println("2- Initialization of the UDP communication:    OK");

  //Define the status LED for the various component.
  pinMode(led_env,OUTPUT);
  pinMode(led_gas,OUTPUT);
  pinMode(led_coo,OUTPUT);
  pinMode(led_LV,OUTPUT);
  pinMode(led_HV,OUTPUT);
  Serial.println("3- Definition of the status LED:               OK");

  //Define pin mode for the redundancy system.
  pinMode(redu_sig,OUTPUT);

  //Configure the pins of the interlock signals
  IL.configInterLockPIN();
  Serial.println("4- Definition of the interlock pins:           OK");

  //Cooling Flowmeter definition, (Flowmeter1)
  c.configuration();
  g.configuration();
  lv.configuration();
  Serial.println("5- Definition of the pinout:                   OK");

  //Initialization of Chipcap2 sensors.
  for(int i=0; i<N_Chipcap; i++){
    e.start_Chipcap(adr_Chipcap[i]);
  }
  Serial.println("6- Definition of the Chipcp sensors:           OK");

  // Measuring the current stata of the system

  for(int i=0; i < 32; i++){
      state_sys = work_condition(state_sys);
          
      env_sensors               = String("/");
      gas_sensors               = String("/");
      Cooling_flow_1_str        = String("/");
      LV_states                 = String("/");
      GEMROC_states             = String("/");
      
      FIFO_environment_buffer_state();
      FIFO_cooling_buffer_state();
      FIFO_gas_buffer_state();
      FIFO_LV_buffer_state();
      Serial.println(FIFO_LV_buffer_state());
      //delay(1000);
  }


  Serial.println("--- End of initialization ---");
  
  if(SUM == true){
    Serial.println("---Super User Mode ON---");
  }
  delay(2000);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void loop(){

  if(state_CNP==true){
    dt_CNP = millis()-time_CNP;
  } else{dt_CNP = 0;}
  

  if(Serial.available()>0){
    
    msg = Serial.readString();
    Serial.println(msg);
    delay(5000);
    if(msg.equals(CNP)){
      digitalWrite(pin_resetControl,LOW);
      delay(2000);
      Serial.println("");
      Serial.println("IT IS NOW POSSIBLE TO CHARGE A NEW PROGRAM or TO RESET THE SYSTEM");
      Serial.println("");
      time_CNP = millis();
      state_CNP = true;
    }
    if(dt_CNP > timeinterval_CNP && state_CNP==true){
      delay(2000);
      Serial.println("");
      Serial.println("REMOVED THE CNP!");
      Serial.println("");
      state_CNP==false;
    }

    if(msg.equals(reset)){

      Serial.println("Reset message recived!");
      Serial.println("Restarting the system...");
       
      delay(5000);
      resetFunc();
     }
    
    if(msg.compareTo(activateSUM)==0){

      SUM                  = true;
      SUM_gas_skip         = true;
      SUM_cooling_skip     = true;
      SUM_environment_skip = true;
      SUM_LV_skip          = true;
      SUM_HV_skip          = true;

      Serial.println("  - SUM ACTIVE!");
      delay(5000);
    }

    if(msg.compareTo(disableSUM)==0){

      SUM                  = false;
      SUM_gas_skip         = false;
      SUM_cooling_skip     = false;
      SUM_environment_skip = false;
      SUM_LV_skip          = false;
      SUM_HV_skip          = false;

      Serial.println("  - SUM DISABLED!");
      delay(5000);
    }
  if(msg.compareTo(activateTESTMODE)==0){
    TESTMODE = 1;
    Serial.println(" ");
    Serial.println("|||||||||  |||||||||  ||||||||| |||||||||    ||||  ||||   |||||||||  ||||||    |||||||||     |||||||||  |||    ||");
    Serial.println("   ||      ||         ||           ||        ||  ||   ||  ||     ||  ||    ||  ||            ||     ||  || ||  || ");
    Serial.println("   ||      ||||       |||||||||    ||        ||  ||   ||  ||     ||  ||    ||  ||||          ||     ||  ||   ||||");
    Serial.println("   ||      ||                ||    ||        ||  ||   ||  ||     ||  ||    ||  ||            ||     ||  ||     ||");
    Serial.println("   ||      |||||||||  |||||||||    ||        ||  ||   ||  |||||||||  ||||||||  |||||||||     |||||||||  ||     ||");
    Serial.println(" ");
    delay(1500);
                                                                                

    }
  if(msg.compareTo(disableTESTMODE)==0){
    TESTMODE = 0;
    Serial.println(" ");
    Serial.println("|||||||||  |||||||||  ||||||||| |||||||||    ||||  ||||   |||||||||  ||||||    |||||||||     |||||||||  |||||||||  |||||||||");
    Serial.println("   ||      ||         ||           ||        ||  ||   ||  ||     ||  ||    ||  ||            ||     ||  ||         ||       ");
    Serial.println("   ||      ||||       |||||||||    ||        ||  ||   ||  ||     ||  ||    ||  ||||          ||     ||  ||||       ||||");
    Serial.println("   ||      ||                ||    ||        ||  ||   ||  ||     ||  ||    ||  ||            ||     ||  ||         ||");
    Serial.println("   ||      |||||||||  |||||||||    ||        ||  ||   ||  |||||||||  ||||||||  |||||||||     |||||||||  ||         ||");
    Serial.println(" ");
    delay(1500);  
    }
  
  }

  digitalWrite(redu_sig,HIGH);                        //At the beginning of the loop the main send a rise-edge to the secondary, (The redudancy system look for a falling edge).
  state_CHECK = state_sys;                            //Save the previous state of the system
  
  state_sys = work_condition(state_sys);              //Determine the current State of the system.
  
  Serial.print("- Current state:     "); Serial.println(state_sys);
  Serial.print("- Preovious state:   "); Serial.println(state_CHECK);

  // byte adm_state_sys = 0;
  adm_state_sys = 0;
  adm_state_sys = max_admissible_state(state_sys);    //Get the max allowed state.

  Serial.print("- Max. Adm. state:   "); Serial.println(adm_state_sys);

  if(UDP_state == true){
    dt_UDP = millis() - time_UDP;

    if(state_CHECK > adm_state_sys){
      dt_UDP = UDP_time_interval + 1;
      UDP_transiotion = true;
      UDP_start_alarm_timer = true;
    }
    if(UDP_transiotion == true){                       
      if(UDP_start_alarm_timer == true){
        time_UDP_post_switchOFF = millis();
        UDP_start_alarm_timer = false;
      }
      dt_UDP_post_switchOFF = millis() - time_UDP_post_switchOFF;
      if(dt_UDP_post_switchOFF > UDP_time_post_alarm){
        time_UDP_post_switchOFF = millis();
        UDP_transiotion = false;
      }
      else{dt_UDP = UDP_time_interval + 1; }
    }

    if(dt_UDP > UDP_time_interval){

      String data_to_send = String("D/" + String(adm_state_sys)+ "/" + String(state_E) + env_sensors + String(state_C) + "/" + Cooling_flow_1_str + "/" + String(state_G) + gas_sensors + LV_states + GEMROC_states);
      send_UDP_message(data_to_send);
      Serial.println(data_to_send);

      time_UDP = millis();

    }

    env_sensors        = String("/");
    gas_sensors        = String("/");
    Cooling_flow_1_str = String("/");
    LV_states          = String("/");
    GEMROC_states      = String("/");

  }

    if(TESTMODE == 0){

    //----- State Machine -----
    
      if(adm_state_sys == state0){
        Serial.println("- DETECTOR STATE:    0");
        if(state_CHECK > adm_state_sys && state_CHECK != state1){
          IL.switch_OFF1(ramp_down,Start_Ramp_down);
          Start_Ramp_down = false;
        }
        else{
          IL.General_INTERLOCK(0);
          if(Start_Ramp_down == false){send_UDP_message("ALL IS OFF");}       // new 16/11/2023
          Start_Ramp_down = true;
          }
      }
      if(adm_state_sys == state1){     
        Serial.println("- DETECTOR STATE:    1");
        IL.interlock_MF_LV(1);
        IL.interlock_MF_HV(0);
        IL.interlock_board1_HV(0);
        Start_Ramp_down = true;
      }
      if(adm_state_sys == state2){
        Serial.println("- DETECTOR STATE:    3");
        //IL.General_INTERLOCK(1);
        IL.interlock_MF_LV(1);
        IL.interlock_MF_HV(1);
        IL.interlock_board1_HV(1);
        Start_Ramp_down = true;
      }
      if(adm_state_sys == state3){
        Serial.println("- DETECTOR STATE:    7!");   
        //IL.General_INTERLOCK(1);
        IL.interlock_MF_LV(1);
        IL.interlock_MF_HV(1);
        IL.interlock_board1_HV(1);
        Start_Ramp_down = true;
      }
  }
  
  if(TESTMODE ==1){
      IL.interlock_MF_LV(1);
      IL.interlock_MF_HV(1);
      IL.interlock_board1_HV(1);
      Serial.println("------------------------------------------------------------------------------------------------");
      Serial.println("  ");       
      Serial.println("|||||||||  |||||||||  ||||||||| |||||||||       ||||  ||||    |||||||||   ||||||     |||||||||");
      Serial.println("   ||      ||         ||           ||           ||  ||   ||   ||     ||   ||    ||   ||");
      Serial.println("   ||      ||||       |||||||||    ||           ||  ||   ||   ||     ||   ||    ||   ||||");
      Serial.println("   ||      ||                ||    ||           ||  ||   ||   ||     ||   ||    ||   ||");
      Serial.println("   ||      |||||||||  |||||||||    ||           ||  ||   ||   |||||||||   ||||||||   |||||||||");
      Serial.println("  ");
      Serial.println("------------------------------------------------------------------------------------------------");
      delay(500);
    }

  
  digitalWrite(redu_sig,LOW);                        //At the end of the loop we have a falling-edge.

  FIFO_environment_buffer_state();
  FIFO_cooling_buffer_state();
  FIFO_gas_buffer_state(); 
  FIFO_LV_buffer_state();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//--- Logic Functions.


//Function that determine which is the max admissible state.
int max_admissible_state(byte state_sys){
  int S=0;
  bool T = true;
  byte max_state = 0B00000000;
  while(T==true){
    if(bitRead(state_sys,S)==1){
      bitWrite(max_state,S,1);
      S++;
      }
    else{T=false;}
  }
  return(max_state);
}

//Detector State Function
//Function that determine the current state of the system.
int work_condition(byte state_sys){

    // state_E = state_environment();
    // state_G = state_gas();
    // state_C = state_cooling();

    state_E  = Services_average_state(environment_buffer_state,16,0.70) &  Services_state_transitions(environment_buffer_state,16,5);
    state_G  = Services_average_state(gas_buffer_state,16,0.70)         &  Services_state_transitions(gas_buffer_state,16,5);
    state_C  = Services_average_state(cooling_buffer_state,16,0.70)     &  Services_state_transitions(cooling_buffer_state,16,5);
    state_L  = Services_average_state(LV_buffer_state,16,0.70)           &  Services_state_transitions(LV_buffer_state,16,5);

    //Serial.print("-----LV STATE: ");Serial.println(state_LV());
    if((state_E && state_G && state_C)==1){bitWrite(state_sys,0,1);}
    else{bitWrite(state_sys,0,0);}
    if(state_L==1){bitWrite(state_sys,1,1);}
    else{bitWrite(state_sys,1,0);}
    if(state_HV()==1){bitWrite(state_sys,2,1);}
    else{bitWrite(state_sys,2,0);}

    return(state_sys);
}

int FIFO_environment_buffer_state(){
    // Serial.println("----- TEST FIFO -----");
    // Serial.print("Before the shift"); Serial.println(environment_buffer_state);
    environment_buffer_state = environment_buffer_state << 1;
    bitWrite(environment_buffer_state,0,state_environment());
    // Serial.print("After the shift"); Serial.println(environment_buffer_state);
    // printOut1(environment_buffer_state);
}
int FIFO_gas_buffer_state(){
    gas_buffer_state = gas_buffer_state << 1;
    bitWrite(gas_buffer_state,0,state_gas());
}
int FIFO_cooling_buffer_state(){
    cooling_buffer_state = cooling_buffer_state << 1;
    bitWrite(cooling_buffer_state,0,state_cooling());
}
int FIFO_LV_buffer_state(){
    LV_buffer_state = LV_buffer_state << 1;
    bitWrite(LV_buffer_state,0,state_LV());
}

//Buffer, byte sequence to analyze; N_monitored_measure, number of examinated measures; th, min average value of ones required.
int Services_average_state(unsigned int buffer, int N_monitored_measures, float th){
    // Serial.print("Buffer examinated:  "); Serial.println(buffer);
    
    float average_ones = 0;
    float n_ones       = 0;

    for(int bits = 0; bits <= (N_monitored_measures - 1); bits++){
      // Serial.print(bits); Serial.print("   "); Serial.println(buffer & (1 << bits));
      if(buffer & (1 << bits)){
        n_ones = n_ones + 1;
      }
    }
    average_ones = n_ones/(float)N_monitored_measures;
    
    // Serial.print("Average number of ones :       "); Serial.println(average_ones);

    if(average_ones > th){return(1);}
    else{return(0);}
}

//Buffer, byte sequence to analyze; N_monitored_measure, number of examinated measures; th, max transitions allowed.
int Services_state_transitions(unsigned int buffer, int N_monitored_measures, float th){

    int num_transitions = 0;

    for(int bits = 0; bits <= (N_monitored_measures - 1); bits++){
      if(bits != (N_monitored_measures - 1)){
        // Serial.print("evaluation of transition  "); Serial.print(bits); Serial.print("   ");
        // Serial.println((bitRead(buffer,bits) & ~bitRead(buffer,bits+1)) + (bitRead(buffer,bits+1) & ~bitRead(buffer,bits)));
        num_transitions = num_transitions + (bitRead(buffer,bits) & ~bitRead(buffer,bits+1)) + (bitRead(buffer,bits+1) & ~bitRead(buffer,bits));
      }
    }
  
    // Serial.print("Number of transitions        : "); Serial.println(num_transitions);

    if(num_transitions > th){return(0);}
    else{return(1);}
}

void printOut1(int c) {
    for (int bits = 15; bits > -1; bits--) {
      // Compare bits 7-0 in byte
      if (c & (1 << bits)) {
        Serial.print("1");
      }
      else {
        Serial.print("0");
      }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

// TO DO: CHANGE THE NAME OF THE VARIABLES!!!!!!! (EX: state_environment)

//////////////////////////////////////////////////////////////////////////////////////////////////////
//---Functions that gets the ENVIRONMENT state and it sends the data to the laptop (if UDP_state == true).
int state_environment(){

  if(SUM == true && SUM_environment_skip == true){
    Serial.println("---Super User Mode ON---");
    Serial.println("The environment state is imposed to 1");
    LED_state_environment = !LED_state_environment;
    digitalWrite(led_env,LED_state_environment);
    return(1);
  }
  else{

    int state_environment = 0;    //Variable that is used in the code to determine the state.
    int single_states = 0;        //Variable that is used to see if all the Chipcap2 are reading correct values.

    //String to send to the PC.
    //String env_sensors = String("E");

    Serial.println("--- ENVIRONMENT ---");

    for(int i=0; i<N_Chipcap; i++){

      single_states = single_states + e.State(adr_Chipcap[i],i);

      //String env_measure = String(adr_Chipcap[i]);
      //env_temp = env_temp + "/" + e.temperature;
      //env_humi = env_humi + "/" + e.humidity;
      env_sensors = env_sensors + e.temperature + "/" + e.humidity + "/";
      //env_sensors = String(e.temperature) + "/" + String(e.humidity);
    }

    Serial.print("  - ENV. sen. within the range: "); Serial.println (single_states);

    if(single_states == N_ENVIRONMENT_SENSORS){
      state_environment = 1;
      digitalWrite(led_env,HIGH);
    }
    else{
      if(UDP_state == true && state_CHECK > adm_state_sys ){
        Serial.println("ENVIRONMENT ALARM!!!!!!!!!!!!!!!!!!");
        Udp.beginPacket(remoteip,remotePort);
        Udp.print("AE/" + env_sensors);
        Udp.endPacket();
      }
      digitalWrite(led_env,LOW);
    }
    Serial.print("  - ENV. status single measure: "); Serial.println(state_environment);
    return(state_environment);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Function that gets the GAS state and it sends the data to the laptop (if UDP_state == true).
int state_gas(){

  if(SUM == true && SUM_gas_skip == true){
    Serial.println("---Super User Mode ON---");
    Serial.println("The gas state is imposed to 1");
    LED_state_gas = !LED_state_gas;
    digitalWrite(led_gas,LED_state_gas);
    return(1);
  }
  else{
    int gas_state    = 0;                  //Variable that is used in the code to determine the state of the gas service
    int single_states = 0;              //Variable that is used to verify if all the BME280 are reading values within the range.

    Serial.println("--- GAS ---");
    for(int i=0; i<N_BME280; i++){
      
      //single_states = single_states + g.State_THP_I2C(adr_BME280[i], i);    // NEEDED IF WE WANT ALSO THIS CONSTRAIN!!!
      g.State_THP_I2C(adr_BME280[i], i);
      String t_str = String(g.temperature);
      String h_str = String(g.humidity);
      String p_str = String(g.pressure);

      gas_sensors = gas_sensors + t_str + "/" + h_str + "/" + p_str + "/";
    }

    single_states = single_states + g.State_gas_flux_Ar() + g.State_gas_flux_ISO();     // Variable to check the state
    gas_sensors   = gas_sensors + g.gas_flux_Ar + "/" + g.gas_flux_ISO;                      //String to send

    Serial.print("  - GAS sen. within the range: "); Serial.println (single_states);

    Serial.print("  - Ar flow status 1         : "); Serial.println (g.State_gas_flux_Ar());
    Serial.print("  - Ar flow 1                : "); Serial.println (g.gas_flux_Ar);
    
    Serial.print("  - ISO flow status 2        : "); Serial.println (g.State_gas_flux_ISO());
    Serial.print("  - ISO flow 2               : "); Serial.println (g.gas_flux_ISO);

    if(single_states == N_gas_flowmeter){
      gas_state = 1;
      digitalWrite(led_gas,HIGH);
    }
    else{
      if(UDP_state == true && state_CHECK > adm_state_sys ){
        Udp.beginPacket(remoteip,remotePort);
        Udp.print("AG/" + gas_sensors);
        Udp.endPacket();
        }
      digitalWrite(led_gas,LOW);
    }

    Serial.print("  - GAS status single measure: "); Serial.println(gas_state);
    return (gas_state);
  }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Functions that gets the cooling states
int state_cooling(){

  if(SUM == true && SUM_cooling_skip == true){
    Serial.println("---Super User Mode ON---");
    Serial.println("The cooling state is imposed to 1");
    LED_state_cooling = !LED_state_cooling;
    digitalWrite(led_coo,LED_state_cooling);
    return(1);
  }
  else{

    Serial.println("--- COOLING ---");
    
    Cooling_flow_1     = c.cooling_measure_flux();
    Cooling_flow_1_str = String(Cooling_flow_1);
    
    if(Cooling_flow_1 < cooling_flow_threshold){
      Serial.println("  - COOLING status  : 0"); 
      if(UDP_state == true && state_CHECK > adm_state_sys ){
        Udp.beginPacket(remoteip,remotePort);
        Udp.print("AC/" + Cooling_flow_1_str);                            
        Udp.endPacket();
      }
      digitalWrite(led_coo,LOW);
      N_coo_alarm_messages++;                                                       
      return(0);}
    else{
      Serial.println("  - COOLING status  : 1"); 
      digitalWrite(led_coo,HIGH);
      N_coo_alarm_messages = 1;                                                       
      return(1);
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int state_UPS(){
  return(1);}


int state_LV(){

  Serial.println("--- LOW VOLTAGE ---");

  if(SUM == true && SUM_LV_skip == true){
    Serial.println("---Super User Mode ON---");
    Serial.println("The LV state is imposed to 1");
    LED_state_cooling = !LED_state_cooling;
    digitalWrite(led_coo,LED_state_cooling);
    return(1);
  }
  else{
    
    if(lv.State() == 1 ){
      digitalWrite(led_LV,HIGH);
    }else{
      digitalWrite(led_LV,LOW);
    }

    
    LV_states      = "/" + String(lv.State_GEMROC_LY1) + "/" + String(lv.State_GEMROC_LY2) + "/" + String(lv.State_GEMROC_LY3);
    GEMROC_states  = "/" + String(lv.State_LY1_GEMROC_01) + "/" + String(lv.State_LY1_GEMROC_23) + "/" + String(lv.State_LY2_GEMROC_45) + "/" + String(lv.State_LY2_GEMROC_67) + "/" + String(lv.State_LY2_GEMROC_89) + "/"
    + String(lv.State_LY2_GEMROC_1011) + "/" + String(lv.State_LY3_GEMROC_1213) + "/" + String(lv.State_LY3_GEMROC_1415) + "/"+ String(lv.State_LY3_GEMROC_1617) + "/"+ String(lv.State_LY3_GEMROC_1819) + "/" + String(lv.State_LY3_GEMROC_2021);
    return(lv.State());
  }
}


int state_HV(){
  return(0);}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//Interrupts functions for the flowmeter YS-401F!
//Counter for the cooling flowmeter, (flowmeter1)
// void pulse_1(){
//   count++;
// }

// //Flowmeter1, (cooling).
// float flow_1(){
//   dt = millis()-t1;  //we cannot use millis in the final code!!(not sure)
//   if(dt>2000){
//     //freq = (float)count /0.5f;
//     freq = (float)count/2.00f;
//     //Serial.print(count);
//     //flux = (freq*1000.0f)/(98.0f*60.0f);
//     flux = (freq)/(98.0f);
//     if(sp_coolingFlow == true){
//       Serial.print("  - IN-flux :"); Serial.println(Cooling_flow_1);
//       /*Serial.println("---------------------");
//       Serial.print("The cooling flux is: ");
//       Serial.print(flux); Serial.println(" L/min");
//       Serial.print("The frequecy flowmeter 1 is: ");
//       Serial.print(freq); Serial.println(" Hz");
//       Serial.println("---------------------");*/
//     }
//     count = 0;
//     t1 = millis();
//   }
//   return(flux);
// }

void send_UDP_message(String message){
  Udp.beginPacket(remoteip,remotePort);
  Udp.print(message);
  Udp.endPacket();
}

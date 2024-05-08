/*
 * Functions to configure, read and get the state of the ChipCap2;
 * Here there is the state function with a sort of band-gap to avoid oscillation of the output.
 * - programmable theshold
*/
#ifndef environment_h
#define environment_h

#include <Wire.h>
#include <CFF_ChipCap2.h>

#define N_ENVIRONMENT_SENSORS 6
#define N_Chipcap 6

// int adr_Chipcap[] = {0x22,0x28};        //Array with the I2C address of each sensor.
int adr_Chipcap[] = {0x22,0x24,0x26,0x28,0x30,0x32};        //Array with the I2C address of each sensor.

class environment{

byte hysteresis_env = 0B00000011;     // - byte with information on the cross of the theshold (GAs).

  private:
    // //Top-limit of the Upper band.
    // float th_T[N_Chipcap] = {29,29};
    // float th_H[N_Chipcap] = {80,70};
    // //Bottom-limit of the Upper band.
    // float th_BT[N_Chipcap] = {27,27};
    // float th_BH[N_Chipcap] = {60,60};
    // //Botton-limit of Lower band.
    // float th_LT[N_Chipcap] = {5,5};
    // float th_LH[N_Chipcap] = {-1,-1};            // (may be is not useful for the humidity)
    // //Top-limit of the Lower band.
    // float th_TLT[N_Chipcap] = {7,7};
    // float th_TLH[N_Chipcap] = {0,0};    

    //Top-limit of the Upper band.
    float th_T[N_Chipcap] = {28,28,28,28,28,28};
    float th_H[N_Chipcap] = {50,50,50,50,50,50};
    //Bottom-limit of the Upper band.
    float th_BT[N_Chipcap] = {26,26,26,26,26,26};
    float th_BH[N_Chipcap] = {45,45,45,45,45,45};
    //Botton-limit of Lower band.
    float th_LT[N_Chipcap] = {5,5,5,5,5,5};
    float th_LH[N_Chipcap] = {-1,-1,-1,-1,-1,-1};            // (may be is not useful for the humidity)
    //Top-limit of the Lower band.
    float th_TLT[N_Chipcap] = {7,7,7,7,7,7};
    float th_TLH[N_Chipcap] = {0,0,0,0,0,0};    
    

    //1 - to Serial print the measure, 0 - to disable the serial pirnt.
    int sp = 1;

  public:
    float temperature;
    float humidity;

    void start_Chipcap(int address);
    void readSensor(int address);
    //int State(int address);
    int State(int address, int pos);    
};

void environment::start_Chipcap(int address){
  CFF_ChipCap2(address).begin();
  Wire.setClock(50000);
  CFF_ChipCap2(address).startCommandMode();
  CFF_ChipCap2(address).startNormalMode();
  Serial.print("   - Configuration of ");Serial.print(address); Serial.println(" done");
  delay(200);
}

void environment::readSensor(int address){
  CFF_ChipCap2 cc2 = CFF_ChipCap2(address);
  cc2.begin();
  Wire.setClock(50000);
  cc2.startNormalMode();
  cc2.readSensor();
  temperature = cc2.temperatureC;
  humidity = cc2.humidity;
}

//Function with the band to get the state from each chipcap2, (new initialization procedure).
int environment::State(int address, int pos){

  CFF_ChipCap2(address).readSensor();

  temperature = CFF_ChipCap2(address).temperatureC;
  humidity = CFF_ChipCap2(address).humidity;

  if(sp){
    Serial.print("  - Env. Temperature: "); Serial.println(temperature);
    Serial.print("  - Env. Humidity:    "); Serial.println(humidity);
  }
  
  //Serial.print("V from .ino  ");Serial.print(bitRead(hysteresis_env,pos)); Serial.print("  "); Serial.println(pos);

  if(( temperature > th_T[pos] || temperature < th_LT[pos] || humidity > th_H[pos] || humidity < th_LH[pos] ) == 1){
    bitWrite(hysteresis_env, pos, 0);
    return(0);
  }
  else if (( (temperature > th_BT[pos] || temperature < th_TLT[pos] || humidity > th_BH[pos] || humidity < th_TLH[pos]) && bitRead(hysteresis_env,pos) == 0) == 1){
    bitWrite(hysteresis_env, pos, 0);
    return(0);
  } 
  else if((temperature < th_BT[pos] && temperature > th_TLT[pos] && humidity < th_BH[pos] && humidity > th_TLH[pos] && bitRead(hysteresis_env,pos) == 0) == 1){
    bitWrite(hysteresis_env, pos, 1);
    return(1);
  }
  else if ((temperature < th_T[pos] && temperature > th_LT[pos] && humidity < th_H[pos] && humidity > th_LH[pos] && bitRead(hysteresis_env,pos) == 1) == 1){
    return(1);
  }

}


/*
//New initialization procedure, (no band)
int environment::State(int address){

  CFF_ChipCap2(address).readSensor();

  temperature = CFF_ChipCap2(address).temperatureC;
  humidity = CFF_ChipCap2(address).humidity;

  Serial.print("-Env. Temperature: "); Serial.println(temperature);
  Serial.print("-Env. Humidity:    "); Serial.println(humidity);

  //return (temperature, humidity);
  if(temperature < th_T && humidity < th_H){return(1);}
  else{Serial.println("Something is wrong with the environment!!!");
       return(0);}
}
*/

/*
//Old initialization prodecure.
int environment::State(int address){
  CFF_ChipCap2 cc2 = CFF_ChipCap2(address);
  cc2.begin();
  Wire.setClock(50000);
  cc2.startNormalMode();
  cc2.readSensor();

  temperature = cc2.temperatureC;
  humidity = cc2.humidity;

  Serial.print("-Env. Temperature: "); Serial.println(temperature);
  Serial.print("-Env. Humidity:    "); Serial.println(humidity);

  //return (temperature, humidity);
  if(temperature < th_T && humidity < th_H){return(1);}
  else{Serial.println("Something is wrong with the environment!!!");
       return(0);}

}
*/

#endif

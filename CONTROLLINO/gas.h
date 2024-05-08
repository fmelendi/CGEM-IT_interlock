/*
 * Functions to configure, read and get the state of the BME280; There are two options for the 
 * communication protocol, SPI and I2C are already inmplemented.
 * Here there is the state function with a sort of band-gap to avoid oscillation of the output. 
*/
#ifndef gas_h
#define gas_h


//Pin for BME280 comunication (trougth SPI)
// #define BME_SCK 5
// #define BME_MISO 3
// #define BME_MOSI 7
//#define BME_CS 


//Library and parameters for the BME280.
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

bool sp_gasFlow = true;                 //Variable to activate the Serial comminication.

#define N_GAS_SENSORS   3
#define N_BME280        1
#define N_gas_flowmeter 2

int adr_BME280[]          = {0x77};
int pin_BME280[]          = {8,8};
int pin_gas_flowmeter_Ar  = CONTROLLINO_A7;
int pin_gas_flowmeter_ISO = CONTROLLINO_A9;

class gas{
byte hysteresis_gas = 0B00000011;     //Byte with the information on the crossing of the theshold (GAS).

  private:
    
    //Top-limit of the Higher band.
    // float th_T = 33;
    // float th_H = 80;
    // float th_P = 1200.00;
    float th_T[N_BME280] = {33};
    float th_H[N_BME280] = {80};
    float th_P[N_BME280] = {1200.00};

    //Bottom-limit of the Higher band.
    // float th_BT = 27;
    // float th_BH = 70;
    // float th_BP = 1100.00;
    float th_BT[N_BME280] = {27};
    float th_BH[N_BME280] = {70};
    float th_BP[N_BME280] = {1100.00};

    //Botton-limit of Lower band.
    // float th_LT = 5;
    // float th_LH = -3;            
    // float th_LP = 700;
    float th_LT[N_BME280] = {5};
    float th_LH[N_BME280] = {-3};            
    float th_LP[N_BME280] = {700};

    //Top-limit of the Lower band.
    // float th_TLT = 7;
    // float th_TLH = 0;
    // float th_TLP = 800;
    float th_TLT[N_BME280] = {7};
    float th_TLH[N_BME280] = {0};
    float th_TLP[N_BME280] = {800};

    float th_Ar             = 1.55;
    float th_ISO            = 0.27;

    int N_flux_gas_measure = 10;
    //1 - to Serial print the measure, 0 - to disable the serial pirnt.
    int sp = 1;

    float flux_sm      = 0.00;
    float flux_sm_Ar   = 0.00;
    float flux_sm_ISO  = 0.00;
    float ref_voltage  = 5.0;
    float N_digits     = 1023.0;

  public:
    void start_SPI(int BME_CS);
    void readSensor_SPI(int BME_CS);
    void readSensor_I2C(int BME_address);
    void read_TempSensorS();
    int State_SPI(int BME_CS, int pos);          //State functions with the band to avoid oscillation.

    void configuration();
    int State_THP_I2C(int BME_address, int pos);
    int State_gas_flux_Ar();
    int State_gas_flux_ISO();
    int State_gas_flux(int pin_to_read, float th_G);

    float gas_flux, gas_flux_Ar, gas_flux_ISO;
    float temperature;
    float pressure;
    float humidity;
};

// void gas::start_SPI(int BME_CS){
//   Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);
//   bme.begin();
// }

// void gas::readSensor_SPI(int BME_CS){
//   Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);
//   bme.begin();
//   temperature = bme.readTemperature();
//   humidity = bme.readHumidity();
//   pressure = bme.readPressure()/100.00F;
// }

void gas::readSensor_I2C(int BME_address){
  Adafruit_BME280 bme;
  bme.begin(BME_address);

  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure()/100.00F;
}

/*
//Function without the band
int gas::State_I2C(int BME_address){
  
  Adafruit_BME280 bme;
  bme.begin(BME_address);

  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure()/100.00F;
  
  Serial.print("-Gas Temperature: "); Serial.println(temperature);
  Serial.print("-Gas Humidity:    "); Serial.println(humidity);
  Serial.print("-Gas Pressure:    "); Serial.println(pressure);

  if(temperature<th_T && humidity < th_H && pressure < th_P){
    return(1);
    }
  else{Serial.println("Something is wrong with the gas!!!");
       return(0);}
}

// Function without band!
int gas::State_SPI(int BME_CS){

  Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);
  bme.begin();

  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure()/100.00F;

  Serial.print("-Gas Temperature: "); Serial.println(temperature);
  Serial.print("-Gas Humidity:    "); Serial.println(humidity);
  Serial.print("-Gas Pressure:    "); Serial.println(pressure);

  if(temperature < th_T && humidity < th_H && pressure < th_P){
    return(1);
    }
  else{Serial.println("Something is wrong with the gas!!!");
       return(0);}
}
*/
void gas::configuration(){
  pinMode(pin_gas_flowmeter_Ar, INPUT);
  pinMode(pin_gas_flowmeter_ISO, INPUT);
}

int gas::State_gas_flux(int pin_to_read, float th_G){
    flux_sm = 0.00;
    for(int i = 0;i < N_flux_gas_measure; i++){
        flux_sm = flux_sm + ((analogRead(pin_to_read)/N_digits)*ref_voltage);
        }
      gas_flux = flux_sm/N_flux_gas_measure; 
      if(sp == 1){
        Serial.print("  - gas flux measure : ");
        Serial.println(gas_flux);
      }
      if(gas_flux < th_G){return(0);}
      else{return(1);}
}


int gas::State_gas_flux_ISO(){
    flux_sm_ISO = 0.00;
    for(int i = 0;i < N_flux_gas_measure; i++){
      flux_sm_ISO = flux_sm_ISO + ((analogRead(pin_gas_flowmeter_ISO)/N_digits)*ref_voltage);
      }
    gas_flux_ISO = flux_sm_ISO/N_flux_gas_measure; 
    if(sp == 1){
      // Serial.print("  - ISO flux measure : ");
      // Serial.println(gas_flux_ISO);
    }
    if(gas_flux_ISO < th_ISO){return(0);}
    else{return(1);}
}

int gas::State_gas_flux_Ar(){
    flux_sm_Ar = 0.00;
    for(int i = 0;i < N_flux_gas_measure; i++){
      flux_sm_Ar = flux_sm_Ar + ((analogRead(pin_gas_flowmeter_Ar)/N_digits)*ref_voltage);
      }
    gas_flux_Ar = flux_sm_Ar/N_flux_gas_measure; 
    if(sp == 1){
      // Serial.print("  - Ar flux measure : ");
      // Serial.println(gas_flux_Ar);
    }
    if(gas_flux_Ar < th_Ar){return(0);}
    else{return(1);}
}

// I2C - Function with the band to get the state from each BME280. 
// NB The variable "pos" is used to identify the sensor.

int gas::State_THP_I2C(int BME_address, int pos){
  
  Adafruit_BME280 bme;
  bme.begin(BME_address);

  Wire.setClock(50000);             //Useful to have a good clock signal on long distances.

  temperature = bme.readTemperature();
  humidity    = bme.readHumidity();
  pressure    = bme.readPressure()/100.00F;
  
  if(sp){
    Serial.print("  - Gas Temperature: "); Serial.println(temperature);
    Serial.print("  - Gas Humidity:    "); Serial.println(humidity);
    Serial.print("  - Gas Pressure:    "); Serial.println(pressure);
  }

  if(temperature > th_T[pos] || temperature < th_LT[pos] || humidity > th_H[pos] || humidity < th_LH[pos] || pressure > th_P[pos] || pressure < th_LP[pos]){
    bitWrite(hysteresis_gas, pos, 0);
    return(0);
  }
  else if((temperature > th_BT[pos] || temperature < th_TLT[pos] || humidity > th_BH[pos] || humidity < th_TLH[pos] || pressure > th_BP[pos] || pressure < th_TLP[pos]) && bitRead(hysteresis_gas,pos) == 0){
    bitWrite(hysteresis_gas, pos, 0);
    return(0);
  }
  else if(temperature < th_BT[pos] && temperature > th_TLT[pos] && humidity < th_BH[pos] && humidity > th_TLH[pos] && pressure < th_BP[pos] && pressure > th_TLP[pos] && bitRead(hysteresis_gas,pos) == 0){
    bitWrite(hysteresis_gas, pos, 1);
    return(1);
  }
  else if (temperature < th_T[pos] && humidity < th_H[pos] && pressure < th_P[pos] && bitRead(hysteresis_gas,pos) == 1){
    return(1);
  }
}

// SPI - Function with the band to get the state from each BME280 .

// int gas::State_SPI(int BME_CS, int pos){

//   Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);
//   bme.begin();

//   temperature = bme.readTemperature();
//   humidity = bme.readHumidity();
//   pressure = bme.readPressure()/100.00F;

//   if(sp){
//     Serial.print("-Gas Temperature: "); Serial.println(temperature);
//     Serial.print("-Gas Humidity:    "); Serial.println(humidity);
//     Serial.print("-Gas Pressure:    "); Serial.println(pressure);
//   }
//   //Serial.print(bitRead(hysteresis,pos)); Serial.print("  "); Serial.print(pos);

//   if(temperature > th_T[pos] || humidity > th_H[pos] || pressure > th_P[pos]){
//     bitWrite(hysteresis_gas, pos, 0);
//     return(0);
//   }
//   else if ((temperature > th_BT[pos] || humidity > th_BH[pos] || pressure > th_BP[pos]) && bitRead(hysteresis_gas,pos) == 0){
//     bitWrite(hysteresis_gas, pos, 0);
//     return(0);
//   }
//   else if((temperature < th_BT[pos] && humidity < th_BH[pos] &&  pressure < th_BP[pos]) && bitRead(hysteresis_gas,pos) == 0){
//     bitWrite(hysteresis_gas, pos, 1);
//     return(1);
//   }
//   else if (temperature < th_T[pos] && humidity < th_H[pos] &&  pressure < th_P[pos] && bitRead(hysteresis_gas,pos) == 1){
//     return(1);
//   }
// }
#endif

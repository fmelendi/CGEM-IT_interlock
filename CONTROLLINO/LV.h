#ifndef LV_h
#define LV_h

// //Pin to identify the GEMROC state.

// #define pin_state_LV_1  4                 
// #define pin_state_LV_2  6                 

#define LY1_pin_GEMROC_01    46
#define LY1_pin_GEMROC_23    47

#define LY2_pin_GEMROC_45    9
#define LY2_pin_GEMROC_67    10
#define LY2_pin_GEMROC_89    11
#define LY2_pin_GEMROC_1011  12

#define LY3_pin_GEMROC_1213  13
#define LY3_pin_GEMROC_1415  42
#define LY3_pin_GEMROC_1617  43
#define LY3_pin_GEMROC_1819  44
#define LY3_pin_GEMROC_2021  45




class LV{
  private:

    bool sp = 1;



  public:

    int State_LY1_GEMROC_01;
    int State_LY1_GEMROC_23;

    int State_LY2_GEMROC_45;
    int State_LY2_GEMROC_67;
    int State_LY2_GEMROC_89;
    int State_LY2_GEMROC_1011;

    int State_LY3_GEMROC_1213;
    int State_LY3_GEMROC_1415;
    int State_LY3_GEMROC_1617;
    int State_LY3_GEMROC_1819;
    int State_LY3_GEMROC_2021;    

    int State();
    void configuration();
    //int Alarm(int alarm);  
     
    
    bool State_GEMROC_LY1;
    bool State_GEMROC_LY2;
    bool State_GEMROC_LY3;
};

void LV::configuration(){

      pinMode(LY1_pin_GEMROC_01,INPUT);
      pinMode(LY1_pin_GEMROC_23,INPUT);

      pinMode(LY2_pin_GEMROC_45,INPUT);
      pinMode(LY2_pin_GEMROC_67,INPUT);
      pinMode(LY2_pin_GEMROC_89,INPUT);
      pinMode(LY2_pin_GEMROC_1011,INPUT);

      pinMode(LY3_pin_GEMROC_1213,INPUT);
      pinMode(LY3_pin_GEMROC_1415,INPUT);
      pinMode(LY3_pin_GEMROC_1617,INPUT);
      pinMode(LY3_pin_GEMROC_1819,INPUT);
      pinMode(LY3_pin_GEMROC_2021,INPUT);
}


int LV::State(){
    
    State_LY1_GEMROC_01     = !digitalRead(LY1_pin_GEMROC_01);
    State_LY1_GEMROC_23     = !digitalRead(LY1_pin_GEMROC_23);

    Serial.print("GEMROC  01                        :");  Serial.println(State_LY1_GEMROC_01);
    Serial.print("GEMROC  23                        :");  Serial.println(State_LY1_GEMROC_23);

    State_LY2_GEMROC_45     = !digitalRead(LY2_pin_GEMROC_45);
    State_LY2_GEMROC_67     = !digitalRead(LY2_pin_GEMROC_67);
    //State_LY2_GEMROC_67     = 1;                                          // memo 67 a 1!
    State_LY2_GEMROC_89     = !digitalRead(LY2_pin_GEMROC_89);
    State_LY2_GEMROC_1011   = !digitalRead(LY2_pin_GEMROC_1011);

    Serial.print("GEMROC  45                        :");  Serial.println(State_LY2_GEMROC_45);
    Serial.print("GEMROC  67                        :");  Serial.println(State_LY2_GEMROC_67);
    Serial.print("GEMROC  89                        :");  Serial.println(State_LY2_GEMROC_89);
    Serial.print("GEMROC  1011                        :");  Serial.println(State_LY2_GEMROC_1011);

    State_LY3_GEMROC_1213   = !digitalRead(LY3_pin_GEMROC_1213);
    State_LY3_GEMROC_1415   = !digitalRead(LY3_pin_GEMROC_1415);
    State_LY3_GEMROC_1617   = !digitalRead(LY3_pin_GEMROC_1617);
    State_LY3_GEMROC_1819   = !digitalRead(LY3_pin_GEMROC_1819);
    //State_LY3_GEMROC_1819   = 1;                                          // memo 1819 a 1!
    State_LY3_GEMROC_2021   = !digitalRead(LY3_pin_GEMROC_2021);

    Serial.print("GEMROC  1213                        :");  Serial.println(State_LY3_GEMROC_1213);
    Serial.print("GEMROC  1415                        :");  Serial.println(State_LY3_GEMROC_1415);
    Serial.print("GEMROC  1617                        :");  Serial.println(State_LY3_GEMROC_1617);
    Serial.print("GEMROC  1819                        :");  Serial.println(State_LY3_GEMROC_1819);
    Serial.print("GEMROC  2021                        :");  Serial.println(State_LY3_GEMROC_2021);
    

    State_GEMROC_LY1 = State_LY1_GEMROC_01 && State_LY1_GEMROC_23;
    State_GEMROC_LY2 = State_LY2_GEMROC_45 && State_LY2_GEMROC_67 && State_LY2_GEMROC_89 && State_LY2_GEMROC_1011;
    State_GEMROC_LY3 = State_LY3_GEMROC_1213 && State_LY3_GEMROC_1415 && State_LY3_GEMROC_1617 && State_LY3_GEMROC_1819 && State_LY3_GEMROC_2021;
  
    Serial.print("  -  GEMROC LY1:         ");Serial.println(State_GEMROC_LY1);
    Serial.print("  -  GEMROC LY2:         ");Serial.println(State_GEMROC_LY2);
    Serial.print("  -  GEMROC LY3:         ");Serial.println(State_GEMROC_LY3);
    //delay(5000);
    if(State_GEMROC_LY1 && State_GEMROC_LY2 && State_GEMROC_LY3 == 1){return(1);}
    else{return(0);}
}


// int LV::Alarm(int alarm){
//   if(digitalRead(alarm)==1){return(1);}
//   else{return(0);}
// }

#endif

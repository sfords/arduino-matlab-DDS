

//SWEEP FUNCTIONS AND VARIABLES
//byte locations on PORTA connected to DDS chip
#define W_CLK 0 //PORTA pin 22
#define FQ_UD 1 //pin 23
#define DATA 2 //pin24
#define RESET 3 //pin25
#define PORTOUT PORTA //set above pins high, or low in form of bytedata
#define PORTIO DDRA //configure portA in setup


typedef enum {NONE, MINF, MAXF , FS, TM, RUN, DONE} modes;
modes mode = NONE;

//sweep variables
int32_t minFreq = 10000;
int32_t maxFreq = 50000;
int32_t currentF = minFreq;
int fStep = 10 ; 
int Tmulti = 1;
int i=1;

//pulses a pin high, then low
#define pulseHigh(pin){PORTOUT |= 1<<pin; PORTOUT &= ~(1<<pin);}

// transfers a byte, a bit at a time, LSB first to the 9850 via serial DATA line
void tfr_byte(byte data)
{
  for (int i=0; i<8; i++, data>>=1) {
    PORTOUT = (data&0x01)<<DATA; // transfers bitdata to the DATA pin byte location on portD, effectively digitalWrite()
    pulseHigh(W_CLK);   //after each bit sent, CLK is pulsed high
  }
}

 // frequency calc from datasheet page 8 = <sys clock> * <frequency tuning word>/2^32
void sendFrequency(double frequency) {
  int32_t freq = frequency * 4294967295/125000000;  // note 125 MHz clock on 9850
  
  for (int b=0; b<4; b++, freq>>=8) {
    tfr_byte(freq & 0xFF);
  }
  
  tfr_byte(0x00);   // Final control byte, all 0 for 9850 chip
  pulseHigh(FQ_UD);  // Done!  Should see output
}

void runsweep () {
  currentF = minFreq;
  Serial.println("sweeping...");
  while(currentF<=maxFreq){ 
    sendFrequency(currentF); //update dds frequency
    if (i==Tmulti) { 
      currentF += fStep; //increase frequency
      i=1;
    }
    else{i++;}
  }
  mode = NONE;
  Serial.println("sweep done");
}

//MATLAB INTERFACE 
long int currentVal = 0;

void handshake() { //make sure matlab and arduino are communicating
     while (Serial.available() <= 0) {
       Serial.println('Z');   // send a capital Z
       delay(1000);
     }
}

void passVal(){ //change sweep variable, dependent on processed data from matlab
  switch (mode){
    case MINF:
      minFreq=currentVal;
      Serial.print("min freq = "); Serial.println(minFreq);
      break;
    case MAXF:
      maxFreq=currentVal;
      Serial.print("max freq = "); Serial.println(maxFreq);
      break;
    case FS:
      fStep=currentVal;
      Serial.print("fStep = "); Serial.println(fStep);
      break;
    case TM:
      Tmulti=currentVal;
      Serial.print("Tmulti = "); Serial.println(Tmulti);
      break;
  }  
}

//data in is in form 'X123456789'
void processByte (const char c){
  if (isdigit(c)){ //if bytein is digit, add to buffer value and shift left
    currentVal *= 10;
    currentVal += c - '0';
  }
  else{ //otherwise, look for mode characters and newline to finalize user input
    switch (c){
      case '\n':
        passVal();
        //Serial.print("currentVal = ");Serial.println(currentVal);
        //Serial.print("mode = "); Serial.println(mode);
        currentVal=0;
        break;
      case 'A':
        mode = MINF;
        break;
      case 'B':
        mode = MAXF;
        break;
      case 'C':
        mode = FS;
        break;
      case 'D':
        mode = TM;
        break;
      case 'R':
        mode = RUN;
        break;
    }
  }
}

//ARDUINO SETUP AND LOOP
void setup() {
  // configure arduino data pins for output
  PORTIO = B00001111; //MSB->LSB; PB1=pin2=w_clk, PB2=pin3=fq_ud, PB2=pin12=data, PB3=pin13=reset
  
  pulseHigh(RESET);
  pulseHigh(W_CLK);
  pulseHigh(FQ_UD);  // this pulse enables serial mode - Datasheet page 12 figure 10
  
  Serial.begin(9600);
  handshake(); 
}

void loop() {
  while (Serial.available() > 0){
    processByte(Serial.read());  //communicating with user&Matlab
  }
  if (mode == RUN){ //execute sweep
    runsweep();
  }   
}

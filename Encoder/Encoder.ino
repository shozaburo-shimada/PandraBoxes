// Library is here: https://github.com/SeeedDocument/Grove-Encoder/tree/master/res

#include <Adafruit_NeoPixel.h>
#include <Servo.h>
#include <CircularLED.h>
#include <Encoder.h>
#include <TimerOne.h>

#define PIN_SERVO       11
#define PIN_LEDSTRIP    6

#define NUM_LEDS        29
#define SPEED_LED       50 //ms
#define ANGLE_LOCK      120 //degree
#define ANGLE_UNLOCK    0 //degree

CircularLED circularLED(9, 8);
Adafruit_NeoPixel pixels(NUM_LEDS, PIN_LEDSTRIP, NEO_GRB + NEO_KHZ800);
//Servo servo;

unsigned int LED[24];
int index_LED;
const int seikai[4] = {1, 1, -1, 1}; // 1: right, 2: left
int seikai_data[4] = {0};

int counter = 0;
int sum = 0; //total index
int num_rotate = 0; //number of rotation

boolean onLock = true;
boolean opening = false;

void setup(){
  Serial.begin(9600);
  Serial.println("Hello Encoder");
  encoder.Timer_init();

  seikai_data[0] = 25 * seikai[0];
  Serial.print("0: ");
  Serial.print(seikai_data[0]);
  Serial.print(", ");
  for(int i = 1; i < 4; i++){
    seikai_data[i] = seikai_data[i - 1] + 25 * seikai[i];
    Serial.print(i);
    Serial.print(": ");
    Serial.print(seikai_data[i]);
    Serial.print(", ");
  }
  Serial.println();

  //Servo Init
  pinMode(11, OUTPUT);
  TCCR2A = 0b10100011; //A Compare match:low, B Compare match:low, WGM2[1:0]:Fast PWM & TOP=ICR2
  TCCR2B = 0b00000110; //WGM2[2]:Fast PWM & TOP=0xFF, WGM2[2:0]= 011, CS3[2:0]: 1/256 => Tpwm = 255/(16MHz/256) = 4.08ms
  servo(ANGLE_LOCK);

  pixels.begin();
  pixels.show();
  
}

void loop(){
  led_encoder();
  check_seikai();

  if(opening == false){
    //LED Control
    pixels.clear();
    for(int i = 0; i < counter; i++){
      if(onLock == true){
        //Red color during lock
        pixels.setPixelColor(i, pixels.Color(150, 0, 0));
      }else{
        //Green color during unlock
        pixels.setPixelColor(i, pixels.Color(0, 150, 0));    
      }
    }
    
    delay(1);
    pixels.show();
    if(counter++ > NUM_LEDS) counter = 0;
    
  }else{
    if(onLock == false){
      Serial.println("Servo UNLOCK");
      blinkLEDS(3, 0, 150, 0);
      servo(ANGLE_UNLOCK);

    }else{
      Serial.println("Servo LOCK");     
      servo(ANGLE_LOCK);
    }
    opening = false;
      
  }

  delay(SPEED_LED);
}

void blinkLEDS(uint8_t time_blink, uint8_t red, uint8_t green, uint8_t blue){
  for(int i = 0; i < time_blink; i++){
    allLEDS(true, red, green, blue);
    delay(200);
    allLEDS(false, 0, 0, 0);
    delay(200);
  }
}

void allLEDS(boolean onoff, uint8_t red, uint8_t green, uint8_t blue){
  pixels.clear();
  if(onoff){
    for(int i = 0; i < NUM_LEDS; i++){
      pixels.setPixelColor(i, pixels.Color(red, green, blue));
    }
  }
  pixels.show();
}

void led_encoder(){
  if (encoder.rotate_flag == 1){
    if (encoder.direct == 1){
      index_LED++;
      sum++;
      if (index_LED > 24) index_LED = 0;
      SenttocircularBar(index_LED);
   
    }else{
      index_LED--;
      sum--;
      if(index_LED < 0) index_LED = 24;
      SenttocircularBar(index_LED);
    }
    encoder.rotate_flag = 0;
  }
}

void SenttocircularBar(int index){
  for (int i = 0; i < 24; i++){
    if (i < index){
      LED[i] = 0xff;
    }else{
      LED[i] = 0;
    }
  }
  circularLED.CircularLEDWrite(LED);
}

void check_seikai(){
  /*
  Serial.print(seikai_data[num_rotate]);
  Serial.print(", ");
  Serial.print(sum);
  Serial.print(", ");
  Serial.print(abs(seikai_data[num_rotate] - counter));
  Serial.println();
  */
  
  if(abs(seikai_data[num_rotate] - sum) == 0){
    //Seikai
    Serial.print(num_rotate);
    Serial.print(":Seikai");
    Serial.println();
    num_rotate++;
    if(num_rotate > 3){
      Serial.println("Clear");
      reset_rotate();
      onLock = false;
      opening = true;
    }
    
    
  }else if(abs(seikai_data[num_rotate] - sum) > 50){
    //Machigai
    Serial.print(num_rotate);
    Serial.print(":Machigai");
    Serial.println();
    //Reset parameter
    reset_rotate();
    onLock = true;
  }

}

void reset_rotate(){
    num_rotate = 0;
    sum = 0;
    index_LED = 0;  
}

void servo(unsigned int angle) {
  if(angle>180) angle = 180;
  OCR2A = 34 + angle/1.6;
}

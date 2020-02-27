#include <Adafruit_NeoPixel.h>
#include <Servo.h>

#define PIN_MAGNET_SIG  4
#define PIN_SERVO       5
#define PIN_LEDSTRIP    6

#define NUM_LEDS        29
#define SPEED_LED       50 //ms
#define ANGLE_LOCK      60 //degree
#define ANGLE_UNLOCK    0 //degree

Adafruit_NeoPixel pixels(NUM_LEDS, PIN_LEDSTRIP, NEO_GRB + NEO_KHZ800);
Servo servo;

int val = 0;
int pre_val = 0;
int counter = 0;
boolean onLock = true;
boolean opening = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Hello Magnet Sensor");
  //pinMode(PIN_MAGNET_SIG, INPUT);
  servo.attach(PIN_SERVO);
  servo.write(ANGLE_LOCK);
  
  pixels.begin();
  pixels.show();

}



void loop() {
  
  pre_val = val;
  val = digitalRead(PIN_MAGNET_SIG);
  
  if(val == 1 && pre_val == 0){
    if(onLock == true){
      //Unlock
      Serial.println("unLock");
      onLock = false;
    }else{
      //Lock
      Serial.println("Lock");
      onLock = true;
    }
    opening = true;
    //Reset LED
    counter = 0;
  }
  
  
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
      servo.write(ANGLE_UNLOCK);

    }else{
      Serial.println("Servo LOCK");     
      servo.write(ANGLE_LOCK);
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

#include <SwitecX25.h>


//-----------------------RGB Variables-----------------------------------//
int LED_RED = 11;
int LED_BLUE = 10;
int LED_GREEN = 9;
int rgbValue = 0;
int light;


//-----------------------Motor Variables--------------------------------//

// standard X25.168 range 315 degrees at 1/10 degree steps
#define STEPS (315*3)

int m1 = 8; //pin 8
int m2 = 7; //pin 7
int m3 = 5; //pin 5
int m4 = 6; //pin 6
static int nextPos;

// For motors connected to pins 8,7,5,6
SwitecX25 motor1(STEPS,m1,m2,m3,m4);

//Serial vars
String input = "";
int ledVal = 0;

//-----------------------Display Variables-------------------------------//
const byte LED_CHAR_SET[10] = {
  B11111100,B01100000,B11011010,B11110010, //0,1,2,3
  B01100110,B10110110,B10111110,B11100000, //4,5,6,7
  B11111110,B11100110};                    //8,9

const byte NONE[1] = {
  B00000000};

const int dataPin  = 4;  // Pin connected to Pin 14 of 74HC595 (Data)
const int clockPin = 2;  // Pin connected to Pin 11 of 74HC595 (Clock)
const int latchPin = 3;  // Pin connected to Pin 12 of 74HC595 (Latch)

//-----------------------Update Variables-------------------------------//
int time = 0;
double targetPosition = 0;
int currentPosition = 0;
long nextUpdate;
int delta;

void setup(void) {
  Serial.begin(9600);
  Serial.print("Go time!");

  //Zero the position, set to zero  
  motor1.zero();

  //Display
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  clearDisplay();

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  nextUpdate = micros();
}

void loop() {

  updateMotor();
  
}

void updateMotor() {
  delta = targetPosition - currentPosition;
  if (micros() > nextUpdate) {
    if((delta) > 0) {
      motor1.stepUp();
      currentPosition++;
    } else if(delta < 0){
      motor1.stepDown();
      currentPosition--;
    }
    
    if (delta != 0) {
      nextUpdate = micros() + 400 + 200000/(abs(delta));
    }
  }
}

void parse_message(String message) {
  Serial.println(message);
  if(message[0] == '(') {
    time = millis();
    int digitVal = 10*(message[1]-'0') + (message[2]-'0'); 
    //Update Digits
    setDisplay(LED_CHAR_SET[digitVal/10],LED_CHAR_SET[digitVal%10]);
  }
    
  if(message[message.length()-1] == ')') {
    double motorVal = 10*(message[message.length()-3]-'0') + (message[message.length()-2]-'0');
    //Convert to a percentage:
    motorVal = motorVal/100;
    //Assuming 0 to 180 degree is 0 to 100% and there's 3
    //steps per degree, calculate the position that matches
    //the percentage.
    targetPosition = motorVal*180*3;
    Serial.println(targetPosition);
  }
 
  if (message[0] == '<') {
    rgbValue = 100*(message[1] -'0') + 10*(message[2]-'0') + (message[3]-'0');
    light = 100*(message[4] -'0') + 10*(message[5]-'0') + (message[6]-'0');
    setLED(rgbValue, 255);
  }
}

void serialEvent() {
  while (Serial.available() > 0){
    char c = Serial.read();
    input += c;

    if (c == ')' || c =='>'){
      parse_message(input);
      input = "";
    }
    if (c == 'u') {
      motor1.stepUp();
    }
    if (c == 'd') {
      motor1.stepDown();
    }
  }
}

void clearDisplay() {
  //Prepare 595 to received data
  digitalWrite(latchPin, LOW); 
  //Shift data to 595
  shiftOut(dataPin, clockPin, LSBFIRST, NONE[1]);
  shiftOut(dataPin,clockPin, LSBFIRST, NONE[1]);
  //Set latch to high to send data
  digitalWrite(latchPin, HIGH);
}



void setDisplay(byte display1, byte display2) {
  //Prepare 595 to received data
  digitalWrite(latchPin, LOW); 
  //Shift data to 595
  shiftOut(dataPin, clockPin, LSBFIRST, display2);
  shiftOut(dataPin, clockPin, LSBFIRST, display1);
  //Set latch to high to send data
  digitalWrite(latchPin, HIGH);
}

void setLED(int hue, int l){
  int col[3] = {0,0,0};
  getRGB(hue, 255, l, col);
  ledWrite(col[0], col[1], col[2]);
}

void getRGB(int hue, int sat, int val, int colors[3]) {
  // hue: 0-259, sat: 0-255, val (lightness): 0-255
  int r, g, b, base;

  if (sat == 0) { // Achromatic color (gray).
    colors[0]=val;
    colors[1]=val;
    colors[2]=val;
  } 
  else  {
    base = ((255 - sat) * val)>>8;
    switch(hue/60) {
    case 0:
      r = val;
      g = (((val-base)*hue)/60)+base;
      b = base;
      break;
    case 1:
      r = (((val-base)*(60-(hue%60)))/60)+base;
      g = val;
      b = base;
      break;
    case 2:
      r = base;
      g = val;
      b = (((val-base)*(hue%60))/60)+base;
      break;
    case 3:
      r = base;
      g = (((val-base)*(60-(hue%60)))/60)+base;
      b = val;
      break;
    case 4:
      r = (((val-base)*(hue%60))/60)+base;
      g = base;
      b = val;
      break;
    case 5:
      r = val;
      g = base;
      b = (((val-base)*(60-(hue%60)))/60)+base;
      break;
    }
    colors[0]=r;
    colors[1]=g;
    colors[2]=b;
  }
}

void ledWrite(int r, int g, int b){
  analogWrite(LED_RED, 255-r);
  analogWrite(LED_GREEN, 255-g);
  analogWrite(LED_BLUE, 255-b);
}



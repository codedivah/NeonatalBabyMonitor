#include <Metro.h>
#include <LiquidCrystal.h>


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
// RGB CONTROL

//int redPin = 11;
//int greenPin = 10;
//int bluePin = 9;
int redPin = 6;
int greenPin = 9;
int bluePin = 10;

int breathThreshold=10000;

int mini;
int maxi;
int Now;
int pressure;
int counts;
boolean timerIsRunning=false;
long previousMillis = 0;
unsigned long currentMillis=0;
boolean state = LOW;//ledState used to set the LED
//boolean breathAlarm=false;
float  temp;

boolean tempAlarm;
int tempThresholdLow=23;//32;
int tempThresholdHigh=27;//40;
boolean breathAlarm=false;

int lcdDisplayCase=0;

int led =13;
float breathingRate;
const int ledPin =  13;      // the number of the LED pin

#define filter_large 0.999 //used to compute running average
#define filter_small 0.001 //make sure these add up to 1
Metro ledBlinkMetro=Metro(1000);
Metro SamplingInterval= Metro(100);

Metro lcdRefresh=Metro(3000);

Metro calculateBreathingRate= Metro(10000);//calculate the breathing rate every 10 secs
void setup(){
  Serial.begin(9600);
  pinMode(led,OUTPUT);
  Serial.println("Starting the baby monitor");
  Serial.println("Please Wait while breathing rate is acquired");
  lcd.begin(16, 2);
}
void loop(){
  //DUMMY LCD CODE
  if(SamplingInterval.check()==1){
    Now=analogRead(A1);
    Now=(filter_large*analogRead(A1))+(filter_small*analogRead(A1));
    if(Now>maxi){
      maxi=Now;
    }
    else if(Now<maxi){
      mini=Now;
      if(mini==0){
        maxi=0;
        counts++;
        previousMillis=millis();
      }
    }

  }
  currentMillis=millis();
  if(currentMillis - previousMillis > breathThreshold) {
    previousMillis = currentMillis;  
    breathAlarm=true; 
  }


  if(breathAlarm){
    if(ledBlinkMetro.check()==true)ledBlink();
  }
  if(tempAlarm){
    if(ledBlinkMetro.check()==true)ledBlink();
  }

  //  if(tempAlarm){
  //    if(ledBlinkMetro.check()==true)ledBlink();
  //  }
   temp=Temp();
  RGBcontrol();
  if( temp<tempThresholdLow){
    // Serial.println(temp);
    tempAlarm=true;
  }
  else  if(temp>tempThresholdHigh)tempAlarm=true;

  if(calculateBreathingRate.check()==1){
    breathingRate=counts*0.1*60;
    Serial.print("breathingRate ");
    Serial.println(breathingRate,3);
    Serial.print("counts ");
    Serial.println(counts);
    counts=0;
    //Also display temp
    Serial.print("Temperature is ");
    Serial.println(temp);
  }


  if(lcdRefresh.check()==1){
    lcd.clear();
    runDisplay(lcdDisplayCase);
    lcdDisplayCase++;
    if(lcdDisplayCase==2)lcdDisplayCase=0;
    if(!breathAlarm&&!tempAlarm)lcdDisplayCase=0;

  }
}

void ledBlink(){
  digitalWrite(led,state);
  state=~state;
}

void setColor(int red, int green, int blue)
{
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);  
}

float Temp(){
  int Temp=analogRead(A0);
  float temp_c= (((Temp*(5000/1024)-0))/10);
  return temp_c;
}

void runDisplay(int lcdDisplayCase){
  switch(lcdDisplayCase){
  case 0:
    //lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("Temperature=");
    lcd.print(Temp());
    //lcd.print()
    lcd.setCursor(0,0);
    lcd.print("Breath Rate=");
    lcd.print(int(breathingRate));
    break;
  case 1:
    // alarm?
    //lcd.clear();
    if(tempAlarm){
      lcd.setCursor(0,1);
      lcd.print("Temp Alarm");
      lcd.print(Temp());
    }
    if(breathAlarm){
      //lcd.print();
      lcd.setCursor(0,0);
      lcd.print("Breath alarm ");
      lcd.print(int(breathingRate));
      break;
    }

  }
}


void RGBcontrol(){
    if(!tempAlarm)setColor(0, 255, 0);  // green
  else if(temp<tempThresholdLow)setColor(0, 0, 255);  // blue
  else if (temp>tempThresholdHigh)setColor(255, 0,0);  // red
}


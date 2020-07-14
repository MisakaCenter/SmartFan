#include <MsTimer2.h>
#include <Servo.h>

Servo Servo1;

const int TrigPin = 4; 
const int EchoPin = 5; 
const int LED1=7;
const int LED2=8;
const int Fan = 6;
int keypressed1_int = 0;
unsigned long time1_isr = 0;
unsigned long triggertime1 = 0;
unsigned long delta = 500;
int inJitter1 = 0;
int inJitter2 = 0;
int keypressed2_int = 0;
unsigned long time2_isr = 0;
unsigned long triggertime2 = 0;
float distance; 
int AFK;
int Fan_Mode=0;//0 auto;1 manual
int Rotate_Mode=0;//0 off;1 on

int angle=90;
int sign=0;//0+ 1-

void Control_Fan();
void Rotate();

void Change_Mode(){
  time1_isr = millis();   
  keypressed1_int = 1;
}

void Change_Rotate(){
  time2_isr = millis();   
  keypressed2_int = 1;
}

void setup() {
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(0,Change_Mode,RISING);
  attachInterrupt(1,Change_Rotate,RISING);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(TrigPin, OUTPUT); 
  pinMode(EchoPin, INPUT); 
  
  pinMode(Fan, OUTPUT); 
  
  digitalWrite(LED1, HIGH); 
  digitalWrite(LED2, LOW); 
  MsTimer2::set(100,Rotate);
  MsTimer2::start();

  Servo1.attach(11);
  Servo1.write(90);
  Serial.begin(9600);
}

void loop() {
   if (keypressed1_int == 1)
  {
    if (inJitter1 == 0)
    {
      triggertime1 = time1_isr;
      inJitter1 = 1;

      if(Fan_Mode==1) {
        digitalWrite(LED1, HIGH); 
        Fan_Mode=0;}
      else{
        Fan_Mode=1;
        digitalWrite(LED1, LOW); 
      }
    }
    if (millis() - triggertime1 > delta)
    { 
      inJitter1 = 0;
    }
    keypressed1_int = 0;
  }

  if (keypressed2_int == 1)
  {
    if (inJitter2 == 0)
    {
      triggertime2 = time2_isr;
      inJitter2 = 1;
      if(Rotate_Mode==1) {
        Rotate_Mode=0;
        sign=0;
        digitalWrite(LED2, LOW); 
      }
      else{
        Rotate_Mode=1;
        digitalWrite(LED2, HIGH); 
      }
    }
    if (millis() - triggertime2 > delta)
    { 
      inJitter2 = 0;
    }
    keypressed2_int = 0;
  }
  
   if(Fan_Mode==0)
      Control_Fan();
   else analogWrite(Fan, 230);

}

void Control_Fan(){
   digitalWrite(TrigPin, LOW); 
   delayMicroseconds(2); 
   digitalWrite(TrigPin, HIGH); 
   delayMicroseconds(10);
   digitalWrite(TrigPin, LOW); 
   distance = pulseIn(EchoPin, HIGH) / 58.00;
   Serial.print(distance); 
   Serial.print("cm"); 
   Serial.println(); 
   int temp;
   if (distance<30)
   {
    temp=map(distance,0,30,150,180);
   }else if(distance<50){
    temp=map(distance,30,50,180,210);
   }else if(distance<70){
    temp=map(distance,50,70,210,230);
   }else{
    temp=-1;
   }
   if(temp>0)
   {
      analogWrite(Fan, temp);
      AFK=0;
   }
   else AFK++;
   if(AFK>3) analogWrite(Fan, 0);
}

void Rotate(){
  if(Rotate_Mode==1)
  {
    if(sign==0)
      angle=angle+1;
    else angle=angle-1;
    if (angle>180){
      sign=(sign+1)%2;
      angle=180;
    }
    if (angle<0){
      sign=(sign+1)%2;
      angle=0;
    }
    Servo1.write(angle);
  }
}

#include <Servo.h>             //Servo library
#include <stdlib.h>
Servo servo;                   //initialize a servo object for the connected servo                  
int angle;                     //angle of servo
#define motp 3                 //movement motor
#define motn 4
#define eco 11                 //distance sensor
#define trig 12
#define led 7                  //indicator led
long duration;                 
int pos=1,temp,tempC,distance,tym=0,l=0,f=0,i,j;                   //pos = position of motor; temp, tempC = temperature, in Celsius; l = light, f = fan
float t=0,t1=0,t2=0;                                               //t = timer for 30 seconds repositioning, t1 = timer for light, t2 = timer for fan
String fray;                                                      
float bill1 = 0,bill2 = 0;                                         // bill1 = bill of device 1 (light) , bill2 = bill of device 2 (fan)

void setup() 
{ 
  Serial.begin(9600);
  servo.attach(2);             //attach the signal pin of servo to pin2 of arduino
  servo.write(28);             //set the initial angle of servo to 28 degree  
  pinMode(3,OUTPUT);           
  pinMode(4,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(trig,OUTPUT);
  pinMode(eco,INPUT);
  pinMode(led,OUTPUT);
} 

  
void loop() 
{ 
  if(tym > 10800)              //if device on for 3 hours then switched off automatically
  {
    if(pos == 1)
    {
       tog_off();
       fanP();
       tog_off();
       lightP();
       t = 0;
       l=0;
       tym = 0;
      }
    else if(pos == 2)
      {
       tog_off();
       lightP();
       tog_off();
       t = 0;
       l=0;
       tym = 0;
      }
  }
  temp = analogRead(A1);       //measure the temperature of the room
  temp = (temp*500)/1023;
  tempC = temp;
  
  
  if (Serial.available())
  {
    String x = Serial.readString();    //string read from bolt cloud
    //Serial.println(x);
    if(x == "a")
    {              // case 1 fan off
      digitalWrite(led,HIGH);
      delay(100);
      digitalWrite(led,LOW);
      if(pos == 2)
      {
       tog_off();
       t = 0;
       f=0;
       tym = 0;
      }
      else if(pos == 1)
      {
       fanP();
       tog_off();
       t = 0;
       f=0;
       tym = 0;
      }
    }
    else if(x == "s")                 //case 2 fan on
    {         
      digitalWrite(led,HIGH);
      delay(100);
      digitalWrite(led,LOW);
      if(pos == 2)
      {
       tog_on();
       t = 0;
       f++;
       tym = 0;
      }
      else if(pos == 1)
      {
       fanP(); 
       tog_on();
       t = 0;
       f++;
       tym = 0;
      }
    }
    else if(x == "d")                  //case 3 light off
    {         
      digitalWrite(led,HIGH);
      delay(100);
      digitalWrite(led,LOW);
      if(pos == 1){
       tog_off();
       t = 0;
       l=0;
       tym = 0;
      }
      else if(pos == 2)
      {
       lightP();
       tog_off();
       t = 0;
       l=0;
       tym = 0;
      }
    }
    else if(x == "f")          //case 4 light on   
    {         
      digitalWrite(led,HIGH);
      delay(100);
      digitalWrite(led,LOW);
      if(pos == 1)
      {
       tog_on();
       t = 0;
       l++;
       tym = 0;
      }
      else if(pos == 2)
      {
       lightP(); 
       tog_on();  
       t = 0;
       l++;
       tym = 0;
      }
    }
    else if(x.startsWith("o"))
    {
      digitalWrite(led,HIGH);
      delay(100);
      digitalWrite(led,LOW);
      j = x.length();
      fray = x.substring(1);
      float z = fray.toInt();
      z = z/1000;
      bill1 = ((z*t1*2.5)/(60*60));             // calculation to find electricity bill in INR
      Serial.println("Bill = " + String(bill1,6) + " INR" + " ");
      j = 0; 
    }
    else if(x.startsWith("p"))
    {
      digitalWrite(led,HIGH);
      delay(100);
      digitalWrite(led,LOW);
      j = x.length();
      fray = x.substring(1);
      float z = fray.toInt();
      z = z/1000;
      bill2 = (z*t2*2.5/(60*60));              // calculation to find electricity bill in INR
      Serial.print("Bill = " + String(bill2,6) + " INR" + " ");
      j = 0; 
    }
    else if(x == "l")
    {
      digitalWrite(led,HIGH);
      delay(100);
      digitalWrite(led,LOW);
      t1 = 0.0;
      t2 = 0.0;
      bill1 = 0.0;
      bill2 = 0.0;
    }
  }

  if(tempC >= 35)              //temperature gone above 35 deg C turn on the fan
    {
      if(pos == 2)
      {
       tog_on();
       t = 0;
       f++;
       tym = 0;
      }
      else if(pos == 1)
      {
       fanP(); 
       tog_on();
       t = 0;
       f++;
       tym = 0;
      }
    }

  if(l>0 || f>0)
  {
    tym = tym + 0.307;
    t1 = t1 + 0.307;
    t2 = t2 + 0.307;
  }

  manual();                   // check gesture controls
    
  delay(100);                 
  t = t + 0.307;              // for repositioning the servo after 30 sec of use
  if(t >= 60 && pos == 2){
    lightP();
  }
}

void lightP()                 // function- light position
{
  digitalWrite(motp,HIGH);
  digitalWrite(motn,LOW);
  delay(13000);
  digitalWrite(motp,LOW);
  digitalWrite(motn,LOW);
  pos = 1;
}

void fanP()                   // function- fan position
{
  digitalWrite(motp,LOW);
  digitalWrite(motn,HIGH);
  delay(13000);
  digitalWrite(motp,LOW);
  digitalWrite(motn,LOW);
  pos = 2;
}

void tog_off()                //toggle off
{
  servo.write(0); //off
  delay(1000);
  servo.write(28);
}

void tog_on()                 //toggle on
{
  servo.write(50); //on 
  delay(1000);
  servo.write(28);
}

void manual()                            //gesture control
{
  digitalWrite(trig,LOW);
  delayMicroseconds(2);
  digitalWrite(trig,HIGH);
  delay(200);
  digitalWrite(trig,LOW);
  duration=pulseIn(eco,HIGH);
  distance=duration*0.034/2;
  if(distance<=20)                   //if distance less than 20cm turn off light and fan
  {
    digitalWrite(led,HIGH);
    delay(100);
    digitalWrite(led,LOW);
    if(pos == 2)
    {
       tog_off();
       lightP();
       tog_off();
       t = 0;
       l=0;
       tym = 0;
    }
    else if(pos == 1 && f>0 && l>0)
    {
       fanP();
       tog_off();
       t = 0;
       l=0;
       f=0;
       tym = 0;
       lightP();
       tog_off();
    }  
    else if(pos == 1 && l>0)
    {
        tog_off();
        t = 0;
        l = 0;
        tym = 0;
    }
    else if(pos == 1 && f>0)
    {
       fanP();
       tog_off();
       t = 0;
       f=0;
       tym = 0;
       lightP();
    }
  }
  else if(distance>=20 && distance<=40)        //if distance is 20cm to 40cm turn on the light
  {
      digitalWrite(led,HIGH);
      delay(100);
      digitalWrite(led,LOW);
      if(pos == 1 )
      {
        tog_on();
        t = 0;
        l++;
        tym = 0;
      }
      else if(pos == 2)
      {
        lightP(); 
        tog_on();
        t = 0;
        l++;
        tym = 0;
      }
  }
}

// Easy Peasy Tachometer
//James May
//#include <Wire.h>
//#include <Adafruit_SSD1306.h>// You may have to edit library for 128x64, //default is 128 x 32.

//#define OLED_WIDTH 128
//#define OLED_HEIGHT 64

//#define OLED_ADDR   0x3C // A very common address for these displays.

//Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT);


float value=0;
float rev=0;
int rpm;
int oldtime=0;        
int time;

void isr()          //interrupt service routine
{
rev++;
}

void setup()
{
 Serial.begin(9600);
pinMode(2, INPUT);
attachInterrupt(0,isr,FALLING);  //attaching the interrupt





delay(1000);// 2 second delay
detachInterrupt(0);           //detaches the interrupt while calculating
time=millis()-oldtime;        //finds the time 
rpm=(rev/time)*60000;         //calculates rpm
oldtime=millis();             //saves the current time
rev=0;


Serial.print(rpm);
  Serial.println(" RPMm");
  
  Serial.print(rpm/2);
  Serial.println(" RPM");
  
   
 
  

  attachInterrupt(0,isr,FALLING);

}

void loop()
{

}

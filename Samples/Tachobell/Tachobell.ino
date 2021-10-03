


double value=0;
double rev=0;
int rpm;
int oldtime=0;        
int time;
#define anemo 35
#define debugl 5

void IRAM_ATTR Ext_INT1_ISR()          //interrupt service routine
{
rev++;
//digitalWrite(debugl,HIGH);
//digitalWrite(debugl,LOW);
}

void setup()
{
 Serial.begin(9600);
pinMode(anemo, INPUT);
pinMode(debugl, OUTPUT);
}

void loop()
{

attachInterrupt(anemo,Ext_INT1_ISR,FALLING);  //attaching the interrupt
delay(10000);// 2 second delay
detachInterrupt(anemo);           //detaches the interrupt while calculating
Serial.println(rev);
Serial.println(time);
time=millis()-oldtime;        //finds the time 
rpm=(rev/time)*60000;         //calculates rpm
oldtime=millis();             //saves the current time
rev=0;

Serial.print(rpm);
  Serial.println(" RPMm");
  
  Serial.print(rpm/2);
  Serial.println(" RPM");
  
   
 
  

  attachInterrupt(anemo,Ext_INT1_ISR,FALLING);


}

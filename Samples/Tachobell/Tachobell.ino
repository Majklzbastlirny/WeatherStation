


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
pinMode(18, INPUT);
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

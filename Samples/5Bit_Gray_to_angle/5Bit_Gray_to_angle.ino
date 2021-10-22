void setup() {
  // put your setup code here, to run once:
pinMode(2, INPUT);
pinMode(3, INPUT);
pinMode(4, INPUT);
pinMode(5, INPUT);
pinMode(6, INPUT);
Serial.begin(9600);
}
void loop() {
  // put your main code here, to run repeatedly:
bool D1 = 0;  
bool D2 = 0;
bool D3 = 0;
bool D4 = 0;
bool D5 = 0;
float WW = 0;

D5 = digitalRead(6);
D4 = digitalRead(5);
D3 = digitalRead(4);
D2 = digitalRead(3);
D1 = digitalRead(2);

if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
  WW = 0;
  }
else if (D1 == 1 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 0) {
  WW = 11.25;
  }
else if (D1 == 1 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
  WW = 22.5;
  }
else if (D1 == 0 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 0) {
  WW = 33.75;
  }
else if (D1 == 0 && D2 == 1 && D3 == 1 && D4 == 0 && D5 == 0) {
  WW = 45;
  }
else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 0 && D5 == 0) {
  WW = 56.25;
  }
else if (D1 == 1 && D2 == 0 && D3 == 1 && D4 == 0 && D5 == 0) {
  WW = 67.5;
  }
else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 0 && D5 == 0) {
  WW = 78.75;
  }
else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 0) {
  WW = 90;
  }
else if (D1 == 1 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 0) {
  WW = 101.25;
  }
else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 0) {
  WW = 112.5;
  }
else if (D1 == 0 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 0) {
  WW = 123.75;
  }
else if (D1 == 0 && D2 == 1 && D3 == 0 && D4 == 1 && D5 == 0) {
  WW = 135;
  }
else if (D1 == 1 && D2 == 1 && D3 == 0 && D4 == 1 && D5 == 0) {
  WW = 146.25;//not
  }
else if (D1 == 1 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 0) {
  WW = 157.5;//not
  }
else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 0) {//15
  WW = 168.75;
  }
else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 1) {
  WW = 180;
  }
else if (D1 == 1 && D2 == 0 && D3 == 0 && D4 == 1 && D5 == 1) {
  WW = 191.25;//not
  }
else if (D1 == 1 && D2 == 1 && D3 == 0 && D4 == 1 && D5 == 1) {
  WW = 202.5;//not
  }
else if (D1 == 0 && D2 == 1 && D3 == 0 && D4 == 1 && D5 == 1) {
  WW = 213.75;
  }
else if (D1 == 0 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 1) {
  WW = 225;
  }
else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 1 && D5 == 1) {
  WW = 236.25;//not
  }
else if (D1 == 1 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 1) {
  WW = 247.5;//not
  }
else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 1 && D5 == 1) {
  WW = 258.75;
  }
else if (D1 == 0 && D2 == 0 && D3 == 1 && D4 == 0 && D5 == 1) {
  WW = 270;
  }
else if (D1 == 1 && D2 == 0 && D3 == 1 && D4 == 0 && D5 == 1) {
  WW = 281.25;
  }
else if (D1 == 1 && D2 == 1 && D3 == 1 && D4 == 0 && D5 == 1) {
  WW = 292.5;
  }
else if (D1 == 0 && D2 == 1 && D3 == 1 && D4 == 0 && D5 == 1) {
  WW = 303.75;
  }
else if (D1 == 0 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 1) {
  WW = 315;
  }
else if (D1 == 1 && D2 == 1 && D3 == 0 && D4 == 0 && D5 == 1) {
  WW = 326.25;
  }
else if (D1 == 1 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 1) {
  WW = 337.5;
  }
else if (D1 == 0 && D2 == 0 && D3 == 0 && D4 == 0 && D5 == 1) {
  WW = 348.75;
  }

else {
  WW=0;
  }
Serial.print(WW);
Serial.print("   ");
//Serial.println(D1, D2, D3, D4, D5);
Serial.print(D1);
Serial.print(" ");
Serial.print(D2);
Serial.print(" ");
Serial.print(D3);
Serial.print(" ");
Serial.print(D4);
Serial.print(" ");
Serial.print(D5);
Serial.println("");
delay(1000);
}

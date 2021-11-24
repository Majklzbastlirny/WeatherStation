/*
 This piece code is designed for wind vane by Yoctupuce
 https://www.yoctopuce.com/EN/article/how-to-measure-wind-part-2
 https://www.thingiverse.com/thing:42858
  
Created By Michal Basler 23.11.2021 for maturita project
https://github.com/Majklzbastlirny  
 */
void setup() {
  // put your setup code here, to run once:
pinMode(2, INPUT);
pinMode(3, INPUT);
pinMode(4, INPUT);
pinMode(5, INPUT);
pinMode(6, INPUT);
Serial.begin(115200);
}
void loop() {
  // put your main code here, to run repeatedly:
bool D1 = 0;  
bool D2 = 0;
bool D3 = 0;
bool D4 = 0;
bool D5 = 0;
float wv = 0;

D5 = digitalRead(6);
D4 = digitalRead(5);
D3 = digitalRead(4);
D2 = digitalRead(3);
D1 = digitalRead(2);

if (D1 ==  1 && D2 == 0  && D3 == 0 && D4 == 0 && D5 == 0 ) { wv = 0; }
else if (D1 ==  1 && D2 == 1  && D3 == 0 && D4 == 0 && D5 == 0 ) { wv = 12; }
else if (D1 ==  1 && D2 == 1  && D3 == 0 && D4 == 1 && D5 == 0 ) { wv = 24; }
else if (D1 ==  1 && D2 == 1  && D3 == 0 && D4 == 1 && D5 == 1 ) { wv = 36; }
else if (D1 ==  1 && D2 == 1  && D3 == 0 && D4 == 0 && D5 == 1 ) { wv = 48; }
else if (D1 ==  0 && D2 == 1  && D3 == 0 && D4 == 0 && D5 == 1 ) { wv = 60; }
else if (D1 ==  0 && D2 == 1  && D3 == 0 && D4 == 0 && D5 == 0 ) { wv = 72; }
else if (D1 ==  0 && D2 == 1  && D3 == 1 && D4 == 0 && D5 == 0 ) { wv = 84; }
else if (D1 ==  0 && D2 == 1  && D3 == 1 && D4 == 0 && D5 == 1 ) { wv = 96; }
else if (D1 ==  1 && D2 == 1  && D3 == 1 && D4 == 0 && D5 == 1 ) { wv = 108; }
else if (D1 ==  1 && D2 == 1  && D3 == 1 && D4 == 0 && D5 == 0 ) { wv = 120; }
else if (D1 ==  1 && D2 == 0  && D3 == 1 && D4 == 0 && D5 == 0 ) { wv = 132; }
else if (D1 ==  0 && D2 == 0  && D3 == 1 && D4 == 0 && D5 == 0 ) { wv = 144; }
else if (D1 ==  0 && D2 == 0  && D3 == 1 && D4 == 1 && D5 == 0 ) { wv = 156; }
else if (D1 ==  1 && D2 == 0  && D3 == 1 && D4 == 1 && D5 == 0 ) { wv = 168; }
else if (D1 ==  1 && D2 == 1  && D3 == 1 && D4 == 1 && D5 == 0 ) { wv = 180; }
else if (D1 ==  0 && D2 == 1  && D3 == 1 && D4 == 1 && D5 == 0 ) { wv = 192; }
else if (D1 ==  0 && D2 == 1  && D3 == 0 && D4 == 1 && D5 == 0 ) { wv = 204; }
else if (D1 ==  0 && D2 == 0  && D3 == 0 && D4 == 1 && D5 == 0 ) { wv = 216; }
else if (D1 ==  0 && D2 == 0  && D3 == 0 && D4 == 1 && D5 == 1 ) { wv = 228; }
else if (D1 ==  0 && D2 == 1  && D3 == 0 && D4 == 1 && D5 == 1 ) { wv = 240; }
else if (D1 ==  0 && D2 == 1  && D3 == 1 && D4 == 1 && D5 == 1 ) { wv = 252; }
else if (D1 ==  0 && D2 == 0  && D3 == 1 && D4 == 1 && D5 == 1 ) { wv = 264; }
else if (D1 ==  0 && D2 == 0  && D3 == 1 && D4 == 0 && D5 == 1 ) { wv = 276; }
else if (D1 ==  0 && D2 == 0  && D3 == 0 && D4 == 0 && D5 == 1 ) { wv = 288; }
else if (D1 ==  1 && D2 == 0  && D3 == 0 && D4 == 0 && D5 == 1 ) { wv = 300; }
else if (D1 ==  1 && D2 == 0  && D3 == 1 && D4 == 0 && D5 == 1 ) { wv = 312; }
else if (D1 ==  1 && D2 == 0  && D3 == 1 && D4 == 1 && D5 == 1 ) { wv = 324; }
else if (D1 ==  1 && D2 == 0  && D3 == 0 && D4 == 1 && D5 == 1 ) { wv = 336; }
else if (D1 ==  1 && D2 == 0  && D3 == 0 && D4 == 1 && D5 == 0 ) { wv = 348; }


else {
  wv=0;
  }
Serial.print(wv);
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

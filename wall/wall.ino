#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SharpIR.h>
#include <Adafruit_MLX90614.h>
#include "pinku.h"

#define tgreen digitalRead(tombolHijau)
#define tred digitalRead(tombolMerah)
#define tyellow digitalRead(tombolKuning)
#define getIR digitalRead(pinIR)

SharpIR sensorkiri( SharpIR::GP2Y0A21YK0F, A0 );
SharpIR sensortengah( SharpIR::GP2Y0A21YK0F, A1 );
SharpIR sensorkanan( SharpIR::GP2Y0A21YK0F, A2 );

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
LiquidCrystal_I2C lcd(0x27, 16, 2);

float suhu;
String myroom[4] = {"Black", "Red", "Green", "Blue"};
String room = myroom[0];

//SETT COLOR
//58 43 15
int cblack[3] = {135, 128, 35};
int cred[3] = {45, 100, 24};
int cgreen[3] = {78, 60, 25};
int cblue[3] = {58, 43, 15}; //BLUE

int range = 7;
int tr, tg, tb;

//Handle
bool jalan = false;
bool startPoint = false;
int dspeed = 150;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(tombolHijau, INPUT_PULLUP);
  pinMode(tombolMerah, INPUT_PULLUP);
  pinMode(tombolKuning, INPUT_PULLUP);

  pinMode(motorDKiriP, OUTPUT);
  pinMode(motorDKiriB, OUTPUT);
  pinMode(motorDKananP, OUTPUT);
  pinMode(motorDKananB, OUTPUT);

  pinMode(motorBKiriP, OUTPUT);
  pinMode(motorBKiriB, OUTPUT);
  pinMode(motorBKananP, OUTPUT);
  pinMode(motorBKananB, OUTPUT);

  //WARNA CONFIG
  pinMode(outWarna, INPUT);
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);

  //LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Robot Start");
  delay(2000);
  updateLCD();

  mlx.begin();
}

void loop() {
  //  Serial.println(getIR);
  //  digitalWrite(motorDKiriB, LOW);
  //  analogWrite(motorDKiriP, 100);
  //setMotor(-255,255);
  //    Serial.print(tred);Serial.print(" ");
  //    Serial.print(tgreen);Serial.print(" ");
  //    Serial.print(tyellow);Serial.println(" ");

  if (!tred) {
    startPoint = false;
    jalan = true;
    tr = cred[0];
    tg = cred[1];
    tb = cred[2];
    room = myroom[1];
    updateRoom();
  }

  if (!tgreen) {
    startPoint = false;
    jalan = true;
    tr = cgreen[0];
    tg = cgreen[1];
    tb = cgreen[2];
    room = myroom[2];
    updateRoom();
  }

  if (!tyellow) {
    startPoint = false;    
    jalan = true;
    tr = cblue[0];
    tg = cblue[1];
    tb = cblue[2];
    room = myroom[3];
    updateRoom();
  }


  //  wallRun();
  while (jalan) {
    int r = getRed();
    int g = getGreen();
    int b = getBlue();
    bool getColor = isColor(r, g, b, tr, tg, tb);

    if (getColor) {
      setMotor(0, 0);
      jalan = false;
      if (!startPoint) {
        while (1) {
          if (!getIR) {
            //cek IR
            while (!getIR) {
              updateLCD();
              delay(250);
            }
            delay(2000);
            balikHitam();
            break;
          }
        }
      }
    }
    else {
      wallRun();
    }
  }


  //calib();
  //  int skiri = sensorkiri.getDistance();
  //  int stengah = sensortengah.getDistance();
  //  int skanan = sensorkanan.getDistance();
  //
  //  Serial.print(skiri);Serial.print(" ");
  //  Serial.print(stengah);Serial.print(" ");
  //  Serial.print(skanan);Serial.println(" ");
  //  delay(100);
  //  Serial.print(green);Serial.print(" ");
  //  Serial.print(red);Serial.print(" ");
  //  Serial.print(yellow);Serial.println(" ");

  //  Serial.print(getRed()); Serial.print(" ");
  //  Serial.print(getGreen()); Serial.print(" ");
  //  Serial.print(getBlue()); Serial.println(" ");

  // put your main code here, to run repeatedly:

}
void balikHitam() {
   startPoint = true;    
  jalan = true;
  tr = cblack[0];
  tg = cblack[1];
  tb = cblack[2];
  room = myroom[0];
  updateRoom();
}
void wallRun() {
  int setKiri = dspeed, setKanan = dspeed;
  int dkiri = sensorkiri.getDistance();
  int dtengah = sensortengah.getDistance();
  int dkanan = sensorkanan.getDistance();

  Serial.print("JARAK: ");
  Serial.print(dkiri); Serial.print(" ");
  Serial.print(dtengah); Serial.print(" ");
  Serial.print(dkanan); Serial.println(" ");

  if (dtengah <= 16) {
    setKiri = -255;
    setKanan = 255;

    setMotor(setKiri, setKanan);
    delay(1900);
  }
  else {
    if (dtengah >= 16) {
      setMotor(125, 125);
    }
    if (dkiri <= 13 ) {
      setMotor(255, 0);
      delay(250);
    }
    if (dkanan <= 12) {
      setMotor(0, 255);
    }
  }
  //

}
void motorDKiri(int myspeed) {
  if (myspeed >= 0) {
    digitalWrite(motorDKiriB, LOW);
    analogWrite(motorDKiriP, myspeed);
  }
  else {
    int convert = (255 - abs(myspeed));
    digitalWrite(motorDKiriB, HIGH);
    analogWrite(motorDKiriP, convert);
  }
}
void motorDKanan(int myspeed) {
  if (myspeed >= 0) {
    digitalWrite(motorDKananB, LOW);
    analogWrite(motorDKananP, myspeed);
  }
  else {
    int convert = (255 - abs(myspeed));
    digitalWrite(motorDKananB, HIGH);
    analogWrite(motorDKananP, convert);
  }
}
void motorBKiri(int myspeed) {
  if (myspeed >= 0) {
    int convert = 255 - myspeed;
    digitalWrite(motorBKiriB, HIGH);
    analogWrite(motorBKiriP, convert);
  }
  else {
    int convert = abs(myspeed);
    digitalWrite(motorBKiriB, LOW);
    analogWrite(motorBKiriP, convert);
  }
}

void motorBKanan(int myspeed) {
  if (myspeed >= 0) {
    int convert = 255 - myspeed;
    digitalWrite(motorBKananB, HIGH);
    analogWrite(motorBKananP, convert);
  }
  else {
    int convert = abs(myspeed);
    digitalWrite(motorBKananB, LOW);
    analogWrite(motorBKananP, convert);
  }
}
void setMotor(int speedL, int speedR) {

  motorDKiri(speedL);
  motorBKiri(speedL);
  motorDKanan(speedR);
  motorBKanan(speedR);

}
void updateLCD() {
  suhu = mlx.readObjectTempC() + 2;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Last Temp: ");
  lcd.print(suhu);
  lcd.setCursor(0, 1);
  lcd.print("Stop Room: ");
  lcd.print(room);

}
void updateRoom() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Last Temp: ");
  lcd.print(suhu);
  lcd.setCursor(0, 1);
  lcd.print("Stop Room: ");
  lcd.print(room);

}
int getRed() {
  int result;
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  result = pulseIn(outWarna, LOW);
  //  result=map(result,red[0],red[1],255,0);
  //  Serial.println(result);
  return result;
}
int getGreen() {
  int result;
  digitalWrite(s2, HIGH);
  digitalWrite(s3, HIGH);
  result = pulseIn(outWarna, LOW);
  //    result=map(result,green[0],green[1],255,0);
  //  Serial.println(result);
  return result;
}
int getBlue() {
  int result;
  digitalWrite(s2, LOW);
  digitalWrite(s3, HIGH);
  result = pulseIn(outWarna, LOW);
  //  result=map(result,blue[0],blue[1],255,0);

  //  Serial.println(result);
  return result;
}
void calib() {
  lcd.clear();
  int vred, vgreen, vblue;
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  vred = pulseIn(outWarna, LOW);
  delay(100);

  digitalWrite(s2, HIGH);
  digitalWrite(s3, HIGH);
  vgreen = pulseIn(outWarna, LOW);
  delay(100);

  digitalWrite(s2, LOW);
  digitalWrite(s3, HIGH);
  vblue = pulseIn(outWarna, LOW);
  delay(100);

  Serial.print(vred); Serial.print(" ");
  Serial.print(vgreen); Serial.print(" ");
  Serial.print(vblue); Serial.println(" ");
  lcd.setCursor(0, 0);

  lcd.print(vred); lcd.print(",");
  lcd.print(vgreen); lcd.print(",");
  lcd.print(vblue); lcd.print(",");
}

bool isColor(int red, int green, int blue, int dred, int dgreen, int dblue) {

  bool cekRed = false, cekGreen = false, cekBlue = false, result = false;
  //  Serial.print("OK");Serial.println(dred-range);
  //  if(red>=dred-range){
  //    Serial.println("TRUE");
  //

  if (red >= dred - range && red <= dred + range) {
    cekRed = true;
  }
  if (green >= dgreen - range && green <= dgreen + range) {
    cekGreen = true;
  }
  if (blue >= dblue - range && blue <= dblue + range) {
    cekBlue = true;
  }


  //Return
  if (cekRed == true && cekGreen == true && cekBlue == true) {
    result = true;
  }
  else {
    result = false;
  }

  Serial.print(red); Serial.print(" R->"); Serial.println(dred);
  Serial.print(green); Serial.print(" G->"); Serial.println(dgreen);
  Serial.print(blue); Serial.print(" B->"); Serial.println(dblue);
  Serial.print(cekRed);  Serial.print(cekGreen);  Serial.println(cekBlue);
  Serial.print(" Status ->"); Serial.println(result);
  return result;

}

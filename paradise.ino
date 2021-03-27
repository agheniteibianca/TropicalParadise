
#include <Servo.h>
#include <LiquidCrystal.h>
#include <TimeLib.h>

#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 


Servo myservo;
int pos = 0;    // variable to store the servo position

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


void setup() {
  Serial.begin(9600);

  //setup time
  pinMode(13, OUTPUT);
  setSyncProvider( requestSync);  //set function to call when sync required
  Serial.println("Waiting for sync message");

  pinMode(9, OUTPUT); //servo
  pinMode(6, OUTPUT); //led
  pinMode(7, OUTPUT); //pompa
}


void deblocare_dreapta() {
  for ( int i = 0; i < 10; i++) {
    for (pos = 180; pos >= 170; pos -= 5) {
      myservo.write(pos);
      delay(15);
    }
    delay(100);
  }
  myservo.write(180);
}


void deblocare_stanga() {
  for ( int i = 0; i < 10; i++) {
    for (pos = 0; pos <= 10; pos += 5) {
      myservo.write(pos);
      delay(15);
    }

    delay(100);
  }
  myservo.write(0);
}



void ciclu_hranire() {

  //      0 <---- 180
  deblocare_dreapta();
  for (pos = 180; pos >= 0; pos -= 5) {
    myservo.write(pos);
    delay(15);
  }
  delay(1500);


  //     0 ---->  180
  deblocare_stanga();
  for (pos = 0; pos <= 180; pos += 5) {
    myservo.write(pos);
    delay(15);
  }
}




void loop() {
   /*******************************setup time*************************/
  if (Serial.available()) {
    processSyncMessage();
  }
  if (timeStatus() != timeNotSet) {
    digitalClockDisplay();
  }
  if (timeStatus() == timeSet) {
    digitalWrite(13, HIGH); // LED on if synced
  } else {
    digitalWrite(13, LOW);  // LED off if needs refresh
  }
  delay(1000);



  /*******************************display time on lcd*************************/
  lcd.setCursor(4, 1);
  if (hour() < 10) {
    lcd.print("0");
  }
  lcd.print(hour());
  lcd.print(":");
  if (minute() < 10) {
    lcd.print("0");
  }
  lcd.print(minute());
  lcd.print(":");
  if (second() < 10) {
    lcd.print("0");

  }
  lcd.print(second());

  /*******************************set lighting schedule*************************/
  if (hour()>18 && hour()<23) {
    digitalWrite(6, HIGH); //turn led on
  }
  else{
     digitalWrite(6, LOW); //turn led off
  }
  
 /*******************************set feeding schedule*************************/
  if (second() == 10) {

    digitalWrite(7, LOW); //turn pump off
    for (int i = 0; i < 5; i++) {  //shine a signal
      digitalWrite(6, HIGH);
      delay(250);
      digitalWrite(6, LOW);
      delay(250);
    }

    myservo.attach(9);
    lcd.clear();
    lcd.print("hranire...");
    ciclu_hranire();
    myservo.detach();

    lcd.clear();
    lcd.print("Paradis tropical!");
    
    digitalWrite(7, HIGH); //turn pump back on
    digitalWrite(6, HIGH); //turn led back on
  }

}
 /*******************************display time on serial monitor*************************/
void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
}

void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}


void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600;

  if (Serial.find(TIME_HEADER)) {
    pctime = Serial.parseInt();
    if ( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
      setTime(pctime); // Sync Arduino clock to the time received on the serial port
    }
  }
}

time_t requestSync()
{
  Serial.write(TIME_REQUEST);
  return 0; // the time will be sent later in response to serial mesg
}

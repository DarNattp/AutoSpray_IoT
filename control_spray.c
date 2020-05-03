#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#define BLYNK_PRINT Serial

// กำหนดขาใช้งาน
int Relay1 =  D0;
int Relay2 =  D1;

char auth[] = "vbLbcojqgq4n67aga0w-We72NvD7Rtcf";
char ssid[] = "Natapat";
char pass[] = "raydar1234";
//char ssid[] = "P'Mhee";
//char pass[] = "raydar1236548";


BlynkTimer timer;
WidgetRTC rtc;

int nowseconds ;
int startsecondswd = 0;

int timesOfsp = 0;


int numRelay = 0;
// Digital clock display of the time
void clockDisplay()
{
  // You can call hour(), minute(), ... at any time
  // Please see Time library examples for details

  String currentTime = String(hour()) + ":" + minute() + ":" + second();
  String currentDate = String(day()) + "/" + month() + "/" + year();
  Serial.print("Current time: ");
  Serial.print(currentTime);
  Serial.print(" ");
  Serial.println(currentDate);
  //Serial.println(day(weekday())); // 1 is Monday

  // Send time to the App
  Blynk.virtualWrite(V5, currentTime);
  // Send date to the App
  Blynk.virtualWrite(V4, currentDate ); //+ ' ' + dayStr(weekday()) if need date of week
  if(timesOfsp==2400+1){
  timesOfsp = 0; //เมื่อฉีดถึง 2400 ครั้งแล้วให้ reset == 0
  }
  Blynk.virtualWrite(V14, String(timesOfsp) + "/" + 2400); 
}

//ฟังชั่นกดแล้วฉีด
BLYNK_WRITE(V1) {

  Serial.println(param.asInt());
  if (param.asInt()) {
    digitalWrite(Relay1, LOW);
    digitalWrite(Relay2, LOW);
    timesOfsp++;
    delay(1000);
    digitalWrite(Relay1, HIGH);
    digitalWrite(Relay2, HIGH);

  } else {
    digitalWrite(Relay1, HIGH);
    digitalWrite(Relay2, HIGH);

  }
}

void activetoday() {       // check if schedule should run today
  if (year() != 1900) {
    Blynk.syncVirtual(V3);
    Blynk.syncVirtual(V6);
  }
}


//ตั้งเวลาฉีด
BLYNK_WRITE(V3) {
  TimeInputParam t(param);

  // Process start time
  if (t.hasStartTime())
  {
    Serial.println(String("Start: ") +
                   t.getStartHour() + ":" +
                   t.getStartMinute() + ":" +
                   t.getStartSecond());
  }
  else
  {
    // Do nothing
  }

  Serial.println(String("Time zone: ") + t.getTZ());
  nowseconds = ((hour() * 3600) + (minute() * 60) + second());
  startsecondswd = (t.getStartHour() * 3600) + (t.getStartMinute() * 60 + t.getStartSecond());
  Serial.println(nowseconds);
  Serial.println(startsecondswd);
  int dayjustment = -1;
  //check วัน และกำหนดเวลาฉีด
  for (int i = 1; i <= 7; i++) {
    if (t.isWeekdaySelected(i)) {
      if (weekday() == 1) {
        dayjustment = 6;
      }
      Serial.println(String("Day ") + i + " is selected");//SHow i by Monday = 1
      Serial.println(String("Today is ") + int(weekday() + dayjustment));
      if (i == weekday() + dayjustment) {
        if (nowseconds == startsecondswd)
        {
          digitalWrite(Relay1, LOW);
          digitalWrite(Relay2, LOW);
          timesOfsp++;
          delay(1000);
          digitalWrite(Relay1, HIGH);
          digitalWrite(Relay2, HIGH);
        }
      }
    }

  }
}







void setup()
{
  // Debug console
  Serial.begin(115200);

  //กำหนดขา OUTPUT
  pinMode(Relay1, OUTPUT); // กำหนดขาทำหน้าที่ให้ขา D0 เป็น OUTPUT
  pinMode(Relay2, OUTPUT); // กำหนดขาทำหน้าที่ให้ขา D1 เป็น OUTPUT
  digitalWrite(Relay1, HIGH);
  digitalWrite(Relay2, HIGH);

  Blynk.begin(auth, ssid, pass);

  setSyncInterval(10 * 60); // Sync interval in seconds (10 minutes)

  // Display digital clock every 1 seconds
  timer.setInterval(1000L, clockDisplay); // 1000L = 1s

  timer.setInterval(1000L, activetoday);
}




/*BLYNK_CONNECTED() {
  //ให้ซิงค์สถานะของขา Virtual V1
  Blynk.syncVirtual(V1);
  //rtc.begin(); // Synchronize time on connection
  }
*/
BLYNK_CONNECTED() {
  Blynk.syncAll();

  // Synchronize time on connection
  rtc.begin();
}

void loop()
{
  Blynk.run();
  timer.run();

}
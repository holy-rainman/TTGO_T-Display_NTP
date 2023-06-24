#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID   "TMPL6TkQj_281"
#define BLYNK_TEMPLATE_NAME "PROJECT"
#define BLYNK_AUTH_TOKEN    "0maGjV7IW3W3PTlmEqK5j9kluzPO7k4C"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "";
char pass[] = "";
BlynkTimer timer;

#include <ezTime.h>
Timezone local;

#include <TFT_eSPI.h> 
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();

BLYNK_CONNECTED() 
{ Blynk.sendInternal("utc", "time");      
  Blynk.sendInternal("utc", "tz_rule");
  Blynk.sendInternal("rtc", "sync");
  Blynk.syncAll();
  Blynk.syncVirtual(V2,V5);
}

int setMinit;
BLYNK_WRITE(V2)
{ setMinit = param.asInt();
}

BLYNK_WRITE(InternalPinUTC) 
{ String cmd = param[0].asStr();
  if (cmd == "time") 
  { const uint64_t utc_time = param[1].asLongLong();
    UTC.setTime(utc_time / 1000, utc_time % 1000);
  } 
  else if (cmd == "tz_rule") 
  { String tz_rule = param[1].asStr();
    local.setPosix(tz_rule);
  }
}
long ts;
BLYNK_WRITE(InternalPinRTC) { ts = param.asLong(); }

char tx2buf[100],tx2TFT[100];
int once=0;
void localTime() 
{ int jam   = local.hour();
  int minit = local.minute();
  int saat  = local.second();
  int hari  = local.day();
  int bulan = local.month();
  int tahun = local.year();
  
  sprintf(tx2buf,"Time:%d:%d:%d  Day:%d/%d/%d",
          jam,minit,saat,hari,bulan,tahun);
  Serial.println(tx2buf);

  tft.setCursor(0,115,2); tft.setTextSize(1);
  tft.print("Time: ");
  if(jam>=10)   { tft.print(jam);   tft.print(":"); }
  if(jam<10)    { tft.print("0");   tft.print(jam); tft.print(":"); }
  if(minit>=10) { tft.print(minit); tft.print(":"); }
  if(minit<10)  { tft.print("0");   tft.print(minit); tft.print(":"); }
  if(saat>=10)  { tft.print(saat); }
  if(saat<10)   { tft.print("0");   tft.print(saat); }

  tft.print("  Day: ");
  if(hari>=10)  { tft.print(hari);  tft.print("/"); }
  if(hari<10)   { tft.print("0");   tft.print(hari); tft.print("/"); }
  if(bulan>=10) { tft.print(bulan); tft.print("/"); }
  if(bulan<10)  { tft.print("0");   tft.print(bulan); tft.print("/"); }
  tft.print(tahun);
  
  Blynk.virtualWrite(V5,tx2buf);

  tft.setCursor(0,0,2); tft.setTextSize(3);
  if(minit%setMinit==0 && once==0) { once=1; tft.print("ON  "); delay(2000); tft.setCursor(0,0,2); tft.print("OFF"); }
  if(minit%setMinit!=0) { once=0; tft.print("OFF  "); }
}

void setup()
{ tft.init(); tft.setRotation(1); tft.fillScreen(TFT_BLACK); tft.setTextColor(TFT_RED,TFT_BLACK);
  
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000L,localTime);
}

void loop()
{ Blynk.run();
  timer.run();
}

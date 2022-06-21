#include "HT16K33.h"

HT16K33  seg(0x70);
uint32_t start, stop;
uint8_t  ar[4];

//for my esp32 I had to go to File | Preferences, and enter https://dl.espressif.com/dl/package_esp32_index.json in 'Additional Boards Manager URLs'

#include "button.h"

Button buttonA(15);
Button buttonB(4);
Button buttonC(16);

#define TWELVE_HOUR_DISPLAY 1
#define TWENTY_FOUR_HOUR_DISPLAY 2
#define DATE_DISPLAY 3
int currentMode;
int currentBrightness;
int brightnessLabel;

#include <WiFi.h>
#include "time.h"

const char* ssid     = "your_ssid";
const char* password = "your_password";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;
void getTimeFromNTPServer() //assumes Serial.begin(115200); has already been called
{  
  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");

    if (WiFi.status() == WL_NO_SSID_AVAIL)
    {
      Serial.print("No such SSID");
      seg.displayInt(-1);
    }
    else
      seg.displayTime(0, 0, true);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF); 
}

void setup()
{
  Serial.begin(115200);
  seg.begin();
  
  Wire.setClock(100000);
  seg.displayOn();
  seg.brightness(15);
    
  currentBrightness = 15;
  brightnessLabel = 4;
  currentMode = TWENTY_FOUR_HOUR_DISPLAY;

  seg.displayTime(0, 0, true);
  getTimeFromNTPServer();
}

void loop()
{
  //process buttons presses
  if (buttonA.checkState() == BUTTON_DOWN)
  {
    // TODO: what mode is this manual setting?
    Serial.println("I am the unused button");
    seg.displayTime(0, 0, true);
    getTimeFromNTPServer();
  }
  else if (buttonB.checkState() == BUTTON_DOWN)
  {
    if (currentMode == TWELVE_HOUR_DISPLAY)
      currentMode = TWENTY_FOUR_HOUR_DISPLAY;
    else if (currentMode == TWENTY_FOUR_HOUR_DISPLAY)
      currentMode = DATE_DISPLAY;
    else if (currentMode == DATE_DISPLAY)
      currentMode = TWELVE_HOUR_DISPLAY;
  }
  else if (buttonC.checkState() == BUTTON_DOWN)
  {
    if (currentBrightness >= 15)
    {
      currentBrightness = 0;
      brightnessLabel = 1;
    }
    else
    {
      currentBrightness+=5;
      brightnessLabel++;
    }

    seg.brightness(currentBrightness);
    seg.displayClear();
    seg.displayColon(0);
    seg.displayInt(brightnessLabel);
    delay(500);
  }

  //process current state
  if (currentMode == TWELVE_HOUR_DISPLAY or currentMode == TWENTY_FOUR_HOUR_DISPLAY)
    TimeUpdateCheck();
  else
    DateUpdate();
}

unsigned long lastUpdateTime = 0;
const int COLON_DELAY = 1000;
bool isColonDisplayed = true;

void TimeUpdateCheck()
{
  if (currentMode == TWELVE_HOUR_DISPLAY)
    seg.displayTime(get_hour()%12, get_min(), isColonDisplayed); 
  else
    seg.displayTime(get_hour(), get_min(), isColonDisplayed);
  
  if ((millis() - lastUpdateTime) > COLON_DELAY)
  {
    lastUpdateTime = millis();
    if (isColonDisplayed)
      isColonDisplayed = false;
    else
      isColonDisplayed = true;
  }
}

void DateUpdate()
{
  seg.displayDate(get_mday(), get_mon());  
}

int get_hour()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return 0;
  }
  
  return timeinfo.tm_hour;
}

int get_min()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return 0;
  }

  return timeinfo.tm_min;
}

int get_mday()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return 0;
  }

  return timeinfo.tm_mday;
}

int get_mon()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return 0;
  }

  return timeinfo.tm_mon + 1; //mon range is 0..11 so add 1
}


void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
}

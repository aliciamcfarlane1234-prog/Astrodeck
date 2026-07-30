// Libraries and files
#include "TFT_eSPI.h"
#include <Arduino.h>
#include <Wire.h>
#include "DFRobot_DS323X.h"
#include <MoonPhase.hpp>
#include <MoonRise.h>
#include <SkyMap.h>
#include <SiderealPlanets.h>
#include "astronaut1.h"
#include "astronaut2.h"
#include "planet.h"
#include "star.h"
#include "waninggibbous.h"
#include "waxinggibbous.h"
#include "newmoon.h"
#include "fullmoon.h"
#include "firstquarter.h"
#include "thirdquarter.h"
#include "waningcrescent.h"
#include "waxingcrescent.h"

// Modes
#define HomePage 1
#define MoonphasePage 2
#define PlanetPage 3
#define StarPage 4
#define ConditionsPage 5
#define HoroscopePage 6

// Buttons
#define left_button 5
#define right_button 12
#define buzzer 32

MoonPhase moonPhase;
MoonRise mr;
SKYMAP_skymap_t skymap;
struct tm timeinfo{};
double latitude = -37.8136;
double longitude = 144.9631;

const char* getPhaseName(int angle) {
    if (angle == 0) return "New Moon";
    if (angle > 0 && angle < 90) return "Waxing Crescent";
    if (angle == 90) return "First Quarter";
    if (angle > 90 && angle < 180) return "Waxing Gibbous";
    if (angle == 180) return "Full Moon";
    if (angle > 180 && angle < 270) return "Waning Gibbous";
    if (angle == 270) return "Last Quarter";
    return "Waning Crescent";
}

uint16_t astronautPurple = 0x4e69;
uint16_t astronautPink =  0xFDD7;

struct PlanetData {
    bool is_visible;
    double altitude;
    double azimuth;
};

// Declare functions earlier
PlanetData printPlanetStatus(const char* name, double ra, double dec, SKYMAP_observer_position_t &obs, SKYMAP_date_time_values_t &dt);
PlanetData printStarStatus(const char* name, double ra, double dec, SKYMAP_observer_position_t &obs, SKYMAP_date_time_values_t &dt);

// Initialise previous button state (both initially not pressed)
bool LastLeftState = HIGH;
bool LastRightState = HIGH;

bool astronautFrame = false;
bool starFrame = false;

unsigned long astronautTimer = 0;
unsigned long StarAnimationTimer = 0;

int mode = HomePage;
String old_time = "60";
int old_day = 32;
int rand_number = 0;

// Other Stuff
DFRobot_DS323X rtc;

TFT_eSPI tft = TFT_eSPI();  // Invoke custom library with default width and height

void setup() {
  Serial.begin(115200);
  // Initialise all components

  // RTC
  while (rtc.begin() != true) {
    delay(1000);
  }
  
  // LCD Screen
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  // Pushbuttons
  pinMode(left_button, INPUT_PULLUP);
  pinMode(right_button, INPUT_PULLUP);

  // Buzzer
  pinMode(buzzer, OUTPUT);

  // Initial State
  HomescreenMode();

  // Initialise horoscope number;
  int rand_number = 0;
}

void loop() {

  CheckButtons();
  HomePageAnimation();
  StarAnimation();

  // Display time
  String current_time = GetTime();
  if (current_time != old_time) {
    tft.fillRect(200, 290, 280, 30, TFT_BLACK);
    tft.setTextDatum(BR_DATUM);
    tft.setTextSize(2);
    tft.setTextFont(1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(current_time, 470, 310);
    old_time = current_time;
  }

}

void CheckButtons() {
  bool LeftState = digitalRead(left_button);
  bool RightState = digitalRead(right_button);

  // Right Button was just pressed
  if (RightState == LOW && LastRightState == HIGH) {

    // Play button click right sound
    playRight();

    if (mode == HomePage) {
      mode = MoonphasePage;
      MoonphaseMode();
    }

    else if (mode == MoonphasePage) {
      mode = PlanetPage;
      PlanetMode();
    }

    else if (mode == PlanetPage) {
      mode = StarPage;
      StarMode();
    }

    else if (mode == StarPage) {
      mode = ConditionsPage;
      ConditionsMode();
    }

    else if (mode == ConditionsPage) {
      mode = HoroscopePage;
      HoroscopeMode();
    }

    else if (mode == HoroscopePage) {
      mode = HomePage;
      HomescreenMode();
    }

  }

  // Left button was just pressed
  if (LeftState == LOW && LastLeftState == HIGH) {
    
    // Play button click left sound
    playLeft();
    
    if (mode == MoonphasePage) {
      mode = HomePage;
      HomescreenMode();
    }

    else if (mode == PlanetPage) {
      mode = MoonphasePage;
      MoonphaseMode();
    }

    else if (mode == StarPage) {
      mode = PlanetPage;
      PlanetMode();
    }

    else if (mode == ConditionsPage) {
      mode = StarPage;
      StarMode();
    }

    else if (mode == HoroscopePage) {
      mode = ConditionsPage;
      ConditionsMode();
    }

    else if (mode == HomePage) {
      mode = HoroscopePage;
      HoroscopeMode();
    }

  }

  // Update LastLeftState and LastRightState
  LastLeftState = LeftState;
  LastRightState = RightState;
}

void HomePageAnimation() {

  // Only change homescreen animation if currently on homepage
  if (mode != HomePage) {
    return;
  }
  // Switch frame every 500 milliseconds
  if (millis() - astronautTimer >= 500) {

    astronautTimer = millis();

    astronautFrame = !astronautFrame;

    if (astronautFrame == true) {
      tft.pushImage(10, 120, 124, 180, epd_bitmap_astronaut1);
    }
    else {
      tft.pushImage(10, 120, 124, 180, epd_bitmap_astronaut2);
    }
  }

}

void StarAnimation() {

  // Switch frame every 500 milliseconds
  if (millis() - StarAnimationTimer >= 500) {

    StarAnimationTimer = millis();

    starFrame = !starFrame;

    if (starFrame == true) {
      drawStar(35, 30, 3);
      drawStar(90, 18, 2);
      drawStar(155, 38, 4);
      drawStar(225, 20, 3);
      drawStar(300, 35, 6);
      drawStar(375, 18, 2);
      drawStar(445, 40, 4);
    }
    else {
      drawStar(35, 30, 5);
      drawStar(90, 18, 1);
      drawStar(155, 38, 5);
      drawStar(225, 20, 2);
      drawStar(300, 35, 3);
      drawStar(375, 18, 4);
      drawStar(445, 40, 2);
    }
  }

}

void HomescreenMode() {

  // Border
  tft.fillRect(0, 0, 200, 320, TFT_BLACK); // Refresh the page, but don't fill in bottom right corner where time is
  tft.fillRect(200, 0, 280, 290, TFT_BLACK);
  tft.drawRect(142, 80, 328, 160, astronautPurple); // (x1, y1, x length, y length)
  tft.drawRect(143, 81, 326, 158, astronautPurple);
  tft.drawRect(144, 82, 324, 156, astronautPurple);
  tft.fillRect(145, 83, 322, 15, astronautPink);
  tft.drawLine(145, 98, 468, 98, astronautPurple);
  tft.drawLine(145, 99, 468, 99, astronautPurple);
  tft.drawLine(145, 100, 468, 100, astronautPurple);

  // Homepage title
  tft.setCursor(190, 110);
  tft.setTextColor(astronautPink);
  tft.setTextSize(3);
  tft.setTextFont(2);
  tft.print("WELCOME TO"); 
  tft.setCursor(147, 170);
  tft.setTextSize(6);
  tft.setTextFont(1);
  tft.setTextColor(TFT_WHITE);
  tft.print("ASTRODECK");

  // Right arrow
  tft.fillRect(400, 260, 45, 10, astronautPink);
  tft.fillTriangle(445, 245, 470, 265, 445, 285, astronautPink);

  // Left arrow
  tft.fillRect(164, 260, 45, 10, astronautPink);
  tft.fillTriangle(164, 245, 139, 265, 164, 285, astronautPink);

}

void MoonphaseMode() {

  // Border
  tft.fillRect(0, 0, 200, 320, TFT_BLACK); // Refresh the page, but don't fill in bottom right corner where time is
  tft.fillRect(200, 0, 280, 290, TFT_BLACK);
  tft.drawRect(80, 60, 320, 200, astronautPurple);
  tft.drawRect(81, 61, 318, 198, astronautPurple);
  tft.drawRect(82, 62, 316, 196, astronautPurple);

  // Title 
  tft.setTextSize(3);
  tft.setTextFont(1);
  tft.setTextColor(astronautPink);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Moon Status", 240, 80);
  
  // Right arrow
  tft.fillRect(410, 155, 35, 10, astronautPink);
  tft.fillTriangle(445, 140, 470, 160, 445, 180, astronautPink);

  // Left arrow
  tft.fillRect(35, 155, 35, 10, astronautPink);
  tft.fillTriangle(35, 140, 10, 160, 35, 180, astronautPink);

  // Compute 
  timeinfo.tm_year = rtc.getYear() - 1900;  
  timeinfo.tm_mon  = rtc.getMonth() - 1;      
  timeinfo.tm_mday = rtc.getDate();           
  timeinfo.tm_hour = rtc.getHour();          
  timeinfo.tm_min  = rtc.getMinute();        
  timeinfo.tm_sec  = rtc.getSecond();       
  timeinfo.tm_isdst = 0;

  time_t now = mktime(&timeinfo);
  struct timeval tv = { .tv_sec = now, .tv_usec = 0 };
  settimeofday(&tv, NULL);

  setenv("TZ", "AEST-10AEDT,M10.1.0,M4.1.0/3", 1);
  tzset();
  
  mr.calculate(latitude, longitude, now);

  String rise_time = printMelbourneTime(mr.riseTime);
  String set_time = printMelbourneTime(mr.setTime);
  
  moonData_t moon = moonPhase.getPhase();
  const char* phaseName = getPhaseName(moon.angleDeg);
  String currentPhaseText = String(phaseName) + " (" + String(moon.angleDeg) + " degrees)";


  // Print moon info text
  tft.setTextSize(1);
  tft.setTextFont(2);
  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(TC_DATUM);
  tft.drawString(mr.isVisible ? "Moon is currently VISIBLE!" : "Moon is currently NOT Visible :(", 240, 115);
  tft.drawString("Moon Phase: " + currentPhaseText, 240, 150);
  tft.drawString("Moonrise time: " + rise_time, 240, 185);
  tft.drawString("Moonset time: " + set_time, 240, 220);

  // Draw corresponding moonphase image
  if (phaseName == "New Moon") {
    tft.pushImage(10, 190, 140, 120, epd_bitmap_newmoon);
  }
  else if (phaseName == "Waxing Crescent") {
    tft.pushImage(10, 190, 140, 120, epd_bitmap_waxingcrescent);
  }
  else if (phaseName == "First Quarter") {
    tft.pushImage(10, 190, 140, 120, epd_bitmap_firstquarter);
  }
  else if (phaseName == "Waxing Gibbous") {
    tft.pushImage(10, 190, 140, 120, epd_bitmap_waxinggibbous);
  }
  else if (phaseName == "Full Moon") {
    tft.pushImage(10, 190, 140, 120, epd_bitmap_fullmoon);
  }
  else if (phaseName == "Waning Gibbous") {
    tft.pushImage(10, 190, 140, 120, epd_bitmap_waninggibbous);
  }
  else if (phaseName == "Last Quarter") {
    tft.pushImage(10, 190, 140, 120, epd_bitmap_thirdquarter);
  }
  else if (phaseName == "Waning Crescent") {
    tft.pushImage(10, 190, 140, 120, epd_bitmap_waningcrescent);
  }
}

void PlanetMode() {

  // Border
  tft.fillRect(0, 0, 200, 320, TFT_BLACK); // Refresh the page, but don't fill in bottom right corner where time is
  tft.fillRect(200, 0, 280, 290, TFT_BLACK);
  tft.drawRect(80, 60, 320, 200, astronautPurple);
  tft.drawRect(81, 61, 318, 198, astronautPurple);
  tft.drawRect(82, 62, 316, 196, astronautPurple);

  // Title
  tft.setTextSize(3);
  tft.setTextFont(1);
  tft.setTextColor(astronautPink);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Planet Visibility", 240, 80);

  // Right arrow
  tft.fillRect(410, 155, 35, 10, astronautPink);
  tft.fillTriangle(445, 140, 470, 160, 445, 180, astronautPink);

  // Left arrow
  tft.fillRect(35, 155, 35, 10, astronautPink);
  tft.fillTriangle(35, 140, 10, 160, 35, 180, astronautPink);

  SKYMAP_observer_position_t observation_location;
  observation_location.latitude = latitude;
  observation_location.longitude = longitude;

  int utc_year, utc_mon, utc_day;
  int utc_min;
  double utc_hour;

  melbourneTimetoUTCTime(utc_year, utc_mon, utc_day, utc_hour);
  
  SKYMAP_date_time_values_t dt;
  dt.year = utc_year;
  dt.month = utc_mon;
  dt.day = utc_day;
  utc_min = rtc.getMinute();
  dt.hour = utc_hour + ((double)utc_min/60);

  SiderealPlanets myPlanet;
  myPlanet.begin();
  myPlanet.setTimeZone(0);
  myPlanet.rejectDST();
  myPlanet.setGMTdate(utc_year, utc_mon, utc_day);
  myPlanet.setGMTtime(utc_hour, utc_min,0);

  // Obtain visibility, altitude and azimuth
  myPlanet.doMercury(); 
  PlanetData Mercury = printPlanetStatus("Mercury", myPlanet.getRAdec(), myPlanet.getDeclinationDec(), observation_location, dt);
  myPlanet.doVenus(); 
  PlanetData Venus = printPlanetStatus("Venus", myPlanet.getRAdec(), myPlanet.getDeclinationDec(), observation_location, dt);
  myPlanet.doMars(); 
  PlanetData Mars = printPlanetStatus("Mars", myPlanet.getRAdec(), myPlanet.getDeclinationDec(), observation_location, dt);
  myPlanet.doJupiter(); 
  PlanetData Jupiter = printPlanetStatus("Jupiter", myPlanet.getRAdec(), myPlanet.getDeclinationDec(), observation_location, dt);
  myPlanet.doSaturn(); 
  PlanetData Saturn = printPlanetStatus("Saturn", myPlanet.getRAdec(), myPlanet.getDeclinationDec(), observation_location, dt);

  // Print planet info text
  tft.setTextSize(1);
  tft.setTextFont(2);
  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(TL_DATUM);
  tft.drawString("Mercury", 100, 125);
  tft.drawString("Venus", 100, 150);
  tft.drawString("Mars", 100, 175);
  tft.drawString("Jupiter", 100, 200);
  tft.drawString("Saturn", 100, 225);

  tft.setTextDatum(TC_DATUM);
  if (Mercury.is_visible) {
    tft.drawString("Yes", 195, 125);
  }
  else {
    tft.drawString("No", 195, 125);
  }
  if (Venus.is_visible) {
    tft.drawString("Yes", 195, 150);
  }
  else {
    tft.drawString("No", 195, 150);
  }
  if (Mars.is_visible) {
    tft.drawString("Yes", 195, 175);
  }
  else {
    tft.drawString("No", 195, 175);
  }
  if (Jupiter.is_visible) {
    tft.drawString("Yes", 195, 200);
  }
  else {
    tft.drawString("No", 195, 200);
  }
  if (Saturn.is_visible) {
    tft.drawString("Yes", 195, 225);
  }
  else {
    tft.drawString("No", 195, 225);
  }

  tft.drawString("Altitude:", 260, 107);
  tft.drawString("Azimuth:", 340, 107);

  tft.drawString(String(Mercury.altitude, 1), 260, 125);
  tft.drawString(String(Venus.altitude, 1), 260, 150);
  tft.drawString(String(Mars.altitude, 1), 260, 175);
  tft.drawString(String(Jupiter.altitude, 1), 260, 200);
  tft.drawString(String(Saturn.altitude, 1), 260, 225);

  tft.drawString(String(Mercury.azimuth, 1), 340, 125);
  tft.drawString(String(Venus.azimuth, 1), 340, 150);
  tft.drawString(String(Mars.azimuth, 1), 340, 175);
  tft.drawString(String(Jupiter.azimuth, 1), 340, 200);
  tft.drawString(String(Saturn.azimuth, 1), 340, 225);

  // Planet image
  tft.pushImage(5, 223, 93, 87, epd_bitmap_planet);

}

void StarMode() {

  // Border
  tft.fillRect(0, 0, 200, 320, TFT_BLACK); // Refresh the page, but don't fill in bottom right corner where time is
  tft.fillRect(200, 0, 280, 290, TFT_BLACK);
  tft.drawRect(80, 60, 320, 200, astronautPurple);
  tft.drawRect(81, 61, 318, 198, astronautPurple);
  tft.drawRect(82, 62, 316, 196, astronautPurple);

  // Title
  tft.setTextSize(3);
  tft.setTextFont(1);
  tft.setTextColor(astronautPink);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Star Visibility", 240, 80);

  // Right arrow
  tft.fillRect(410, 155, 35, 10, astronautPink);
  tft.fillTriangle(445, 140, 470, 160, 445, 180, astronautPink);

  // Left arrow
  tft.fillRect(35, 155, 35, 10, astronautPink);
  tft.fillTriangle(35, 140, 10, 160, 35, 180, astronautPink);

  SKYMAP_observer_position_t observation_location;
  observation_location.latitude = latitude;
  observation_location.longitude = longitude;

  int utc_year, utc_mon, utc_day;
  int utc_min;
  double utc_hour;

  melbourneTimetoUTCTime(utc_year, utc_mon, utc_day, utc_hour);
  
  SKYMAP_date_time_values_t dt;
  dt.year = utc_year;
  dt.month = utc_mon;
  dt.day = utc_day;
  utc_min = rtc.getMinute();
  dt.hour = utc_hour + ((double)utc_min/60);

  SiderealPlanets myPlanet;
  myPlanet.begin();
  myPlanet.setTimeZone(0);
  myPlanet.rejectDST();
  myPlanet.setGMTdate(utc_year, utc_mon, utc_day);
  myPlanet.setGMTtime(utc_hour, utc_min,0);

  // Obtain visibility, altitude and azimuth
  PlanetData Lyra = printStarStatus("Lyra", 285, 40, observation_location, dt);
  PlanetData Antares = printStarStatus("Antares", 247.36, -26.43, observation_location, dt);
  PlanetData SouthernCross = printStarStatus("Southern Cross", 187.5, -60, observation_location, dt);
  PlanetData AlphaCentaurus = printStarStatus("Alpha Centaurus", 219.90, -60.83, observation_location, dt);
  PlanetData Achernar = printStarStatus("Achernar", 24.428, -57.237, observation_location, dt);
  PlanetData Sirius = printStarStatus("Sirius", 101.52, -16.742, observation_location, dt);

  // Print star info text
  tft.setTextSize(1);
  tft.setTextFont(2);
  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(TL_DATUM);
  tft.drawString("Lyra", 100, 110);
  tft.drawString("Antares", 100, 135);
  tft.drawString("Southern Cross", 100, 160);
  tft.drawString("Alpha Centaurus", 100, 185);
  tft.drawString("Achernar", 100, 210);
  tft.drawString("Sirius", 100, 235);

  tft.setTextDatum(TC_DATUM);
  if (Lyra.is_visible) {
    tft.drawString("Yes", 225, 110);
  }
  else {
    tft.drawString("No", 225, 110);
  }
  if (Antares.is_visible) {
    tft.drawString("Yes", 225, 135);
  }
  else {
    tft.drawString("No", 225, 135);
  }
  if (SouthernCross.is_visible) {
    tft.drawString("Yes", 225, 160);
  }
  else {
    tft.drawString("No", 225, 160);
  }
  if (AlphaCentaurus.is_visible) {
    tft.drawString("Yes", 225, 185);
  }
  else {
    tft.drawString("No", 225, 185);
  }
  if (Achernar.is_visible) {
    tft.drawString("Yes", 225, 210);
  }
  else {
    tft.drawString("No", 225, 210);
  }
  if (Sirius.is_visible) {
    tft.drawString("Yes", 225, 235);
  }
  else {
    tft.drawString("No", 225, 235);
  }

  tft.drawString(String(Lyra.azimuth, 1) + " " + "degrees" + " " + giveDirection(Lyra.azimuth), 310, 110);
  tft.drawString(String(Antares.azimuth, 1) + " " + "degrees" +  " " + giveDirection(Antares.azimuth), 310, 135);
  tft.drawString(String(SouthernCross.azimuth, 1) + " " + "degrees" + " " + giveDirection(SouthernCross.azimuth), 310, 160);
  tft.drawString(String(AlphaCentaurus.azimuth, 1) + " " + "degrees" + " " + giveDirection(AlphaCentaurus.azimuth), 310, 185);
  tft.drawString(String(Achernar.azimuth, 1) + " " + "degrees" + " " + giveDirection(Achernar.azimuth), 310, 210);
  tft.drawString(String(Sirius.azimuth, 1) + " " + "degrees" + " " + giveDirection(Sirius.azimuth), 310, 235);

  // Print image
  tft.pushImage(10, 224, 90, 86, epd_bitmap_star);

}

void ConditionsMode() {

  // Border
  tft.fillRect(0, 0, 200, 320, TFT_BLACK); // Refresh the page, but don't fill in bottom right corner where time is
  tft.fillRect(200, 0, 280, 290, TFT_BLACK);
  tft.drawRect(80, 60, 320, 200, astronautPurple);
  tft.drawRect(81, 61, 318, 198, astronautPurple);
  tft.drawRect(82, 62, 316, 196, astronautPurple);

  // Right arrow
  tft.fillRect(410, 155, 35, 10, astronautPink);
  tft.fillTriangle(445, 140, 470, 160, 445, 180, astronautPink);

  // Left arrow
  tft.fillRect(35, 155, 35, 10, astronautPink);
  tft.fillTriangle(35, 140, 10, 160, 35, 180, astronautPink);

  // Title
  tft.setTextSize(3);
  tft.setTextFont(1);
  tft.setTextColor(astronautPink);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Conditions", 240, 80);

  // Temperature text
  float temp = rtc.getTemperatureC();
  tft.setTextSize(1);
  tft.setTextFont(2);
  tft.setTextDatum(TC_DATUM);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("The temperature is: " + String(temp, 1) + "C", 240, 120);

  // Sunrise and sunset calculations
  SKYMAP_observer_position_t observation_location;
  observation_location.latitude = latitude;
  observation_location.longitude = longitude;

  int utc_year, utc_mon, utc_day;
  int utc_min;
  double utc_hour;

  melbourneTimetoUTCTime(utc_year, utc_mon, utc_day, utc_hour);
  
  SKYMAP_date_time_values_t dt;
  dt.year = utc_year;
  dt.month = utc_mon;
  dt.day = utc_day;
  utc_min = rtc.getMinute();
  dt.hour = utc_hour + ((double)utc_min/60);

  SiderealPlanets myPlanet;
  myPlanet.begin();
  myPlanet.setTimeZone(0);
  myPlanet.rejectDST();
  myPlanet.setGMTdate(utc_year, utc_mon, utc_day);
  myPlanet.setGMTtime(utc_hour, utc_min,0);
  
  SiderealPlanets mySun;
  mySun.begin();
  mySun.rejectDST();
  mySun.setTimeZone(10);
  mySun.setLatLong(latitude, longitude);

  mySun.setGMTdate(utc_year, utc_mon, utc_day);
  mySun.setLocalTime(rtc.getHour(),rtc.getMinute(),rtc.getSecond());
  mySun.doSunRiseSetTimes();
  mySun.doSun();
  mySun.printDegMinSecs(mySun.getSunriseTime());
  mySun.printDegMinSecs(mySun.getSunsetTime());
  double sunrise_time = mySun.getSunriseTime();
  double sunset_time = mySun.getSunsetTime();
  String sunriseTime = String(int(sunrise_time)) + ":" + String(int(round((sunrise_time - int(sunrise_time))*60)));
  String sunsetTime = String(int(sunset_time)) + ":" + String(int(round((sunset_time - int(sunset_time))*60)));
  
  // Print sunrise and sunset time
  tft.drawString("Sunrise time: " + sunriseTime, 240, 150);
  tft.drawString("Sunset time: " + sunsetTime, 240, 180);

  // See if it's light or dark outside
  double current_time = rtc.getHour() + (rtc.getMinute()/60.0);
  if (sunrise_time < current_time && current_time < sunset_time) {
    tft.drawString("The sun is out!! Go touch some grass :)", 240, 210);
  }
  else {
    tft.drawString("It's dark outside!! Let's go stargazing :)", 240, 210);
  }

}

void HoroscopeMode() {

  // Border
  tft.fillRect(0, 0, 200, 320, TFT_BLACK); // Refresh the page, but don't fill in bottom right corner where time is
  tft.fillRect(200, 0, 280, 290, TFT_BLACK);
  tft.drawRect(80, 60, 320, 200, astronautPurple);
  tft.drawRect(81, 61, 318, 198, astronautPurple);
  tft.drawRect(82, 62, 316, 196, astronautPurple);

  // Title
  tft.setTextSize(3);
  tft.setTextFont(1);
  tft.setTextColor(astronautPink);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Daily Horoscope", 240, 80);

  // Right arrow
  tft.fillRect(410, 155, 35, 10, astronautPink);
  tft.fillTriangle(445, 140, 470, 160, 445, 180, astronautPink);

  // Left arrow
  tft.fillRect(35, 155, 35, 10, astronautPink);
  tft.fillTriangle(35, 140, 10, 160, 35, 180, astronautPink);

  // Check if it's a new day for daily horoscope message
  int current_day = rtc.getDate();
  if (current_day != old_day) {
    rand_number = random(5);
    old_day = current_day;
  } 

  // // Randomly generate a daily message from one of the Cody Crafters
  tft.setTextSize(1);
  tft.setTextFont(2);
  tft.setTextDatum(TC_DATUM);
  tft.setTextColor(TFT_WHITE);
  if (rand_number == 0) {
    tft.drawString("Today you align most with Alicia.", 240, 120);
    tft.drawString("Alicia's advice for today is:", 240, 150);
    int rand = random(3);
    if (rand == 0) {
      tft.drawString("You should go and touch grass today!", 240, 180);
    }
    else if (rand == 1) {
      tft.drawString("Remember, it's the journey, not the destination.", 240, 180);
      tft.drawString("Be easy on yourself and have fun!", 240, 210);
    }
    else if (rand == 2) {
      tft.drawString("Don't forget to add a little whimsy to your life!", 240, 180);
    }
  }
  else if (rand_number == 1) {
    tft.drawString("Today you align most with Hsien-wei.", 240, 120);
    tft.drawString("Hsien-wei's advice for today is:", 240, 150);
    tft.drawString("After the rain comes the rainbow!", 240, 180);
  }
  else if (rand_number == 2) {
    tft.drawString("Today you align most with Chantelle.", 240, 120);
    tft.drawString("Chantelle's advice for today is:", 240, 150);
    int rand = random(3);
    if (rand == 0) {
      tft.drawString("Fate shines upon you today!", 240, 180);
      tft.drawString("Or perhaps its the stars!", 240, 210);
    }
    else if (rand == 1) {
      tft.drawString("SUPER MEGA GOODLUCK!!!", 240, 180);
      tft.drawString("I envision positive gacha pulls in your future!", 240, 210);
    }
    else if (rand == 2) {
      tft.drawString("Get a breath of fresh air", 240, 180);
      tft.drawString("and watch the sunset tonight!", 240, 210);
    }
  }
  else if (rand_number == 3) {
    tft.drawString("Today you align most with Rachel.", 240, 120);
    tft.drawString("Rachel's advice for today is:", 240, 150);
    tft.drawString("Life is an adventure.", 240, 180);
    tft.drawString("Take risks and go outside!", 240, 210);
  }
  else if (rand_number == 4) {
    tft.drawString("Today you align most with Caitlin.", 240, 120);
    tft.drawString("Caitlin's advice for today is:", 240, 150);
    int rand = random(2);
    if (rand == 0) {
      tft.drawString("Turn it off then turn it back on again.", 240, 180);
    }
    else if (rand == 1) {
      tft.drawString("When in doubt, have a nap.", 240, 180);
    }
  }

}

// Function to draw stars in the background
void drawStar(int x, int y, int length) {

  // Max star length will be 7
  int max_length = 7;

  //Erase old star
  tft.fillRect(x - max_length, y - max_length, 2*max_length, 2*max_length, TFT_BLACK);

  // Draw new star
  tft.drawLine(x - length, y, x + length, y, TFT_WHITE); // Horizontal line
  tft.drawLine(x, y - length, x, y + length, TFT_WHITE); // Vertical line
  tft.drawLine(x - length/2, y - length/2, x + length/2, y + length/2, TFT_WHITE); // Positive gradient diagonal line
  tft.drawLine(x - length/2, y + length/2, x + length/2, y - length/2, TFT_WHITE); // Negative gradient diagonal line
}


String GetTime() {
  return static_cast<String>(rtc.getDate()) + "/" + static_cast<String>(rtc.getMonth()) + "/" + static_cast<String>(rtc.getYear()) + 
  " " + static_cast<String>(rtc.getHour()) + ":" + static_cast<String>(rtc.getMinute()) + ":" + static_cast<String>(rtc.getSecond());
}

// Coverts Unix time to Melbourne time
String printMelbourneTime(time_t unixTime){
    setenv("TZ", "AEST-10AEDT,M10.1.0,M4.1.0/3", 1);
    tzset();
    struct tm *melbourne = localtime(&unixTime);
    return static_cast<String>(melbourne->tm_mday) + "/" + static_cast<String>(melbourne->tm_mon + 1)
    + " " + static_cast<String>(melbourne->tm_hour) + ":" + static_cast<String>(melbourne->tm_min);
}

void melbourneTimetoUTCTime(int &utc_year, int &utc_mon, int &utc_day, double &utc_hour) {
    int melb_year = rtc.getYear();
    int melb_mon = rtc.getMonth();
    int melb_day = rtc.getDate();
    int melb_hour = rtc.getHour();

    utc_year = melb_year;
    utc_mon = melb_mon;
    utc_day = melb_day;

    if (melb_hour >= 10) {
        utc_hour = melb_hour - 10;
    } else {
        // If subtracting 10 drops below 0, wrap around to the previous day
        utc_hour = melb_hour - 10 + 24;
        utc_day = melb_day - 1;
    }
}

PlanetData printPlanetStatus(const char* name, double ra, double dec, SKYMAP_observer_position_t &obs, SKYMAP_date_time_values_t &dt) {
    PlanetData data; // Create struct instance
    
    SKYMAP_star_t planetObj;
    planetObj.right_ascension = ra * 15.0;
    planetObj.declination = dec;
    
    skymap.observer_position = obs;
    skymap.date_time = dt;
    skymap.object_to_search = planetObj;
    
    SKYMAP_search_result_t search_result = SKYMAP_observe_object(&skymap);
    
    // Store values inside our struct
    data.is_visible = SKYMAP_is_object_visible(&search_result);
    data.azimuth    = search_result.azimuth;
    data.altitude   = search_result.altitude;

    return data; // Return all 3 values
}

PlanetData printStarStatus(const char* name, double ra, double dec, SKYMAP_observer_position_t &obs, SKYMAP_date_time_values_t &dt) {
    PlanetData data; // Create struct instance
    
    SKYMAP_star_t starObj;
    starObj.right_ascension = ra; //does not times by 15 as everything is in degrees
    starObj.declination = dec;
    
    skymap.observer_position = obs;
    skymap.date_time = dt;
    skymap.object_to_search = starObj;
    
    SKYMAP_search_result_t search_result = SKYMAP_observe_object(&skymap);
    
    // Store values inside our struct
    data.is_visible = SKYMAP_is_object_visible(&search_result);
    data.azimuth    = search_result.azimuth;
    data.altitude   = search_result.altitude;

    return data; // Return all 3 values
}

const char* giveDirection(float azimuth)
{
    if (azimuth >= 337.5 || azimuth < 22.5)
        return "N";

    if (azimuth >= 22.5 && azimuth < 67.5)
        return "NE";

    if (azimuth >= 67.5 && azimuth < 112.5)
        return "E";

    if (azimuth >= 112.5 && azimuth < 157.5)
        return "SE";

    if (azimuth >= 157.5 && azimuth < 202.5)
        return "S";

    if (azimuth >= 202.5 && azimuth < 247.5)
        return "SW";

    if (azimuth >= 247.5 && azimuth < 292.5)
        return "W";

    return "NW";
}

// Buzzer functions
void playTone(int frequency, int duration) {
  tone(buzzer, frequency, duration);
}

void playRight() {
  playTone(523, 120);
  playTone(659, 120);
} 

void playLeft() {
  playTone(659, 120);
  playTone(523, 120);
}


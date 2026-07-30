# Astrodeck

This repository includes code for the Astrodeck, an astronomy-themed cyberdeck built by the Crafty Coders team during the Hardwired Hackathon hosted by MUEEC and CISSA.

The Astrodeck runs on an ESP32 and displays up-to-date astronomy information when pages are opened on a 4" ILI9488 SPI TFT display (480x320 resolution). Navigation is handled using two push buttons (left and right), with audio feedback from a passive buzzer. A DFRobot Fermion DS3231 RTC module provides accurate timekeeping, and the project is housed in a custom 3D-printed case.

### Features
- Persistent date and time display across all pages
- Home screen with a title and animated astronaut
- Moon Status page displaying current moon phase (with matching moon image), moonrise time and moonset time
- Planet Visibility page displaying the visibility status of 5 planets, plus their altitudes and azimuths.
- Star Visibility page displaying the visibility status of 6 stars and their direction in the sky
- Conditions page showing the temperature, sunrise time and sunset time
- Horoscope page featuring a new message of advice each day

### Hardware
- ESP32
- 4" ILI9488 SPI TFT LCD
- DFRobot Fermion DS33231 RTC module
- 2 push buttons
- Passive buzzer
- Connecting wires
- Optional 3D-printed enclosure

### Graphics
The interface includes:

- Eight moon phase images that update based on the current moon phase
- Star animations built from lines
- Astronaut animation frames
- Planet and star images

All images are stored as RGB565 data in .h files for use with the TFT display.

### Audio
The passive buzzer plays different tones depending on whether the left or right pushbutton is pressed.

### Libraries
This project makes use of several open-source libraries for astronomical calculations, including:
- MoonPhase
- MoonRIse
- SkyMap
- SiderealPlanets

Additional libraries are used for interfacing the display and controlling hardware, including:
- TFT_eSPI
- DFRobot_DS323X

### Setup
1. Install the required external libraries.
2. Install the ESP32 board packagee in Arduino IDE.
3. Add the included 'User_Setup' file to the TFT_eSPI library folder.
4. Upload the code to an ESP32 board.





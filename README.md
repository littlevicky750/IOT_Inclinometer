# IOT_Spirit_Level

If compile with Arduino 2.0  
-install library list in platformio.ini  
-install ALL library in /lib

For PCB V6
# Improve
- Function to calibrate Z-axis only.
- Used IMU temperature for IMU warm-up detection.
- LED will flash when collecting calibration data.
- Print Mac address to serial (and print to debug file when in expert mode) before wi-fi starts.
- Save more info to SD card.
- Save debug info to debug file while in expert mode.
- Start calculating time out after imu warm-up.
# Rewrite
- IMU calibration data collection.
- IMU calibration button interact.
- Main page button interact.
# Debug
- OLED menu SD icon show.
- Local clock time reset.

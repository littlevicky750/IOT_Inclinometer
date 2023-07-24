# IOT_Spirit_Level

UI Change.
- Relative logic change in AngleCalaulate.h.
- Relative output format change in IMU42688.h.
- Relative page ID change.
- Relative SD saving format change.
- Relative Wi-Fi sending format change.

Change display unit setting method and logic.
- Add save unit setting function in IMU42688.h.
- Add convert angle float into a string with the current unit in IMU42688.h.

Add display IMU's X, Y, Z, and T page
- Relative output format change in IMU42688.h.

Change sensor temperature Butterworth filter coefficient.
Add temperature stability test in warm-up justification. 
Add print the Wi-Fi Error code.
Add print resume IMU's calibrated parameter to SD Debug file function.

Debug
- Fix the LED flash error when pressing Button 0.
- Fix occasionally NTP time zone errors.

Hardware change.
- Power on logic change according to hardware change.

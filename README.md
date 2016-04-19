# GPSDog
Arduino 3G / GPS watch guard for movable objects

## Password
Password size is max 8 and all ASCII chars without space.

## SMS commands

- ```INIT password number sign ON/OFF```
- ```RESET password```
- ```STATUS```
- ```SET INTERVAL min```
- ```SET FORWARD idx```
- ```SET GEOFIX val```
- ```SET UNIT KMH/MPH```
- ```STORE idx ADD number sign ON/OFF```
- ```STORE idx DEL```
- ```STORE idx SHOW```
- ```WATCH ON/OFF/?```
- ```PROTECT ON/OFF/?```
- ```ALARM ON/OFF/?```
- ```FORWARD ON/OFF/?```
- ```STOP```
- ```VERSION```

# Hardware

- Arduino uno r3
- 3G / GPS Shield from Cooking-Hacks: https://www.cooking-hacks.com/3g-gprs-shield-for-arduino-3g-gps
- Step-Down power supply: https://www.itead.cc/lm2596-dc-dc-buck-converter-step-down-power-module-output-1-25v-35v.html


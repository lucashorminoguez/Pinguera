# PINGUERA

A simple app built in Arduino IDE for ESP8266 to verify the connection with different hosts.
- Displays the process on an LCD 16x2 and 7 LEDs.
- A message is sent when an error appears through WhatsApp using CallMeBot.

---

## CIRCUIT DESIGN : 

![protoboard](https://github.com/lucashorminoguez/Pinguera/blob/main/resources/protoboard.png)

### RESTRICTED PINS:
* D8 must be LOW when initiate
* D3,D4 must be on HIGH when initiate that's why they have a different logic 

---

## CONFIG.H :

---

## Future Work :
- Modularize the code (separating files)
- Create config.h with #defines to make changes easy
- Put more than one number phone
- Design a best version of the prototype board

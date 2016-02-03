# Jeelink Chacon
Arduino sketch to send OOK messages to control Chacon power plugs over 433MHz RF using a Jeelink.

## Usage:
* Open a serial connection to the Jeelink (baudrate 9600)
* send a message of format `x,y,z k` to the jeelink to control device y of group x. z can be 0 or 1, indicating whether to turn the power plug on or off.
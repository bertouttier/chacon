// This example is based in JCW's KAKU RF sockets control code
// 2009-02-21 jc@wippler.nl http://opensource.org/licenses/mit-license.php
 
// Note that 868 MHz RFM12B's can send 433 MHz just fine, even though the RF
// circuitry is presumably not optimized for that band. Maybe the range will
// be limited, or maybe it's just because 868 is nearly a multiple of 433 ?
 
#include <JeeLib.h>
#include <util/parity.h>

static byte value, stack[RF12_MAXDATA+4], top;
 
// Turn transmitter on or off, but also apply asymmetric correction and account
// for 25 us SPI overhead to end up with the proper on-the-air pulse widths.
// With thanks to JGJ Veken for his help in getting these values right.
static void ookPulse(int on, int off) {
    rf12_onOff(1);
    delayMicroseconds(on + 150);
    rf12_onOff(0);
    delayMicroseconds(off - 200);
}
 
static unsigned long getCommand(int addr, int device, int on) {
  unsigned long addr2;
  unsigned long device2;
  unsigned long on2;
  unsigned long cmd = 0;
  
  switch(addr) {
    case 1:
      addr2 = 0b10101000;
      break;
    case 2:
      addr2 = 0b10100010;
      break;
    case 3:
      addr2 = 0b10001010;
      break;
    case 4:
      addr2 = 0b00101010;
      break;
  }
  
  switch(device) {
    case 1:
      device2 = 0b10101000;
      break;
    case 2:
      device2 = 0b10100010;
      break;
    case 3:
      device2 = 0b10001010;
      break;
    case 4:
      device2 = 0b00101010;
      break;
  }
  
  if(on) {
    on2 = 0b10101010;
  }else{
    on2 = 0b00101010;
  }
  
  cmd = on2 << 16 | device2 << 8 | addr2;
  return cmd;
} 

static void OOKSend(char addr, char device, char on) {
  unsigned long cmd = 0;
  cmd = getCommand(addr, device, on);
  
  for (byte i = 0; i < 10; ++i) {
      for (byte bit = 0; bit < 25; ++bit) {
          int on = bitRead(cmd, bit) ? 1056 : 395;
          ookPulse(on, 1313 - on);
      }
      delay(10); // approximate
  }
}

static void handleInput (char c) {
  if ('0' <= c && c <= '9')
    value = 10 * value + c - '0';
  else if (c == ',') {
    if (top < sizeof stack)
      stack[top++] = value;
    value = 0;
  } else if ('a' <= c && c <='z') {
    Serial.print("> ");
    for (byte i = 0; i < top; ++i) {
      Serial.print((int) stack[i]);
      Serial.print(',');
    }
    Serial.print((int) value);
    Serial.println(c);
    switch (c) {
      default:
        Serial.println("Error. Try <addr>,<device>,<on> k");
        break;
      case 'k': // send KAKU command: <addr>,<dev>,<on>k
        OOKSend(stack[0], stack[1], value);
        break;
    }
    value = top = 0;
    memset(stack, 0, sizeof stack);
  } else if (' ' < c && c < 'A')
    Serial.println("Error. Try <addr>,<device>,<on> k");
}



void setup() {
    Serial.begin(9600);
    Serial.println("\n[OOK_RF sockets]");
    rf12_initialize(0, RF12_433MHZ);
}

void loop() {
  if (Serial.available())
    handleInput(Serial.read());
}

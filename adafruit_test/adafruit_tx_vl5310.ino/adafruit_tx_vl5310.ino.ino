/* RFM69 library and code by Felix Rusu - felix@lowpowerlab.com
  // Get libraries at: https://github.com/LowPowerLab/
  // Make sure you adjust the settings in the configuration section below !!!
  // **********************************************************************************
  // Copyright Felix Rusu, LowPowerLab.com
  // Library and code by Felix Rusu - felix@lowpowerlab.com
  // **********************************************************************************
  // License
  // **********************************************************************************
  // This program is free software; you can redistribute it
  // and/or modify it under the terms of the GNU General
  // Public License as published by the Free Software
  // Foundation; either version 3 of the License, or
  // (at your option) any later version.
  //
  // This program is distributed in the hope that it will
  // be useful, but WITHOUT ANY WARRANTY; without even the
  // implied warranty of MERCHANTABILITY or FITNESS FOR A
  // PARTICULAR PURPOSE. See the GNU General Public
  // License for more details.
  //
  // You should have received a copy of the GNU General
  // Public License along with this program.
  // If not, see <http://www.gnu.org/licenses></http:>.
  //
  // Licence can be viewed at
  // http://www.gnu.org/licenses/gpl-3.0.txt
  //
  // Please maintain this license information along with authorship
  // and copyright notices in any redistribution of this code
  // **********************************************************************************/

#include <RFM69.h>    //get it here: https://www.github.com/lowpowerlab/rfm69
//#include <SPI.h>
#include <Wire.h>
#include <VL53L0X.h>
VL53L0X sensor;
#define MAX_DISTANCE 950

int fill_lvl, per_fl = 0;
//*********************************************************************************************
// *********** IMPORTANT SETTINGS - YOU MUST CHANGE/ONFIGURE TO FIT YOUR HARDWARE *************
//*********************************************************************************************
#define NETWORKID     100  // The same on all nodes that talk to each other
#define NODEID        2    // The unique identifier of this node
#define RECEIVER      1    // The recipient of packets

//Match frequency to the hardware version of the radio on your Feather
//#define FREQUENCY     RF69_433MHZ
//#define FREQUENCY     RF69_868MHZ
#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HCW   true // set to 'true' if you are using an RFM69HCW module
#define VBATPIN A9

//*********************************************************************************************
#define SERIAL_BAUD   115200

// for Feather 32u4 Radio
#define RFM69_CS      8
#define RFM69_IRQ     7
#define RFM69_IRQN    4  // Pin 7 is IRQ 4!
#define RFM69_RST     4


/* for Feather M0 Radio
  #define RFM69_CS      8
  #define RFM69_IRQ     3
  #define RFM69_IRQN    3  // Pin 3 is IRQ 3!
  #define RFM69_RST     4
*/

/* ESP8266 feather w/wing
  #define RFM69_CS      2
  #define RFM69_IRQ     15
  #define RFM69_IRQN    digitalPinToInterrupt(RFM69_IRQ )
  #define RFM69_RST     16
*/

/* Feather 32u4 w/wing
  #define RFM69_RST     11   // "A"
  #define RFM69_CS      10   // "B"
  #define RFM69_IRQ     2    // "SDA" (only SDA/SCL/RX/TX have IRQ!)
  #define RFM69_IRQN    digitalPinToInterrupt(RFM69_IRQ )
*/

/* Feather m0 w/wing
  #define RFM69_RST     11   // "A"
  #define RFM69_CS      10   // "B"
  #define RFM69_IRQ     6    // "D"
  #define RFM69_IRQN    digitalPinToInterrupt(RFM69_IRQ )
*/

/* Teensy 3.x w/wing
  #define RFM69_RST     9   // "A"
  #define RFM69_CS      10   // "B"
  #define RFM69_IRQ     4    // "C"
  #define RFM69_IRQN    digitalPinToInterrupt(RFM69_IRQ )
*/

/* WICED Feather w/wing
  #define RFM69_RST     PA4     // "A"
  #define RFM69_CS      PB4     // "B"
  #define RFM69_IRQ     PA15    // "C"
  #define RFM69_IRQN    RFM69_IRQ
*/

#define LED           13  // onboard blinky
//#define LED           0 //use 0 on ESP8266

int16_t packetnum = 0;  // packet counter, we increment per xmission

RFM69 radio = RFM69(RFM69_CS, RFM69_IRQ, IS_RFM69HCW, RFM69_IRQN);

void setup() {
  //while (!Serial); // wait until serial console is open, remove if not tethered to computer. Delete this line on ESP8266
  Serial.begin(SERIAL_BAUD);
  sensor.init();
  sensor.setTimeout(1000);
  sensor.startContinuous();
  Serial.println("Feather RFM69HCW Transmitter");

  // Hard Reset the RFM module
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, HIGH);
  delay(100);
  digitalWrite(RFM69_RST, LOW);
  delay(100);

  // Initialize radio
  radio.initialize(FREQUENCY, NODEID, NETWORKID);
  if (IS_RFM69HCW) {
    radio.setHighPower();    // Only for RFM69HCW & HW!
  }
  radio.setPowerLevel(31); // power output ranges from 0 (5dBm) to 31 (20dBm)

  radio.encrypt(ENCRYPTKEY);

  pinMode(LED, OUTPUT);
  Serial.print("\nTransmitting at ");
  Serial.print(FREQUENCY == RF69_433MHZ ? 433 : FREQUENCY == RF69_868MHZ ? 868 : 915);
  Serial.println(" MHz");
}


void loop() {
  delay(1000);  // Wait 1 second between transmits, could also 'sleep' here!


  if (sensor.timeoutOccurred())
  {
    //Serial.print(" TIMEOUT");
    sensor.init();
    sensor.setTimeout(1000);
    //Serial.print("TIMEOUT DONE");
    sensor.startContinuous();
  }

  //Serial.println();

  delay(50);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  fill_lvl = sensor.readRangeContinuousMillimeters();
  if (fill_lvl > MAX_DISTANCE)
  {
    fill_lvl = MAX_DISTANCE;
  }

  per_fl = map(fill_lvl, 10, MAX_DISTANCE, 10, 0);
  if (per_fl > 10)
  {
    per_fl = 10;
  }

  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  Serial.print("VBat: " ); Serial.println(measuredvbat);


  char radiopacket[20] = "";
  String string_to_send_concat = String(NODEID,DEC) + "AA" + String(measuredvbat,2)+ "AA" + String(per_fl,DEC);
  string_to_send_concat.toCharArray(radiopacket,20);
  //itoa(String(measuredvbat,2), radiopacket, 10);
  //Serial.print("Sending "); 
  Serial.println(radiopacket);

  if (radio.sendWithRetry(RECEIVER, radiopacket, strlen(radiopacket))) { //target node Id, message as string or byte array, message length
    Serial.println("OK");
    Blink(LED, 50, 3); //blink LED 3 times, 50ms between blinks
  }

  radio.receiveDone(); //put radio in RX mode
  Serial.flush(); //make sure all serial data is clocked out before sleeping the MCU
}

void Blink(byte PIN, byte DELAY_MS, byte loops)
{
  for (byte i = 0; i < loops; i++)
  {
    digitalWrite(PIN, HIGH);
    delay(DELAY_MS);
    digitalWrite(PIN, LOW);
    delay(DELAY_MS);
  }
}

// Si4463 CW Beacon
//
// Simple CW beacon for Arduino and SiLabs Si4463, forked from RA9YER Alexander
// https://github.com/alexander-sholohov and slightly modified by IU5HKU Marco
// 
// Hardware info
// ---------------------
// serial debug port baud rate: 74880
// Si5351A is connected via I2C on pin D1 (SCL) and D2 (SDA) as marked on Wemos D1 mini Lite
// freq0 is used in clock0 output, freq1 in clock1, freq2 in clock2
// on SV1AFN board clock0 is marked J1, clock1 J2, clock2 J3.
//
// Hardware Requirements
// ---------------------
// This firmware must be run on an Arduino compatible board
// tested on Arduino Nano and Arduino Uno.
//
//The MIT License (MIT)
// ---------------------
//Copyright (c) 2016 Alexander Sholohov <ra9yer@yahoo.com>
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "src/morse/morse.h"
#include "src/si4463/si4463.h"
#include <avr/sleep.h>
#include <avr/wdt.h>

const int nSEL = 7;   // out
const int siMOSI = 6; // out
const int siMISO = 5; // in
const int siSCK = 4;  // out
const int siSDN = 2;  // out (Si4463 shutdown)

const int pinOOK = 3; // out

const int pinLED = 13; // out (onboard led)
const int pinEXLED = 12; // out (external led)

bool prevTransmittingActive = false;

// watchdog interrupt
ISR (WDT_vect) 
{
   wdt_disable();  // disable watchdog
}  // end of WDT_vect

// going to sleep
void Sleep(){
  // disable ADC
  ADCSRA = 0;  

  // clear various "reset" flags
  MCUSR = 0;     
  // allow changes, disable reset
  WDTCSR = bit (WDCE) | bit (WDE);
  // set interrupt mode and an interval 
  WDTCSR = bit (WDIE) | bit (WDP3) | bit (WDP0);    // set WDIE, and 8 seconds delay
  wdt_reset();  // pat the dog
  
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  noInterrupts ();           // timed sequence follows
  sleep_enable();
 
  // turn off brown-out enable in software
  MCUCR = bit (BODS) | bit (BODSE);
  MCUCR = bit (BODS); 
  interrupts ();             // guarantees next instruction executed
  sleep_cpu ();  
  
  // cancel sleep as a precaution
  sleep_disable();
}

CMorse morse;
CSi4463 transmitter(nSEL, siMOSI, siMISO, siSCK, pinOOK, siSDN);

//------------------------------------------
void setup() {
  pinMode(nSEL, OUTPUT);
  pinMode(siMOSI, OUTPUT);
  pinMode(siMISO, INPUT);
  pinMode(siSCK, OUTPUT);
  pinMode(siSDN, OUTPUT);

  pinMode(pinOOK, OUTPUT);
  pinMode(pinLED, OUTPUT);
  pinMode(pinEXLED, OUTPUT);
  
  //Serial.begin(9600);

  morse.setText("____ VVV VVV IU5HKU/B IU5HKU/B JN53HB JN53HB K"); // <---- place your text here
  
  //transmitter.setDebugSerial(&Serial);
  
  morse.start();
}

//------------------------------------------
void loop() {

  // TODO: Remove. Research purpose only.
  /*
  if (Serial.available()) {
    int inByte = Serial.read();
    if( inByte == 'o' )
    {
      Serial.println(F("do si initialize"));
      transmitter.initialize();
    }
    else if( inByte == 't' )
    {
      Serial.println(F("do tx"));
      transmitter.activateTX();
    }
    else if( inByte == 'r' )
    {
      Serial.println(F("do ready"));
      transmitter.activateRX();
    }
    else if( inByte == '1' )
    {
      Serial.println(F("o1"));
      digitalWrite(pinOOK, HIGH);
    }
    else if( inByte == '0' )
    {
      Serial.println(F("o0"));
      digitalWrite(pinOOK, LOW);
    }
    else if( inByte == '2' )
    {
      Serial.println(F("morse start"));
      morse.start();
    }
    else if( inByte == '3' )
    {
      Serial.println(F("morse stop"));
      morse.stop();
    }
    else
    {
      Serial.println(F("wrong command"));
    }
    
  }
  */

  // We give CPU time to Morse subsystem.
  morse.handleTimeout();

  // Send commands to si4463 module based on morse state.
  
  bool txActive = morse.isTransmittingActive();

  // detect state changing
  if( prevTransmittingActive != txActive )
  {
    if( txActive )
    {
      transmitter.exitSDN();    // wakeup si4463 from sleep
      transmitter.initialize(); // POR
      transmitter.activateTX(); // TX Mode
      prevTransmittingActive = true;
    }
    else
    {
      transmitter.activateRX(); // RX Mode
      transmitter.controlOOK( false );
      prevTransmittingActive = false;

      transmitter.enterSDN();   // put si4463 to sleep
      Sleep();                  // put Arduino to sleep for 8"
    }
  }

  if( txActive )
  {
    // if transmitting is active we handle CW on/off
    bool toneOn = morse.isToneActive();
    transmitter.controlOOK( toneOn );

    // LED duplicate
    digitalWrite(pinLED, (toneOn)? HIGH : LOW );
    digitalWrite(pinEXLED, (toneOn)? HIGH : LOW );
  }
}

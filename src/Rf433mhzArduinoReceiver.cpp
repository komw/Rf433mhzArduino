/*
  Rf433mhzArduinoReceiver - An Arduino library to operate low cost 433 MHz transmiter/receiver (based on rc-switch project)
  Copyright (c) 2015 Szymon Gładysz.  All right reserved.

  Contributors:
  - Szymon Gładysz / komw(at)sgladysz(dot)com
  
  Project home: https://github.com/komw/rf433mhz_arduino_receiver
  Based on:  http://code.google.com/p/rc-switch/

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Rf433mhzArduinoReceiver.h"



unsigned int Rf433mhzArduinoReceiver::nReceivedDelay = 0;
unsigned int Rf433mhzArduinoReceiver::nReceivedProtocol = 1;
int Rf433mhzArduinoReceiver::nReceiveTolerance = 60;
unsigned int Rf433mhzArduinoReceiver::timings[Rf433mhzArduinoReceiver_MAX_CHANGES];

unsigned long Rf433mhzArduinoReceiver::nCodesArray[5000] = {0};
int Rf433mhzArduinoReceiver::nCodesPointer = 0;
Rf433mhzArduinoReceiver::Rf433mhzArduinoReceiver() {
  this->nTransmitterPin = -1;
  this->nRepeatTransmit = 10;
  this->nProtocol = 1;
  this->nReceiverInterrupt = -1;
  this->nPulseLength = 350;
  Rf433mhzArduinoReceiver::nCodesPointer = 3;
}

/**
 * Enable transmissions
 *
 * @param nTransmitterPin    Arduino Pin to which the sender is connected to
 */
void Rf433mhzArduinoReceiver::enableTransmit(int nTransmitterPin) {
  this->nTransmitterPin = nTransmitterPin;
  pinMode(this->nTransmitterPin, OUTPUT);
}

/**
 * Sends a "0" Bit
 *                       _    
 * Waveform Protocol 1: | |___
 *                       _  
 * Waveform Protocol 2: | |__
 */
void Rf433mhzArduinoReceiver::send0() {
    if (this->nProtocol == 1){
        this->transmit(1,3);
    }
    else if (this->nProtocol == 2) {
        this->transmit(1,2);
    }
    else if (this->nProtocol == 3) {
        this->transmit(4,11);
    }
}

/**
 * Sends a "1" Bit
 *                       ___  
 * Waveform Protocol 1: |   |_
 *                       __  
 * Waveform Protocol 2: |  |_
 */
void Rf433mhzArduinoReceiver::send1() {
      if (this->nProtocol == 1){
        this->transmit(3,1);
    }
    else if (this->nProtocol == 2) {
        this->transmit(2,1);
    }
    else if (this->nProtocol == 3) {
        this->transmit(9,6);
    }
}



void Rf433mhzArduinoReceiver::send(char* sCodeWord) {
  for (int nRepeat=0; nRepeat<nRepeatTransmit; nRepeat++) {
    int i = 0;
    while (sCodeWord[i] != '\0') {
      switch(sCodeWord[i]) {
        case '0':
          this->send0();
        break;
        case '1':
          this->send1();
        break;
      }
      i++;
    }
    this->sendSync();
  }
}

void Rf433mhzArduinoReceiver::sendSync() {

    if (this->nProtocol == 1){
        this->transmit(1,31);
    }
    else if (this->nProtocol == 2) {
        this->transmit(1,10);
    }
    else if (this->nProtocol == 3) {
        this->transmit(1,71);
    }
}

void Rf433mhzArduinoReceiver::transmit(int nHighPulses, int nLowPulses) {
    boolean disabled_Receive = false;
    int nReceiverInterrupt_backup = nReceiverInterrupt;
    if (this->nTransmitterPin != -1) {
        if (this->nReceiverInterrupt != -1) {
            disabled_Receive = true;
        }
        digitalWrite(this->nTransmitterPin, HIGH);
        delayMicroseconds( this->nPulseLength * nHighPulses);
        digitalWrite(this->nTransmitterPin, LOW);
        delayMicroseconds( this->nPulseLength * nLowPulses);

        if(disabled_Receive){
            this->enableReceive(nReceiverInterrupt_backup);
        }
    }
}


/**
 * Enable receiving data
 */
void Rf433mhzArduinoReceiver::enableReceive(int interrupt) {
  this->nReceiverInterrupt = interrupt;
  attachInterrupt(this->nReceiverInterrupt, handleInterrupt, CHANGE);
}



bool Rf433mhzArduinoReceiver::receiveProtocol1(unsigned int changeCount){
    
      unsigned long code = 0;
      unsigned long delay = Rf433mhzArduinoReceiver::timings[0] / 31;
      unsigned long delayTolerance = delay * Rf433mhzArduinoReceiver::nReceiveTolerance * 0.01;    

      for (int i = 1; i<changeCount ; i=i+2) {
      
          if (Rf433mhzArduinoReceiver::timings[i] > delay-delayTolerance && Rf433mhzArduinoReceiver::timings[i] < delay+delayTolerance && Rf433mhzArduinoReceiver::timings[i+1] > delay*3-delayTolerance && Rf433mhzArduinoReceiver::timings[i+1] < delay*3+delayTolerance) {
            code = code << 1;
          } else if (Rf433mhzArduinoReceiver::timings[i] > delay*3-delayTolerance && Rf433mhzArduinoReceiver::timings[i] < delay*3+delayTolerance && Rf433mhzArduinoReceiver::timings[i+1] > delay-delayTolerance && Rf433mhzArduinoReceiver::timings[i+1] < delay+delayTolerance) {
            code+=1;
            code = code << 1;
          } else {
            // Failed
            i = changeCount;
            code = 0;
          }
      }      
      code = code >> 1;
    if (changeCount > 6) {    // ignore < 4bit values as there are no devices sending 4bit values => noise
      Rf433mhzArduinoReceiver::nReceivedDelay = delay;
      Rf433mhzArduinoReceiver::nReceivedProtocol = 1;
 
     if(code!=0){
       if(Rf433mhzArduinoReceiver::nCodesPointer<256){
          Rf433mhzArduinoReceiver::nCodesArray[Rf433mhzArduinoReceiver::nCodesPointer++] = code;
       }
      }
    }

    if (code == 0){
        return false;
    }else if (code != 0){
        return true;
    }
}

 unsigned long Rf433mhzArduinoReceiver::popLastCode() {
    if(Rf433mhzArduinoReceiver::nCodesPointer>0){
       return Rf433mhzArduinoReceiver::nCodesArray[Rf433mhzArduinoReceiver::nCodesPointer--];
    }
    return 0;
 }

void Rf433mhzArduinoReceiver::handleInterrupt() {

  static unsigned int duration;
  static unsigned int changeCount;
  static unsigned long lastTime;
  static unsigned int repeatCount;
  

  long time = micros();
  duration = time - lastTime;
 
  if (duration > 5000 && duration > Rf433mhzArduinoReceiver::timings[0] - 200 && duration < Rf433mhzArduinoReceiver::timings[0] + 200) {
    repeatCount++;
    changeCount--;
    if (repeatCount == 2) {
      Rf433mhzArduinoReceiver::receiveProtocol1(changeCount);
      repeatCount = 0;
    }
    changeCount = 0;
  } else if (duration > 5000) {
    changeCount = 0;
  }
 
  if (changeCount >= Rf433mhzArduinoReceiver_MAX_CHANGES) {
    changeCount = 0;
    repeatCount = 0;
    
  }
  Rf433mhzArduinoReceiver::timings[changeCount++] = duration;
  lastTime = time;  
}

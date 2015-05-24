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




#ifndef _Rf433mhzArduinoReceiver_h
#define _Rf433mhzArduinoReceiver_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#elif defined(ENERGIA) // LaunchPad, FraunchPad and StellarPad specific
    #include "Energia.h"	
#else
    #include "WProgram.h"
#endif

#define Rf433mhzArduinoReceiver_MAX_CHANGES 67

class Rf433mhzArduinoReceiver {

  public:
    Rf433mhzArduinoReceiver();
    static unsigned long popLastCode();
    void send(char* Code);
    void enableReceive(int interrupt);
    void enableTransmit(int nTransmitterPin);
  private:
    int nReceiverInterrupt;
    int nTransmitterPin;
    int nPulseLength;
    int nRepeatTransmit;
    char nProtocol;
    void send0();
    void send1();
    void sendSync();
    void transmit(int nHighPulses, int nLowPulses);
    static int nReceiveTolerance;
    static int nCodesPointer;
    static unsigned int nReceivedDelay;
    static unsigned int nReceivedProtocol;    
    static bool receiveProtocol1(unsigned int changeCount);
    static void handleInterrupt();
    static unsigned int timings[Rf433mhzArduinoReceiver_MAX_CHANGES];
    static unsigned long nCodesArray[256];

};

#endif

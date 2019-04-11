#include "configuration.h" //TODO REMOVE

#ifdef PLUGIN_NEXTION

/*
HMI Nextion Library
Itead NX4827T043_011
http://imall.itead.cc/nextion-nx2432t043.html

Bentley Born
bentley@crcibernetica.com

Ricardo Mena C
ricardo@crcibernetica.com


http://crcibernetica.com

 License
 **********************************************************************************
 This program is free software; you can redistribute it 
 and/or modify it under the terms of the GNU General    
 Public License as published by the Free Software       
 Foundation; either version 3 of the License, or        
 (at your option) any later version.                    
                                                        
 This program is distributed in the hope that it will   
 be useful, but WITHOUT ANY WARRANTY; without even the  
 implied warranty of MERCHANTABILITY or FITNESS FOR A   
 PARTICULAR PURPOSE. See the GNU General Public        
 License for more details.                              
                                                        
 You should have received a copy of the GNU General    
 Public License along with this program.
 If not, see <http://www.gnu.org/licenses/>.
                                                        
 Licence can be viewed at                               
 http://www.gnu.org/licenses/gpl-3.0.txt

 Please maintain this license information along with authorship
 and copyright notices in any redistribution of this code
 **********************************************************************************
 */

#ifndef __NEXTION_O_H__
#define __NEXTION_O_H__

#include <Arduino.h>

#include <SoftwareSerial.h>

class Nextion
{
private:
  void flushSerial();

  bool software_serial = false;

  SoftwareSerial *soft_nextion;

  HardwareSerial *hard_nextion;

public:
  Nextion(SoftwareSerial &next, uint32_t baud); //Constructor

  Nextion(HardwareSerial &next, uint32_t baud); //Constructor

  void buttonToggle(boolean &buttonState, String objName, uint8_t picDefualtId, uint8_t picPressedId);

  uint8_t buttonOnOff(String find_component, String unknown_component, uint8_t pin, int btn_prev_state);

  boolean setComponentValue(String component, int value);

  boolean ack(void);

  unsigned int getComponentValue(String component);

  boolean setComponentText(String component, String txt);

  boolean updateProgressBar(int x, int y, int maxWidth, int maxHeight, int value, int emptyPictureID, int fullPictureID, int orientation = 0);

  String getComponentText(String component, uint32_t timeout = 100);

  String listen(unsigned long timeout = 100);

  void sendCommand(const char *cmd);

  uint8_t pageId(void);

  boolean init(const char *pageId = "0");

protected:
  void serial_setTimeout(unsigned long timeout);
  size_t serial_readBytes(char *buffer, size_t length);
  int serial_available(void);
  int serial_read(void);
  size_t serial_print(const char *x);
  size_t serial_write(int n);
  void serial_flush();
};
#endif
#endif

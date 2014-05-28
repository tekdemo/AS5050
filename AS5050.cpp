//////////////////////////////////////////////////////////////////////////
//                     libAS5050
// This library aims to provide easy and convenient
// communication with the AS5050 magnetic rotary encoder IC.
//////////////////////////////////////////////////////////////////////////
// Written and maintained by Dan Sheadel (tekdemo@gmail.com)
// Code available at https://github.com/tekdemo/AS5050
//////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it 
// and/or modify it under the terms of the GNU General    
// Public License as published by the Free Software       
// Foundation; either version 2 of the License, or        
// (at your option) any later version.                    
//                                                        
// This program is distributed in the hope that it will   
// be useful, but WITHOUT ANY WARRANTY; without even the  
// implied warranty of MERCHANTABILITY or FITNESS FOR A   
// PARTICULAR PURPOSE.  See the GNU General Public        
// License for more details.                              
//                                                        
// You should have received a copy of the GNU General    
// Public License along with this program; if not, write 
// to the Free Software Foundation, Inc.,                
// 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//                                                        
// Licence can be viewed at                               
// http://www.fsf.org/licenses/gpl.txt                    
//
// Please maintain this license information along with authorship
// and copyright notices in any redistribution of this code
//
//////////////////////////////////////////////////////////////////////////



#include "Arduino.h"
#include "AS5050.h"

#ifndef _SPI_H_INCLUDED
#warning +============================================+
#warning | Please include SPI.h from the main sketch. |
#warning | This issue is a limitation of the Arduino  |
#warning | libraries and cannot be resolved here.     |
#warning +============================================+
#endif

#include "SPI.h"


AS5050::AS5050(byte pin, byte spi_speed){
  /*CONSTRUCTOR
  * Sets up the required values for the function, and configures the 
  * hardware SPI to operate correctly
  */
  _pin=pin;

  //Prepare the SPI interface
  pinMode(_pin,OUTPUT); 
  SPI.setClockDivider(spi_speed);       //this board supports speedy! :D
  SPI.setBitOrder(MSBFIRST);            //Match the expected bit order
  SPI.setDataMode(SPI_MODE1);           //falling edge (CPOL 1), low idle (CPHA 0);
  //pull pin mode low to assert slave
  //digitalWrite(_pin,LOW);

  //Read angle twice to initialize chip and get to a known good state
  //Reading once won't work, as _last_angle will be set incorrectly
  angle();
  _init_angle=angle();
  //angle() will glitch on startup if it's >768, reset it
  rotations=0;
};

unsigned int AS5050::send(unsigned int reg_a){
  spi_data response,reg;
  reg.value=reg_a;
  //This function does not take care of parity stuff,
  //due to peculiarities with it.
  
  SPI.begin();
  digitalWrite(_pin,LOW);  //Start Transaction
  
  //Send data in MSB order
  response.bytes.msb=SPI.transfer(reg.bytes.msb);
  response.bytes.lsb=SPI.transfer(reg.bytes.lsb);

  digitalWrite(_pin,HIGH);	//End Transaction
  SPI.end();
  return response.value; 
};


unsigned int AS5050::read(unsigned int reg){
  /* Data packet looks like this:
  MSB |14 .......... 2|   1      | LSB
  R/W | ADRESS <13:0> | ERR_FLAG | |Parity
  */

  //Prepare data command for sending across the wire
  reg= (reg<<1) |(AS_READ); //make room for parity and set RW bi
  reg|= __builtin_parity(reg);  //set in the parity bit

  send(reg);              //send data
  //delayMicroseconds(1);       //hold time between transactions:50ns
  reg=send(REG_NOP); //receive response from chip
  
  //Save the parity error for analysis
  error.transaction|=__builtin_parity(reg&(~RES_PARITY)) != (reg&RES_PARITY);

  return reg; //remove error and parity bits  
}


//FIXME: Make the Write return and verify the response.
unsigned int AS5050::write(unsigned int reg,unsigned int data){
  
  //Prepare register data
  reg=(reg<<1)|(AS_WRITE);      //add parity bit place and set RW bit  
  reg|=__builtin_parity(reg);   //Set the parity bit
  
  //prepare data for transmit
  data=data<<2;                  //Don't care and parity placeholders
  data|=__builtin_parity(data);  //Set the parity bit on the data

  send(reg);          //send the register we wish to write to
  send(data);         //set the data we want in there
  data=send(REG_NOP); //Get confirmation from chip
        
  //save error and parity data
  error.transaction=data & (RES_ERROR_FLAG); //save error data
  error.transaction|=__builtin_parity(data&(~RES_PARITY)) != (data&RES_PARITY); //save parity errors
 
  return data;      //remove parity and EF bits and return data. 
};  
  
int AS5050::angle(){
  //This function strips out the error and parity 
  //data in the data frame, and handles the errors
  unsigned int data=read(REG_ANGLE);
  
  /* Response from chip is this:
   14 | 13 | 12 ... 2                       | 1  | 0
   AH | AL |  <data 10bit >                 | EF | PAR
  */
  
  //save error data to the error register
  error.transaction=(data|RES_ALARM_HIGH|RES_ALARM_LOW);
  //Check parity of transaction
  error.transaction|=__builtin_parity(data&(~RES_PARITY)) != (data&RES_PARITY);
  
  
  //Automatically handle errors if we've enabled it
  #if AS5050_AUTO_ERROR_HANDLING==1
  if(error.transaction){
    handleErrors();
    //If there's a parity error, the chip responds with an angle of 0, so just use the last angle to prevent glitching
    if(error.transaction&RES_PARITY) return _last_angle;
  }
  #endif
  
  //TODO this needs some work to avoid magic numbers
  int angle=(data&0x3FFE)>>2; //strip away alarm bits, then parity and error flags

  //track rollovers for continous angle monitoring
  if(_last_angle>768 && angle<=256)rotations+=1;
  else if(_last_angle<256 && angle>=768)rotations-=1;
  _last_angle=angle;

  return angle;

}
float AS5050::angleDegrees(){
    //Rewrite of arduino's map function, to make sure we don't lose resolution
    //return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    return angle()*360/(float)AS5050_ANGULAR_RESOLUTION;
}
float AS5050::angleRad(){
  //return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  return angle()*(AS5050_TAU)/(float)AS5050_ANGULAR_RESOLUTION;
}



long int AS5050::totalAngle(){
    return angle()+rotations*1024 ;
}
float AS5050::totalAngleDegrees(){
    return angleDegrees()+360*rotations;
}
float AS5050::totalAngleRad(){
    return angleDegrees()+AS5050_TAU*rotations;
}


long int AS5050::deltaAngle(){
    return (angle()-_init_angle)+rotations*1024;
}
float AS5050::deltaAngleDegrees(){
    return (deltaAngle())*360/(float)AS5050_ANGULAR_RESOLUTION;
}
float AS5050::deltaAngleRad(){
     return (deltaAngle())*AS5050_TAU/(float)AS5050_ANGULAR_RESOLUTION;
}

void AS5050::setHome(){
    //Reset the home for the deltas/total functions
    rotations=0;
    _init_angle=0;
}

unsigned int AS5050::handleErrors(){  //now, handle errors: 
  unsigned int error_t=error.transaction; //save current register
  //We have two errors to look at: 
  //error.transaction indicates the basic error types, pulled out of reading angles and
  //general data.
  if(error.transaction&(RES_ALARM_HIGH|RES_ALARM_LOW)){
    //Handle errors here, so end users don't have to care
    switch(error_t&(RES_ALARM_HIGH|RES_ALARM_LOW)){
      case RES_ALARM_HIGH: //gain too high, decrease gain
        gain=read(REG_GAIN_CONTROL); //get information about current gain
        write(REG_GAIN_CONTROL,--gain);  //increment gain and send it back
        break;
      case RES_ALARM_LOW: //gain too low, increase gain
        gain=read(REG_GAIN_CONTROL); //get information about current gain
        write(REG_GAIN_CONTROL,++gain); //increment gain and send it back
        break;
      default://General errors
        //TODO Read and handle DAC overflow issues and the like.
        //Currently just disregards everything and exits.  
        break;
      }//switch

    error.transaction=0;               //Reset the transaction error register

    //This command returns 0 on successful clear
    //otherwise, this command can handle it later
    error.status=read(REG_CLEAR_ERROR) ;

    //If the error is still there, reset the AS5050 to remove them
    #if AS5050_RESET_ON_ERRORS==1
    if(error.status)write(REG_SOFTWARE_RESET,DATA_SWRESET_SPI);
    #endif
    }//if
  
  return error.status; 
};


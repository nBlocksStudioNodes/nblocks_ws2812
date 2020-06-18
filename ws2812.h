/**
* @file     ws2812.h
* @brief    header file for neopixel LED interface
* 
*
* @author   Ian Aherne
*
* @date     29/08/18
*/

#ifndef WS2812_H
#define WS2812_H

#include "mbed.h"



class ws2812{
    
    public:
    
        ws2812(PinName MOSI);
        void send_colour(uint8_t,uint8_t,uint8_t,uint8_t);
    
    
    
    private:
    
    SPI _spi;
    
    
    
    
};

#endif
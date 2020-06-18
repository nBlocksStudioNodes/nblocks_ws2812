/**


To control the WS2812 intelligent control LED a single wire protocol is used
with a frequency of 800khz. The integrated Leds have a Green, Red, and blue section
in order to control the LEDs a 24 bit value is sent 8bits for green 8 for red and 8 for blue.
eg. 255,0,0 = green, 0,255,0 = red, 0,0,255 = blue. Values of each can be increased or decreased
to achieve a blend of colour and brightness.

For the WS2812 to distinguish between 1 and 0 in the single wire protocol the 
duty cycle of the transmission is varied.
                    0 code = high for 0.4us low for 0.8us
                    1 code = high for 0.8us low for 0.45us
                    
please note Data sheet timings are incorrect see adfruits website for correct timings.
                    
To achieve the high transmission rates a SPI hack is used using only the MOSI pin
without a clock, the SPI frequency is set at 2.4 Mhz triple the required frequency,
the SPI format is set to 12 bit packets.

To achieve the precise timing for 1 and 0 the followoing is sent at tripple the frequency 

                    0 code = 100
                    1 code = 110
                    
The above codes sent at 3 x 800Khz will replicate the the data sheet code timings 
and frees up the processor to do other things. Instead of sending 24 bits to light one LED
72 bits are sent at 3 times the frequency which to the appears to the WS2812 to be 24.

each SPI write is a 12 bit packet so 6 packets are sent consecutively to replicate
the 24 bits required to control the WS2812

Please note for correct operation a pull down resistor should be used on MOSI pin
this can not be set in firm ware using Mbed interface.
                 

*/

#include "ws2812.h"

/**
* @brief    ws2812
* @details  Constructor sets frequency and packet size.           
* @param    NA
* @return   NA
* 
*/

ws2812::ws2812(PinName MOSI): _spi(MOSI,NC,NC,NC){
    
    _spi.frequency(2400000);
    _spi.format(12); 
    
  
    
}

/**
* @brief    send_colour()
* @details  Function takes the  8 bit colour values and codes them to neo bit 
*           format 1= 110 0 =100 8 bit values becom 24 bit values and each are 
*           split and sent via SPI in 12 bit packets.
* @param    colour values for green,red,blue and number of LEDs to light
* @return   NA
* 
*/


void ws2812 :: send_colour(uint8_t green, uint8_t red, uint8_t blue,uint8_t numberOfLeds){
    
    
    uint8_t colours[3];
    uint16_t packet[6] ={0};
    uint8_t bit;
    uint32_t neoBit;
    uint32_t neoColour = 0;
    
 // bit manipulation to convert bits into 3 bits   
    colours[2] = green;
    colours[1] = red;
    colours[0] = blue;
    
    for(int i = 2; i>=0; i--){ // loop through green,red,blue.
           
        for (int j = 7; j>=0; j--){ // loop through 8 bits of colour.
            
           bit = (colours[i] >> j) & 0x01;//check bit value and convert to 110 or 100.
           
           switch(bit){            
            
                case 0:
                    neoBit = 0x4;
                    break;
                    
                case 1:
                    neoBit = 0x6;
                    break;
                    
                default:
                    break;
                   
            } 
                       
            neoColour  |= neoBit << (j*3); // fill neoColour with 3 bit neobits.                     
        }
        
        packet[(2*i)+1] = (neoColour & 0xFFF000) >> 12; // split neoColour into two 12 bit packets for transmission
        packet[2*i] = neoColour & 0x000FFF;
        
        neoColour = 0; // reset neocolour once used
                  
    }
      
   
 // Send SPI packets 12 bits per packet for the amount of leds that is to be illuminated
 
 for(int k = 0; k < numberOfLeds; k++){
    
    for(int l = 5; l>=0; l--){
        
        _spi.write(packet[l]);
    }
 }
 
 wait_us(50);
  
}
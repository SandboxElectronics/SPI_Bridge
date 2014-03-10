/* 
Description:
This is a example code for Sandbox Electronics' I2C to SPI bridge module.
You can get one of those products on 
http://sandboxelectronics.com

Version:
V0.1

Release Date:
2014-02-16

Author:
Tiequan Shao          info@sandboxelectronics.com

Lisence:
CC BY-NC-SA 3.0

Please keep the above information when you use this code in your project. 
*/


#ifndef _SC18IS602_H_
#define _SC18IS602_H_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <SPI.h>
//Device Address

#define     SC18IS602_ADDRESS_000     (0X50)
#define     SC18IS602_ADDRESS_001     (0X52)
#define     SC18IS602_ADDRESS_010     (0X54)
#define     SC18IS602_ADDRESS_011     (0X56)
#define     SC18IS602_ADDRESS_100     (0X58)
#define     SC18IS602_ADDRESS_101     (0X5A)
#define     SC18IS602_ADDRESS_110     (0X5C)
#define     SC18IS602_ADDRESS_111     (0X5E)

//Clock Divider
#define     SC18IS602_CLOCK_1843K     (0X00)
#define     SC18IS602_CLOCK_461K      (0X01)
#define     SC18IS602_CLOCK_115K      (0X02)
#define     SC18IS602_CLOCK_58K       (0X03)

#define     SC18IS602_SPI_MODE0       (0x00)
#define     SC18IS602_SPI_MODE1       (0x01)
#define     SC18IS602_SPI_MODE2       (0x02)
#define     SC18IS602_SPI_MODE3       (0x03)

#define     SC18IS602_SS_0   		 (0X00)
#define     SC18IS602_SS_1  		 (0X01)
#define     SC18IS602_SS_2   		 (0X02)
#define     SC18IS602_SS_3   		 (0X03)



class SC18IS602 
{ 
    public:
        SC18IS602(uint8_t addr = SC18IS602_ADDRESS_000);
        void begin();
		void begin(uint8_t ss_pin);
		void end();
		void setBitOrder(uint8_t order); //LSBFIRST MSBFIRST
		void setClockDivider(uint8_t divider);
		void setDataMode(uint8_t mode);
		uint8_t transfer(uint8_t value);
		uint8_t transfer(uint8_t* buffer, uint8_t length);
        void pinMode(uint8_t pin, uint8_t io);
        void digitalWrite(uint8_t pin, uint8_t value);
        uint8_t digitalRead(uint8_t pin);
		void InterruptClear(void);
    
    private:
        uint8_t device_address;
		uint8_t ss_pin;
		uint8_t reg_f0_config; //configuration register 0xF0
		uint8_t reg_f4_gpio_w; //gpio write register
		uint8_t reg_f5_gpio_r; //gpio read register
		uint8_t reg_f6_gpio_e; //gpio enable register
		uint8_t reg_f7_gpio_d; //gpio direction
	    void ResetDevice(void);
		void GPIOEnable(uint8_t pin);
		void SSEnable(uint8_t pin);
		
		
		uint8_t ReadBytes(uint8_t* buffer, uint8_t length);
		uint8_t ReadByte(void);
		void WriteBytes(uint8_t* buffer, uint8_t length);
        void WriteRegister(uint8_t reg_addr, uint8_t val);


		
};

#endif





    
    

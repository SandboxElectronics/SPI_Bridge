/* 
Description:
This is a example code for Sandbox Electronics' I2C/SPI to UART bridge module.
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



#include <Wire.h>
#include <SC18IS602.h>
#include <SPI.h>

#ifdef __AVR__
 #define WIRE Wire
#else // Arduino Due
 #define WIRE Wire1
#endif


SC18IS602::SC18IS602(uint8_t addr)
{
    device_address = (addr>>1);
}


void SC18IS602::ResetDevice(void)
{

	reg_f0_config = 0x00; //configuration register 0xF0
	reg_f4_gpio_w = 0x00; //gpio write register
	reg_f5_gpio_r = 0x0F; //gpio read register
	reg_f6_gpio_e = 0x00; //gpio enable register
	reg_f7_gpio_d = 0xAA; //gpio direction
	WriteRegister(0xf0,reg_f0_config);
	WriteRegister(0xf4,reg_f4_gpio_w);
//	WriteRegister(reg_f5_gpio_r,0x0F);
	WriteRegister(0xf6,reg_f6_gpio_e);
	WriteRegister(0xf7,reg_f7_gpio_d);
	
	pinMode(0,INPUT);
	pinMode(1,INPUT);
	pinMode(2,INPUT);
	pinMode(3,INPUT);
	GPIOEnable(0);
	GPIOEnable(1);
	GPIOEnable(2);
	GPIOEnable(3);

} 

void SC18IS602::GPIOEnable(uint8_t pin)
{
	reg_f6_gpio_e |= ( 0x01<<pin );
	WriteRegister(0xf6,reg_f6_gpio_e);	
}

void SC18IS602::SSEnable(uint8_t pin)
{
	reg_f6_gpio_e &= (uint8_t)(~(0x01<<pin ));
	WriteRegister(0xf6,reg_f6_gpio_e);
}

void SC18IS602::begin(void) 
{
    WIRE.begin();
	ResetDevice();
	ss_pin = SC18IS602_SS_0;
	
}

void SC18IS602::begin(uint8_t pin) 
{
    WIRE.begin();

	ResetDevice();
	ss_pin = SC18IS602_SS_0;

	SSEnable(pin);
	ss_pin = pin;
}

uint8_t SC18IS602::transfer(uint8_t value)
{
	uint8_t result = 0;
	WriteBytes(&value,1);
	ReadBytes(&result,1);
	return result;
}
uint8_t SC18IS602::transfer(uint8_t* buffer, uint8_t length)
{
	WriteBytes(buffer,length);
	return (ReadBytes(buffer,length));
}


void SC18IS602::pinMode(uint8_t pin, uint8_t i_o)
{
    reg_f7_gpio_d &= (((uint8_t)~(0x03))<<(pin<1)); //clear the two control bit of the pin
	if (i_o == INPUT) {
		reg_f7_gpio_d |= (0x02<<(pin<<1)); //10 for input
	} else {
		reg_f7_gpio_d |= (0x01<<(pin<<1)); //01 for output
	}
	WriteRegister(0xF7, reg_f7_gpio_d);
}

void SC18IS602::digitalWrite(uint8_t pin, uint8_t value)
{	
	if ( value==0 ) {
		reg_f4_gpio_w &= (((uint8_t)~(0x01))<<pin);
	} else {
		reg_f4_gpio_w |= (0x01<<pin);
	}
	
    WriteRegister(0xF4, reg_f4_gpio_w);
}

uint8_t SC18IS602::digitalRead(uint8_t pin)
{
   
	WriteRegister(0xF5, reg_f5_gpio_r);
    reg_f5_gpio_r = ReadByte();
    if (reg_f5_gpio_r & (0x01<<pin) ) {
		return 1;
	}
	
	return 0;
}

void SC18IS602::WriteBytes(uint8_t* buffer, uint8_t length)
{
	uint8_t i;
	
	do {
		WIRE.beginTransmission(device_address);
		WIRE.write(0x01<<ss_pin);
		for (i=0;i<length;i++) {
			WIRE.write(buffer[i]);
		}
	} while ( WIRE.endTransmission(1) == 2) ; //keep trying if the device does not ack on it's device address
	//WIRE.endTransmission();
	delay(10);
	return;

}
uint8_t SC18IS602::ReadBytes(uint8_t* buffer, uint8_t length)
{
    uint8_t i;
	uint8_t count;
	//Serial.println("*******readING*********");
	while (WIRE.requestFrom(device_address,length) == 0);
	//Serial.println("*******read DONE*********");
	count = WIRE.available();
	for(i=0;i<count;i++) {
		buffer[i] = WIRE.read();
	}
	//Serial.println("*******read count*********");
	//Serial.println(count,DEC);
    return count;
}

uint8_t SC18IS602::ReadByte(void)
{
    
	while (WIRE.requestFrom(device_address,(uint8_t)1) == 0);

    return WIRE.read();
}

void SC18IS602::WriteRegister(uint8_t reg_addr, uint8_t val)
{
   do {
		WIRE.beginTransmission(device_address);
		WIRE.write(reg_addr);
		WIRE.write(val);
	} while ( WIRE.endTransmission(1) == 2) ; //keep trying if the device does not ack on it's device address
    //WIRE.endTransmission(true);
    delay(10);
    return ;
}



void SC18IS602::setBitOrder(uint8_t order)
{

	reg_f0_config &= 0xDF; 
	if ( order == LSBFIRST) {
		reg_f0_config |= 0x20;
	} else {
		reg_f0_config &= 0xEF;
	}
	
	WriteRegister(0xF0, reg_f0_config);
	
	return;
}



 //LSBFIRST MSBFIRST

void SC18IS602::setClockDivider(uint8_t divider)
{
	reg_f0_config &= 0xFC;
	
	reg_f0_config |= divider;
	
	WriteRegister(0xF0, reg_f0_config);
	
	return;

}
void SC18IS602::setDataMode(uint8_t mode)
{
	reg_f0_config &= 0xF3;
	switch (mode) {
		case SC18IS602_SPI_MODE0:
			break;
		case SC18IS602_SPI_MODE1:
			reg_f0_config |=0x04;
			break;
		case SC18IS602_SPI_MODE2:
			reg_f0_config |=0x08;
			break;			
		case SC18IS602_SPI_MODE3:
			reg_f0_config |=0x0C;
			break;
		default:
			break;
	}
	
	WriteRegister(0xF0, reg_f0_config);
	
	return;
}


void SC18IS602::InterruptClear(void)
{
	uint8_t isr_clear = 0xf1;
	
	WriteBytes(&isr_clear, 1);
	
	return;
}

void SC18IS602::end(void)
{
return;
}










    
    

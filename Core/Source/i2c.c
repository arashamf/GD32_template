#include "i2c.h"
#include "gpio.h"

#define I2C0_OWN_ADDRESS7      0x10

//-----------------------------------------------------------------//
void i2c_config(void)
{ 
    I2C_gpio_config();
    i2c_clock_config(I2C0, 400000, I2C_DTCY_2); // I2C clock configure
    // I2C address configure 
    i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C0_OWN_ADDRESS7);
    i2c_enable(I2C0);     // enable I2C0  
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);  // enable acknowledge
}

//-------------------------------------------------------------------------------------//
void i2c_write_bytes (uint8_t slave_address, uint8_t * data_buffer, uint8_t numb_byte)
{
    uint8_t count = 0;

    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY)); //wait until I2C bus is idle 
    i2c_start_on_bus(I2C0);     //send a start condition to I2C bus 
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND)); //wait until SBSEND bit is set 
    i2c_master_addressing(I2C0, slave_address, I2C_TRANSMITTER);	//send slave address to I2C bus
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)); //wait until ADDSEND bit is set
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND); //clear ADDSEND bit
    while(!i2c_flag_get(I2C0, I2C_FLAG_TBE)); //wait until the transmit data buffer is empty

    for(count=0; count<numb_byte; count++)
    {
        i2c_data_transmit(I2C0, *(data_buffer + count));         // data transmission 
        while(!i2c_flag_get(I2C0, I2C_FLAG_TBE)); //wait until the TBE bit is set
    }
    i2c_stop_on_bus(I2C0); //send a stop condition to I2C bus
    while(I2C_CTL0(I2C0)&0x0200); //wait until stop condition generate

}

//--------------------------------------------------------------------------------------//
void i2c_write_buffer_16bit_registr (uint8_t slave_address, uint16_t registr_adr, uint8_t * data_buffer, uint8_t numb_byte)
{
	uint8_t tmp_buf[2] ;
	tmp_buf[0] = (uint8_t)(registr_adr>>8);
	tmp_buf[1] = (uint8_t)(0x00FF & registr_adr);
	uint8_t count = 0;
	
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY)); //wait until I2C bus is idle 
    i2c_start_on_bus(I2C0);     //send a start condition to I2C bus 
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND)); //wait until SBSEND bit is set 
    i2c_master_addressing(I2C0, slave_address, I2C_TRANSMITTER);	//send slave address to I2C bus
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)); //wait until ADDSEND bit is set
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND); //clear ADDSEND bit
    while(!i2c_flag_get(I2C0, I2C_FLAG_TBE)); //wait until the transmit data buffer is empty

    for (count = 0; count < 2; count++) 
	{
        i2c_data_transmit(I2C0, tmp_buf[count]); 
		while(!i2c_flag_get(I2C0, I2C_FLAG_TBE)); //wait until the TBE bit is set
	}
	
	for (count = 0; count < numb_byte; count++)
    {			
		i2c_data_transmit(I2C0, *(data_buffer+count));
		while(!i2c_flag_get(I2C0, I2C_FLAG_TBE)); //wait until the TBE bit is set
	}

    i2c_stop_on_bus(I2C0); //send a stop condition to I2C bus
    while(I2C_CTL0(I2C0)&0x0200); //wait until stop condition generate
}

//-------------------------------------------------------------------------------------//
void i2c_write_buffer_8bit_registr (uint8_t slave_address, uint8_t registr_adr, uint8_t * data_buffer, uint8_t numb_byte)
{
    uint8_t count = 0;
	
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY)); //wait until I2C bus is idle 
    i2c_start_on_bus(I2C0);     //send a start condition to I2C bus 
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND)); //wait until SBSEND bit is set 
    i2c_master_addressing(I2C0, slave_address, I2C_TRANSMITTER);	//send slave address to I2C bus
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND)); //wait until ADDSEND bit is set
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND); //clear ADDSEND bit
    while(!i2c_flag_get(I2C0, I2C_FLAG_TBE)); //wait until the transmit data buffer is empty

    i2c_data_transmit(I2C0, registr_adr); 
	while(!i2c_flag_get(I2C0, I2C_FLAG_TBE)); //wait until the TBE bit is set

    for (count = 0; count < numb_byte; count++)
    {			
		i2c_data_transmit(I2C0, *(data_buffer+count));
		while(!i2c_flag_get(I2C0, I2C_FLAG_TBE)); //wait until the TBE bit is set
	}

    i2c_stop_on_bus(I2C0); //send a stop condition to I2C bus
    while(I2C_CTL0(I2C0)&0x0200); //wait until stop condition generate
}

#include "spi.h"
#include "gpio.h"

//-------------------------------------------------------------------------------------//
#define DISPLAY_SPI SPI0

//-------------------------------------------------------------------------------------//
void spi0_config(void)
{
    SPI0_gpio_init ();
    rcu_periph_clock_enable(RCU_SPI0);
    spi_i2s_deinit(SPI0); //deinitialize SPI and the parameters 

    SPI_CTL0(SPI0) &= (~SPI_CTL0_CKPH); //Capture the first data at the first clock transition
    SPI_CTL0(SPI0) &= (~SPI_CTL0_CKPL); //CLK pin is pulled low when SPI is idle
    SPI_CTL0(SPI0) |= (SPI_CTL0_MSTMOD); //Master mode
    SPI_CTL0(SPI0) |= SPI_PSC_64;       //Master clock prescaler selection
    SPI_CTL0(SPI0) &= (~SPI_CTL0_LF);  // 0: Transmit MSB first
    SPI_CTL0(SPI0) |= SPI_CTL0_SWNSSEN; //NSS software mode. The NSS level depends on SWNSS bit.
    SPI_CTL0(SPI0) |= SPI_CTL0_SWNSS; //NSS pin is pulled high
    SPI_CTL0(SPI0) &= (~SPI_CTL0_RO); //Full-duplex mode
    SPI_CTL0(SPI0) &= (~SPI_CTL0_FF16); //8-bit data frame format
    SPI_CTL0(SPI0) |= SPI_CTL0_BDEN; //1 line bidirectional transmit mode. The information transfers between the MOSI pin in master and the MISO pin in slave.
    SPI_CTL0(SPI0) |= SPI_CTL0_BDOEN; //When BDEN is set, this bit determines the direction of transfer. 1: Work in transmit-only mode
    SPI_CTL0(SPI0) |= (uint32_t)SPI_CTL0_SPIEN; //SPI peripheral is enabled

    spi_enable(SPI0);
}

//-------------------------------------------------------------------------------------//
void spi_write_byte (uint8_t byte)
{	
    while (spi_i2s_flag_get(DISPLAY_SPI, SPI_FLAG_TBE) == RESET) {}
    spi_i2s_data_transmit(DISPLAY_SPI, byte);
}

//-------------------------------------------------------------------------------------//
void spi_write_buffer (uint8_t * data, uint16_t data_size)
{	
	for (uint8_t count = 0; count < data_size; count++)
	{	
		while (spi_i2s_flag_get(DISPLAY_SPI, SPI_FLAG_TBE) == RESET) {}
		spi_i2s_data_transmit   (DISPLAY_SPI, (uint8_t)(*(data + count)));
	}
}

//------------------------------------------------------------------------------------//
void spi1_config(void)
{
    SPI1_gpio_init ();
    rcu_periph_clock_enable(RCU_SPI1);
    spi_i2s_deinit(SPI1); //deinitialize SPI and the parameters 

    SPI_CTL0(SPI1) &= (~SPI_CTL0_CKPH); //Capture the first data at the first clock transition
    SPI_CTL0(SPI1) &= (~SPI_CTL0_CKPL); //CLK pin is pulled low when SPI is idle
    SPI_CTL0(SPI1) |= (SPI_CTL0_MSTMOD); //Master mode
    SPI_CTL0(SPI1) |= SPI_PSC_64;       //Master clock prescaler selection
    SPI_CTL0(SPI1) &= (~SPI_CTL0_LF);  // 0: Transmit MSB first
    SPI_CTL0(SPI1) |= SPI_CTL0_SWNSSEN; //NSS software mode. The NSS level depends on SWNSS bit.
    //SPI_CTL0(SPI1) &= (~SPI_CTL0_SWNSS); //0:NSS pin is pulled low
    SPI_CTL0(SPI1) |= SPI_CTL0_SWNSS; //1: NSS pin is pulled high
    SPI_CTL0(SPI1) &= (~SPI_CTL0_RO); //Full-duplex mode
    SPI_CTL0(SPI1) &= (~SPI_CTL0_FF16); //8-bit data frame format
    SPI_CTL0(SPI1) |= SPI_CTL0_BDEN; //1 line bidirectional transmit mode. The information transfers between the MOSI pin in master and the MISO pin in slave.
    SPI_CTL0(SPI1) |= SPI_CTL0_BDOEN; //When BDEN is set, this bit determines the direction of transfer. 1: Work in transmit-only mode
    SPI_CTL0(SPI1) |= (uint32_t)SPI_CTL0_SPIEN; //SPI peripheral is enabled

    spi_enable(SPI1);
}


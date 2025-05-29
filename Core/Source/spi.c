#include "spi.h"
#include "gpio.h"
#include "OLED.h"

//-------------------------------------------------------------------------------------//
#define DISPLAY_SPI SPI0
#define LCD_PAGE_NUMBER 8

//-------------------------------------------------------------------------------------//
char lcd_buf[LCD_BUFFER_SIZE];
uint8_t count = 0;

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
    SPI_CTL0(SPI1) |= SPI_CTL0_SWNSS; //1: NSS pin is pulled high
    SPI_CTL0(SPI1) &= (~SPI_CTL0_RO); //Full-duplex mode
    SPI_CTL0(SPI1) &= (~SPI_CTL0_FF16); //8-bit data frame format
    SPI_CTL0(SPI1) |= SPI_CTL0_BDEN; //1 line bidirectional transmit mode. The information transfers between the MOSI pin in master and the MISO pin in slave.
    SPI_CTL0(SPI1) |= SPI_CTL0_BDOEN; //When BDEN is set, this bit determines the direction of transfer. 1: Work in transmit-only mode
    SPI_CTL0(SPI1) |= (uint32_t)SPI_CTL0_SPIEN; //SPI peripheral is enabled

    spi_enable(SPI1);
}

//------------------------------------------------------------------------------------//
void dma_config(void)
{
    rcu_periph_clock_enable(RCU_DMA0);
    dma_deinit(DMA0, DMA_CH2);

    dma_parameter_struct dma_init_struct;
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI0); //адрес регистра SPI0
    dma_init_struct.memory_addr  = (uint32_t)oled_buffer; //указатель на буффер
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_HIGH;
    dma_init_struct.number       = OLED_WIDTH;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE; //отключение инкремента адреса периферии
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE; //включение инкремента адреса буффера
    dma_init(DMA0, DMA_CH2, &dma_init_struct);     // configure DMA mode 

    dma_circulation_disable(DMA0, DMA_CH2); //disable DMA circulation mode
    dma_memory_to_memory_disable(DMA0, DMA_CH2); //disable memory to memory mode

    dma_channel_disable(DMA0,DMA_CH2); //отключение канала 2 DMA
    spi_dma_enable(SPI0, SPI_DMA_TRANSMIT); //коммутация канала 2 DMA с SPI0 TRANSMIT

    nvic_irq_enable(DMA0_Channel2_IRQn, 2, 0); //глобальное разрешение  прерываний от DMA0 Channel2
    dma_interrupt_flag_clear(DMA0, DMA_CH2, DMA_INTF_GIF); //global interrupt flag of channel
    dma_interrupt_flag_clear(DMA0, DMA_CH2, DMA_INTF_FTFIF); //full transfer finish flag of channel
    dma_interrupt_enable(DMA0, DMA_CH2, DMA_INT_FTF); //enable bit for channel full transfer finish interrupt
}

//-------------------------------------------------------------------------------------//
void spi_write_buffer_DMA (void)
{	
    spi_dma_disable(SPI0, SPI_DMA_TRANSMIT);
	dma_channel_disable(DMA0, DMA_CH2);
    dma_memory_width_config(DMA0, DMA_CH2, OLED_BUFFERSIZE/LCD_PAGE_NUMBER );
    dma_memory_address_config(DMA0, DMA_CH2, (uint32_t)&oled_buffer[(OLED_WIDTH*count)]);

	if (count < LCD_PAGE_NUMBER )
	{
		OLED_PageAdress(count);
        spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);
        LCD_CS(ON);
		LCD_DC(ON);
		dma_channel_enable(DMA0, DMA_CH2);
		count++;
	}	
	else
	{
		count = 0;
		dma_channel_disable(DMA0, DMA_CH2);
        LCD_DC(OFF);
		LCD_CS(OFF);
	}
}

//-------------------------------------------------------------------------------------//
void DMA0_Channel2_Callback(void)
{
	spi_write_buffer_DMA ();
}


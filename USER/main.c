#include "stm32f10x.h"
#include "sys.h"
#include "usart.h"
#include "spi.h"

int main(void)
{
	uint16_t spi_data = 0;
    uart_init(9600);
	spi_init();
	printf("Program Running...\n");
    while(1)
    {
		if(spi_readbyte(&spi_data))
		{
			spi_writebyte(spi_data);
		}
		
    }
}

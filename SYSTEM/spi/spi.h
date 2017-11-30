#ifndef __SPI_H_
#define __SPI_H_

#include "stm32f10x.h"

#define QUEUE_SIZE	20

typedef struct QUEUE
{
	int32_t head;
	int32_t tail;
	uint16_t data[QUEUE_SIZE];
	int32_t cnt;
}fifo_queue;

void spi_init(void);
uint8_t spi_readbyte(uint16_t *rxdata);
void spi_writebyte(uint16_t txdata);
#endif

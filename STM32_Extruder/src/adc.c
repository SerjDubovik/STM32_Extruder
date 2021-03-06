#include "stm32f10x.h"
#include "adc.h"
#include "modbus.h"






extern unsigned short	array_mb[leth_array];				// ?????? ??? ???????? ????????/?????????? ????

unsigned short dma_adc[DMA_BUFF_SIZE] 	= {}; 						// ???????? ?????? ??? ?????? ?????? ?? ??? ????? ???
unsigned long adc_buf[4] = {}; 										// ???????? ?????? ??? ??????????





void ADC1_2_IRQHandler(void)
{
    if(ADC1->SR & ADC_SR_EOC)
	{

	}

	ADC1->SR=0;											//?????????? ??? ????? ? ???????? ???????
}



void DMA1_Channel1_IRQHandler(void)
{
	uint32_t buf =  DMA1->ISR;





	if( DMA1->ISR & DMA_ISR_HTIF1)						// ???? ?????? ????????? ?????
	{
		adc_buf[0] = 0;
		adc_buf[1] = 0;
		adc_buf[2] = 0;
		adc_buf[3] = 0;

		for(int i = 0, count = 0; i < 20; i++)
		{
			adc_buf[0] += dma_adc[count++];
			adc_buf[1] += dma_adc[count++];
			adc_buf[2] += dma_adc[count++];
			adc_buf[3] += dma_adc[count++];
		}

		array_mb[13]	= adc_buf[0] / 20;
		array_mb[14]	= adc_buf[1] / 20;
		array_mb[15]	= adc_buf[2] / 20;
		array_mb[16]	= adc_buf[3] / 20;

		array_mb[9]		= array_mb[13];
		array_mb[10]	= array_mb[14];
		array_mb[11]	= array_mb[15];
		array_mb[12]	= array_mb[16];
	}




	if( DMA1->ISR & DMA_ISR_TCIF1)						// ???? ?????? ?????? ?????
	{

		adc_buf[0] = 0;
		adc_buf[1] = 0;
		adc_buf[2] = 0;
		adc_buf[3] = 0;

		for(int i = 0, count = 80; i < 20; i++)
		{
			adc_buf[0] += dma_adc[count++];
			adc_buf[1] += dma_adc[count++];
			adc_buf[2] += dma_adc[count++];
			adc_buf[3] += dma_adc[count++];

		}

		array_mb[13]	= adc_buf[0] / 20;
		array_mb[14]	= adc_buf[1] / 20;
		array_mb[15]	= adc_buf[2] / 20;
		array_mb[16]	= adc_buf[3] / 20;

		array_mb[9]		= array_mb[13];
		array_mb[10]	= array_mb[14];
		array_mb[11]	= array_mb[15];
		array_mb[12]	= array_mb[16];


	}

	DMA1->IFCR = buf;
}







void init_adc()
{

	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; 				// ???????? ???????????? ???

	//????????? ???????? ??? ADC
	RCC->CFGR &= ~RCC_CFGR_ADCPRE;             			//??????? ????? ???????????? "ADC Prescaler"
	RCC->CFGR |=  RCC_CFGR_ADCPRE_DIV6;        			//?????????? "ADC Prescaler" ?????? 6

	ADC1->CR1 = 0; 										// ???????? ??????? ??????????
	ADC1->SQR1 = 0; 									// ???????? ??????? SQR1
	ADC1->CR2 |= ADC_CR2_CAL; 							// ???? ??????????

	while (!(ADC1->CR2 & ADC_CR2_CAL)){}; 				// ????? ????????? ??????????




	RCC->AHBENR |= RCC_AHBENR_DMA1EN; 					// ????????? ???????????? ??????? DMA ??????
	DMA1_Channel1->CPAR = ADC1_DR_Address; 				// ????????? ????? ????????? - ??????? ?????????? ?????????????? ??? ??? ?????????? ???????
	DMA1_Channel1->CMAR = (uint32_t)dma_adc; 			// ?????? ????? ?????? - ??????? ????? ??????? ? RAM // dma_adc array_mb
	DMA1_Channel1->CCR &= ~DMA_CCR1_DIR; 				// ????????? ??????????? ???????? ??????, ?? ????????? ? ??????
	DMA1_Channel1->CNDTR = DMA_BUFF_SIZE; 				// ?????????? ???????????? ????????
	DMA1_Channel1->CCR &= ~DMA_CCR1_PINC; 				// ????? ????????? ?? ?????????????? ????? ?????? ?????????
	DMA1_Channel1->CCR |= DMA_CCR1_MINC; 				// ????? ?????? ?????????????? ????? ?????? ?????????.
	DMA1_Channel1->CCR |= DMA_CCR1_PSIZE_0; 			// ??????????? ?????? ????????? - 16 ???
	DMA1_Channel1->CCR |= DMA_CCR1_MSIZE_0; 			// ??????????? ?????? ?????? - 16 ???
	DMA1_Channel1->CCR |= DMA_CCR1_PL; 					// ????????? - ????? ???????
	DMA1_Channel1->CCR |= DMA_CCR1_CIRC; 				// ????????? ?????? DMA ? ??????????? ??????
	DMA1_Channel1->CCR |= DMA_CCR1_EN; 					// ????????? ?????? 1-?? ?????? DMA



														// GPIOA.0	 Isens 	- ???? ? ???????? ???????					// 0
														// GPIOA.4	 Uzpt 	- ????, ?????????? ????? ??????????? ????	// 100
														// GPIOA.5	 Un 	- ???????? ??????????. ???????? ?????		// 101
														// GPIOA.6	 Ibreak - ??? ???????. ???????? ???????.			// 110

	ADC1->SQR1 =  ADC_SQR1_L_1 | ADC_SQR1_L_0;			// 4 ?????????? ??????? ??? ????????????
	ADC1->SQR3 = ADC_SQR3_SQ2_2 | ADC_SQR3_SQ3_2 | ADC_SQR3_SQ3_0 | ADC_SQR3_SQ4_2 |  ADC_SQR3_SQ4_1;

	ADC1->SMPR2 = 0x00000FFF;							// ????? ??????? 4-? ??????? ? ??????


	//NVIC->ISER[0] |= NVIC_ISER_SETENA_18;				// ????????? ?????????? ?? ???
	//NVIC_EnableIRQ (ADC1_2_IRQn);						// ????????? ?????????? ?? ???

	//????????? ?????????? ?? ????????? ??????????????
	ADC1->CR1 |= ADC_CR1_EOCIE;

	ADC1->CR2 = ADC_CR2_EXTSEL; 						// ??????? ?????????? ??????? ?????? SWSTART
	ADC1->CR2 |= ADC_CR2_EXTTRIG; 						// ????????? ??????? ?????? ??????????? ??????
	ADC1->CR2 |= ADC_CR2_CONT; 							// ???????? ????? ???????????? ??????????????

	ADC1->CR1 |= ADC_CR1_SCAN; 							// ???????? ????? ???????????? ?????????? ???????

	//ADC1->CR1 |= ADC_CR1_DISCEN;						// ??????????? ?????
	ADC1->CR2 |= ADC_CR2_ADON; 							// ???????? ???
	ADC1->CR2 |= ADC_CR2_ADON; 							// ???????? ???
	ADC1->CR2 |= ADC_CR2_SWSTART;  						// ???? ?????????????? ?????????? ??????

	ADC1->CR2 |=  ADC_CR2_DMA;                   		// Bit 8 DMA: Direct memory access mode (for single ADC mode)


	NVIC_EnableIRQ (DMA1_Channel1_IRQn);				// ????????? ?????????? ??????? ?????? ???
	DMA1_Channel1->CCR |= DMA_CCR1_TCIE;				// ????????? ?????????? ?? ?????????? ??????
	DMA1_Channel1->CCR |= DMA_CCR1_HTIE;				// ????????? ?????????? ?? ?????????? ???????? ??????


} // ?????? ?????????????


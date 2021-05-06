#include "stm32f10x.h"
#include "pwm.h"


// ������ ����������
unsigned long count_test = 0;
int triger = 0;





void init_timer3(void)						// ��������� ������� 3. ����� ��� �������� ��������
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->CR1 |= TIM_CR1_ARPE; 				//shadow
	TIM3->CR1 |= TIM_CR1_DIR;

	TIM3->PSC = 71;							// 72MHz/(PSC+1)	���� ���  - 1 ���
	TIM3->ARR = 500;						// 1000 - 1 ��
											// 50 - 55,55 ��
											// 25 - 111,111 ��

	TIM3->DIER |= TIM_DIER_UIE;
	TIM3->CR1 |= TIM_CR1_CEN;

	NVIC_EnableIRQ (TIM3_IRQn);
}


void TIM3_IRQHandler (void)									// ���������� ������� ��� ������� ������� ������� ������
{
	unsigned short status;
	status = TIM3->SR;
	TIM3->SR = ~status;

	if(status & TIM_SR_UIF)
	{


			if(triger == 0)
			{
				triger = 1;

				relay_2(on);

			}
			else
			{
				triger = 0;

				relay_2(off);

			}



	}
} // ������ TIM2_IRQHandler








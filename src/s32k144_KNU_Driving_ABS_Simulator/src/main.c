#include "device_registers.h"
#include "clocks_and_modes.h"

int lpit0_ch0_flag_counter = 0; /*< LPIT0 timeout counter */
unsigned int j = 0; 			/*FND select pin index */
unsigned int Delaytime = 0; 	/* Delay Time Setting Variable*/

// 7SEG 관련 변수
unsigned int num,num0,num1 = 0;
unsigned int FND_SEL[4]={ 0x001,0x002 };
unsigned int FND_DATA[10] =
{ 0x53C, 0x018, 0x92C, 0x83C, 0xC18, 0xC34, 0xD34, 0x41C, 0xD3C, 0xC1C };
unsigned int CLEAR_SEL = 0x003, CLEAR_DATA = 0xD3C;


void GPIO_PORT_INIT()
{
    // GPIO PCC 설정 - CGC 마스킹
    PCC->PCCn[PCC_PORTA_INDEX] |= PCC_PCCn_CGC_MASK;

    /* GPIOA PDDR 설정
    // INPUT: PA6 PA7 PA8 PA9
    // OUTPUT: PA11 PA12 PA13
    */
    PTA->PDDR &= ~((1<<6)|(1<<7)|(1<<8)|(1<<9));
    PTA->PDDR |= (1<<11)|(1<<12)|(1<<13);

    // PORTA: PA6 PA7 PA8 PA9 PA11 PA12 PA13 MUX 설정
    PORTA->PCR[6] |= PORT_PCR_MUX(1);
    PORTA->PCR[7] |= PORT_PCR_MUX(1);
    PORTA->PCR[8] |= PORT_PCR_MUX(1);
    PORTA->PCR[9] |= PORT_PCR_MUX(1);
    PORTA->PCR[11] |= PORT_PCR_MUX(1);
    PORTA->PCR[12] |= PORT_PCR_MUX(1);
    PORTA->PCR[13] |= PORT_PCR_MUX(1);
}

void LPIT0_init (uint32_t delay){
    uint32_t timeout;
    /*!
     * LPIT Clocking:
     * ==============================
     */
   PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);    /* Clock Src = 6 (SPLL2_DIV2_CLK)*/
   PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clk to LPIT0 regs       */

   /*!
    * LPIT Initialization:
    */
   LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;
   /* DBG_EN-0: Timer chans stop in Debug mode */
   /* DOZE_EN=0: Timer chans are stopped in DOZE mode */
   /* SW_RST=0: SW reset does not reset timer chans, regs */
   /* M_CEN=1: enable module clk (allows writing other LPIT0 regs) */

   timeout=delay * 40000;
   LPIT0->TMR[0].TVAL = timeout;      /* Chan 0 Timeout period: 40M clocks */
   LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_T_EN_MASK;
   /* T_EN=1: Timer channel is enabled */
   /* CHAIN=0: channel chaining is disabled */
   /* MODE=0: 32 periodic counter mode */
   /* TSOT=0: Timer decrements immediately based on restart */
   /* TSOI=0: Timer does not stop after timeout */
   /* TROT=0 Timer will not reload on trigger */
   /* TRG_SRC=0: External trigger soruce */
   /* TRG_SEL=0: Timer chan 0 trigger source is selected*/
}

void WDOG_disable (void){
	WDOG->CNT=0xD928C520;     /* Unlock watchdog */
	WDOG->TOVAL=0x0000FFFF;   /* Maximum timeout value */
	WDOG->CS = 0x00002100;    /* Disable watchdog */
}


void delay_ms (uint32_t ms){
	LPIT0_init(ms);           /* Initialize PIT0 for delay time, timeout  */
    while (0 == (LPIT0->MSR & LPIT_MSR_TIF0_MASK)) {} /* Wait for LPIT0 CH0 Flag */
    lpit0_ch0_flag_counter++;         /* Increment LPIT0 timeout counter */
    LPIT0->MSR |= LPIT_MSR_TIF0_MASK; /* Clear LPIT0 timer flag 0 */
}

void SEG_PORT_INIT()
{
	// 7SEG PCC 설정 - CGC 마스킹
    PCC->PCCn[PCC_PORTB_INDEX] |= PCC_PCCn_CGC_MASK;

    /* 7SEG PDDR 설정
    // OUTPUT: PB0 PB1 PB2 PB3 PB4 PB5 PB8 PB10 PB10
    */
    PTB->PDDR |= (1<<0)|(1<<1)|(1<<2)|(1<<3)|(1<<4)|
    			 (1<<5)|(1<<8)|(1<<10)|(1<<11);

    // PORTB: PB0 PB1 PB2 PB3 PB4 PB5 PB8 PB10 PB11 MUX 설정
    PORTB->PCR[0] |= PORT_PCR_MUX(1);
    PORTB->PCR[1] |= PORT_PCR_MUX(1);
    PORTB->PCR[2] |= PORT_PCR_MUX(1);
    PORTB->PCR[3] |= PORT_PCR_MUX(1);
    PORTB->PCR[4] |= PORT_PCR_MUX(1);
    PORTB->PCR[5] |= PORT_PCR_MUX(1);
	PORTB->PCR[8] |= PORT_PCR_MUX(1);
	PORTB->PCR[10] |= PORT_PCR_MUX(1);
	PORTB->PCR[11] |= PORT_PCR_MUX(1);
}

void Seg_out(int number){
	Delaytime = 1;

	num1=(number/10)%10;
	num0= number%10;

	// 10자리수 출력
	PTB->PSOR = FND_SEL[j];
	PTB->PCOR = CLEAR_SEL;
	PTB->PSOR = FND_DATA[num1];
	delay_ms(Delaytime);
	PTB->PCOR = CLEAR_DATA;
	j++;

	// 1자리수 출력
	PTB->PSOR = FND_SEL[j];
	PTB->PCOR = CLEAR_SEL;
	PTB->PSOR = FND_DATA[num0];
	delay_ms(Delaytime);
	PTB->PCOR = CLEAR_DATA;
	j=0;
}



void TEST()
{
	if(!(PTA->PDIR & (1<<6)))
	    PTA->PCOR |= (1<<11);
	else
	    PTA->PSOR |= (1<<11);

	if(!(PTA->PDIR & (1<<7)))
		PTA->PCOR |= (1<<12);
	else
	    PTA->PSOR |= (1<<12);

    if(!(PTA->PDIR & (1<<8)))
        PTA->PCOR |= (1<<13);
    else
        PTA->PSOR |= (1<<13);
}

// test
int main(void){
	WDOG_disable();        /* Disable WatchDogs*/
    SOSC_init_8MHz();      /* Initialize system oscilator for 8 MHz xtal */
    SPLL_init_160MHz();    /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
    NormalRUNmode_80MHz(); /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
    SystemCoreClockUpdate();

    GPIO_PORT_INIT();
    SEG_PORT_INIT();

    int num = 88;
    while(1)
    {
        TEST();
        Seg_out(num);
        delay_ms(1000);
    }
}

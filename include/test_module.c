#include "S32K144.h"
#include "device_registers.h"           // Device header

// test
unsigned int state_ = 0;

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

	if(state_ == 1)
	{
		PTA->PCOR |= (1<<11);
		PTA->PSOR |= (1<<12);
		PTA->PSOR |= (1<<13);
	}else if(state_ == 2)
	{
		PTA->PSOR |= (1<<11);
		PTA->PCOR |= (1<<12);
		PTA->PSOR |= (1<<13);
	}else if(state_ == 3)
	{
		PTA->PSOR |= (1<<11);
		PTA->PSOR |= (1<<12);
		PTA->PCOR |= (1<<13);
	}
}

void TEST2()
{
	PTC->PCOR |= (1<<0);
	PTC->PCOR |= (1<<1);
	PTC->PCOR |= (1<<2);
}

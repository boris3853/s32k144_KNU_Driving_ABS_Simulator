#include "S32K144.h"
#include "device_registers.h"

void SERVO_init (void)
{
	/*!
	 * Pins definitions
	 * ===================================================
	 *
	 * Pin number        | Function
	 * ----------------- |------------------
	 * PTD16			 | FTM0CH1
	 */
  PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;   /* Enable clock for PORTD */
  PORTD->PCR[16] |= PORT_PCR_MUX(2);           		/* Port D16: MUX = ALT2, FTM0CH1 */
}

/*
 * libAT91sam7a1.c
 *
 *  Created on: Aug 12, 2009
 *      Author: johan
 */

#include <dev/arm/at91sam7.h>

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_ConfigureIt
//* \brief Interrupt Handler Initialization
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_AIC_ConfigureIt (
	AT91PS_AIC pAic,  // \arg pointer to the AIC registers
	unsigned int irq_id,     // \arg interrupt number to initialize
	unsigned int priority,   // \arg priority to give to the interrupt
	unsigned int src_type,   // \arg activation and sense of activation
	void (*newHandler) (void) ) // \arg address of the interrupt handler
{
	unsigned int oldHandler;
    unsigned int mask ;

    oldHandler = pAic->AIC_SVR[irq_id];

    mask = 0x1 << irq_id ;
    //* Disable the interrupt on the interrupt controller
    pAic->AIC_IDCR = mask ;
    //* Save the interrupt handler routine pointer and the interrupt priority
    pAic->AIC_SVR[irq_id] = (unsigned int) newHandler ;
    //* Store the Source Mode Register
    pAic->AIC_SMR[irq_id] = src_type | priority  ;
    //* Clear the interrupt on the interrupt controller
    pAic->AIC_ICCR = mask ;

	return oldHandler;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_EnableIt
//* \brief Enable corresponding IT number
//*----------------------------------------------------------------------------
__inline void AT91F_AIC_EnableIt (
	AT91PS_AIC pAic,      // \arg pointer to the AIC registers
	unsigned int irq_id ) // \arg interrupt number to initialize
{
    //* Enable the interrupt on the interrupt controller
    pAic->AIC_IECR = 0x1 << irq_id ;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_DisableIt
//* \brief Disable corresponding IT number
//*----------------------------------------------------------------------------
__inline void AT91F_AIC_DisableIt (
	AT91PS_AIC pAic,      // \arg pointer to the AIC registers
	unsigned int irq_id ) // \arg interrupt number to initialize
{
    unsigned int mask = 0x1 << irq_id;
    //* Disable the interrupt on the interrupt controller
    pAic->AIC_IDCR = mask ;
    //* Clear the interrupt on the Interrupt Controller ( if one is pending )
    pAic->AIC_ICCR = mask ;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_ClearIt
//* \brief Clear corresponding IT number
//*----------------------------------------------------------------------------
__inline void AT91F_AIC_ClearIt (
	AT91PS_AIC pAic,     // \arg pointer to the AIC registers
	unsigned int irq_id) // \arg interrupt number to initialize
{
    //* Clear the interrupt on the Interrupt Controller ( if one is pending )
    pAic->AIC_ICCR = (0x1 << irq_id);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_AcknowledgeIt
//* \brief Acknowledge corresponding IT number
//*----------------------------------------------------------------------------
__inline void AT91F_AIC_AcknowledgeIt (
	AT91PS_AIC pAic)     // \arg pointer to the AIC registers
{
    pAic->AIC_EOICR = pAic->AIC_EOICR;
}


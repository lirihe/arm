#ifndef AT91SAM7A1_H
#define AT91SAM7A1_H

typedef volatile unsigned int AT91_REG;	// Hardware register definition

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Advanced Interrupt Controller
// *****************************************************************************
typedef struct _AT91S_AIC {
	AT91_REG	 AIC_SMR[32]; 	// Source Mode Register
	AT91_REG	 AIC_SVR[32]; 	// Source Vector Register
	AT91_REG	 AIC_IVR; 	// IRQ Vector Register
	AT91_REG	 AIC_FVR; 	// FIQ Vector Register
	AT91_REG	 AIC_ISR; 	// Interrupt Status Register
	AT91_REG	 AIC_IPR; 	// Interrupt Pending Register
	AT91_REG	 AIC_IMR; 	// Interrupt Mask Register
	AT91_REG	 AIC_CISR; 	// Core Interrupt Status Register
	AT91_REG	 Reserved0[2]; 	// 
	AT91_REG	 AIC_IECR; 	// Interrupt Enable Command Register
	AT91_REG	 AIC_IDCR; 	// Interrupt Disable Command Register
	AT91_REG	 AIC_ICCR; 	// Interrupt Clear Command Register
	AT91_REG	 AIC_ISCR; 	// Interrupt Set Command Register
	AT91_REG	 AIC_EOICR; 	// End of Interrupt Command Register
	AT91_REG	 AIC_SPU; 	// Spurious Vector Register
} AT91S_AIC, *AT91PS_AIC;

// -------- AIC_SMR : (AIC Offset: 0x0) Control Register -------- 
#define 	AT91C_AIC_PRIOR       ((unsigned int) 0x7 <<  0) // (AIC) Priority Level
#define 	AT91C_AIC_PRIOR_LOWEST               ((unsigned int) 0x0) // (AIC) Lowest priority level
#define 	AT91C_AIC_PRIOR_HIGHEST              ((unsigned int) 0x7) // (AIC) Highest priority level
#define 	AT91C_AIC_SRCTYPE     ((unsigned int) 0x3 <<  5) // (AIC) Interrupt Source Type
#define 	AT91C_AIC_SRCTYPE_EXT_LOW_LEVEL        ((unsigned int) 0x0 <<  5) // (AIC) External Sources Code Label Low-level Sensitive
#define 	AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL       ((unsigned int) 0x0 <<  5) // (AIC) Internal Sources Code Label High-level Sensitive
#define 	AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE    ((unsigned int) 0x1 <<  5) // (AIC) Internal Sources Code Label Positive Edge triggered
#define 	AT91C_AIC_SRCTYPE_EXT_NEGATIVE_EDGE    ((unsigned int) 0x1 <<  5) // (AIC) External Sources Code Label Negative Edge triggered
#define 	AT91C_AIC_SRCTYPE_HIGH_LEVEL           ((unsigned int) 0x2 <<  5) // (AIC) Internal Or External Sources Code Label High-level Sensitive
#define 	AT91C_AIC_SRCTYPE_POSITIVE_EDGE        ((unsigned int) 0x3 <<  5) // (AIC) Internal Or External Sources Code Label Positive Edge triggered
// -------- AIC_CISR : (AIC Offset: 0x114) AIC Core Interrupt Status Register -------- 
#define 	AT91C_AIC_NFIQ        ((unsigned int) 0x1 <<  0) // (AIC) NFIQ Status
#define 	AT91C_AIC_NIRQ        ((unsigned int) 0x1 <<  1) // (AIC) NIRQ Status

// *****************************************************************************
//               INTERRUPT VECTORS FOR AT91SAM7A1
// *****************************************************************************
#define AT91C_ID_FIQ				0
#define AT91C_ID_SWIRQ0				1
#define AT91C_ID_WD					2
#define AT91C_ID_WT					3
#define AT91C_ID_USART0				4
#define AT91C_ID_USART1				5
#define AT91C_ID_USART2				6
#define AT91C_ID_SPI				7
#define AT91C_ID_SWIRQ1				8
#define AT91C_ID_SWIRQ2				9
#define AT91C_ID_ADC0				10
#define AT91C_ID_SWIRQ3				11
#define AT91C_ID_GPT0CH0			12
#define AT91C_ID_GPT0CH1			13
#define AT91C_ID_GPT0CH2			14
#define AT91C_ID_SWIRQ4				15
#define AT91C_ID_PWM				16
#define AT91C_ID_SWIRQ5				17
#define AT91C_ID_SWIRQ6				18
#define AT91C_ID_SWIRQ7				19
#define AT91C_ID_CAN				20
#define AT91C_ID_UPIO				21
#define AT91C_ID_CAPT0				22
#define AT91C_ID_CAPT1				23
#define AT91C_ID_ST0				24
#define AT91C_ID_ST1				25
#define AT91C_ID_SWIRQ8				26
#define AT91C_ID_SWIRQ9				27
#define AT91C_ID_IRQ0				28
#define AT91C_ID_SWIRQ10			29
#define AT91C_ID_SWIRQ11			30
#define AT91C_ID_SWIRQ12			31

#define AT91C_AIC_BRANCH_OPCODE ((void (*) ()) 0xE51FFF20) // ldr, pc, [pc, #-&F20]

extern unsigned int AT91F_AIC_ConfigureIt();
extern void AT91F_AIC_EnableIt();
extern void AT91F_AIC_DisableIt();
extern void AT91F_AIC_ClearIt();
extern void AT91F_AIC_AcknowledgeIt();
extern void low_level_init();

/* Clock Manager */
typedef struct {
	AT91_REG	 CM_CE;
	AT91_REG	 CM_CD;
	AT91_REG	 CM_CS;
	AT91_REG	 CM_PST;
	AT91_REG	 CM_PDIV;
	AT91_REG	 CM_OST;
	AT91_REG	 CM_MDIV;
} AT91S_CM, *AT91PS_CM;



/****************************************************************************
* USART Module structure
****************************************************************************/
typedef struct
{
	AT91_REG	 PER;               /* PIO Enable Register            */
	AT91_REG	 PDR;               /* PIO Disable Register           */
	AT91_REG	 PSR;               /* PIO Status Register            */
	AT91_REG	 ReservedA;
	AT91_REG	 OER;               /* Output Enable Register         */
	AT91_REG	 ODR;               /* Output Disable Register        */
	AT91_REG	 OSR;               /* Output Status Register         */
	AT91_REG	 ReservedB[5];
	AT91_REG	 SODR;              /* Set Output Data Register       */
	AT91_REG	 CODR;              /* Clear Output Data Register     */
	AT91_REG	 ODSR;              /* Output Data Status Register    */
	AT91_REG	 PDSR;              /* Pin Data Status Register       */
	AT91_REG	 MDER;              /* Multi-Driver Enable Register   */
	AT91_REG	 MDDR;              /* Multi-Driver Disable Register  */
	AT91_REG	 MDSR;              /* Multi-Driver Status Register   */
	AT91_REG	 ReservedC;
	AT91_REG	 ECR;               /* Enable Clock Register          */
	AT91_REG	 DCR;               /* Disable Clock Register         */
	AT91_REG	 PMSR;              /* Power Managt Status Register   */
	AT91_REG	 ReservedD;
	AT91_REG	 CR;                /* Control Register               */
	AT91_REG	 MR;                /* Mode Register                  */
	AT91_REG	 ReservedE[2];
	AT91_REG	 SR;                /* Status Register                */
	AT91_REG	 IER;               /* Interrupt Enable Register      */
	AT91_REG	 IDR;               /* Interrupt Disable Register     */
	AT91_REG	 IMR;               /* Interrupt Mask Register        */
	AT91_REG	 RHR;               /* Receive Holding Register       */
	AT91_REG	 THR;               /* Transmit Holding Register      */
	AT91_REG	 BRGR;              /* Baud Rate Generator Register   */
	AT91_REG	 RTOR;              /* Receiver Timeout Register      */
	AT91_REG	 TTGR;              /* Transmitter Timeguard Register */
} AT91S_USART, *AT91PS_USART;

typedef volatile struct {
	AT91_REG	 ECR;
	AT91_REG	 DCR;
	AT91_REG	 PMSR;
} AT91S_PMC, *AT91PS_PMC;

typedef volatile struct {
	AT91_REG	 PER;               /* PIO Enable Register            */
	AT91_REG	 PDR;               /* PIO Disable Register           */
	AT91_REG	 PSR;               /* PIO Status Register            */
	AT91_REG	 ReservedA;
	AT91_REG	 OER;               /* Output Enable Register         */
	AT91_REG	 ODR;               /* Output Disable Register        */
	AT91_REG	 OSR;               /* Output Status Register         */
	AT91_REG	 ReservedB[5];
	AT91_REG	 SODR;              /* Set Output Data Register       */
	AT91_REG	 CODR;              /* Clear Output Data Register     */
	AT91_REG	 ODSR;              /* Output Data Status Register    */
	AT91_REG	 PDSR;              /* Pin Data Status Register       */
	AT91_REG	 MDER;              /* Multi-Driver Enable Register   */
	AT91_REG	 MDDR;              /* Multi-Driver Disable Register  */
	AT91_REG	 MDSR;              /* Multi-Driver Status Register   */
	AT91_REG	 ReservedC;
	AT91_REG	 ECR;               /* Enable Clock Register          */
	AT91_REG	 DCR;               /* Disable Clock Register         */
	AT91_REG	 PMSR;              /* Power Managt Status Register   */
	AT91_REG	 ReservedD;
} AT91S_GPT, *AT91PS_GPT;


/****************************************************************************
* PIO controller block PER, PDR, PSR, OER, ODR, OSR, SODR, CODR, ODSR,
*                      PDSR, MDER, MDDR, MDSR, SR, IER, IDR, IMR
****************************************************************************/
#define SCK                (0x01 << 16)  // SCK
#define TXD                (0x01 << 17)  // TXD
#define RXD                (0x01 << 18)  // RXD

/****************************************************************************
* PMC controller block ECR, DCR, PMSR
****************************************************************************/
#define PIO                (0x01 << 0)   // PIO
#define USART              (0x01 << 1)   // USART

/****************************************************************************
* Control register  CR
****************************************************************************/
#define SWRST              (0x01 << 0)   // USART software reset
#define RSTRX              (0x01 << 2)   // Reset receiver
#define RSTTX              (0x01 << 3)   // Reset transmitter
#define RXEN               (0x01 << 4)   // Receiver enable
#define RXDIS              (0x01 << 5)   // Receiver disable
#define TXEN               (0x01 << 6)   // Transmitter enable
#define TXDIS              (0x01 << 7)   // Transmitter disable
#define RSTSTA             (0x01 << 8)   // Reset status bits
#define STTBRK             (0x01 << 9)   // Start break
#define STPBRK             (0x01 << 10)  // Stop break
#define STTTO              (0x01 << 11)  // Start timeout
#define SENDA              (0x01 << 12)  // Send address

/****************************************************************************
* Mode register  MR
****************************************************************************/
#define USCLKS             (0x03 << 4)   // Clock selection mask
#define USCLKS_MCKI        (0x00 << 4)   // Internal clock MCKI
#define USCLKS_MCKI_8      (0x01 << 4)   // Internal clock MCKI/8
#define USCLKS_SCK         (0x02 << 4)   // External clock

#define CHRL               (0x03 << 6)   // Byte length
#define CHRL_5             (0x00 << 6)   // Five bits
#define CHRL_6             (0x01 << 6)   // Six bits
#define CHRL_7             (0x02 << 6)   // Seven bits
#define CHRL_8             (0x03 << 6)   // Heigh bits

#define SYNC               (0x01 << 8)   // Synchronous mode
#define ASYNC              (0x00 << 8)   // Asynchronous mode

#define PAR                (0x07 << 9)   // Parity mode
#define PAR_EVEN           (0x00 << 9)   // Even parity
#define PAR_ODD            (0x01 << 9)   // Odd parity
#define PAR_SPACE          (0x02 << 9)   // Space parity (forced to 0)
#define PAR_MARK           (0x03 << 9)   // Mark parity (forced to 1)
#define PAR_NO             (0x04 << 9)   // No parity
#define PAR_MULTIDROP      (0x06 << 9)   // Multi drop parity

#define IDLE               (0x01 << 10)  // Idle (J1708 Protocol)
#define IDLEFLAG           (0x01 << 11)  // Idle Flag (J1708 Protocol)

#define NBSTOP             (0x03 << 12)  // Stop bit number
#define NBSTOP_1           (0x00 << 12)  // 1 stop bit
#define NBSTOP_15          (0x01 << 12)  // 1.5 stop bit
#define NBSTOP_2           (0x02 << 12)  // 2 stop bit

#define CHMODE             (0x03 << 14)  // Channel mode
#define CHMODE_NORMAL      (0x00 << 14)  // Normal channel
#define CHMODE_AUTO        (0x01 << 14)  // Automatic echo channel
#define CHMODE_LOCAL       (0x02 << 14)  // Local loopback channel
#define CHMODE_REMOTE      (0x03 << 14)  // Remote loopback channel

#define MODE9              (0x01 << 17)  // 9 bit mode
#define MODE8              (0x00 << 17)  // 8 bit mode
#define CLKO               (0x01 << 18)  // Clock output
#define CLKI               (0x00 << 18)  // Clock input

/****************************************************************************
* Channel Status and Interrupt registers  IER, IDR, IMR, SR
****************************************************************************/
#define RXRDY              (0x01 << 0)   // Receiver ready
#define TXRDY              (0x01 << 1)   // Transmitter ready
#define RXBRK              (0x01 << 2)   // Receiver break
#define ENDRX              (0x01 << 3)   // End of receiver PDC transfer
#define ENDTX              (0x01 << 4)   // End of transmitter PDC transfer
#define USOVRE             (0x01 << 5)   // Overrun error
#define FRAME              (0x01 << 6)   // Framing error
#define PARE               (0x01 << 7)   // Parity error
#define TIMEOUT            (0x01 << 8)   // Receiver time out
#define TXEMPTY            (0x01 << 9)   // Transmitter empty
#define IDLE               (0x01 << 10)  // end of J1587 protocol
#define IDLEFLAG           (0x01 << 11)  // Frame being received

#define SCK                (0x01 << 16)  // Serial clock / RS-485 direction
/****************************************************************************
* Time out
****************************************************************************/
#define TRANSMIT_TIMEOUT   0xFF
#define RECEIVE_TIMEOUT    0xFF
#define US_BDR_9600        1

#define GPT_PER_TIOB_MASK 0x10000U
#define GPT_PER_TIOB 0x10000U
#define GPT_PER_TIOB_BIT 16
#define GPT_PER_TIOA_MASK 0x20000U
#define GPT_PER_TIOA 0x20000U
#define GPT_PER_TIOA_BIT 17
#define GPT_PER_TCLK_MASK 0x40000U
#define GPT_PER_TCLK 0x40000U
#define GPT_PER_TCLK_BIT 18

#define ST0_BASE 0xFFFE4000

#define ST0_ECR (*(volatile unsigned long *)0xFFFE4050)
#define ST0_ECR_OFFSET 0x50
#define ST0_ECR_ST_MASK 0x2U
#define ST0_ECR_ST 0x2U
#define ST0_ECR_ST_BIT 1

#define ST0_DCR (*(volatile unsigned long *)0xFFFE4054)
#define ST0_DCR_OFFSET 0x54
#define ST0_DCR_ST_MASK 0x2U
#define ST0_DCR_ST 0x2U
#define ST0_DCR_ST_BIT 1

#define ST0_PMSR (*(volatile unsigned long *)0xFFFE4058)
#define ST0_PMSR_OFFSET 0x58
#define ST0_PMSR_ST_MASK 0x2U
#define ST0_PMSR_ST 0x2U
#define ST0_PMSR_ST_BIT 1

#define ST0_CR (*(volatile unsigned long *)0xFFFE4060)
#define ST0_CR_OFFSET 0x60
#define ST0_CR_SWRST_MASK 0x1U
#define ST0_CR_SWRST 0x1U
#define ST0_CR_SWRST_BIT 0
#define ST0_CR_CHEN0_MASK 0x2U
#define ST0_CR_CHEN0 0x2U
#define ST0_CR_CHEN0_BIT 1
#define ST0_CR_CHDIS0_MASK 0x4U
#define ST0_CR_CHDIS0 0x4U
#define ST0_CR_CHDIS0_BIT 2
#define ST0_CR_CHEN1_MASK 0x8U
#define ST0_CR_CHEN1 0x8U
#define ST0_CR_CHEN1_BIT 3
#define ST0_CR_CHDIS1_MASK 0x10U
#define ST0_CR_CHDIS1 0x10U
#define ST0_CR_CHDIS1_BIT 4

#define ST0_CSR (*(volatile unsigned long *)0xFFFE406C)
#define ST0_CSR_OFFSET 0x6C
#define ST0_CSR_CHEND0_MASK 0x1U
#define ST0_CSR_CHEND0 0x1U
#define ST0_CSR_CHEND0_BIT 0
#define ST0_CSR_CHDIS0_MASK 0x2U
#define ST0_CSR_CHDIS0 0x2U
#define ST0_CSR_CHDIS0_BIT 1
#define ST0_CSR_CHLD0_MASK 0x4U
#define ST0_CSR_CHLD0 0x4U
#define ST0_CSR_CHLD0_BIT 2
#define ST0_CSR_CHEND1_MASK 0x8U
#define ST0_CSR_CHEND1 0x8U
#define ST0_CSR_CHEND1_BIT 3
#define ST0_CSR_CHDIS1_MASK 0x10U
#define ST0_CSR_CHDIS1 0x10U
#define ST0_CSR_CHDIS1_BIT 4
#define ST0_CSR_CHLD1_MASK 0x20U
#define ST0_CSR_CHLD1 0x20U
#define ST0_CSR_CHLD1_BIT 5

#define ST0_SR (*(volatile unsigned long *)0xFFFE4070)
#define ST0_SR_OFFSET 0x70
#define ST0_SR_CHEND0_MASK 0x1U
#define ST0_SR_CHEND0 0x1U
#define ST0_SR_CHEND0_BIT 0
#define ST0_SR_CHDIS0_MASK 0x2U
#define ST0_SR_CHDIS0 0x2U
#define ST0_SR_CHDIS0_BIT 1
#define ST0_SR_CHLD0_MASK 0x4U
#define ST0_SR_CHLD0 0x4U
#define ST0_SR_CHLD0_BIT 2
#define ST0_SR_CHEND1_MASK 0x8U
#define ST0_SR_CHEND1 0x8U
#define ST0_SR_CHEND1_BIT 3
#define ST0_SR_CHDIS1_MASK 0x10U
#define ST0_SR_CHDIS1 0x10U
#define ST0_SR_CHDIS1_BIT 4
#define ST0_SR_CHLD1_MASK 0x20U
#define ST0_SR_CHLD1 0x20U
#define ST0_SR_CHLD1_BIT 5
#define ST0_SR_CHENS0_MASK 0x1000000U
#define ST0_SR_CHENS0 0x1000000U
#define ST0_SR_CHENS0_BIT 24
#define ST0_SR_CHENS1_MASK 0x2000000U
#define ST0_SR_CHENS1 0x2000000U
#define ST0_SR_CHENS1_BIT 25

#define ST0_IER (*(volatile unsigned long *)0xFFFE4074)
#define ST0_IER_OFFSET 0x74
#define ST0_IER_CHEND0_MASK 0x1U
#define ST0_IER_CHEND0 0x1U
#define ST0_IER_CHEND0_BIT 0
#define ST0_IER_CHDIS0_MASK 0x2U
#define ST0_IER_CHDIS0 0x2U
#define ST0_IER_CHDIS0_BIT 1
#define ST0_IER_CHLD0_MASK 0x4U
#define ST0_IER_CHLD0 0x4U
#define ST0_IER_CHLD0_BIT 2
#define ST0_IER_CHEND1_MASK 0x8U
#define ST0_IER_CHEND1 0x8U
#define ST0_IER_CHEND1_BIT 3
#define ST0_IER_CHDIS1_MASK 0x10U
#define ST0_IER_CHDIS1 0x10U
#define ST0_IER_CHDIS1_BIT 4
#define ST0_IER_CHLD1_MASK 0x20U
#define ST0_IER_CHLD1 0x20U
#define ST0_IER_CHLD1_BIT 5

#define ST0_IDR (*(volatile unsigned long *)0xFFFE4078)
#define ST0_IDR_OFFSET 0x78
#define ST0_IDR_CHEND0_MASK 0x1U
#define ST0_IDR_CHEND0 0x1U
#define ST0_IDR_CHEND0_BIT 0
#define ST0_IDR_CHDIS0_MASK 0x2U
#define ST0_IDR_CHDIS0 0x2U
#define ST0_IDR_CHDIS0_BIT 1
#define ST0_IDR_CHLD0_MASK 0x4U
#define ST0_IDR_CHLD0 0x4U
#define ST0_IDR_CHLD0_BIT 2
#define ST0_IDR_CHEND1_MASK 0x8U
#define ST0_IDR_CHEND1 0x8U
#define ST0_IDR_CHEND1_BIT 3
#define ST0_IDR_CHDIS1_MASK 0x10U
#define ST0_IDR_CHDIS1 0x10U
#define ST0_IDR_CHDIS1_BIT 4
#define ST0_IDR_CHLD1_MASK 0x20U
#define ST0_IDR_CHLD1 0x20U
#define ST0_IDR_CHLD1_BIT 5

#define ST0_IMR (*(volatile unsigned long *)0xFFFE407C)
#define ST0_IMR_OFFSET 0x7C
#define ST0_IMR_CHEND0_MASK 0x1U
#define ST0_IMR_CHEND0 0x1U
#define ST0_IMR_CHEND0_BIT 0
#define ST0_IMR_CHDIS0_MASK 0x2U
#define ST0_IMR_CHDIS0 0x2U
#define ST0_IMR_CHDIS0_BIT 1
#define ST0_IMR_CHLD0_MASK 0x4U
#define ST0_IMR_CHLD0 0x4U
#define ST0_IMR_CHLD0_BIT 2
#define ST0_IMR_CHEND1_MASK 0x8U
#define ST0_IMR_CHEND1 0x8U
#define ST0_IMR_CHEND1_BIT 3
#define ST0_IMR_CHDIS1_MASK 0x10U
#define ST0_IMR_CHDIS1 0x10U
#define ST0_IMR_CHDIS1_BIT 4
#define ST0_IMR_CHLD1_MASK 0x20U
#define ST0_IMR_CHLD1 0x20U
#define ST0_IMR_CHLD1_BIT 5

#define ST0_PR0 (*(volatile unsigned long *)0xFFFE4080)
#define ST0_PR0_OFFSET 0x80
#define ST0_PR0_PRESCALAR_MASK 0xFU
#define ST0_PR0_PRESCALAR_BIT 0
#define ST0_PR0_SELECTCLK_MASK 0x10U
#define ST0_PR0_SELECTCLK 0x10U
#define ST0_PR0_SELECTCLK_BIT 4
#define ST0_PR0_AUTOREL_MASK 0x20U
#define ST0_PR0_AUTOREL 0x20U
#define ST0_PR0_AUTOREL_BIT 5
#define ST0_PR0_SYSCAL_MASK 0x7FF00U
#define ST0_PR0_SYSCAL_BIT 8

#define ST0_CT0 (*(volatile unsigned long *)0xFFFE4084)
#define ST0_CT0_OFFSET 0x84
#define ST0_CT0_LOAD_MASK 0xFFFFU
#define ST0_CT0_LOAD_BIT 0

#define ST0_PR1 (*(volatile unsigned long *)0xFFFE4088)
#define ST0_PR1_OFFSET 0x88
#define ST0_PR1_PRESCALAR_MASK 0xFU
#define ST0_PR1_PRESCALAR_BIT 0
#define ST0_PR1_SELECTCLK_MASK 0x10U
#define ST0_PR1_SELECTCLK 0x10U
#define ST0_PR1_SELECTCLK_BIT 4
#define ST0_PR1_AUTOREL_MASK 0x20U
#define ST0_PR1_AUTOREL 0x20U
#define ST0_PR1_AUTOREL_BIT 5
#define ST0_PR1_SYSCAL_MASK 0x7FF00U
#define ST0_PR1_SYSCAL_BIT 8

#define ST0_CT1 (*(volatile unsigned long *)0xFFFE408C)
#define ST0_CT1_OFFSET 0x8C
#define ST0_CT1_LOAD_MASK 0xFFFFU
#define ST0_CT1_LOAD_BIT 0

#define ST0_CCV0 (*(volatile unsigned long *)0xFFFE4200)
#define ST0_CCV0_OFFSET 0x200
#define ST0_CCV0_COUNT_MASK 0xFFFFU
#define ST0_CCV0_COUNT_BIT 0

#define ST0_CCV1 (*(volatile unsigned long *)0xFFFE4204)
#define ST0_CCV1_OFFSET 0x204
#define ST0_CCV1_COUNT_MASK 0xFFFFU
#define ST0_CCV1_COUNT_BIT 0


#define PDC_BASE 0xFFFF8000

#define PDC_PRA0 (*(volatile unsigned long *)0xFFFF8080)
#define PDC_PRA0_OFFSET 0x80
#define PDC_PRA0_CHPRA_MASK 0x0U
#define PDC_PRA0_CHPRA_BIT 0

#define PDC_CR0 (*(volatile unsigned long *)0xFFFF8084)
#define PDC_CR0_OFFSET 0x84
#define PDC_CR0_DIR_MASK 0x1U
#define PDC_CR0_DIR 0x1U
#define PDC_CR0_DIR_BIT 0
#define PDC_CR0_SIZE_MASK 0x6U
#define PDC_CR0_SIZE_BIT 1

#define PDC_MPR0 (*(volatile unsigned long *)0xFFFF8088)
#define PDC_MPR0_OFFSET 0x88
#define PDC_MPR0_CHPTR_MASK 0x0U
#define PDC_MPR0_CHPTR_BIT 0

#define PDC_TCR0 (*(volatile unsigned long *)0xFFFF808C)
#define PDC_TCR0_OFFSET 0x8C
#define PDC_TCR0_CHCTR_MASK 0xFFFFU
#define PDC_TCR0_CHCTR_BIT 0

#define PDC_PRA1 (*(volatile unsigned long *)0xFFFF8090)
#define PDC_PRA1_OFFSET 0x90
#define PDC_PRA1_CHPRA_MASK 0x0U
#define PDC_PRA1_CHPRA_BIT 0

#define PDC_CR1 (*(volatile unsigned long *)0xFFFF8094)
#define PDC_CR1_OFFSET 0x94
#define PDC_CR1_DIR_MASK 0x1U
#define PDC_CR1_DIR 0x1U
#define PDC_CR1_DIR_BIT 0
#define PDC_CR1_SIZE_MASK 0x6U
#define PDC_CR1_SIZE_BIT 1

#define PDC_MPR1 (*(volatile unsigned long *)0xFFFF8098)
#define PDC_MPR1_OFFSET 0x98
#define PDC_MPR1_CHPTR_MASK 0x0U
#define PDC_MPR1_CHPTR_BIT 0

#define PDC_TCR1 (*(volatile unsigned long *)0xFFFF809C)
#define PDC_TCR1_OFFSET 0x9C
#define PDC_TCR1_CHCTR_MASK 0xFFFFU
#define PDC_TCR1_CHCTR_BIT 0

#define PDC_PRA2 (*(volatile unsigned long *)0xFFFF80A0)
#define PDC_PRA2_OFFSET 0xA0
#define PDC_PRA2_CHPRA_MASK 0x0U
#define PDC_PRA2_CHPRA_BIT 0

#define PDC_CR2 (*(volatile unsigned long *)0xFFFF80A4)
#define PDC_CR2_OFFSET 0xA4
#define PDC_CR2_DIR_MASK 0x1U
#define PDC_CR2_DIR 0x1U
#define PDC_CR2_DIR_BIT 0
#define PDC_CR2_SIZE_MASK 0x6U
#define PDC_CR2_SIZE_BIT 1

#define PDC_MPR2 (*(volatile unsigned long *)0xFFFF80A8)
#define PDC_MPR2_OFFSET 0xA8
#define PDC_MPR2_CHPTR_MASK 0x0U
#define PDC_MPR2_CHPTR_BIT 0

#define PDC_TCR2 (*(volatile unsigned long *)0xFFFF80AC)
#define PDC_TCR2_OFFSET 0xAC
#define PDC_TCR2_CHCTR_MASK 0xFFFFU
#define PDC_TCR2_CHCTR_BIT 0

#define PDC_PRA3 (*(volatile unsigned long *)0xFFFF80B0)
#define PDC_PRA3_OFFSET 0xB0
#define PDC_PRA3_CHPRA_MASK 0x0U
#define PDC_PRA3_CHPRA_BIT 0

#define PDC_CR3 (*(volatile unsigned long *)0xFFFF80B4)
#define PDC_CR3_OFFSET 0xB4
#define PDC_CR3_DIR_MASK 0x1U
#define PDC_CR3_DIR 0x1U
#define PDC_CR3_DIR_BIT 0
#define PDC_CR3_SIZE_MASK 0x6U
#define PDC_CR3_SIZE_BIT 1

#define PDC_MPR3 (*(volatile unsigned long *)0xFFFF80B8)
#define PDC_MPR3_OFFSET 0xB8
#define PDC_MPR3_CHPTR_MASK 0x0U
#define PDC_MPR3_CHPTR_BIT 0

#define PDC_TCR3 (*(volatile unsigned long *)0xFFFF80BC)
#define PDC_TCR3_OFFSET 0xBC
#define PDC_TCR3_CHCTR_MASK 0xFFFFU
#define PDC_TCR3_CHCTR_BIT 0

#define PDC_PRA4 (*(volatile unsigned long *)0xFFFF80C0)
#define PDC_PRA4_OFFSET 0xC0
#define PDC_PRA4_CHPRA_MASK 0x0U
#define PDC_PRA4_CHPRA_BIT 0

#define PDC_CR4 (*(volatile unsigned long *)0xFFFF80C4)
#define PDC_CR4_OFFSET 0xC4
#define PDC_CR4_DIR_MASK 0x1U
#define PDC_CR4_DIR 0x1U
#define PDC_CR4_DIR_BIT 0
#define PDC_CR4_SIZE_MASK 0x6U
#define PDC_CR4_SIZE_BIT 1

#define PDC_MPR4 (*(volatile unsigned long *)0xFFFF80C8)
#define PDC_MPR4_OFFSET 0xC8
#define PDC_MPR4_CHPTR_MASK 0x0U
#define PDC_MPR4_CHPTR_BIT 0

#define PDC_TCR4 (*(volatile unsigned long *)0xFFFF80CC)
#define PDC_TCR4_OFFSET 0xCC
#define PDC_TCR4_CHCTR_MASK 0xFFFFU
#define PDC_TCR4_CHCTR_BIT 0

#define PDC_PRA5 (*(volatile unsigned long *)0xFFFF80D0)
#define PDC_PRA5_OFFSET 0xD0
#define PDC_PRA5_CHPRA_MASK 0x0U
#define PDC_PRA5_CHPRA_BIT 0

#define PDC_CR5 (*(volatile unsigned long *)0xFFFF80D4)
#define PDC_CR5_OFFSET 0xD4
#define PDC_CR5_DIR_MASK 0x1U
#define PDC_CR5_DIR 0x1U
#define PDC_CR5_DIR_BIT 0
#define PDC_CR5_SIZE_MASK 0x6U
#define PDC_CR5_SIZE_BIT 1

#define PDC_MPR5 (*(volatile unsigned long *)0xFFFF80D8)
#define PDC_MPR5_OFFSET 0xD8
#define PDC_MPR5_CHPTR_MASK 0x0U
#define PDC_MPR5_CHPTR_BIT 0

#define PDC_TCR5 (*(volatile unsigned long *)0xFFFF80DC)
#define PDC_TCR5_OFFSET 0xDC
#define PDC_TCR5_CHCTR_MASK 0xFFFFU
#define PDC_TCR5_CHCTR_BIT 0

#define PDC_PRA6 (*(volatile unsigned long *)0xFFFF80E0)
#define PDC_PRA6_OFFSET 0xE0
#define PDC_PRA6_CHPRA_MASK 0x0U
#define PDC_PRA6_CHPRA_BIT 0

#define PDC_CR6 (*(volatile unsigned long *)0xFFFF80E4)
#define PDC_CR6_OFFSET 0xE4
#define PDC_CR6_DIR_MASK 0x1U
#define PDC_CR6_DIR 0x1U
#define PDC_CR6_DIR_BIT 0
#define PDC_CR6_SIZE_MASK 0x6U
#define PDC_CR6_SIZE_BIT 1

#define PDC_MPR6 (*(volatile unsigned long *)0xFFFF80E8)
#define PDC_MPR6_OFFSET 0xE8
#define PDC_MPR6_CHPTR_MASK 0x0U
#define PDC_MPR6_CHPTR_BIT 0

#define PDC_TCR6 (*(volatile unsigned long *)0xFFFF80EC)
#define PDC_TCR6_OFFSET 0xEC
#define PDC_TCR6_CHCTR_MASK 0xFFFFU
#define PDC_TCR6_CHCTR_BIT 0

#define PDC_PRA7 (*(volatile unsigned long *)0xFFFF80F0)
#define PDC_PRA7_OFFSET 0xF0
#define PDC_PRA7_CHPRA_MASK 0x0U
#define PDC_PRA7_CHPRA_BIT 0

#define PDC_CR7 (*(volatile unsigned long *)0xFFFF80F4)
#define PDC_CR7_OFFSET 0xF4
#define PDC_CR7_DIR_MASK 0x1U
#define PDC_CR7_DIR 0x1U
#define PDC_CR7_DIR_BIT 0
#define PDC_CR7_SIZE_MASK 0x6U
#define PDC_CR7_SIZE_BIT 1

#define PDC_MPR7 (*(volatile unsigned long *)0xFFFF80F8)
#define PDC_MPR7_OFFSET 0xF8
#define PDC_MPR7_CHPTR_MASK 0x0U
#define PDC_MPR7_CHPTR_BIT 0

#define PDC_TCR7 (*(volatile unsigned long *)0xFFFF80FC)
#define PDC_TCR7_OFFSET 0xFC
#define PDC_TCR7_CHCTR_MASK 0xFFFFU
#define PDC_TCR7_CHCTR_BIT 0

#define PDC_PRA8 (*(volatile unsigned long *)0xFFFF8100)
#define PDC_PRA8_OFFSET 0x100
#define PDC_PRA8_CHPRA_MASK 0x0U
#define PDC_PRA8_CHPRA_BIT 0

#define PDC_CR8 (*(volatile unsigned long *)0xFFFF8104)
#define PDC_CR8_OFFSET 0x104
#define PDC_CR8_DIR_MASK 0x1U
#define PDC_CR8_DIR 0x1U
#define PDC_CR8_DIR_BIT 0
#define PDC_CR8_SIZE_MASK 0x6U
#define PDC_CR8_SIZE_BIT 1

#define PDC_MPR8 (*(volatile unsigned long *)0xFFFF8108)
#define PDC_MPR8_OFFSET 0x108
#define PDC_MPR8_CHPTR_MASK 0x0U
#define PDC_MPR8_CHPTR_BIT 0

#define PDC_TCR8 (*(volatile unsigned long *)0xFFFF810C)
#define PDC_TCR8_OFFSET 0x10C
#define PDC_TCR8_CHCTR_MASK 0xFFFFU
#define PDC_TCR8_CHCTR_BIT 0

#define PDC_PRA9 (*(volatile unsigned long *)0xFFFF8110)
#define PDC_PRA9_OFFSET 0x110
#define PDC_PRA9_CHPRA_MASK 0x0U
#define PDC_PRA9_CHPRA_BIT 0

#define PDC_CR9 (*(volatile unsigned long *)0xFFFF8114)
#define PDC_CR9_OFFSET 0x114
#define PDC_CR9_DIR_MASK 0x1U
#define PDC_CR9_DIR 0x1U
#define PDC_CR9_DIR_BIT 0
#define PDC_CR9_SIZE_MASK 0x6U
#define PDC_CR9_SIZE_BIT 1

#define PDC_MPR9 (*(volatile unsigned long *)0xFFFF8118)
#define PDC_MPR9_OFFSET 0x118
#define PDC_MPR9_CHPTR_MASK 0x0U
#define PDC_MPR9_CHPTR_BIT 0

#define PDC_TCR9 (*(volatile unsigned long *)0xFFFF811C)
#define PDC_TCR9_OFFSET 0x11C
#define PDC_TCR9_CHCTR_MASK 0xFFFFU
#define PDC_TCR9_CHCTR_BIT 0

#define PDC_PRA10 (*(volatile unsigned long *)0xFFFF8120)
#define PDC_PRA10_OFFSET 0x120
#define PDC_PRA10_CHPRA_MASK 0x0U
#define PDC_PRA10_CHPRA_BIT 0

#define PDC_CR10 (*(volatile unsigned long *)0xFFFF8124)
#define PDC_CR10_OFFSET 0x124
#define PDC_CR10_DIR_MASK 0x1U
#define PDC_CR10_DIR 0x1U
#define PDC_CR10_DIR_BIT 0
#define PDC_CR10_SIZE_MASK 0x6U
#define PDC_CR10_SIZE_BIT 1

#define PDC_MPR10 (*(volatile unsigned long *)0xFFFF8128)
#define PDC_MPR10_OFFSET 0x128
#define PDC_MPR10_CHPTR_MASK 0x0U
#define PDC_MPR10_CHPTR_BIT 0

#define PDC_TCR10 (*(volatile unsigned long *)0xFFFF812C)
#define PDC_TCR10_OFFSET 0x12C
#define PDC_TCR10_CHCTR_MASK 0xFFFFU
#define PDC_TCR10_CHCTR_BIT 0

/**
 * SPI definitions
 */

#define SPI_BASE 0xFFFB4000

#define SPI_PER (*(volatile unsigned long *)0xFFFB4000)
#define SPI_PER_OFFSET 0x0
#define SPI_PER_SPCK_MASK 0x10000U
#define SPI_PER_SPCK 0x10000U
#define SPI_PER_SPCK_BIT 16
#define SPI_PER_MISO_MASK 0x20000U
#define SPI_PER_MISO 0x20000U
#define SPI_PER_MISO_BIT 17
#define SPI_PER_MOSI_MASK 0x40000U
#define SPI_PER_MOSI 0x40000U
#define SPI_PER_MOSI_BIT 18
#define SPI_PER_NPCS0_MASK 0x80000U
#define SPI_PER_NPCS0 0x80000U
#define SPI_PER_NPCS0_BIT 19
#define SPI_PER_NPCS1_MASK 0x100000U
#define SPI_PER_NPCS1 0x100000U
#define SPI_PER_NPCS1_BIT 20
#define SPI_PER_NPCS2_MASK 0x200000U
#define SPI_PER_NPCS2 0x200000U
#define SPI_PER_NPCS2_BIT 21
#define SPI_PER_NPCS3_MASK 0x400000U
#define SPI_PER_NPCS3 0x400000U
#define SPI_PER_NPCS3_BIT 22

#define SPI_PDR (*(volatile unsigned long *)0xFFFB4004)
#define SPI_PDR_OFFSET 0x4
#define SPI_PDR_SPCK_MASK 0x10000U
#define SPI_PDR_SPCK 0x10000U
#define SPI_PDR_SPCK_BIT 16
#define SPI_PDR_MISO_MASK 0x20000U
#define SPI_PDR_MISO 0x20000U
#define SPI_PDR_MISO_BIT 17
#define SPI_PDR_MOSI_MASK 0x40000U
#define SPI_PDR_MOSI 0x40000U
#define SPI_PDR_MOSI_BIT 18
#define SPI_PDR_NPCS0_MASK 0x80000U
#define SPI_PDR_NPCS0 0x80000U
#define SPI_PDR_NPCS0_BIT 19
#define SPI_PDR_NPCS1_MASK 0x100000U
#define SPI_PDR_NPCS1 0x100000U
#define SPI_PDR_NPCS1_BIT 20
#define SPI_PDR_NPCS2_MASK 0x200000U
#define SPI_PDR_NPCS2 0x200000U
#define SPI_PDR_NPCS2_BIT 21
#define SPI_PDR_NPCS3_MASK 0x400000U
#define SPI_PDR_NPCS3 0x400000U
#define SPI_PDR_NPCS3_BIT 22

#define SPI_PSR (*(volatile unsigned long *)0xFFFB4008)
#define SPI_PSR_OFFSET 0x8
#define SPI_PSR_SPCK_MASK 0x10000U
#define SPI_PSR_SPCK 0x10000U
#define SPI_PSR_SPCK_BIT 16
#define SPI_PSR_MISO_MASK 0x20000U
#define SPI_PSR_MISO 0x20000U
#define SPI_PSR_MISO_BIT 17
#define SPI_PSR_MOSI_MASK 0x40000U
#define SPI_PSR_MOSI 0x40000U
#define SPI_PSR_MOSI_BIT 18
#define SPI_PSR_NPCS0_MASK 0x80000U
#define SPI_PSR_NPCS0 0x80000U
#define SPI_PSR_NPCS0_BIT 19
#define SPI_PSR_NPCS1_MASK 0x100000U
#define SPI_PSR_NPCS1 0x100000U
#define SPI_PSR_NPCS1_BIT 20
#define SPI_PSR_NPCS2_MASK 0x200000U
#define SPI_PSR_NPCS2 0x200000U
#define SPI_PSR_NPCS2_BIT 21
#define SPI_PSR_NPCS3_MASK 0x400000U
#define SPI_PSR_NPCS3 0x400000U
#define SPI_PSR_NPCS3_BIT 22

#define SPI_OER (*(volatile unsigned long *)0xFFFB4010)
#define SPI_OER_OFFSET 0x10
#define SPI_OER_SPCK_MASK 0x10000U
#define SPI_OER_SPCK 0x10000U
#define SPI_OER_SPCK_BIT 16
#define SPI_OER_MISO_MASK 0x20000U
#define SPI_OER_MISO 0x20000U
#define SPI_OER_MISO_BIT 17
#define SPI_OER_MOSI_MASK 0x40000U
#define SPI_OER_MOSI 0x40000U
#define SPI_OER_MOSI_BIT 18
#define SPI_OER_NPCS0_MASK 0x80000U
#define SPI_OER_NPCS0 0x80000U
#define SPI_OER_NPCS0_BIT 19
#define SPI_OER_NPCS1_MASK 0x100000U
#define SPI_OER_NPCS1 0x100000U
#define SPI_OER_NPCS1_BIT 20
#define SPI_OER_NPCS2_MASK 0x200000U
#define SPI_OER_NPCS2 0x200000U
#define SPI_OER_NPCS2_BIT 21
#define SPI_OER_NPCS3_MASK 0x400000U
#define SPI_OER_NPCS3 0x400000U
#define SPI_OER_NPCS3_BIT 22

#define SPI_ODR (*(volatile unsigned long *)0xFFFB4014)
#define SPI_ODR_OFFSET 0x14
#define SPI_ODR_SPCK_MASK 0x10000U
#define SPI_ODR_SPCK 0x10000U
#define SPI_ODR_SPCK_BIT 16
#define SPI_ODR_MISO_MASK 0x20000U
#define SPI_ODR_MISO 0x20000U
#define SPI_ODR_MISO_BIT 17
#define SPI_ODR_MOSI_MASK 0x40000U
#define SPI_ODR_MOSI 0x40000U
#define SPI_ODR_MOSI_BIT 18
#define SPI_ODR_NPCS0_MASK 0x80000U
#define SPI_ODR_NPCS0 0x80000U
#define SPI_ODR_NPCS0_BIT 19
#define SPI_ODR_NPCS1_MASK 0x100000U
#define SPI_ODR_NPCS1 0x100000U
#define SPI_ODR_NPCS1_BIT 20
#define SPI_ODR_NPCS2_MASK 0x200000U
#define SPI_ODR_NPCS2 0x200000U
#define SPI_ODR_NPCS2_BIT 21
#define SPI_ODR_NPCS3_MASK 0x400000U
#define SPI_ODR_NPCS3 0x400000U
#define SPI_ODR_NPCS3_BIT 22

#define SPI_OSR (*(volatile unsigned long *)0xFFFB4018)
#define SPI_OSR_OFFSET 0x18
#define SPI_OSR_SPCK_MASK 0x10000U
#define SPI_OSR_SPCK 0x10000U
#define SPI_OSR_SPCK_BIT 16
#define SPI_OSR_MISO_MASK 0x20000U
#define SPI_OSR_MISO 0x20000U
#define SPI_OSR_MISO_BIT 17
#define SPI_OSR_MOSI_MASK 0x40000U
#define SPI_OSR_MOSI 0x40000U
#define SPI_OSR_MOSI_BIT 18
#define SPI_OSR_NPCS0_MASK 0x80000U
#define SPI_OSR_NPCS0 0x80000U
#define SPI_OSR_NPCS0_BIT 19
#define SPI_OSR_NPCS1_MASK 0x100000U
#define SPI_OSR_NPCS1 0x100000U
#define SPI_OSR_NPCS1_BIT 20
#define SPI_OSR_NPCS2_MASK 0x200000U
#define SPI_OSR_NPCS2 0x200000U
#define SPI_OSR_NPCS2_BIT 21
#define SPI_OSR_NPCS3_MASK 0x400000U
#define SPI_OSR_NPCS3 0x400000U
#define SPI_OSR_NPCS3_BIT 22

#define SPI_SODR (*(volatile unsigned long *)0xFFFB4030)
#define SPI_SODR_OFFSET 0x30
#define SPI_SODR_SPCK_MASK 0x10000U
#define SPI_SODR_SPCK 0x10000U
#define SPI_SODR_SPCK_BIT 16
#define SPI_SODR_MISO_MASK 0x20000U
#define SPI_SODR_MISO 0x20000U
#define SPI_SODR_MISO_BIT 17
#define SPI_SODR_MOSI_MASK 0x40000U
#define SPI_SODR_MOSI 0x40000U
#define SPI_SODR_MOSI_BIT 18
#define SPI_SODR_NPCS0_MASK 0x80000U
#define SPI_SODR_NPCS0 0x80000U
#define SPI_SODR_NPCS0_BIT 19
#define SPI_SODR_NPCS1_MASK 0x100000U
#define SPI_SODR_NPCS1 0x100000U
#define SPI_SODR_NPCS1_BIT 20
#define SPI_SODR_NPCS2_MASK 0x200000U
#define SPI_SODR_NPCS2 0x200000U
#define SPI_SODR_NPCS2_BIT 21
#define SPI_SODR_NPCS3_MASK 0x400000U
#define SPI_SODR_NPCS3 0x400000U
#define SPI_SODR_NPCS3_BIT 22

#define SPI_CODR (*(volatile unsigned long *)0xFFFB4034)
#define SPI_CODR_OFFSET 0x34
#define SPI_CODR_SPCK_MASK 0x10000U
#define SPI_CODR_SPCK 0x10000U
#define SPI_CODR_SPCK_BIT 16
#define SPI_CODR_MISO_MASK 0x20000U
#define SPI_CODR_MISO 0x20000U
#define SPI_CODR_MISO_BIT 17
#define SPI_CODR_MOSI_MASK 0x40000U
#define SPI_CODR_MOSI 0x40000U
#define SPI_CODR_MOSI_BIT 18
#define SPI_CODR_NPCS0_MASK 0x80000U
#define SPI_CODR_NPCS0 0x80000U
#define SPI_CODR_NPCS0_BIT 19
#define SPI_CODR_NPCS1_MASK 0x100000U
#define SPI_CODR_NPCS1 0x100000U
#define SPI_CODR_NPCS1_BIT 20
#define SPI_CODR_NPCS2_MASK 0x200000U
#define SPI_CODR_NPCS2 0x200000U
#define SPI_CODR_NPCS2_BIT 21
#define SPI_CODR_NPCS3_MASK 0x400000U
#define SPI_CODR_NPCS3 0x400000U
#define SPI_CODR_NPCS3_BIT 22

#define SPI_ODSR (*(volatile unsigned long *)0xFFFB4038)
#define SPI_ODSR_OFFSET 0x38
#define SPI_ODSR_SPCK_MASK 0x10000U
#define SPI_ODSR_SPCK 0x10000U
#define SPI_ODSR_SPCK_BIT 16
#define SPI_ODSR_MISO_MASK 0x20000U
#define SPI_ODSR_MISO 0x20000U
#define SPI_ODSR_MISO_BIT 17
#define SPI_ODSR_MOSI_MASK 0x40000U
#define SPI_ODSR_MOSI 0x40000U
#define SPI_ODSR_MOSI_BIT 18
#define SPI_ODSR_NPCS0_MASK 0x80000U
#define SPI_ODSR_NPCS0 0x80000U
#define SPI_ODSR_NPCS0_BIT 19
#define SPI_ODSR_NPCS1_MASK 0x100000U
#define SPI_ODSR_NPCS1 0x100000U
#define SPI_ODSR_NPCS1_BIT 20
#define SPI_ODSR_NPCS2_MASK 0x200000U
#define SPI_ODSR_NPCS2 0x200000U
#define SPI_ODSR_NPCS2_BIT 21
#define SPI_ODSR_NPCS3_MASK 0x400000U
#define SPI_ODSR_NPCS3 0x400000U
#define SPI_ODSR_NPCS3_BIT 22

#define SPI_PDSR (*(volatile unsigned long *)0xFFFB403C)
#define SPI_PDSR_OFFSET 0x3C
#define SPI_PDSR_SPCK_MASK 0x10000U
#define SPI_PDSR_SPCK 0x10000U
#define SPI_PDSR_SPCK_BIT 16
#define SPI_PDSR_MISO_MASK 0x20000U
#define SPI_PDSR_MISO 0x20000U
#define SPI_PDSR_MISO_BIT 17
#define SPI_PDSR_MOSI_MASK 0x40000U
#define SPI_PDSR_MOSI 0x40000U
#define SPI_PDSR_MOSI_BIT 18
#define SPI_PDSR_NPCS0_MASK 0x80000U
#define SPI_PDSR_NPCS0 0x80000U
#define SPI_PDSR_NPCS0_BIT 19
#define SPI_PDSR_NPCS1_MASK 0x100000U
#define SPI_PDSR_NPCS1 0x100000U
#define SPI_PDSR_NPCS1_BIT 20
#define SPI_PDSR_NPCS2_MASK 0x200000U
#define SPI_PDSR_NPCS2 0x200000U
#define SPI_PDSR_NPCS2_BIT 21
#define SPI_PDSR_NPCS3_MASK 0x400000U
#define SPI_PDSR_NPCS3 0x400000U
#define SPI_PDSR_NPCS3_BIT 22

#define SPI_MDER (*(volatile unsigned long *)0xFFFB4040)
#define SPI_MDER_OFFSET 0x40
#define SPI_MDER_SPCK_MASK 0x10000U
#define SPI_MDER_SPCK 0x10000U
#define SPI_MDER_SPCK_BIT 16
#define SPI_MDER_MISO_MASK 0x20000U
#define SPI_MDER_MISO 0x20000U
#define SPI_MDER_MISO_BIT 17
#define SPI_MDER_MOSI_MASK 0x40000U
#define SPI_MDER_MOSI 0x40000U
#define SPI_MDER_MOSI_BIT 18
#define SPI_MDER_NPCS0_MASK 0x80000U
#define SPI_MDER_NPCS0 0x80000U
#define SPI_MDER_NPCS0_BIT 19
#define SPI_MDER_NPCS1_MASK 0x100000U
#define SPI_MDER_NPCS1 0x100000U
#define SPI_MDER_NPCS1_BIT 20
#define SPI_MDER_NPCS2_MASK 0x200000U
#define SPI_MDER_NPCS2 0x200000U
#define SPI_MDER_NPCS2_BIT 21
#define SPI_MDER_NPCS3_MASK 0x400000U
#define SPI_MDER_NPCS3 0x400000U
#define SPI_MDER_NPCS3_BIT 22

#define SPI_MDDR (*(volatile unsigned long *)0xFFFB4044)
#define SPI_MDDR_OFFSET 0x44
#define SPI_MDDR_SPCK_MASK 0x10000U
#define SPI_MDDR_SPCK 0x10000U
#define SPI_MDDR_SPCK_BIT 16
#define SPI_MDDR_MISO_MASK 0x20000U
#define SPI_MDDR_MISO 0x20000U
#define SPI_MDDR_MISO_BIT 17
#define SPI_MDDR_MOSI_MASK 0x40000U
#define SPI_MDDR_MOSI 0x40000U
#define SPI_MDDR_MOSI_BIT 18
#define SPI_MDDR_NPCS0_MASK 0x80000U
#define SPI_MDDR_NPCS0 0x80000U
#define SPI_MDDR_NPCS0_BIT 19
#define SPI_MDDR_NPCS1_MASK 0x100000U
#define SPI_MDDR_NPCS1 0x100000U
#define SPI_MDDR_NPCS1_BIT 20
#define SPI_MDDR_NPCS2_MASK 0x200000U
#define SPI_MDDR_NPCS2 0x200000U
#define SPI_MDDR_NPCS2_BIT 21
#define SPI_MDDR_NPCS3_MASK 0x400000U
#define SPI_MDDR_NPCS3 0x400000U
#define SPI_MDDR_NPCS3_BIT 22

#define SPI_MDSR (*(volatile unsigned long *)0xFFFB4048)
#define SPI_MDSR_OFFSET 0x48
#define SPI_MDSR_SPCK_MASK 0x10000U
#define SPI_MDSR_SPCK 0x10000U
#define SPI_MDSR_SPCK_BIT 16
#define SPI_MDSR_MISO_MASK 0x20000U
#define SPI_MDSR_MISO 0x20000U
#define SPI_MDSR_MISO_BIT 17
#define SPI_MDSR_MOSI_MASK 0x40000U
#define SPI_MDSR_MOSI 0x40000U
#define SPI_MDSR_MOSI_BIT 18
#define SPI_MDSR_NPCS0_MASK 0x80000U
#define SPI_MDSR_NPCS0 0x80000U
#define SPI_MDSR_NPCS0_BIT 19
#define SPI_MDSR_NPCS1_MASK 0x100000U
#define SPI_MDSR_NPCS1 0x100000U
#define SPI_MDSR_NPCS1_BIT 20
#define SPI_MDSR_NPCS2_MASK 0x200000U
#define SPI_MDSR_NPCS2 0x200000U
#define SPI_MDSR_NPCS2_BIT 21
#define SPI_MDSR_NPCS3_MASK 0x400000U
#define SPI_MDSR_NPCS3 0x400000U
#define SPI_MDSR_NPCS3_BIT 22

#define SPI_ECR (*(volatile unsigned long *)0xFFFB4050)
#define SPI_ECR_OFFSET 0x50
#define SPI_ECR_PIO_MASK 0x1U
#define SPI_ECR_PIO 0x1U
#define SPI_ECR_PIO_BIT 0
#define SPI_ECR_SPI_MASK 0x2U
#define SPI_ECR_SPI 0x2U
#define SPI_ECR_SPI_BIT 1

#define SPI_DCR (*(volatile unsigned long *)0xFFFB4054)
#define SPI_DCR_OFFSET 0x54
#define SPI_DCR_PIO_MASK 0x1U
#define SPI_DCR_PIO 0x1U
#define SPI_DCR_PIO_BIT 0
#define SPI_DCR_SPI_MASK 0x2U
#define SPI_DCR_SPI 0x2U
#define SPI_DCR_SPI_BIT 1

#define SPI_PMSR (*(volatile unsigned long *)0xFFFB4058)
#define SPI_PMSR_OFFSET 0x58
#define SPI_PMSR_PIO_MASK 0x1U
#define SPI_PMSR_PIO 0x1U
#define SPI_PMSR_PIO_BIT 0
#define SPI_PMSR_SPI_MASK 0x2U
#define SPI_PMSR_SPI 0x2U
#define SPI_PMSR_SPI_BIT 1

#define SPI_CR (*(volatile unsigned long *)0xFFFB4060)
#define SPI_CR_OFFSET 0x60
#define SPI_CR_SWRST_MASK 0x1U
#define SPI_CR_SWRST 0x1U
#define SPI_CR_SWRST_BIT 0
#define SPI_CR_SPIEN_MASK 0x2U
#define SPI_CR_SPIEN 0x2U
#define SPI_CR_SPIEN_BIT 1
#define SPI_CR_SPIDIS_MASK 0x4U
#define SPI_CR_SPIDIS 0x4U
#define SPI_CR_SPIDIS_BIT 2

#define SPI_MR (*(volatile unsigned long *)0xFFFB4064)
#define SPI_MR_OFFSET 0x64
#define SPI_MR_MSTR_MASK 0x1U
#define SPI_MR_MSTR 0x1U
#define SPI_MR_MSTR_BIT 0
#define SPI_MR_PS_MASK 0x2U
#define SPI_MR_PS 0x2U
#define SPI_MR_PS_BIT 1
#define SPI_MR_PCSDEC_MASK 0x4U
#define SPI_MR_PCSDEC 0x4U
#define SPI_MR_PCSDEC_BIT 2
#define SPI_MR_DIV32_MASK 0x8U
#define SPI_MR_DIV32 0x8U
#define SPI_MR_DIV32_BIT 3
#define SPI_MR_LLB_MASK 0x80U
#define SPI_MR_LLB 0x80U
#define SPI_MR_LLB_BIT 7
#define SPI_MR_PCS_MASK 0xF0000U
#define SPI_MR_PCS_BIT 16
#define SPI_MR_DLYBCS_MASK 0x1000000U
#define SPI_MR_DLYBCS 0x1000000U
#define SPI_MR_DLYBCS_BIT 24

#define SPI_SR (*(volatile unsigned long *)0xFFFB4070)
#define SPI_SR_OFFSET 0x70
#define SPI_SR_RDRF_MASK 0x1U
#define SPI_SR_RDRF 0x1U
#define SPI_SR_RDRF_BIT 0
#define SPI_SR_TDRE_MASK 0x2U
#define SPI_SR_TDRE 0x2U
#define SPI_SR_TDRE_BIT 1
#define SPI_SR_MODF_MASK 0x4U
#define SPI_SR_MODF 0x4U
#define SPI_SR_MODF_BIT 2
#define SPI_SR_SPIOVRE_MASK 0x8U
#define SPI_SR_SPIOVRE 0x8U
#define SPI_SR_SPIOVRE_BIT 3
#define SPI_SR_REND_MASK 0x10U
#define SPI_SR_REND 0x10U
#define SPI_SR_REND_BIT 4
#define SPI_SR_TEND_MASK 0x20U
#define SPI_SR_TEND 0x20U
#define SPI_SR_TEND_BIT 5
#define SPI_SR_SPIENS_MASK 0x100U
#define SPI_SR_SPIENS 0x100U
#define SPI_SR_SPIENS_BIT 8
#define SPI_SR_SPCK_MASK 0x10000U
#define SPI_SR_SPCK 0x10000U
#define SPI_SR_SPCK_BIT 16
#define SPI_SR_MISO_MASK 0x20000U
#define SPI_SR_MISO 0x20000U
#define SPI_SR_MISO_BIT 17
#define SPI_SR_MOSI_MASK 0x40000U
#define SPI_SR_MOSI 0x40000U
#define SPI_SR_MOSI_BIT 18
#define SPI_SR_NPCS0_MASK 0x80000U
#define SPI_SR_NPCS0 0x80000U
#define SPI_SR_NPCS0_BIT 19
#define SPI_SR_NPCS1_MASK 0x100000U
#define SPI_SR_NPCS1 0x100000U
#define SPI_SR_NPCS1_BIT 20
#define SPI_SR_NPCS2_MASK 0x200000U
#define SPI_SR_NPCS2 0x200000U
#define SPI_SR_NPCS2_BIT 21
#define SPI_SR_NPCS3_MASK 0x400000U
#define SPI_SR_NPCS3 0x400000U
#define SPI_SR_NPCS3_BIT 22

#define SPI_IER (*(volatile unsigned long *)0xFFFB4074)
#define SPI_IER_OFFSET 0x74
#define SPI_IER_RDRF_MASK 0x1U
#define SPI_IER_RDRF 0x1U
#define SPI_IER_RDRF_BIT 0
#define SPI_IER_TDRE_MASK 0x2U
#define SPI_IER_TDRE 0x2U
#define SPI_IER_TDRE_BIT 1
#define SPI_IER_MODF_MASK 0x4U
#define SPI_IER_MODF 0x4U
#define SPI_IER_MODF_BIT 2
#define SPI_IER_SPIOVRE_MASK 0x8U
#define SPI_IER_SPIOVRE 0x8U
#define SPI_IER_SPIOVRE_BIT 3
#define SPI_IER_REND_MASK 0x10U
#define SPI_IER_REND 0x10U
#define SPI_IER_REND_BIT 4
#define SPI_IER_TEND_MASK 0x20U
#define SPI_IER_TEND 0x20U
#define SPI_IER_TEND_BIT 5
#define SPI_IER_SPCK_MASK 0x10000U
#define SPI_IER_SPCK 0x10000U
#define SPI_IER_SPCK_BIT 16
#define SPI_IER_MISO_MASK 0x20000U
#define SPI_IER_MISO 0x20000U
#define SPI_IER_MISO_BIT 17
#define SPI_IER_MOSI_MASK 0x40000U
#define SPI_IER_MOSI 0x40000U
#define SPI_IER_MOSI_BIT 18
#define SPI_IER_NPCS0_MASK 0x80000U
#define SPI_IER_NPCS0 0x80000U
#define SPI_IER_NPCS0_BIT 19
#define SPI_IER_NPCS1_MASK 0x100000U
#define SPI_IER_NPCS1 0x100000U
#define SPI_IER_NPCS1_BIT 20
#define SPI_IER_NPCS2_MASK 0x200000U
#define SPI_IER_NPCS2 0x200000U
#define SPI_IER_NPCS2_BIT 21
#define SPI_IER_NPCS3_MASK 0x400000U
#define SPI_IER_NPCS3 0x400000U
#define SPI_IER_NPCS3_BIT 22

#define SPI_IDR (*(volatile unsigned long *)0xFFFB4078)
#define SPI_IDR_OFFSET 0x78
#define SPI_IDR_RDRF_MASK 0x1U
#define SPI_IDR_RDRF 0x1U
#define SPI_IDR_RDRF_BIT 0
#define SPI_IDR_TDRE_MASK 0x2U
#define SPI_IDR_TDRE 0x2U
#define SPI_IDR_TDRE_BIT 1
#define SPI_IDR_MODF_MASK 0x4U
#define SPI_IDR_MODF 0x4U
#define SPI_IDR_MODF_BIT 2
#define SPI_IDR_SPIOVRE_MASK 0x8U
#define SPI_IDR_SPIOVRE 0x8U
#define SPI_IDR_SPIOVRE_BIT 3
#define SPI_IDR_REND_MASK 0x10U
#define SPI_IDR_REND 0x10U
#define SPI_IDR_REND_BIT 4
#define SPI_IDR_TEND_MASK 0x20U
#define SPI_IDR_TEND 0x20U
#define SPI_IDR_TEND_BIT 5
#define SPI_IDR_SPCK_MASK 0x10000U
#define SPI_IDR_SPCK 0x10000U
#define SPI_IDR_SPCK_BIT 16
#define SPI_IDR_MISO_MASK 0x20000U
#define SPI_IDR_MISO 0x20000U
#define SPI_IDR_MISO_BIT 17
#define SPI_IDR_MOSI_MASK 0x40000U
#define SPI_IDR_MOSI 0x40000U
#define SPI_IDR_MOSI_BIT 18
#define SPI_IDR_NPCS0_MASK 0x80000U
#define SPI_IDR_NPCS0 0x80000U
#define SPI_IDR_NPCS0_BIT 19
#define SPI_IDR_NPCS1_MASK 0x100000U
#define SPI_IDR_NPCS1 0x100000U
#define SPI_IDR_NPCS1_BIT 20
#define SPI_IDR_NPCS2_MASK 0x200000U
#define SPI_IDR_NPCS2 0x200000U
#define SPI_IDR_NPCS2_BIT 21
#define SPI_IDR_NPCS3_MASK 0x400000U
#define SPI_IDR_NPCS3 0x400000U
#define SPI_IDR_NPCS3_BIT 22

#define SPI_IMR (*(volatile unsigned long *)0xFFFB407C)
#define SPI_IMR_OFFSET 0x7C
#define SPI_IMR_RDRF_MASK 0x1U
#define SPI_IMR_RDRF 0x1U
#define SPI_IMR_RDRF_BIT 0
#define SPI_IMR_TDRE_MASK 0x2U
#define SPI_IMR_TDRE 0x2U
#define SPI_IMR_TDRE_BIT 1
#define SPI_IMR_MODF_MASK 0x4U
#define SPI_IMR_MODF 0x4U
#define SPI_IMR_MODF_BIT 2
#define SPI_IMR_SPIOVRE_MASK 0x8U
#define SPI_IMR_SPIOVRE 0x8U
#define SPI_IMR_SPIOVRE_BIT 3
#define SPI_IMR_REND_MASK 0x10U
#define SPI_IMR_REND 0x10U
#define SPI_IMR_REND_BIT 4
#define SPI_IMR_TEND_MASK 0x20U
#define SPI_IMR_TEND 0x20U
#define SPI_IMR_TEND_BIT 5
#define SPI_IMR_SPCK_MASK 0x10000U
#define SPI_IMR_SPCK 0x10000U
#define SPI_IMR_SPCK_BIT 16
#define SPI_IMR_MISO_MASK 0x20000U
#define SPI_IMR_MISO 0x20000U
#define SPI_IMR_MISO_BIT 17
#define SPI_IMR_MOSI_MASK 0x40000U
#define SPI_IMR_MOSI 0x40000U
#define SPI_IMR_MOSI_BIT 18
#define SPI_IMR_NPCS0_MASK 0x80000U
#define SPI_IMR_NPCS0 0x80000U
#define SPI_IMR_NPCS0_BIT 19
#define SPI_IMR_NPCS1_MASK 0x100000U
#define SPI_IMR_NPCS1 0x100000U
#define SPI_IMR_NPCS1_BIT 20
#define SPI_IMR_NPCS2_MASK 0x200000U
#define SPI_IMR_NPCS2 0x200000U
#define SPI_IMR_NPCS2_BIT 21
#define SPI_IMR_NPCS3_MASK 0x400000U
#define SPI_IMR_NPCS3 0x400000U
#define SPI_IMR_NPCS3_BIT 22

#define SPI_RDR (*(volatile unsigned long *)0xFFFB4080)
#define SPI_RDR_OFFSET 0x80
#define SPI_RDR_RD_MASK 0xFFFFU
#define SPI_RDR_RD_BIT 0
#define SPI_RDR_PCS_MASK 0xF0000U
#define SPI_RDR_PCS_BIT 16

#define SPI_TDR (*(volatile unsigned long *)0xFFFB4084)
#define SPI_TDR_OFFSET 0x84
#define SPI_TDR_TD_MASK 0xFFFFU
#define SPI_TDR_TD_BIT 0
#define SPI_TDR_PCS_MASK 0xF0000U
#define SPI_TDR_PCS_BIT 16

#define SPI_CSR0 (*(volatile unsigned long *)0xFFFB4090)
#define SPI_CSR0_OFFSET 0x90
#define SPI_CSR0_CPOL_MASK 0x1U
#define SPI_CSR0_CPOL 0x1U
#define SPI_CSR0_CPOL_BIT 0
#define SPI_CSR0_NCPHA_MASK 0x1U
#define SPI_CSR0_NCPHA 0x1U
#define SPI_CSR0_NCPHA_BIT 0
#define SPI_CSR0_BITS_MASK 0xF0U
#define SPI_CSR0_BITS_BIT 4
#define SPI_CSR0_SCBR_MASK 0xFF00U
#define SPI_CSR0_SCBR_BIT 8
#define SPI_CSR0_DLYBS_MASK 0xFF0000U
#define SPI_CSR0_DLYBS_BIT 16
#define SPI_CSR0_DLYBCT_MASK 0xFF000000U
#define SPI_CSR0_DLYBCT_BIT 24

#define SPI_CSR1 (*(volatile unsigned long *)0xFFFB4094)
#define SPI_CSR1_OFFSET 0x94
#define SPI_CSR1_CPOL_MASK 0x1U
#define SPI_CSR1_CPOL 0x1U
#define SPI_CSR1_CPOL_BIT 0
#define SPI_CSR1_NCPHA_MASK 0x1U
#define SPI_CSR1_NCPHA 0x1U
#define SPI_CSR1_NCPHA_BIT 0
#define SPI_CSR1_BITS_MASK 0xF0U
#define SPI_CSR1_BITS_BIT 4
#define SPI_CSR1_SCBR_MASK 0xFF00U
#define SPI_CSR1_SCBR_BIT 8
#define SPI_CSR1_DLYBS_MASK 0xFF0000U
#define SPI_CSR1_DLYBS_BIT 16
#define SPI_CSR1_DLYBCT_MASK 0xFF000000U
#define SPI_CSR1_DLYBCT_BIT 24

#define SPI_CSR2 (*(volatile unsigned long *)0xFFFB4098)
#define SPI_CSR2_OFFSET 0x98
#define SPI_CSR2_CPOL_MASK 0x1U
#define SPI_CSR2_CPOL 0x1U
#define SPI_CSR2_CPOL_BIT 0
#define SPI_CSR2_NCPHA_MASK 0x1U
#define SPI_CSR2_NCPHA 0x1U
#define SPI_CSR2_NCPHA_BIT 0
#define SPI_CSR2_BITS_MASK 0xF0U
#define SPI_CSR2_BITS_BIT 4
#define SPI_CSR2_SCBR_MASK 0xFF00U
#define SPI_CSR2_SCBR_BIT 8
#define SPI_CSR2_DLYBS_MASK 0xFF0000U
#define SPI_CSR2_DLYBS_BIT 16
#define SPI_CSR2_DLYBCT_MASK 0xFF000000U
#define SPI_CSR2_DLYBCT_BIT 24

#define SPI_CSR3 (*(volatile unsigned long *)0xFFFB409C)
#define SPI_CSR3_OFFSET 0x9C
#define SPI_CSR3_CPOL_MASK 0x1U
#define SPI_CSR3_CPOL 0x1U
#define SPI_CSR3_CPOL_BIT 0
#define SPI_CSR3_NCPHA_MASK 0x1U
#define SPI_CSR3_NCPHA 0x1U
#define SPI_CSR3_NCPHA_BIT 0
#define SPI_CSR3_BITS_MASK 0xF0U
#define SPI_CSR3_BITS_BIT 4
#define SPI_CSR3_SCBR_MASK 0xFF00U
#define SPI_CSR3_SCBR_BIT 8
#define SPI_CSR3_DLYBS_MASK 0xFF0000U
#define SPI_CSR3_DLYBS_BIT 16
#define SPI_CSR3_DLYBCT_MASK 0xFF000000U
#define SPI_CSR3_DLYBCT_BIT 24


#define UPIO_BASE 0xFFFD8000

#define UPIO_OER (*(volatile unsigned long *)0xFFFD8010)
#define UPIO_OER_OFFSET 0x10
#define UPIO_OER_P0_MASK 0x1U
#define UPIO_OER_P0 0x1U
#define UPIO_OER_P0_BIT 0
#define UPIO_OER_P1_MASK 0x2U
#define UPIO_OER_P1 0x2U
#define UPIO_OER_P1_BIT 1
#define UPIO_OER_P2_MASK 0x4U
#define UPIO_OER_P2 0x4U
#define UPIO_OER_P2_BIT 2
#define UPIO_OER_P3_MASK 0x8U
#define UPIO_OER_P3 0x8U
#define UPIO_OER_P3_BIT 3
#define UPIO_OER_P4_MASK 0x10U
#define UPIO_OER_P4 0x10U
#define UPIO_OER_P4_BIT 4
#define UPIO_OER_P5_MASK 0x20U
#define UPIO_OER_P5 0x20U
#define UPIO_OER_P5_BIT 5
#define UPIO_OER_P6_MASK 0x40U
#define UPIO_OER_P6 0x40U
#define UPIO_OER_P6_BIT 6
#define UPIO_OER_P7_MASK 0x80U
#define UPIO_OER_P7 0x80U
#define UPIO_OER_P7_BIT 7
#define UPIO_OER_P8_MASK 0x100U
#define UPIO_OER_P8 0x100U
#define UPIO_OER_P8_BIT 8
#define UPIO_OER_P9_MASK 0x200U
#define UPIO_OER_P9 0x200U
#define UPIO_OER_P9_BIT 9
#define UPIO_OER_P10_MASK 0x400U
#define UPIO_OER_P10 0x400U
#define UPIO_OER_P10_BIT 10
#define UPIO_OER_P11_MASK 0x800U
#define UPIO_OER_P11 0x800U
#define UPIO_OER_P11_BIT 11
#define UPIO_OER_P12_MASK 0x1000U
#define UPIO_OER_P12 0x1000U
#define UPIO_OER_P12_BIT 12
#define UPIO_OER_P13_MASK 0x2000U
#define UPIO_OER_P13 0x2000U
#define UPIO_OER_P13_BIT 13
#define UPIO_OER_P14_MASK 0x4000U
#define UPIO_OER_P14 0x4000U
#define UPIO_OER_P14_BIT 14
#define UPIO_OER_P15_MASK 0x8000U
#define UPIO_OER_P15 0x8000U
#define UPIO_OER_P15_BIT 15
#define UPIO_OER_P16_MASK 0x10000U
#define UPIO_OER_P16 0x10000U
#define UPIO_OER_P16_BIT 16
#define UPIO_OER_P17_MASK 0x20000U
#define UPIO_OER_P17 0x20000U
#define UPIO_OER_P17_BIT 17

#define UPIO_ODR (*(volatile unsigned long *)0xFFFD8014)
#define UPIO_ODR_OFFSET 0x14
#define UPIO_ODR_P0_MASK 0x1U
#define UPIO_ODR_P0 0x1U
#define UPIO_ODR_P0_BIT 0
#define UPIO_ODR_P1_MASK 0x2U
#define UPIO_ODR_P1 0x2U
#define UPIO_ODR_P1_BIT 1
#define UPIO_ODR_P2_MASK 0x4U
#define UPIO_ODR_P2 0x4U
#define UPIO_ODR_P2_BIT 2
#define UPIO_ODR_P3_MASK 0x8U
#define UPIO_ODR_P3 0x8U
#define UPIO_ODR_P3_BIT 3
#define UPIO_ODR_P4_MASK 0x10U
#define UPIO_ODR_P4 0x10U
#define UPIO_ODR_P4_BIT 4
#define UPIO_ODR_P5_MASK 0x20U
#define UPIO_ODR_P5 0x20U
#define UPIO_ODR_P5_BIT 5
#define UPIO_ODR_P6_MASK 0x40U
#define UPIO_ODR_P6 0x40U
#define UPIO_ODR_P6_BIT 6
#define UPIO_ODR_P7_MASK 0x80U
#define UPIO_ODR_P7 0x80U
#define UPIO_ODR_P7_BIT 7
#define UPIO_ODR_P8_MASK 0x100U
#define UPIO_ODR_P8 0x100U
#define UPIO_ODR_P8_BIT 8
#define UPIO_ODR_P9_MASK 0x200U
#define UPIO_ODR_P9 0x200U
#define UPIO_ODR_P9_BIT 9
#define UPIO_ODR_P10_MASK 0x400U
#define UPIO_ODR_P10 0x400U
#define UPIO_ODR_P10_BIT 10
#define UPIO_ODR_P11_MASK 0x800U
#define UPIO_ODR_P11 0x800U
#define UPIO_ODR_P11_BIT 11
#define UPIO_ODR_P12_MASK 0x1000U
#define UPIO_ODR_P12 0x1000U
#define UPIO_ODR_P12_BIT 12
#define UPIO_ODR_P13_MASK 0x2000U
#define UPIO_ODR_P13 0x2000U
#define UPIO_ODR_P13_BIT 13
#define UPIO_ODR_P14_MASK 0x4000U
#define UPIO_ODR_P14 0x4000U
#define UPIO_ODR_P14_BIT 14
#define UPIO_ODR_P15_MASK 0x8000U
#define UPIO_ODR_P15 0x8000U
#define UPIO_ODR_P15_BIT 15
#define UPIO_ODR_P16_MASK 0x10000U
#define UPIO_ODR_P16 0x10000U
#define UPIO_ODR_P16_BIT 16
#define UPIO_ODR_P17_MASK 0x20000U
#define UPIO_ODR_P17 0x20000U
#define UPIO_ODR_P17_BIT 17

#define UPIO_OSR (*(volatile unsigned long *)0xFFFD8018)
#define UPIO_OSR_OFFSET 0x18
#define UPIO_OSR_P0_MASK 0x1U
#define UPIO_OSR_P0 0x1U
#define UPIO_OSR_P0_BIT 0
#define UPIO_OSR_P1_MASK 0x2U
#define UPIO_OSR_P1 0x2U
#define UPIO_OSR_P1_BIT 1
#define UPIO_OSR_P2_MASK 0x4U
#define UPIO_OSR_P2 0x4U
#define UPIO_OSR_P2_BIT 2
#define UPIO_OSR_P3_MASK 0x8U
#define UPIO_OSR_P3 0x8U
#define UPIO_OSR_P3_BIT 3
#define UPIO_OSR_P4_MASK 0x10U
#define UPIO_OSR_P4 0x10U
#define UPIO_OSR_P4_BIT 4
#define UPIO_OSR_P5_MASK 0x20U
#define UPIO_OSR_P5 0x20U
#define UPIO_OSR_P5_BIT 5
#define UPIO_OSR_P6_MASK 0x40U
#define UPIO_OSR_P6 0x40U
#define UPIO_OSR_P6_BIT 6
#define UPIO_OSR_P7_MASK 0x80U
#define UPIO_OSR_P7 0x80U
#define UPIO_OSR_P7_BIT 7
#define UPIO_OSR_P8_MASK 0x100U
#define UPIO_OSR_P8 0x100U
#define UPIO_OSR_P8_BIT 8
#define UPIO_OSR_P9_MASK 0x200U
#define UPIO_OSR_P9 0x200U
#define UPIO_OSR_P9_BIT 9
#define UPIO_OSR_P10_MASK 0x400U
#define UPIO_OSR_P10 0x400U
#define UPIO_OSR_P10_BIT 10
#define UPIO_OSR_P11_MASK 0x800U
#define UPIO_OSR_P11 0x800U
#define UPIO_OSR_P11_BIT 11
#define UPIO_OSR_P12_MASK 0x1000U
#define UPIO_OSR_P12 0x1000U
#define UPIO_OSR_P12_BIT 12
#define UPIO_OSR_P13_MASK 0x2000U
#define UPIO_OSR_P13 0x2000U
#define UPIO_OSR_P13_BIT 13
#define UPIO_OSR_P14_MASK 0x4000U
#define UPIO_OSR_P14 0x4000U
#define UPIO_OSR_P14_BIT 14
#define UPIO_OSR_P15_MASK 0x8000U
#define UPIO_OSR_P15 0x8000U
#define UPIO_OSR_P15_BIT 15
#define UPIO_OSR_P16_MASK 0x10000U
#define UPIO_OSR_P16 0x10000U
#define UPIO_OSR_P16_BIT 16
#define UPIO_OSR_P17_MASK 0x20000U
#define UPIO_OSR_P17 0x20000U
#define UPIO_OSR_P17_BIT 17

#define UPIO_SODR (*(volatile unsigned long *)0xFFFD8030)
#define UPIO_SODR_OFFSET 0x30
#define UPIO_SODR_P0_MASK 0x1U
#define UPIO_SODR_P0 0x1U
#define UPIO_SODR_P0_BIT 0
#define UPIO_SODR_P1_MASK 0x2U
#define UPIO_SODR_P1 0x2U
#define UPIO_SODR_P1_BIT 1
#define UPIO_SODR_P2_MASK 0x4U
#define UPIO_SODR_P2 0x4U
#define UPIO_SODR_P2_BIT 2
#define UPIO_SODR_P3_MASK 0x8U
#define UPIO_SODR_P3 0x8U
#define UPIO_SODR_P3_BIT 3
#define UPIO_SODR_P4_MASK 0x10U
#define UPIO_SODR_P4 0x10U
#define UPIO_SODR_P4_BIT 4
#define UPIO_SODR_P5_MASK 0x20U
#define UPIO_SODR_P5 0x20U
#define UPIO_SODR_P5_BIT 5
#define UPIO_SODR_P6_MASK 0x40U
#define UPIO_SODR_P6 0x40U
#define UPIO_SODR_P6_BIT 6
#define UPIO_SODR_P7_MASK 0x80U
#define UPIO_SODR_P7 0x80U
#define UPIO_SODR_P7_BIT 7
#define UPIO_SODR_P8_MASK 0x100U
#define UPIO_SODR_P8 0x100U
#define UPIO_SODR_P8_BIT 8
#define UPIO_SODR_P9_MASK 0x200U
#define UPIO_SODR_P9 0x200U
#define UPIO_SODR_P9_BIT 9
#define UPIO_SODR_P10_MASK 0x400U
#define UPIO_SODR_P10 0x400U
#define UPIO_SODR_P10_BIT 10
#define UPIO_SODR_P11_MASK 0x800U
#define UPIO_SODR_P11 0x800U
#define UPIO_SODR_P11_BIT 11
#define UPIO_SODR_P12_MASK 0x1000U
#define UPIO_SODR_P12 0x1000U
#define UPIO_SODR_P12_BIT 12
#define UPIO_SODR_P13_MASK 0x2000U
#define UPIO_SODR_P13 0x2000U
#define UPIO_SODR_P13_BIT 13
#define UPIO_SODR_P14_MASK 0x4000U
#define UPIO_SODR_P14 0x4000U
#define UPIO_SODR_P14_BIT 14
#define UPIO_SODR_P15_MASK 0x8000U
#define UPIO_SODR_P15 0x8000U
#define UPIO_SODR_P15_BIT 15
#define UPIO_SODR_P16_MASK 0x10000U
#define UPIO_SODR_P16 0x10000U
#define UPIO_SODR_P16_BIT 16
#define UPIO_SODR_P17_MASK 0x20000U
#define UPIO_SODR_P17 0x20000U
#define UPIO_SODR_P17_BIT 17

#define UPIO_CODR (*(volatile unsigned long *)0xFFFD8034)
#define UPIO_CODR_OFFSET 0x34
#define UPIO_CODR_P0_MASK 0x1U
#define UPIO_CODR_P0 0x1U
#define UPIO_CODR_P0_BIT 0
#define UPIO_CODR_P1_MASK 0x2U
#define UPIO_CODR_P1 0x2U
#define UPIO_CODR_P1_BIT 1
#define UPIO_CODR_P2_MASK 0x4U
#define UPIO_CODR_P2 0x4U
#define UPIO_CODR_P2_BIT 2
#define UPIO_CODR_P3_MASK 0x8U
#define UPIO_CODR_P3 0x8U
#define UPIO_CODR_P3_BIT 3
#define UPIO_CODR_P4_MASK 0x10U
#define UPIO_CODR_P4 0x10U
#define UPIO_CODR_P4_BIT 4
#define UPIO_CODR_P5_MASK 0x20U
#define UPIO_CODR_P5 0x20U
#define UPIO_CODR_P5_BIT 5
#define UPIO_CODR_P6_MASK 0x40U
#define UPIO_CODR_P6 0x40U
#define UPIO_CODR_P6_BIT 6
#define UPIO_CODR_P7_MASK 0x80U
#define UPIO_CODR_P7 0x80U
#define UPIO_CODR_P7_BIT 7
#define UPIO_CODR_P8_MASK 0x100U
#define UPIO_CODR_P8 0x100U
#define UPIO_CODR_P8_BIT 8
#define UPIO_CODR_P9_MASK 0x200U
#define UPIO_CODR_P9 0x200U
#define UPIO_CODR_P9_BIT 9
#define UPIO_CODR_P10_MASK 0x400U
#define UPIO_CODR_P10 0x400U
#define UPIO_CODR_P10_BIT 10
#define UPIO_CODR_P11_MASK 0x800U
#define UPIO_CODR_P11 0x800U
#define UPIO_CODR_P11_BIT 11
#define UPIO_CODR_P12_MASK 0x1000U
#define UPIO_CODR_P12 0x1000U
#define UPIO_CODR_P12_BIT 12
#define UPIO_CODR_P13_MASK 0x2000U
#define UPIO_CODR_P13 0x2000U
#define UPIO_CODR_P13_BIT 13
#define UPIO_CODR_P14_MASK 0x4000U
#define UPIO_CODR_P14 0x4000U
#define UPIO_CODR_P14_BIT 14
#define UPIO_CODR_P15_MASK 0x8000U
#define UPIO_CODR_P15 0x8000U
#define UPIO_CODR_P15_BIT 15
#define UPIO_CODR_P16_MASK 0x10000U
#define UPIO_CODR_P16 0x10000U
#define UPIO_CODR_P16_BIT 16
#define UPIO_CODR_P17_MASK 0x20000U
#define UPIO_CODR_P17 0x20000U
#define UPIO_CODR_P17_BIT 17

#define UPIO_ODSR (*(volatile unsigned long *)0xFFFD8038)
#define UPIO_ODSR_OFFSET 0x38
#define UPIO_ODSR_P0_MASK 0x1U
#define UPIO_ODSR_P0 0x1U
#define UPIO_ODSR_P0_BIT 0
#define UPIO_ODSR_P1_MASK 0x2U
#define UPIO_ODSR_P1 0x2U
#define UPIO_ODSR_P1_BIT 1
#define UPIO_ODSR_P2_MASK 0x4U
#define UPIO_ODSR_P2 0x4U
#define UPIO_ODSR_P2_BIT 2
#define UPIO_ODSR_P3_MASK 0x8U
#define UPIO_ODSR_P3 0x8U
#define UPIO_ODSR_P3_BIT 3
#define UPIO_ODSR_P4_MASK 0x10U
#define UPIO_ODSR_P4 0x10U
#define UPIO_ODSR_P4_BIT 4
#define UPIO_ODSR_P5_MASK 0x20U
#define UPIO_ODSR_P5 0x20U
#define UPIO_ODSR_P5_BIT 5
#define UPIO_ODSR_P6_MASK 0x40U
#define UPIO_ODSR_P6 0x40U
#define UPIO_ODSR_P6_BIT 6
#define UPIO_ODSR_P7_MASK 0x80U
#define UPIO_ODSR_P7 0x80U
#define UPIO_ODSR_P7_BIT 7
#define UPIO_ODSR_P8_MASK 0x100U
#define UPIO_ODSR_P8 0x100U
#define UPIO_ODSR_P8_BIT 8
#define UPIO_ODSR_P9_MASK 0x200U
#define UPIO_ODSR_P9 0x200U
#define UPIO_ODSR_P9_BIT 9
#define UPIO_ODSR_P10_MASK 0x400U
#define UPIO_ODSR_P10 0x400U
#define UPIO_ODSR_P10_BIT 10
#define UPIO_ODSR_P11_MASK 0x800U
#define UPIO_ODSR_P11 0x800U
#define UPIO_ODSR_P11_BIT 11
#define UPIO_ODSR_P12_MASK 0x1000U
#define UPIO_ODSR_P12 0x1000U
#define UPIO_ODSR_P12_BIT 12
#define UPIO_ODSR_P13_MASK 0x2000U
#define UPIO_ODSR_P13 0x2000U
#define UPIO_ODSR_P13_BIT 13
#define UPIO_ODSR_P14_MASK 0x4000U
#define UPIO_ODSR_P14 0x4000U
#define UPIO_ODSR_P14_BIT 14
#define UPIO_ODSR_P15_MASK 0x8000U
#define UPIO_ODSR_P15 0x8000U
#define UPIO_ODSR_P15_BIT 15
#define UPIO_ODSR_P16_MASK 0x10000U
#define UPIO_ODSR_P16 0x10000U
#define UPIO_ODSR_P16_BIT 16
#define UPIO_ODSR_P17_MASK 0x20000U
#define UPIO_ODSR_P17 0x20000U
#define UPIO_ODSR_P17_BIT 17

#define UPIO_PDSR (*(volatile unsigned long *)0xFFFD803C)
#define UPIO_PDSR_OFFSET 0x3C
#define UPIO_PDSR_P0_MASK 0x1U
#define UPIO_PDSR_P0 0x1U
#define UPIO_PDSR_P0_BIT 0
#define UPIO_PDSR_P1_MASK 0x2U
#define UPIO_PDSR_P1 0x2U
#define UPIO_PDSR_P1_BIT 1
#define UPIO_PDSR_P2_MASK 0x4U
#define UPIO_PDSR_P2 0x4U
#define UPIO_PDSR_P2_BIT 2
#define UPIO_PDSR_P3_MASK 0x8U
#define UPIO_PDSR_P3 0x8U
#define UPIO_PDSR_P3_BIT 3
#define UPIO_PDSR_P4_MASK 0x10U
#define UPIO_PDSR_P4 0x10U
#define UPIO_PDSR_P4_BIT 4
#define UPIO_PDSR_P5_MASK 0x20U
#define UPIO_PDSR_P5 0x20U
#define UPIO_PDSR_P5_BIT 5
#define UPIO_PDSR_P6_MASK 0x40U
#define UPIO_PDSR_P6 0x40U
#define UPIO_PDSR_P6_BIT 6
#define UPIO_PDSR_P7_MASK 0x80U
#define UPIO_PDSR_P7 0x80U
#define UPIO_PDSR_P7_BIT 7
#define UPIO_PDSR_P8_MASK 0x100U
#define UPIO_PDSR_P8 0x100U
#define UPIO_PDSR_P8_BIT 8
#define UPIO_PDSR_P9_MASK 0x200U
#define UPIO_PDSR_P9 0x200U
#define UPIO_PDSR_P9_BIT 9
#define UPIO_PDSR_P10_MASK 0x400U
#define UPIO_PDSR_P10 0x400U
#define UPIO_PDSR_P10_BIT 10
#define UPIO_PDSR_P11_MASK 0x800U
#define UPIO_PDSR_P11 0x800U
#define UPIO_PDSR_P11_BIT 11
#define UPIO_PDSR_P12_MASK 0x1000U
#define UPIO_PDSR_P12 0x1000U
#define UPIO_PDSR_P12_BIT 12
#define UPIO_PDSR_P13_MASK 0x2000U
#define UPIO_PDSR_P13 0x2000U
#define UPIO_PDSR_P13_BIT 13
#define UPIO_PDSR_P14_MASK 0x4000U
#define UPIO_PDSR_P14 0x4000U
#define UPIO_PDSR_P14_BIT 14
#define UPIO_PDSR_P15_MASK 0x8000U
#define UPIO_PDSR_P15 0x8000U
#define UPIO_PDSR_P15_BIT 15
#define UPIO_PDSR_P16_MASK 0x10000U
#define UPIO_PDSR_P16 0x10000U
#define UPIO_PDSR_P16_BIT 16
#define UPIO_PDSR_P17_MASK 0x20000U
#define UPIO_PDSR_P17 0x20000U
#define UPIO_PDSR_P17_BIT 17

#define UPIO_MDER (*(volatile unsigned long *)0xFFFD8040)
#define UPIO_MDER_OFFSET 0x40
#define UPIO_MDER_P0_MASK 0x1U
#define UPIO_MDER_P0 0x1U
#define UPIO_MDER_P0_BIT 0
#define UPIO_MDER_P1_MASK 0x2U
#define UPIO_MDER_P1 0x2U
#define UPIO_MDER_P1_BIT 1
#define UPIO_MDER_P2_MASK 0x4U
#define UPIO_MDER_P2 0x4U
#define UPIO_MDER_P2_BIT 2
#define UPIO_MDER_P3_MASK 0x8U
#define UPIO_MDER_P3 0x8U
#define UPIO_MDER_P3_BIT 3
#define UPIO_MDER_P4_MASK 0x10U
#define UPIO_MDER_P4 0x10U
#define UPIO_MDER_P4_BIT 4
#define UPIO_MDER_P5_MASK 0x20U
#define UPIO_MDER_P5 0x20U
#define UPIO_MDER_P5_BIT 5
#define UPIO_MDER_P6_MASK 0x40U
#define UPIO_MDER_P6 0x40U
#define UPIO_MDER_P6_BIT 6
#define UPIO_MDER_P7_MASK 0x80U
#define UPIO_MDER_P7 0x80U
#define UPIO_MDER_P7_BIT 7
#define UPIO_MDER_P8_MASK 0x100U
#define UPIO_MDER_P8 0x100U
#define UPIO_MDER_P8_BIT 8
#define UPIO_MDER_P9_MASK 0x200U
#define UPIO_MDER_P9 0x200U
#define UPIO_MDER_P9_BIT 9
#define UPIO_MDER_P10_MASK 0x400U
#define UPIO_MDER_P10 0x400U
#define UPIO_MDER_P10_BIT 10
#define UPIO_MDER_P11_MASK 0x800U
#define UPIO_MDER_P11 0x800U
#define UPIO_MDER_P11_BIT 11
#define UPIO_MDER_P12_MASK 0x1000U
#define UPIO_MDER_P12 0x1000U
#define UPIO_MDER_P12_BIT 12
#define UPIO_MDER_P13_MASK 0x2000U
#define UPIO_MDER_P13 0x2000U
#define UPIO_MDER_P13_BIT 13
#define UPIO_MDER_P14_MASK 0x4000U
#define UPIO_MDER_P14 0x4000U
#define UPIO_MDER_P14_BIT 14
#define UPIO_MDER_P15_MASK 0x8000U
#define UPIO_MDER_P15 0x8000U
#define UPIO_MDER_P15_BIT 15
#define UPIO_MDER_P16_MASK 0x10000U
#define UPIO_MDER_P16 0x10000U
#define UPIO_MDER_P16_BIT 16
#define UPIO_MDER_P17_MASK 0x20000U
#define UPIO_MDER_P17 0x20000U
#define UPIO_MDER_P17_BIT 17

#define UPIO_MDDR (*(volatile unsigned long *)0xFFFD8044)
#define UPIO_MDDR_OFFSET 0x44
#define UPIO_MDDR_P0_MASK 0x1U
#define UPIO_MDDR_P0 0x1U
#define UPIO_MDDR_P0_BIT 0
#define UPIO_MDDR_P1_MASK 0x2U
#define UPIO_MDDR_P1 0x2U
#define UPIO_MDDR_P1_BIT 1
#define UPIO_MDDR_P2_MASK 0x4U
#define UPIO_MDDR_P2 0x4U
#define UPIO_MDDR_P2_BIT 2
#define UPIO_MDDR_P3_MASK 0x8U
#define UPIO_MDDR_P3 0x8U
#define UPIO_MDDR_P3_BIT 3
#define UPIO_MDDR_P4_MASK 0x10U
#define UPIO_MDDR_P4 0x10U
#define UPIO_MDDR_P4_BIT 4
#define UPIO_MDDR_P5_MASK 0x20U
#define UPIO_MDDR_P5 0x20U
#define UPIO_MDDR_P5_BIT 5
#define UPIO_MDDR_P6_MASK 0x40U
#define UPIO_MDDR_P6 0x40U
#define UPIO_MDDR_P6_BIT 6
#define UPIO_MDDR_P7_MASK 0x80U
#define UPIO_MDDR_P7 0x80U
#define UPIO_MDDR_P7_BIT 7
#define UPIO_MDDR_P8_MASK 0x100U
#define UPIO_MDDR_P8 0x100U
#define UPIO_MDDR_P8_BIT 8
#define UPIO_MDDR_P9_MASK 0x200U
#define UPIO_MDDR_P9 0x200U
#define UPIO_MDDR_P9_BIT 9
#define UPIO_MDDR_P10_MASK 0x400U
#define UPIO_MDDR_P10 0x400U
#define UPIO_MDDR_P10_BIT 10
#define UPIO_MDDR_P11_MASK 0x800U
#define UPIO_MDDR_P11 0x800U
#define UPIO_MDDR_P11_BIT 11
#define UPIO_MDDR_P12_MASK 0x1000U
#define UPIO_MDDR_P12 0x1000U
#define UPIO_MDDR_P12_BIT 12
#define UPIO_MDDR_P13_MASK 0x2000U
#define UPIO_MDDR_P13 0x2000U
#define UPIO_MDDR_P13_BIT 13
#define UPIO_MDDR_P14_MASK 0x4000U
#define UPIO_MDDR_P14 0x4000U
#define UPIO_MDDR_P14_BIT 14
#define UPIO_MDDR_P15_MASK 0x8000U
#define UPIO_MDDR_P15 0x8000U
#define UPIO_MDDR_P15_BIT 15
#define UPIO_MDDR_P16_MASK 0x10000U
#define UPIO_MDDR_P16 0x10000U
#define UPIO_MDDR_P16_BIT 16
#define UPIO_MDDR_P17_MASK 0x20000U
#define UPIO_MDDR_P17 0x20000U
#define UPIO_MDDR_P17_BIT 17

#define UPIO_MDSR (*(volatile unsigned long *)0xFFFD8048)
#define UPIO_MDSR_OFFSET 0x48
#define UPIO_MDSR_P0_MASK 0x1U
#define UPIO_MDSR_P0 0x1U
#define UPIO_MDSR_P0_BIT 0
#define UPIO_MDSR_P1_MASK 0x2U
#define UPIO_MDSR_P1 0x2U
#define UPIO_MDSR_P1_BIT 1
#define UPIO_MDSR_P2_MASK 0x4U
#define UPIO_MDSR_P2 0x4U
#define UPIO_MDSR_P2_BIT 2
#define UPIO_MDSR_P3_MASK 0x8U
#define UPIO_MDSR_P3 0x8U
#define UPIO_MDSR_P3_BIT 3
#define UPIO_MDSR_P4_MASK 0x10U
#define UPIO_MDSR_P4 0x10U
#define UPIO_MDSR_P4_BIT 4
#define UPIO_MDSR_P5_MASK 0x20U
#define UPIO_MDSR_P5 0x20U
#define UPIO_MDSR_P5_BIT 5
#define UPIO_MDSR_P6_MASK 0x40U
#define UPIO_MDSR_P6 0x40U
#define UPIO_MDSR_P6_BIT 6
#define UPIO_MDSR_P7_MASK 0x80U
#define UPIO_MDSR_P7 0x80U
#define UPIO_MDSR_P7_BIT 7
#define UPIO_MDSR_P8_MASK 0x100U
#define UPIO_MDSR_P8 0x100U
#define UPIO_MDSR_P8_BIT 8
#define UPIO_MDSR_P9_MASK 0x200U
#define UPIO_MDSR_P9 0x200U
#define UPIO_MDSR_P9_BIT 9
#define UPIO_MDSR_P10_MASK 0x400U
#define UPIO_MDSR_P10 0x400U
#define UPIO_MDSR_P10_BIT 10
#define UPIO_MDSR_P11_MASK 0x800U
#define UPIO_MDSR_P11 0x800U
#define UPIO_MDSR_P11_BIT 11
#define UPIO_MDSR_P12_MASK 0x1000U
#define UPIO_MDSR_P12 0x1000U
#define UPIO_MDSR_P12_BIT 12
#define UPIO_MDSR_P13_MASK 0x2000U
#define UPIO_MDSR_P13 0x2000U
#define UPIO_MDSR_P13_BIT 13
#define UPIO_MDSR_P14_MASK 0x4000U
#define UPIO_MDSR_P14 0x4000U
#define UPIO_MDSR_P14_BIT 14
#define UPIO_MDSR_P15_MASK 0x8000U
#define UPIO_MDSR_P15 0x8000U
#define UPIO_MDSR_P15_BIT 15
#define UPIO_MDSR_P16_MASK 0x10000U
#define UPIO_MDSR_P16 0x10000U
#define UPIO_MDSR_P16_BIT 16
#define UPIO_MDSR_P17_MASK 0x20000U
#define UPIO_MDSR_P17 0x20000U
#define UPIO_MDSR_P17_BIT 17

#define UPIO_ECR (*(volatile unsigned long *)0xFFFD8050)
#define UPIO_ECR_OFFSET 0x50
#define UPIO_ECR_PIO_MASK 0x1U
#define UPIO_ECR_PIO 0x1U
#define UPIO_ECR_PIO_BIT 0

#define UPIO_DCR (*(volatile unsigned long *)0xFFFD8054)
#define UPIO_DCR_OFFSET 0x54
#define UPIO_DCR_PIO_MASK 0x1U
#define UPIO_DCR_PIO 0x1U
#define UPIO_DCR_PIO_BIT 0

#define UPIO_PMSR (*(volatile unsigned long *)0xFFFD8058)
#define UPIO_PMSR_OFFSET 0x58
#define UPIO_PMSR_PIO_MASK 0x1U
#define UPIO_PMSR_PIO 0x1U
#define UPIO_PMSR_PIO_BIT 0

#define UPIO_CR (*(volatile unsigned long *)0xFFFD8060)
#define UPIO_CR_OFFSET 0x60
#define UPIO_CR_SWRST_MASK 0x1U
#define UPIO_CR_SWRST 0x1U
#define UPIO_CR_SWRST_BIT 0

#define UPIO_SR (*(volatile unsigned long *)0xFFFD8070)
#define UPIO_SR_OFFSET 0x70
#define UPIO_SR_P0_MASK 0x1U
#define UPIO_SR_P0 0x1U
#define UPIO_SR_P0_BIT 0
#define UPIO_SR_P1_MASK 0x2U
#define UPIO_SR_P1 0x2U
#define UPIO_SR_P1_BIT 1
#define UPIO_SR_P2_MASK 0x4U
#define UPIO_SR_P2 0x4U
#define UPIO_SR_P2_BIT 2
#define UPIO_SR_P3_MASK 0x8U
#define UPIO_SR_P3 0x8U
#define UPIO_SR_P3_BIT 3
#define UPIO_SR_P4_MASK 0x10U
#define UPIO_SR_P4 0x10U
#define UPIO_SR_P4_BIT 4
#define UPIO_SR_P5_MASK 0x20U
#define UPIO_SR_P5 0x20U
#define UPIO_SR_P5_BIT 5
#define UPIO_SR_P6_MASK 0x40U
#define UPIO_SR_P6 0x40U
#define UPIO_SR_P6_BIT 6
#define UPIO_SR_P7_MASK 0x80U
#define UPIO_SR_P7 0x80U
#define UPIO_SR_P7_BIT 7
#define UPIO_SR_P8_MASK 0x100U
#define UPIO_SR_P8 0x100U
#define UPIO_SR_P8_BIT 8
#define UPIO_SR_P9_MASK 0x200U
#define UPIO_SR_P9 0x200U
#define UPIO_SR_P9_BIT 9
#define UPIO_SR_P10_MASK 0x400U
#define UPIO_SR_P10 0x400U
#define UPIO_SR_P10_BIT 10
#define UPIO_SR_P11_MASK 0x800U
#define UPIO_SR_P11 0x800U
#define UPIO_SR_P11_BIT 11
#define UPIO_SR_P12_MASK 0x1000U
#define UPIO_SR_P12 0x1000U
#define UPIO_SR_P12_BIT 12
#define UPIO_SR_P13_MASK 0x2000U
#define UPIO_SR_P13 0x2000U
#define UPIO_SR_P13_BIT 13
#define UPIO_SR_P14_MASK 0x4000U
#define UPIO_SR_P14 0x4000U
#define UPIO_SR_P14_BIT 14
#define UPIO_SR_P15_MASK 0x8000U
#define UPIO_SR_P15 0x8000U
#define UPIO_SR_P15_BIT 15
#define UPIO_SR_P16_MASK 0x10000U
#define UPIO_SR_P16 0x10000U
#define UPIO_SR_P16_BIT 16
#define UPIO_SR_P17_MASK 0x20000U
#define UPIO_SR_P17 0x20000U
#define UPIO_SR_P17_BIT 17

#define UPIO_IER (*(volatile unsigned long *)0xFFFD8074)
#define UPIO_IER_OFFSET 0x74
#define UPIO_IER_P0_MASK 0x1U
#define UPIO_IER_P0 0x1U
#define UPIO_IER_P0_BIT 0
#define UPIO_IER_P1_MASK 0x2U
#define UPIO_IER_P1 0x2U
#define UPIO_IER_P1_BIT 1
#define UPIO_IER_P2_MASK 0x4U
#define UPIO_IER_P2 0x4U
#define UPIO_IER_P2_BIT 2
#define UPIO_IER_P3_MASK 0x8U
#define UPIO_IER_P3 0x8U
#define UPIO_IER_P3_BIT 3
#define UPIO_IER_P4_MASK 0x10U
#define UPIO_IER_P4 0x10U
#define UPIO_IER_P4_BIT 4
#define UPIO_IER_P5_MASK 0x20U
#define UPIO_IER_P5 0x20U
#define UPIO_IER_P5_BIT 5
#define UPIO_IER_P6_MASK 0x40U
#define UPIO_IER_P6 0x40U
#define UPIO_IER_P6_BIT 6
#define UPIO_IER_P7_MASK 0x80U
#define UPIO_IER_P7 0x80U
#define UPIO_IER_P7_BIT 7
#define UPIO_IER_P8_MASK 0x100U
#define UPIO_IER_P8 0x100U
#define UPIO_IER_P8_BIT 8
#define UPIO_IER_P9_MASK 0x200U
#define UPIO_IER_P9 0x200U
#define UPIO_IER_P9_BIT 9
#define UPIO_IER_P10_MASK 0x400U
#define UPIO_IER_P10 0x400U
#define UPIO_IER_P10_BIT 10
#define UPIO_IER_P11_MASK 0x800U
#define UPIO_IER_P11 0x800U
#define UPIO_IER_P11_BIT 11
#define UPIO_IER_P12_MASK 0x1000U
#define UPIO_IER_P12 0x1000U
#define UPIO_IER_P12_BIT 12
#define UPIO_IER_P13_MASK 0x2000U
#define UPIO_IER_P13 0x2000U
#define UPIO_IER_P13_BIT 13
#define UPIO_IER_P14_MASK 0x4000U
#define UPIO_IER_P14 0x4000U
#define UPIO_IER_P14_BIT 14
#define UPIO_IER_P15_MASK 0x8000U
#define UPIO_IER_P15 0x8000U
#define UPIO_IER_P15_BIT 15
#define UPIO_IER_P16_MASK 0x10000U
#define UPIO_IER_P16 0x10000U
#define UPIO_IER_P16_BIT 16
#define UPIO_IER_P17_MASK 0x20000U
#define UPIO_IER_P17 0x20000U
#define UPIO_IER_P17_BIT 17

#define UPIO_IDR (*(volatile unsigned long *)0xFFFD8078)
#define UPIO_IDR_OFFSET 0x78
#define UPIO_IDR_P0_MASK 0x1U
#define UPIO_IDR_P0 0x1U
#define UPIO_IDR_P0_BIT 0
#define UPIO_IDR_P1_MASK 0x2U
#define UPIO_IDR_P1 0x2U
#define UPIO_IDR_P1_BIT 1
#define UPIO_IDR_P2_MASK 0x4U
#define UPIO_IDR_P2 0x4U
#define UPIO_IDR_P2_BIT 2
#define UPIO_IDR_P3_MASK 0x8U
#define UPIO_IDR_P3 0x8U
#define UPIO_IDR_P3_BIT 3
#define UPIO_IDR_P4_MASK 0x10U
#define UPIO_IDR_P4 0x10U
#define UPIO_IDR_P4_BIT 4
#define UPIO_IDR_P5_MASK 0x20U
#define UPIO_IDR_P5 0x20U
#define UPIO_IDR_P5_BIT 5
#define UPIO_IDR_P6_MASK 0x40U
#define UPIO_IDR_P6 0x40U
#define UPIO_IDR_P6_BIT 6
#define UPIO_IDR_P7_MASK 0x80U
#define UPIO_IDR_P7 0x80U
#define UPIO_IDR_P7_BIT 7
#define UPIO_IDR_P8_MASK 0x100U
#define UPIO_IDR_P8 0x100U
#define UPIO_IDR_P8_BIT 8
#define UPIO_IDR_P9_MASK 0x200U
#define UPIO_IDR_P9 0x200U
#define UPIO_IDR_P9_BIT 9
#define UPIO_IDR_P10_MASK 0x400U
#define UPIO_IDR_P10 0x400U
#define UPIO_IDR_P10_BIT 10
#define UPIO_IDR_P11_MASK 0x800U
#define UPIO_IDR_P11 0x800U
#define UPIO_IDR_P11_BIT 11
#define UPIO_IDR_P12_MASK 0x1000U
#define UPIO_IDR_P12 0x1000U
#define UPIO_IDR_P12_BIT 12
#define UPIO_IDR_P13_MASK 0x2000U
#define UPIO_IDR_P13 0x2000U
#define UPIO_IDR_P13_BIT 13
#define UPIO_IDR_P14_MASK 0x4000U
#define UPIO_IDR_P14 0x4000U
#define UPIO_IDR_P14_BIT 14
#define UPIO_IDR_P15_MASK 0x8000U
#define UPIO_IDR_P15 0x8000U
#define UPIO_IDR_P15_BIT 15
#define UPIO_IDR_P16_MASK 0x10000U
#define UPIO_IDR_P16 0x10000U
#define UPIO_IDR_P16_BIT 16
#define UPIO_IDR_P17_MASK 0x20000U
#define UPIO_IDR_P17 0x20000U
#define UPIO_IDR_P17_BIT 17

#define UPIO_IMR (*(volatile unsigned long *)0xFFFD807C)
#define UPIO_IMR_OFFSET 0x7C
#define UPIO_IMR_P0_MASK 0x1U
#define UPIO_IMR_P0 0x1U
#define UPIO_IMR_P0_BIT 0
#define UPIO_IMR_P1_MASK 0x2U
#define UPIO_IMR_P1 0x2U
#define UPIO_IMR_P1_BIT 1
#define UPIO_IMR_P2_MASK 0x4U
#define UPIO_IMR_P2 0x4U
#define UPIO_IMR_P2_BIT 2
#define UPIO_IMR_P3_MASK 0x8U
#define UPIO_IMR_P3 0x8U
#define UPIO_IMR_P3_BIT 3
#define UPIO_IMR_P4_MASK 0x10U
#define UPIO_IMR_P4 0x10U
#define UPIO_IMR_P4_BIT 4
#define UPIO_IMR_P5_MASK 0x20U
#define UPIO_IMR_P5 0x20U
#define UPIO_IMR_P5_BIT 5
#define UPIO_IMR_P6_MASK 0x40U
#define UPIO_IMR_P6 0x40U
#define UPIO_IMR_P6_BIT 6
#define UPIO_IMR_P7_MASK 0x80U
#define UPIO_IMR_P7 0x80U
#define UPIO_IMR_P7_BIT 7
#define UPIO_IMR_P8_MASK 0x100U
#define UPIO_IMR_P8 0x100U
#define UPIO_IMR_P8_BIT 8
#define UPIO_IMR_P9_MASK 0x200U
#define UPIO_IMR_P9 0x200U
#define UPIO_IMR_P9_BIT 9
#define UPIO_IMR_P10_MASK 0x400U
#define UPIO_IMR_P10 0x400U
#define UPIO_IMR_P10_BIT 10
#define UPIO_IMR_P11_MASK 0x800U
#define UPIO_IMR_P11 0x800U
#define UPIO_IMR_P11_BIT 11
#define UPIO_IMR_P12_MASK 0x1000U
#define UPIO_IMR_P12 0x1000U
#define UPIO_IMR_P12_BIT 12
#define UPIO_IMR_P13_MASK 0x2000U
#define UPIO_IMR_P13 0x2000U
#define UPIO_IMR_P13_BIT 13
#define UPIO_IMR_P14_MASK 0x4000U
#define UPIO_IMR_P14 0x4000U
#define UPIO_IMR_P14_BIT 14
#define UPIO_IMR_P15_MASK 0x8000U
#define UPIO_IMR_P15 0x8000U
#define UPIO_IMR_P15_BIT 15
#define UPIO_IMR_P16_MASK 0x10000U
#define UPIO_IMR_P16 0x10000U
#define UPIO_IMR_P16_BIT 16
#define UPIO_IMR_P17_MASK 0x20000U
#define UPIO_IMR_P17 0x20000U
#define UPIO_IMR_P17_BIT 17


#define PWM_BASE 0xFFFD0000

#define PWM_PER (*(volatile unsigned long *)0xFFFD0000)
#define PWM_PER_OFFSET 0x0
#define PWM_PER_PWM0_MASK 0x10000U
#define PWM_PER_PWM0 0x10000U
#define PWM_PER_PWM0_BIT 16
#define PWM_PER_PWM1_MASK 0x20000U
#define PWM_PER_PWM1 0x20000U
#define PWM_PER_PWM1_BIT 17
#define PWM_PER_PWM2_MASK 0x40000U
#define PWM_PER_PWM2 0x40000U
#define PWM_PER_PWM2_BIT 18
#define PWM_PER_PWM3_MASK 0x80000U
#define PWM_PER_PWM3 0x80000U
#define PWM_PER_PWM3_BIT 19

#define PWM_PDR (*(volatile unsigned long *)0xFFFD0004)
#define PWM_PDR_OFFSET 0x4
#define PWM_PDR_PWM0_MASK 0x10000U
#define PWM_PDR_PWM0 0x10000U
#define PWM_PDR_PWM0_BIT 16
#define PWM_PDR_PWM1_MASK 0x20000U
#define PWM_PDR_PWM1 0x20000U
#define PWM_PDR_PWM1_BIT 17
#define PWM_PDR_PWM2_MASK 0x40000U
#define PWM_PDR_PWM2 0x40000U
#define PWM_PDR_PWM2_BIT 18
#define PWM_PDR_PWM3_MASK 0x80000U
#define PWM_PDR_PWM3 0x80000U
#define PWM_PDR_PWM3_BIT 19

#define PWM_PSR (*(volatile unsigned long *)0xFFFD0008)
#define PWM_PSR_OFFSET 0x8
#define PWM_PSR_PWM0_MASK 0x10000U
#define PWM_PSR_PWM0 0x10000U
#define PWM_PSR_PWM0_BIT 16
#define PWM_PSR_PWM1_MASK 0x20000U
#define PWM_PSR_PWM1 0x20000U
#define PWM_PSR_PWM1_BIT 17
#define PWM_PSR_PWM2_MASK 0x40000U
#define PWM_PSR_PWM2 0x40000U
#define PWM_PSR_PWM2_BIT 18
#define PWM_PSR_PWM3_MASK 0x80000U
#define PWM_PSR_PWM3 0x80000U
#define PWM_PSR_PWM3_BIT 19

#define PWM_OER (*(volatile unsigned long *)0xFFFD0010)
#define PWM_OER_OFFSET 0x10
#define PWM_OER_PWM0_MASK 0x10000U
#define PWM_OER_PWM0 0x10000U
#define PWM_OER_PWM0_BIT 16
#define PWM_OER_PWM1_MASK 0x20000U
#define PWM_OER_PWM1 0x20000U
#define PWM_OER_PWM1_BIT 17
#define PWM_OER_PWM2_MASK 0x40000U
#define PWM_OER_PWM2 0x40000U
#define PWM_OER_PWM2_BIT 18
#define PWM_OER_PWM3_MASK 0x80000U
#define PWM_OER_PWM3 0x80000U
#define PWM_OER_PWM3_BIT 19

#define PWM_ODR (*(volatile unsigned long *)0xFFFD0014)
#define PWM_ODR_OFFSET 0x14
#define PWM_ODR_PWM0_MASK 0x10000U
#define PWM_ODR_PWM0 0x10000U
#define PWM_ODR_PWM0_BIT 16
#define PWM_ODR_PWM1_MASK 0x20000U
#define PWM_ODR_PWM1 0x20000U
#define PWM_ODR_PWM1_BIT 17
#define PWM_ODR_PWM2_MASK 0x40000U
#define PWM_ODR_PWM2 0x40000U
#define PWM_ODR_PWM2_BIT 18
#define PWM_ODR_PWM3_MASK 0x80000U
#define PWM_ODR_PWM3 0x80000U
#define PWM_ODR_PWM3_BIT 19

#define PWM_OSR (*(volatile unsigned long *)0xFFFD0018)
#define PWM_OSR_OFFSET 0x18
#define PWM_OSR_PWM0_MASK 0x10000U
#define PWM_OSR_PWM0 0x10000U
#define PWM_OSR_PWM0_BIT 16
#define PWM_OSR_PWM1_MASK 0x20000U
#define PWM_OSR_PWM1 0x20000U
#define PWM_OSR_PWM1_BIT 17
#define PWM_OSR_PWM2_MASK 0x40000U
#define PWM_OSR_PWM2 0x40000U
#define PWM_OSR_PWM2_BIT 18
#define PWM_OSR_PWM3_MASK 0x80000U
#define PWM_OSR_PWM3 0x80000U
#define PWM_OSR_PWM3_BIT 19

#define PWM_SODR (*(volatile unsigned long *)0xFFFD0030)
#define PWM_SODR_OFFSET 0x30
#define PWM_SODR_PWM0_MASK 0x10000U
#define PWM_SODR_PWM0 0x10000U
#define PWM_SODR_PWM0_BIT 16
#define PWM_SODR_PWM1_MASK 0x20000U
#define PWM_SODR_PWM1 0x20000U
#define PWM_SODR_PWM1_BIT 17
#define PWM_SODR_PWM2_MASK 0x40000U
#define PWM_SODR_PWM2 0x40000U
#define PWM_SODR_PWM2_BIT 18
#define PWM_SODR_PWM3_MASK 0x80000U
#define PWM_SODR_PWM3 0x80000U
#define PWM_SODR_PWM3_BIT 19

#define PWM_CODR (*(volatile unsigned long *)0xFFFD0034)
#define PWM_CODR_OFFSET 0x34
#define PWM_CODR_PWM0_MASK 0x10000U
#define PWM_CODR_PWM0 0x10000U
#define PWM_CODR_PWM0_BIT 16
#define PWM_CODR_PWM1_MASK 0x20000U
#define PWM_CODR_PWM1 0x20000U
#define PWM_CODR_PWM1_BIT 17
#define PWM_CODR_PWM2_MASK 0x40000U
#define PWM_CODR_PWM2 0x40000U
#define PWM_CODR_PWM2_BIT 18
#define PWM_CODR_PWM3_MASK 0x80000U
#define PWM_CODR_PWM3 0x80000U
#define PWM_CODR_PWM3_BIT 19

#define PWM_ODSR (*(volatile unsigned long *)0xFFFD0038)
#define PWM_ODSR_OFFSET 0x38
#define PWM_ODSR_PWM0_MASK 0x10000U
#define PWM_ODSR_PWM0 0x10000U
#define PWM_ODSR_PWM0_BIT 16
#define PWM_ODSR_PWM1_MASK 0x20000U
#define PWM_ODSR_PWM1 0x20000U
#define PWM_ODSR_PWM1_BIT 17
#define PWM_ODSR_PWM2_MASK 0x40000U
#define PWM_ODSR_PWM2 0x40000U
#define PWM_ODSR_PWM2_BIT 18
#define PWM_ODSR_PWM3_MASK 0x80000U
#define PWM_ODSR_PWM3 0x80000U
#define PWM_ODSR_PWM3_BIT 19

#define PWM_PDSR (*(volatile unsigned long *)0xFFFD003C)
#define PWM_PDSR_OFFSET 0x3C
#define PWM_PDSR_PWM0_MASK 0x10000U
#define PWM_PDSR_PWM0 0x10000U
#define PWM_PDSR_PWM0_BIT 16
#define PWM_PDSR_PWM1_MASK 0x20000U
#define PWM_PDSR_PWM1 0x20000U
#define PWM_PDSR_PWM1_BIT 17
#define PWM_PDSR_PWM2_MASK 0x40000U
#define PWM_PDSR_PWM2 0x40000U
#define PWM_PDSR_PWM2_BIT 18
#define PWM_PDSR_PWM3_MASK 0x80000U
#define PWM_PDSR_PWM3 0x80000U
#define PWM_PDSR_PWM3_BIT 19

#define PWM_MDER (*(volatile unsigned long *)0xFFFD0040)
#define PWM_MDER_OFFSET 0x40
#define PWM_MDER_PWM0_MASK 0x10000U
#define PWM_MDER_PWM0 0x10000U
#define PWM_MDER_PWM0_BIT 16
#define PWM_MDER_PWM1_MASK 0x20000U
#define PWM_MDER_PWM1 0x20000U
#define PWM_MDER_PWM1_BIT 17
#define PWM_MDER_PWM2_MASK 0x40000U
#define PWM_MDER_PWM2 0x40000U
#define PWM_MDER_PWM2_BIT 18
#define PWM_MDER_PWM3_MASK 0x80000U
#define PWM_MDER_PWM3 0x80000U
#define PWM_MDER_PWM3_BIT 19

#define PWM_MDDR (*(volatile unsigned long *)0xFFFD0044)
#define PWM_MDDR_OFFSET 0x44
#define PWM_MDDR_PWM0_MASK 0x10000U
#define PWM_MDDR_PWM0 0x10000U
#define PWM_MDDR_PWM0_BIT 16
#define PWM_MDDR_PWM1_MASK 0x20000U
#define PWM_MDDR_PWM1 0x20000U
#define PWM_MDDR_PWM1_BIT 17
#define PWM_MDDR_PWM2_MASK 0x40000U
#define PWM_MDDR_PWM2 0x40000U
#define PWM_MDDR_PWM2_BIT 18
#define PWM_MDDR_PWM3_MASK 0x80000U
#define PWM_MDDR_PWM3 0x80000U
#define PWM_MDDR_PWM3_BIT 19

#define PWM_MDSR (*(volatile unsigned long *)0xFFFD0048)
#define PWM_MDSR_OFFSET 0x48
#define PWM_MDSR_PWM0_MASK 0x10000U
#define PWM_MDSR_PWM0 0x10000U
#define PWM_MDSR_PWM0_BIT 16
#define PWM_MDSR_PWM1_MASK 0x20000U
#define PWM_MDSR_PWM1 0x20000U
#define PWM_MDSR_PWM1_BIT 17
#define PWM_MDSR_PWM2_MASK 0x40000U
#define PWM_MDSR_PWM2 0x40000U
#define PWM_MDSR_PWM2_BIT 18
#define PWM_MDSR_PWM3_MASK 0x80000U
#define PWM_MDSR_PWM3 0x80000U
#define PWM_MDSR_PWM3_BIT 19

#define PWM_ECR (*(volatile unsigned long *)0xFFFD0050)
#define PWM_ECR_OFFSET 0x50
#define PWM_ECR_PWM_MASK 0x2U
#define PWM_ECR_PWM 0x2U
#define PWM_ECR_PWM_BIT 1

#define PWM_DCR (*(volatile unsigned long *)0xFFFD0054)
#define PWM_DCR_OFFSET 0x54
#define PWM_DCR_PWM_MASK 0x2U
#define PWM_DCR_PWM 0x2U
#define PWM_DCR_PWM_BIT 1

#define PWM_PMSR (*(volatile unsigned long *)0xFFFD0058)
#define PWM_PMSR_OFFSET 0x58
#define PWM_PMSR_PWM_MASK 0x2U
#define PWM_PMSR_PWM 0x2U
#define PWM_PMSR_PWM_BIT 1

#define PWM_CR (*(volatile unsigned long *)0xFFFD0060)
#define PWM_CR_OFFSET 0x60
#define PWM_CR_SWRST_MASK 0x1U
#define PWM_CR_SWRST 0x1U
#define PWM_CR_SWRST_BIT 0
#define PWM_CR_PWMEN0_MASK 0x2U
#define PWM_CR_PWMEN0 0x2U
#define PWM_CR_PWMEN0_BIT 1
#define PWM_CR_PWMDIS0_MASK 0x4U
#define PWM_CR_PWMDIS0 0x4U
#define PWM_CR_PWMDIS0_BIT 2
#define PWM_CR_PWMEN1_MASK 0x8U
#define PWM_CR_PWMEN1 0x8U
#define PWM_CR_PWMEN1_BIT 3
#define PWM_CR_PWMDIS1_MASK 0x10U
#define PWM_CR_PWMDIS1 0x10U
#define PWM_CR_PWMDIS1_BIT 4
#define PWM_CR_PWMEN2_MASK 0x20U
#define PWM_CR_PWMEN2 0x20U
#define PWM_CR_PWMEN2_BIT 5
#define PWM_CR_PWMDIS2_MASK 0x40U
#define PWM_CR_PWMDIS2 0x40U
#define PWM_CR_PWMDIS2_BIT 6
#define PWM_CR_PWMEN3_MASK 0x80U
#define PWM_CR_PWMEN3 0x80U
#define PWM_CR_PWMEN3_BIT 7
#define PWM_CR_PWMDIS3_MASK 0x100U
#define PWM_CR_PWMDIS3 0x100U
#define PWM_CR_PWMDIS3_BIT 8

#define PWM_MR (*(volatile unsigned long *)0xFFFD0064)
#define PWM_MR_OFFSET 0x64
#define PWM_MR_PRESCAL0_MASK 0xFU
#define PWM_MR_PRESCAL0_BIT 0
#define PWM_MR_PL0_MASK 0x10U
#define PWM_MR_PL0 0x10U
#define PWM_MR_PL0_BIT 4
#define PWM_MR_PRESCAL1_MASK 0xF00U
#define PWM_MR_PRESCAL1_BIT 8
#define PWM_MR_PL1_MASK 0x1000U
#define PWM_MR_PL1 0x1000U
#define PWM_MR_PL1_BIT 12
#define PWM_MR_PRESCAL2_MASK 0xF0000U
#define PWM_MR_PRESCAL2_BIT 16
#define PWM_MR_PL2_MASK 0x100000U
#define PWM_MR_PL2 0x100000U
#define PWM_MR_PL2_BIT 20
#define PWM_MR_PRESCAL3_MASK 0xF000000U
#define PWM_MR_PRESCAL3_BIT 24
#define PWM_MR_PL3_MASK 0x10000000U
#define PWM_MR_PL3 0x10000000U
#define PWM_MR_PL3_BIT 28

#define PWM_CSR (*(volatile unsigned long *)0xFFFD006C)
#define PWM_CSR_OFFSET 0x6C
#define PWM_CSR_PSTA0_MASK 0x1U
#define PWM_CSR_PSTA0 0x1U
#define PWM_CSR_PSTA0_BIT 0
#define PWM_CSR_PEND0_MASK 0x2U
#define PWM_CSR_PEND0 0x2U
#define PWM_CSR_PEND0_BIT 1
#define PWM_CSR_PSTA1_MASK 0x4U
#define PWM_CSR_PSTA1 0x4U
#define PWM_CSR_PSTA1_BIT 2
#define PWM_CSR_PEND1_MASK 0x8U
#define PWM_CSR_PEND1 0x8U
#define PWM_CSR_PEND1_BIT 3
#define PWM_CSR_PSTA2_MASK 0x10U
#define PWM_CSR_PSTA2 0x10U
#define PWM_CSR_PSTA2_BIT 4
#define PWM_CSR_PEND2_MASK 0x20U
#define PWM_CSR_PEND2 0x20U
#define PWM_CSR_PEND2_BIT 5
#define PWM_CSR_PSTA3_MASK 0x40U
#define PWM_CSR_PSTA3 0x40U
#define PWM_CSR_PSTA3_BIT 6
#define PWM_CSR_PEND3_MASK 0x80U
#define PWM_CSR_PEND3 0x80U
#define PWM_CSR_PEND3_BIT 7

#define PWM_SR (*(volatile unsigned long *)0xFFFD0070)
#define PWM_SR_OFFSET 0x70
#define PWM_SR_PSTA0_MASK 0x1U
#define PWM_SR_PSTA0 0x1U
#define PWM_SR_PSTA0_BIT 0
#define PWM_SR_PEND0_MASK 0x2U
#define PWM_SR_PEND0 0x2U
#define PWM_SR_PEND0_BIT 1
#define PWM_SR_PSTA1_MASK 0x4U
#define PWM_SR_PSTA1 0x4U
#define PWM_SR_PSTA1_BIT 2
#define PWM_SR_PEND1_MASK 0x8U
#define PWM_SR_PEND1 0x8U
#define PWM_SR_PEND1_BIT 3
#define PWM_SR_PSTA2_MASK 0x10U
#define PWM_SR_PSTA2 0x10U
#define PWM_SR_PSTA2_BIT 4
#define PWM_SR_PEND2_MASK 0x20U
#define PWM_SR_PEND2 0x20U
#define PWM_SR_PEND2_BIT 5
#define PWM_SR_PSTA3_MASK 0x40U
#define PWM_SR_PSTA3 0x40U
#define PWM_SR_PSTA3_BIT 6
#define PWM_SR_PEND3_MASK 0x80U
#define PWM_SR_PEND3 0x80U
#define PWM_SR_PEND3_BIT 7
#define PWM_SR_PWMENS0_MASK 0x100U
#define PWM_SR_PWMENS0 0x100U
#define PWM_SR_PWMENS0_BIT 8
#define PWM_SR_PWMENS1_MASK 0x200U
#define PWM_SR_PWMENS1 0x200U
#define PWM_SR_PWMENS1_BIT 9
#define PWM_SR_PWMENS2_MASK 0x400U
#define PWM_SR_PWMENS2 0x400U
#define PWM_SR_PWMENS2_BIT 10
#define PWM_SR_PWMENS3_MASK 0x800U
#define PWM_SR_PWMENS3 0x800U
#define PWM_SR_PWMENS3_BIT 11

#define PWM_IER (*(volatile unsigned long *)0xFFFD0074)
#define PWM_IER_OFFSET 0x74
#define PWM_IER_PSTA0_MASK 0x1U
#define PWM_IER_PSTA0 0x1U
#define PWM_IER_PSTA0_BIT 0
#define PWM_IER_PEND0_MASK 0x2U
#define PWM_IER_PEND0 0x2U
#define PWM_IER_PEND0_BIT 1
#define PWM_IER_PSTA1_MASK 0x4U
#define PWM_IER_PSTA1 0x4U
#define PWM_IER_PSTA1_BIT 2
#define PWM_IER_PEND1_MASK 0x8U
#define PWM_IER_PEND1 0x8U
#define PWM_IER_PEND1_BIT 3
#define PWM_IER_PSTA2_MASK 0x10U
#define PWM_IER_PSTA2 0x10U
#define PWM_IER_PSTA2_BIT 4
#define PWM_IER_PEND2_MASK 0x20U
#define PWM_IER_PEND2 0x20U
#define PWM_IER_PEND2_BIT 5
#define PWM_IER_PSTA3_MASK 0x40U
#define PWM_IER_PSTA3 0x40U
#define PWM_IER_PSTA3_BIT 6
#define PWM_IER_PEND3_MASK 0x80U
#define PWM_IER_PEND3 0x80U
#define PWM_IER_PEND3_BIT 7

#define PWM_IDR (*(volatile unsigned long *)0xFFFD0078)
#define PWM_IDR_OFFSET 0x78
#define PWM_IDR_PSTA0_MASK 0x1U
#define PWM_IDR_PSTA0 0x1U
#define PWM_IDR_PSTA0_BIT 0
#define PWM_IDR_PEND0_MASK 0x2U
#define PWM_IDR_PEND0 0x2U
#define PWM_IDR_PEND0_BIT 1
#define PWM_IDR_PSTA1_MASK 0x4U
#define PWM_IDR_PSTA1 0x4U
#define PWM_IDR_PSTA1_BIT 2
#define PWM_IDR_PEND1_MASK 0x8U
#define PWM_IDR_PEND1 0x8U
#define PWM_IDR_PEND1_BIT 3
#define PWM_IDR_PSTA2_MASK 0x10U
#define PWM_IDR_PSTA2 0x10U
#define PWM_IDR_PSTA2_BIT 4
#define PWM_IDR_PEND2_MASK 0x20U
#define PWM_IDR_PEND2 0x20U
#define PWM_IDR_PEND2_BIT 5
#define PWM_IDR_PSTA3_MASK 0x40U
#define PWM_IDR_PSTA3 0x40U
#define PWM_IDR_PSTA3_BIT 6
#define PWM_IDR_PEND3_MASK 0x80U
#define PWM_IDR_PEND3 0x80U
#define PWM_IDR_PEND3_BIT 7

#define PWM_IMR (*(volatile unsigned long *)0xFFFD007C)
#define PWM_IMR_OFFSET 0x7C
#define PWM_IMR_PSTA0_MASK 0x1U
#define PWM_IMR_PSTA0 0x1U
#define PWM_IMR_PSTA0_BIT 0
#define PWM_IMR_PEND0_MASK 0x2U
#define PWM_IMR_PEND0 0x2U
#define PWM_IMR_PEND0_BIT 1
#define PWM_IMR_PSTA1_MASK 0x4U
#define PWM_IMR_PSTA1 0x4U
#define PWM_IMR_PSTA1_BIT 2
#define PWM_IMR_PEND1_MASK 0x8U
#define PWM_IMR_PEND1 0x8U
#define PWM_IMR_PEND1_BIT 3
#define PWM_IMR_PSTA2_MASK 0x10U
#define PWM_IMR_PSTA2 0x10U
#define PWM_IMR_PSTA2_BIT 4
#define PWM_IMR_PEND2_MASK 0x20U
#define PWM_IMR_PEND2 0x20U
#define PWM_IMR_PEND2_BIT 5
#define PWM_IMR_PSTA3_MASK 0x40U
#define PWM_IMR_PSTA3 0x40U
#define PWM_IMR_PSTA3_BIT 6
#define PWM_IMR_PEND3_MASK 0x80U
#define PWM_IMR_PEND3 0x80U
#define PWM_IMR_PEND3_BIT 7

#define PWM_DLY_0 (*(volatile unsigned long *)0xFFFD0080)
#define PWM_DLY_0_OFFSET 0x80
#define PWM_DLY_0_DELAY_MASK 0xFFFFU
#define PWM_DLY_0_DELAY_BIT 0

#define PWM_PUL_0 (*(volatile unsigned long *)0xFFFD0084)
#define PWM_PUL_0_OFFSET 0x84
#define PWM_PUL_0_PULSE_MASK 0xFFFFU
#define PWM_PUL_0_PULSE_BIT 0

#define PWM_DLY_1 (*(volatile unsigned long *)0xFFFD0088)
#define PWM_DLY_1_OFFSET 0x88
#define PWM_DLY_1_DELAY_MASK 0xFFFFU
#define PWM_DLY_1_DELAY_BIT 0

#define PWM_PUL_1 (*(volatile unsigned long *)0xFFFD008C)
#define PWM_PUL_1_OFFSET 0x8C
#define PWM_PUL_1_PULSE_MASK 0xFFFFU
#define PWM_PUL_1_PULSE_BIT 0

#define PWM_DLY_2 (*(volatile unsigned long *)0xFFFD0090)
#define PWM_DLY_2_OFFSET 0x90
#define PWM_DLY_2_DELAY_MASK 0xFFFFU
#define PWM_DLY_2_DELAY_BIT 0

#define PWM_PUL_2 (*(volatile unsigned long *)0xFFFD0094)
#define PWM_PUL_2_OFFSET 0x94
#define PWM_PUL_2_PULSE_MASK 0xFFFFU
#define PWM_PUL_2_PULSE_BIT 0

#define PWM_DLY_3 (*(volatile unsigned long *)0xFFFD0098)
#define PWM_DLY_3_OFFSET 0x98
#define PWM_DLY_3_DELAY_MASK 0xFFFFU
#define PWM_DLY_3_DELAY_BIT 0

#define PWM_PUL_3 (*(volatile unsigned long *)0xFFFD009C)
#define PWM_PUL_3_OFFSET 0x9C
#define PWM_PUL_3_PULSE_MASK 0xFFFFU
#define PWM_PUL_3_PULSE_BIT 0


// *****************************************************************************
//               BASE ADDRESS DEFINITIONS FOR AT91SAM7A1
// *****************************************************************************
#define AT91C_BASE_SYS			((AT91PS_SYS) 	0xFFFFF000) // (SYS) Base Address
#define AT91C_BASE_AIC			((AT91PS_AIC) 	0xFFFFF000) // (AIC) Base Address
#define AT91C_BASE_CM			((AT91PS_CM) 	0xFFFEC000) // (CM) Base Address
#define AT91C_BASE_USART0		((AT91PS_USART) 0xFFFA8000) // (USART0) Base Address
#define AT91C_BASE_USART1		((AT91PS_USART) 0xFFFAC000) // (USART0) Base Address
#define AT91C_BASE_USART2		((AT91PS_USART) 0xFFFB0000) // (USART0) Base Address
#define AT91C_BASE_PMC			((AT91PS_PMC) 	0xFFFF4050) // (PMC) Base Address
#define AT91C_BASE_GPT0			((AT91PS_GPT) 	0xFFFC8000) // (PMC) Base Address
#define AT91C_BASE_GPT1			((AT91PS_GPT) 	0xFFFC8100) // (PMC) Base Address
#define AT91C_BASE_GPT2			((AT91PS_GPT) 	0xFFFC8200) // (PMC) Base Address


#endif

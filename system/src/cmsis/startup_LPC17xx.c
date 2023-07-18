/* File: startup_LPC17xx.c
 * Purpose: startup file for LPC17xx Cortex-M3 devices.
 *          Should be used with GCC 'GNU Tools ARM Embedded'
 * Version: CMSIS 4.3.0
 * Date: 06 August 2015
 *
 * Created from the CMSIS template for the specified device
 * Quantum Leaps, www.state-machine.com
 *
 * NOTE: The function assert_failed defined at the end of this file
 * determines the error/assertion handling policy for the application and
 * might need to be customized for each project. This function is defined
 * using the GNU-ARM language extensions to avoid accessing the stack,
 * which might be corrupted by the time assert_failed is called.
 */
/* Copyright (c) 2011 - 2014 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
 ---------------------------------------------------------------------------*/


#include <cortexm/exception-handlers.h>


/* Function prototypes -----------------------------------------------------*/
void Default_Handler(void);  /* Default empty handler */
void Reset_Handler(void);    /* Reset Handler */
void SystemInit(void);       /* CMSIS system initialization */
void Q_onAssert(char const *module, int loc); /* QP assertion handler */

/*----------------------------------------------------------------------------
* weak aliases for each Exception handler to the Default_Handler.
* Any function with the same name will override these definitions.
*/
/* Cortex-M Processor fault exceptions... */
void NMI_Handler           (void) __attribute__ ((weak));
void HardFault_Handler     (void) __attribute__ ((weak));
void MemManage_Handler     (void) __attribute__ ((weak));
void BusFault_Handler      (void) __attribute__ ((weak));
void UsageFault_Handler    (void) __attribute__ ((weak));

/* Cortex-M Processor non-fault exceptions... */
void SVC_Handler           (void) __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler      (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler       (void) __attribute__ ((weak, alias("Default_Handler")));

/* external interrupts...   */
void WDT_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER0_IRQHandler      (void);// __attribute__ ((weak, alias("Default_Handler")));
void TIMER1_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER2_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void TIMER3_IRQHandler      (void) __attribute__ ((weak, alias("Default_Handler")));
void UART0_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART1_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART2_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void UART3_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void PWM1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void I2C2_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SPI_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void SSP0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void SSP1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void PLL0_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void RTC_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void EINT0_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void EINT1_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void EINT2_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void EINT3_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void ADC_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void BOD_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void USB_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void CAN_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void DMA_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void I2S_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void ENET_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void RIT_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void MCPWM_IRQHandler       (void) __attribute__ ((weak, alias("Default_Handler")));
void QEI_IRQHandler         (void) __attribute__ ((weak, alias("Default_Handler")));
void PLL1_IRQHandler        (void) __attribute__ ((weak, alias("Default_Handler")));
void USBActivity_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));
void CANActivity_IRQHandler (void) __attribute__ ((weak, alias("Default_Handler")));

extern unsigned int _estack;

typedef void
(* const pHandler)(void);

// ----------------------------------------------------------------------------

// The table of interrupt handlers. It has an explicit section name
// and relies on the linker script to place it at the correct location
// in memory.

__attribute__ ((section(".isr_vector"),used))
pHandler __isr_vectors[] =
  {
	(pHandler) &_estack,               // The initial stack pointer
    (int)&Reset_Handler,          /* Reset Handler                   */
    (int)&NMI_Handler,            /* NMI Handler                     */
    (int)&HardFault_Handler,      /* Hard Fault Handler              */
    (int)&MemManage_Handler,      /* The MPU fault handler           */
    (int)&BusFault_Handler,       /* The bus fault handler           */
    (int)&UsageFault_Handler,     /* The usage fault handler         */
    0,                            /* Reserved                        */
    0,                            /* Reserved                        */
    0,                            /* Reserved                        */
    0,                            /* Reserved                        */
    (int)&SVC_Handler,            /* SVCall handler                  */
    (int)&DebugMon_Handler,       /* Debug monitor handler           */
    0,                            /* Reserved                        */
    (int)&PendSV_Handler,         /* The PendSV handler              */
    (int)&SysTick_Handler,        /* The SysTick handler             */

    /*IRQ handlers... */
    (int)&WDT_IRQHandler,         /* WDT                              */
    (int)&TIMER0_IRQHandler,      /* TIMER0                           */
    (int)&TIMER1_IRQHandler,      /* TIMER1                           */
    (int)&TIMER2_IRQHandler,      /* TIMER2                           */
    (int)&TIMER3_IRQHandler,      /* TIMER3                           */
    (int)&UART0_IRQHandler,       /* UART0                            */
    (int)&UART1_IRQHandler,       /* UART1                            */
    (int)&UART2_IRQHandler,       /* UART2                            */
    (int)&UART3_IRQHandler,       /* UART3                            */
    (int)&PWM1_IRQHandler,        /* PWM1                             */
    (int)&I2C0_IRQHandler,        /* I2C0                             */
    (int)&I2C1_IRQHandler,        /* I2C1                             */
    (int)&I2C2_IRQHandler,        /* I2C2                             */
    (int)&SPI_IRQHandler,         /* SPI                              */
    (int)&SSP0_IRQHandler,        /* SSP0                             */
    (int)&SSP1_IRQHandler,        /* SSP1                             */
    (int)&PLL0_IRQHandler,        /* PLL0 (Main PLL)                  */
    (int)&RTC_IRQHandler,         /* RTC                              */
    (int)&EINT0_IRQHandler,       /* EINT0                            */
    (int)&EINT1_IRQHandler,       /* EINT1                            */
    (int)&EINT2_IRQHandler,       /* EINT2                            */
    (int)&EINT3_IRQHandler,       /* EINT3                            */
    (int)&ADC_IRQHandler,         /* ADC                              */
    (int)&BOD_IRQHandler,         /* BOD                              */
    (int)&USB_IRQHandler,         /* USB                              */
    (int)&CAN_IRQHandler,         /* CAN                              */
    (int)&DMA_IRQHandler,         /* GP DMA                           */
    (int)&I2S_IRQHandler,         /* I2S                              */
    (int)&ENET_IRQHandler,        /* Ethernet                         */
    (int)&RIT_IRQHandler,         /* RITINT                           */
    (int)&MCPWM_IRQHandler,       /* Motor Control PWM                */
    (int)&QEI_IRQHandler,         /* Quadrature Encoder               */
    (int)&PLL1_IRQHandler,        /* PLL1 (USB PLL)                   */
    (int)&USBActivity_IRQHandler, /* USB Activity interrupt to wakeup */
    (int)&CANActivity_IRQHandler, /* CAN Activity interrupt to wakeup */
};

// ----------------------------------------------------------------------------

// Processor ends up here if an unexpected interrupt occurs or a
// specific handler is not present in the application code.
// When in DEBUG, trigger a debug exception to clearly notify
// the user of the exception and help identify the cause.

void __attribute__ ((section(".after_vectors")))
Default_Handler(void)
{
#if defined(DEBUG)
__DEBUG_BKPT();
#endif
while (1)
  {
    ;
  }
}

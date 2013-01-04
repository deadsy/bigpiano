//-----------------------------------------------------------------------------
/*

ISR Entry Points

*/
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "timer.h"
#include "color.h"
#include "led.h"

//-----------------------------------------------------------------------------
// ISR Entry Points

ISR(USART_RX_vect) {
    uart_rx_isr();
}

ISR(USART_UDRE_vect) {
    uart_tx_isr();
}

ISR(TIMER0_OVF_vect) {
    led_isr();
}

ISR(TIMER1_OVF_vect) {
    timer_ovf_isr();
}

//-----------------------------------------------------------------------------

/*
 * usart.h
 *
 *  Created on: 2017/09/22
 *      Author: drassala
 */

#ifndef USART1_H_
#define USART1_H_

#include <stm32f4xx.h>
#include <misc.h>			 // I recommend you have a look at these in the ST firmware folder
#include <stm32f4xx_usart.h> // under Libraries/STM32F4xx_StdPeriph_Driver/inc and src
#include "string.h"

#define USART1_MAX_STRLEN 128 // this is the maximum string length of our string in characters
volatile char usart1_received_string[USART1_MAX_STRLEN+1]; // this will hold the recieved string

/* This funcion initializes the USART1 peripheral
 *
 * Arguments: baudrate --> the baudrate at which the USART is
 * 						   supposed to operate
 */
void init_USART1(uint32_t baudrate);

/* This function is used to transmit a string of characters via USART1
 *
 * It takes one argument: (volatile) char *s is the string you want to send
 *
 * Note: The string has to be passed to the function as a pointer because
 * 		 the compiler doesn't know the 'string' data type. In standard
 * 		 C a string is just an array of characters
 *
 * Note 2: At the moment it takes a volatile char because the received_string variable
 * 		   declared as volatile char --> otherwise the compiler will spit out warnings
 * */
void USART1_puts(volatile char *s);
void uart_debug_putc( unsigned char c);

#endif /* USART1_H_ */

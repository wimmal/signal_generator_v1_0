/*
 * sg_functions.h
 *
 *  Created on: 11 Dec 2015
 *      Author: wimmal
 */

#ifndef SG_FUNCTIONS_H_
#define SG_FUNCTIONS_H_

#include <stdint.h>




void set_dac(int dac, uint8_t data_high, uint8_t data_low);
void output(int dac, int amp, float freq, int function);
int read_buttons();
void set_gpios();
void set_leds();
void set_ss();
char * my_itoa(float input);
void sg_output_a();
void sg_output_b();
int sg_spi();
int sg_spi(unsigned short data);
void sg_spi_command();
void blink();
void exec_wave(int pru);



#endif /* SG_FUNCTIONS_H_ */

/*
 * sg_functions.cpp
 *
 *  Created on: 11 Dec 2015
 *      Author: wimmal
 */

#include "sg_functions.h"
#include "SimpleGPIO.h"
#include "BBB_I2C.h"
#include "sg_file.h"
#include "SPIDevice.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>

#include<sys/ioctl.h>
#include<linux/spi/spidev.h>

//#include "SPIDevice.h"

using namespace cacaosd_bbb_i2c;
using namespace std;
//using namespace exploringBB;

#define DEV_ADD_0 		0x62
#define DEV_ADD_1 		0x63
#define SEVEN_SEG_LCD_0 0x70
#define SEVEN_SEG_LCD_1 0x71

#define pb_function 	66
#define pb_freq_up 		67
#define pb_freq_down 	69
#define pb_pos_freq 	68
#define pb_unit_freq 	45
#define pb_amp_up 		44
#define pb_amp_down 	23
#define pb_pos_amp 		26
#define pb_unit_u 		47
#define pb_ch_sel 		46
#define pb_out_a 		27
#define pb_out_b 		65	//was 32

#define led_power 		22
#define led_func_sinus 	61
#define led_func_rect 	86
#define led_func_saw1 	88
#define led_func_saw2 	87
#define led_freq0 		89
#define led_freq1 		10
#define led_freq2 		11
#define led_freq3 		9
#define led_amp0 		81
#define led_amp1 		8
#define led_amp2 		80
#define led_amp3 		78
#define led_unit_hz 	79
#define led_unit_khz 	76
#define led_unit_mhz 	77
#define led_ch_a 		74
#define led_ch_b 		60
#define led_unit_v 		72
#define led_unit_mv 	31
#define led_output_a 	70
#define led_output_b 	71
//#define dds0_ctrl		30

#define DAC0 0
#define DAC1 1

#define zero 	0x3F
#define one 	0x06
#define two 	0x5B
#define three 	0x4F
#define four 	0x66
#define five 	0x6D
#define six 	0x7D
#define seven	0x07
#define eight 	0x7F
#define nine 	0x6F
#define dark 	0x00
//#define all 	0xFF

#define pru0	0
#define pru1	1

int function_type = 0;
float freq_A = 2000;
float freq_B = 1.000;
int freq_A_output = 1;
int freq_B_output = 1;
float amp_A = 1.000;
float amp_B = 1.000;
int amp_A_output = 0;
int amp_B_output = 0;
int unit_freq = 0;   // 0 = Hz, 1 = kHz, 2 = Mhz
int unit_amp = 0;   // 0 = V, 1 = mV
int out_a = 0;
int out_b = 0;
int ch_select = 0;   // 0 = Channel A, 1 = Channel B
float delay_a = 0.0;
float delay_b = 0.0;
int pressed_func = 0;
int pressed_freq_down = 0;
int pressed_freq_up = 0;
int pressed_pos_freq = 0;
int pressed_unit_freq = 0;
int pressed_amp_up = 0;
int pressed_amp_down = 0;
int pressed_pos_amp = 0;
int pressed_unit_amp = 0;
int pressed_ch_sel = 0;
int pressed_out_a = 0;
int pressed_out_b = 0;
int func_led_pos = led_func_sinus;
int freq_led_pos = led_freq0;
int freq_unit_led_pos = led_unit_mv;
int amp_led_pos = led_amp0;
int amp_unit_led_pos = led_unit_hz;
int ch_select_led_pos = led_ch_a;
int dez_pos_freq = 0;    // 0 = no decimal point, 1 = 1 decimal position, etc. max = 3
int dez_pos_amp = 0;
int pos_freq = 0;
int pos_amp = 0;
int out_a_start = 0;
int out_b_start = 0;
int dez_pos_check_freq = 0;
unsigned int value = LOW;
char output_string[5];
uint8_t data_low;
uint8_t data_high;

int sine[256]= {
		2048, 2098, 2148, 2198, 2248, 2298, 2348, 2398,
		2447, 2496, 2545, 2594, 2642, 2690, 2737, 2784,
		2831, 2877, 2923, 2968, 3013, 3057, 3100, 3143,
		3185, 3226, 3267, 3307, 3346, 3385, 3423, 3459,
		3495, 3530, 3565, 3598, 3630, 3662, 3692, 3722,
		3750, 3777, 3804, 3829, 3853, 3876, 3898, 3919,
		3939, 3958, 3975, 3992, 4007, 4021, 4034, 4045,
		4056, 4065, 4073, 4080, 4085, 4089, 4093, 4094,
		4095, 4094, 4093, 4089, 4085, 4080, 4073, 4065,
		4056, 4045, 4034, 4021, 4007, 3992, 3975, 3958,
		3939, 3919, 3898, 3876, 3853, 3829, 3804, 3777,
		3750, 3722, 3692, 3662, 3630, 3598, 3565, 3530,
		3495, 3459, 3423, 3385, 3346, 3307, 3267, 3226,
		3185, 3143, 3100, 3057, 3013, 2968, 2923, 2877,
		2831, 2784, 2737, 2690, 2642, 2594, 2545, 2496,
		2447, 2398, 2348, 2298, 2248, 2198, 2148, 2098,
		2048, 1997, 1947, 1897, 1847, 1797, 1747, 1697,
		1648, 1599, 1550, 1501, 1453, 1405, 1358, 1311,
		1264, 1218, 1172, 1127, 1082, 1038,  995,  952,
		910,  869,  828,  788,  749,  710,  672,  636,
		600,  565,  530,  497,  465,  433,  403,  373,
		345,  318,  291,  266,  242,  219,  197,  176,
		156,  137,  120,  103,   88,   74,   61,   50,
		39,   30,   22,   15,   10,    6,    2,    1,
		0,    1,    2,    6,   10,   15,   22,   30,
		39,   50,   61,   74,   88,  103,  120,  137,
		156,  176,  197,  219,  242,  266,  291,  318,
		345,  373,  403,  433,  465,  497,  530,  565,
		600,  636,  672,  710,  749,  788,  828,  869,
		910,  952,  995, 1038, 1082, 1127, 1172, 1218,
		1264, 1311, 1358, 1405, 1453, 1501, 1550, 1599,
		164328, 1697, 1747, 1797, 1847, 1897, 1947, 1997 };


void output(int dac, int amp, float freq, int function){

	int max_amp = 4095;
	float multiplicator = 1638;
	int calc_amp;
	float delay_c = 1/freq * 1000000;
	int delay = delay_c;

	calc_amp = amp * multiplicator;

	if (calc_amp > max_amp){
		calc_amp = max_amp;
	}

	if (function == 0){
		for (int i = 0; i < 256; i++) {

			data_high = ((sine[i] >> 8) & 0xFF);
			data_low = ((sine[i] >> 0) & 0xFF);
			set_dac(dac, data_high, data_low);
		}
	}
	else if (function == 1){

		data_high = ((calc_amp >> 8) & 0xFF);
		data_low = ((calc_amp >> 0) & 0xFF);

		set_dac(dac, data_high, data_low);
		usleep(delay/2);
		set_dac(dac, 0x00, 0x00);
		usleep(delay/2);
	}
	else if (function == 2){

		int amp_step = calc_amp /100;
		int amp_set = amp_step;

		while (amp_set < calc_amp){
			data_high = ((amp_set >> 8) & 0xFF);
			data_low = ((amp_set >> 0) & 0xFF);
			set_dac(dac, data_high, data_low);
			amp_set += amp_step;
			usleep(delay/100);
		}
	}
	else {
		int amp_step = calc_amp /100;
		int amp_set = calc_amp;

		while (amp_set > 0){
			data_high = ((amp_set >> 8) & 0xFF);
			data_low = ((amp_set >> 0) & 0xFF);
			set_dac(dac, data_high, data_low);
			amp_set -= amp_step;
			usleep(delay/100);
		}
	}
}

void set_dac(int dac, uint8_t data_high, uint8_t data_low){

	if (dac == 0){
		BBB_I2C dac0(DEV_ADD_0, I2C_BUS);
		dac0.writeByte(data_high, data_low);
	}
	else if (dac == 1){
		BBB_I2C dac1(DEV_ADD_1, I2C_BUS);
		dac1.writeByte(data_high, data_low);
	}
}

void set_gpios(){

	gpio_export(pb_function);    // Export the GPIO
	gpio_export(pb_freq_up);
	gpio_export(pb_freq_down);
	gpio_export(pb_pos_freq);
	gpio_export(pb_unit_freq);
	gpio_export(pb_amp_up);
	gpio_export(pb_amp_down);
	gpio_export(pb_pos_amp);
	gpio_export(pb_unit_u);
	gpio_export(pb_ch_sel);
	gpio_export(pb_out_a);
	gpio_export(pb_out_b);
	gpio_export(led_power);
	gpio_export(led_func_sinus);
	gpio_export(led_func_rect);
	gpio_export(led_func_saw1);
	gpio_export(led_func_saw2);
	gpio_export(led_freq0);
	gpio_export(led_freq1);
	gpio_export(led_freq2);
	gpio_export(led_freq3);
	gpio_export(led_amp0);
	gpio_export(led_amp1);
	gpio_export(led_amp2);
	gpio_export(led_amp3);
	gpio_export(led_unit_hz);
	gpio_export(led_unit_khz);
	gpio_export(led_unit_mhz);
	gpio_export(led_ch_a);
	gpio_export(led_ch_b);
	gpio_export(led_unit_v);
	gpio_export(led_unit_mv);
	gpio_export(led_output_a);
	gpio_export(led_output_b);
	//gpio_export(dds0_ctrl);

	gpio_set_dir(pb_function, INPUT_PIN);   // The push button is an input
	gpio_set_dir(pb_freq_up, INPUT_PIN);
	gpio_set_dir(pb_freq_down, INPUT_PIN);
	gpio_set_dir(pb_pos_freq, INPUT_PIN);
	gpio_set_dir(pb_unit_freq, INPUT_PIN);
	gpio_set_dir(pb_amp_up, INPUT_PIN);
	gpio_set_dir(pb_amp_down, INPUT_PIN);
	gpio_set_dir(pb_pos_amp, INPUT_PIN);
	gpio_set_dir(pb_unit_u, INPUT_PIN);
	gpio_set_dir(pb_ch_sel, INPUT_PIN);
	gpio_set_dir(pb_out_a, INPUT_PIN);
	gpio_set_dir(pb_out_b, INPUT_PIN);

	gpio_set_dir(led_power, OUTPUT_PIN);   // The LED is an output
	gpio_set_dir(led_func_sinus, OUTPUT_PIN);
	gpio_set_dir(led_func_rect, OUTPUT_PIN);
	gpio_set_dir(led_func_saw1, OUTPUT_PIN);
	gpio_set_dir(led_func_saw2, OUTPUT_PIN);
	gpio_set_dir(led_freq0, OUTPUT_PIN);
	gpio_set_dir(led_freq1, OUTPUT_PIN);
	gpio_set_dir(led_freq2, OUTPUT_PIN);
	gpio_set_dir(led_freq3, OUTPUT_PIN);
	gpio_set_dir(led_amp0, OUTPUT_PIN);
	gpio_set_dir(led_amp1, OUTPUT_PIN);
	gpio_set_dir(led_amp2, OUTPUT_PIN);
	gpio_set_dir(led_amp3, OUTPUT_PIN);
	gpio_set_dir(led_unit_hz, OUTPUT_PIN);
	gpio_set_dir(led_unit_khz, OUTPUT_PIN);
	gpio_set_dir(led_unit_mhz, OUTPUT_PIN);
	gpio_set_dir(led_ch_a, OUTPUT_PIN);
	gpio_set_dir(led_ch_b, OUTPUT_PIN);
	gpio_set_dir(led_unit_v, OUTPUT_PIN);
	gpio_set_dir(led_unit_mv, OUTPUT_PIN);
	gpio_set_dir(led_output_a, OUTPUT_PIN);
	gpio_set_dir(led_output_b, OUTPUT_PIN);
	//gpio_set_dir(dds0_ctrl, OUTPUT_PIN);
}

int read_buttons(){

	gpio_get_value(pb_function, &value);
	if (value == HIGH){
		if(pressed_func == 0){
			pressed_func = 1;
			//cout << "Function Select" << endl;
			function_type = function_type + 1;
			if( (function_type == 4)){
				function_type = 0;
			}
			gpio_set_value(func_led_pos, LOW);
			if(function_type == 0){
				func_led_pos = led_func_sinus;
				//cout << "Sinus" << endl;;
			}
			else if(function_type == 1){
				func_led_pos = led_func_rect;
				//cout << "Rechteck" << endl;;
			}
			else if(function_type == 2){
				func_led_pos = led_func_saw1;
				//cout << "Saegezahn" << endl;;
			}
			else if(function_type == 3){
				func_led_pos = led_func_saw2;
				//cout << "S2" << endl;;
			}
			gpio_set_value(func_led_pos, HIGH);
		}
	}
	else{
		pressed_func = 0;
	}

	gpio_get_value(pb_freq_up, &value);
	if (value == HIGH){
		if(pressed_freq_up == 0){
			pressed_freq_up = 1;

			if (freq_A < 10){
				dez_pos_freq = 0;
			}
			else if (freq_A < 100){
				dez_pos_freq = 1;
			}
			else {
				dez_pos_freq = 2;
			}

			if (freq_B < 10){
				dez_pos_freq = 0;
			}
			else if (freq_B < 100){
				dez_pos_freq = 1;
			}
			else {
				dez_pos_freq = 2;
			}

			if( ch_select == 0){
				if( pos_freq == 0){
					if(freq_A < 10){
						freq_A = freq_A + 1;
					}
					else if(freq_A < 100){
						freq_A = freq_A + 10;
					}
					else if(freq_A < 1000){
						freq_A = freq_A + 100;
					}
					else if(freq_A < 10000){
						freq_A = freq_A + 1000;
					}
				}
				else if( pos_freq == 1){
					if(freq_A < 10){
						freq_A = freq_A + 0.1;
					}
					else if(freq_A < 100){
						freq_A = freq_A + 1;
					}
					else if(freq_A < 1000){
						freq_A = freq_A + 10;
					}
					else if(freq_A < 10000){
						freq_A = freq_A + 100;
					}
				}
				else if( pos_freq == 2){
					if(freq_A < 10){
						freq_A = freq_A + 0.01;
					}
					else if(freq_A < 100){
						freq_A = freq_A + 0.1;
					}
					else if(freq_A < 1000){
						freq_A = freq_A + 1;
					}
					else if(freq_A < 10000){
						freq_A = freq_A + 10;
					}
				}
				else if( pos_freq == 3){
					if(freq_A < 10){
						freq_A = freq_A + 0.001;
					}
					else if(freq_A < 100){
						freq_A = freq_A + 0.01;
					}
					else if(freq_A < 1000){
						freq_A = freq_A + 0.1;
					}
					else if(freq_A < 10000){
						freq_A = freq_A + 1;			// todo  Maximale Frequenz herausfinden und als Maximum definieren
					}
				}
			}
			else if( ch_select == 1){
				if( pos_freq == 0){
					if(freq_B < 10){
						freq_B = freq_B + 1;
					}
					else if(freq_B < 100){
						freq_B = freq_B + 10;
					}
					else if(freq_B < 1000){
						freq_B = freq_B + 100;
					}
					else if(freq_B < 10000){
						freq_B = freq_B + 1000;
					}
				}
				else if( pos_freq == 1){
					if(freq_B < 10){
						freq_B = freq_B + 0.1;
					}
					else if(freq_B < 100){
						freq_B = freq_B + 1;
					}
					else if(freq_B < 1000){
						freq_B = freq_B + 10;
					}
					else if(freq_B < 10000){
						freq_B = freq_B + 100;
					}
				}
				else if( pos_freq == 2){
					if(freq_B < 10){
						freq_B = freq_B + 0.01;
					}
					else if(freq_B < 100){
						freq_B = freq_B + 0.1;
					}
					else if(freq_B < 1000){
						freq_B = freq_B + 1;
					}
					else if(freq_B < 10000){
						freq_B = freq_B + 10;
					}
				}
				else if( pos_freq == 3){
					if(freq_B < 10){
						freq_B = freq_B + 0.001;
					}
					else if(freq_B < 100){
						freq_B = freq_B + 0.01;
					}
					else if(freq_B < 1000){
						freq_B = freq_B + 0.1;
					}
					else if(freq_B < 10000){
						freq_B = freq_B + 1;
					}
				}
				//todo Maximale Frequenz herausfinden und als Maximum definieren
			}
			return 1;
		}
	}
	else{
		pressed_freq_up = 0;
	}

	gpio_get_value(pb_freq_down, &value);
	if (value == HIGH){
		if( pressed_freq_down == 0){
			pressed_freq_down = 1;
			if (freq_A < 10){
				dez_pos_freq = 0;
			}
			else if (freq_A < 100){
				dez_pos_freq = 1;
			}
			else {
				dez_pos_freq = 2;
			}

			if (freq_B < 10){
				dez_pos_freq = 0;
			}
			else if (freq_B < 100){
				dez_pos_freq = 1;
			}
			else {
				dez_pos_freq = 2;
			}

			if( ch_select == 0){
				if( pos_freq == 0){
					if(freq_A < 10){
						freq_A = freq_A - 1;
					}
					else if(freq_A < 100){
						freq_A = freq_A - 10;
					}
					else if(freq_A < 1000){
						freq_A = freq_A - 100;
					}
					else if(freq_A < 10000){
						freq_A = freq_A - 1000;
					}
				}
				else if( pos_freq == 1){
					if(freq_A < 10){
						freq_A = freq_A - 0.1;
					}
					else if(freq_A < 100){
						freq_A = freq_A - 1;
					}
					else if(freq_A < 1000){
						freq_A = freq_A - 10;
					}
					else if(freq_A < 10000){
						freq_A = freq_A - 100;
					}
				}
				else if( pos_freq == 2){
					if(freq_A < 10){
						freq_A = freq_A - 0.01;
					}
					else if(freq_A < 100){
						freq_A = freq_A - 0.1;
					}
					else if(freq_A < 1000){
						freq_A = freq_A - 1;
					}
					else if(freq_A < 10000){
						freq_A = freq_A - 10;
					}
				}
				else if( pos_freq == 3){
					if(freq_A < 10){
						freq_A = freq_A - 0.001;
					}
					else if(freq_A < 100){
						freq_A = freq_A - 0.01;
					}
					else if(freq_A < 1000){
						freq_A = freq_A - 0.1;
					}
					else if(freq_A < 10000){
						freq_A = freq_A - 1;
					}
				}
				if( freq_A < 0.001){
					freq_A = 0.001;
				}
			}
			else if( ch_select == 1){
				if( pos_freq == 0){
					if(freq_B < 10){
						freq_B = freq_B - 1;
					}
					else if(freq_B < 100){
						freq_B = freq_B - 10;
					}
					else if(freq_B < 1000){
						freq_B = freq_B - 100;
					}
					else if(freq_B < 10000){
						freq_B = freq_B - 1000;
					}
				}
				else if( pos_freq == 1){
					if(freq_B < 10){
						freq_B = freq_B - 0.1;
					}
					else if(freq_B < 100){
						freq_B = freq_B - 1;
					}
					else if(freq_B < 1000){
						freq_B = freq_B - 10;
					}
					else if(freq_B < 10000){
						freq_B = freq_B - 100;
					}
				}
				else if( pos_freq == 2){
					if(freq_B < 10){
						freq_B = freq_B - 0.01;
					}
					else if(freq_B < 100){
						freq_B = freq_B - 0.1;
					}
					else if(freq_B < 1000){
						freq_B = freq_B - 1;
					}
					else if(freq_B < 10000){
						freq_B = freq_B - 10;
					}
				}
				else if( pos_freq == 3){
					if(freq_B < 10){
						freq_B = freq_B - 0.001;
					}
					else if(freq_B < 100){
						freq_B = freq_B - 0.01;
					}
					else if(freq_B < 1000){
						freq_B = freq_B - 0.1;
					}
					else if(freq_B < 10000){
						freq_B = freq_B - 1;
					}
				}
				if( freq_B < 0.001){
					freq_B = 0.001;
				}
			}
			return 1;
		}
	}
	else{
		pressed_freq_down = 0;
	}

	gpio_get_value(pb_pos_freq, &value);
	if (value == HIGH){
		if( pressed_pos_freq == 0){
			pressed_pos_freq = 1;
			gpio_set_value(freq_led_pos, LOW);
			pos_freq ++;
			if( pos_freq == 4){
				pos_freq = 0;
			}
			if( pos_freq == 0){
				freq_led_pos = led_freq0;
			}
			else if( pos_freq == 1){
				freq_led_pos = led_freq1;
			}
			else if( pos_freq == 2){
				freq_led_pos = led_freq2;
			}
			else if( pos_freq == 3){
				freq_led_pos = led_freq3;
			}
			gpio_set_value(freq_led_pos, HIGH);
		}
	}
	else{
		pressed_pos_freq = 0;
	}

	gpio_get_value(pb_unit_freq, &value);
	if (value == HIGH){
		if( pressed_unit_freq == 0){
			pressed_unit_freq = 1;
			gpio_set_value(freq_unit_led_pos, LOW);
			unit_freq += 1;
			if( unit_freq == 3){
				unit_freq = 0;
			}
			if( unit_freq == 0){
				freq_unit_led_pos = led_unit_hz;
			}
			else if( unit_freq == 1){
				freq_unit_led_pos = led_unit_khz;
			}
			else if( unit_freq == 2){
				freq_unit_led_pos = led_unit_mhz;
			}
			gpio_set_value(freq_unit_led_pos, HIGH);
		}
	}
	else {
		pressed_unit_freq = 0;
	}

	gpio_get_value(pb_amp_up, &value);
	if (value == HIGH){
		if( pressed_amp_up == 0){
			pressed_amp_up = 1;
			if( ch_select == 0){
				if( pos_amp == 0){
					if( dez_pos_amp == 0){
						amp_A = amp_A + 1;
					}
					else if( dez_pos_amp == 1){
						amp_A = amp_A + 0.1;
					}
					else if( dez_pos_amp == 2){
						amp_A = amp_A + 0.01;
					}
					else if( dez_pos_amp == 3){
						amp_A = amp_A + 0.001;
					}
				}
				else if( pos_amp == 1){
					if( dez_pos_amp == 0){
						amp_A = amp_A + 10;
					}
					else if( dez_pos_amp == 1){
						amp_A = amp_A + 1;
					}
					else if( dez_pos_amp == 2){
						amp_A = amp_A + 0.1;
					}
					else if( dez_pos_amp == 3){
						amp_A = amp_A + 0.01;
					}
				}
				else if( pos_amp == 2){
					if( dez_pos_amp == 0){
						amp_A = amp_A + 100;
					}
					else if( dez_pos_amp == 1){
						amp_A = amp_A + 10;
					}
					else if( dez_pos_amp == 2){
						amp_A = amp_A + 1;
					}
					else if( dez_pos_amp == 3){
						amp_A = amp_A + 0.1;
					}
				}
				else if( pos_amp == 3){
					if( dez_pos_amp == 0){
						amp_A = amp_A + 1000;
					}
					else if( dez_pos_amp == 1){
						amp_A = amp_A + 100;
					}
					else if( dez_pos_amp == 2){
						amp_A = amp_A + 10;
					}
					else if( dez_pos_amp == 3){
						amp_A = amp_A + 1;
					}
					// todo Maximale Amplitude herausfinden und als Maximum definieren
				}
			}
			else if( ch_select == 1){
				if( pos_amp == 0){
					if( dez_pos_amp == 0){
						amp_B = amp_B + 1;
					}
					else if( dez_pos_amp == 1){
						amp_B = amp_B + 0.1;
					}
					else if( dez_pos_amp == 2){
						amp_B = amp_B + 0.01;
					}
					else if( dez_pos_amp == 3){
						amp_B = amp_B + 0.001;
					}
				}
				else if( pos_amp == 1){
					if( dez_pos_amp == 0){
						amp_B = amp_B + 10;
					}
					else if( dez_pos_amp == 1){
						amp_B = amp_B + 1;
					}
					else if( dez_pos_amp == 2){
						amp_B = amp_B + 0.1;
					}
					else if( dez_pos_amp == 3){
						amp_B = amp_B + 0.01;
					}
				}
				else if( pos_amp == 2){
					if( dez_pos_amp == 0){
						amp_B = amp_B + 100;
					}
					else if( dez_pos_amp == 1){
						amp_B = amp_B + 10;
					}
					else if( dez_pos_amp == 2){
						amp_B = amp_B + 1;
					}
					else if( dez_pos_amp == 3){
						amp_B = amp_B + 0.1;
					}
				}
				else if( pos_amp == 3){
					if( dez_pos_amp == 0){
						amp_B = amp_B + 1000;
					}
					else if( dez_pos_amp == 1){
						amp_B = amp_B + 100;
					}
					else if( dez_pos_amp == 2){
						amp_B = amp_B + 10;
					}
					else if( dez_pos_amp == 3){
						amp_B = amp_B + 1;
					}
				}
			}
			//todo ){ Maximale Amplitude herausfinden und als Maximum definieren derzeit 2,5V
			return 1;
		}
	}
	else{
		pressed_amp_up = 0;
	}

	gpio_get_value(pb_amp_down, &value);
	if (value == HIGH){
		if( pressed_amp_down == 0){
			pressed_amp_down = 1;
			if( ch_select == 0){
				if( pos_amp == 0){
					if( dez_pos_amp == 0){
						amp_A = amp_A - 1;
					}
					else if( dez_pos_amp == 1){
						amp_A = amp_A - 0.1;
					}
					else if( dez_pos_amp == 2){
						amp_A = amp_A - 0.01;
					}
					else if( dez_pos_amp == 3){
						amp_A = amp_A - 0.001;
					}
				}
				else if( pos_amp == 1){
					if( dez_pos_amp == 0){
						amp_A = amp_A - 10;
					}
					else if( dez_pos_amp == 1){
						amp_A = amp_A - 1;
					}
					else if( dez_pos_amp == 2){
						amp_A = amp_A - 0.1;
					}
					else if( dez_pos_amp == 3){
						amp_A = amp_A - 0.01;
					}
				}
				else if( pos_amp == 2){
					if( dez_pos_amp == 0){
						amp_A = amp_A - 100;
					}
					else if( dez_pos_amp == 1){
						amp_A = amp_A - 10;
					}
					else if( dez_pos_amp == 2){
						amp_A = amp_A - 1;
					}
					else if( dez_pos_amp == 3){
						amp_A = amp_A - 0.1;
					}
				}
				else if( pos_amp == 3){
					if( dez_pos_amp == 0){
						amp_A = amp_A - 1000;
					}
					else if( dez_pos_amp == 1){
						amp_A = amp_A - 100;
					}
					else if( dez_pos_amp == 2){
						amp_A = amp_A - 10;
					}
					else if( dez_pos_amp == 3){
						amp_A = amp_A - 1;
					}
				}
				if( amp_A < 0){
					amp_A = 0;
				}
			}
			else if( ch_select == 1){
				if( pos_amp == 0){
					if( dez_pos_amp == 0){
						amp_B = amp_B - 1;
					}
					else if( dez_pos_amp == 1){
						amp_B = amp_B - 0.1;
					}
					else if( dez_pos_amp == 2){
						amp_B = amp_B - 0.01;
					}
					else if( dez_pos_amp == 3){
						amp_B = amp_B - 0.001;
					}
				}
				else if( pos_amp == 1){
					if( dez_pos_amp == 0){
						amp_B = amp_B - 10;
					}
					else if( dez_pos_amp == 1){
						amp_B = amp_B - 1;
					}
					else if( dez_pos_amp == 2){
						amp_B = amp_B - 0.1;
					}
					else if( dez_pos_amp == 3){
						amp_B = amp_B - 0.01;
					}
				}
				else if( pos_amp == 2){
					if( dez_pos_amp == 0){
						amp_B = amp_B - 100;
					}
					else if( dez_pos_amp == 1){
						amp_B = amp_B - 10;
					}
					else if( dez_pos_amp == 2){
						amp_B = amp_B - 1;
					}
					else if( dez_pos_amp == 3){
						amp_B = amp_B - 0.1;
					}
				}
				else if( pos_amp == 3){
					if( dez_pos_amp == 0){
						amp_B = amp_B - 1000;
					}
					else if( dez_pos_amp == 1){
						amp_B = amp_B - 100;
					}
					else if( dez_pos_amp == 2){
						amp_B = amp_B - 10;
					}
					else if( dez_pos_amp == 3){
						amp_B = amp_B - 1;
					}
				}
				if( amp_B < 0){
					amp_B = 0;
				}
			}
			return 1;
		}
	}
	else{
		pressed_amp_down = 0;
	}

	gpio_get_value(pb_pos_amp, &value);
	if (value == HIGH){
		if( pressed_pos_amp == 0){
			pressed_pos_amp = 1;
			gpio_set_value(amp_led_pos, LOW);
			pos_amp ++;
			if( pos_amp == 4){
				pos_amp = 0;
			}
			if( pos_amp == 0){
				amp_led_pos = led_amp0;
			}
			else if( pos_amp == 1){
				amp_led_pos = led_amp1;
			}
			else if( pos_amp == 2){
				amp_led_pos = led_amp2;
			}
			else if( pos_amp == 3){
				amp_led_pos = led_amp3;
			}
			gpio_set_value(amp_led_pos, HIGH);
		}
	}
	else{
		pressed_pos_amp = 0;
	}

	gpio_get_value(pb_unit_u, &value);
	if (value == HIGH){
		if( pressed_unit_amp == 0){
			pressed_unit_amp = 1;
			gpio_set_value(amp_unit_led_pos, LOW);
			unit_amp ++;
			if( unit_amp == 2){
				unit_amp = 0;
			}
			if( unit_amp == 0){
				amp_unit_led_pos = led_unit_v;
			}
			else if( unit_amp == 1){
				amp_unit_led_pos = led_unit_mv;
			}
			gpio_set_value(amp_unit_led_pos, HIGH);
		}
	}
	else{
		pressed_unit_amp = 0;
	}

	gpio_get_value(pb_ch_sel, &value);
	if (value == HIGH){
		if( pressed_ch_sel == 0){
			pressed_ch_sel = 1;
			gpio_set_value(ch_select_led_pos, LOW);
			ch_select ++;
			if( ch_select == 2){
				ch_select = 0;
			}
			if( ch_select == 0){
				ch_select_led_pos = led_ch_a;
			}
			else if( ch_select == 1){
				ch_select_led_pos = led_ch_b;
			}
			gpio_set_value(ch_select_led_pos, HIGH);
			return 1;
		}
	}
	else{
		pressed_ch_sel = 0;
	}

	gpio_get_value(pb_out_a, &value);
	if (value == HIGH){
		if( pressed_out_a == 0){
			pressed_out_a = 1;
			if( out_a == 0){
				out_a = 1;
				out_a_start = 1;
				gpio_set_value(led_output_a, HIGH);
			}
			else if( out_a == 1){
				gpio_set_value(led_output_a, LOW);
				out_a = 0;
				out_a_start = 0;
			}
		}
	}
	else{
		pressed_out_a = 0;
	}

	gpio_get_value(pb_out_b, &value);
	if (value == HIGH){
		if( pressed_out_b == 0){
			pressed_out_b = 1;
			if( out_b == 0){
				out_b = 1;
				out_b_start = 1;
				gpio_set_value(led_output_b, HIGH);
			}
			else if( out_b == 1){
				gpio_set_value(led_output_b, LOW);
				out_b = 0;
				out_b_start = 0;
			}
		}
	}
	else{
		pressed_out_b = 0;
	}
	return 0;
}

void set_leds(){
	gpio_set_value(led_power, HIGH);
	gpio_set_value(led_func_sinus, HIGH);
	gpio_set_value(led_func_rect, LOW);
	gpio_set_value(led_func_saw1, LOW);
	gpio_set_value(led_func_saw2, LOW);
	gpio_set_value(led_freq0, HIGH);
	gpio_set_value(led_freq1, LOW);
	gpio_set_value(led_freq2, LOW);
	gpio_set_value(led_freq3, LOW);
	gpio_set_value(led_amp0, HIGH);
	gpio_set_value(led_amp1, LOW);
	gpio_set_value(led_amp2, LOW);
	gpio_set_value(led_amp3, LOW);
	gpio_set_value(led_unit_hz, HIGH);
	gpio_set_value(led_unit_khz, LOW);
	gpio_set_value(led_unit_mhz, LOW);
	gpio_set_value(led_ch_a, HIGH);
	gpio_set_value(led_ch_b, LOW);
	gpio_set_value(led_unit_v, LOW);
	gpio_set_value(led_unit_mv, HIGH);
	gpio_set_value(led_output_a, LOW);
	gpio_set_value(led_output_b, LOW);
}

void set_ss(){

	char ss_out0[4];
	int dez_pos0;
	char ss_out1[4];
	int dez_pos1;
	float freq_out;
	float amp_out;
	unsigned char single = 0xFF;

	if (ch_select == 0){
		freq_out = freq_A;
		amp_out = amp_A;
	}
	else{
		freq_out = freq_B;
		amp_out = amp_B;
	}

	const char *ss_val0 = my_itoa(amp_out);

	ss_out0[0] = ss_val0[0];
	ss_out0[1] = ss_val0[1];
	ss_out0[2] = ss_val0[2];
	ss_out0[3] = ss_val0[3];
	dez_pos0 = ss_val0[4];

	for (int i = 0; i < 4; i++){
		if (ss_out0[i] == 0){
			ss_out0[i] = zero;
		}
		else if (ss_out0[i] == 1){
			ss_out0[i] = one;
		}
		else if (ss_out0[i] == 2){
			ss_out0[i] = two;
		}
		else if (ss_out0[i] == 3){
			ss_out0[i] = three;
		}
		else if (ss_out0[i] == 4){
			ss_out0[i] = four;
		}
		else if (ss_out0[i] == 5){
			ss_out0[i] = five;
		}
		else if (ss_out0[i] == 6){
			ss_out0[i] = six;
		}
		else if (ss_out0[i] == 7){
			ss_out0[i] = seven;
		}
		else if (ss_out0[i] == 8){
			ss_out0[i] = eight;
		}
		else if (ss_out0[i] == 9){
			ss_out0[i] = nine;
		}
	}

	if (dez_pos0 == 0){
		ss_out0[0] = ss_out0[0] + 0x80;
	}
	else if (dez_pos0 == 1){
		ss_out0[1] = ss_out0[1] + 0x80;
	}
	else if (dez_pos0 == 2){
		ss_out0[2] = ss_out0[2] + 0x80;
	}

	BBB_I2C ss0(SEVEN_SEG_LCD_0, I2C_BUS);

	//start oszillators
	ss0.writeByte(single, 0x21);
	//displays on
	ss0.writeByte(single, 0x81);
	//set brightness
	ss0.writeByte(single, 0xEF);
	//write to sevensegment
	ss0.writeByte(0x00, ss_out0[0]);
	ss0.writeByte(0x02, ss_out0[1]);
	ss0.writeByte(0x04, dark);
	ss0.writeByte(0x06, ss_out0[2]);
	ss0.writeByte(0x08, ss_out0[3]);

	const char *ss_val1 = my_itoa(freq_out);

	ss_out1[0] = ss_val1[0];
	ss_out1[1] = ss_val1[1];
	ss_out1[2] = ss_val1[2];
	ss_out1[3] = ss_val1[3];
	dez_pos1 = ss_val1[4];

	for (int i = 0; i < 4; i++){
		if (ss_out1[i] == 0){
			ss_out1[i] = zero;
		}
		else if (ss_out1[i] == 1){
			ss_out1[i] = one;
		}
		else if (ss_out1[i] == 2){
			ss_out1[i] = two;
		}
		else if (ss_out1[i] == 3){
			ss_out1[i] = three;
		}
		else if (ss_out1[i] == 4){
			ss_out1[i] = four;
		}
		else if (ss_out1[i] == 5){
			ss_out1[i] = five;
		}
		else if (ss_out1[i] == 6){
			ss_out1[i] = six;
		}
		else if (ss_out1[i] == 7){
			ss_out1[i] = seven;
		}
		else if (ss_out1[i] == 8){
			ss_out1[i] = eight;
		}
		else if (ss_out1[i] == 9){
			ss_out1[i] = nine;
		}
	}
	if (dez_pos1 == 0){
		ss_out1[0] = ss_out1[0] + 0x80;
	}
	else if (dez_pos1 == 1){
		ss_out1[1] = ss_out1[1] + 0x80;
	}
	else if (dez_pos1 == 2){
		ss_out1[2] = ss_out1[2] + 0x80;
	}

	BBB_I2C ss1(SEVEN_SEG_LCD_1, I2C_BUS);

	ss1.writeByte(single, 0x21);
	ss1.writeByte(single, 0x81);
	ss1.writeByte(single, 0xEF);

	ss1.writeByte(0x00, ss_out1[0]);
	ss1.writeByte(0x02, ss_out1[1]);
	ss1.writeByte(0x04, dark);
	ss1.writeByte(0x06, ss_out1[2]);
	ss1.writeByte(0x08, ss_out1[3]);

}

char * my_itoa(float input){

	char convert[7];
	int los;
	int dez_pos;
	int convert_from = input * 1000;

	if (convert_from < 10){
		los = 1;
		dez_pos = 0;
	}
	else if (convert_from < 100){
		los = 2;
		dez_pos = 0;
	}
	else if (convert_from < 1000){
		los = 3;
		dez_pos = 0;
	}
	else if (convert_from < 10000){
		los = 4;
		dez_pos = 0;
	}
	else if (convert_from < 100000){
		los = 5;
		dez_pos = 1;
	}
	else if (convert_from < 1000000){
		los = 6;
		dez_pos = 2;
	}
	else{
		los = 7;
		dez_pos = 3;
	}

	convert[0] = convert_from % 10;
	convert_from = convert_from / 10;
	convert[1] = convert_from % 10;
	convert_from = convert_from / 10;
	convert[2] = convert_from % 10;
	convert_from = convert_from / 10;
	convert[3] = convert_from % 10;
	convert_from = convert_from / 10;
	convert[4] = convert_from % 10;
	convert_from = convert_from / 10;
	convert[5] = convert_from % 10;
	convert_from = convert_from / 10;
	convert[6] = convert_from % 10;
	convert_from = convert_from / 10;

	if (dez_pos == 0){
		output_string[0] = convert[3];
		output_string[1] = convert[2];
		output_string[2] = convert[1];
		output_string[3] = convert[0];
		output_string[4] = 0x0;
	}
	else if (dez_pos == 1){
		output_string[0] = convert[4];
		output_string[1] = convert[3];
		output_string[2] = convert[2];
		output_string[3] = convert[1];
		output_string[4] = 0x1;
	}
	else if (dez_pos == 2){
		output_string[0] = convert[5];
		output_string[1] = convert[4];
		output_string[2] = convert[3];
		output_string[3] = convert[2];
		output_string[4] = 0x2;
	}
	else{
		output_string[0] = convert[6];
		output_string[1] = convert[5];
		output_string[2] = convert[4];
		output_string[3] = convert[3];
		output_string[4] = 0x3;
	}
	return output_string;
}

void sg_output_a(){

	create_File(freq_A, amp_A, function_type, pru0);
	exec_wave(pru0);
}

void sg_output_b(){

	create_File(freq_A, amp_A, function_type, pru1);
	exec_wave(pru1);
}


void exec_wave(int pru){

	char command[50];

	if(pru == 0){
		strcpy( command, "cd /etc/pwm; ./build" );
		system(command);
		strcpy( command, "cd /etc/pwm; ./pwm &" );
		system(command);
	}
	else{
		strcpy( command, "cd /etc/pwm; ./build1" );
		system(command);
		strcpy( command, "cd /etc/pwm; ./pwm1 &" );
		system(command);
	}
}

/*EOF*/

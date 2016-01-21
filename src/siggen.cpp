//============================================================================
// Name        : siggen.cpp
// Author      : Michael Weber
// Version     : 0.2
// Copyright   : Your copyright notice
// Description : main file for the signal generator
//============================================================================

#include "SimpleGPIO.h"
#include "BBB_I2C.h"
#include "sg_functions.h"
#include "spi595.h"
#include "SPIDevice.h"
#include "sg_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

using namespace std;

extern int out_a;
extern int out_b;
bool pressed = 0;
bool run_a = true;
bool run_b = true;

int main (void)
{

	set_gpios();	//set GPIOs as input or output
	set_leds();		//set wheter a LED should light up initialy or not
	set_ss();		//set the seven-segment displays with the default values

	while(1){

		pressed = read_buttons();	//read buttons and return true if a button was pressed

		if(pressed){ 	//if a button was pressed, the seven-segment 
			set_ss();	//displays are updated with the new values
		}

		if (out_a){		//if button Output A was pressed the PRU will be programmed
			if (run_a){	//and Channel A starts to output the desired waveform
				sg_output_a();
				out_a = 0;
				run_a = false;
			}
			else{
				run_a = true;
				out_a = 0;
			}
		}

		if (out_b){
			if (run_b){
				sg_output_b();
				out_b = 0;
				run_b = false;
			}
			else{
				run_b = true;
				out_b = 0;
			}
		}
	}

	return 0;

}
/*EOF*/

/*
_file.cpp
 *
 *  Created on: 6 Jan 2016
 *      Author: wimmal
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

void create_File(float freq, float amp, int function, int pru){


	if(pru == 0){		//################## PRU0 ###################

		float calc_delay;
		float calc_amp;

		calc_delay = (1/freq) * 400000;
		calc_amp = amp * 100 / 6;

		FILE *fp;

		fp = fopen("/etc/pwm/pwm.c", "w+");

		fprintf(fp, "#include <stdio.h>\n");
		fprintf(fp, "#include <stdlib.h>\n");
		fprintf(fp, "#include <prussdrv.h>\n");
		fprintf(fp, "#include <pruss_intc_mapping.h>\n");
		fprintf(fp, "#include <math.h>\n");

		fprintf(fp, "#define PRU_NUM 	0\n");
		fprintf(fp, "static void *pru0DataMemory;\n");
		fprintf(fp, "static unsigned int *pru0DataMemory_int;\n");

		fprintf(fp, "int main (void)\n");
		fprintf(fp, "{\n");
		fprintf(fp, "if(getuid()!=0){\n");
		fprintf(fp, "exit(EXIT_FAILURE);\n");
		fprintf(fp, "}\n");
		fprintf(fp, "int n, i;\n");
		// Initialize structure used by prussdrv_pruintc_intc
		// PRUSS_INTC_INITDATA is found in pruss_intc_mapping.h
		fprintf(fp, "tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;\n");

		// Allocate and initialize memory
		fprintf(fp, "prussdrv_init ();\n");
		fprintf(fp, "prussdrv_open (PRU_EVTOUT_0);\n");

		// Map PRU's INTC
		fprintf(fp, "prussdrv_pruintc_init(&pruss_intc_initdata);\n");

		// Generate a periodic wave in an array of 100 values - using ints

		fprintf(fp, "float gain = %' .1f;\n", calc_amp);
		fprintf(fp, "float phase = 0.0f;\n");
		fprintf(fp, "float bias = 10.0f;\n");
		fprintf(fp, "float freq = 2.0f * 3.14159f / %' .1f;\n", calc_delay);
		switch(function){

		case 0	:	fprintf(fp, "unsigned int waveform[100];\n");
		fprintf(fp, "for (i=0; i<100; i++){\n");
		fprintf(fp, "waveform[i] = (unsigned int)(bias + (gain * sin((i * freq) + phase)));\n");
		fprintf(fp, "}\n");
		break;

		case 1	:	fprintf(fp, "unsigned int waveform[4];\n");
		fprintf(fp, "waveform[0] = 0;\n");
		fprintf(fp, "waveform[1] = gain;\n");
		fprintf(fp, "waveform[2] = 0;\n");
		fprintf(fp, "waveform[3] = gain;\n");
		break;

		case 2	:	fprintf(fp, "unsigned int waveform[100];\n");
		fprintf(fp, "float step = gain / 100;\n");
		fprintf(fp, "for (i=0; i<100; i++){\n");
		fprintf(fp, "waveform[i] = (unsigned int)(bias + (step * i));\n");
		fprintf(fp, "}\n");
		break;

		case 3	:	fprintf(fp, "unsigned int waveform[100];\n");
		fprintf(fp, "float step = gain / 100;\n");
		fprintf(fp, "for (i=0; i<100; i++){\n");
		fprintf(fp, "waveform[i] = (unsigned int)(bias + (gain -(step * i)));\n");
		fprintf(fp, "}\n");
		break;

		default	:	fprintf(fp, "unsigned int waveform[100];\n");
		fprintf(fp, "for (i=0; i<100; i++){\n");
		fprintf(fp, "waveform[i] = (unsigned int)(bias + (gain * sin((i * freq) + phase)));\n");
		fprintf(fp, "}\n");
		break;

		}
		// place the samples in memory
		fprintf(fp, "prussdrv_map_prumem(PRUSS0_PRU0_DATARAM, &pru0DataMemory);\n");
		fprintf(fp, "pru0DataMemory_int = (unsigned int *) pru0DataMemory;\n");
		fprintf(fp, "unsigned int sampletimestep = 1;\n");  //delay factor
		fprintf(fp, "*(pru0DataMemory_int) = sampletimestep;\n");
		if (function == 1){
			fprintf(fp, "unsigned int numbersamples = 2;\n");  //number of samples
		}
		else{
			fprintf(fp, "unsigned int numbersamples = 100;\n");  //number of samples
		}
		fprintf(fp, "*(pru0DataMemory_int+1) = numbersamples;\n");
		// copy the waveform data into PRU memory
		fprintf(fp, "for (i=0; i<numbersamples; i++){\n");
		fprintf(fp, "*(pru0DataMemory_int+2+i) = waveform[i];\n");
		fprintf(fp, "}\n");

		// Load and execute binary on PRU
		fprintf(fp, "prussdrv_exec_program (PRU_NUM, \"./pwm.bin\");\n");

		// Wait for event completion from PRU
		fprintf(fp, "n = prussdrv_pru_wait_event (PRU_EVTOUT_0);\n");  // This assumes the PRU generates an interrupt
		// connected to event out 0 immediately before halting

		// Disable PRU and close memory mappings
		fprintf(fp, "prussdrv_pru_disable(PRU_NUM);\n");
		fprintf(fp, "prussdrv_exit ();\n");
		fprintf(fp, "return EXIT_SUCCESS;\n");
		fprintf(fp, "}\n");

		fclose(fp);

	}
	else{			//################## PRU1 ###################

		float calc_delay;
		float calc_amp;

		calc_delay = (1/freq) * 400000;
		calc_amp = amp * 100 / 6;

		FILE *fp;

		fp = fopen("/etc/pwm/pwm1.c", "w+");

		fprintf(fp, "#include <stdio.h>\n");
		fprintf(fp, "#include <stdlib.h>\n");
		fprintf(fp, "#include <prussdrv.h>\n");
		fprintf(fp, "#include <pruss_intc_mapping.h>\n");
		fprintf(fp, "#include <math.h>\n");

		fprintf(fp, "#define PRU_NUM 	1\n");
		fprintf(fp, "static void *pru1DataMemory;\n");
		fprintf(fp, "static unsigned int *pru1DataMemory_int;\n");

		fprintf(fp, "int main (void)\n");
		fprintf(fp, "{\n");
		fprintf(fp, "if(getuid()!=0){\n");
		fprintf(fp, "exit(EXIT_FAILURE);\n");
		fprintf(fp, "}\n");
		fprintf(fp, "int n, i;\n");
		// Initialize structure used by prussdrv_pruintc_intc
		// PRUSS_INTC_INITDATA is found in pruss_intc_mapping.h
		fprintf(fp, "tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;\n");

		// Allocate and initialize memory
		fprintf(fp, "prussdrv_init ();\n");
		fprintf(fp, "prussdrv_open (PRU_EVTOUT_0);\n");

		// Map PRU's INTC
		fprintf(fp, "prussdrv_pruintc_init(&pruss_intc_initdata);\n");

		// Generate a periodic sine wave in an array of 100 values - using ints

		fprintf(fp, "float gain = %' .1f;\n", calc_amp);
		fprintf(fp, "float phase = 0.0f;\n");
		fprintf(fp, "float bias = 10.0f;\n");
		fprintf(fp, "float freq = 2.0f * 3.14159f / %' .1f;\n", calc_delay);
		switch(function){

		case 0	:	fprintf(fp, "unsigned int waveform[100];\n");
		fprintf(fp, "for (i=0; i<100; i++){\n");
		fprintf(fp, "waveform[i] = (unsigned int)(bias + (gain * sin((i * freq) + phase)));\n");
		fprintf(fp, "}\n");
		break;

		case 1	:	fprintf(fp, "unsigned int waveform[4];\n");
		fprintf(fp, "waveform[0] = 0;\n");
		fprintf(fp, "waveform[1] = gain;\n");
		fprintf(fp, "waveform[2] = 0;\n");
		fprintf(fp, "waveform[3] = gain;\n");
		break;

		case 2	:	fprintf(fp, "unsigned int waveform[100];\n");
		fprintf(fp, "float step = gain / 100;\n");
		fprintf(fp, "for (i=0; i<100; i++){\n");
		fprintf(fp, "waveform[i] = (unsigned int)(bias + (step * i));\n");
		fprintf(fp, "}\n");
		break;

		case 3	:	fprintf(fp, "unsigned int waveform[100];\n");
		fprintf(fp, "float step = gain / 100;\n");
		fprintf(fp, "for (i=0; i<100; i++){\n");
		fprintf(fp, "waveform[i] = (unsigned int)(bias + (gain -(step * i)));\n");
		fprintf(fp, "}\n");
		break;

		default	:	fprintf(fp, "unsigned int waveform[100];\n");
		fprintf(fp, "for (i=0; i<100; i++){\n");
		fprintf(fp, "waveform[i] = (unsigned int)(bias + (gain * sin((i * freq) + phase)));\n");
		fprintf(fp, "}\n");
		break;

		}
		// place the samples in memory
		fprintf(fp, "prussdrv_map_prumem(PRUSS0_PRU1_DATARAM, &pru1DataMemory);\n");
		fprintf(fp, "pru1DataMemory_int = (unsigned int *) pru1DataMemory;\n");
		fprintf(fp, "unsigned int sampletimestep = 1;\n");  //delay factor
		fprintf(fp, "*(pru1DataMemory_int) = sampletimestep;\n");
		if (function == 1){
			fprintf(fp, "unsigned int numbersamples = 2;\n");  //number of samples
		}
		else{
			fprintf(fp, "unsigned int numbersamples = 100;\n");  //number of samples
		}
		fprintf(fp, "*(pru1DataMemory_int+1) = numbersamples;\n");
		// copy the waveform data into PRU memory
		fprintf(fp, "for (i=0; i<numbersamples; i++){\n");
		fprintf(fp, "*(pru1DataMemory_int+2+i) = waveform[i];\n");
		fprintf(fp, "}\n");

		// Load and execute binary on PRU
		fprintf(fp, "prussdrv_exec_program (PRU_NUM, \"./pwm.bin\");\n");

		// Wait for event completion from PRU
		fprintf(fp, "n = prussdrv_pru_wait_event (PRU_EVTOUT_0);\n");  // This assumes the PRU generates an interrupt
		// connected to event out 0 immediately before halting

		// Disable PRU and close memory mappings
		fprintf(fp, "prussdrv_pru_disable(PRU_NUM);\n");
		fprintf(fp, "prussdrv_exit ();\n");
		fprintf(fp, "return EXIT_SUCCESS;\n");
		fprintf(fp, "}\n");

		fclose(fp);

	}




}


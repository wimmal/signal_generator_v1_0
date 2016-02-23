#include <stdio.h>
#include <stdlib.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>
#include <math.h>
#define PRU_NUM 	1
static void *pru1DataMemory;
static unsigned int *pru1DataMemory_int;
int main (void)
{
if(getuid()!=0){
exit(EXIT_FAILURE);
}
int n, i;
tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
prussdrv_init ();
prussdrv_open (PRU_EVTOUT_0);
prussdrv_pruintc_init(&pruss_intc_initdata);
float gain =  16.7;
float phase = 0.0f;
float bias = 10.0f;
float freq = 2.0f * 3.14159f /  400000.0;
unsigned int waveform[100];
for (i=0; i<100; i++){
waveform[i] = (unsigned int)(bias + (gain * sin((i * freq) + phase)));
}
prussdrv_map_prumem(PRUSS0_PRU1_DATARAM, &pru1DataMemory);
pru1DataMemory_int = (unsigned int *) pru1DataMemory;
unsigned int sampletimestep = 1;
*(pru1DataMemory_int) = sampletimestep;
unsigned int numbersamples = 100;
*(pru1DataMemory_int+1) = numbersamples;
for (i=0; i<numbersamples; i++){
*(pru1DataMemory_int+2+i) = waveform[i];
}
prussdrv_exec_program (PRU_NUM, "./pwm.bin");
n = prussdrv_pru_wait_event (PRU_EVTOUT_0);
prussdrv_pru_disable(PRU_NUM);
prussdrv_exit ();
return EXIT_SUCCESS;
}

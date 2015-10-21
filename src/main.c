#include "..\h\p33FJ256GP506.h"
#include "..\h\WM8510CodecDrv.h"
#include "..\h\sask.h"
#include "..\h\SFMDrv.h"
#include "..\h\G711.h"

_FGS(GWRP_OFF & GCP_OFF);
_FOSCSEL(FNOSC_FRC);
_FOSC(FCKSM_CSECMD & OSCIOFNC_ON & POSCMD_NONE);
_FWDT(FWDTEN_OFF);

/* FRAME_SIZE - Size of each audio frame
 * SPEECH_SEGMENT_SIZE - Size of intro speech segment
 * WRITE_START_ADDRESS - Serial Flash Memory write address
 * */

#define FRAME_SIZE              128
#define SPEECH_SEGMENT_SIZE     98049L
#define WRITE_START_ADDRESS 0x20000
#define PI                      3.1416

/* Allocate memory for buffers and drivers
 * codecBuffer - Buffer used by the codec driver
 * samples - buffer contained raw audio data
 * encodedSamples - buffer containing G.711 encoded data
 * decodedSamples - buffer containing G.711 decoded data
 * flashMemoryBuffer - buffer used by the SFM driver
 * */

int     codecBuffer     [WM8510DRV_DRV_BUFFER_SIZE];
int     samples         [FRAME_SIZE];
char    encodedSamples  [FRAME_SIZE];
int     decodedSamples  [FRAME_SIZE];

/* Instantiate the drivers structures
 * and create handles.
 * */

WM8510Handle codec;
WM8510Handle * codecHandle = &codec;

/* flags
 * record - if set means recording
 * playback - if set mean playback
 * erasedBeforeRecord - means SFM eras complete before record
 * */

/*
double fabs(double z){
    return (z > 0 ) ? z : -z;
}*/

int sign(double z){
    int result;

    if (z > 0) {
        result = 1;
    } else if (z == 0) {
        result = 0;
    } else {
        result = -1;
    }

    return result;
}

int max_abs_int( int *arr ){
    int abs_Mayor = 0;
    int sampleIndex;

    for (sampleIndex = 0; sampleIndex < FRAME_SIZE; sampleIndex++) {
        int actual_abs = fabs(arr[sampleIndex]);
        abs_Mayor = abs_Mayor < actual_abs ? actual_abs : abs_Mayor;
    }

    return abs_Mayor;
}

int max_abs_double( double arr[] ){
    int abs_Mayor = 0;
    int sampleIndex;

    for (sampleIndex = 0; sampleIndex < FRAME_SIZE; sampleIndex++) {
        int actual_abs = fabs(arr[sampleIndex]);
        abs_Mayor = abs_Mayor < actual_abs ? actual_abs : abs_Mayor;
    }

    return abs_Mayor;
}

void tremolo_effect( int *decodedSamples){
    int Fc = 1;
    double alpha = .8;
    int sampleIndex;
    int Fs = 5000;

    double trem[FRAME_SIZE], trem_Sample[FRAME_SIZE];

    for(sampleIndex = 0; sampleIndex < FRAME_SIZE; sampleIndex++){
        trem[sampleIndex] =  1 + alpha*sin(2*PI*sampleIndex*(Fc/Fs));
    }


    for(sampleIndex = 0; sampleIndex < FRAME_SIZE ; sampleIndex++){
        trem_Sample[sampleIndex] = trem[sampleIndex]*decodedSamples[sampleIndex];

    }

    for(sampleIndex = 0; sampleIndex < FRAME_SIZE ; sampleIndex++){
        decodedSamples[sampleIndex] = trem_Sample[sampleIndex];
    }
}

void fuzz_effect( int * decodedSamples){
    double q[FRAME_SIZE], z[FRAME_SIZE], y[FRAME_SIZE];
    int gain = 6;
    float mix = 1;
    int sampleIndex;
    double maxAbsDecodedSample = max_abs_int(decodedSamples);

    for(sampleIndex = 0; sampleIndex < FRAME_SIZE; sampleIndex++) {
        q[sampleIndex] = decodedSamples[sampleIndex] * gain/maxAbsDecodedSample;
        z[sampleIndex] = sign(-q[sampleIndex]) * (1- exp(sign(-q[sampleIndex]) * q[sampleIndex]));
    }

    double maxAbsZ = max_abs_double(z);

    for(sampleIndex = 0; sampleIndex < FRAME_SIZE; sampleIndex++) {
        y[sampleIndex] = mix*z[sampleIndex]*maxAbsDecodedSample/maxAbsZ + (1 -mix)*decodedSamples[sampleIndex];
    }

    double maxAbsY = max_abs_double(y);

    for(sampleIndex = 0; sampleIndex < FRAME_SIZE; sampleIndex++) {
        decodedSamples[sampleIndex] = y[sampleIndex]*maxAbsDecodedSample/maxAbsY;
    }
}

double tremolo_effect2(double trem_triangular, int *ptr_subida, int *decodedSamples){
    int sampleIndex;
    double delta = 5e-4;
    double maxf = 0.5;
    double minf = -0.5;

    for(sampleIndex = 0; sampleIndex < FRAME_SIZE ; sampleIndex++){
        decodedSamples[sampleIndex] = trem_triangular*decodedSamples[sampleIndex];

        if(*ptr_subida == 1){
            trem_triangular += delta;
        } else {
            trem_triangular -= delta;
        }

        if(trem_triangular >= maxf){
            *ptr_subida = 0;
        } else if(trem_triangular <= minf){
            *ptr_subida = 1;
        }

    }

    return trem_triangular;
}

double wah_wah_effect(double wah_triangular, int *ptr_subida, int * decodedSamples, double *last_yb, double *last_yh, double *last_yl, int *first_time_wah_wah_function){
    //Si el minimo se incrementa a 3000 se vuelve similar a un tremolo
    double minf = 2000;
    double maxf = 4000;
    double Fw = 5000;// frecuencia de la que modula///SE exgtiende el tiempo entre wah-wahs 
    double Fs = 44100;
    double delta = Fw/Fs;//Fw/Fs;//Fw/Fs=.045351->  (3000-500)/delta = 55125
    double damp = 0.05;//amortiguamiento de la caida  crecimiento y decrecimiento
    double F1;
    int sampleIndex;
    double yh[FRAME_SIZE], yb[FRAME_SIZE], yl[FRAME_SIZE];
    double Q1;

    F1 = 2*sin(PI*wah_triangular/Fs);
    Q1 = 2*damp;

    if(*first_time_wah_wah_function == 1){
        //**** INICIO CASO 1
        //CASO DEL PRIMER VALOR DLE FRAME TOMA EL DEL FRAME ANTERIOR
        yh[0] = decodedSamples[0];
        yb[0] = F1*yh[0];
        yl[0] = F1*yb[0];
        //***END CASO 1
        *first_time_wah_wah_function = 0;
    } else {
        yh[0] = decodedSamples[0] - (*last_yl) - Q1*(*last_yb);
        yb[0] = F1*yh[0] + (*last_yb);
        yl[0] = F1*yb[0] + (*last_yl);
    }

    if(*ptr_subida == 1){
        wah_triangular += delta;
    } else {
        wah_triangular -= delta;
    }

    if(wah_triangular >= maxf){
       *ptr_subida = 0;
    } else if(wah_triangular <= minf){
       *ptr_subida = 1;
    }

    for(sampleIndex = 1; sampleIndex < FRAME_SIZE ; sampleIndex++){
        yh[sampleIndex] = decodedSamples[sampleIndex] - yl[sampleIndex-1] - Q1*yb[sampleIndex -1];
        yb[sampleIndex] = F1*yh[sampleIndex] + yb[sampleIndex -1];
        yl[sampleIndex] = F1*yb[sampleIndex] + yl[sampleIndex -1];
        F1 = 2*sin(PI*wah_triangular/Fs);

        if(*ptr_subida == 1){
            wah_triangular += delta;
        } else {
            wah_triangular -= delta;
        }

        if(wah_triangular >= maxf){
            *ptr_subida = 0;
        } else if(wah_triangular <= minf){
            *ptr_subida = 1;
        }
    }

    double max_yb = max_abs_double(yb);

    for(sampleIndex = 0; sampleIndex < FRAME_SIZE; sampleIndex++) {
        decodedSamples[sampleIndex] = yb[sampleIndex];//max_yb;
        //Revisar si es necesaria la asignacion del ultimo valor en last_y*
        if(sampleIndex == FRAME_SIZE -1){
            *last_yb=yb[sampleIndex];
            *last_yh=yh[sampleIndex];
            *last_yl=yl[sampleIndex];
        }
    }

    return wah_triangular;
}

double phaser_effect(double phaser_triangular, int *ptr_subida, int * decodedSamples, double *last_yb, double *last_yh, double *last_yl, int *first_time_phaser_function){
    double minf = 500;
    double maxf = 2000;
    double minf2= 10000;
    double maxf2= 20000;

    double Fw = 8000;
    double Fs = 44100;
    double delta = Fw/Fs;//Fw/Fs=.045351->  (3000-500)/delta = 55125
    double damp = 0.04;
    double F1;
    int sampleIndex;
    double yh[FRAME_SIZE], yb[FRAME_SIZE], yl[FRAME_SIZE];
    double Q1;

    F1 = 2*sin(PI*phaser_triangular/Fs);
    Q1 = 2*damp;

    if(*first_time_phaser_function == 1){
        //**** INICIO CASO 1
        //CASO DEL PRIMER VALOR DLE FRAME TOMA EL DEL FRAME ANTERIOR
        yh[0] = decodedSamples[0];
        yb[0] = F1*yh[0];
        yl[0] = F1*yb[0];
        //***END CASO 1
        *first_time_phaser_function = 0;
    } else {
        yh[0] = decodedSamples[0] - (*last_yl) - Q1*(*last_yb);
        yb[0] = F1*yh[0] + (*last_yb);
        yl[0] = F1*yb[0] + (*last_yl);
    }

    if(*ptr_subida == 1){
        phaser_triangular += delta;
    } else {
        phaser_triangular -= delta;
    }

    if(phaser_triangular >= maxf){
       *ptr_subida = 0;
    } else if(phaser_triangular <= minf){
       *ptr_subida = 1;
    }

    for(sampleIndex = 1; sampleIndex < FRAME_SIZE ; sampleIndex++){
        yh[sampleIndex] = decodedSamples[sampleIndex] - yl[sampleIndex-1] - Q1*yb[sampleIndex -1];
        yb[sampleIndex] = F1*yh[sampleIndex] + yb[sampleIndex -1];
        yl[sampleIndex] = F1*yb[sampleIndex] + yl[sampleIndex -1];
        F1 = 2*sin(PI*phaser_triangular/Fs);

        if(*ptr_subida == 1){
        if(phaser_triangular == maxf){
            phaser_triangular = minf2;          
        }
            phaser_triangular += delta;
        } else {
        if(phaser_triangular == minf2){
            phaser_triangular = maxf;           
        }
            
            phaser_triangular -= delta;
        }

        if(phaser_triangular >= maxf2){
            *ptr_subida = 0;
        } else if(phaser_triangular <= minf){
            *ptr_subida = 1;
        }
    }

    double max_yb = max_abs_double(yb);

    for(sampleIndex = 0; sampleIndex < FRAME_SIZE; sampleIndex++) {
        decodedSamples[sampleIndex] = yb[sampleIndex];//max_yb;
        //Revisar si es necesaria la asignacion del ultimo valor en last_y*
        if(sampleIndex == FRAME_SIZE - 1){
            *last_yb=yb[sampleIndex];
            *last_yh=yh[sampleIndex];
            *last_yl=yl[sampleIndex];
        }
    }

    return phaser_triangular;
}

int flanger_effect(int * decodedSamples, int previous_decoded_samples[], int * first_time_flanger_function, int cont){
    //double max_time_delay = 0.0029; //3ms max delay in seconds 
    int rate = 1; //rate of flange in hz
    int max_sample_delay = 127; //Max time delay 8khz so 0-16ms
    int Fs = 8000;
    double amp = 0.7;
    double sin_ref[FRAME_SIZE];
    double cur_sin;
    int cur_delay; 
    int index;
/*    cont++;

    if(cont%1000 <500){
        for(index = 0; index < FRAME_SIZE; index++){
            decodedSamples[index] = 0;
        }      
     }
*/

    if(*first_time_flanger_function == 1) {
        for(index = 0; index < FRAME_SIZE; index++){
            previous_decoded_samples[index] = decodedSamples[index];
        }

        *first_time_flanger_function = 0;
     } else {
        int tempSample[FRAME_SIZE];
        int sampleIndex = 0;

        for(index = 0; index < FRAME_SIZE; index++){
            tempSample[index] = decodedSamples[index];
        }

        for (sampleIndex = 0; sampleIndex < FRAME_SIZE; sampleIndex++){
            //Sample index deberia empezar desde 129 hasta n(multiplo de 2pi) hasta q sea fs/rate
            sin_ref[sampleIndex] = sin(2*PI*sampleIndex*(rate/Fs));
            cur_sin = fabs(sin_ref[sampleIndex]);
            cur_delay = ceil(cur_sin*max_sample_delay);
            cur_delay=(cur_delay<0)? - cur_delay:cur_delay;

            decodedSamples[sampleIndex] = amp*(decodedSamples[sampleIndex] + previous_decoded_samples[max_sample_delay - cur_delay]);
        }
        
        for (sampleIndex = 0; sampleIndex < FRAME_SIZE; sampleIndex++){
            //previous_decoded_samples[sampleIndex] = tempSample[sampleIndex];
			decodedSamples[sampleIndex] = previous_decoded_samples[sampleIndex];
        }
    }

    return cont;
}

int main(void) {
    int selector_efecto = 0;
    
    ///tremolo2
    double trem_triangular = -0.5;
    int trem_subida = 1;
    ///tremolo2
    
    ///wahwah
    double wah_triangular = 500;
    int wah_subida = 1;
    double last_wah_wah_yb, last_wah_wah_yh, last_wah_wah_yl;
    int first_time_wah_wah_function = 1;
    ///wahwah
    
    ///phaser
    double phaser_triangular = 500;
    int phaser_subida = 1;
    double last_phaser_yb, last_phaser_yh, last_phaser_yl;
    int first_time_phaser_function = 1;
    ///Phaser

    ///flanger
    int previous_decoded_samples[FRAME_SIZE];
    int first_time_flanger_function = 1;
    int cont = 0;
    ///flanger 
    
    /* Configure Oscillator to operate the device at 40MHz.
     * Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
     * Fosc= 700Mhz for 7.37M input clock */
    
    PLLFBD=41;              /* M=39 */
    CLKDIVbits.PLLPOST=0;       /* N1=2 */
    CLKDIVbits.PLLPRE=0;        /* N2=2 */
    OSCTUN=0;

    __builtin_write_OSCCONH(0x01);      /*  Initiate Clock Switch to FRC with PLL*/
    __builtin_write_OSCCONL(0x01);
    while (OSCCONbits.COSC != 0b01);    /*  Wait for Clock switch to occur  */
    while(!OSCCONbits.LOCK);

    /* Intialize the board and the drivers  */
    SASKInit();
    WM8510Init(codecHandle,codecBuffer);

    /* Start Audio input and output function    */
    WM8510Start(codecHandle);

    /* Configure codec for 8K operation */
    WM8510SampleRate8KConfig(codecHandle);

    /* Main processing loop. Executed for every input and
     * output frame */

    while(1) {
        /*Obtain Audio Samples  */
        while(WM8510IsReadBusy(codecHandle));
        WM8510Read(codecHandle,samples,FRAME_SIZE);

        G711Lin2Ulaw(samples,encodedSamples,FRAME_SIZE);

        /* Decode the samples   */
        G711Ulaw2Lin (encodedSamples,decodedSamples, FRAME_SIZE);

        /* Wait till the codec is available for a new  frame    */
        while(WM8510IsWriteBusy(codecHandle));

        switch(selector_efecto) {
            case 0:
                // prenden los leds
                RED_LED=SASK_LED_OFF;
                YELLOW_LED=SASK_LED_OFF;
                GREEN_LED=SASK_LED_OFF;
                // se activa el clean
                break;

            case 1:
                // prenden los leds
                RED_LED=SASK_LED_OFF;
                YELLOW_LED=SASK_LED_OFF;
                GREEN_LED=SASK_LED_ON;
                // Se activa el efecto de tremolo
                tremolo_effect(decodedSamples);
                break;

            case 2:
                // prenden los leds
                RED_LED=SASK_LED_OFF;
                YELLOW_LED=SASK_LED_ON;
                GREEN_LED=SASK_LED_OFF;
                // Se activa el efecto de tremolo2
                 trem_triangular = tremolo_effect2(trem_triangular, &trem_subida, decodedSamples);
                break;
            case 3:
                // prenden los leds
                RED_LED=SASK_LED_OFF;
                YELLOW_LED=SASK_LED_ON;
                GREEN_LED=SASK_LED_ON;
                // Se activa el efecto de fuzz
                fuzz_effect(decodedSamples);
                break;
            case 4:
                // prenden los leds
                RED_LED=SASK_LED_ON;
                YELLOW_LED=SASK_LED_OFF;
                GREEN_LED=SASK_LED_OFF;
                // Se activa el efecto de wah wah
                wah_triangular = wah_wah_effect(wah_triangular, &wah_subida, decodedSamples, &last_wah_wah_yb, &last_wah_wah_yh, &last_wah_wah_yl, &first_time_wah_wah_function);
                break;
            case 5:
                // prenden los leds
                RED_LED=SASK_LED_ON;
                YELLOW_LED=SASK_LED_OFF;
                GREEN_LED=SASK_LED_ON;
                // Se activa el efecto de wah wah
                phaser_triangular = phaser_effect(phaser_triangular, &phaser_subida, decodedSamples, &last_phaser_yb, &last_phaser_yh, &last_phaser_yl, &first_time_phaser_function);
                break;
            case 6:
                // prenden los leds
                RED_LED=SASK_LED_ON;
                YELLOW_LED=SASK_LED_ON;
                GREEN_LED=SASK_LED_OFF;
                // Se activa el efecto de wah wah
                cont = flanger_effect(decodedSamples, previous_decoded_samples, &first_time_flanger_function, cont);
                break;
            case 7:
                // prenden los leds
                RED_LED=SASK_LED_ON;
                YELLOW_LED=SASK_LED_ON;
                GREEN_LED=SASK_LED_ON;
                // Se activa el efecto de wah wah
                break;
            default:
                break;

        }
        /* Write the frame to the output    */
        WM8510Write (codecHandle,decodedSamples,FRAME_SIZE);

        if(CheckSwitchS1()){
            selector_efecto--;
        }

        if(CheckSwitchS2()){
            selector_efecto++;
        }
        // rango de 0-7 para selector de efectos
        if (selector_efecto < 0 ) {
            selector_efecto = 7;
        }
        if (selector_efecto > 7  ) {
            selector_efecto = 0;
        }
    }
}

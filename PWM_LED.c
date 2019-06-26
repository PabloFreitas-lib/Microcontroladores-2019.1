#include "bib.h"
#define GPIO_PF1_M1PWM5         0x00050405
#define GPIO_PF2_M1PWM6         0x00050805
#define GPIO_PF3_M1PWM7         0x00050C05
/*
 *  PF1 as PWM outputs --> Timer0 B
 *  PF2 as PWM outputs --> Timer1 A
 *  PF3 as PWM outputs --> Timer1 B
 */
void ADC0handler(void);
void Systickhandler(void);

//about 2ms at 80Mhz
#define time 56666
#define PWM_FREQ 55

int i;
int R[30] = {255,255,255,255,255,255,255,255,255,255,175 ,23,   1,  1,  1,  1,  1,1 ,1};
int G[30] = {9  ,  5,  1,  1,  1,  1, 90, 50,130,220,255,255,255,255,200,100,130,50,1};
int B[30] = {240,200,160,100, 40,  1,  1,  1,  1,  1,  1,  1, 50,100,255,255,255,255,255};

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    //Nokia
   /* Nokia5110_Init();
    Nokia5110_Clear();
    Nokia5110_ClearBuffer();*/

    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);// parao pwm dos leds
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);//para os leds
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);//para os leds
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);// para a temperatura

    //ADC_config();
    //verifica_temp();

    GPIOPinConfigure(GPIO_PF1_M1PWM5);
    GPIOPinConfigure(GPIO_PF2_M1PWM6);
    GPIOPinConfigure(GPIO_PF3_M1PWM7);

    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, 320);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, 320);

    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, 255);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, 255);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, 255);

    PWMGenEnable(PWM1_BASE, PWM_GEN_2);
    PWMGenEnable(PWM1_BASE, PWM_GEN_3);

    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT | PWM_OUT_6_BIT | PWM_OUT_7_BIT, true);



    //IntMasterEnable(); // todas as interrupt
    /*SysTickPeriodSet(0xFFFFFF);

    SysTickEnable();
    SysTickIntEnable();
    IntEnable(INT_ADC0SS0);
    ADCIntEnable(ADC0_BASE, 0);*/


    while(1)
    {
       for( i = 0; i<16; i++){
           PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,R[i]);
           PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,G[i]);
           PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,B[i]);
           SysCtlDelay(1000000);
       }
        // temperatura
        /*ADCProcessorTrigger(ADC0_BASE, 0);
        // Passo 11
        ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3]+ ui32ADC0Value[4] + ui32ADC0Value[5] + ui32ADC0Value[6] + ui32ADC0Value[7] + 4)/8;

        // Passo 12
        ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096);

        //Tarefa 1 lab6
        temperatura_calc(ui32TempValueC);*/

    }
}

void ADC0handler(void)
{
    // clear the interrupt flag, grab the data, set the 'done' flag
    ADCIntClear(ADC0_BASE, 0);
    // Passo 10
   // ( modulo | seq | Endereco de onde armazenar)
   ADCSequenceDataGet(ADC0_BASE, 0, ui32ADC0Value);
   verifica_temp();
}


void Systickhandler(void)
{

    ADCIntDisable(ADC0_BASE, 0);
   if(++ctd == 10)
   {
    ctd=0;
    IntEnable(INT_ADC0SS0);
    ADCIntEnable(ADC0_BASE, 0);
   }
}

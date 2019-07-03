#ifndef BIB_H
#define BIB_H

#include <stdint.h>
#include <stdbool.h>

#include "utils/ustdlib.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/debug.h"
#include "driverlib/hibernate.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/eeprom.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/uart.h"
#include "driverlib/systick.h"
#include "utils/uartstdio.h"
#include "driverlib/timer.h"
#include "inc/tm4c123gh6pm.h"


#include "Nokia5110.h"
#include "bild.h"


int i=0;
uint32_t ui32ADC0Value[8];
volatile uint32_t ui32TempAvg;
volatile uint32_t ui32TempValueC;
int Celta=0;
//volatile uint32_t ui32TempValueF;
volatile uint32_t newtemp;
const int sampleFreq = 8000; // Sample Frequency
uint16_t samplePeriod;
unsigned int n1=0,n2=0,n3=0,n4=0;
unsigned int n1_t=0,n2_t=0,n3_t=0,n4_t=0;
int ctd=0,ctd2=0,i;
unsigned int h1=2,h2=0,m1=3,m2=9;
int systick_relogio = 1600000, systick_cronometro = 1500000;

int ctd_relogio=0;
int ein_minute = 470;
int ctd_trasicao=0;
int ctd_tela3=100;

int qual_tela =0;

float T1=16.2,T2=15.3,T3=14.1;
int ctd_tela1=100;

int temp_atual=100,temp_min=100,temp_max=100;



void max_temp_display(int aux_temp)
{
        n4_t = aux_temp/100;
        aux_temp = aux_temp%100;
        n3_t = aux_temp/10;
        n2_t = aux_temp%10;

        if(qual_tela == 2)
        {


        Nokia5110_OutString("TM:");

        if(n4_t == 1)
            Nokia5110_OutChar('1');
        else if(n4_t == 2)
            Nokia5110_OutChar('2');
        else if(n4_t == 3)
            Nokia5110_OutChar('3');
        else if(n4_t == 4)
            Nokia5110_OutChar('4');
        else if(n4_t == 5)
            Nokia5110_OutChar('5');
        else if(n4_t == 6)
            Nokia5110_OutChar('6');
        else if(n4_t == 7)
            Nokia5110_OutChar('7');
        else if(n4_t == 8)
            Nokia5110_OutChar('8');
        else if(n4_t == 9)
            Nokia5110_OutChar('9');
        else if(n4_t == 0)
            Nokia5110_OutChar('0');

        if(n3_t == 1)
            Nokia5110_OutChar('1');
        else if(n3_t == 2)
            Nokia5110_OutChar('2');
        else if(n3_t == 3)
            Nokia5110_OutChar('3');
        else if(n3_t == 4)
            Nokia5110_OutChar('4');
        else if(n3_t == 5)
            Nokia5110_OutChar('5');
        else if(n3_t == 6)
            Nokia5110_OutChar('6');
        else if(n3_t == 7)
            Nokia5110_OutChar('7');
        else if(n3_t == 8)
            Nokia5110_OutChar('8');
        else if(n3_t == 9)
            Nokia5110_OutChar('9');
        else if(n3_t == 0)
            Nokia5110_OutChar('0');

        Nokia5110_OutChar('.');

        if(n2_t == 1)
            Nokia5110_OutChar('1');
        else if(n2_t == 2)
            Nokia5110_OutChar('2');
        else if(n2_t == 3)
            Nokia5110_OutChar('3');
        else if(n2_t == 4)
            Nokia5110_OutChar('4');
        else if(n2_t == 5)
            Nokia5110_OutChar('5');
        else if(n2_t == 6)
            Nokia5110_OutChar('6');
        else if(n2_t == 7)
            Nokia5110_OutChar('7');
        else if(n2_t == 8)
            Nokia5110_OutChar('8');
        else if(n2_t == 9)
            Nokia5110_OutChar('9');
        else if(n2_t == 0)
            Nokia5110_OutChar('0');

        Nokia5110_OutString("    C");
        }

}


void min_temp_display(int aux_temp)
{
        n4_t = aux_temp/100;
        aux_temp = aux_temp%100;
        n3_t = aux_temp/10;
        n2_t = aux_temp%10;

        if(qual_tela == 2)
        {


        Nokia5110_OutString("Tm:");

        if(n4_t == 1)
            Nokia5110_OutChar('1');
        else if(n4_t == 2)
            Nokia5110_OutChar('2');
        else if(n4_t == 3)
            Nokia5110_OutChar('3');
        else if(n4_t == 4)
            Nokia5110_OutChar('4');
        else if(n4_t == 5)
            Nokia5110_OutChar('5');
        else if(n4_t == 6)
            Nokia5110_OutChar('6');
        else if(n4_t == 7)
            Nokia5110_OutChar('7');
        else if(n4_t == 8)
            Nokia5110_OutChar('8');
        else if(n4_t == 9)
            Nokia5110_OutChar('9');
        else if(n4_t == 0)
            Nokia5110_OutChar('0');

        if(n3_t == 1)
            Nokia5110_OutChar('1');
        else if(n3_t == 2)
            Nokia5110_OutChar('2');
        else if(n3_t == 3)
            Nokia5110_OutChar('3');
        else if(n3_t == 4)
            Nokia5110_OutChar('4');
        else if(n3_t == 5)
            Nokia5110_OutChar('5');
        else if(n3_t == 6)
            Nokia5110_OutChar('6');
        else if(n3_t == 7)
            Nokia5110_OutChar('7');
        else if(n3_t == 8)
            Nokia5110_OutChar('8');
        else if(n3_t == 9)
            Nokia5110_OutChar('9');
        else if(n3_t == 0)
            Nokia5110_OutChar('0');

        Nokia5110_OutChar('.');

        if(n2_t == 1)
            Nokia5110_OutChar('1');
        else if(n2_t == 2)
            Nokia5110_OutChar('2');
        else if(n2_t == 3)
            Nokia5110_OutChar('3');
        else if(n2_t == 4)
            Nokia5110_OutChar('4');
        else if(n2_t == 5)
            Nokia5110_OutChar('5');
        else if(n2_t == 6)
            Nokia5110_OutChar('6');
        else if(n2_t == 7)
            Nokia5110_OutChar('7');
        else if(n2_t == 8)
            Nokia5110_OutChar('8');
        else if(n2_t == 9)
            Nokia5110_OutChar('9');
        else if(n2_t == 0)
            Nokia5110_OutChar('0');

        Nokia5110_OutString("    C");
        }

}

void atual_temp_display(int aux_temp)
{
        n4_t = aux_temp/100;
        aux_temp = aux_temp%100;
        n3_t = aux_temp/10;
        n2_t = aux_temp%10;

        if(qual_tela == 2)
        {


        Nokia5110_OutString("TA:");

        if(n4_t == 1)
            Nokia5110_OutChar('1');
        else if(n4_t == 2)
            Nokia5110_OutChar('2');
        else if(n4_t == 3)
            Nokia5110_OutChar('3');
        else if(n4_t == 4)
            Nokia5110_OutChar('4');
        else if(n4_t == 5)
            Nokia5110_OutChar('5');
        else if(n4_t == 6)
            Nokia5110_OutChar('6');
        else if(n4_t == 7)
            Nokia5110_OutChar('7');
        else if(n4_t == 8)
            Nokia5110_OutChar('8');
        else if(n4_t == 9)
            Nokia5110_OutChar('9');
        else if(n4_t == 0)
            Nokia5110_OutChar('0');

        if(n3_t == 1)
            Nokia5110_OutChar('1');
        else if(n3_t == 2)
            Nokia5110_OutChar('2');
        else if(n3_t == 3)
            Nokia5110_OutChar('3');
        else if(n3_t == 4)
            Nokia5110_OutChar('4');
        else if(n3_t == 5)
            Nokia5110_OutChar('5');
        else if(n3_t == 6)
            Nokia5110_OutChar('6');
        else if(n3_t == 7)
            Nokia5110_OutChar('7');
        else if(n3_t == 8)
            Nokia5110_OutChar('8');
        else if(n3_t == 9)
            Nokia5110_OutChar('9');
        else if(n3_t == 0)
            Nokia5110_OutChar('0');

        Nokia5110_OutChar('.');

        if(n2_t == 1)
            Nokia5110_OutChar('1');
        else if(n2_t == 2)
            Nokia5110_OutChar('2');
        else if(n2_t == 3)
            Nokia5110_OutChar('3');
        else if(n2_t == 4)
            Nokia5110_OutChar('4');
        else if(n2_t == 5)
            Nokia5110_OutChar('5');
        else if(n2_t == 6)
            Nokia5110_OutChar('6');
        else if(n2_t == 7)
            Nokia5110_OutChar('7');
        else if(n2_t == 8)
            Nokia5110_OutChar('8');
        else if(n2_t == 9)
            Nokia5110_OutChar('9');
        else if(n2_t == 0)
            Nokia5110_OutChar('0');

        Nokia5110_OutString("    C");
        }

}



void define_hoarario(int h1_t,int h2_t,int m1_t, int m2_t)
{
    h1=h1_t;h2=h2_t;m1=m1_t;m2=m2_t;
}
void LedVermelho(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
}

void LedVerde(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
}

void LedAzul(void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
}

void setTemperatura(int temp)
{
    for(i=24;i>9;i--)
                Nokia5110_ClrPxl(i, 23);

    if(temp <= 12)
    {
        for(i=24;i>19;i--)
            Nokia5110_SetPxl(i, 23);
    }
    if(temp >12 && temp < 14)
    {
        for(i=24;i>17;i--)
            Nokia5110_SetPxl(i, 23);
    }
    if(temp >=14 && temp < 18)
    {
        for(i=24;i>12;i--)
            Nokia5110_SetPxl(i, 23);
    }
    if(temp >= 18)
    {
        for(i=24;i>9;i--)
            Nokia5110_SetPxl(i, 23);
    }


    Nokia5110_DisplayBuffer();
}

void ADC_config(void)
{
    // Passo 4 (modulo |seq |fonte de disparo | prioridade )
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);

    // Passo 5
    // (modulo | seq | passo | fonte(nesse caso sensor de temperatura))
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE,0,7,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);// adicionamos interrupcao e onde terminar a sequencia


    // Passo 6
    // ( modulo | seq )
    ADCSequenceEnable(ADC0_BASE, 0);
}

void temperatura_calc(uint32_t temperatura)
{
    n4 = temperatura/100;
    temperatura = temperatura%100;
    n3 = temperatura/10;
    n2 = temperatura%10;
}


void verifica_temp(void)
{
    temp_atual = ui32TempValueC;
    temp_min = 134;
    temp_max = temp_atual + 20;

    if(temp_min >= temp_atual)
    {
        temp_min = temp_atual;
    }
    else if(temp_max <= temp_atual)
    {
        temp_max = temp_atual;
    }
    temperatura_calc(ui32TempValueC);
    if(qual_tela == 1)
    {
        Nokia5110_PrintBMP2(0, 0, termometro, 84, 48);
        //Primeiro numero
        if(n4<10)
        Nokia5110_PrintBMP2(40, 10, numeros_t[n4], 12, 14);

        // Segundo Numero
        Nokia5110_PrintBMP2(55, 10, numeros_t[n3], 12, 14);

        // Casa decimal
        Nokia5110_PrintBMP2(70, 10, numeros_t[n2], 12, 14);

        Nokia5110_PrintBMP2(68, 17, ponto, 2, 7);

        setTemperatura(ui32TempValueC/10);

        Nokia5110_DisplayBuffer();
    }
}

void verifica_horario(void)
{
    Nokia5110_Clear();
    Nokia5110_ClearBuffer();
    //Primeiro numero Hora 1 [ X - - -] 0 - 2

    Nokia5110_PrintBMP2(20, 20, numeros_t[h1], 12, 14);

    // Segundo Numero Hora 2 [ - X - -] 0 - 9

    Nokia5110_PrintBMP2(35, 20, numeros_t[h2], 12, 14);

    // Casa decimal Minuto 1 [ - - X -] 0 -5
    Nokia5110_PrintBMP2(50, 20, numeros_t[m1], 12, 14);

    // Casa decimal Minuto 1 [ - - - X] 0 -9
    Nokia5110_PrintBMP2(65, 20, numeros_t[m2], 12, 14);

    Nokia5110_PrintBMP2(48, 25, dois_pontos, 2, 7);

    Nokia5110_DisplayBuffer();

    Nokia5110_OutString("Mittwoch 10.Juni");
}


void relogio(void)
{

        if(m2>9)
        {
            ++m1;
            m2=0;
        }
        if(m1>5)
        {
            ++h2;
            m1=0;
        }
        if(h1 == 0 && h2>9) // se h1 for 0 entao h2
        {
            ++h1;
            h2=0;
        }
        if(h1 == 1 && h2 > 9)
        {
            ++h1;
            h2=0;
        }
        if(h1 == 2 && h2 ==4)
        {
            h1=0;h2=0;m1=0;m2=0;
        }
        if(qual_tela == 0)
            verifica_horario();
}


void config_Timer(void)
{
    TimerConfigure(TIMER0_BASE,TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE,TIMER_A,10);
    TimerControlTrigger(TIMER0_BASE,TIMER_A,true);
}

void pinos_Led_RGB(void)
{
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1);
}
void Clock_B_F(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}


#endif

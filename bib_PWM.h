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
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"

#include "driverlib/pin_map.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"


#define GPIO_PF1_M1PWM5         0x00050405
#define GPIO_PF2_M1PWM6         0x00050805
#define GPIO_PF3_M1PWM7         0x00050C05



#include "Nokia5110.h"
#include "bild.h"

#define PWM_FREQUENCY 55



int i=0;
uint32_t ui32ADC0Value[8];
uint32_t ui32TempAvg;
uint32_t ui32TempValueC;
int Celta=0;
//volatile uint32_t ui32TempValueF;
volatile uint32_t newtemp;
const int sampleFreq = 8000; // Sample Frequency
uint16_t samplePeriod;
int n1=0,n2=0,n3=0,n4=0;
int n1_t=0,n2_t=0,n3_t=0,n4_t=0;
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
int uart_recebida =0;

int ctd_cores=0;
int i;
int R[30] = {255,255,255,255,255,255,255,255,255,255,175 ,23,1,1,1,1,1,1,1};
int G[30] = {9,5,1,1,1,1,90,50,130,220,255,255,255,255,200,100,130,50,1};
int B[30] = {240,200,160,100, 40,  1,  1,  1,  1,  1,  1,  1, 50,100,255,255,255,255,255};

int R_temperatura[9]= {1,1,1,1,62,253,255,255,255};
int G_temperatura[9]={50,164,255,255,255,255,170,120,1};
int B_temperatura[9]={255,255,244,92,1,1,1,1,1};
//                      B  B   B  G  G  Y O O  R


int temperaturas_negativas_R[3]={1,1,1};
int temperaturas_negativas_G[3]={164,75,35};
int temperaturas_negativas_B[3]={255,255,255};

int temperaturas_positivas_R[12]={1,1,1,62,138,215,255,255,255,255,255,255};
int temperaturas_positivas_G[12]={225,255,255,255,255,255,210,170,120,60,15,1};
int temperaturas_positivas_B[12]={255,131,16,1,1,1,1,1,1,1,1,1};

void temp_simulation(void);
void max_temp_display(int aux_temp);
void setTemperatura(int temp);
void ADC_config(void);
void verifica_temp(void);
void temperatura_calc(uint32_t temperatura);
void flow(void);
void temperatura_calc_v2(int temperatura);
void temperatura_calc_v3(int temperatura);


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
    if(temp >=14 && temp < 16)
    {
        for(i=24;i>15;i--)
            Nokia5110_SetPxl(i, 23);
    }
    if(temp >=16 && temp < 18)
        {
            for(i=24;i>13;i--)
                Nokia5110_SetPxl(i, 23);
        }
    if(temp >= 18)
    {
        for(i=24;i>11;i--)
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

int temp_aux=100;
int sinal =1;
int up =1;
/*
 * tentar fazer essa ir de -10 ate 10 somente
 */
void temp_simulation(void)
{
    Nokia5110_Clear();
    Nokia5110_ClearBuffer();
    temperatura_calc_v2(temp_aux);

    Nokia5110_PrintBMP2(0, 0, termometro, 84, 48);
    //Primeiro numero
    if(up == 1)
    {
        if(temp_aux == 100 && sinal == 1)
        {
            temperatura_calc_v2(temp_aux);
            Nokia5110_PrintBMP2(30, 17, menos_Bitmap, 4, 3);
            if(n4<10)
                Nokia5110_PrintBMP2(40, 10, numeros_t[n4], 12, 14);

            // Segundo Numero
                Nokia5110_PrintBMP2(55, 10, numeros_t[n3], 12, 14);

            // Casa decimal
            Nokia5110_PrintBMP2(70, 10, numeros_t[n2], 12, 14);

            Nokia5110_PrintBMP2(68, 17, ponto, 2, 7);
            temp_aux--;
            //SysCtlDelay(SysCtlClockGet()/50000);
        }
        else if(temp_aux <= 99 && sinal == 1 && temp_aux >0)
        {
            temperatura_calc_v3(temp_aux);
            Nokia5110_PrintBMP2(45, 17, menos_Bitmap, 4, 3);
            // Segundo Numero
                Nokia5110_PrintBMP2(55, 10, numeros_t[n3], 12, 14);

            // Casa decimal
            Nokia5110_PrintBMP2(70, 10, numeros_t[n2], 12, 14);

            Nokia5110_PrintBMP2(68, 17, ponto, 2, 7);
            temp_aux--;
            //SysCtlDelay(SysCtlClockGet()/50000);
        }
        else if(temp_aux == 0 && temp_aux < 100)
        {
            sinal =0;
            temperatura_calc_v3(temp_aux);
            // Segundo Numero
            Nokia5110_PrintBMP2(55, 10, numeros_t[n3], 12, 14);

            // Casa decimal
            Nokia5110_PrintBMP2(70, 10, numeros_t[n2], 12, 14);

            Nokia5110_PrintBMP2(68, 17, ponto, 2, 7);
            temp_aux++;
        }

        else if( temp_aux < 100)
        {
            sinal =0;
            temperatura_calc_v3(temp_aux);
            // Segundo Numero
            Nokia5110_PrintBMP2(55, 10, numeros_t[n3], 12, 14);
            // Casa decimal
            Nokia5110_PrintBMP2(70, 10, numeros_t[n2], 12, 14);

            Nokia5110_PrintBMP2(68, 17, ponto, 2, 7);
            temp_aux++;
        }
        else if( temp_aux < 800)
        {
            sinal =0;
            temperatura_calc(temp_aux);
            // Segundo Numero
            Nokia5110_PrintBMP2(40, 10, numeros_t[n4], 12, 14);

            // Segundo Numero
            Nokia5110_PrintBMP2(55, 10, numeros_t[n3], 12, 14);

            // Casa decimal
            Nokia5110_PrintBMP2(70, 10, numeros_t[n2], 12, 14);

            Nokia5110_PrintBMP2(68, 17, ponto, 2, 7);
            temp_aux++;

        }
        else if( temp_aux == 800 && up ==1)
        {
            up = 0;
        }
    }
    else if( up == 0)
    {

        if( temp_aux <= 800 && temp_aux >= 100 && sinal == 0)
        {
            sinal =0;
            temperatura_calc(temp_aux);
            // Segundo Numero
            Nokia5110_PrintBMP2(40, 10, numeros_t[n4], 12, 14);

            // Segundo Numero
            Nokia5110_PrintBMP2(55, 10, numeros_t[n3], 12, 14);

            // Casa decimal
            Nokia5110_PrintBMP2(70, 10, numeros_t[n2], 12, 14);

            Nokia5110_PrintBMP2(68, 17, ponto, 2, 7);
            temp_aux--;

        }
        else if(temp_aux < 100 && temp_aux >=0 && sinal == 0)
        {
            sinal =0;
            temperatura_calc_v3(temp_aux);
            // Segundo Numero
                Nokia5110_PrintBMP2(55, 10, numeros_t[n3], 12, 14);

            // Casa decimal
            Nokia5110_PrintBMP2(70, 10, numeros_t[n2], 12, 14);

            Nokia5110_PrintBMP2(68, 17, ponto, 2, 7);
            temp_aux--;
            if(temp_aux == -1)
            {
                sinal =1;
                temp_aux=1;
            }
            //SysCtlDelay(SysCtlClockGet()/50000);
        }
        else if(temp_aux < 100 && sinal ==1)
        {
            temperatura_calc_v3(temp_aux);
            Nokia5110_PrintBMP2(45, 17, menos_Bitmap, 4, 3);
            // Segundo Numero
                Nokia5110_PrintBMP2(55, 10, numeros_t[n3], 12, 14);

            // Casa decimal
            Nokia5110_PrintBMP2(70, 10, numeros_t[n2], 12, 14);

            Nokia5110_PrintBMP2(68, 17, ponto, 2, 7);
            temp_aux++;
            //SysCtlDelay(SysCtlClockGet()/50000);
        }
        else if(temp_aux == 0 && temp_aux < 100 && up ==1 )
        {
            sinal =0;
            temperatura_calc_v3(temp_aux);
            // Segundo Numero
            Nokia5110_PrintBMP2(55, 10, numeros_t[n3], 12, 14);

            // Casa decimal
            Nokia5110_PrintBMP2(70, 10, numeros_t[n2], 12, 14);

            Nokia5110_PrintBMP2(68, 17, ponto, 2, 7);
            temp_aux++;
        }

        else if( temp_aux < 100 && up ==1)
        {
            sinal =0;
            temperatura_calc_v3(temp_aux);
            // Segundo Numero
            Nokia5110_PrintBMP2(55, 10, numeros_t[n3], 12, 14);
            // Casa decimal
            Nokia5110_PrintBMP2(70, 10, numeros_t[n2], 12, 14);

            Nokia5110_PrintBMP2(68, 17, ponto, 2, 7);
            temp_aux++;

        }
        else if(temp_aux == 100)
        {

            up =1;
            sinal =1;
        }

    }

    setTemperatura(temp_aux/10);

    Nokia5110_DisplayBuffer();

}

void verifica_temp(void)
{
    Nokia5110_Clear();
    Nokia5110_ClearBuffer();

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

        if(ui32TempValueC <100)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,R_temperatura[0]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,G_temperatura[0]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,B_temperatura[0]);
        }
        else if(ui32TempValueC >=100 && ui32TempValueC <115)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,R_temperatura[1]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,G_temperatura[1]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,B_temperatura[1]);
        }
        else if(ui32TempValueC >=115 && ui32TempValueC <130)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,R_temperatura[2]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,G_temperatura[2]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,B_temperatura[2]);
        }
        else if(ui32TempValueC >=135 && ui32TempValueC <150)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,R_temperatura[3]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,G_temperatura[3]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,B_temperatura[3]);
        }
        else if(ui32TempValueC >=150 && ui32TempValueC <165)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,R_temperatura[4]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,G_temperatura[4]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,B_temperatura[4]);
        }
        else if(ui32TempValueC >=165 && ui32TempValueC <180)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,R_temperatura[5]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,G_temperatura[5]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,B_temperatura[5]);
        }
        else if(ui32TempValueC >=180 && ui32TempValueC <190)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,R_temperatura[6]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,G_temperatura[6]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,B_temperatura[6]);
        }
        else if(ui32TempValueC >=190 && ui32TempValueC <200)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,R_temperatura[7]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,G_temperatura[7]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,B_temperatura[7]);
        }
        else if(ui32TempValueC >=200 && ui32TempValueC <290)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,R_temperatura[8]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,G_temperatura[8]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,B_temperatura[8]);
        }


        SysCtlDelay(SysCtlClockGet()/25);
}

void temperatura_calc(uint32_t temperatura)
{
    n4 = temperatura/100;
    temperatura = temperatura%100;
    n3 = temperatura/10;
    n2 = temperatura%10;
    //verifica_temp();
}

void temperatura_calc_v2(int temperatura)
{
    n4 = temperatura/100;
    temperatura = temperatura%100;
    n3 = temperatura/10;
    n2 = temperatura%10;
}

void temperatura_calc_v3(int temperatura)
{
    n3 = temperatura/10;
    n2 = temperatura%10;
}

void flow(void)
{
    temp_simulation();
    // -10 ate 0 crescendo
    if(sinal == 1 && temp_aux<= 100)
    {
        if(temp_aux<=100 && temp_aux >= 67)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,temperaturas_negativas_R[2]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,temperaturas_negativas_G[2]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,temperaturas_negativas_B[2]);
        }
        else if( temp_aux <=67 && temp_aux >= 33)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,temperaturas_negativas_R[1]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,temperaturas_negativas_G[1]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,temperaturas_negativas_B[1]);
        }
        else if( temp_aux <= 33 && temp_aux >=0)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,temperaturas_negativas_R[0]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,temperaturas_negativas_G[0]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,temperaturas_negativas_B[0]);
        }
    }
    // de 0 ate 80 crescendo
    else if( sinal == 0 && temp_aux <= 800)
    {
        if(temp_aux <=66 && temp_aux >=0)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,temperaturas_positivas_R[0]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,temperaturas_positivas_G[0]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,temperaturas_positivas_B[0]);
        }
        else if(temp_aux <=132 && temp_aux >=66)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,temperaturas_positivas_R[1]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,temperaturas_positivas_G[1]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,temperaturas_positivas_B[1]);
        }
        else if(temp_aux <=198 && temp_aux >=132)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,temperaturas_positivas_R[2]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,temperaturas_positivas_G[2]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,temperaturas_positivas_B[2]);
        }
        else if(temp_aux <=264 && temp_aux >=198)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,temperaturas_positivas_R[3]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,temperaturas_positivas_G[3]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,temperaturas_positivas_B[3]);
        }
        else if(temp_aux <=330 && temp_aux >=0)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,temperaturas_positivas_R[4]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,temperaturas_positivas_G[4]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,temperaturas_positivas_B[4]);
        }
        else if(temp_aux <=396 && temp_aux >=0)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,temperaturas_positivas_R[5]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,temperaturas_positivas_G[5]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,temperaturas_positivas_B[5]);
        }
        else if(temp_aux <=462 && temp_aux >=0)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,temperaturas_positivas_R[6]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,temperaturas_positivas_G[6]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,temperaturas_positivas_B[6]);
        }
        else if(temp_aux <=528 && temp_aux >=0)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,temperaturas_positivas_R[7]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,temperaturas_positivas_G[7]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,temperaturas_positivas_B[7]);
        }
        else if(temp_aux <=100 && temp_aux >=0)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,temperaturas_positivas_R[8]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,temperaturas_positivas_G[8]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,temperaturas_positivas_B[8]);
        }
        else if(temp_aux <=594 && temp_aux >=0)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,temperaturas_positivas_R[9]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,temperaturas_positivas_G[9]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,temperaturas_positivas_B[9]);
        }
        else if(temp_aux <=660 && temp_aux >=0)
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,temperaturas_positivas_R[10]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,temperaturas_positivas_G[10]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,temperaturas_positivas_B[10]);
        }
        else
        {
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,temperaturas_positivas_R[11]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,temperaturas_positivas_G[11]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,temperaturas_positivas_B[11]);
        }
    }

        SysCtlDelay(SysCtlClockGet()/80);
}

#endif

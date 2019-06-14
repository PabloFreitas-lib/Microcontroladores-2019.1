#include <stdint.h>
#include <stdbool.h>
#include <math.h>

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
#include "driverlib/ssi.h"

#include <string.h>

//#define rampa
//#define seno
//#define quadrada
//#define triangular
/*
 * Usamos o PA2 e PA5
 */

/**
 * main.c
 */

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int32_t contador;
int UP=0,cont=0,freq=0,amplitude;

float i=0.0;
float seno_t=0.0;
float fRadians = (M_PI/180);

int ordem =0;
unsigned char anterior= ' ',tipo= ' ';

volatile uint8_t command;

//int UP1=0;
//int UP2=0;



void Handler_UART0(void)
{
    // formato,frequencia,amplitude
    //Ex: s,16000,1600
     unsigned char dado = UARTCharGet(UART0_BASE);
     if(anterior == ' ')
     {
         if(dado == 'r')
         {
             tipo = 'r';
         }
         else if(dado == 'q')
         {
             tipo = 'q';
         }
         else if(dado == 's')
         {
             tipo = 's';
         }
         else if(dado == 't')
         {
             tipo = 't';
         }
         else if(dado == ',')
         {
             anterior = ',';
         }
     }

     if(anterior == ',' && cont <= 1)
     {
         if(dado == ',')
         {
             cont++;
         }
         else
         {
             freq *= 10;
             freq = freq+(dado-48);
         }
     }
     else if(anterior == ',' && cont > 1 && dado != 13)
     {
         amplitude *= 10;
         amplitude = amplitude+(dado-48);
     }


     UARTCharPut(UART0_BASE, dado);
     UARTIntClear(UART0_BASE, UART_INT_RX|UART_INT_RT);

     SysTickEnable ();
     SysTickIntEnable();
     SysTickPeriodSet (freq);//40000

 }


void SystickHandler(void)
{

    //#ifdef rampa
    if(tipo == 'r')
    {
        contador+=450;
    }
    //#endif

    //#ifdef quadrada
    else if(tipo == 'q')
    {
        if(UP)
        {
            contador = amplitude;
            UP=0;
        }
        else
        {
            contador =0;
            UP=1;
        }
        //SysCtlDelay(SysCtlClockGet()/250);
    }
    //#endif

    //#ifdef seno
    else if(tipo == 's')
    {
       /* contador = (1 + sinf(fRadians * i))*(amplitude);
        if(!UP1)
        {
            i+=0.5;
            if(i>=90)
            {
                UP1=1;
            }
        }
        else
        {
            i-=0.5;
            if(i<=0)
            {
                UP1=0;
            }
        }*/
        if(i<M_PI)
        {
            seno_t = 1 + sin(i);
            i+=0.01;
        }
        else
        {
            i=-M_PI;
        }
        contador = seno_t*(amplitude/2);
    }
    //#endif

//#ifdef triangular
    else if(tipo == 't')
    {
        if(!UP)
        {
            contador+=50;
            if(contador >=amplitude)
            {
                UP = 1;
            }
        }
        else
        {
            contador-=50;
            if(contador <=0)
            {
                UP = 0;
            }
        }
    }

//#endif
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0x00);
    SSIDataPut(SSI0_BASE, (0x3FFF & contador)); // valor do contador mascarado
    while(SSIBusy(SSI0_BASE)){}
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0xFF);
}




int main(void)
{
    // Passo 2
        SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ); // 80 MHz
        // Passo 3
        SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0); //A2 e A7
        SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
        // Passo 4
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); //TX e SCLK
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); //SYNC
        // Passo 5
        GPIOPinConfigure(GPIO_PA2_SSI0CLK);
        GPIOPinConfigure(GPIO_PA5_SSI0TX);
        GPIOPinConfigure(GPIO_PA0_U0RX);
        GPIOPinConfigure(GPIO_PA1_U0TX);
        // Passo 6
        GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_4);
        // Passo 7
        GPIOPadConfigSet(GPIO_PORTA_BASE,GPIO_PIN_2 | GPIO_PIN_5, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
        GPIOPadConfigSet(GPIO_PORTB_BASE,GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
        GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
        // Passo 8
        GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_5);
        GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0|GPIO_PIN_1);
        // Passo 9
        //while(!SysCtlPeripheralReady(SYSCTL_PERIPH_SSI0)){}
        SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_2, SSI_MODE_MASTER, 2500000, 16);
        UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));

        // Passo 9
        SSIEnable(SSI0_BASE);
        UARTEnable(UART0_BASE);


        UARTIntEnable(UART0_BASE, UART_INT_RX|UART_INT_RT);
        IntEnable(INT_UART0);
        IntMasterEnable();

        UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX4_8);//rx 4 -> 4 bytes de dados
        UARTFIFOEnable(UART0_BASE);

        SysTickEnable ();
        SysTickIntEnable();
        SysTickPeriodSet (freq);//40000
    while(1){}
}

/*
 * DAC8311 -- SSI
 * PB4 -> Sync/CS
 * PA2 --> SCLK
 * PA7 --> DIN
 *
 * Coa max 20MHz
 *
 * Taxa de conversão 12uS -> 83kSa/s
 *
 * Vout = AVdd * Valor_Digital/2^(n_bits)
 * nese caso Valor digital [0,2~14 -1]  e 14 bits
 *
 * SSI Modo 2
 * 16 bits
 *
 * Dado atualizado boarda subida default clock nivel '1'
 */

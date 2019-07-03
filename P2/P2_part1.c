#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "utils/ustdlib.h"
#include "utils/uartstdio.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/tm4c123gh6pm.h"

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
#include "driverlib/timer.h"
#include "driverlib/ssi.h"

//#define rampa
//#define seno
//#define quadrada
//#define triangular

/*
 * Usamos o PA2 e PA5
 */

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int32_t contador;
int UP=0,posicao_uart=0,f_wave=0,amplitude;
float i=0.0;
float seno_aux=0.0;
float fRadians = (M_PI/180);
unsigned char back= ' ',type= ' ';
int erro = 13;

/*
 * Funcoes desenvolvidas depois da main
 */
void check_first(unsigned char data_t);
void check_numbers(unsigned char data_t);
void Handler_UART0(void);
void SystickHandler(void);



/*
 * =======================COMECO DA MAIN ================================================================
 */
int main(void)
{
        // Passo 2
        SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ); // 80 MHz

        // Passo 3
        SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

        // Passo 4
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

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
        SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_2, SSI_MODE_MASTER, 2500000, 16);
        UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));

        // Passo 9
        SSIEnable(SSI0_BASE);
        UARTEnable(UART0_BASE);

        UARTIntEnable(UART0_BASE, UART_INT_RX|UART_INT_RT);
        IntEnable(INT_UART0);
        IntMasterEnable();

        UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX4_8);
        UARTFIFOEnable(UART0_BASE);

        SysTickEnable ();
        SysTickIntEnable();
        SysTickPeriodSet(f_wave);
    while(1){}
}
/*
 * =======================FIM DA MAIN ================================================================
 */

/*
 *
 * ==================================FUNCOES AUXILIARES =================================================
 */
/*
 * Verificacao do tipo de onde
 * s --> seno
 * t --> triangular
 * r --> rampa
 * q -->quadrada
 */
void check_first(unsigned char data_t)
{
    if(back == ' ')
     {
         if(data_t == 'r')
         {
             type = 'r';
         }
         else if(data_t == 'q')
         {
             type = 'q';
         }
         else if(data_t == 's')
         {
             type = 's';
         }
         else if(data_t == 't')
         {
             type = 't';
         }
         else if(data_t == ',')
         {
             back = ',';
         }
     }
}

/*
 * Verifica a frequencia e a amplitude, respectivamente nessa ordem.
 * Frequencia vai direto para o valor do systick e amplitude define o valor maximo que a onda pode chegar em seu respectivo formato
 */
void check_numbers(unsigned char data_t)
{
    if(back == ',' && posicao_uart <= 1)
     {
         if(data_t == ',')
         {
             posicao_uart++;
         }
         else
         {
             f_wave *= 10;
             f_wave += (data_t-48);
         }
     }
     else if( (data_t != erro) &&  (back == ',') && (posicao_uart > 1) )
     {
         amplitude *= 10;
         amplitude +=(data_t-48);
     }
}
/* ==================================FUNCOES AUXILIARES *FIM* =================================================*/

/* ==================================== UART ---- SYSTICK ======================================================*/
/*
 * Recebimento das informacoes via UART
 */
void UART0Handler(void)
{
    UARTIntClear(UART0_BASE, UART_INT_RX|UART_INT_RT);
    // formato,frequencia,amplitude
    //Ex: s,16000,1600

     unsigned char data = UARTCharGet(UART0_BASE);
     check_first(data);
     check_numbers(data);

     UARTCharPut(UART0_BASE, data);

     SysTickEnable ();
     SysTickIntEnable();
     SysTickPeriodSet (f_wave);

 }

/*
 * Base para a periodicidade da onda
 */

void SystickHandler(void)
{

    //#ifdef rampa
    if(type == 'r')
    {
        contador+=450;
    }
    //#endif

    //#ifdef quadrada
    else if(type == 'q')
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
    else if(type == 's')
    {

        if(i<M_PI)
                {
                    seno_aux = 1 + sin(i);
                    i+= 0.01;
                }
                else if(i>M_PI)
                {
                    i = -M_PI;
                }
                contador = seno_aux*(amplitude/2);

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

    }
    //#endif

//#ifdef triangular
    else if(type == 't')
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
/*
 * Parte feita ainda para o SSI
 */
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0x00);
    SSIDataPut(SSI0_BASE, (0x3FFF & contador));

    while(SSIBusy(SSI0_BASE)){}

    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0xFF);
}


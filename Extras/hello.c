#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#define mestre
//#define escravo
//#define catodo
#define anodo

#define ESC_REG(x)                  (*((volatile uint32_t *)(x)))

#define SYSCTL_RCGC2_R              0x400FE108
#define SYSCTL_RCGC2_GPIOF          0x00000020
#define GPIO_PORTF_DIR_R            0x40025400
#define GPIO_PORTF_DEN_R            0x4002551C
#define GPIO_PORTF_DATA_R           0x400253FC
#define GPIO_PORTF_DR2R             0x40025500
#define GPIO_PORTF_PUR              0x40025510

#define SYSCTL_RCGCGPIO             0x400FE608
#define GPIOHBCTL                   0x400FE06C
#define GPIO_O_DIR                  0x400
#define GPIO_O_DR2R                 0x500
#define GPIO_O_DEN                  0x51C
#define GPIO_O_PUR                  0x510
#define GPIO_O_PDR                  0x514
#define GPIO_O_DATA                 0x000

#define portalGPIO_a                0x01
#define portalGPIO_b                0x02
#define portalGPIO_c                0x04
#define portalGPIO_d                0x08
#define portalGPIO_e                0x10
#define portalGPIO_f                0x20

#define portalA_base                0x40004000
#define portalB_base                0x40005000
#define portalC_base                0x40006000
#define portalD_base                0x40007000
#define portalE_base                0x40024000
#define portalF_base                0x40025000

#define pino0                       0x01
#define pino1                       0x02
#define pino2                       0x04
#define pino3                       0x08
#define pino4                       0x10
#define pino5                       0x20
#define pino6                       0x40
#define pino7                       0x80

#define GPIO_O_LOCK                 0x520
#define GPIO_O_CR                   0x524
#define GPIO_LOCK_KEY               0x4C4F434B


#ifdef anodo

unsigned int vector_numbers[17]=      {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,0x03};
unsigned int vector_numbers_part1[17]={0x30,0x00,0x50,0x40,0x60,0x60,0x70,0x00,0x70,0x60,0x70,0x70,0x30,0x50,0x70,0x70,0x00};
unsigned int vector_numbers_part2[17]={0x0F,0x06,0x0B,0x0F,0x06,0x0D,0x0D,0x07,0x0F,0x0F,0x07,0x0C,0x09,0x0E,0x09,0x01,0x03};
unsigned int vector_digits[4]   ={0x8C,0x4C,0xC8,0xC4}; // sinal baixo funciona
unsigned int vector_digits_part1[4]={0x80,0x40,0xC0,0xC0};
unsigned int vector_digits_part2[4]={0x0C,0x0C,0x08,0x04};

int um_minuto_anodo = 3000;
#endif

#ifdef catodo
unsigned int vector_numbers[17]=      {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E,0xFC};
unsigned int vector_numbers_part1[17]={0x00,0x09,0x04,0x00,0x09,0x02,0x02,0x08,0x00,0x00,0x08,0x03,0x06,0x01,0x06,0x0E,0x0C};
unsigned int vector_numbers_part2[17]={0xC0,0xF0,0xA0,0xB0,0x90,0x90,0x80,0xF0,0x80,0x90,0x80,0x80,0xC0,0xA0,0x80,0x80,0xF0};

unsigned int vector_digits[5]={0x40,0x80,0x04,0x08,0xCC}; // sinal alto funciona
unsigned int vector_digits_part1[4]={0x40,0x80,0x00,0x00};
unsigned int vector_digits_part2[4]={0x00,0x00,0x04,0x08};

int um_minuto_catodo = 1500;
#endif

const float timer_duvidoso_mili_80MHz = 3800000;  // ~um segundo
const float timer_doopler = 0.35;



void delay_system(float mS)
{

   mS = (mS/1000) * timer_duvidoso_mili_80MHz;
   while(mS > 0)
   mS--;
}

void habilita_clockGPIO(uint32_t portalGPIO)
{
    ESC_REG(SYSCTL_RCGCGPIO)|=portalGPIO;
    //nao jogar fora o conteudo anterior
}


void configuraPino_saida(uint32_t portal, uint8_t pino)
{
    ESC_REG(portal+GPIO_O_DIR)|=pino;
    ESC_REG(portal+GPIO_O_DR2R)|=pino;
    ESC_REG(portal+GPIO_O_DEN)|=pino;
}

void configuraPino_entrada(uint32_t portal, uint8_t pino)
{
    ESC_REG(portal+GPIO_O_DIR) &=~(pino);//inverso
    ESC_REG(portal+GPIO_O_DR2R)|=pino;
    ESC_REG(portal+GPIO_O_DEN)|=pino;
}

uint32_t GPIO_Leitura(uint32_t portal, uint8_t pino)
{
    return (ESC_REG(portal + (GPIO_O_DATA+(pino<<2))));
}

void GPIO_escrita(uint32_t portal, uint8_t pino, uint8_t valor)
{
    ESC_REG(portal + (GPIO_O_DATA+(pino<<2)))=valor;
}

void numero(int i)
{
    GPIO_escrita(GPIO_PORTE_BASE, pino0|pino1|pino2|pino3, vector_numbers[i]);
    GPIO_escrita(GPIO_PORTC_BASE, pino4|pino5|pino6|pino7, vector_numbers[i]);
}
void digito(int i)
{
    GPIO_escrita(GPIO_PORTB_BASE, pino6|pino7, vector_digits[i]);
    GPIO_escrita(GPIO_PORTD_BASE, pino3|pino2, vector_digits[i]);
}


#ifdef anodo
void pontos_intermitentes(void)
{
       GPIO_escrita(GPIO_PORTB_BASE, pino6|pino7, pino6|pino7);
       GPIO_escrita(GPIO_PORTD_BASE, pino3|pino2|pino6, pino3|pino2|~pino6);
       GPIO_escrita(GPIO_PORTE_BASE, pino0|pino1|pino2, (pino0)|(pino1)|~(pino2));
}
#endif


#ifdef catodo
void pontos_intermitentes(void)
{
       GPIO_escrita(GPIO_PORTB_BASE, pino6|pino7, 0x00|0x00);
       GPIO_escrita(GPIO_PORTD_BASE, pino3|pino2|pino6, 0x00|0x00|pino6);
       //GPIO_escrita(portalE_BASE, pino0|pino1|pino2, ~pino0|~pino1|pino2);

       GPIO_escrita(GPIO_PORTE_BASE, pino0|pino1|pino2|pino3, ~pino0|~pino1|~pino2|~pino3);
       GPIO_escrita(GPIO_PORTC_BASE, pino4|pino5|pino6|pino7, ~pino4|~pino5|~pino6|~pino7);
}
#endif

#ifdef anodo
void limpa_diplay(void)
{
    GPIO_escrita(GPIO_PORTB_BASE, pino6|pino7, 0x00|0x00);
    GPIO_escrita(GPIO_PORTD_BASE, pino3|pino2|pino6, 0x00|0x00|pino6);
    GPIO_escrita(GPIO_PORTE_BASE, pino0|pino1|pino2|pino3, 0x00|0x00|0x00|0x00);
    GPIO_escrita(GPIO_PORTC_BASE, pino4|pino5|pino6|pino7, 0x00|0x00|0x00|0x00);
}
#endif

#ifdef catodo

void limpa_diplay(void)
{
    GPIO_escrita(GPIO_PORTB_BASE, pino6|pino7, pino6|pino7);
    GPIO_escrita(GPIO_PORTD_BASE, pino3|pino2|pino6, pino3|pino2|0x00);
    GPIO_escrita(GPIO_PORTE_BASE, pino0|pino1|pino2|pino3, pino0|pino1|pino2|pino3);
    GPIO_escrita(GPIO_PORTC_BASE, pino4|pino5|pino6|pino7, pino4|pino5|pino6|pino7);
}

#endif



void unlock_GPIO(uint32_t portal)
{
       ESC_REG(portal + GPIO_O_LOCK) = GPIO_LOCK_KEY;
       ESC_REG(portal + GPIO_O_CR) = 0x01;
}

void lock_GPIO(uint32_t portal)
{
    ESC_REG(portal + GPIO_O_LOCK) = 0;
}

void escreve_4_digitos(int n4,int n3,int n2,int n1)
{

                                        // [ - - - n1]
                                       digito(3);
                                       numero(n1);
                                       delay_system(timer_doopler);

                                       // [ - - - -]
                                       limpa_diplay();
                                       delay_system(timer_doopler);

                                       // [ - - n2 -]

                                       digito(2);
                                       numero(n2);
                                       delay_system(timer_doopler);

                                       // [ - - - -]
                                       limpa_diplay();
                                       delay_system(timer_doopler);

                                       // [ - n3 - -]
                                       digito(1);
                                       numero(n3);
                                       delay_system(timer_doopler);

                                       // [ - - - -]
                                       limpa_diplay();
                                       delay_system(timer_doopler);

                                       // [ n4 - - -]
                                       digito(0);
                                       numero(n4);
                                       delay_system(timer_doopler);

                                       // [ - - - -]
                                       limpa_diplay();
                                       delay_system(5);
}

void relogio(int n4,int n3,int n2,int n1){


int j=0;
            while(n4<3) // dezena hora 0 - 2
            {
                if(n3==10 && (n4 == 0 || n4 == 1) )
                {
                n4++;
                n3=0;
                }
                else if (n4 == 2 && n3==5)
                {
                    n4++;
                }
                while(n3<10) //unidade hora 0 - 9
                {
                    if(n2==7)
                    {
                      n3++;
                      n2=0;
                    }
                   while(n2<7) //minuto dezena 0 - 6
                   {
                       if(n1==10)
                       {
                           n2++;
                           n1=0;
                       }
                       while(n1<10) // minuto unidade 0 - 9
                       {
                         // display [ - - n2 n2 ]
                         #ifdef catodo
                            for (j=0; j<um_minuto_catodo; j++)
                         #endif

                         #ifdef anodo
                            for (j=0; j<um_minuto_anodo; j++)
                         #endif
                         {
                           escreve_4_digitos(n4, n3, n2, n1);

                           #ifdef anodo
                               if(j%25 == 0)
                               {
                                   pontos_intermitentes();
                                   delay_system(timer_doopler);

                                   limpa_diplay();
                                   delay_system(timer_doopler);
                               }
                            #endif

                            #ifdef catodo

                            #endif
                         }
                         n1++; // fim do while n1
                         }
                      }// fim do while n2
                   }// fim do while n3
                } // fim do while n4
            n1=0,n2=0,n3=0,n4=0; // recomeco
}
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}
#ifdef mestre

int le_numero_part1(uint8_t valor)
{
    return(GPIO_Leitura(GPIO_PORTE_BASE, pino0|pino1|pino2|pino3) == valor);
}
int le_numero_part2(uint8_t valor)
{
    return(GPIO_Leitura(GPIO_PORTC_BASE, pino4|pino5|pino6|pino7) == valor);
}
void le_numero(int i)
{

    if( le_numero_part1(vector_numbers_part1[i])  && le_numero_part2(vector_numbers_part2[i]) )
    {
        UARTprintf("Correto :Numero %d\n",i);
        delay_system(1000);
    }
    else
    {
        UARTprintf("Errado :Numero %d\n",i);
                delay_system(1000);
    }
}
int le_digito_parte1(uint8_t valor)
{
   return(GPIO_Leitura(GPIO_PORTB_BASE, pino6|pino7) ==  valor);
}

int le_digito_parte2(uint8_t valor)
{
   return(GPIO_Leitura(GPIO_PORTD_BASE, pino2|pino3) ==  valor);
}
int le_digito(int i)
{
    if( le_digito_parte1(vector_digits_part1[i]) && le_digito_parte2(vector_digits_part2[i]))
    {
        UARTprintf("Certo :Digito %d\n",i);
        return(1);
    }
    else
    {
        UARTprintf("Errado :Digito %d\n",i);
        return(0);
    }
}
#endif
//*****************************************************************************
//
// Print "Hello World!" to the UART on the evaluation board.
//
//*****************************************************************************
int
main(void)
{
    volatile uint32_t ui32Loop;

    //
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    //
    //ROM_FPULazyStackingEnable();

    //
    // Set the clocking to run directly from the crystal.
    //
    //ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       //SYSCTL_OSC_MAIN);

    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    habilita_clockGPIO(portalGPIO_e|portalGPIO_c|portalGPIO_d | portalGPIO_b| portalGPIO_f);

    ui32Loop = ESC_REG(SYSCTL_RCGC2_R);

    //
    // Enable the GPIO pins for the LED (PF2 & PF3).
    //
#ifdef mestre
    unlock_GPIO(GPIO_PORTF_BASE);

    configuraPino_entrada(GPIO_PORTC_BASE,pino4|pino5|pino6|pino7);
    ESC_REG(GPIO_PORTC_BASE+GPIO_O_PDR)|= pino4|pino5|pino6|pino7;

    configuraPino_entrada(GPIO_PORTE_BASE,pino0|pino1|pino2|pino3);
    ESC_REG(GPIO_PORTE_BASE+GPIO_O_PDR)|= pino0|pino1|pino2|pino3;

    configuraPino_entrada(GPIO_PORTB_BASE, pino6|pino7);
    ESC_REG(GPIO_PORTB_BASE+GPIO_O_PDR)|= pino6|pino7;

    configuraPino_entrada(GPIO_PORTD_BASE, pino2|pino3|pino6);
    ESC_REG(GPIO_PORTD_BASE+GPIO_O_PDR)|= pino2|pino3|pino6;

    configuraPino_entrada(GPIO_PORTF_BASE, pino2|pino3|pino1);
    ESC_REG(GPIO_PORTF_BASE+GPIO_O_PDR)|= pino2|pino3|pino1;

    configuraPino_saida(GPIO_PORTF_BASE,pino0|pino4);

    lock_GPIO(GPIO_PORTF_BASE);

    // Initialize the UART.
    //
    ConfigureUART();
#endif

#ifdef escravo
        configuraPino_saida(GPIO_PORTC_BASE,pino4|pino5|pino6|pino7);
        configuraPino_saida(GPIO_PORTE_BASE,pino0|pino1|pino2|pino3);
        configuraPino_saida(GPIO_PORTB_BASE, pino6|pino7);
        configuraPino_saida(GPIO_PORTD_BASE, pino2|pino3|pino6);

        /*unlock_GPIO(GPIO_PORTF_BASE);
        configuraPino_entrada(GPIO_PORTF_BASE,pino0|pino4);
        ESC_REG(GPIO_PORTF_BASE+GPIO_O_PUR) |= pino0|pino4;
        lock_GPIO(GPIO_PORTF_BASE);*/
#endif

    while(1)
    {
#ifdef mestre
        //if(le_digito(0))
        //{
            le_numero(1);
        //}
#endif

#ifdef escravo
        for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++){}
            relogio(1, 5, 4, 1);

#endif
    }
}

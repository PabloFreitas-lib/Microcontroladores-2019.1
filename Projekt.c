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

#define ESC_REG(x)                  (*((volatile uint32_t *)(x)))


//serve para da permissao de uso do pino0 do portalF
#define GPIO_O_LOCK                 0x520
#define GPIO_O_CR                   0x524
#define GPIO_LOCK_KEY               0x4C4F434B



int i=0,j=0;

int btw_towers = 21;    // Espacamento entre cada torre de 21 pixels
int dG = 22;            // Disco grande de 22 pixels
int dM = 16;            // Disco medio 16 pixels
int dP = 10;            // Disco pequeno 10 pixels

/* Posicoes no eixo y da torre
 *
 * --> 38-39 |     |       |   top
 * --> 40-41 |     |       |   middle
 * --> 42-43 |     |       |   bottle
 *    ========================
 *    Altura de cada torre [23-43] pixels
 */
int bottle = 42;
int middle = 40;
int top    = 38;

// unlock GPIOLOCK register using direct register programming

void unlock_GPIO(uint32_t portal)
{
       ESC_REG(portal + GPIO_O_LOCK) = GPIO_LOCK_KEY;
       ESC_REG(portal + GPIO_O_CR) = 0x01;
}

void lock_GPIO(uint32_t portal)
{
    ESC_REG(portal + GPIO_O_LOCK) = 0;
}

void pinos_Led_RGB(void)
{
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1);
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

void configura_botoes(void)
{
    unlock_GPIO(GPIO_PORTF_BASE);

    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);

    lock_GPIO(GPIO_PORTF_BASE);
}

void Systickhandler(void)
{
    LedAzul();
}

void Handle_PortalFInt(void)
{
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);

    LedVerde();

    if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)!= GPIO_PIN_0 )
    {
    LedVerde();
    }
    if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)!= GPIO_PIN_4 )
    {
    LedVermelho();
    }
}

void Clock_A_F(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}
/*------------------------------- Funcoes para o jogo -----------------------------*/
void torre(int posicao_y,int tamanho,int torre)
{
    for(j=(btw_towers - (tamanho/2))+(btw_towers*torre); j <= ((btw_towers + (tamanho/2)) + (btw_towers*torre)); j++)
    Nokia5110_SetPxl(posicao_y,j);

    for(j=(btw_towers - (tamanho/2))+(btw_towers*torre); j <= ((btw_towers + (tamanho/2)) + (btw_towers*torre)); j++)
    Nokia5110_SetPxl(posicao_y+1,j);
}

void discos_iniciais(void)
{
    // Disco grande
    int torre =0;
    int tamanho=dG;
    int posicao_y = bottle;
    for(j=(btw_towers - (tamanho/2))+(btw_towers*torre); j <= ((btw_towers + (tamanho/2)) + (btw_towers*torre)); j++)
    Nokia5110_SetPxl(posicao_y,j);

    for(j=(btw_towers - (tamanho/2))+(btw_towers*torre); j <= ((btw_towers + (tamanho/2)) + (btw_towers*torre)); j++)
    Nokia5110_SetPxl(posicao_y+1,j);

    // Disco Medio
    torre =0;
    tamanho=dM;
    posicao_y = middle;
    for(j=(btw_towers - (tamanho/2))+(btw_towers*torre); j <= ((btw_towers + (tamanho/2)) + (btw_towers*torre)); j++)
    Nokia5110_SetPxl(posicao_y,j);

    for(j=(btw_towers - (tamanho/2))+(btw_towers*torre); j <= ((btw_towers + (tamanho/2)) + (btw_towers*torre)); j++)
    Nokia5110_SetPxl(posicao_y+1,j);

    // Disco Pequeno
    torre =0;
    tamanho=dP;
    posicao_y = top;
    for(j=(btw_towers - (tamanho/2))+(btw_towers*torre); j <= ((btw_towers + (tamanho/2)) + (btw_towers*torre)); j++)
    Nokia5110_SetPxl(posicao_y,j);

    for(j=(btw_towers - (tamanho/2))+(btw_towers*torre); j <= ((btw_towers + (tamanho/2)) + (btw_towers*torre)); j++)
    Nokia5110_SetPxl(posicao_y+1,j);
}

/*
 * Essa funcao ainda precisa saber onde esta o disco antes do movimento para apaga-lo do display
 */
void move_disco(int posicao_y,int tamanho,int torre)
{
    for(j=(btw_towers - (tamanho/2))+(btw_towers*torre); j <= ((btw_towers + (tamanho/2)) + (btw_towers*torre)); j++)
    Nokia5110_SetPxl(posicao_y,j);

    for(j=(btw_towers - (tamanho/2))+(btw_towers*torre); j <= ((btw_towers + (tamanho/2)) + (btw_towers*torre)); j++)
    Nokia5110_SetPxl(posicao_y+1,j);
}

/*
 * Restaura as torre que parte é apagada ao apagar os discos durante o movimento
 */
void draw_torre(void)
{
    for(j=23;j<=43;j++)
    {
        Nokia5110_SetPxl(j, btw_towers);
        Nokia5110_SetPxl(j, btw_towers*2);
        Nokia5110_SetPxl(j, btw_towers*3);
    }
}

/*
 * Limpa do disco da sua antiga posicao para colocar na posicao nova
 */
void disco_clear(int posicao_y,int tamanho,int torre)
{
    for(j=(btw_towers - (tamanho/2))+(btw_towers*torre); j <= ((btw_towers + (tamanho/2)) + (btw_towers*torre)); j++)
        Nokia5110_ClrPxl(posicao_y,j);

    for(j=(btw_towers - (tamanho/2))+(btw_towers*torre); j <= ((btw_towers + (tamanho/2)) + (btw_towers*torre)); j++)
        Nokia5110_ClrPxl(posicao_y+1,j);

    draw_torre();
}

void inicializa_torre_hanoi(void)
{
    // Primeira imagem das torres e bancada
    Nokia5110_PrintBMP2(0, 0, torre_hanoi_Bitmap, 84, 48);
    //ilustracao dos discos utilizando o SetPixel
    discos_iniciais();

    Nokia5110_DisplayBuffer();
}


int main(void){

    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    //Habilita Clock do Portais A ate o F
    //Clock_A_F();
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);


    //Display inicializacao
    Nokia5110_Init();
    Nokia5110_Clear();

    inicializa_torre_hanoi();

    //Botoes
    //configura_botoes();

    //Pinos
    //pinos_Led_RGB();

    //SysCtlDelay(20000000);

    //disco_clear(bottle, dG, 0);
    //torre(bottle, dG, 1);
    //Nokia5110_DisplayBuffer();

    //LedAzul();

    //SysTickPeriodSet(16);

    //IntMasterEnable();

    //SysTickEnable();
    //SysTickIntEnable();


    // Interrupcao Portais

    //IntEnable(INT_GPIOF);
    //GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_4, GPIO_FALLING_EDGE);
    //GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_4);

    while(1)
    {

       // if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)!= GPIO_PIN_0 )
       //{
       //LedVerde();
       //}
       //if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)!= GPIO_PIN_4 )
       //{
       //LedVermelho();
       //}
    }
}



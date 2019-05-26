#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/debug.h"

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

#define ESC_REG(x)                  (*((volatile uint32_t *)(x)))


#define catodo

//serve para da permissao de uso do GPIO_PIN_0 do portalF
#define GPIO_O_LOCK                 0x520
#define GPIO_O_CR                   0x524
#define GPIO_LOCK_KEY               0x4C4F434B


#ifdef anodo
                                                                                                                 //U
unsigned int vector_numbers[18]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,0x3E,0x31};
unsigned int vector_numbers_dp[10]={~0x50,~0x79,~0x24,~0x30,~0x19,~0x12,~0x02,~0x78,~0x00,~0x10};
unsigned int vector_digits[4]={0x8C,0x4C,0xC8,0xC4}; // sinal baixo funciona
int um_minuto_anodo = 3000;


#endif

#ifdef catodo

unsigned int vector_numbers[18]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E,~0x3E,~0x31}; // sem o DP ( pontinho)
unsigned int vector_numbers_dp[10]={0x50,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};
unsigned int vector_digits[4]={0x40,0x80,0x04,0x08}; // sinal alto funciona
int um_minuto_catodo = 1500;

#endif

// **** VAR Globais ************************
int systick_cronometro = 100000,systick_relogio = 16000000,systick_debouncer_number =150000;

const float timer_duvidoso_mili_80MHz = 3800000;  // ~um segundo
const float timer_doopler = 0.35;

//usado para varredura da matrix de botao se os botoes que representa as linhas estivem de PULLUP
int vector_matrix[5] = {0x23,0x32,0x31,0x11};
unsigned int n1=0,n2=0,n3=0,n4=0;
// **** FIM Var Globais ***********************


//----------------------------------------------FUNCOES GERAIS-------------------------------------------------------------------

void delay_system(float mS)
{
   mS = (mS/1000) * timer_duvidoso_mili_80MHz;
   while(mS > 0)
   mS--;
}

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

//----------------------------------------------FUNCOES GERAIS [FIM]-------------------------------------------------------------------


//----------------- DISPLAY 7 SEGMENTOS -------------------------------------
void numero(int i)
{
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, vector_numbers[i]);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, vector_numbers[i]);
}
void numero_dp(int i)
{
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, vector_numbers_dp[i]);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, vector_numbers_dp[i]);
}

void digito(int i)
{
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6|GPIO_PIN_7, vector_digits[i]);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3|GPIO_PIN_2, vector_digits[i]);

    #ifdef anodo
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6,GPIO_PIN_6);
    #endif

    #ifdef catodo
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6,~GPIO_PIN_6);
    #endif
}

#ifdef anodo


void limpa_digito(void)
{
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_6|GPIO_PIN_7);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3|GPIO_PIN_2, GPIO_PIN_3|GPIO_PIN_2);
}


void limpa_diplay(void)
{
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6|GPIO_PIN_7, 0x00|0x00);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_6, 0x00|0x00|GPIO_PIN_6);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00|0x00|0x00|0x00);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0x00|0x00|0x00|0x00);
}

#endif

#ifdef catodo


void limpa_digito(void)
{

    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6|GPIO_PIN_7, ~GPIO_PIN_6|~GPIO_PIN_7);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3|GPIO_PIN_2, ~GPIO_PIN_3|~GPIO_PIN_2);
}


void limpa_diplay(void)
{
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_6|GPIO_PIN_7);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_6, GPIO_PIN_3|GPIO_PIN_2|~GPIO_PIN_6);
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
}
#endif

void escreve_temperatura(int n4,int n3,int n2,int n1)
{
    // [ - - - n1]
    digito(3);
    numero(12);
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
    numero_dp(n3);
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
    delay_system(timer_doopler);
}

void clock_display(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
}

void pinos_display(void)
{
    //Pinos do display
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_6|GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6);
}

void temperatura_calc(uint32_t temperatura)
{
    n4 = temperatura/10;
    n3 = temperatura%10;
}
int main(void)
{
    uint32_t ui32ADC0Value[4];
    volatile uint32_t ui32TempAvg;
    volatile uint32_t ui32TempValueC;
    volatile uint32_t ui32TempValueF;

        SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

        SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

        //habilita_clockGPIO(portalGPIO_e|portalGPIO_c|portalGPIO_d | portalGPIO_b| portalGPIO_f);
        clock_display();

        //Pinos do display
        pinos_display();

        ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
        ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
        ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
        ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
        ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);

    ADCSequenceEnable(ADC0_BASE, 1);
    while(1)
    {
        ADCIntClear(ADC0_BASE, 1);
        ADCProcessorTrigger(ADC0_BASE, 1);

        while(!ADCIntStatus(ADC0_BASE, 1, false)){}


        ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);

        ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
        ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
        ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;
        temperatura_calc(ui32TempValueC);
        escreve_temperatura(n4, n3, n2, n1);
    }
}

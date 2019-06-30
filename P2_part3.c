#include "bib.h"


void ADC0handler(void);
void Systickhandler(void);
void UART0Handler(void);


int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    //Nokia
    Nokia5110_Init();
    Nokia5110_Clear();
    Nokia5110_ClearBuffer();

    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);// parao pwm dos leds
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);//para os leds
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);//para os leds
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);// para a temperatura

    //Uart
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);


    IntMasterEnable(); // todas as interrupt
    ADC_config();


    // PWM
    GPIOPinConfigure(GPIO_PF1_M1PWM5);
    GPIOPinConfigure(GPIO_PF2_M1PWM6);
    GPIOPinConfigure(GPIO_PF3_M1PWM7);

    //UART
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD); // Uart

    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0|GPIO_PIN_1); // UART
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); // PWM

    //UART
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));

    //PWM
    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    //PWM
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, 320);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, 320);

    //PWM
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, 255);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, 255);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, 255);

    //PWM
    PWMGenEnable(PWM1_BASE, PWM_GEN_2);
    PWMGenEnable(PWM1_BASE, PWM_GEN_3);

    //PWM
    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT | PWM_OUT_6_BIT | PWM_OUT_7_BIT, true);

    UARTEnable(UART0_BASE);
    UARTIntEnable(UART0_BASE, UART_INT_RX|UART_INT_RT);


    IntEnable(INT_UART0);
    IntEnable(INT_ADC0SS0);
    ADCIntEnable(ADC0_BASE, 0);



    //UART
    UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX4_8);
    UARTFIFOEnable(UART0_BASE);
    IntMasterEnable();

    //SysTickIntEnable();
    //SysTickEnable();
    //SysTickPeriodSet(0xFFFFFF);


    while(1)
    {
        if(uart_recebida == 1)
        {
            //IntMasterDisable();
            flow();
        }

        // temperatura
        ADCProcessorTrigger(ADC0_BASE, 0);
        // Passo 11
        ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3]+ ui32ADC0Value[4] + ui32ADC0Value[5] + ui32ADC0Value[6] + ui32ADC0Value[7] + 4)/8;

        // Passo 12
        ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096);

        //Tarefa 1 lab6
        temperatura_calc(ui32TempValueC);

        if(++ctd == 200)
        {
            verifica_temp();
            ctd=0;
        }
    }
}

void UART0Handler(void)
{
    UARTIntClear(UART0_BASE, UART_INT_RX|UART_INT_RT);
    unsigned char aux_char;
    aux_char = UARTCharGet(UART0_BASE);
    UARTCharPut(UART0_BASE, aux_char);
    if(aux_char == 'p')
    uart_recebida =1;
    if(aux_char == 's')
    uart_recebida =0;

}

void ADC0handler(void)
{
    // clear the interrupt flag, grab the data, set the 'done' flag
    ADCIntClear(ADC0_BASE, 0);
    // Passo 10
   // ( modulo | seq | Endereco de onde armazenar)
   ADCSequenceDataGet(ADC0_BASE, 0, ui32ADC0Value);
   //verifica_temp();
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

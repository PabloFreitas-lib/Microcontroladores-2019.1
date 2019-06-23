#include "bib.h"

/*
 * Maximo do termometro [23,9] minimo [23,24]
 *
 */
/*
 * 14 até 29
 */

void ADC0handler(void);
void Systickhandler(void);
void Timer0IntHandler(void);

int main(void)
{
       SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
       Nokia5110_Init();
       Nokia5110_Clear();
       Nokia5110_ClearBuffer();

       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

       SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
       SysCtlDelay(2);

       define_hoarario(1, 2, 3, 7);
       IntMasterEnable();

       ADC_config();
       pinos_Led_RGB();

       SysTickPeriodSet(0xFFFFFF);

        SysTickEnable();
        SysTickIntEnable();
        IntEnable(INT_ADC0SS0);
        ADCIntEnable(ADC0_BASE, 0);
       //Primeira parte da Tela
        if(qual_tela == 0)
            verifica_horario();

#ifdef tela2
        //Faz parte da segunda Tela
        Nokia5110_PrintBMP2(0, 0, termometro, 84, 48);
        Nokia5110_DisplayBuffer();
#endif

#ifdef tela3
        Nokia5110_OutString("Dallas 18B203Temp Sensor            ");
        //Nokia5110_OutString("T1:");Nokia5110_OutChar('7');Nokia5110_OutChar('7');Nokia5110_OutChar('.');Nokia5110_OutChar('7');Nokia5110_OutString("    C");
        max_temp_display(143);
        Nokia5110_OutString("T2:");Nokia5110_OutChar('7');Nokia5110_OutChar('7');Nokia5110_OutChar('.');Nokia5110_OutChar('7');Nokia5110_OutString("    C");
        Nokia5110_OutString("T3:");Nokia5110_OutChar('1');Nokia5110_OutChar('4');Nokia5110_OutChar('.');Nokia5110_OutChar('1');Nokia5110_OutString("    C");
#endif

       while(1)
       {
           ADCProcessorTrigger(ADC0_BASE, 0);
           // Passo 11
           ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3]+ ui32ADC0Value[4] + ui32ADC0Value[5] + ui32ADC0Value[6] + ui32ADC0Value[7] + 4)/8;

           // Passo 12
           ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096);

           //Tarefa 1 lab6
           temperatura_calc(ui32TempValueC);
       }
}

// this is the interrupt routine for the ADC
void ADC0handler(void)
{
    // clear the interrupt flag, grab the data, set the 'done' flag
    ADCIntClear(ADC0_BASE, 0);
    // Passo 10
   // ( modulo | seq | Endereco de onde armazenar)
   ADCSequenceDataGet(ADC0_BASE, 0, ui32ADC0Value);
//#ifdef tela2
   verifica_temp();
//#endif

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

//#ifdef tela1
   ctd_relogio++;
    if(ctd_relogio == 470)
    {
        ctd_relogio=0;
        m2++;
            relogio();
    }

    if(ctd_tela3==100 )
    {
        if(qual_tela ==2)
        {
            Nokia5110_Clear();
            Nokia5110_ClearBuffer();
            Nokia5110_OutString("Dallas 18B203Temp Sensor            ");
        max_temp_display(temp_max);
        min_temp_display(temp_min);
        atual_temp_display(temp_atual);
        }
    }
    ctd_tela3--;
    if(ctd_tela3 < 0)
    {
        ctd_tela3 = 100;
    }

    if(ctd_tela1 == 100)
    {
        if(qual_tela == 0)
            verifica_horario();
    }
    ctd_tela1--;
    if(ctd_tela1 < 0)
        ctd_tela1 = 100;


    ctd_trasicao++;
        if(ctd_trasicao == 47)
        {
            ctd_trasicao = 0;
            ++qual_tela;
            if(qual_tela == 3)
            {
                qual_tela =0;
                verifica_horario();
                //LedVerde();
            }
        }
}


// Interrupts
void Timer0IntHandler(void)
{
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    LedAzul();
}



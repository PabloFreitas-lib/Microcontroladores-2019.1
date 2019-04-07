/*
 * _________________________________

The execution speed of a microcontroller is determined by an external crystal.
EK-TM4C123GXL have a 16MHz crystal.

Most microcontrollers include a Phase-Lock-Loop (PLL) that allows software to adjust the excecution speed of the computer.

Slowing down the bus clock will require less power to operate and generates less heat.
Speeding up the bus clock allows for more calculations per second, at the cost of more power to operate, generating more heat.

The default bus speed for the internal oscillator is 16MHz. The internal oscillator is less precise than the external one,
but requires less power and does not need an crystal.

If we wish to have accurate control of time, we will activate the external crystal (main oscillator) and use the PLL to select the desired bus speed.

There is an register called OSCSRC (Oscillator Source), where an oscillator is chosen for the clock.
The clock source will be the Main Oscilator (with the crystal frequency clock) setting the OSCSRC bit to 0.

CLOCK CONFIGURATION

RCC [p. 220]
The control for the system clock is provided by the Run-Mode Clock Configuration (RCC and RCC2).
The RCC2 is provided for a larger assortment of clock configuration options.

In the RCC register, there is the XTAL bits, wich defines the Crystal Frequency in MHz.
[p. 265] Setting XTAL to 10101 (0x15) will make the 400MHz output of the Voltage Controlled Oscillator (VCO) yield in a 16MHz clock in the input of the Phase/Frequency Detector.
 If the clock is too slow, the detector will add to the charge pump, increasing the input to the VCO. If the clock is too fast, the detector will subtract from the charge pump,
 decreasing the input to the VCO.

In the RCC2 register, there is the SYSDIV2 bits [p. 223], with defines a division factor value. It can be set from 4 to 124, making the clock operate in a frequency less than 400MHz.
 For example, if you chose 4, the response will be 400/(4+1) = 80MHz.

Choosing the XTAL changes the precision of the external crystal. Choosing the SYSDIV changes the velocity of the system.

ACCURATE TIME DELAYS USING SYSTICK

The SYSTICK is a value that decreases by 1 every 12.5ns (if the clock is running at 80MHz).
The RELOAD register is set to the number of bus cycles one wishes to wait.
CURRENT will clear the counter and will clear the count flag of the CTRL register.
After SysTick has been decremented 'delay' times, the count flag will be set and the while loop will terminate.
The maximum time one can wait is about 200ms. To provide longer delays, it is possible to call SysTick_Wait repeatedly.

*/


#include <stdint.h>

//#define mili
//#define micro
#define delay_SYSTICK

#define ESC_REG(x) (*((volatile uint32_t *)(x)))

#define SYSCTL_RCGC2_R              0x400FE108
#define SYSCTL_RCGC2_GPIOF          0x00000020

#define GPIO_PORTF_DIR_R            0x40025400
#define GPIO_PORTF_DEN_R            0x4002551C
#define GPIO_PORTF_DATA_R           0x400253FC

#define RCC                         0x400FE060
#define RCC2                        0x400FE070
#define RCC_bypass                  1<<11 // ou 0x00000800
#define RCC_USESYSDIV               1<<22 // ou 0x00400000

#define RCC2_bypass2                1<<11 // ou 0x00000800
#define RCC_XTALM                   0x7C0 // Crystal Value. This field specifies the crystal value attached to the main oscillator.
#define RCC_PIOSC                   0x10
#define RCC_OSCSRC                  0x30 // Selects the input source for the OSC. The values are: MOSC || PIOSC || PIOSC/4 || LFIOSC. For more see RCC2 register.
#define RCC_XTAL_16MHZ              0x540 // 0x15<<6
#define RCC2_USERCC2                1<<31 // 0x80000000
#define RCC2_OSCSRCM                7<<4 // 0x70
#define RCC_PWRDN                   1<<13 // 0x2000
#define MISC                        0x400FE058
#define INT_PLL_LOCK                0x40

#define RCC_SYSDIV_M                0x07800000 // 0x1F<<23     System Clock Divisor:
#define RCC_SYSDIV_1                0x00000000  // divide por 1
#define RCC_SYSDIV_2                0x00800000  // divide por 2
#define RCC_SYSDIV_3                0x01000000  // divide por 3
#define RCC_SYSDIV_4                0x01800000  // divide por 4
#define RCC_SYSDIV_5                0x02000000  // divide por 5
#define RCC_SYSDIV_6                0x02800000  // divide por 6
#define RCC_SYSDIV_7                0x03000000  // divide por 7
#define RCC_SYSDIV_8                0x03800000  // divide por 8
#define RCC_SYSDIV_9                0x04000000  // divide por 9
#define RCC_SYSDIV_10               0x04800000  // divide por 10
#define RCC_SYSDIV_11               0x05000000  // divide por 11
#define RCC_SYSDIV_12               0x05800000  // divide por 12
#define RCC_SYSDIV_13               0x06000000  // divide por 13
#define RCC_SYSDIV_14               0x06800000  // divide por 14
#define RCC_SYSDIV_15               0x07000000  // divide por 15
#define RCC_SYSDIV_16               0x07800000  // divide por 16
#define RCC_SYSDIV_50               0x18800000  // divide por 50
#define RCC_SYSDIV_64               0x1F800000  // divide por 64

#define RCC2_SYSDIV_M               0x1F800000 // Procurar como entender a tabela 5-4 pagina 223
#define RCC2_DIV400                 1<<30        // Divide PLL as 400 MHz versus 200 MHz
#define RIS                         0x400FE050

#define config_80MHZ                0xC1000000 //RCC2
#define config_50MHZ                0x01C00000 // Divisao por 4 e usar o sysdiv
#define config_20MHZ                0x04C00000 // Divisao por 10 e usar o sysdiv
#define config_4MHZ                 0x98C00000 //RCC2 divisao por 50
#define config_3MHZ                 0x9FC00000 //RCC2 divisao maxima(64) pag 224

const float timer_duvidoso_mili = 10004000;
const float timer_duvidoso_micro = 10400000000;




//Defines for precise delay

#define NVIC_ST_CTRL_R      0xE000E010  // SysTick Control and Status Register [p. 138]
#define NVIC_ST_RELOAD_R    0xE000E014  // SysTick Reload Value Register [p. 140]
#define NVIC_ST_CURRENT_R   0xE000E018  // SysTick Current Value Register [p. 141]
#define NVIC_ST_CTRL_COUNT  0x00010000  // Count Flag

#define OSCSRC_MOSC         0x0         // main oscillator
#define OSCSRC_PIOSC        0x1         // precision internal oscillator (default)
#define OSCSRC_PIOSC4       0x2         // PIOSC/4
#define OSCSRC_LFIOSC       0x3         // low-frequency internal oscillator

/*
 * 219 - Clock Control( todos os tipos de clock)
 *
 *
 *
 *
 */
// diferem no bit mais significativo

// RCC difere do RCC2 por tamanho
void config_Clock(uint32_t config)
{
                        //Passo 1:
    /*
     * Bypass the PLL and system clock divider by setting the BYPASS bit and clearing the USESYS bit in the RCC register,
     * thereby configuring the microcontroller ro run off a "raw" clock source and allowing for the new PLL configuration
     *  to be validated before switching the system clock to the PLL
     */

    uint32_t RCC_temp,RCC2_temp,delay;
    //Escreve oq ja foi feito no registrador
    RCC_temp = ESC_REG(RCC);
    RCC2_temp = ESC_REG(RCC2);


    // Bypass PLL e limpar divisores de clock
    RCC_temp |= RCC_bypass; // The system clock is derived from the OSC source and divided by the divisor specified by SYSDIV.
    RCC_temp &= ~(RCC_USESYSDIV); // The system clock is used undivided.
    RCC2_temp |= RCC2_bypass2; // The system clock is derived from the OSC source and divided by the divisor specified by SYSDIV2.

    //Escreve oq ja foi feito no registrador
    ESC_REG(RCC) = RCC_temp;
    ESC_REG(RCC2) = RCC2_temp;

                        // Passo 2:
    /*
     * Select the crystal value (XTAL) and oscillator source (OSCSRC), and clear the PWRDN bit in RCC/RCC2. Setting the XTAL field
     * automatically pulls valid PLL configuration data for the apropriate crystal, and clearing the PWRDN bit Powers the enables
     * the PLL and its output.
     */

    // limpando o conteudo do crystal M e as fontes de clock
    RCC_temp &= ~ (RCC_XTALM | RCC_OSCSRC ); // Used LFIOSC (Low-frequency internal oscillator)
    //RCC_temp |= RCC_PIOSC | RCC_XTAL_16MHZ;
    RCC_temp |= OSCSRC_PIOSC4 | RCC_XTAL_16MHZ; // Edited to tryout OSCSRC_PIOSC4
    RCC2_temp &= ~(RCC2_USERCC2 | RCC2_OSCSRCM);
    RCC2_temp |= (config & (RCC2_USERCC2)) | RCC_XTAL_16MHZ | RCC_PIOSC;
    // verifica se vai usar o RCC2 Frequencia

    // powerdown
    RCC_temp &= ~(RCC_PWRDN); // The PLL is operating normally.
    RCC2_temp &= ~(RCC_PWRDN); // The PLL is operating normally.
    ESC_REG(MISC) = INT_PLL_LOCK; // limpar status de coisas erradas passadas

    //Escreve oq ja foi feito no registrador
    ESC_REG(RCC) = RCC_temp;
    ESC_REG(RCC2) = RCC2_temp;

                        // Passo 3:
    /*
     * Select the desired system divider (SYSDIV) in RCC/RCC2 and set the USESYS bit in RCC. The SYSDIV field determines the system
     *  frequency for the MCU.
     */
    RCC_temp &= ~(RCC_SYSDIV_M | RCC_USESYSDIV );
    RCC_temp |= config & ( RCC_SYSDIV_M |RCC_USESYSDIV );
    RCC2_temp &= ~(RCC2_SYSDIV_M | RCC2_DIV400 );
    RCC2_temp |= config & (RCC2_SYSDIV_M | RCC2_DIV400);

                        // Passo 4:
    /*
     * Wait for the PLL to lock by ṕolling the PLLL RIS bit in the Raw Interrupt Status (RIS) register.
     *
     */
    for(delay=32768;delay > 0;delay--)
    {
        if(ESC_REG(RIS) & INT_PLL_LOCK)
        {
        break;
        }
    }

                        // Passo 5:

    /*
     * Enable use of the PLL by clearing the BYPASS bit in RCC/RCC2.
     */

    RCC_temp &= ~(RCC_bypass);
    RCC2_temp &= ~(RCC2_bypass2);

    //Escreve oq ja foi feito no registrador
    ESC_REG(RCC) = RCC_temp;
    ESC_REG(RCC2) = RCC2_temp;
}


#ifdef mili
    void delay_system(float mS)
    {
        mS = (mS/1000) * timer_duvidoso_mili;
        while(mS > 0)
            mS--;
    }
#endif

#ifdef micro
    void delay_system(float uS)
    {
        uS = (uS/1000000)*timer_duvidoso_mili;
        while(uS > 0)
            uS--;
    }
#endif



    void SysTick_Init(void){
       ESC_REG(NVIC_ST_CTRL_R) = 0;             // disable SysTick during setup
       ESC_REG(NVIC_ST_CTRL_R) = 0x5;           // enable SysTick with System Clock (101)
    }

    // the delay parameter is in units of the 80MHz core clock (12.5ns)
    void SysTick_Wait(unsigned long delay){
       ESC_REG(NVIC_ST_RELOAD_R) = delay-1;                 // number of counts to wait
       ESC_REG(NVIC_ST_CURRENT_R) = 0;                      // any value written to CURRENT is cleared
       while(~(ESC_REG(NVIC_ST_CTRL_R))&0x00010000){}       // wait for count flag
    }

    void SysTick_Wait10ms(unsigned long delay){
       unsigned long i;
       for(i=0;i<delay;i++){
           SysTick_Wait(800000);   // wait 10ms (800000*12.5ns = 10ms)
       }
    }


#ifdef delay_SYSTICK

    void Delay_ms(unsigned long time, unsigned int frequency){
            if(time>10){
                Delay_ms(time/10,frequency);
            }
            SysTick_Wait(time*frequency);
        }
#endif


int main(void)
{
volatile uint32_t ui32Loop;
config_Clock(config_20MHZ);
// Habilita o portal de GPIO F
ESC_REG(SYSCTL_RCGC2_R) = SYSCTL_RCGC2_GPIOF;
// Faz leitura dummy para efeito de atraso
ui32Loop = ESC_REG(SYSCTL_RCGC2_R);
// Habilita o pino 3 do portal F, configura como saída digital
ESC_REG(GPIO_PORTF_DIR_R) = 0x08;
ESC_REG(GPIO_PORTF_DEN_R) = 0x08;
// Loop principal

    #ifdef delay_SYSTICK
        SysTick_Init();
    #endif
    while(1)
    {
        // Acende o LED
        ESC_REG(GPIO_PORTF_DATA_R)|= 0x08;

        // Atraso
        //for(ui32Loop = 0; ui32Loop < 2000000; ui32Loop++){}
        #ifdef mili
        delay_system(200); //  2*5500000 ~ 1segundo em microsegundos como parametro
        #endif

        #ifdef delay_SYSTICK
                Delay_ms(200,80e6);
        #endif
        // Apaga o LED
        ESC_REG(GPIO_PORTF_DATA_R) &= ~(0x08);

        // Atraso
                //for(ui32Loop = 0; ui32Loop < 2000000; ui32Loop++){}
        #ifdef mili
               delay_system(200); //  2*5500000 ~ 1segundo em microsegundos como parametro
        #endif

        #ifdef delay_SYSTICK
                Delay_ms(200,80e6);
        #endif
    }
}

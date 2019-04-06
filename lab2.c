#include <stdint.h>

#define ESC_REG(x) (*((volatile uint32_t *)(x)))
#define SYSCTL_RCGC2_R 0x400FE108
#define SYSCTL_RCGC2_GPIOF 0x00000020
#define GPIO_PORTF_DIR_R 0x40025400
#define GPIO_PORTF_DEN_R 0x4002551C
#define GPIO_PORTF_DATA_R 0x400253FC

#define RCC 0x400FE060
#define RCC2 0x400FE070
#define RCC_bypass 1<<11 // ou 0x00000800
#define RCC_USESYSDIV 1<<22 // ou 0x00400000
#define RCC2_bypass2 1<<11 // ou 0x00000800
#define RCC_XTALM 0x7C0
#define RCC_PIOSC 0x10
#define RCC_OSCSRC 0x30
#define RCC_XTAL_16MHZ 0x540 // 0x15<<6
#define RCC2_USERCC2 1<<31 // 0x80000000
#define RCC2_OSCSRCM 7<<4 // 0x70
#define RCC_PWRDN 1<<13 // 0x2000
#define MISC 0x400FE058
#define INT_PLL_LOCK 0x40
#define RCC_SYSDIV_M 0x07800000 // 0x1F<<23
#define RCC2_SYSDIV_M 0x1F800000
#define RCC2_DIV400 1<<30
#define RIS 0x400FE050

#define config_80MHZ 0xC1000000 //RCC2
#define config_50MHZ 0x01C00000 // Divisao por 4 e usar o sysdiv
#define config_20MHZ 0x04C00000 // Divisao por 10 e usar o sysdiv
#define config_4MHZ 0x98C00000 //RCC2 divisao por 50
#define config_3MHZ 0x9FC00000 //RCC2 divisao maxima(64) pag 224
#define micro
const float timer_duvidoso_mili = 10800000;
const float timer_duvidoso_micro = 10400000000;
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
    //primeiro passo
    uint32_t RCC_temp,RCC2_temp,delay;
    RCC_temp = ESC_REG(RCC);
    RCC2_temp = ESC_REG(RCC2);

    // Bypass PLL e limpar divisores de clock
    RCC_temp |= RCC_bypass;
    RCC_temp &= ~(RCC_USESYSDIV);
    RCC2_temp |= RCC2_bypass2;

    ESC_REG(RCC) = RCC_temp;
    ESC_REG(RCC2) = RCC2_temp;

    // passo 2

    // limpando o conteudo do crystal M e as fontes de clock
    RCC_temp &= ~ (RCC_XTALM | RCC_OSCSRC );
    RCC_temp |= RCC_PIOSC | RCC_XTAL_16MHZ;
    RCC2_temp &= ~(RCC2_USERCC2 | RCC2_OSCSRCM);
    RCC2_temp |= (config & (RCC2_USERCC2)) | RCC_XTAL_16MHZ | RCC_PIOSC;
    // verifica se vai usar o RCC2 Frequencia

    // powerdown
    RCC_temp &= ~(RCC_PWRDN);
    RCC2_temp &= ~(RCC_PWRDN); // mesmo enderecos
    ESC_REG(MISC) = INT_PLL_LOCK; // limpar status de coisas erradas passadas

    ESC_REG(RCC) = RCC_temp;
    ESC_REG(RCC2) = RCC2_temp;

    // Passo 3
    RCC_temp &= ~(RCC_SYSDIV_M | RCC_USESYSDIV );
    RCC_temp |= config & ( RCC_SYSDIV_M |RCC_USESYSDIV );
    RCC2_temp &= ~(RCC2_SYSDIV_M | RCC2_DIV400 );
    RCC2_temp |= config & (RCC2_SYSDIV_M | RCC2_DIV400);

    // Passo 4
    for(delay=32768;delay > 0;delay--)
    {
        if(ESC_REG(RIS) & INT_PLL_LOCK)
        {
        break;
        }
    }

    // Passo 5
    RCC_temp &= ~(RCC_bypass);
    RCC2_temp &= ~(RCC2_bypass2);
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
int main(void)
{
volatile uint32_t ui32Loop;
config_Clock(config_80MHZ);
// Habilita o portal de GPIO F
ESC_REG(SYSCTL_RCGC2_R) = SYSCTL_RCGC2_GPIOF;
// Faz leitura dummy para efeito de atraso
ui32Loop = ESC_REG(SYSCTL_RCGC2_R);
// Habilita o pino 3 do portal F, configura como saída digital
ESC_REG(GPIO_PORTF_DIR_R) = 0x08;
ESC_REG(GPIO_PORTF_DEN_R) = 0x08;
// Loop principal



    while(1)
    {
        // Acende o LED
        ESC_REG(GPIO_PORTF_DATA_R)|= 0x08;

        // Atraso
        //for(ui32Loop = 0; ui32Loop < 2000000; ui32Loop++){}
        delay_system(1); //  2*5500000 ~ 1segundo em microsegundos como parametro

        // Apaga o LED
        ESC_REG(GPIO_PORTF_DATA_R) &= ~(0x08);

        // Atraso
        //for(ui32Loop = 0; ui32Loop < 2000000; ui32Loop++){}
        delay_system(1);
    }
}

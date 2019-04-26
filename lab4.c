#include <stdint.h>

#define ESC_REG(x)                  (*((volatile uint32_t *)(x)))

#define SYSCTL_RCGC2_GPIOF          0x00000020
#define SYSCTL_RCGCGPIO             0x400FE608
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
#define RCC_XTALM                   0x7C0
#define RCC_PIOSC                   0x10
#define RCC_OSCSRC                  0x30
#define RCC_XTAL_16MHZ              0x540 // 0x15<<6
#define RCC2_USERCC2                1<<31 // 0x80000000
#define RCC2_OSCSRCM                7<<4    // 0x70
#define RCC_PWRDN                   1<<13   // 0x2000
#define MISC                        0x400FE058
#define INT_PLL_LOCK                0x40
#define RCC_SYSDIV_M                0x07800000 // 0x1F<<23
#define RCC2_SYSDIV_M               0x1F800000
#define RCC2_DIV400                 1<<30
#define RIS                         0x400FE050
#define GPIO_O_ICR                  0x41C

#define config_80MHZ                0xC1000000      //RCC2
#define config_50MHZ                0x01C00000      // Divisao por 4 e usar o sysdiv
#define config_20MHZ                0x04C00000      // Divisao por 10 e usar o sysdiv
#define config_4MHZ                 0x98C00000      //RCC2 divisao por 50
#define config_3MHZ                 0x9FC00000      //RCC2 divisao maxima(64) pag 224

//  diferem no bit mais significativo

#define portalA_BASE                0x40004000
#define portalB_BASE                0x40005000
#define portalC_BASE                0x40006000
#define portalD_BASE                0x40007000
#define portalE_BASE                0x40024000
#define portalF_BASE                0x40025000

#define portalGPIO_a                0x01
#define portalGPIO_b                0x02
#define portalGPIO_c                0x04
#define portalGPIO_d                0x08
#define portalGPIO_e                0x10
#define portalGPIO_f                0x20

#define pino0                       0x01
#define pino1                       0x02
#define pino2                       0x04
#define pino3                       0x08
#define pino4                       0x10
#define pino5                       0x20
#define pino6                       0x40
#define pino7                       0x80

#define GPIO_O_DIR                  0x400
#define GPIO_O_DR2R                 0x500
#define GPIO_O_DEN                  0x51C
#define GPIO_O_PUR                  0x510
#define GPIO_O_DATA                 0x000

#define EN0                         0xE000E100
#define EN1                         0xE000E104
#define EN2                         0xE000E108
#define EN3                         0xE000E10C
#define EN4                         0xE000E110

#define int_port_A                  0
#define int_port_B                  1
#define int_port_C                  2
#define int_port_D                  3
#define int_port_E                  4
#define int_port_F                  30
#define GPIO_O_IBE                  0x408
#define GPIO_O_IS                   0x404
#define GPIO_O_IEV                  0x40C

#define GPIO_FallingEdge            0x0
#define GPIO_RisingEdge             0x4
#define GPIO_BothEdges              0x1
#define GPIO_LowLevel               0x2
#define GPIO_HighLevel              0x6

#define GPIO_O_IM                   0x410
#define NVIC_ST_CTRL                0xE000E010
#define NVIC_ST_CTRL_CLK_SRC        0x4
#define NVIC_ST_CTRL_ENABLE         0x1
#define NVIC_ST_CTRL_INTEN          0x2
#define NVIC_ST_RELOAD              0xE000E014

const float timer_duvidoso_mili_80MHz = 3800000;  // ~um segundo


void habilita_clockGPIO(uint32_t portalGPIO)
{
    ESC_REG(SYSCTL_RCGCGPIO)|=portalGPIO;
    //nao jogar fora o conteudo anterior
}
void delay_system(float mS)
{

   mS = (mS/1000) * timer_duvidoso_mili_80MHz;
   while(mS > 0)
   mS--;
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

void habilita_interrupcao_global(void){
    //passo3
    __asm( " mrs r0, PRIMASK\n"
    " cpsie i\n"
    " bx lr\n");
}

void habilitaInterrupcao(uint32_t ui32Interrupcao){
    //passo2
    if(ui32Interrupcao < 32){
        ESC_REG(EN0)    |= 1<<ui32Interrupcao;
    }
}


void configInt_GPIO(uint32_t portal, uint8_t pino, uint32_t tipo){
    ESC_REG(portal + GPIO_O_IBE) = ((tipo & 1) ?
                                    (ESC_REG(portal + GPIO_O_IBE) | pino) :
                                    (ESC_REG(portal + GPIO_O_IBE) & ~(pino)));
    ESC_REG(portal + GPIO_O_IS) = ((tipo & 2) ?
                                   (ESC_REG(portal + GPIO_O_IS) | pino) :
                                   (ESC_REG(portal + GPIO_O_IS) & ~(pino)));
    ESC_REG(portal + GPIO_O_IEV) = ((tipo & 4) ?
                                    (ESC_REG(portal + GPIO_O_IEV) | pino) :
                                    (ESC_REG(portal + GPIO_O_IEV) & ~(pino)));
}

void habilitaInt_GPIO(uint32_t portal, uint8_t pino){
    //passo1 Device Arm
    ESC_REG(portal + GPIO_O_IM) |= pino;
}

void desabilitaInt_GPIO(uint32_t portal, uint8_t pino){
    ESC_REG(portal + GPIO_O_IM) &= ~(pino);
}

void habilitaSystick(void){
    ESC_REG(NVIC_ST_CTRL) |= NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_ENABLE;
}

void habilitaIntSystick(void){
    ESC_REG(NVIC_ST_CTRL) |= NVIC_ST_CTRL_INTEN;
}

void desabilitaIntSystick(void){
    ESC_REG(NVIC_ST_CTRL) &= ~(NVIC_ST_CTRL_INTEN);
}

void configPeriodoSystick(uint32_t periodo){
    ESC_REG(NVIC_ST_RELOAD) = periodo - 1;
}

void limpaInt_GPIO(uint32_t portal, uint8_t pino){
    ESC_REG(portal + GPIO_O_ICR) = pino;
}

void trataIntGPIOF(void){
    // primeira coisa: descobrir qual foi a fonte da interrupção
    limpaInt_GPIO(portalF_BASE, pino4);

    if(GPIO_Leitura(portalF_BASE, pino2)== pino2)
    {
    GPIO_escrita(portalF_BASE, pino2, ~pino2);
    delay_system(200);
    }
    else
    {
        GPIO_escrita(portalF_BASE, pino2, pino2);
        delay_system(200);
    }
}

void trataSystick(void)
{
    if(GPIO_Leitura(portalF_BASE, pino2)== pino2)
        {
        GPIO_escrita(portalF_BASE, pino2, ~pino2);
        }
        else
        {
            GPIO_escrita(portalF_BASE, pino2, pino2);
        }
}

int main(void)
{
    volatile uint32_t ui32Loop;
    habilita_clockGPIO(portalGPIO_f);

    // Faz leitura dummy para efeito de atraso
    ui32Loop = ESC_REG(SYSCTL_RCGC2_R);

    configuraPino_saida(portalF_BASE, pino2);

    //configuraPino_entrada(portalF_BASE, pino4);
    //ESC_REG(portalF_BASE+GPIO_O_PUR)|= pino4;

    //GPIO_escrita(portalF_BASE, pino2, pino2);

    habilita_interrupcao_global();
    //habilitaInterrupcao(30);
    //configInt_GPIO(portalF_BASE, pino4, GPIO_FallingEdge);
    //habilitaInt_GPIO(portalF_BASE, pino4);
    habilitaSystick();
    habilitaIntSystick();
    configPeriodoSystick(200000000);

    // Loop principal
    while(1)
    {
    }
}

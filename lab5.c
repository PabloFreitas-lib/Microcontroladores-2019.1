#include <stdint.h>

#define catodo
#define interrupcao
//#define systick
#define timer_code

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

#define GPIO_O_LOCK                 0x520
#define GPIO_O_CR                   0x524
#define GPIO_LOCK_KEY               0x4C4F434B


/*#define config_80MHZ              0xC1000000      //RCC2
#define config_50MHZ                0x01C00000      // Divisao por 4 e usar o sysdiv
#define config_20MHZ                0x04C00000      // Divisao por 10 e usar o sysdiv
#define config_4MHZ                 0x98C00000      //RCC2 divisao por 50
#define config_3MHZ                 0x9FC00000      //RCC2 divisao maxima(64) pag 224*/

//  diferem no bit mais significativo

#define portalA_BASE                0x40004000
#define portalB_BASE                0x40005000
#define portalC_BASE                0x40006000
#define portalD_BASE                0x40007000
#define portalE_BASE                0x40024000
#define portalF_BASE                0x40025000

#define portalA_base                0x40004000
#define portalB_base                0x40005000
#define portalC_base                0x40006000
#define portalD_base                0x40007000
#define portalE_base                0x40024000
#define portalF_base                0x40025000

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

#ifdef anodo

unsigned int vector_numbers[17]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,0x03};
unsigned int vector_digits[4]={0x8C,0x4C,0xC8,0xC4}; // sinal baixo funciona
int um_minuto_anodo = 3000;
#endif

#ifdef catodo
unsigned int vector_numbers[17]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E,0xFC};
unsigned int vector_digits[5]={0x40,0x80,0x04,0x08,0xCC}; // sinal alto funciona
int um_minuto_catodo = 1500;
#endif



                            // timer

#define RCGCTIMER                   0x400FE604

#define TIMER_O_CTL                 0x00C
#define TIMER_O_CFG                 0x000

#define TIMER_CTL_TAEN              0x1
#define TIMER_CTL_TBEN              0x100

#define TIMER_O_TAMR                  0x004 //0x04
#define TIMER_O_TBMR                  0x008 //0x08

#define TIMER_TAMR_TAPWMIE            0x200
#define TIMER_TBMR_TAPWMIE            0x200

#define TIMER_O_TAILR                 0x028 // 0x28
#define TIMER_O_TBILR                 0x02C // 0x2C

#define TIMER_O_IMR                 0x018


//FLAGS
#define TIMER_A_MATCH               0x10
#define TIMER_A_TIMEOUT          0x001 // 0x1
#define TIMER_B_MATCH               0x800
#define TIMER_B_TIMEOUT          0x100


#define TIMER_O_ICR                 0x024


//modulos
#define TIMER_0                    0x01
#define TIMER_1                    0x02
#define TIMER_2                    0x04
#define TIMER_3                    0x08
#define TIMER_4                    0x10
#define TIMER_5                    0x20

//bases
#define TIMER0_BASE                 0x40030000
#define TIMER1_BASE                 0x40031000
#define TIMER2_BASE                 0x40032000
#define TIMER3_BASE                 0x40033000
#define TIMER4_BASE                 0x40034000
#define TIMER5_BASE                 0x40035000

#define ONE_SHOT                   0x21
#define ONE_SHOT_UP                0x31
#define PERIODIC                   0x22
#define PERIODIC_UP                0x32
#define SPLIT_PAIR                 0x04000000

#define A_ONE_SHOT                 0x21
#define A_ONE_SHOT_UP              0x31
#define A_PERIODIC                 0x22
#define A_PERIODIC_UP              0x32

#define B_ONE_SHOT                 0x2100
#define B_ONE_SHOT_UP              0x3100
#define B_PERIODIC                 0x2200
#define B_PERIODIC_UP              0x3200

#define TIMER_A                    0xFF
#define TIMER_B                    0xFF00
#define TIMER_BOTH                 0xFFFF

#define INT_TIMER_0A                19
#define INT_TIMER_0B                20
#define INT_TIMER_1A                21
#define INT_TIMER_1B                22
#define INT_TIMER_2A                23
#define INT_TIMER_2B                24
#define INT_TIMER_3A                35
#define INT_TIMER_3B                36
#define INT_TIMER_4A                70
#define INT_TIMER_4B                71
#define INT_TIMER_5A                92
#define INT_TIMER_5B                93


//----------final timers--------------------------------------------

const float timer_duvidoso_mili_80MHz = 3800000;  // ~um segundo
const float timer_doopler = 0.33;

                    // GERAIS


void unlock_GPIO(uint32_t portal)
{
       ESC_REG(portal + GPIO_O_LOCK) = GPIO_LOCK_KEY;
       ESC_REG(portal + GPIO_O_CR) = 0x01;
}

void lock_GPIO(uint32_t portal)
{
    ESC_REG(portal + GPIO_O_LOCK) = 0;
}
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


void numero(int i)
{
    GPIO_escrita(portalE_base, pino0|pino1|pino2|pino3, vector_numbers[i]);
    GPIO_escrita(portalC_base, pino4|pino5|pino6|pino7, vector_numbers[i]);
}
void digito(int i)
{
    GPIO_escrita(portalB_base, pino6|pino7, vector_digits[i]);
    GPIO_escrita(portalD_base, pino3|pino2, vector_digits[i]);
}


#ifdef anodo
void pontos_intermitentes(void)
{
       GPIO_escrita(portalB_base, pino6|pino7, pino6|pino7);
       GPIO_escrita(portalD_base, pino3|pino2, pino3|pino2);
       GPIO_escrita(portalD_base, pino6, ~(pino6));
       GPIO_escrita(portalE_base, pino0|pino1|pino2, (pino0)|(pino1)|~(pino2));
}
#endif


#ifdef catodo
void pontos_intermitentes(void)
{
       GPIO_escrita(portalB_base, pino6|pino7, 0x00|0x00);
       GPIO_escrita(portalD_base, pino3|pino2|pino6, 0x00|0x00|pino6);
       //GPIO_escrita(portalE_base, pino0|pino1|pino2, ~pino0|~pino1|pino2);

       GPIO_escrita(portalE_base, pino0|pino1|pino2|pino3, ~pino0|~pino1|~pino2|~pino3);
       GPIO_escrita(portalC_base, pino4|pino5|pino6|pino7, ~pino4|~pino5|~pino6|~pino7);
}
#endif

#ifdef anodo
void limpa_diplay(void)
{
    GPIO_escrita(portalB_base, pino6|pino7, 0x00|0x00);
    GPIO_escrita(portalD_base, pino3|pino2|pino6, 0x00|0x00|pino6);
    GPIO_escrita(portalE_base, pino0|pino1|pino2|pino3, 0x00|0x00|0x00|0x00);
    GPIO_escrita(portalC_base, pino4|pino5|pino6|pino7, 0x00|0x00|0x00|0x00);
}
#endif

#ifdef catodo

void limpa_diplay(void)
{
    GPIO_escrita(portalB_base, pino6|pino7, pino6|pino7);
    GPIO_escrita(portalD_base, pino3|pino2|pino6, pino3|pino2|0x00);
    GPIO_escrita(portalE_base, pino0|pino1|pino2|pino3, pino0|pino1|pino2|pino3);
    GPIO_escrita(portalC_base, pino4|pino5|pino6|pino7, pino4|pino5|pino6|pino7);
}

#endif




                        // interrupcao


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





                        // TIMER

void habilita_periferico_timer(uint32_t modulo) // habilita timer
{
    ESC_REG(RCGCTIMER)|= modulo;
}

void desabilita_periferico_timer(uint32_t modulo)
{
    ESC_REG(RCGCTIMER)&= ~(modulo);
}

void configuraTimer(uint32_t timer_base,uint32_t config)
{
    ESC_REG(timer_base + TIMER_O_CTL) &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN); // limpa

    ESC_REG(timer_base + TIMER_O_CFG) = config >> 24;
    ESC_REG(timer_base + TIMER_O_TAMR) = ((config & 0xFF) | TIMER_TAMR_TAPWMIE);
    ESC_REG(timer_base + TIMER_O_TBMR) = (((config >> 8) & 0xFF) | TIMER_TBMR_TAPWMIE);
}

void configuraPeriodoTimer(uint32_t timer_base, uint32_t timer, uint32_t valor)
{
    if(timer & TIMER_A)
    {
        ESC_REG(timer_base + TIMER_O_TAILR) = valor;
    }
    if(timer & TIMER_B)
    {
        ESC_REG(timer_base + TIMER_O_TBILR) = valor;
    }
}

void habilitaIntTimer(uint32_t timer_base,uint32_t flag)
{
    ESC_REG(timer_base + TIMER_O_IMR)|=  flag;
}
void desabilitaIntTimer(uint32_t timer_base,uint32_t flag)
{
    ESC_REG(timer_base + TIMER_O_IMR)&=  ~flag;
}

void limpaIntTimer(uint32_t timer_base,uint32_t flag)
{
    ESC_REG(timer_base + TIMER_O_ICR) =  flag;
}

void habilitaTimer(uint32_t timer_base,uint32_t timer) // CTL
{
    ESC_REG(timer_base + TIMER_O_CTL) |= (timer & (TIMER_CTL_TAEN | TIMER_CTL_TBEN));
}

void desabilitaTimer(uint32_t timer_base,uint32_t timer)
{
    ESC_REG(timer_base + TIMER_O_CTL) &= ~ timer & (TIMER_CTL_TAEN | TIMER_CTL_TBEN);
}

void trataTimer(void)
{
    desabilitaIntTimer(TIMER0_BASE,TIMER_A_TIMEOUT);
    habilitaInt_GPIO(portalF_BASE,pino0|pino4);
    limpaIntTimer(TIMER0_BASE,TIMER_A_TIMEOUT);
}



void toggle(uint8_t pino,uint32_t portal)
{
    if(GPIO_Leitura(portal, pino)== pino)
    {
        GPIO_escrita(portal, pino, ~pino);
        configuraPeriodoTimer(TIMER0_BASE, TIMER_A, 200000);
        habilitaIntTimer(TIMER0_BASE,TIMER_A_TIMEOUT); // IMR
        habilitaTimer(TIMER0_BASE , TIMER_A); // CTL
        desabilitaInt_GPIO(portal,pino);
        limpaInt_GPIO(portal,pino);
    }
    else
    {
        GPIO_escrita(portal, pino, pino);
        configuraPeriodoTimer(TIMER0_BASE, TIMER_A, 200000);
        habilitaIntTimer(TIMER0_BASE,TIMER_A_TIMEOUT); // IMR
        habilitaTimer(TIMER0_BASE , TIMER_A); // CTL
        desabilitaInt_GPIO(portal,pino);
        limpaInt_GPIO(portal,pino);
    }
}


void trataIntGPIOF(void)
{
    // primeira coisa: descobrir qual foi a fonte da interrupção
    limpaInt_GPIO(portalF_BASE, pino4);

    if(GPIO_Leitura(portalF_BASE, pino0)!= pino0)
    {
        toggle(pino2, portalF_BASE);
    }
    if(GPIO_Leitura(portalF_BASE, pino4)!= pino4)
    {
        toggle(pino1, portalF_BASE);
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


                            // MAIN
int main(void)
{
    volatile uint32_t ui32Loop;
    habilita_clockGPIO(portalGPIO_f);
    habilita_periferico_timer(TIMER_0);




    // Faz leitura dummy para efeito de atraso
    ui32Loop = ESC_REG(SYSCTL_RCGC2_R);


    configuraPino_saida(portalF_BASE, pino2|pino1|pino3);

    unlock_GPIO(portalF_BASE);

    configuraPino_entrada(portalF_BASE, pino4|pino0);
    ESC_REG(portalF_BASE+GPIO_O_PUR)|= pino4|pino0;

    lock_GPIO(portalF_BASE);

    //GPIO_escrita(portalF_BASE, pino2, pino2);

#ifdef interrupcao

    configInt_GPIO(portalF_BASE, pino4|pino0, GPIO_FallingEdge);
    habilita_interrupcao_global();
    habilitaInterrupcao(int_port_F);
    habilitaInt_GPIO(portalF_BASE, pino4|pino0);

#endif

#ifdef systick

    habilitaSystick();
    habilitaIntSystick();
    configPeriodoSystick(200000000);

#endif

#ifdef timer_code

    configuraTimer(TIMER0_BASE, A_ONE_SHOT);
    configuraPeriodoTimer(TIMER0_BASE, TIMER_A, 200000);
    habilitaInterrupcao(INT_TIMER_0A);



#endif


    // Loop principal
    while(1){}
}






/*
 * tratabotao
 * {
 * limpainterrupcao
 * verifica se ta no sw1 e sw2
 * toggle
 * limpaint botao
 * desabilita intrupcao botao
 * dispara timer
 * }
 *
 * tratartimer{
 * limpaint timer
 * desabilidta timer
 * habilita botao
 * }
 *
 * */

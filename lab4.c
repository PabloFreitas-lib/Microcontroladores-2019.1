#include <stdint.h>

#define catodo

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

#define GPIO_O_LOCK                 0x520
#define GPIO_O_CR                   0x524
#define GPIO_LOCK_KEY               0x4C4F434B

const float timer_duvidoso_mili_80MHz = 3800000;  // ~um segundo
const float timer_doopler = 0.35;

int n1=0,n2=0,n3=0,n4=0;

int systick_cronometro = 5000000;
int pause = 0;

#ifdef anodo

unsigned int vector_numbers[17]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,0x03};
unsigned int vector_digits[4]={0x8C,0x4C,0xC8,0xC4}; // sinal baixo funciona
int um_minuto_anodo = 3000;
#endif

#ifdef catodo
unsigned int vector_numbers[17]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E,0xFC};
unsigned int vector_digits[4]={0x40,0x80,0x04,0x08}; // sinal alto funciona
int um_minuto_catodo = 1500;
#endif

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
    GPIO_escrita(portalE_BASE, pino0|pino1|pino2|pino3, vector_numbers[i]);
    GPIO_escrita(portalC_BASE, pino4|pino5|pino6|pino7, vector_numbers[i]);
}
void digito(int i)
{
    GPIO_escrita(portalB_BASE, pino6|pino7, vector_digits[i]);
    GPIO_escrita(portalD_BASE, pino3|pino2, vector_digits[i]);
}


#ifdef anodo
void pontos_intermitentes(void)
{
       GPIO_escrita(portalB_BASE, pino6|pino7, pino6|pino7);
       GPIO_escrita(portalD_BASE, pino3|pino2, pino3|pino2);
       GPIO_escrita(portalD_BASE, pino6, ~(pino6));
       GPIO_escrita(portalE_BASE, pino0|pino1|pino2, (pino0)|(pino1)|~(pino2));
}
#endif


#ifdef catodo
void pontos_intermitentes(void)
{
       GPIO_escrita(portalB_BASE, pino6|pino7, 0x00|0x00);
       GPIO_escrita(portalD_BASE, pino3|pino2|pino6, 0x00|0x00|pino6);

       GPIO_escrita(portalE_BASE, pino0|pino1|pino2|pino3, ~pino0|~pino1|~pino2|~pino3);
       GPIO_escrita(portalC_BASE, pino4|pino5|pino6|pino7, ~pino4|~pino5|~pino6|~pino7);
}
#endif


#ifdef anodo
void limpa_diplay(void)
{
    GPIO_escrita(portal_BASE, pino6|pino7, 0x00|0x00);
    GPIO_escrita(portalD_BASE, pino3|pino2|pino6, 0x00|0x00|pino6);
    GPIO_escrita(portalE_BASE, pino0|pino1|pino2|pino3, 0x00|0x00|0x00|0x00);
    GPIO_escrita(portalC_BASE, pino4|pino5|pino6|pino7, 0x00|0x00|0x00|0x00);
}
#endif

#ifdef catodo

void limpa_diplay(void)
{
    GPIO_escrita(portalB_BASE, pino6|pino7, pino6|pino7);
    GPIO_escrita(portalD_BASE, pino3|pino2|pino6, pino3|pino2|0x00);
    GPIO_escrita(portalE_BASE, pino0|pino1|pino2|pino3, pino0|pino1|pino2|pino3);
    GPIO_escrita(portalC_BASE, pino4|pino5|pino6|pino7, pino4|pino5|pino6|pino7);
}

#endif

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

void relogio_systick(void)
{

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
                             //#ifdef catodo
                             //   for (j=0; j<um_minuto_catodo; j++)
                             //#endif

                             #ifdef anodo
                                for (j=0; j<um_minuto_anodo; j++)
                             #endif
                             //{
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
                             //n1++; // fim do while n1
                             //}
                          }// fim do while n2
                       }// fim do while n3
                    } // fim do while n4
                n1=0,n2=0,n3=0,n4=0; // recomeco

}


void unlock_GPIO(uint32_t portal)
{
       ESC_REG(portal + GPIO_O_LOCK) = GPIO_LOCK_KEY;
       ESC_REG(portal + GPIO_O_CR) = 0x01;
}

void lock_GPIO(uint32_t portal)
{
    ESC_REG(portal + GPIO_O_LOCK) = 0;
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

void toggle(uint8_t pino)
{
    if(GPIO_Leitura(portalF_BASE, pino)== pino)
    {
        GPIO_escrita(portalF_BASE, pino, ~pino);
        delay_system(200);
    }
    else
    {
        GPIO_escrita(portalF_BASE, pino, pino);
        delay_system(200);
    }

}

void cronometro_systick(void){

int j=0;



    while(n4<=5) // dezena minuto 0 - 6
    {
        while(n3<=9) //unidade minuto 0 - 9
        {
            while(n2<=9) //segundo dezena 0 - 6
            {
                while(n1<=9) // segundos unidade 0 - 9
                {
                    for (j=0; j<4; j++)
                    {
                        escreve_4_digitos(n4, n3, n2, n1);
                        if(GPIO_Leitura(portalF_BASE,pino0)!= pino0) // botao zerar
                            {
                                n4=0,n3=0,n2=0,n1=0;
                                delay_system(45);
                            }

                        #ifdef anodo
                        if(j%50==0)
                        {
                            pontos_intermitentes();
                            delay_system(timer_doopler);

                            limpa_diplay();
                            delay_system(timer_doopler);
                        }
                        #endif
                    }
                }
                if(n1==10)
                {
                    n2++;
                    n1=0;
                }
            }
            if(n2==10)
            {
                ++n3;
                n2=0;
            }
        }
        if(n3==10)
        {
            ++n4;
            n3=0;
        }
    }
    n1=0,n2=0,n3=0,n4=0;
}

void trataIntGPIOF(void)
{
    // primeira coisa: descobrir qual foi a fonte da interrupção
    limpaInt_GPIO(portalF_BASE, pino4);

    /*if(GPIO_Leitura(portalF_BASE,pino4)!= pino4) // botao pausar/continuar
    {
        delay_system(45);
        if(!pause)
        {
            pause=1;
            desabilitaIntSystick();
        }
        else if(pause)
        {
            pause=0;
            habilitaSystick();
            habilitaIntSystick();
            configPeriodoSystick(systick_cronometro);
        }
    }*/



}

void trataSystick(void)
{
   n1++;
}

void cronometro(void){

int pause = 0;
int n1=0,n2=0,n3=0,n4=0,j=0;

    while(n4<=5) // dezena minuto 0 - 6
    {

        while(n3<=9) //unidade minuto 0 - 9
        {
            while(n2<=9) //segundo dezena 0 - 6
            {
                while(n1<=9) // segundos unidade 0 - 9
                {
                    for (j=0; j<4; j++)
                    {
                        escreve_4_digitos(n4, n3, n2, n1);

                        if(GPIO_Leitura(portalF_BASE,pino4)!= pino4) // botao pausar/continuar
                        {
                            delay_system(45);
                            if(!pause)
                            {
                                pause=1;
                            }
                            else if(pause)
                            {
                                pause=0;
                            }
                        }
                        if(GPIO_Leitura(portalF_BASE,pino0)!= pino0) // botao zerar
                        {
                            delay_system(45);
                            n1=0;n2=0;n3=0;n4=0;
                        }
                        #ifdef anodo
                        if(j%50==0)
                        {
                            pontos_intermitentes();
                            delay_system(timer_doopler);

                            limpa_diplay();
                            delay_system(timer_doopler);
                        }
                        #endif
                    }
                    //if(!pause)
                      //                          n1++;
                }

                if(n1==10)
                                {
                                n2++;
                                n1=0;
                                }
            }
            if(n2==10)
                        {
                            ++n3;
                            n2=0;
                        }
        }
    if(n3==10)
           {
           ++n4;
           n3=0;
           }
    }
    n1=0,n2=0,n3=0,n4=0;
}

int main(void)
{
    volatile uint32_t ui32Loop;
    habilita_clockGPIO(portalGPIO_e|portalGPIO_c|portalGPIO_d | portalGPIO_b| portalGPIO_f);

    // Faz leitura dummy para efeito de atraso
    ui32Loop = ESC_REG(SYSCTL_RCGC2_R);

    configuraPino_saida(portalC_BASE,pino4|pino5|pino6|pino7);
    configuraPino_saida(portalE_BASE,pino0|pino1|pino2|pino3);
    configuraPino_saida(portalB_BASE, pino6|pino7);
    configuraPino_saida(portalD_BASE, pino2|pino3|pino6);

    //configura pino botao sw1

    unlock_GPIO(portalF_BASE);

    configuraPino_entrada(portalF_BASE,pino0|pino4);
    ESC_REG(portalF_BASE+GPIO_O_PUR) |= pino0|pino4;

    lock_GPIO(portalF_BASE);

    //configuraPino_saida(portalF_BASE, pino1|pino2);

    habilita_interrupcao_global();
    habilitaSystick();
    habilitaIntSystick();
    configPeriodoSystick(systick_cronometro);

    //habilita_interrupcao_global();
    //habilitaInterrupcao(30);
    //configInt_GPIO(portalF_BASE, pino4|pino0, GPIO_FallingEdge);
    //habilitaInt_GPIO(portalF_BASE, pino4|pino0);



    while(1)
    {
        cronometro();
    }
}

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//definicoes para o ifdef

//#define anodo //Placa Preta ou display 7segmentos branco
#define catodo  // Placa verde ou display 7segmentos preto
//#define botao_sw2
//#define matrix
//#define relogio_on
//#define cronometro_on
#define questao_10

// nao sei oq fazem
#define SYSCTL_RCGC2_GPIOF          0x00000020
#define GPIOHBCTL                   0x400FE06C

//escrita em registrador
#define ESC_REG(x)                  (*((volatile uint32_t *)(x)))

//clock
#define SYSCTL_RCGC2_R              0x400FE108 // leitura dummy
#define SYSCTL_RCGCGPIO             0x400FE608 // usada na habilita clockGPIO



// configuracoes entrada/saida
#define GPIO_O_DIR                  0x400
#define GPIO_O_DR2R                 0x500
#define GPIO_O_DEN                  0x51C
#define GPIO_O_PUR                  0x510
#define GPIO_O_DATA                 0x000

//argumentos para habilitar o clock do portalX
#define portalGPIO_a                0x01
#define portalGPIO_b                0x02
#define portalGPIO_c                0x04
#define portalGPIO_d                0x08
#define portalGPIO_e                0x10
#define portalGPIO_f                0x20

//base dos portais
#define portalA_base                0x40004000
#define portalB_base                0x40005000
#define portalC_base                0x40006000
#define portalD_base                0x40007000
#define portalE_base                0x40024000
#define portalF_base                0x40025000

//pinos dos portais
#define pino0                       0x01
#define pino1                       0x02
#define pino2                       0x04
#define pino3                       0x08
#define pino4                       0x10
#define pino5                       0x20
#define pino6                       0x40
#define pino7                       0x80

//serve para da permissao de uso do pino0 do portalF
#define GPIO_O_LOCK                 0x520
#define GPIO_O_CR                   0x524
#define GPIO_LOCK_KEY               0x4C4F434B


#ifdef anodo

unsigned int vector_numbers[16]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};
unsigned int vector_digits[4]={0x8C,0x4C,0xC8,0xC4}; // sinal baixo funciona
int um_minuto_anodo = 3000;

#endif

#ifdef catodo

unsigned int vector_numbers[16]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E};
unsigned int vector_digits[4]={0x40,0x80,0x04,0x08}; // sinal alto funciona
int um_minuto_catodo = 1500;

#endif


// Defines to Interrupcao/Systick
#define GPIO_O_ICR                  0x41C
#define EN0                         0xE000E100
#define EN1                         0xE000E104
#define EN2                         0xE000E108
#define EN3                         0xE000E10C
#define EN4                         0xE000E110

// habilitaInterrupcao chama como argumento um desses
#define int_port_A                  0
#define int_port_B                  1
#define int_port_C                  2
#define int_port_D                  3
#define int_port_E                  4
#define int_port_F                  30

#define GPIO_O_IBE                  0x408
#define GPIO_O_IS                   0x404
#define GPIO_O_IEV                  0x40C

// configInt_GPIO no terceiro argumento chama um desses
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


// *********** VAR Globais ***********************************************************************
int systick_cronometro = 1500000,systick_relogio = 16000000,systick_debouncer_number =150000;

const float timer_duvidoso_mili_80MHz = 3800000;  // ~um segundo
const float timer_doopler = 0.35;

//usado para varredura da matrix de botao se os botoes que representa as linhas estivem de PULLUP
int vector_matrix[4] = {0x0E,0x0D,0x0B,0x07};
unsigned int n1=0,n2=0,n3=0,n4=0,j=0,c=0,l=0,ctd=0,pause=1,i=0,decimo_segundo=10,first=0,m2,sw1=0,sw2=0;
unsigned int h1=1,h2=5,m1=4,m2=4;

// ************ FIM Var Globais *******************************************************************


//----------------------------------------------FUNCOES GERAIS-------------------------------------------------------------------

void delay_system(float mS)
{
   mS = (mS/1000) * timer_duvidoso_mili_80MHz;
   while(mS > 0)
   mS--;
}

void habilita_clockGPIO(uint32_t portalGPIO)
{
    ESC_REG(SYSCTL_RCGCGPIO)|=portalGPIO;
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

uint32_t GPIO_leitura(uint32_t portal, uint8_t pino)
{
    return (ESC_REG(portal + (GPIO_O_DATA+(pino<<2))));
}

void GPIO_escrita(uint32_t portal, uint8_t pino, uint8_t valor)
{
    ESC_REG(portal + (GPIO_O_DATA+(pino<<2)))=valor;
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
    GPIO_escrita(portalE_base, pino0|pino1|pino2|pino3, vector_numbers[i]);
    GPIO_escrita(portalC_base, pino4|pino5|pino6|pino7, vector_numbers[i]);
}

void digito(int i)
{
    GPIO_escrita(portalB_base, pino6|pino7, vector_digits[i]);
    GPIO_escrita(portalD_base, pino3|pino2, vector_digits[i]);

    #ifdef anodo
        GPIO_escrita(portalD_base, pino6,pino6);
    #endif

    #ifdef catodo
            GPIO_escrita(portalD_base, pino6,~pino6);
    #endif
}





#ifdef anodo

void digito_numeros_iguais(void)
{
    GPIO_escrita(portalB_base, pino6|pino7, 0x33);
    GPIO_escrita(portalD_base, pino3|pino2, 0x33);

    #ifdef anodo
        GPIO_escrita(portalD_base, pino6,pino6);
    #endif
    #ifdef catodo
            GPIO_escrita(portalD_base, pino6,~pino6);
    #endif
}

void limpa_digito(void)
{
    GPIO_escrita(portalB_base, pino6|pino7, pino6|pino7);
    GPIO_escrita(portalD_base, pino3|pino2, pino3|pino2);
}

void digito_ponto_intermitente(void)
{
    GPIO_escrita(portalD_base, pino6, ~(pino6));
}

void segmento_ponto_intermitente(void)
{
    GPIO_escrita(portalE_base, pino0|pino1|pino2,0x03); //(pino0)|(pino1)|~(pino2)
}

void limpa_diplay(void)
{
    GPIO_escrita(portalB_base, pino6|pino7, 0x00|0x00);
    GPIO_escrita(portalD_base, pino3|pino2|pino6, 0x00|0x00|pino6);
    GPIO_escrita(portalE_base, pino0|pino1|pino2|pino3, 0x00|0x00|0x00|0x00);
    GPIO_escrita(portalC_base, pino4|pino5|pino6|pino7, 0x00|0x00|0x00|0x00);
}

#endif


#ifdef catodo

void digito_numeros_iguais(void)
{
    GPIO_escrita(portalB_base, pino6|pino7, 0xCC);
    GPIO_escrita(portalD_base, pino3|pino2, 0xCC);
}

void limpa_digito(void)
{
    GPIO_escrita(portalB_base, pino6|pino7, 0x3F);
    GPIO_escrita(portalD_base, pino3|pino2, 0x73);
}

void digito_ponto_intermitente(void)
{
    GPIO_escrita(portalD_base, pino6, pino6);
}
void segmento_ponto_intermitente(void)
{
    GPIO_escrita(portalE_base, pino0|pino1|pino2|pino3, ~(0x03));
}

void limpa_diplay(void)
{
    GPIO_escrita(portalB_base, pino6|pino7, pino6|pino7);
    GPIO_escrita(portalD_base, pino3|pino2|pino6, pino3|pino2|0x00);
    GPIO_escrita(portalE_base, pino0|pino1|pino2|pino3, pino0|pino1|pino2|pino3);
    GPIO_escrita(portalC_base, pino4|pino5|pino6|pino7, pino4|pino5|pino6|pino7);
}

#endif

void pontos_intermitentes(void)
{
        limpa_digito();
        digito_ponto_intermitente();
        segmento_ponto_intermitente();
}

//----------------- DISPLAY 7 SEGMENTOS {FIM}-------------------------------------

//                      FUNCOES GERAIS PARA AS QUESTOES
void toggle(uint8_t pino)
{
    if(GPIO_leitura(portalF_base, pino)== pino)
    {
        GPIO_escrita(portalF_base, pino, ~pino);
        delay_system(200);
    }
    else
    {
        GPIO_escrita(portalF_base, pino, pino);
        delay_system(200);
    }

}

// ---------------- Interrupcao --------------------------------------------------

// Primeira coisa a ser chamada na main
void habilita_interrupcao_global(void)
{
    //passo3
    __asm( " mrs r0, PRIMASK\n"
    " cpsie i\n"
    " bx lr\n");
}

// -----------INTERRUPCAO DOS PORTAIS ---------------------------------------------

// segunda coisa para ser chamada
void habilitaInterrupcao(uint32_t ui32Interrupcao){
    //passo2
    if(ui32Interrupcao < 32){
        ESC_REG(EN0)    |= 1<<ui32Interrupcao;
    }
}

// terceira coisa a ser chamada
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
//quarta coisa a ser chamada
void habilitaInt_GPIO(uint32_t portal, uint8_t pino){
    //passo1 Device Arm
    ESC_REG(portal + GPIO_O_IM) |= pino;
}

void desabilitaInt_GPIO(uint32_t portal, uint8_t pino){
    ESC_REG(portal + GPIO_O_IM) &= ~(pino);
}
// --------------- INTERRUPCOES PORTAIS FIM-------------------

// -------------- SYSTICK -------------------------
// Segunda coisa a ser chamada na main
void habilitaSystick(void){
    ESC_REG(NVIC_ST_CTRL) |= NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_ENABLE;
}
// Terceira coisa a ser chama na main
void habilitaIntSystick(void){
    ESC_REG(NVIC_ST_CTRL) |= NVIC_ST_CTRL_INTEN;
}

void desabilitaIntSystick(void){
    ESC_REG(NVIC_ST_CTRL) &= ~(NVIC_ST_CTRL_INTEN);
}
// quarta coisa a ser chamada na main
void configPeriodoSystick(uint32_t periodo){
    ESC_REG(NVIC_ST_RELOAD) = periodo - 1;
}
// ------------ SYSTICK FIM -------------------------

// chamar sempre no inicio de uma trata
void limpaInt_GPIO(uint32_t portal, uint8_t pino)
{
    ESC_REG(portal + GPIO_O_ICR) = pino;
}


// ----------- TRATA SYSTICK/ INTERRUPCAO / TIMER


void Systick_debouncer(void)
{
    habilitaSystick();
    habilitaIntSystick();
    configPeriodoSystick(systick_debouncer_number);
}

void trataIntGPIOF(void)
{
#ifdef cronometro_on

    // primeira coisa: descobrir qual foi a fonte da interrupção
    desabilitaIntSystick();
    limpaInt_GPIO(portalF_base, pino4|pino0);
    if(GPIO_leitura(portalF_base,pino4)!= pino4) // botao pausar/continuar
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

    if(GPIO_leitura(portalF_base,pino0)!= pino0) // botao zerar
   {
        pause=1;
        n1=0;n2=0;n3=0;n4=0;
        delay_system(5);
   }
    habilitaSystick();
    habilitaIntSystick();
    configPeriodoSystick(systick_cronometro);

#endif




#ifdef questao_10

    limpaInt_GPIO(portalF_base, pino4|pino0);
    sw1=0,sw2=0;
    if(GPIO_leitura(portalF_base, pino4)!=pino4)
    {
        toggle(pino1);
        Systick_debouncer();
    }
    if(GPIO_leitura(portalF_base, pino0)!=pino0)
    {
        toggle(pino2);
        Systick_debouncer();
    }



#endif

}

void trataSystick(void)
{
#ifdef cronometro_on
    if(!pause)
    {
       n1++;
   }
#endif

#ifdef relogio_on
    ctd++;
        pontos_intermitentes();
        delay_system(timer_doopler);

        limpa_diplay();
        delay_system(timer_doopler);
    if(ctd==60)
    {
    ctd=0;
    m2++;
    }
#endif

#ifdef questao_10

desabilitaIntSystick();
#endif
}

// ------------ FIM DAS TRATAS
//  -------------------------- INTERRUPCAO FIM -----------------------------------

//funcoes para as questoes

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
    delay_system(timer_doopler);
}

void cronometro(void)
{
    if(n1==10)
    {
        n2++;
        n1=0;
    }
    if(n2==10)
    {
        n3++;
        n2=0;
    }
    if(n3==10)
    {
        n4++;
        n3=0;
    }
    if(n4==10)
    {
        n1=0,n2=0,n3=0,n4=0;
    }
        escreve_4_digitos(n4, n3, n2, n1);

   //}


}


void relogio(void)
{
    if(h2==10 && (h1 == 0 || h1 == 1) )
    {
        h1++;
        h2=0;
    }
    else if (h1 == 2 && h2==5)
    {
        h1++;
    }

    if(m1==7)
    {
        h2++;
        m1=0;
    }
    if(m2==10)
    {
        m1++;
        m2=0;
    }
        escreve_4_digitos(h1, h2, m1, m2);
}




int main(void)
{
    //m2=6;
    volatile uint32_t ui32Loop;
    habilita_clockGPIO(portalGPIO_e|portalGPIO_c|portalGPIO_d | portalGPIO_b| portalGPIO_f);

    // Faz leitura dummy para efeito de atraso
    ui32Loop = ESC_REG(SYSCTL_RCGC2_R);

    //Pinos do display
    configuraPino_saida(portalC_base,pino4|pino5|pino6|pino7);
    configuraPino_saida(portalE_base,pino0|pino1|pino2|pino3);
    configuraPino_saida(portalB_base, pino6|pino7);
    configuraPino_saida(portalD_base, pino2|pino3|pino6);

    //configura pino botao sw1

    unlock_GPIO(portalF_base);

    configuraPino_entrada(portalF_base,pino0|pino4);
    ESC_REG(portalF_base+GPIO_O_PUR) |= pino0|pino4;

    lock_GPIO(portalF_base);

    configuraPino_saida(portalF_base, pino1|pino2);

    habilita_interrupcao_global();

    habilitaInterrupcao(int_port_F);
    configInt_GPIO(portalF_base, pino4|pino0, GPIO_FallingEdge);
    habilitaInt_GPIO(portalF_base, pino4|pino0);

    while(1)
    {

    }
}

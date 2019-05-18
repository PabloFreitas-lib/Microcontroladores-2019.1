#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

//definicoes para o ifdef

//#define anodo //Placa Preta ou display 7segmentos branco
#define catodo  // Placa verde ou display 7segmentos preto
#define matrix

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
                                                                                                                 //U
unsigned int vector_numbers[18]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,0x3E,0x31};
unsigned int vector_digits[4]={0x8C,0x4C,0xC8,0xC4}; // sinal baixo funciona
int um_minuto_anodo = 3000;

#endif

#ifdef catodo

unsigned int vector_numbers[18]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E,~0x3E,~0x31};
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


// **** VAR Globais ************************
int systick_cronometro = 100000,systick_relogio = 16000000,systick_debouncer_number =150000;

const float timer_duvidoso_mili_80MHz = 3800000;  // ~um segundo
const float timer_doopler = 0.35;

//usado para varredura da matrix de botao se os botoes que representa as linhas estivem de PULLUP
int vector_matrix[5] = {0x23,0x32,0x31,0x11};
unsigned int n1=0,n2=0,n3=0,n4=0,j=0,c=0,l=0,ctd=0,pause=1,i=0,decimo_segundo=10,first=0,m2,sw1=0,sw2=0;
unsigned int h1=1,h2=5,m1=4,m2=4;
int number_grande;
int n1_t,n2_t,n3_t,n4_t;
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
    GPIOPinWrite(portalE_base, pino0|pino1|pino2|pino3, vector_numbers[i]);
    GPIOPinWrite(portalC_base, pino4|pino5|pino6|pino7, vector_numbers[i]);
}

void digito(int i)
{
    GPIOPinWrite(portalB_base, pino6|pino7, vector_digits[i]);
    GPIOPinWrite(portalD_base, pino3|pino2, vector_digits[i]);

    #ifdef anodo
        GPIOPinWrite(portalD_base, pino6,pino6);
    #endif

    #ifdef catodo
            GPIOPinWrite(portalD_base, pino6,~pino6);
    #endif
}

#ifdef anodo


void limpa_digito(void)
{
    GPIOPinWrite(portalB_base, pino6|pino7, pino6|pino7);
    GPIOPinWrite(portalD_base, pino3|pino2, pino3|pino2);
}


void limpa_diplay(void)
{
    GPIOPinWrite(portalB_base, pino6|pino7, 0x00|0x00);
    GPIOPinWrite(portalD_base, pino3|pino2|pino6, 0x00|0x00|pino6);
    GPIOPinWrite(portalE_base, pino0|pino1|pino2|pino3, 0x00|0x00|0x00|0x00);
    GPIOPinWrite(portalC_base, pino4|pino5|pino6|pino7, 0x00|0x00|0x00|0x00);
}

#endif

#ifdef catodo


void limpa_digito(void)
{
    GPIOPinWrite(portalB_base, pino6|pino7, ~pino6|~pino7);
    GPIOPinWrite(portalD_base, pino3|pino2, ~pino3|~pino2);
}


void limpa_diplay(void)
{
    GPIOPinWrite(portalB_base, pino6|pino7, pino6|pino7);
    GPIOPinWrite(portalD_base, pino3|pino2|pino6, pino3|pino2|~pino6);
    GPIOPinWrite(portalE_base, pino0|pino1|pino2|pino3, pino0|pino1|pino2|pino3);
    GPIOPinWrite(portalC_base, pino4|pino5|pino6|pino7, pino4|pino5|pino6|pino7);
}

#endif
//----------------- DISPLAY 7 SEGMENTOS {FIM}-------------------------------------


//                      FUNCOES GERAIS PARA AS QUESTOES


// ---------------- Interrupcao --------------------------------------------------

void habilitaInterrupcao(uint32_t ui32Interrupcao){
    //passo2
    if(ui32Interrupcao < 32){
        ESC_REG(EN0)    |= 1<<ui32Interrupcao;
    }
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


void erro(void)
{
    while(1)
    escreve_4_digitos(14,17,17,0);
}

void over(void)
{
    while(1)
    escreve_4_digitos(0,16,14,17);
}

void conversao_hexa_decimal(void) // hexa --> decimal numero max 270F ~ 9999
{
    if(n1 >2 || (n1==2 && n2>7) || (n1==2 && n2==7 && n3>0))
    {
       over();
    }

    number_grande = n1*(16*16*16)+ n2*(16*16)+ n3*(16)+ n4;

    n1=number_grande/1000;
    number_grande = number_grande%1000;

    n2=number_grande/100;
    number_grande = number_grande%100;

    n3=number_grande/10;
    number_grande = number_grande%10;

    n4=number_grande;
    //number_grande = number_grande%1000;


}

void conversao_decimal_hexa(void) // decimal --> hexa
{
    if(n1>9 || n2>9 || n3 >9 || n4 > 9 )
    {
        over();
    }
    number_grande = (n1 * (10 * 10 * 10) + n2 * (10 * 10) + n3 * 10 + n4);
    n4 = number_grande % 16;
    n3 = (number_grande%(16*16)-n1)/16;
    n2 = (number_grande %(16*16*16)-n1-n2)/(16*16);
    n1 = (number_grande-n1-n2-n3)/(16*16*16);

}

void trataIntGPIOF(void)
{
    desabilitaIntSystick();
    limpaInt_GPIO(portalF_base, pino0|pino1|pino2|pino3|pino4);
    if(ctd>=4)
    {
        if( GPIOPinRead(portalF_base, pino1)!= pino1 || GPIOPinRead(portalF_base, pino2)!= pino2 || GPIOPinRead(portalF_base, pino3)!= pino3 )
        {
            erro();
        }
        if(GPIOPinRead(portalF_base, pino0)!= pino0)
        {
            conversao_hexa_decimal();
            delay_system(100);
        }

        if(GPIOPinRead(portalF_base, pino4)!= pino4)
        {
            conversao_decimal_hexa();
            delay_system(100);
        }

    }
    else
    {


    if(!first)
    {
        if(GPIOPinRead(portalF_base, pino0)!= pino0)
        {
            n4=4*c;
            first=1;
            ++ctd;
            n1_t = n1;n2_t = n2;n3_t = n3;n4_t = n4;
        }
        if(GPIOPinRead(portalF_base, pino1)!= pino1)
      {
          n4=(4*c)+1;
          first=1;
          ++ctd;
      }
      if(GPIOPinRead(portalF_base, pino2)!= pino2)
      {
          n4=(4*c)+2;
          first=1;
          ++ctd;
      }
      if(GPIOPinRead(portalF_base, pino3)!= pino3)
      {
          n4=(4*c)+3;
          first=1;
          ++ctd;
      }
    }
    else
    {
        if(GPIOPinRead(portalF_base, pino0)!= pino0)
        {
            n1=n2;n2=n3;n3=n4;
            n4=4*c;
            ++ctd;
            n1_t = n1;n2_t = n2;n3_t = n3;n4_t = n4;
        }
        if(GPIOPinRead(portalF_base, pino1)!= pino1)
        {
            n1=n2;n2=n3;n3=n4;
            n4=(4*c)+1;
            ++ctd;
        }
        if( GPIOPinRead(portalF_base, pino2)!= pino2)
        {
            n1=n2;n2=n3;n3=n4;
            n4=(4*c)+2;
            ++ctd;
        }
        if(GPIOPinRead(portalF_base, pino3)!= pino3)
        {
            n1=n2;n2=n3;n3=n4;
            n4=(4*c)+3;
            ++ctd;
        }
    }
    delay_system(60); // debouncer para todos as interrupc

    habilitaSystick();
    habilitaIntSystick();
    configPeriodoSystick(systick_cronometro);
    }


}


// ------------ FIM DAS TRATAS
//  -------------------------- INTERRUPCAO FIM -----------------------------------

//funcoes para as questoes



/*
 * Feito o uso da matrix de botao e com armazenamento do ultimo digito e deslocamento para esquerda
 */

void trataSystick(void)
{
    if(ctd>=4)
    {

    }
    else
    {
       c++;
       if(c==4)
       {
           c=0;
       }
       GPIOPinWrite(portalF_base, pino4, vector_matrix[c]);
       GPIOPinWrite(portalB_base, pino0|pino1|pino5, vector_matrix[c]);
    }
}



int main(void)
{
    volatile uint32_t ui32Loop;
    //habilita_clockGPIO(portalGPIO_e|portalGPIO_c|portalGPIO_d | portalGPIO_b| portalGPIO_f);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Faz leitura dummy para efeito de atraso
    ui32Loop = ESC_REG(SYSCTL_RCGC2_R);

    //Pinos do display
    GPIOPinTypeGPIOOutput(portalC_base,pino4|pino5|pino6|pino7);
    GPIOPinTypeGPIOOutput(portalE_base,pino0|pino1|pino2|pino3);
    GPIOPinTypeGPIOOutput(portalB_base, pino6|pino7);
    GPIOPinTypeGPIOOutput(portalD_base, pino2|pino3|pino6);

    //configura pino botao sw1

#ifdef matrix

    unlock_GPIO(portalF_base);
    // pinos matrix de botao
    GPIOPinTypeGPIOOutput(portalF_base,pino4);
    GPIOPinTypeGPIOOutput(portalB_base,pino0|pino1|pino5);


    GPIOPinTypeGPIOInput(portalF_base, pino0|pino1|pino2|pino3);
    GPIOPadConfigSet(portalF_base,pino0|pino1|pino2|pino3,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);

    lock_GPIO(portalF_base);
#endif

    IntMasterEnable();

    habilitaInterrupcao(int_port_F);
    GPIOIntTypeSet(portalF_base, GPIO_INT_PIN_0|GPIO_INT_PIN_1|GPIO_INT_PIN_2|GPIO_INT_PIN_3, GPIO_FALLING_EDGE);
    GPIOIntEnable(portalF_base, GPIO_INT_PIN_0|GPIO_INT_PIN_1|GPIO_INT_PIN_2|GPIO_INT_PIN_3);


    habilitaSystick();
    habilitaIntSystick();
    configPeriodoSystick(systick_cronometro);

    while(1)
    {
        if(ctd>=4)
        {
            GPIOIntDisable(portalF_base,pino1|pino2|pino3);
              unlock_GPIO(portalF_base);

              GPIOPinTypeGPIOInput(portalF_base, pino0|pino4);
              GPIOPadConfigSet(portalF_base,pino0|pino4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);

              lock_GPIO(portalF_base);


              GPIOIntTypeSet(portalF_base, GPIO_INT_PIN_4 | GPIO_INT_PIN_0, GPIO_FALLING_EDGE);
              GPIOIntEnable(portalF_base, GPIO_INT_PIN_4 | GPIO_INT_PIN_0);
        }
        escreve_4_digitos(n1,n2,n3,n4);
    }
}

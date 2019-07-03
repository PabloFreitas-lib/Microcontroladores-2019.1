#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//definicoes para o ifdef

//#define anodo //Placa Preta ou display 7segmentos branco
#define catodo  // Placa verde ou display 7segmentos preto
//#define botao_sw2
//#define matrix

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

const float timer_duvidoso_mili_80MHz = 3800000;  // ~um segundo
const float timer_doopler = 0.35;

//usado para varredura da matrix de botao se os botoes que representa as linhas estivem de PULLUP
int vector_matrix[4] = {0x0E,0x0D,0x0B,0x07};
unsigned int n1=0,n2=0,n3=0,n4=0,j=0,c=0,l=0,pause=1,i=0,decimo_segundo=10,first=0;




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
}





#ifdef anodo

void digito_numeros_iguais(void)
{
    GPIO_escrita(portalB_base, pino6|pino7, ~0xCC);
    GPIO_escrita(portalD_base, pino3|pino2, ~0xCC);
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


// -------------------- QUESTOES DO LAB_03 ---------------------------------------

/* QUESTAO 2
 * Com esta tabela faça com que todos esses números sejam apresentados
 * no display de sete segmentos, de forma sequencial e em todos os 4
 * dígitos ao mesmo tempo.
*/
void questao2(void)
{
    for(i =0; i < 16;i++)
    {
        digito_numeros_iguais();
        numero(i);
        delay_system(100);
    }
}
/* Questao4
 * Faça então com que apareça um número crescente no display, de forma
 * ao display indicar o valor armazenado em uma variável.
 */
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
/* Questao 3
 * Verificado os números, faça a multiplexação dos dígitos utilizando um
 * delay qualquer que possibilite a escrita de diferentes números em cada
 * um dos dígitos do display
 *
 */
void questao3(void)
{
escreve_4_digitos(4, 3, 2, 1);
}



/* QUESTAO 5
 * Programe a variável do item anterior para ser incrementada apenas
 * quando o botão SW1 for acionado.
 */

void sw1_incremento(void)
{

limpa_diplay();
delay_system(timer_doopler);
    if(n1==16)
    {
        n2++;
        n1=0;
    }
    if(n2==16)
    {
        n3++;
        n2=0;
    }
    if(n3==16)
    {
        n4++;
        n3=0;
    }
    if(n4==16)
    {
        n1=0;n2=0;n3=0;n3=0;
    }
    if(GPIO_leitura(portalF_base,pino4)!= pino4 )
    {
        n1++;
        delay_system(50); // primeira tecnica de debauncer questao 6
    }
    escreve_4_digitos(n4, n3, n2, n1);
}

/*
 * Altere seu programa para utilização da matriz de botões, o valor
 * indicado no último dígito do display deverá ser correspondente ao botão
 * pressionado seguindo a ordem da figura abaixo, os outros dígitos
 * deverão ficar apagados.
 */
void questao_8(void)
{
    digito(3); // ultimo digito
    numero(n4);
        for(c=0;c<4;c++)
        {
            for(l=0;l<4;l++)
            {
                GPIO_escrita(portalF_base, pino0|pino1|pino2|pino3, vector_matrix[c]);

                if(l==0 && GPIO_leitura(portalF_base, pino4)!= pino4)
                {
                  n4=c;
                  delay_system(50); // deboucer tecnica 1
                }
                if(l==1 && GPIO_leitura(portalB_base, pino0)!= pino0)
                {
                    n4=c+4;
                    delay_system(50);// deboucer tecnica 1
                }
                if(l==2 && GPIO_leitura(portalB_base, pino1)!= pino1)
                {
                    n4=c+8;
                    delay_system(50);// deboucer tecnica 1
                }
                if(l==3 && GPIO_leitura(portalB_base, pino5)!= pino5)
                {
                    n4=c+12;
                    delay_system(50);// deboucer tecnica 1
                }
            }
        }
}


/*
 * Por definicao o cronometro comeca pausado, para comecar a contagem é necessario apertar o sw1.
 * Vendo assim é valido a verificacao no comeco da main se o sw1 esta programado como saida e ver
 * se ta como Pullup ou Pulldown.
 * Foi utilizado somente tecnica de debouncer to tipo 1.
 * Para achar o decimo de segundo foi feito na tentativa e erro.
 *  */
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
        for(i=0;i<decimo_segundo;i++)
        {
        escreve_4_digitos(n4, n3, n2, n1);
        }

            //pontos_intermitentes();
            //delay_system(timer_doopler);

            //limpa_diplay();
            //delay_system(timer_doopler);

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
   //}

   if(!pause)
   {
       n1++;
   }
}


void relogio(int n4,int n3,int n2,int n1)
{
    while(1){

    if(n3==10 && (n4 == 0 || n4 == 1) )
    {
        n4++;
        n3=0;
    }
    else if (n4 == 2 && n3==5)
    {
        n4++;
    }

    if(n2==7)
    {
        n3++;
        n2=0;
    }
    if(n1==10)
    {
        n2++;
        n1=0;
    }

    for (j=0; j<6380; j++)
    {
        escreve_4_digitos(n4, n3, n2, n1);
        if(j%25 == 0)
        {
            pontos_intermitentes();
            delay_system(timer_doopler);

            limpa_diplay();
            delay_system(timer_doopler);
        }
    }
    n1++;
    }
}

/*
 * Feito o uso da matrix de botao e com armazenamento do ultimo digito e deslocamento para esquerda
 */
void matrix_botao(void)
{
    while(1)
    {
        if(!first)
        {
            for(c=0;c<4;c++) for (j=0; j<50; j++)
            {
                    {
                        for(l=0;l<4;l++)
                        {
                            GPIO_escrita(portalF_base, pino0|pino1|pino2|pino3, vector_matrix[c]);
                            if(l==0 && GPIO_leitura(portalF_base, pino4)!= pino4)
                            {
                                n4=c;
                                first=1;
                                delay_system(50);
                            }
                            if(l==1 && GPIO_leitura(portalB_base, pino0)!= pino0)
                            {
                                n4=c+4;
                                first=1;
                                delay_system(50);
                            }
                            if(l==2 && GPIO_leitura(portalB_base, pino1)!= pino1)
                            {
                                n4=c+8;
                                first=1;
                                delay_system(50);
                            }
                            if(l==3 && GPIO_leitura(portalB_base, pino5)!= pino5)
                            {
                                n4=c+12;
                                first=1;
                                delay_system(50);
                            }
                        }
                    }
            }
        }
        else
        {
            for(c=0;c<4;c++)
            {
                escreve_4_digitos(n1,n2,n3,n4);
                for(l=0;l<4;l++)
                {
                    GPIO_escrita(portalF_base, pino0|pino1|pino2|pino3, vector_matrix[c]);
                    if(l==0 && GPIO_leitura(portalF_base, pino4)!= pino4)
                    {
                        n1=n2;n2=n3;n3=n4;
                        n4=c;
                        delay_system(50);
                    }
                    if(l==1 && GPIO_leitura(portalB_base, pino0)!= pino0)
                    {
                        n1=n2;n2=n3;n3=n4;
                        n4=c+4;
                        delay_system(50);
                    }
                    if(l==2 && GPIO_leitura(portalB_base, pino1)!= pino1)
                    {
                        n1=n2;n2=n3;n3=n4;
                        n4=c+8;
                        delay_system(50);
                    }
                    if(l==3 && GPIO_leitura(portalB_base, pino5)!= pino5)
                    {
                        n1=n2;n2=n3;n3=n4;
                        n4=c+12;
                        delay_system(50);
                    }
                }
            }
        }
}
}



// ----------------------------------------------------------------- Fim das funcoes----------------------------------------------------------------------

int main(void)
 {
    volatile uint32_t ui32Loop;


    habilita_clockGPIO(portalGPIO_e|portalGPIO_c|portalGPIO_d | portalGPIO_b| portalGPIO_f);


    //funcao que pasa quanquer portal, escrevo o bit dentro deste reg
    // Faz leitura dummy para efeito de atraso
    ui32Loop = ESC_REG(SYSCTL_RCGC2_R);

    //display configuracoes
    configuraPino_saida(portalC_base,pino4|pino5|pino6|pino7);
    configuraPino_saida(portalE_base,pino0|pino1|pino2|pino3);
    configuraPino_saida(portalB_base, pino6|pino7);
    configuraPino_saida(portalD_base, pino2|pino3|pino6);





#ifdef botao_sw2
    //configura pino botao sw1
    unlock_GPIO(portalF_base);

    configuraPino_entrada(portalF_base,pino0|pino4);
    ESC_REG(portalF_base+GPIO_O_PUR) |= pino0|pino4;

    lock_GPIO(portalF_base);

#endif

#ifdef matrix

    unlock_GPIO(portalF_base);
    // pinos matrix de botao
    configuraPino_saida(portalF_base,pino0|pino1|pino2|pino3);

    configuraPino_entrada(portalF_base, pino4);
    ESC_REG(portalF_base+GPIO_O_PUR) |= pino4;
    configuraPino_entrada(portalB_base,pino0|pino1|pino5);
    ESC_REG(portalB_base+GPIO_O_PUR) |= pino0|pino1|pino5;

    lock_GPIO(portalF_base);
#endif

    // Loop principal
    while(1)
    {


    }
 }

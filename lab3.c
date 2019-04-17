#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define ESC_REG(x)                  (*((volatile uint32_t *)(x)))

#define SYSCTL_RCGC2_R              0x400FE108
#define SYSCTL_RCGC2_GPIOF          0x00000020
#define GPIO_PORTF_DIR_R            0x40025400
#define GPIO_PORTF_DEN_R            0x4002551C
#define GPIO_PORTF_DATA_R           0x400253FC
#define GPIO_PORTF_DR2R             0x40025500
#define GPIO_PORTF_PUR              0x40025510

#define SYSCTL_RCGCGPIO             0x400FE608
#define GPIOHBCTL                   0x400FE06C
#define GPIO_O_DIR                  0x400
#define GPIO_O_DR2R                 0x500
#define GPIO_O_DEN                  0x51C
#define GPIO_O_PUR                  0x510
#define GPIO_O_DATA                 0x000

#define portalGPIO_a                0x01
#define portalGPIO_b                0x02
#define portalGPIO_c                0x04
#define portalGPIO_d                0x08
#define portalGPIO_e                0x10
#define portalGPIO_f                0x20
                                         //AHB                          //APB

#define portalA_base_AHB            0x40058000                //0x40004000
#define portalB_base_AHB            0x40059000                //0x40005000
#define portalC_base_AHB            0x4005A000                //0x40006000
#define portalD_base_AHB            0x4005B000                //0x40007000
#define portalE_base_AHB            0x4005C000                //0x40024000
#define portalF_base_AHB            0x4005D000                //0x40025000

#define portalA_base                0x40004000
#define portalB_base                0x40005000
#define portalC_base                0x40006000
#define portalD_base                0x40007000
#define portalE_base                0x40024000
#define portalF_base                0x40025000


#define pino0                       0x01
#define pino1                       0x02
#define pino2                       0x04
#define pino3                       0x08
#define pino4                       0x10
#define pino5                       0x20
#define pino6                       0x40
#define pino7                       0x80

#define GPIO_O_LOCK                 0x520
#define GPIO_O_CR                   0x524
#define GPIO_LOCK_KEY               0x4C4F434B
//poderia ter passado em decimal


unsigned int vector_numbers[18]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,0x03,0x00};
unsigned int vector_digits[5]={0x8C,0x4C,0xC8,0xC4,0xCC};
//                  Display    1d    2d   3d   4d
const float timer_duvidoso_mili_80MHz = 3800000;  // ~um segundo
const float timer_doopler = 1;



//----------------------------------------------FUNCOES-------------------------------------------------------------------
void delay_system(float mS)
{

   mS = (mS/1000) * timer_duvidoso_mili_80MHz;
   while(mS > 0)
   mS--;
}

void habilita_clockGPIO(uint32_t portalGPIO)
{
    ESC_REG(SYSCTL_RCGCGPIO)|=portalGPIO;
    //nao jogar fora o conteudo anterior
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


void limpa_diplay(void)
{
    GPIO_escrita(portalB_base, pino6|pino7, 0x00|0x00);
    GPIO_escrita(portalD_base, pino3|pino2|pino6, 0x00|0x00|pino6);
    GPIO_escrita(portalE_base, pino0|pino1|pino2|pino3, 0x00|0x00|0x00|0x00);
    GPIO_escrita(portalC_base, pino4|pino5|pino6|pino7, 0x00|0x00|0x00|0x00);
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


void questao2(void)
{
    int i=0;
         for(i =0; i < 16;i++)
         {
             numero(i);
             delay_system(100);
         }
}

void  botao(void){
// primeira tecnica de debouncer
int n1=0,n2=0,n3;
               while(n2<16)
               {
                   if(n1==16){
                       n2++;
                       n1=0;
                   }
                   while(n1<16){
                       // Esse while equivale a 1000 ms de delay enquanto os dois digitos estão no display ao "mesmo tempo" , mas na verdade eles tao simutaneamente ligando e desligando no ponto que o olho humano nao percebe
                       // display [ - - n2 n2 ]
                           for (n3=0; n3<50; n3++)
                           {
                                if(GPIO_leitura(portalF_base,pino4)!= pino4 )
                                {
                                    delay_system(50);
                                    n1++;
                                }
                               digito(3);
                               numero(n1);
                               delay_system(timer_doopler);

                               limpa_diplay();
                               delay_system(timer_doopler);

                               digito(2);
                               numero(n2);
                               delay_system(timer_doopler);

                               limpa_diplay();
                               delay_system(timer_doopler);
                           }
                    }

               }
}

void questao3_4(void){
     int n1=0,n2=0,n3=0;
                //mS = 1000; //~ um segundo
                for(n2=0;n2< 16;n2++)
                {
                    for(n1=0;n1<16;n1++){
                        // Esse while equivale a 1000 ms de delay enquanto os dois digitos estão no display ao "mesmo tempo" , mas na verdade eles tao simutaneamente ligando e desligando no ponto que o olho humano nao percebe
                        // display [ - - n2 n2 ]
                            for (n3=0; n3<50; n3++)
                            {
                            digito(3);
                            numero(n1);
                            delay_system(timer_doopler);

                            limpa_diplay();
                            delay_system(timer_doopler);

                            digito(2);
                            numero(n2);
                            delay_system(timer_doopler);

                            limpa_diplay();
                            delay_system(timer_doopler);
                            }
                     }
                    n1=0;
                }
    }



void pontos_intermitentes(void)
{
       GPIO_escrita(portalB_base, pino6|pino7, pino6|pino7);
       GPIO_escrita(portalD_base, pino3|pino2, pino3|pino2);
       GPIO_escrita(portalD_base, pino6, ~(pino6));
       GPIO_escrita(portalE_base, pino0|pino1|pino2, (pino0)|(pino1)|~(pino2));
}


void cronometro(void){


            int pause = 0;
            int n1=0,n2=0,n3=0,n4=0,j=0;
                        while(n4<7) // dezena minuto 0 - 6
                        {
                            if(n3==10)
                            {
                            n4++;
                            n3=0;
                            }
                            while(n3<10) //unidade minuto 0 - 9
                            {
                                if(n2==7)
                                {
                                  n3++;
                                  n2=0;
                                }
                               while(n2<7) //segundo dezena 0 - 6
                               {
                                   if(n1==10)
                                   {
                                       n2++;
                                       n1=0;
                                   }
                                   while(n1<10) // segundos unidade 0 - 9
                                   {
                                       // display [ - - n2 n2 ]
                                           for (j=0; j<50; j++)
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
                                               if(j==30)
                                               {
                                                pontos_intermitentes();
                                                delay_system(timer_doopler);

                                                limpa_diplay();
                                                delay_system(timer_doopler);
                                                }
                                           }
                                           if(GPIO_leitura(portalF_base,pino4)!= pino4)
                                           {
                                               delay_system(15);
                                               if(!pause)
                                               {
                                                   pause=1;
                                                   //n1++;
                                               }else if(pause)
                                               {
                                                   pause=0;
                                               }
                                           }
                                           if(!pause)
                                               n1++;
                                           if(GPIO_leitura(portalF_base,pino0)!= pino0)
                                           {
                                               delay_system(15);
                                               n1=0;n2=0;n3=0;n4=0;
                                           }
                               }
                            }
                        }

                        n1=0,n2=0,n3=0,n4=0;

                        }
}




void relogio(void){


int n1=4,n2=2,n3=9,n4=0,j=0;
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
                               for (j=0; j<3000; j++)
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

                                   if(j%25 == 0)
                                   {
                                       pontos_intermitentes();
                                       delay_system(timer_doopler);

                                       limpa_diplay();
                                       delay_system(timer_doopler);
                                   }


                                   }
                               }
                               n1++;
                        }
                   }
                }
            n1=0,n2=0,n3=0,n4=0;
            }




// ----------------------------------------------------------------- Fim das funcoes----------------------------------------------------------------------

int main(void)
 {
    volatile uint32_t ui32Loop;


    habilita_clockGPIO(portalGPIO_e|portalGPIO_c|portalGPIO_d | portalGPIO_b| portalGPIO_f);


    //funcao que pasa quanquer portal, escrevo o bit dentro deste reg
    // Faz leitura dummy para efeito de atraso
    ui32Loop = ESC_REG(SYSCTL_RCGC2_R);

    configuraPino_saida(portalC_base,pino4|pino5|pino6|pino7);
    configuraPino_saida(portalE_base,pino0|pino1|pino2|pino3);
    configuraPino_saida(portalB_base, pino6|pino7);
    configuraPino_saida(portalD_base, pino2|pino3|pino6);

    // pinos matrix de botao
    //configuraPino_saida(portalF_base, pino0|pino1|pino2|pino3);

    //configuraPino_entrada(portalF_base,pino4);
    //ESC_REG(portalF_base+GPIO_O_PUR)|= pino4;



    //configura pino botao sw1
    configuraPino_entrada(portalF_base,pino4);
    ESC_REG(portalF_base+GPIO_O_PUR) |= pino4;


    unlock_GPIO(portalF_base);
    // Habilita o pino 0 do portal, configura como entrada com weak pull up
    configuraPino_entrada(portalF_base,pino0); // SW2
    ESC_REG(portalF_base+GPIO_O_PUR)|= pino0;
    lock_GPIO(portalF_base);


    // Loop principal
    while(1)
    {

        // Atraso
    for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++){}
           //digito(0);
           //numero(1);
           relogio();
    }
 }




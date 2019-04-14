#include <stdint.h>
#include <string.h>

//#define question2
//#define question3
//#define question4
#define ESC_REG(x)                  (*((volatile uint32_t *)(x)))

#define REAL

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
unsigned int vector_numbers[17]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,
                         0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,0x03};
unsigned int vector_digits[3]={0xC8,0x4C,0x8C,0xC4};
//                  Display    1d    2d   3d   4d
const float timer_duvidoso_mili_80MHz = 3800000;

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

void numero_digitado(char *a)
{
int tam = strlen(a);
    if(tam == 1)
    {
        digito(3);numero(0);digito(2);numero(0);digito(1);numero(0);digito(0);numero(a[0]-48);

    }else if(tam == 2)
    {
        digito(3);numero(0);digito(2);numero(0);digito(1);numero(a[0]-48);digito(0);numero(a[1]-48);

    }else if(tam == 3)
    {
        digito(3);numero(0);digito(2);numero(a[0]-48);digito(1);numero(a[1]-48);digito(0);numero(a[2]-48);
    }else if(tam == 4)
    {
        digito(3);numero(a[0]-48);digito(2);numero(a[1]-48);digito(1);numero(a[2]-48);digito(0);numero(a[3]-48);
    }
}

// somente le numeros de 1 - 9
int * le_display(void)
{
int num[4],n;

    for(d=0;d<4;d++)
    {
        for(n =0;n<10;n++)
        {
            if(GPIO_leitura(portalB_base,pino6|pino7)!=vector_digits[d] && GPIO_leitura(portalD_base,pino3|pino2)!=vector_digits[d])
            {
                if((GPIO_leitura(portalE_base,pino0|pino1|pino2|pino3)!=vector_numbers[n]) &&  (GPIO_leitura(portalC_base,pino4|pino5|pino6|pino7)!=vector_numbers[n]))
                {
                   num[d] = i;
                }
            }
        }
    }
    return(num);
}


int main(void)
 {
    volatile uint32_t ui32Loop;


    habilita_clockGPIO(portalGPIO_e|portalGPIO_c|portalGPIO_d | portalGPIO_b);


    //funcao que pasa quanquer portal, escrevo o bit dentro deste reg
    // Faz leitura dummy para efeito de atraso
    ui32Loop = ESC_REG(SYSCTL_RCGC2_R);

    configuraPino_saida(portalC_base,pino4|pino5|pino6|pino7);
    configuraPino_saida(portalE_base,pino0|pino1|pino2|pino3);
    configuraPino_saida(portalB_base, pino6|pino7);
    configuraPino_saida(portalD_base, pino2|pino3);

    //configura pino botao sw1
    configuraPino_entrada(portalF_base,pino4);
    ESC_REG(portalF_base+GPIO_O_PUR)|= pino4;



    // Loop principal
    while(1)
    {

        // Atraso
        for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++){}

    #ifdef question2
     int n;
     // comeca tudo com 0
     digito(0);numero(0);digito(1);numero(0);digito(2);numero(0);digito(3);numero(0);
     delay_system(1000);

         for( n= 0;n < 13;n++)
         {
             digito(0);numero(n);digito(1);numero(n+1);digito(2);numero(n+2);digito(3);numero(n+3);
             delay_system(1000);
         }
    #endif

    #ifdef question4
    char num[5] = "9999"
            // comeca tudo com 0
            digito(0);numero(0);digito(1);numero(0);digito(2);numero(0);digito(3);numero(0);
            delay_system(1000);

            numero_digitado(num);
            delay_system(1000);
    #endif

    #ifdef question5
            int num[5] = le_display();
    #endif
    }

}



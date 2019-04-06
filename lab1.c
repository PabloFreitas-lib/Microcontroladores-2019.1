#include <stdint.h>
#define REAL
#define ESC_REG(x)                  (*((volatile uint32_t *)(x)))
//#define WHREG(x)                    (*((volatile uint32_t *)(x)))
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


void habilita_clockGPIO(uint32_t portalGPIO)
{
    ESC_REG(SYSCTL_RCGCGPIO)|=portalGPIO;
    //nao jogar fora o conteudo anterior
}

void habilita_AHB(uint32_t portalGPIO)
{
    ESC_REG(GPIOHBCTL)|=portalGPIO;
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

uint32_t GPIO_Leitura(uint32_t portal, uint8_t pino)
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
int main(void)
{
    volatile uint32_t ui32Loop;

    // Habilita o portal de GPIO F
    habilita_clockGPIO(portalGPIO_f);
    habilita_AHB(portalGPIO_f);

    //funcao que pasa quanquer portal, escrevo o bit dentro deste reg
    // Faz leitura dummy para efeito de atraso
    ui32Loop = ESC_REG(SYSCTL_RCGC2_R);

    // Habilita o pino 3 do portal F, configura como saída digital
    configuraPino_saida(portalF_base_AHB,pino1); // red
    configuraPino_saida(portalF_base_AHB,pino3); // green
    configuraPino_saida(portalF_base_AHB,pino2); // blue


    // Habilita o pino 4 do portal, configura como entrada com weak pull up
    configuraPino_entrada(portalF_base_AHB,pino4); // SW1
    ESC_REG(portalF_base_AHB+GPIO_O_PUR)|= pino4;

    unlock_GPIO(portalF_base_AHB);

    // Habilita o pino 0 do portal, configura como entrada com weak pull up
    configuraPino_entrada(portalF_base_AHB,pino0); // SW2
    ESC_REG(portalF_base_AHB+GPIO_O_PUR)|= pino0;

    lock_GPIO(portalF_base_AHB);

    // Loop principal
    while(1)
    {

        // Atraso
        for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++){}
#ifdef REAL
        if (GPIO_Leitura(portalF_base_AHB, pino4) != pino4) // (ESC_REG (GPIO_PORTF_DATA_R) & 0x10) == 0x10 CHECKS SW1
        {
            GPIO_escrita(portalF_base_AHB, pino3, 0xFF); //;ESC_REG(GPIO_PORTF_DATA_R)|= 0x08
        }
        else
        {
            GPIO_escrita(portalF_base_AHB, pino3, 0x00);//; ESC_REG(GPIO_PORTF_DATA_R)&= ~(0x08)
        }
        // SW2
        if (GPIO_Leitura(portalF_base_AHB, pino0) != pino0)
        {
            GPIO_escrita(portalF_base_AHB, pino2, 0xFF); //;ESC_REG(GPIO_PORTF_DATA_R)|= 0x08

        }
        else
        {
            GPIO_escrita(portalF_base_AHB, pino2, 0x00); //;ESC_REG(GPIO_PORTF_DATA_R)|= 0x08
        }

        if ((GPIO_Leitura(portalF_base_AHB, pino0) != pino0) && (GPIO_Leitura(portalF_base_AHB, pino4) != pino4))
        {

            GPIO_escrita(portalF_base_AHB, pino1, 0xFF); //;ESC_REG(GPIO_PORTF_DATA_R)|= 0x08
            GPIO_escrita(portalF_base_AHB, pino3, 0x00);//; ESC_REG(GPIO_PORTF_DATA_R)&= ~(0x08)
            GPIO_escrita(portalF_base_AHB, pino2, 0x00); //;ESC_REG(GPIO_PORTF_DATA_R)|= 0x08
        }
        else
        {
            GPIO_escrita(portalF_base_AHB, pino1, 0x00); //;ESC_REG(GPIO_PORTF_DATA_R)|= 0x08
        }
#endif

#ifdef TESTE
        if (GPIO_Leitura(portalF_base_AHB, pino4) != pino4  && GPIO_Leitura(portalF_base_AHB, pino0) != pino0 )
                                       {
                                           GPIO_escrita(portalF_base_AHB, pino1, 0xFF);//; ESC_REG(GPIO_PORTF_DATA_R)&= ~(0x08)
                                           GPIO_escrita(portalF_base_AHB, pino2, 0x00);//; ESC_REG(GPIO_PORTF_DATA_R)&= ~(0x08)
                                           GPIO_escrita(portalF_base_AHB, pino3, 0x00);//; ESC_REG(GPIO_PORTF_DATA_R)&= ~(0x08)


                                       }

        else if (GPIO_Leitura(portalF_base_AHB, pino4) != pino4 ) // (ESC_REG (GPIO_PORTF_DATA_R) & 0x10) == 0x10 CHECKS SW1
                {
                    GPIO_escrita(portalF_base_AHB, pino3, 0xFF); //;ESC_REG(GPIO_PORTF_DATA_R)|= 0x08
                    GPIO_escrita(portalF_base_AHB, pino1, 0x00);//; ESC_REG(GPIO_PORTF_DATA_R)&= ~(0x08)

                }
                else if ( GPIO_Leitura(portalF_base_AHB, pino0) != pino0 )
                {
                    GPIO_escrita(portalF_base_AHB, pino2, 0xFF);//; ESC_REG(GPIO_PORTF_DATA_R)&= ~(0x08)
                    GPIO_escrita(portalF_base_AHB, pino1, 0x00);//; ESC_REG(GPIO_PORTF_DATA_R)&= ~(0x08)

                }
                else{
                    GPIO_escrita(portalF_base_AHB, pino1, 0x00);//; ESC_REG(GPIO_PORTF_DATA_R)&= ~(0x08)
                    GPIO_escrita(portalF_base_AHB, pino2, 0x00);//; ESC_REG(GPIO_PORTF_DATA_R)&= ~(0x08)
                    GPIO_escrita(portalF_base_AHB, pino3, 0x00);//; ESC_REG(GPIO_PORTF_DATA_R)&= ~(0x08)

                }
#endif
    }

 }

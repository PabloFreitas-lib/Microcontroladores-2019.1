#include <stdint.h>

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
    habilita_clockGPIO(portalGPIO_e|portalGPIO_c|portalGPIO_d | portalGPIO_b);
    //habilita_AHB(portalGPIO_f);

    //funcao que pasa quanquer portal, escrevo o bit dentro deste reg
    // Faz leitura dummy para efeito de atraso
    ui32Loop = ESC_REG(SYSCTL_RCGC2_R);

    // Habilita o pino 3 do portal F, configura como saída digital
    configuraPino_saida(portalC_base,pino4|pino5|pino6|pino7);
    configuraPino_saida(portalE_base,pino0|pino1|pino2|pino3);
    configuraPino_saida(portalB_base, pino6|pino7);
    configuraPino_saida(portalD_base, pino2|pino3);


    // Habilita o pino 4 do portal, configura como entrada com weak pull up
    //configuraPino_entrada(portalF_base_AHB,pino4); // SW1
    //ESC_REG(portalF_base_AHB+GPIO_O_PUR)|= pino4;

    //unlock_GPIO(portalF_base_AHB);

    // Habilita o pino 0 do portal, configura como entrada com weak pull up
    //configuraPino_entrada(portalF_base_AHB,pino0); // SW2
    //ESC_REG(portalF_base_AHB+GPIO_O_PUR)|= pino0;

    //lock_GPIO(portalF_base_AHB);

    // Loop principal
    while(1)
    {

        // Atraso
        for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++){}
        GPIO_escrita(portalB_base, pino6|pino7, (pino6|pino7));
        GPIO_escrita(portalD_base, pino3|pino2, ~(pino3)|pino2);
        GPIO_escrita(portalE_base, pino1|pino2, (pino1|pino2));
    }

}

/*
 * Questoes teoricas:
 * =========================================================================
 * 1. Qual a vantagem e desvantagem de utilizar a configuração de corrente
 * de saída com a mínima de 2mA? E com a máxima de 8mA?
 *
 *  Resposta:
 *  2mA
 *      Vantagens: Economia de energia no sistema, limitar a corrente de saída.
 *   Desvantagens : pode ser uma corrente insuficiente para algumas aplicações.
 *
 *  8mA
 *      Vantagens: Limitar a corrente de saída.
 *      Desvantagens : Consumo maior de energia pelo sistema.
 *==========================================================================
 * 2. Há outras possibilidades de configuração de um pino de entrada além da
 * padrão. Descreva essas possibilidades e indique uma possibilidade de uso.
 *
 *
 *  Resposta:
 *
 *  Digital -> Works with high and low signal. Our apliccation of a LED on
 *  and off was a exemple.
 *
 *  Analogica-> Continuous signal that contains time-varying quantities.
 *  Ex: Analog temperature sensors (RTD) have higher resolution (positive
 *  and negative temperature)
 *
 *  PWM-> Method of reducing the average power delivered by an electrical
 *  signal, by effectively chopping it up into discrete parts.
 *  Ex :Controlling the velocity of a motor or the brightness of a LED.
 *
 *  I2C0 or Two-Wire Interface (TWI)-> I2C is a serial communication protocol,
 *  so data is transferred bit by bit along a single wire (the SDA line).
 *  Ex :Using IMU.
 *
 *  TX RX (Hardware Serial) -> Sender and Reciever. Antenas.
 *
 *  Serial Peripheral Interface (SPI) -> Interface bus commonly used to send
 *  data between microcontrollers and small peripherals such as shift registers,
 *  sensors, and SD cards
 *
 *  =========================================================================
 * 3. O periférico utilizado na configuração dos portais é uma versão legada,
 * que não é a mais eficiente. Comente sobre o uso dos portais
 * configurados como AHB.
 *
 * Resposta:
 * The AHB bus provides better back-to-back access performance than the APB bus. (DataSheet)
 *
 *
 * 4. Os LEDs da placa são ativados através do controle sobre um transistor.
 * Como eles poderiam ser ligados diretamente ao microcontrolador?
 *
 * Resposta:
 *
 *
 *
 *
 * 5. Os pinos ligados aos botões precisam ser configurados como pull-up,
 * qual a razão para que não sejam colocados estes resistores externamente
 * na placa?
 *
 * Resposta:
 *      Se for conectado um resistor externamente na placa não será mais permitido outra
 *      configuração do pino, além de poder causar dano à placa.
 *
 */

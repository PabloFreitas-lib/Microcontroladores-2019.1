#include <stdint.h>
#include <stdbool.h>

#include "utils/ustdlib.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/debug.h"
#include "driverlib/hibernate.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/eeprom.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/uart.h"
#include "driverlib/systick.h"
#include "utils/uartstdio.h"
#include "driverlib/timer.h"
#include "inc/tm4c123gh6pm.h"


#include "Nokia5110.h"
#include "bild.h"

#define ESC_REG(x)                  (*((volatile uint32_t *)(x)))


//serve para da permissao de uso do pino0 do portalF
#define GPIO_O_LOCK                 0x520
#define GPIO_O_CR                   0x524
#define GPIO_LOCK_KEY               0x4C4F434B



int i=0,j=0;

int btw_towers = 21;    // Espacamento entre cada torre de 21 pixels

int dG = 22;            // Disco grande de 22 pixels
int dM = 16;            // Disco medio 16 pixels
int dP = 10;            // Disco pequeno 10 pixels

int bottle = 42;
int middle = 40;
int top    = 38;

/* Posicoes no eixo y da torre
 * Altura de cada torre [23-43] pixels
 *
 *          ________________________
 *          ||                      ||
 *          ||     |    |     |     ||
 *--> 38-39 ||     |    |     |     || top
 *--> 40-41 ||     |    |     |     || middle
 *--> 42-43 ||_____|____|_____|_____|| bottle
 *           <-21-><-21-><-21-><-21->   21*4 = 84
 *
 *    |    1   0   0   |
 *    |    2   0   0   |
 *    |    3   0   0   |
 *    O um representa o dP o dois o dM e o tres o dG respectivamente.
 */

int matriz_torre[3][3];

int curso_setinha = -1; // analogo a posicao de qual torre a setinha ou disco se encontra
int vector_matrix[5] = {0x23,0x32,0x31,0x11}; // utilizado na matriz de botao
int c=0; // incrementador da matriz de botão para multiplexar
int systick_decimo_segundos =100000; // faixa com que muda a linha da verificação da matriz e tambem atualizacao da qtd_movimentos

int buffer_status = 0,tamanho_buffer=0,posicao_y_buffer = 0; // conceito de buffer utilizado para servir de armazenamento de informacoes
int debouncer = 15; // divisor do getclock par servir de estimativa de delay
int setinha_on = 0; // criado para corrigir o erro de comecar o jogo apertando o botao de catar disco
int qtd_movimentos = 0; // Selecionar a peca já contara com movimento mesmo que coloque no mesmo lugar

// unlock GPIOLOCK register using direct register programming

/*
 * Utilizado para acessar a PF0
 * argumentos :
 * -> Base do Portal ex: GPIO_PORTF_BASE
 */

void unlock_GPIO(uint32_t portal)
{
       ESC_REG(portal + GPIO_O_LOCK) = GPIO_LOCK_KEY;
       ESC_REG(portal + GPIO_O_CR) = 0x01;
}

/*
 * Utilizado para fechar a possibilidade de mudanca na PF0
 * argumentos :
 * -> Base do Portal ex: GPIO_PORTF_BASE
 */

void lock_GPIO(uint32_t portal)
{
    ESC_REG(portal + GPIO_O_LOCK) = 0;
}

/*
 * Utilizado para configurar os botões da matriz de botões como pullUp, maximo de 2mA
 */

void configura_botoes(void)
{
    unlock_GPIO(GPIO_PORTF_BASE);

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_4);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_5);

    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);

    lock_GPIO(GPIO_PORTF_BASE);
}


/*
 * Libera o clock para os portais GPIO de a B até F
 */
void Clock_B_F(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}
/*------------------------------- Funcoes para o jogo -----------------------------*/

/*
 * Verifica se tore esta vazia
 * retorna 1 se tiver vazia
 * retorna 0 se não estiver vazia
 * Argumentos:
 * Torre: podendo ser 0(primeira) 1(segunda) 2(terceira)
 */
int torre_vazia(int torre)
{
    for(i=0;i<3;i++)
    {
        if(matriz_torre[i][torre] != 0)
            return(0);
    }
    return(1);
}

/*
 * Retorna a posicao_y primeira que encontrar vendo a torre de cima para baixo:
 * Podendo ser Top middle ou bottle
 * Argumentos:
 * Torre: podendo ser 0(primeira) 1(segunda) 2(terceira)
 */

int posicao_disco_matriz(int torre)
{
        for(i=0;i<3;i++)
        {

            if(matriz_torre[i][torre] != 0)
            {
                //Verifica a posicao do disco
                if(i==0)
                {
                    return(top);
                }
                if(i==1)
                {
                    return(middle);
                }
                if(i==2)
                {
                    return(bottle);
                }
            }
        }
        return(-1);
}

/*
 * Retorna o tamanho do primeiro disco que encontrar vendo a torre de cima para baixo
 * Podendo ser dP(disco Pequeno) dM(disco Medio) ou dG(disco Grande) e -1 quando a torre se encontra vazia
 * Argumentos:
 * Torre: podendo ser 0(primeira) 1(segunda) 2(terceira)
 */

int tam_disco_matriz(int torre)
{
            for(i=0;i<3;i++)
            {
                if(matriz_torre[i][torre] != 0)
                {
                    //Verifica o Tamanho do disco
                    if(matriz_torre[i][torre]==1)
                    {
                        return(dP);
                    }
                    if(matriz_torre[i][torre]==2)
                    {
                        return(dM);
                    }
                    if(matriz_torre[i][torre]==3)
                    {
                        return(dG);
                    }
                }
            }
    return(-1);
}

/*
 * Movimento do disco e atualizacao na logica da matriz
 */
void move_disco(int posicao_y,int tamanho,int torre)
{
    //Parte grafica
    for(j=(btw_towers - (tamanho/2))+(btw_towers*torre); j <= ((btw_towers + (tamanho/2)) + (btw_towers*torre)); j++)
    Nokia5110_SetPxl(posicao_y,j);

    for(j=(btw_towers - (tamanho/2))+(btw_towers*torre); j <= ((btw_towers + (tamanho/2)) + (btw_towers*torre)); j++)
    Nokia5110_SetPxl(posicao_y+1,j);

    // Faz a atualizacao da matriz fazendo parte da logica do problema, nao a parte grafica
    if(posicao_y == bottle)
    {
        if(tamanho == dP )
        matriz_torre[2][torre] = 1;
        if(tamanho == dM)
            matriz_torre[2][torre] = 2;
        if(tamanho == dG)
            matriz_torre[2][torre] = 3;

    }
    if(posicao_y == middle)
    {
        if(tamanho == dP )
            matriz_torre[1][torre] = 1;
        if(tamanho == dM)
            matriz_torre[1][torre] = 2;
        if(tamanho == dG)
            matriz_torre[1][torre] = 3;
    }
    if(posicao_y == top)
    {
        if(tamanho == dP )
            matriz_torre[0][torre] = 1;
        if(tamanho == dM)
            matriz_torre[0][torre] = 2;
        if(tamanho == dG)
            matriz_torre[0][torre] = 3;
    }

    Nokia5110_DisplayBuffer();
}

/*
 * Iniciliza como padrao os discos ordenados em ordem do maior para o menor de baixo para cima, na torre 0 ( extrema da esquerda), sendo objetivo colocar todos na ordem crescente na extrema direita
 * Lembrar de acompanhar os movimentos graficos com os da matriz para ter um embasamento na logica do problema
 */

void discos_iniciais(void)
{
    // Disco grande
        int torre =0;
        int tamanho=dG;
        int posicao_y = bottle;
        move_disco(posicao_y, tamanho, torre);

    // Disco Medio
        tamanho=dM;
        posicao_y = middle;
        move_disco(posicao_y, tamanho, torre);

    // Disco Pequeno
        tamanho=dP;
        posicao_y = top;
        move_disco(posicao_y, tamanho, torre);
}

/*
 * Restaura as torre que parte é apagada ao apagar os discos durante o movimento
 */
void draw_torre(void)
{
    for(j=23;j<=43;j++)
    {
        Nokia5110_SetPxl(j, btw_towers);
        Nokia5110_SetPxl(j, btw_towers*2);
        Nokia5110_SetPxl(j, btw_towers*3);
    }
    Nokia5110_DisplayBuffer();
}

/*
 * Limpa do disco da sua antiga posicao para colocar na posicao nova
 */
void disco_clear(int posicao_y,int tamanho,int torre)
{
    //Parte grafica
    for(j=(btw_towers - (tamanho/2))+(btw_towers*torre); j <= ((btw_towers + (tamanho/2)) + (btw_towers*torre)); j++)
        Nokia5110_ClrPxl(posicao_y,j);

    for(j=(btw_towers - (tamanho/2))+(btw_towers*torre); j <= ((btw_towers + (tamanho/2)) + (btw_towers*torre)); j++)
        Nokia5110_ClrPxl(posicao_y+1,j);

    // Faz a atualizacao da matriz fazendo parte da logica do problema, nao a parte grafica
    if(posicao_y == bottle)
    {
        matriz_torre[2][torre] = 0;
    }
    if(posicao_y == middle)
    {
        matriz_torre[1][torre] = 0;
    }
    if(posicao_y == top)
    {
        matriz_torre[0][torre] = 0;
    }

    //Restaura as torres
    draw_torre();
}

void setinha_clear(int torre)
{
    if(torre < 0)
    {
        torre=2;
    }
    if(torre > 2)
    {
        torre=0;
    }
    //Barra da setinha
    for(j=(btw_towers - 7); j <= (btw_towers); j++)
            Nokia5110_ClrPxl(j  ,btw_towers +(btw_towers*torre));

            Nokia5110_ClrPxl(18,18 +(torre*btw_towers));
            Nokia5110_ClrPxl(19,19 +(torre*btw_towers));
            Nokia5110_ClrPxl(20,20 +(torre*btw_towers));

            Nokia5110_ClrPxl(20,22+(torre*btw_towers));
            Nokia5110_ClrPxl(19,23+(torre*btw_towers));
            Nokia5110_ClrPxl(18,24+(torre*btw_towers));
}


void setinha(int torre)
{
    if(torre == 0)
    {
        setinha_clear(2);
        setinha_clear(1);
    }
    if(torre == 1)
   {
       setinha_clear(0);
       setinha_clear(2);
   }
    if(torre == 2)
   {
       setinha_clear(0);
       setinha_clear(1);
   }


    //Barra da setinha
    for(j=(btw_towers - 7); j <= (btw_towers); j++)
            Nokia5110_SetPxl(j  ,btw_towers +(btw_towers*torre));

            Nokia5110_SetPxl(18,18 +(torre*btw_towers));
            Nokia5110_SetPxl(19,19 +(torre*btw_towers));
            Nokia5110_SetPxl(20,20 +(torre*btw_towers));

            Nokia5110_SetPxl(20,22+(torre*btw_towers));
            Nokia5110_SetPxl(19,23+(torre*btw_towers));
            Nokia5110_SetPxl(18,24+(torre*btw_towers));

            Nokia5110_DisplayBuffer();

}


void inicializa_torre_hanoi(void)
{
    // Primeira imagem das torres e bancada
    Nokia5110_PrintBMP2(0, 0, torre_hanoi_Bitmap, 84, 48);
    //ilustracao dos discos utilizando o SetPixel
    discos_iniciais();

    Nokia5110_DisplayBuffer();
}

void disco_alto_clear(int torre,int tamanho)
{

    //Parte grafica
    for(j=(btw_towers - (tamanho/2))+(btw_towers*torre); j <= ((btw_towers + (tamanho/2)) + (btw_towers*torre)); j++)
    Nokia5110_ClrPxl(btw_towers-1,j);

    for(j=(btw_towers - (tamanho/2))+(btw_towers*torre); j <= ((btw_towers + (tamanho/2)) + (btw_towers*torre)); j++)
    Nokia5110_ClrPxl(btw_towers,j);

    Nokia5110_DisplayBuffer();
}

void disco_alto(int torre,int tamanho)
{
    if(torre == 0)
    {
        disco_alto_clear(2,tamanho);
        disco_alto_clear(1,tamanho);
    }
    if(torre == 1)
    {
        disco_alto_clear(0,tamanho);
        disco_alto_clear(2,tamanho);
    }
    if(torre == 2)
    {
        disco_alto_clear(0,tamanho);
        disco_alto_clear(1,tamanho);
    }

    //Parte grafica
    for(j=(btw_towers - (tamanho/2))+(btw_towers*torre); j <= ((btw_towers + (tamanho/2)) + (btw_towers*torre)); j++)
    Nokia5110_SetPxl(btw_towers-1,j);

    for(j=(btw_towers - (tamanho/2))+(btw_towers*torre); j <= ((btw_towers + (tamanho/2)) + (btw_towers*torre)); j++)
    Nokia5110_SetPxl(btw_towers,j);

    Nokia5110_DisplayBuffer();

}
void display_qtd_movimentos(void)
{
    Nokia5110_SetCursor(0, 0);
    Nokia5110_OutUDec(qtd_movimentos);
}

void Systickhandler(void)
{
    c++;
    if(c==4)
    {
        c=0;
    }
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, vector_matrix[c]);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_5, vector_matrix[c]);

    display_qtd_movimentos();


}

void Handle_PortalFInt(void)
{
    SysTickIntDisable();
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    /*
     * Se o buffer estiver vazio ( a mao nao tiver com nenhum disco e estiver percorrendo as torres com a setinha)
     */
    if(c==0)
    {
        if(!buffer_status)
        {
            //Move a setinha para a esquerda
            if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)!= GPIO_PIN_0 )
            {
                setinha_on =1;
                --curso_setinha;
               if(curso_setinha < 0)
               {
                   curso_setinha = 2;
               }
               setinha(curso_setinha);
               SysCtlDelay(SysCtlClockGet()/debouncer);
            }

            //Move a setinha para a direita
            if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2)!= GPIO_PIN_2 )
            {
                setinha_on =1;
                ++curso_setinha;
                if(curso_setinha > 2)
                {
                    curso_setinha = 0;
                }
                setinha(curso_setinha);
                SysCtlDelay(SysCtlClockGet()/debouncer);
            }

            /* Simbolicamente pega o disco do topo da torre que a setinha se encontra para efetuar o movimento
             * Deve-se apagar o curso setinha antigo e deixa no lugar da setinha o disco que foi pego no topo da torre
             */
            if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1)!= GPIO_PIN_1 )
            {
                if(!torre_vazia(curso_setinha) && setinha_on != 0)
                {
                    buffer_status = 1;
                    setinha_clear(curso_setinha);

                    tamanho_buffer = tam_disco_matriz(curso_setinha);
                    posicao_y_buffer = posicao_disco_matriz(curso_setinha);
                    disco_clear(posicao_y_buffer, tamanho_buffer, curso_setinha);

                    disco_alto(curso_setinha, tamanho_buffer);
                    SysCtlDelay(SysCtlClockGet()/debouncer);
                }
            }
        }
        else
        {
            //Move a setinha para a esquerda
            if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)!= GPIO_PIN_0 )
            {
                --curso_setinha;
               if(curso_setinha < 0)
               {
                   curso_setinha = 2;
               }
               disco_alto(curso_setinha, tamanho_buffer);
               SysCtlDelay(SysCtlClockGet()/debouncer);
            }

            //Move a setinha para a direita
            if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2)!= GPIO_PIN_2 )
            {
                ++curso_setinha;
                if(curso_setinha > 2)
                {
                    curso_setinha = 0;
                }
                disco_alto(curso_setinha, tamanho_buffer);
                SysCtlDelay(SysCtlClockGet()/debouncer);
            }

            /* Simbolicamente pega o disco do topo da torre que a setinha se encontra para efetuar o movimento
             * Deve-se apagar o curso setinha antigo e deixa no lugar da setinha o disco que foi pego no topo da torre
             */
            if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1)!= GPIO_PIN_1 )
            {
                if(posicao_disco_matriz(curso_setinha) != top && ((tam_disco_matriz(curso_setinha)==-1)| (tamanho_buffer < tam_disco_matriz(curso_setinha) )))
                {
                    ++qtd_movimentos;
                    if(posicao_disco_matriz(curso_setinha) == -1)
                    {
                        posicao_y_buffer = bottle;
                    }
                    else if(posicao_disco_matriz(curso_setinha) == bottle)
                    {
                        posicao_y_buffer = middle;
                    }
                    else if(posicao_disco_matriz(curso_setinha) == middle)
                    {
                        posicao_y_buffer = top;
                    }

                    buffer_status = 0;
                    disco_alto_clear(curso_setinha, tamanho_buffer);
                    move_disco(posicao_y_buffer, tamanho_buffer, curso_setinha);

                    setinha(curso_setinha);
                    SysCtlDelay(SysCtlClockGet()/debouncer);
                }
            }
        }
    }

    SysTickPeriodSet(systick_decimo_segundos);
    SysTickEnable();
    SysTickIntEnable();
}

int main(void){

    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    Nokia5110_Init();
    Nokia5110_Clear();

    //Habilita Clock do Portais B ate o F
    Clock_B_F();

    //Botoes
    configura_botoes();

    //Display inicializacao

    inicializa_torre_hanoi();

    //movimento(top, dP, 0, bottle, 2);
    IntMasterEnable();

    IntEnable(INT_GPIOF);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_1|GPIO_INT_PIN_2|GPIO_INT_PIN_3, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_1|GPIO_INT_PIN_2|GPIO_INT_PIN_3);

    SysTickPeriodSet(systick_decimo_segundos);
    SysTickEnable();
    SysTickIntEnable();

    while(1)
    {
    }
}

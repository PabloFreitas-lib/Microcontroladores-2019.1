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

#include "Nokia5110.h"
#include "bild.h"

int i=0,j=0;

int btw_towers = 21;    // Espacamento entre cada torre de 21 pixels
int dG = 22;            // Disco grande de 22 pixels
int dM = 16;            // Disco medio 16 pixels
int dP = 10;            // Disco pequeno 10 pixels

/* Posicoes no eixo y da torre
 *
 * --> 43 |     |       |   top
 * --> 44 |     |       |   middle
 * --> 45 |     |       |   bottle
 *    ========================
 */
int bottle = 45;
int middle = 44;
int top    = 43;

void torre(int posicao_y,int tamanho,int torre)
{
    for(j=(btw_towers - (tamanho/2))+(btw_towers*torre); j <= ((btw_towers + (tamanho/2)) + (btw_towers*torre)); j++)
    Nokia5110_SetPxl(posicao_y,j);
}


int main(void){

    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    Nokia5110_Init();
    Nokia5110_Clear();
    Nokia5110_PrintBMP2(0, 0, torre_hanoi_Bitmap, 84, 48);
    torre(top, dG, 2);
    torre(middle, dM, 0);
    torre(top, dP, 0);

    Nokia5110_DisplayBuffer();
    while(1)
    {

    }
}



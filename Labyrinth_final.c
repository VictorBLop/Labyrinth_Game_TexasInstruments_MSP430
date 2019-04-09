#include <msp430.h>

//#include "msp430g2553.h"
#include "grlib.h"
//#include "uart_STDIO.h"
#include "Crystalfontz128x128_ST7735.h"
#include "HAL_MSP430G2_Crystalfontz128x128_ST7735.h"
#include <stdio.h>

/*
 * main.c
 */

int lee_ch(char canal){
    ADC10CTL0 &= ~ENC;                  //deshabilita el ADC
    ADC10CTL1&=(0x0fff);                //Borra canal anterior
    ADC10CTL1|=canal<<12;               //selecciona nuevo canal
    ADC10CTL0|= ENC;                    //Habilita el ADC
    ADC10CTL0|=ADC10SC;                 //Empieza la conversión
    LPM0;                               //Espera fin en modo LPM0
    return(ADC10MEM);                   //Devuelve valor leido
    }

void inicia_ADC(char canales){
    ADC10CTL0 &= ~ENC;      //deshabilita ADC
    ADC10CTL0 = ADC10ON | ADC10SHT_3 | SREF_0|ADC10IE; //enciende ADC, S/H lento, REF:VCC, con INT
    ADC10CTL1 = CONSEQ_0 | ADC10SSEL_0 | ADC10DIV_0 | SHS_0 | INCH_0;
    //Modo simple, reloj ADC, sin subdivision, Disparo soft, Canal 0
    ADC10AE0 = canales; //habilita los canales indicados
    ADC10CTL0 |= ENC; //Habilita el ADC
}

void conf_reloj(char VEL){
    BCSCTL2 = SELM_0 | DIVM_0 | DIVS_0;
    switch(VEL){
    case 1:
        if (CALBC1_1MHZ != 0xFF) {
            DCOCTL = 0x00;
            BCSCTL1 = CALBC1_1MHZ;      /* Set DCO to 1MHz */
            DCOCTL = CALDCO_1MHZ;
        }
        break;
    case 8:

        if (CALBC1_8MHZ != 0xFF) {
            __delay_cycles(100000);
            DCOCTL = 0x00;
            BCSCTL1 = CALBC1_8MHZ;      /* Set DCO to 8MHz */
            DCOCTL = CALDCO_8MHZ;
        }
        break;
    case 12:
        if (CALBC1_12MHZ != 0xFF) {
            __delay_cycles(100000);
            DCOCTL = 0x00;
            BCSCTL1 = CALBC1_12MHZ;     /* Set DCO to 12MHz */
            DCOCTL = CALDCO_12MHZ;
        }
        break;
    case 16:
        if (CALBC1_16MHZ != 0xFF) {
            __delay_cycles(100000);
            DCOCTL = 0x00;
            BCSCTL1 = CALBC1_16MHZ;     /* Set DCO to 16MHz */
            DCOCTL = CALDCO_16MHZ;
        }
        break;
    default:
        if (CALBC1_1MHZ != 0xFF) {
            DCOCTL = 0x00;
            BCSCTL1 = CALBC1_1MHZ;      /* Set DCO to 1MHz */
            DCOCTL = CALDCO_1MHZ;
        }
        break;

    }
    BCSCTL1 |= XT2OFF | DIVA_0;
    BCSCTL3 = XT2S_0 | LFXT1S_2 | XCAP_1;
}

Graphics_Context g_sContext;

unsigned long int color;
char i,j,k=0,kmax=0,l,aux,z=0,zmax=0,muerte=0,fin=0,salto,kmuerte;//aux_v, l;
const char radio=4,ancho_margen=6,ancho=6,largo=8,radio_peq=3;
int espera=0;
int x,y;
char estado=3;
char ini=0, t=0, pulso=0;
char mat[7][7];
char mat1[7][7]={{0,0,0,0,0,0,2},
                {0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0},
                {0,0,0,2,0,0,0},
                {0,2,0,0,0,0,0},
                {0,0,0,0,0,0,2},
                {0,0,1,1,1,1,1}
};
char mat2[7][7]={{0,0,0,0,0,0,2},
                {0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0},
                {0,0,0,2,1,1,1},
                {0,2,0,0,0,0,0},
                {0,0,0,0,0,0,2},
                {0,1,1,1,1,0,0}
};
int muros[5][3];
int agujeros[5][2];

int vel_x, vel_y;

const int Pos_ini_x=16, Pos_ini_y=16;
int Posicion_Bola_x=16, Prev_pos_x=16;
int Prev_pos_y=16, Posicion_Bola_y=16;

const int limite1=999;
const int limite2=9999;
int main(void) {

    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    conf_reloj(16);
    Crystalfontz128x128_Init();

    /* Set default screen orientation */
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    /* Initializes graphics context */
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128);
    Graphics_setFont(&g_sContext, &g_sFontFixed6x8);

    //UARTinit(16);
    inicia_ADC(BIT0);

        P1OUT=BIT1+BIT2;
        P1REN=BIT1+BIT2;

        TA1CCTL0=CCIE;          //CCIE=1
        TA1CTL=TASSEL_1| MC_1;  //ACLK, DIV=1, UP
        TA1CCR0=499;           //periodo=100ms (10000)

        P2DIR|=BIT6;
        P2SEL|=BIT6;
        P2SEL2&=~(BIT6+BIT7);
        P2SEL&=~(BIT7);
        TA0CCTL1=OUTMOD_7;
        TA0CTL=TASSEL_2| MC_1;
        TA1CCTL0=CCIE;
        TA1CTL=TASSEL_1| MC_1;
        TA0CCR0=0;
        TA0CCR1=0;

    __bis_SR_register(GIE);

    for(i=0; i<7; i++){
        for(j=0; j<7; j++){
            mat[i][j]=mat1[i][j];
        }
    }

    while(1){
            LPM0;

            x=lee_ch(0);
            y=lee_ch(3);

            //Maquina estados MOVIMIENTO
            switch (estado){
                //Pantalla START
                case 3:
                    if(P1IN&BIT1 && P1IN&BIT2 && t>9)
                        pulso=0;

                    if(t==0){
                        Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_LIGHT_GREEN);
                        Graphics_clearDisplay(&g_sContext);
                    }
                    else if(t>9){
                    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
                    Graphics_drawString(&g_sContext,
                            "PULSE",
                            15,
                            30,
                            50,
                            TRANSPARENT_TEXT);

                    Graphics_drawString(&g_sContext,
                                                "START",
                                                15,
                                                30,
                                                70,
                                                TRANSPARENT_TEXT);
                    }
                    t++;
                    if(t==19)
                        t=0;

                    if(pulso==0){
                    if(!(P1IN&BIT1) || !(P1IN&BIT2)){
                        estado=0;
                        ini=1;
                        t=0;
                    }
                    }

                        break;

                //Actualizacion velocidad
                case 0:
                    if(ini==1){
                        ini=0;

                        Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_DARK_GOLDENROD);
                        Graphics_clearDisplay(&g_sContext);
                        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
                        for(i=0;i<ancho_margen;i++){
                            Graphics_Rectangle marco = {
                                    0+i,
                                    0+i,
                                    127-i,
                                    127-i
                            };
                            Graphics_drawRectangle(&g_sContext,&marco);
                        }

                        for (i=0; i<7; i++){
                                    for(j=0; j<7; j++){
                                        if((i==0 && j==0) || (i==6 && j==0)){
                                            Graphics_Rectangle cuadrado ={
                                                  (j+1)*16-8,
                                                  (i+1)*16-8,
                                                  (j+1)*16+8,
                                                  (i+1)*16+8
                                                                 };
                                            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
                                            Graphics_fillRectangle(&g_sContext,&cuadrado);
                                        }
                                    else if(mat[i][j]==0){
                                            Graphics_Rectangle cuadrado ={
                                                  (j+1)*16-8,
                                                  (i+1)*16-8,
                                                  (j+1)*16+8,
                                                  (i+1)*16+8
                                                                 };
                                            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_DARK_GOLDENROD);
                                            Graphics_fillRectangle(&g_sContext,&cuadrado);
                                        }
                                        else if(mat[i][j]==1){
                                            if(mat[i][j-1]==1 && j!=0)
                                                muros[aux][2]=(j+1)*16;
                                            else{
                                                muros[k][0]=(j+1)*16;
                                                muros[k][1]=(i+1)*16;
                                                muros[k][2]=(j+1)*16;
                                                aux=k;
                                                k++;
                                            }
                                            if(j==0)
                                                muros[aux][0]=8;
                                            if(j==6)
                                                muros[aux][2]=119;
                                            kmax=k;

                                            Graphics_Rectangle cuadrado1 ={
                                                     (j+1)*16-8,
                                                     (i+1)*16-8,
                                                     (j+1)*16+8,
                                                     (i+1)*16+8
                                            };
                                            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_DARK_GOLDENROD);
                                            Graphics_fillRectangle(&g_sContext,&cuadrado1);

                                            Graphics_Rectangle cuadrado ={
                                                 (j+1)*16-(largo-1),
                                                 (i+1)*16-(ancho-1),
                                                 (j+1)*16+(largo-2),
                                                 (i+1)*16+(ancho-2)
                                                                    };
                                            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BROWN);
                                            Graphics_fillRectangle(&g_sContext,&cuadrado);
                                        }
                                        else if(mat[i][j]==2){

                                            Graphics_Rectangle cuadrado ={
                                                  (j+1)*16-8,
                                                  (i+1)*16-8,
                                                  (j+1)*16+8,
                                                  (i+1)*16+8
                                                                 };

                                            agujeros[z][0]=(j+1)*16;
                                            agujeros[z][1]=(i+1)*16;
                                            z++;
                                            zmax=z;

                                            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_DARK_GOLDENROD);
                                            Graphics_fillRectangle(&g_sContext,&cuadrado);
                                            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
                                            Graphics_fillCircle(&g_sContext,(j+1)*16,(i+1)*16,4);

                                        }
                                    }

                                }
                    }

                    TA0CCR0=0;
                    TA0CCR1=0;
                    //Eje x
                    if(x>514-10 && x<514+10)
                    {
                        if(vel_x > 0) {
                            vel_x-=1;
                            if(vel_x<0){
                                vel_x=0;
                             }
                        }

                        else if(vel_x < 0){
                            vel_x+=1;
                            if(vel_x > 0)
                                vel_x=0;
                        }
                    }
                    else if(x>=514+10 && x<514+10+150)
                        vel_x+=1;
                    else if(x>=514+10+150)
                        vel_x+=2;
                    else if(x<=514-10 && x>514-10-150)
                        vel_x-=1;
                    else if(x<=514-10-150)
                        vel_x-=2;

                    //Eje y
                    if(y>507-10 && y<507+10)
                    {
                        if(vel_y > 0) {
                            vel_y-=1;
                            if(vel_y<0){
                                vel_y=0;
                             }
                        }

                        else if(vel_y < 0){
                            vel_y+=1;
                            if(vel_y > 0)
                                vel_y=0;
                        }
                    }
                    else if(y>=507+10 && y<507+10+150)
                        vel_y-=1;
                    else if(y>=507+10+150)
                        vel_y-=2;
                    else if(y<=507-10 && y>507-10-150)
                        vel_y+=1;
                    else if(y<=507-10-150)
                        vel_y+=2;

                    //Saturacion velocidad
                    if(vel_x>6)
                        vel_x=6;
                    else if(vel_x<-6)
                        vel_x=-6;

                    if(vel_y>6)
                        vel_y=6;
                    else if(vel_y<-6)
                        vel_y=-6;

                    estado=1;
                    break;

                //Actualizacion posicion
                case 1:
                    Posicion_Bola_x+=vel_x;
                    Posicion_Bola_y+=vel_y;

                    //Saturacion posicion margenes
                    //por la derecha
                    if (Posicion_Bola_x<=(ancho_margen+radio)){
                        Posicion_Bola_x=(ancho_margen+radio);
                        if(vel_x!=0){
                            vel_x=-(vel_x);
                            if(Prev_pos_x!=Posicion_Bola_x){
                                TA0CCR0=limite2;
                                TA0CCR1=limite1;
                            }
                        }
                    }
                    //por la izquierda
                    else if(Posicion_Bola_x>=127-(ancho_margen+radio)){
                        Posicion_Bola_x=127-(ancho_margen+radio);
                        if(vel_x!=0){
                            vel_x=-(vel_x);
                            if(Prev_pos_x!=Posicion_Bola_x){
                                TA0CCR0=limite2;
                                TA0CCR1=limite1;
                            }
                        }
                    }
                    //por abajo
                    if(Posicion_Bola_y<=(ancho_margen+radio)){
                        Posicion_Bola_y=(ancho_margen+radio);
                        if(vel_y!=0){
                            vel_y=-(vel_y);
                            if(Prev_pos_y!=Posicion_Bola_y){
                                TA0CCR0=limite2;
                                TA0CCR1=limite1;
                            }
                        }
                    }
                    //por arriba
                    else if(Posicion_Bola_y>=127-(ancho_margen+radio)){
                        Posicion_Bola_y=127-(ancho_margen+radio);
                        if(vel_y!=0){
                            vel_y=-(vel_y);
                            if(Prev_pos_y!=Posicion_Bola_y){
                                TA0CCR0=limite2;
                                TA0CCR1=limite1;
                            }
                        }
                    }

                    //Muerte en agujero
                    for(k=0;k<zmax;k++){
                        if ((Posicion_Bola_x>(agujeros[k][0]-radio) && Posicion_Bola_x<agujeros[k][0]+radio) && (Posicion_Bola_y<agujeros[k][1]+radio && Posicion_Bola_y>agujeros[k][1]-radio))
                        {
                            muerte=1;
                            kmuerte=k;
                            k=zmax;
                        }
                                Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
                                Graphics_fillCircle(&g_sContext,agujeros[k][0],agujeros[k][1],radio);
                    }

                    //Saturacion posicion muros
                        for(k=0;k<kmax;k++){
                        //Choque por la derecha
                        if ((Posicion_Bola_x-(largo+radio)<=muros[k][2] && Posicion_Bola_x>muros[k][2]) && (Posicion_Bola_y<=muros[k][1]+ancho && Posicion_Bola_y>=muros[k][1]-ancho)){
                            Posicion_Bola_x=muros[k][2]+(largo+radio);
                            if(vel_x!=0){
                                vel_x=-(vel_x+1);
                                if(Prev_pos_x!=Posicion_Bola_x){
                                    TA0CCR0=limite2;
                                    TA0CCR1=limite1;
                                }
                            }
                            k=kmax;
                        }
                        //Choques por la izquierda
                        else if ((Posicion_Bola_x+(largo+radio)>=muros[k][0] && Posicion_Bola_x<muros[k][0]) && (Posicion_Bola_y<=muros[k][1]+ancho && Posicion_Bola_y>=muros[k][1]-ancho)){
                            Posicion_Bola_x=muros[k][0]-(largo+radio);
                            if(vel_x!=0){
                                vel_x=-(vel_x-1);
                                if(Prev_pos_x!=Posicion_Bola_x){
                                    TA0CCR0=limite2;
                                    TA0CCR1=limite1;
                                }
                            }
                            k=kmax;
                        }
                        //Choques por abajo
                        else if ((Posicion_Bola_y-(ancho+radio)<=muros[k][1] && Posicion_Bola_y>muros[k][1]) && (Posicion_Bola_x<=muros[k][2]+largo && Posicion_Bola_x>=muros[k][0]-largo)){
                            Posicion_Bola_y=muros[k][1]+(ancho+radio);
                            if(vel_y!=0){
                                vel_y=-(vel_y+1);
                                if(Prev_pos_y!=Posicion_Bola_y){
                                    TA0CCR0=limite2;
                                    TA0CCR1=limite1;
                                }
                            }
                            k=kmax;
                        }
                        //Choques por arriba
                        else if ((Posicion_Bola_y+(ancho+radio)>=muros[k][1] && Posicion_Bola_y<muros[k][1]) && (Posicion_Bola_x<=muros[k][2]+largo && Posicion_Bola_x>=muros[k][0]-largo)){
                            Posicion_Bola_y=muros[k][1]-(ancho+radio);
                            if(vel_y!=0){
                                vel_y=-(vel_y-1);
                                if(Prev_pos_y!=Posicion_Bola_y){
                                    TA0CCR0=limite2;
                                    TA0CCR1=limite1;
                                }
                            }
                            k=kmax;
                        }
                    }
                        Graphics_Rectangle cuadrado ={
                              16-8,
                              16-8,
                              16+8,
                              16+8
                                             };
                        Graphics_Rectangle cuadrado1 ={
                              16-8,
                              127-16-8,
                              16+8,
                              127-16+8
                                             };
                        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
                        Graphics_fillRectangle(&g_sContext,&cuadrado);
                        Graphics_fillRectangle(&g_sContext,&cuadrado1);

                        if((Posicion_Bola_x<16+4 && Posicion_Bola_x>16-4) && (Posicion_Bola_y<127-16+4 && Posicion_Bola_y>127-16-4)){
                            fin=1;
                            Posicion_Bola_x=16;
                            Posicion_Bola_y=16;
                            vel_x=0;
                            vel_y=0;
                        }

                    estado=0;
                    if(muerte==1){
                        estado=2;
                        muerte=0;
                    }
                    if(fin==1){
                        estado=5;
                        fin=0;
                    }
                    break;

                //Muerte
                case 2:
                    for(i=0; i<112; i++){
                        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);

                        Graphics_Rectangle cuadrado ={
                                 agujeros[kmuerte][0]-radio-i,
                                 agujeros[kmuerte][1]-radio-i,
                                 agujeros[kmuerte][0]+radio+i,
                                 agujeros[kmuerte][1]+radio+i
                        };
                        Graphics_drawRectangle(&g_sContext,&cuadrado);

                        if(Posicion_Bola_x > 64)
                            Posicion_Bola_x--;
                        else if(Posicion_Bola_x <64)
                            Posicion_Bola_x++;

                        else if(Posicion_Bola_y > 64)
                            Posicion_Bola_y--;
                        else if(Posicion_Bola_y <64)
                            Posicion_Bola_y++;
                        if(Posicion_Bola_x!=64 || Posicion_Bola_y!=64){
                            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
                            Graphics_fillCircle(&g_sContext,Prev_pos_x,Prev_pos_y,4);
                        }

                        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE_SMOKE);
                        Graphics_fillCircle(&g_sContext,Posicion_Bola_x,Posicion_Bola_y,4);

                        Prev_pos_x=Posicion_Bola_x;
                        Prev_pos_y=Posicion_Bola_y;

                        while(espera<9999)
                            espera++;
                        espera=0;
                    }

                    for(i=0; i<7; i++){
                        for(j=0; j<7; j++){
                            mat[i][j]=mat1[i][j];
                        }
                    }

                    estado=4;
                    break;

                //Pantalla de GAME OVER
                case 4:
                    if(t==0){
                        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
                        Graphics_Rectangle square ={
                                 45,
                                 30,
                                 85,
                                 50
                        };
                        Graphics_fillRectangle(&g_sContext,&square);
                        Graphics_Rectangle square2 ={
                                 45,
                                 90,
                                 85,
                                 110
                        };
                        Graphics_fillRectangle(&g_sContext,&square2);

                        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE_SMOKE);
                        Graphics_fillCircle(&g_sContext,64,64,4);
                    }
                    else if(t>9){
                    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
                    Graphics_drawString(&g_sContext,
                            "GAME",
                            15,
                            50,
                            34,
                            TRANSPARENT_TEXT);

                    Graphics_drawString(&g_sContext,
                            "OVER",
                            15,
                            50,
                            94,
                            TRANSPARENT_TEXT);

                    }
                    t++;
                    if(t==19)
                        t=0;

                    if(!(P1IN&BIT1) || !(P1IN&BIT2)){
                        estado=3;
                        t=0;
                        pulso=1;
                        Posicion_Bola_x=Pos_ini_x;
                        Posicion_Bola_y=Pos_ini_y;
                        Prev_pos_x=Posicion_Bola_x;
                        Prev_pos_y=Posicion_Bola_y;
                        vel_x=0;
                        vel_y=0;
                        k=0;
                        z=0;
                        Graphics_clearDisplay(&g_sContext);
                    }
                        break;

                //Pasa de nivel
                case 5:
                    for(l=0;l<=128;l+=2){
                        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
                        for(i=0;i<ancho_margen;i++){
                            Graphics_Rectangle marco = {
                                    0+i,
                                    0+i-l,
                                    127-i,
                                    127-i-l
                            };
                            Graphics_drawRectangle(&g_sContext,&marco);
                        }
                        for(i=0;i<ancho_margen;i++){
                            Graphics_Rectangle marco = {
                                    0+i,
                                    0+i+127-l,
                                    127-i,
                                    127-i+127-l
                            };
                            Graphics_drawRectangle(&g_sContext,&marco);
                        }

                        for (i=0; i<7; i++){
                                    for(j=0; j<7; j++){
                                        if((i==0 && j==0) || (i==6 && j==0)){
                                            Graphics_Rectangle cuadrado ={
                                                  (j+1)*16-8,
                                                  (i+1)*16-8-l,
                                                  (j+1)*16+8,
                                                  (i+1)*16+8-l
                                                                 };
                                            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
                                            Graphics_fillRectangle(&g_sContext,&cuadrado);
                                        }
                                    else if(mat[i][j]==0){
                                            Graphics_Rectangle cuadrado ={
                                                  (j+1)*16-8,
                                                  (i+1)*16-8-l,
                                                  (j+1)*16+8,
                                                  (i+1)*16+8-l
                                                                 };
                                            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_DARK_GOLDENROD);
                                            Graphics_fillRectangle(&g_sContext,&cuadrado);
                                        }
                                        else if(mat[i][j]==1){
                                            Graphics_Rectangle cuadrado ={
                                                     (j+1)*16-8,
                                                     (i+1)*16-8-l,
                                                     (j+1)*16+8,
                                                     (i+1)*16+8-l
                                            };
                                            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_DARK_GOLDENROD);
                                            Graphics_fillRectangle(&g_sContext,&cuadrado);

                                            Graphics_Rectangle cuadrado1 ={
                                                 (j+1)*16-(largo-1),
                                                 (i+1)*16-(ancho-1)-l,
                                                 (j+1)*16+(largo-2),
                                                 (i+1)*16+(ancho-2)-l
                                                                    };

                                            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BROWN);
                                            Graphics_fillRectangle(&g_sContext,&cuadrado1);
                                        }
                                        else if(mat[i][j]==2){
                                            Graphics_Rectangle cuadrado ={
                                                  (j+1)*16-8,
                                                  (i+1)*16-8-l,
                                                  (j+1)*16+8,
                                                  (i+1)*16+8-l
                                                                 };
                                            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_DARK_GOLDENROD);
                                            Graphics_fillRectangle(&g_sContext,&cuadrado);
                                            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
                                            Graphics_fillCircle(&g_sContext,(j+1)*16,(i+1)*16-l,4);
                                        }

                                        if((i==0 && j==0) || (i==6 && j==0)){
                                            Graphics_Rectangle cuadrado ={
                                                  (j+1)*16-8,
                                                  (i+1)*16-8+127-l,
                                                  (j+1)*16+8,
                                                  (i+1)*16+8+127-l
                                                                 };
                                            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
                                            Graphics_fillRectangle(&g_sContext,&cuadrado);
                                        }
                                    else if(mat2[i][j]==0){
                                            Graphics_Rectangle cuadrado ={
                                                  (j+1)*16-8,
                                                  (i+1)*16-8+127-l,
                                                  (j+1)*16+8,
                                                  (i+1)*16+8+127-l
                                                                 };
                                            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_DARK_GOLDENROD);
                                            Graphics_fillRectangle(&g_sContext,&cuadrado);
                                        }
                                        else if(mat2[i][j]==1){
                                            Graphics_Rectangle cuadrado ={
                                                     (j+1)*16-8,
                                                     (i+1)*16-8+127-l,
                                                     (j+1)*16+8,
                                                     (i+1)*16+8+127-l
                                            };
                                            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_DARK_GOLDENROD);
                                            Graphics_fillRectangle(&g_sContext,&cuadrado);

                                            Graphics_Rectangle cuadrado1 ={
                                                 (j+1)*16-(largo-1),
                                                 (i+1)*16-(ancho-1)+127-l,
                                                 (j+1)*16+(largo-2),
                                                 (i+1)*16+(ancho-2)+127-l
                                                                    };
                                            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BROWN);
                                            Graphics_fillRectangle(&g_sContext,&cuadrado1);
                                        }
                                        else if(mat2[i][j]==2){
                                            Graphics_Rectangle cuadrado ={
                                                  (j+1)*16-8,
                                                  (i+1)*16-8+127-l,
                                                  (j+1)*16+8,
                                                  (i+1)*16+8+127-l
                                                                 };
                                            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_DARK_GOLDENROD);
                                            Graphics_fillRectangle(&g_sContext,&cuadrado);
                                            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
                                            Graphics_fillCircle(&g_sContext,(j+1)*16,(i+1)*16+127-l,4);
                                        }
                                    }
                                }
                    }

                    estado=0;
                    ini=1;
                    k=0;
                    z=0;

                    for(i=0; i<7; i++){
                        for(j=0; j<7; j++){
                            mat[i][j]=mat2[i][j];
                        }
                    }

                    break;
                           }

            if(estado==0 || estado==1){
            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_DARK_GOLDENROD);
            Graphics_fillCircle(&g_sContext,Prev_pos_x,Prev_pos_y,4);

            Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE_SMOKE);
            Graphics_fillCircle(&g_sContext,Posicion_Bola_x,Posicion_Bola_y,radio);

            //Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
           // Graphics_drawCircle(&g_sContext,Posicion_Bola_x,Posicion_Bola_y,4);

            Prev_pos_x=Posicion_Bola_x;
            Prev_pos_y=Posicion_Bola_y;
            }
    }
    }

    #pragma vector=ADC10_VECTOR
    __interrupt void ConvertidorAD(void)
    {
        LPM0_EXIT;  //Despierta al micro al final de la conversión
    }

    #pragma vector=TIMER1_A0_VECTOR
    __interrupt void TIMER1_A0_ISR_HOOK(void)
    {
        LPM0_EXIT;  //Despierta al micro al final del tiempo
    }


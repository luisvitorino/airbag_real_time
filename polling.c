/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"

int main(void)
{
    int motorista = 0;
    int passageiro = 0;
    int frontal = 0;
    for(;;)
    {
    motorista =  Motorista_Read();
    if (motorista != 0)
    {
        //Rotina de acionamento dos airbags
        CyDelay(12);
        Ab_4_Write(1);
        Ab_8_Write(1);
        CyDelay(5);
        Ab_3_Write(1);
        Ab_7_Write(1);
        CyDelay(13);
        Ab_1_Write(1);
        Ab_2_Write(1);
        Ab_5_Write(1);
        Ab_6_Write(1);
        CyDelay(5000);
        Ab_1_Write(0);
        Ab_2_Write(0);
        Ab_3_Write(0);
        Ab_4_Write(0);
        Ab_5_Write(0);
        Ab_6_Write(0);
        Ab_7_Write(0);
        Ab_8_Write(0);
    }
    
    passageiro = Passageiro_Read();
     if (passageiro != 0)
    {
        //Rotina de acionamento dos airbags
        CyDelay(12);
        Ab_3_Write(1);
        Ab_7_Write(1);
        CyDelay(5);
        Ab_4_Write(1);
        Ab_8_Write(1);
        CyDelay(13);
        Ab_1_Write(1);
        Ab_2_Write(1);
        Ab_5_Write(1);
        Ab_6_Write(1);
        CyDelay(5000);
        Ab_1_Write(0);
        Ab_2_Write(0);
        Ab_3_Write(0);
        Ab_4_Write(0);
        Ab_5_Write(0);
        Ab_6_Write(0);
        Ab_7_Write(0);
        Ab_8_Write(0);
        
    }
    
     if (frontal != 0)
    {
        //Rotina de acionamento dos airbags
        CyDelay(30);
        Ab_4_Write(1);
        Ab_1_Write(1);
        Ab_3_Write(1);
        Ab_2_Write(1);
        CyDelay(10);
        Ab_5_Write(1);
        Ab_6_Write(1);
        Ab_7_Write(1);
        Ab_8_Write(1);
        CyDelay(5000);
        Ab_1_Write(0);
        Ab_2_Write(0);
        Ab_3_Write(0);
        Ab_4_Write(0);
        Ab_5_Write(0);
        Ab_6_Write(0);
        Ab_7_Write(0);
        Ab_8_Write(0);
        }
}
}

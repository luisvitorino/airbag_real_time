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

CY_ISR(isr_1_frontal){         //   Botão colisão Frontal
    CyGlobalIntDisable;        //   Desabilita as interrupções para que não ocorra interrupção do acionamento dos airbags
    CyDelay(30);               //   Delay de 30ms para acionar o primeiro conjunto de airbags
    Ab_1_Write(1);
    Ab_2_Write(1);
    Ab_3_Write(1);
    Ab_4_Write(1);
    CyDelay(10);               //   Delay de 40ms para acionar segundo conjunto de airbas
    Ab_5_Write(1);
    Ab_6_Write(1);
    Ab_7_Write(1);
    Ab_8_Write(1);
    CyDelay(5000);             //   Delay de 3 segundos para dar tempo de colher os dados com o osciloscópio. Após esse tempo, desliga todos os LEDs
    Ab_1_Write(0);
    Ab_2_Write(0);
    Ab_3_Write(0);
    Ab_4_Write(0);
    Ab_5_Write(0);
    Ab_6_Write(0);
    Ab_7_Write(0);
    Ab_8_Write(0);
    Frontal_ClearInterrupt();  //   Termina a interrupção
    CyGlobalIntEnable;         //   Habilita novamente as interrupções globais
}
CY_ISR(isr_2_passageiro){       //   Botão de colisão do Passageiro   
    CyGlobalIntDisable;
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
    Passageiro_ClearInterrupt();
    CyGlobalIntEnable;
}
   
CY_ISR(isr_3_motorista){    // Botão de colisão do Motorista 
    CyGlobalIntDisable;
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
    Motorista_ClearInterrupt();
    CyGlobalIntEnable;
}

int main(void)
{
 
    CyGlobalIntEnable;                   // Habilita interrupções globais 

    isr_1_StartEx(isr_1_frontal);        // Habilita interrupção da colisão frontal
    isr_2_StartEx(isr_2_passageiro);     // Habilita interrupção da colisão Lateral - Passageiro
    isr_3_StartEx(isr_3_motorista);      // Habilita interrupção da colisão Lateral - Motorista


    for(;;) // Loop 
    {
         /*
            Sistema de acionamento de airbags por interrupção. 
         */ 
    }   
}
/* [] END OF FILE */

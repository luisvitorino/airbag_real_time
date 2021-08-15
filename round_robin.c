/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                             (c) Copyright 2012; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                           EXAMPLE CODE
*
*                                           CYPRESS PSoC5
*                                              with the
*                                     CY8CKIT-050 Development Kit
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : DC
                  
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>


/*
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*/  

#define  APP_USER_IF_SIGN_ON                        0u
#define  APP_USER_IF_VER_TICK_RATE                  1u
#define  APP_USER_IF_CPU                            2u
#define  APP_USER_IF_CTXSW                          3u
#define  APP_USER_IF_STATE_MAX                      4u

/*
*********************************************************************************************************
*                                            LOCAL VARIABLES
*********************************************************************************************************
*/

static  OS_TCB   App_TaskStartTCB;
static  CPU_STK  App_TaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB   App_TaskUserIF_TCB;
static  OS_TCB   App_Motorista_TCB;
static  OS_TCB   App_Passageiro_TCB;
static  OS_TCB   App_Frontal_TCB;
static  OS_TCB   App_AB_F_TCB;
static  OS_TCB   App_AB_M_TCB;
static  OS_TCB   App_AB_P_TCB;



static  CPU_STK  App_TaskUserIFStk[APP_CFG_TASK_USER_IF_STK_SIZE];
static  CPU_STK  App_MotoristaStk[APP_CFG_MOTORISTA_STK_SIZE];
static  CPU_STK  App_PassageiroStk[APP_CFG_PASSAGEIRO_STK_SIZE];
static  CPU_STK  App_FrontalStk[APP_CFG_FRONTAL_STK_SIZE];
static  CPU_STK  App_ABFStk[APP_CFG_ABF_STK_SIZE];
static  CPU_STK  App_ABMStk[APP_CFG_ABM_STK_SIZE];
static  CPU_STK  App_ABPStk[APP_CFG_ABP_STK_SIZE];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  App_TaskStart  (void *p_arg);

static  void  App_TaskUserIF (void *p_arg);
static void Frontal(void *p_arg);
static void Motorista(void *p_arg);
static void Passageiro(void *p_arg);
static void Ab_F (void *p_arg);
static void Ab_M (void *p_arg);
static void Ab_P (void *p_arg);
static  void  App_TaskCreate (void);
static  void  App_ObjCreate  (void);


//Variaveis Globais 
CPU_BOOLEAN EstBotao_M;
CPU_BOOLEAN EstBotao_F;
CPU_BOOLEAN EstBotao_P;


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : Startup Code.
*
* Note(s)     : none.
*********************************************************************************************************
*/

int  main (void)
{
    OS_ERR  os_err;


    BSP_PreInit();                                              /* Perform BSP pre-initialization.                      */

    CPU_Init();                                                 /* Initialize the uC/CPU services                       */

    OSInit(&os_err);                                            /* Init uC/OS-III.                                      */
    
    OSSchedRoundRobinCfg(1, 
                         1,
                         &os_err);
    

    OSTaskCreate((OS_TCB      *)&App_TaskStartTCB,              /* Create the start task                                */
                 (CPU_CHAR    *)"Start",
                 (OS_TASK_PTR  )App_TaskStart, 
                 (void        *)0,
                 (OS_PRIO      )APP_CFG_TASK_START_PRIO,
                 (CPU_STK     *)&App_TaskStartStk[0],
                 (CPU_STK_SIZE )APP_CFG_TASK_START_STK_SIZE_LIMIT,
                 (CPU_STK_SIZE )APP_CFG_TASK_START_STK_SIZE,
                 (OS_MSG_QTY   )0u,
                 (OS_TICK      )0u,
                 (void        *)0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR      *)&os_err);

    OSStart(&os_err);                                            /* Start multitasking (i.e. give control to uC/OS-III).  */
}


/*
*********************************************************************************************************
*                                          App_TaskStart()
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none
*
* Note(s)     : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  App_TaskStart (void *p_arg)
{
    OS_ERR       err;
        

   (void)p_arg;
   

    BSP_PostInit();                                               /* Perform BSP post-initialization functions.       */
    
    BSP_CPU_TickInit();                                           /* Perfrom Tick Initialization                      */

#if (OS_CFG_STAT_TASK_EN > 0u)
    OSStatTaskCPUUsageInit(&err);
#endif    

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif      

    App_TaskCreate();                                             /* Create application tasks.                        */

    App_ObjCreate();                                              /* Create kernel objects                            */
    
#if (APP_CFG_PROBE_COM_EN == DEF_ENABLED)
    App_ProbeInit();                                              /* Enabling uC/Probe                                */
#endif
    
    BSP_LED_Off(0);

    while (DEF_TRUE) {                                            /* Task body, always written as an infinite loop.   */
        BSP_LED_Toggle(0);
        OSTimeDlyHMSM(0, 0, 0, 100, 
                      OS_OPT_TIME_HMSM_STRICT, 
                      &err);
    }
}


/*
*********************************************************************************************************
*                                          App_TaskCreate()
*
* Description : Create application tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_TaskCreate (void)
{

    OS_ERR  os_err;
    OS_ERR  suspends;
    
    OSTaskCreate((OS_TCB      *)&App_TaskUserIF_TCB,
                 (CPU_CHAR    *)"Start",
                 (OS_TASK_PTR  )App_TaskUserIF, 
                 (void        *)0,
                 (OS_PRIO      )15u,
                 (CPU_STK     *)&App_TaskUserIFStk[0],
                 (CPU_STK_SIZE )APP_CFG_TASK_USER_IF_STK_SIZE_LIMIT,
                 (CPU_STK_SIZE )APP_CFG_TASK_USER_IF_STK_SIZE,
                 (OS_MSG_QTY   )0u,
                 (OS_TICK      )0u,
                 (void        *)0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR      *)&os_err);
    
    
    
    OSTaskCreate((OS_TCB      *)&App_Motorista_TCB,
                 (CPU_CHAR    *)"Start",
                 (OS_TASK_PTR  )Motorista, 
                 (void        *)0,
                 (OS_PRIO      )8u,
                 (CPU_STK     *)&App_MotoristaStk[0],
                 (CPU_STK_SIZE )APP_CFG_TASK_USER_IF_STK_SIZE_LIMIT,
                 (CPU_STK_SIZE )APP_CFG_TASK_USER_IF_STK_SIZE,
                 (OS_MSG_QTY   )0u,
                 (OS_TICK      )0u,
                 (void        *)0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR      *)&os_err);
    
    OSTaskCreate((OS_TCB      *)&App_Passageiro_TCB,
                 (CPU_CHAR    *)"Start",
                 (OS_TASK_PTR  )Passageiro, 
                 (void        *)0,
                 (OS_PRIO      )8u,
                 (CPU_STK     *)&App_PassageiroStk[0],
                 (CPU_STK_SIZE )APP_CFG_TASK_USER_IF_STK_SIZE_LIMIT,
                 (CPU_STK_SIZE )APP_CFG_TASK_USER_IF_STK_SIZE,
                 (OS_MSG_QTY   )0u,
                 (OS_TICK      )0u,
                 (void        *)0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR      *)&os_err);
    
    OSTaskCreate((OS_TCB      *)&App_Frontal_TCB,
                 (CPU_CHAR    *)"Start",
                 (OS_TASK_PTR  )Frontal, 
                 (void        *)0,
                 (OS_PRIO      )8u,
                 (CPU_STK     *)&App_FrontalStk[0],
                 (CPU_STK_SIZE )APP_CFG_TASK_USER_IF_STK_SIZE_LIMIT,
                 (CPU_STK_SIZE )APP_CFG_TASK_USER_IF_STK_SIZE,
                 (OS_MSG_QTY   )0u,
                 (OS_TICK      )0u,
                 (void        *)0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR      *)&os_err);

    OSTaskCreate((OS_TCB      *)&App_AB_F_TCB,
                 (CPU_CHAR    *)"Start",
                 (OS_TASK_PTR  )Ab_F, 
                 (void        *)0,
                 (OS_PRIO      )5u,
                 (CPU_STK     *)&App_ABFStk[0],
                 (CPU_STK_SIZE )APP_CFG_TASK_USER_IF_STK_SIZE_LIMIT,
                 (CPU_STK_SIZE )APP_CFG_TASK_USER_IF_STK_SIZE,
                 (OS_MSG_QTY   )0u,
                 (OS_TICK      )0u,
                 (void        *)0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR      *)&os_err);
    
    OSTaskCreate((OS_TCB      *)&App_AB_M_TCB,
                 (CPU_CHAR    *)"Start",
                 (OS_TASK_PTR  )Ab_M, 
                 (void        *)0,
                 (OS_PRIO      )5u,
                 (CPU_STK     *)&App_ABMStk[0],
                 (CPU_STK_SIZE )APP_CFG_TASK_USER_IF_STK_SIZE_LIMIT,
                 (CPU_STK_SIZE )APP_CFG_TASK_USER_IF_STK_SIZE,
                 (OS_MSG_QTY   )0u,
                 (OS_TICK      )0u,
                 (void        *)0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR      *)&os_err);
    
    OSTaskCreate((OS_TCB      *)&App_AB_P_TCB,
                 (CPU_CHAR    *)"Start",
                 (OS_TASK_PTR  )Ab_P, 
                 (void        *)0,
                 (OS_PRIO      )5u,
                 (CPU_STK     *)&App_ABPStk[0],
                 (CPU_STK_SIZE )APP_CFG_TASK_USER_IF_STK_SIZE_LIMIT,
                 (CPU_STK_SIZE )APP_CFG_TASK_USER_IF_STK_SIZE,
                 (OS_MSG_QTY   )0u,
                 (OS_TICK      )0u,
                 (void        *)0,
                 (OS_OPT       )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR      *)&os_err);

OSTaskSuspend(&App_AB_F_TCB,&suspends);
OSTaskSuspend(&App_AB_M_TCB,&suspends);
OSTaskSuspend(&App_AB_P_TCB,&suspends);

}

/*
*********************************************************************************************************
*                                          App_ObjCreate()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_ObjCreate (void)
{

}


/*
*********************************************************************************************************
*                                          App_TaskUserIF()
*
* Description : User interface task
*
* Argument(s) : p_arg   is the argument passed to 'App_TaskUserIF()' by 'OSTaskCreate()'.
*
* Return(s)   : none
*
* Note(s)     : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/


static  void  App_TaskUserIF (void *p_arg)
{
   CPU_INT32U   val;
   CPU_INT08U   user_if_state_cur;
   CPU_INT08U   user_if_state_prev;   
   CPU_BOOLEAN  pb2_state_cur;
   CPU_BOOLEAN  pb2_state_prev;
   CPU_BOOLEAN  pb3_state_cur;
   CPU_BOOLEAN  pb3_state_prev;
   OS_ERR       os_err;
   CPU_CHAR     line_str[17];
   
   
   (void)p_arg;
   
   LCD_Disp_Start();                                             /* Initialize the LCD screen.                       */
   
   LCD_Disp_Position(0u, 0u);                                    /* Clear LCD screen                                 */
   LCD_Disp_PrintString((uint8 *)"                ");
   LCD_Disp_Position(1u, 0u);
   LCD_Disp_PrintString((uint8 *)"                ");
   
   LCD_Disp_PrintString((uint8 *)"Micrium uC/OS-III");

   user_if_state_cur  = APP_USER_IF_SIGN_ON;
   user_if_state_prev = APP_USER_IF_SIGN_ON;
   pb2_state_cur      = DEF_OFF;
   pb2_state_prev     = DEF_OFF;
   pb3_state_cur      = DEF_OFF;
   pb3_state_prev     = DEF_OFF;
   
   
    while (DEF_TRUE) {    

        if (user_if_state_cur != user_if_state_prev) {
            LCD_Disp_Position(0u, 0u);
            LCD_Disp_PrintString((uint8 *)"                ");
            LCD_Disp_Position(1u, 0u);
            LCD_Disp_PrintString((uint8 *)"                ");
            user_if_state_prev = user_if_state_cur;
        }

        switch (user_if_state_cur) {
            case APP_USER_IF_VER_TICK_RATE:
                 Str_Copy(line_str, "uC/OS-III Vx.yyy");
                 val          = (CPU_INT32U)OSVersion(&os_err);
                 line_str[11] = (val        )  / 1000u + '0';
                 line_str[13] = (val % 1000u)  /  100u + '0';
                 line_str[14] = (val %  100u)  /   10u + '0';
                 line_str[15] = (val %   10u)          + '0';
                 LCD_Disp_Position(0u, 0u);
                 LCD_Disp_PrintString((uint8 *)line_str);

                 Str_Copy(line_str, "TickRate:   xxxx");
                 val = (CPU_INT32U)OSCfg_TickRate_Hz;
                 Str_FmtNbr_Int32U((CPU_INT32U     )val, 
                                   (CPU_INT08U     )4u,
                                   (CPU_INT08U     )10,
                                   (CPU_CHAR       )0,
                                   (CPU_BOOLEAN    )DEF_NO,
                                   (CPU_BOOLEAN    )DEF_YES,
                                   (CPU_CHAR      *)&line_str[12]);
                 LCD_Disp_Position(1u, 0u);
                 LCD_Disp_PrintString((uint8 *)line_str);
                 break;

            case APP_USER_IF_CPU:
                 Str_Copy(line_str, "CPU Usage:xx %  ");
                 val          = (CPU_INT32U)OSStatTaskCPUUsage;
                 line_str[10] = (val / 10u) + '0';
                 line_str[11] = (val % 10u) + '0';
                 LCD_Disp_Position(0u, 0u);
                 LCD_Disp_PrintString((uint8 *)line_str);
                 
                 Str_Copy(line_str, "CPU Speed:xxxMHz");
                 val          = (CPU_INT32U)BSP_CPU_ClkFreq();
                 val         /= 1000000u;
                 line_str[10] = (val       ) / 100u + '0';
                 line_str[11] = (val % 100u) /  10u + '0';
                 line_str[12] = (val % 10u)         + '0';
                 LCD_Disp_Position(1u, 0u);
                 LCD_Disp_PrintString((uint8 *)line_str);
                 break;

            case APP_USER_IF_CTXSW:
                 Str_Copy(line_str, "#Ticks: xxxxxxxx");
                 val = (CPU_INT32U)OSTickCtr;
                 Str_FmtNbr_Int32U((CPU_INT32U     )val, 
                                   (CPU_INT08U     )8u,
                                   (CPU_INT08U     )10,
                                   (CPU_CHAR       )0,
                                   (CPU_BOOLEAN    )DEF_NO,
                                   (CPU_BOOLEAN    )DEF_YES,
                                   (CPU_CHAR      *)&line_str[8]);
                 LCD_Disp_Position(0u, 0u);
                 LCD_Disp_PrintString((uint8 *)line_str);
                 
                 Str_Copy(line_str, "#CtxSw: xxxxxxxx");
                 val          = (CPU_INT32U)OSTaskCtxSwCtr;
                 Str_FmtNbr_Int32U((CPU_INT32U     )val, 
                                   (CPU_INT08U     )8u,
                                   (CPU_INT08U     )10,
                                   (CPU_CHAR       )0,
                                   (CPU_BOOLEAN    )DEF_NO,
                                   (CPU_BOOLEAN    )DEF_YES,
                                   (CPU_CHAR      *)&line_str[8]);
                 LCD_Disp_Position(1u, 0u);
                 LCD_Disp_PrintString((uint8 *)line_str);
                 break;

            case APP_USER_IF_SIGN_ON:
            default:
                 LCD_Disp_Position(0u, 0u);
                 LCD_Disp_PrintString((uint8 *)"=== Micrium === ");
                 LCD_Disp_Position(1u, 0u);
                 LCD_Disp_PrintString((uint8 *)"   uC/OS-III    ");
                 break;
        }

        pb2_state_cur = BSP_PB_StatusGet(2);
        pb3_state_cur = BSP_PB_StatusGet(3);
        if ((pb2_state_cur  == DEF_ON ) &&
            (pb2_state_prev == DEF_OFF)) {     
            user_if_state_cur = (user_if_state_cur + 1u) % APP_USER_IF_STATE_MAX;
        }

        if ((pb3_state_cur  == DEF_ON ) &&
            (pb3_state_prev == DEF_OFF)) { 
            user_if_state_cur = (user_if_state_cur - 1u) % APP_USER_IF_STATE_MAX;
        }

        OSTimeDlyHMSM(0, 0, 0, 10, 
                      OS_OPT_TIME_HMSM_STRICT, 
                      &os_err);

        pb2_state_prev = pb2_state_cur;
        pb3_state_prev = pb3_state_cur;	
    }
}

static void Motorista(void *p_arg)  
{
    OS_ERR       err;
   while(1)
    {
    EstBotao_M = Mot_Read();
    if(EstBotao_M == 0){
     OSTimeDlyHMSM(0, 0, 0, 5, 
                      OS_OPT_TIME_HMSM_STRICT, 
                      &err);
    }else{
    OSTaskResume(&App_AB_M_TCB,&err);
    OSTimeDlyHMSM(0, 0, 0, 40, 
                      OS_OPT_TIME_HMSM_STRICT, 
                      &err);
    }
    }
}    

static void Frontal(void *p_arg)  
{
    OS_ERR       err;
   while(1)
    {
    EstBotao_F = Fron_Read();
    if(EstBotao_F == 0){
        OSTimeDlyHMSM(0, 0, 0, 6, 
                      OS_OPT_TIME_HMSM_STRICT, 
                      &err);
    }else{
        OSTaskResume(&App_AB_F_TCB,&err);
         OSTimeDlyHMSM(0, 0, 0, 40, 
                      OS_OPT_TIME_HMSM_STRICT, 
                      &err);
    }
    }
}  

static void Passageiro(void *p_arg)  
{
    OS_ERR       err;
   while(1)
    {
    EstBotao_P = Pass_Read();
    if(EstBotao_P == 0){
        OSTimeDlyHMSM(0, 0, 0, 5, 
                      OS_OPT_TIME_HMSM_STRICT, 
                      &err);
    }else{
        OSTaskResume(&App_AB_P_TCB,&err);
        OSTimeDlyHMSM(0, 0, 0, 40, 
                      OS_OPT_TIME_HMSM_STRICT, 
                      &err);
    }
    }
}  

static void Ab_F(void *p_arg)
{
    OS_ERR       err;
    while(1)
    {
    CyDelay(30);               //   Delay de 30ms para acionar o primeiro conjunto de airbags
    AB_1_Write(1);
    AB_2_Write(1);
    AB_3_Write(1);
    AB_4_Write(1);
    CyDelay(10);               //   Delay de 40ms para acionar segundo conjunto de airbas
    AB_5_Write(1);
    AB_6_Write(1);
    AB_7_Write(1);
    AB_8_Write(1);
    CyDelay(5000);             //   Delay de 3 segundos para dar tempo de colher os dados com o osciloscópio. Após esse tempo, desliga todos os LEDs
    AB_1_Write(0);
    AB_2_Write(0);
    AB_3_Write(0);
    AB_4_Write(0);
    AB_5_Write(0);
    AB_6_Write(0);
    AB_7_Write(0);
    AB_8_Write(0);
    OSTimeDlyHMSM(0, 0, 0, 5000, 
                      OS_OPT_TIME_HMSM_STRICT, 
                      &err);
    }
}    

static void Ab_M(void *p_arg)
{
    OS_ERR       err;
    while(1)
    {
   CyDelay(12);
    AB_4_Write(1);
    AB_8_Write(1);
    CyDelay(5);
    AB_3_Write(1);
    AB_7_Write(1);
    CyDelay(13);
    AB_1_Write(1);
    AB_2_Write(1);
    AB_5_Write(1);
    AB_6_Write(1);
    CyDelay(5000);
    AB_1_Write(0);
    AB_2_Write(0);
    AB_3_Write(0);
    AB_4_Write(0);
    AB_5_Write(0);
    AB_6_Write(0);
    AB_7_Write(0);
    AB_8_Write(0);
    OSTimeDlyHMSM(0, 0, 0, 5000, 
                      OS_OPT_TIME_HMSM_STRICT, 
                      &err);
    }
}

static void Ab_P(void *p_arg)
{
    OS_ERR       err;
    while(1)
    {
   CyDelay(12);
    AB_3_Write(1);
    AB_7_Write(1);
    CyDelay(5);
    AB_4_Write(1);
    AB_8_Write(1);
    CyDelay(13);
    AB_1_Write(1);
    AB_2_Write(1);
    AB_5_Write(1);
    AB_6_Write(1);
    CyDelay(5000);
    AB_1_Write(0);
    AB_2_Write(0);
    AB_3_Write(0);
    AB_4_Write(0);
    AB_5_Write(0);
    AB_6_Write(0);
    AB_7_Write(0);
    AB_8_Write(0);
    OSTimeDlyHMSM(0, 0, 0, 5000, 
                      OS_OPT_TIME_HMSM_STRICT, 
                      &err);
    }
}
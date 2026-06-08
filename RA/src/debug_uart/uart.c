#include "uart.h"

/* ===== ????????????????STM32 main.c?????????===== */

volatile uint8_t uart2_flag = 0;
volatile uint8_t uart3_flag = 0;
volatile uint8_t uart4_flag = 0;
volatile uint8_t uart5_flag = 0;

volatile uint8_t RData2[255] = {0};
volatile uint8_t RData_Tap2 = 0;

volatile uint8_t RData4[255] = {0};
volatile uint8_t RData_Tap4 = 0;

float imu_x = 0;
float imu_y = 0;
float imu_z = 0;
float dht11 = 0;
float humi = 0;
float temp = 0;
float ph = 0;
float cond = 0;
float turb = 0;
uint8_t timedata[] = "00:00";

int ceshi=0;

/* ===== FSP???????????? ===== */
volatile bool uart2_send_complete_flag = false;
volatile bool uart3_send_complete_flag = false;
volatile bool uart4_send_complete_flag = false;
volatile bool uart5_send_complete_flag = false;

/* ===== ???????????? ===== */

/* UART2??????????STM32 USART1?????????? + printf?????*/
void UART2_Init(void){
    fsp_err_t err=FSP_SUCCESS;
    err=R_SCI_UART_Open(&g_uart2_ctrl,&g_uart2_cfg);
    assert(FSP_SUCCESS==err);
}

/* UART3??????????STM32 USART3??????TData[9]??*/
void UART3_Init(void){
    fsp_err_t err=FSP_SUCCESS;
    err=R_SCI_UART_Open(&g_uart3_ctrl,&g_uart3_cfg);
    assert(FSP_SUCCESS==err);
}

/* UART4??????????STM32 UART4??JSON????*/
void UART4_Init(void){
    fsp_err_t err=FSP_SUCCESS;
    err=R_SCI_UART_Open(&g_uart4_ctrl,&g_uart4_cfg);
    assert(FSP_SUCCESS==err);
}

/* UART5??????????STM32 USART2??????????????'A'???????*/
void UART5_Init(void){
    fsp_err_t err=FSP_SUCCESS;
    err=R_SCI_UART_Open(&g_uart5_ctrl,&g_uart5_cfg);
    assert(FSP_SUCCESS==err);
}


void uart2_callback(uart_callback_args_t *p_args){

    switch (p_args->event)
    {
        case UART_EVENT_RX_CHAR:{
            RData2[RData_Tap2++] = p_args->data;
            if (RData2[0] != 0x0A) RData_Tap2 = 0;
            if((RData2[RData_Tap2-1] == 0x0A)&&(RData2[RData_Tap2-2] == 0x0D)){
                RData_Tap2 = 0;
                uart2_flag = 1;
		    }
            break;
        }
        case UART_EVENT_TX_COMPLETE:{
            uart2_send_complete_flag = true;
            break;
        }
    default:break;
    }
}

/* UART3 ????????????STM32 USART3?????????*/
void uart3_callback(uart_callback_args_t *p_args){
    if (p_args->event == UART_EVENT_TX_COMPLETE){
        uart3_send_complete_flag = true;
    }
}

/* UART4 ??????????????STM32 HAL_UART_RxCpltCallback??UART4??????*/
void uart4_callback(uart_callback_args_t *p_args){
    switch (p_args->event)
    {
    case UART_EVENT_RX_CHAR:{
		RData4[RData_Tap4] = p_args->data;
		RData_Tap4++;
		if((RData4[RData_Tap4-1] == '}')&&(RData4[RData_Tap4-2] == '}')){
            uart4_flag = 1;
			RData_Tap4 = 0;
		}
        break;
    }
    case UART_EVENT_TX_COMPLETE:{
        uart4_send_complete_flag = true;
        break;
    }
    }
}

/* UART5 ??????????????STM32 HAL_UART_RxCpltCallback??USART2??????*/
void uart5_callback(uart_callback_args_t *p_args){

    switch (p_args->event)
    {
    case UART_EVENT_RX_CHAR:{
        if (p_args->data == 'A') uart5_flag = 1;
        break;
    }
    case UART_EVENT_TX_COMPLETE:{
        uart5_send_complete_flag = true;
        break;
    }
    default:break;
    }

}


/**
 * @brief printf?????UART2?????STM32 USART1??
 * @note  ????????????????TEI???????????
 */
int fputc(int ch, FILE *f)
{
    (void)f;
    R_SCI_UART_Write(&g_uart2_ctrl, (uint8_t *)&ch, 1);
    while (uart2_send_complete_flag == false);
    uart2_send_complete_flag = false;

    return ch;
}

/**
 * @brief ?????????????????
 */
void printf_string(char* name, char* showdata){
    printf("%s=\"%s\"\xff\xff\xff", name, showdata);
}

/**
 * @brief ????????????????
 */
void printf_number(char* name, int num){
    printf("%s=%d\xff\xff\xff", name, num);
}

/**
 * @brief ????????????????????10000???????
 */
void printf_float(char* name, float num){
    printf("%s=%d\xff\xff\xff", name, (int)(num * 10000));
}


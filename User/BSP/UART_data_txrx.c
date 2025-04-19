/**
 * @file UART_data_txrx.c
 * @author sethome
 * @brief �������ݷ��ͽ���
 * @version 0.1
 * @date 2022-11-20
 *
 * @copyright Copyright (c) 2022 sethome
 *
 */
// #include "fifo.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "fifo.h"

#include "UART_data_txrx.h"

#include "DT7.h"
#include "referee_system.h"
#include "Global_status.h"
#include "Auto_control.h"

// DMA���Ʊ���
extern DMA_HandleTypeDef hdma_uart5_rx; // ң���������ý���
extern DMA_HandleTypeDef hdma_uart7_rx; // ����7������ͼ��ģ��
extern DMA_HandleTypeDef hdma_uart7_tx;
extern DMA_HandleTypeDef hdma_usart10_rx; // ����10�����ӵ�Դ����ģ��
extern DMA_HandleTypeDef hdma_usart10_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern DMA_HandleTypeDef hdma_usart3_rx;
extern DMA_HandleTypeDef hdma_usart3_tx;

// ���ڿ��Ʊ���
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart5; // ң�����������ò���
extern UART_HandleTypeDef huart7;
extern UART_HandleTypeDef huart10;

// ����������+DMA���ϣ�������������
transmit_data UART1_data;
transmit_data UART2_data;
transmit_data UART3_data;
transmit_data UART5_data;
transmit_data UART7_data;
transmit_data UART10_data;

/**
 * @brief ���ڳ�ʼ��
 *
 * @return * void
 */
void uart_init(void)
{
  UART_DMA_rxtx_start(&UART1_data, &huart1, &hdma_usart1_rx, &hdma_usart1_rx);
  UART_DMA_rxtx_start(&UART2_data, &huart2, &hdma_usart2_rx, &hdma_usart2_rx);
  UART_DMA_rxtx_start(&UART3_data, &huart3, &hdma_usart3_rx, &hdma_usart3_rx);
  UART_DMA_rxtx_start(&UART5_data, &huart5, &hdma_uart5_rx, &hdma_uart5_rx);
  UART_DMA_rxtx_start(&UART7_data, &huart7, &hdma_uart7_rx, &hdma_uart7_tx);
  UART_DMA_rxtx_start(&UART10_data, &huart10, &hdma_usart10_rx, &hdma_usart10_tx);
}

/**
 * @brief DMA�������ж��������d(?��?`)o��ܰ��ʾ��������ͷ�ļ����ú��Զ��崮�ڻ�������С��
 * @param data �������ϰ�ָ��
 * @param huart ����ָ��
 * @param hdma_usart_rx ���ڽ���dmaָ��
 * @param hdma_usart_tx ���ڷ���dmaָ��
 */
void UART_DMA_rxtx_start(transmit_data *data, UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma_usart_rx, DMA_HandleTypeDef *hdma_usart_tx)
{
  data->huart = huart;                 // ���ڿ��Ʊ���
  data->hdma_usart_rx = hdma_usart_rx; // DMA���ջ���
  data->hdma_usart_tx = hdma_usart_tx; // DMA���ͻ���

  HAL_UARTEx_ReceiveToIdle_DMA(huart, data->rev_data, UART_BUFFER_SIZE); // ����DMA�������ݽ���
  __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);                        // �رս��ܹ����ж�
}

/**
 * @brief ���ڷ�������
 *
 * @param uart ���ʹ������ϰ�
 * @param data �������ݣ����ݱ��ͷ��ˣ���Ȼ�����ղ�����
 * @param size ���ݴ�С
 */
void UART_send_data(transmit_data uart, uint8_t data[], uint16_t size)
{
  //+++++++++++++++//while(HAL_DMA_GetState(UART6_data.hdma_usart_tx) != HAL_DMA_STATE_READY)
  HAL_UART_Transmit_DMA(uart.huart, data, size); // ���ީd(?��?`)o
}

/**
 * @brief ���ڽ��ܿջص����������ڽ��ܲ��������ݣ��������ݴ�����
 *
 * @param huart ���ܵ����ݵĴ���
 * @param Size ���ݴ�С
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if (huart == &huart5)
  {
    DT7_decode_data(UART5_data.rev_data);
    HAL_UARTEx_ReceiveToIdle_DMA(huart, UART5_data.rev_data, UART_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
  }
  else if (huart == &huart7)
  {
    fifo_s_puts(&referee_fifo, (char *)UART7_data.rev_data, (int)Size);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart7, UART7_data.rev_data, UART_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
  }
  else if (huart == &huart10)
  {
    fifo_s_puts(&referee_image_fifo, (char *)UART10_data.rev_data, (int)Size);
    HAL_UARTEx_ReceiveToIdle_DMA(huart, UART10_data.rev_data, UART_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
  }
  else if (huart == &huart1)
  {
    if (UART1_data.rev_data[0] == 0xA5)
    {
      Global.Auto.input.Auto_control_online = 20;
      decodeMINIPCdata(&fromMINIPC, UART1_data.rev_data, Size);
      MINIPC_to_STM32();
    }
    HAL_UARTEx_ReceiveToIdle_DMA(huart, UART1_data.rev_data, UART_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
  }
  else if (huart == &huart2)
  {
    HAL_UARTEx_ReceiveToIdle_DMA(huart, UART2_data.rev_data, UART_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
  }
  else if (huart == &huart3)
  {
    HAL_UARTEx_ReceiveToIdle_DMA(huart, UART3_data.rev_data, UART_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
  }
}

/**
 * @brief ���ڽ�������жϻص����������ڽ��ܶ�������
 *
 * @param huart ���ܴ��ں�
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
}

// ����������������
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart == &huart7)
  {
    HAL_UARTEx_ReceiveToIdle_DMA(&huart7, UART7_data.rev_data, UART_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
  }
  else if (huart == &huart10)
  {
    HAL_UARTEx_ReceiveToIdle_DMA(&huart10, UART10_data.rev_data, UART_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
  }
  else if (huart == &huart5)
  {
    HAL_UARTEx_ReceiveToIdle_DMA(&huart5, UART5_data.rev_data, UART_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
  }
  else if (huart == &huart1)
  {
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, UART1_data.rev_data, UART_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
  }
}
// end of file
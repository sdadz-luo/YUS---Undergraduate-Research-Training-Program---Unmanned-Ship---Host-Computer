#include "lora_proto.h"
#include "../debug_uart/uart.h"

/*============================================================================*
 * CRC-8 计算（多项式 0x31，初始 0x00，结果不异或）
 *============================================================================*/
uint8_t Lora_CalcCRC8(const uint8_t *data, uint16_t len)
{
    uint8_t crc = 0x00;

    for (uint16_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; bit++)
        {
            if (crc & 0x80)
                crc = (uint8_t)((crc << 1) ^ LORA_CRC_POLY);
            else
                crc <<= 1;
        }
    }

    return crc;
}

/*============================================================================*
 * 屏包构建: EE <船号> <组件> <数据> CRC8 FF  (6 字节)
 *============================================================================*/
void Lora_BuildScrPacket(uint8_t boat, uint8_t comp, uint8_t comp_data,
                         uint8_t *packet)
{
    packet[0] = SCR_PKT_HEADER;                 /* EE  */
    packet[1] = boat;                           /* 船号 */
    packet[2] = comp;                           /* 组件 */
    packet[3] = comp_data;                      /* 数据 */
    packet[4] = Lora_CalcCRC8(packet, SCR_PKT_CRC_LEN); /* CRC8 */
    packet[5] = SCR_PKT_TAIL;                   /* FF  */
}

/*============================================================================*
 * 摇杆包构建: CC 01 <小白方向> 02 <小黑方向> CRC8 33  (7 字节)
 *============================================================================*/
void Lora_BuildJoyPacket(uint8_t dir1, uint8_t dir2, uint8_t *packet)
{
    packet[0] = JOY_PKT_HEADER;                 /* CC     */
    packet[1] = JOY_PKT_BOAT1_ID;               /* 01     */
    packet[2] = JOYDIR_TO_LORA(dir1);           /* 小白方向 */
    packet[3] = JOY_PKT_BOAT2_ID;               /* 02     */
    packet[4] = JOYDIR_TO_LORA(dir2);           /* 小黑方向 */
    packet[5] = Lora_CalcCRC8(packet, JOY_PKT_CRC_LEN); /* CRC8 */
    packet[6] = JOY_PKT_TAIL;                   /* 33    */
}

/*============================================================================*
 * 发送数据包
 *============================================================================*/
void Lora_SendPacket(const uint8_t *packet, uint16_t len)
{
    Uart2_SendData(packet, len);
}

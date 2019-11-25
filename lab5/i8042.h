#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define KEYBOARD_IRQ 1
#define MOUSE_IRQ 12
#define MOUSE_IRQ_PIC2 4

#define ESC 0x81

#define FIRST_BYTE_SCAN 0xE0

#define KBC_STAT_REG 0x64
#define KBC_CMD_REG 0x64
#define KBC_OUT_BUF 0x60
#define KBC_IN_BUF 0x60

#define PAR_ERR BIT(7)
#define TO_ERR BIT(6)
#define OBF  BIT(0)
#define IBF BIT(1)
#define AUX BIT(5)

#define READ_COM_BYTE 0x20
#define WRITE_COM_BYTE 0x60
#define CHECK_KBC 0xAA
#define CHECK_KBD_INTERFACE 0xAB
#define DIS_KBD_INTERFACE 0xAD
#define EN_KBD_INTERFACE 0xAE
#define WRITE_MOUSE_BYTE 0xD4

#define POLL_DELAY 20000
#define KBC_N_TRIES 2

#define BREAK_CODE BIT(7)
#define ENABLE_INT_KBD BIT(0)
#define ENABLE_INT_MOUSE BIT(1)
#define DISABLE_MOUSE BIT(5)
#define DISABLE_KBD_INTERFACE BIT(4)

#define READ_DATA 0xEB          
#define STREAM_MODE 0xEA         
#define ENABLE_DATA_REPORT 0xF4  
#define DISABLE_DATA_REPORT 0xF5 
#define REMOTE_MODE 0xF0  

#define LB BIT(0)     
#define RB BIT(1)     
#define MB BIT(2)     
#define X_OV BIT(6)  
#define Y_OV BIT(7)  
#define X_MSB BIT(4) 
#define Y_MSB BIT(5)

#define LB_BIT 0
#define RB_BIT 1
#define MB_BIT 2
#define X_OV_BIT 6
#define Y_OV_BIT 7
#define X_MSB_BIT 4 
#define Y_MSB_BIT 5 

#define ACK 0xFA
#define NACK 0xFE
#define ERROR 0xFC

#define DISABLE_IRQ_LINE 0
#define ENABLE_IRQ_LINE 1

#endif /* _LCOM_I8042_H */

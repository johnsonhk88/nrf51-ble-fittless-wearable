/*************************************************************
0.96 inches OLED driver header file




**************************************************************/


#ifndef     _096_OLED_H
#define     _096_OLED_H


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf_gpio.h"



//OLEDģʽ����
//0:4�ߴ���ģʽ
//1:����8080ģʽ
#define OLED_MODE 0



//-----------------OLED�˿ڶ���----------------  					   

#define OLED_CS_Clr()   nrf_gpio_pin_clear(SPI0_SS)   // GPIO_ResetBits(GPIOD,GPIO_Pin_6)
#define OLED_CS_Set()   nrf_gpio_pin_set(SPI0_SS)//GPIO_SetBits(GPIOD,GPIO_Pin_6)

#define OLED_RST_Clr()   nrf_gpio_pin_clear(OLED_RST)      //GPIO_ResetBits(GPIOG,GPIO_Pin_15)
#define OLED_RST_Set()   nrf_gpio_pin_set(OLED_RST)  //GPIO_SetBits(GPIOG,GPIO_Pin_15)

#define OLED_RS_Clr()    nrf_gpio_pin_clear(OLED_RS)//GPIO_ResetBits(GPIOD,GPIO_Pin_3)
#define OLED_RS_Set()    nrf_gpio_pin_set(OLED_RS)  //GPIO_SetBits(GPIOD,GPIO_Pin_3)


#define OLED_WR_Clr()   //GPIO_ResetBits(GPIOG,GPIO_Pin_14)
#define OLED_WR_Set()   //GPIO_SetBits(GPIOG,GPIO_Pin_14)

#define OLED_RD_Clr()   //GPIO_ResetBits(GPIOG,GPIO_Pin_13)
#define OLED_RD_Set()   //GPIO_SetBits(GPIOG,GPIO_Pin_13)



//PC0~7,��Ϊ������
#define DATAOUT(x)    //GPIO_Write(GPIOC,x);//���  
//ʹ��4�ߴ��нӿ�ʱʹ�� 

#define OLED_SCLK_Clr() //GPIO_ResetBits(GPIOC,GPIO_Pin_0)
#define OLED_SCLK_Set() //GPIO_SetBits(GPIOC,GPIO_Pin_0)

#define OLED_SDIN_Clr() //GPIO_ResetBits(GPIOC,GPIO_Pin_1)
#define OLED_SDIN_Set() //GPIO_SetBits(GPIOC,GPIO_Pin_1)



#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����
//OLED�����ú���
void OLED_WR_Byte(uint8_t dat,uint8_t cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);		   
							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
void OLED_ShowString(uint8_t x,uint8_t y, uint8_t *p);	 





#endif


#include "oled.h"


#define    OLED_SCL_PORT_PIN                   __GET_PIN(B, 0)
#define    OLED_SDA_PORT_PIN                   __GET_PIN(A, 7)
#define    OLED_RST_PORT_PIN                   __GET_PIN(A, 6)
#define    OLED_DC_PORT_PIN                    __GET_PIN(A, 5)
#define    PIN_HIGH                            1
#define    PIN_LOW                             0

#if  (CONFIG_CS_PIN == 1)
    #define     OLED_CS_PORT_PIN                __GET_PIN(A, 4)
#endif


#if  defined (STM32F10X_HD)

#define    __OLED_PIN_CONFIG(port,pin)          {GPIO_InitStructure.GPIO_Pin = GPIO_Pin_##pin;  GPIO_Init(GPIO##port, &GPIO_InitStructure); }
#define    __OLED_PORT(port,pin)                 GPIO##port, GPIO_Pin_##pin    //port =A,B,C,D...
#define    __GET_PIN(port,pin)                   port, pin

#define    OLED_PIN_CLOCK_ENABLE(status)         RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE); 

#define    OLED_PIN_CONFIG(scl,sda,rst,dc,cs)    { \
                                                    GPIO_InitTypeDef GPIO_InitStructure; \
                                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; \
                                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; \
                                                    __OLED_PIN_CONFIG(scl);   \
                                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  \
                                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; \
                                                    __OLED_PIN_CONFIG(sda);   \
                                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; \
                                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; \
                                                    __OLED_PIN_CONFIG(rst);   \
                                                    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; \
                                                    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; \
                                                    __OLED_PIN_CONFIG(dc);   \
                                                  }

#define    OLED_CS_PIN_CONFIG(cs)                 {   \
                                                     GPIO_InitTypeDef GPIO_InitStructure; \
                                                     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; \
                                                     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; \
                                                     __OLED_PIN_CONFIG(cs);   \
                                                  }
                                                
#endif                                          
                                                
#define    __OLED_PIN(port,pin,value)             {  if(value == PIN_HIGH) \
                                                         GPIO_SetBits(__OLED_PORT(port, pin)); \
                                                     else \
                                                         GPIO_ResetBits(__OLED_PORT(port, pin));    \
                                                  }



#define    __OLED_PIN_SET(pin,value)              __OLED_PIN(pin,value)
                                                 
#define    OLED_SCL_H()                           { __OLED_PIN_SET(OLED_SCL_PORT_PIN,PIN_HIGH) }
#define    OLED_SCL_L()                           { __OLED_PIN_SET(OLED_SCL_PORT_PIN,PIN_LOW)  }
                                                  
#define    OLED_SDA_H()                           { __OLED_PIN_SET(OLED_SDA_PORT_PIN,PIN_HIGH) }
#define    OLED_SDA_L()                           { __OLED_PIN_SET(OLED_SDA_PORT_PIN,PIN_LOW)  }
                                                  
#define    OLED_RST_H()                           { __OLED_PIN_SET(OLED_RST_PORT_PIN,PIN_HIGH) }
#define    OLED_RST_L()                           { __OLED_PIN_SET(OLED_RST_PORT_PIN,PIN_LOW)  }   
                                                  
#define    OLED_DC_H()                            { __OLED_PIN_SET(OLED_DC_PORT_PIN,PIN_HIGH)  }
#define    OLED_DC_L()                            { __OLED_PIN_SET(OLED_DC_PORT_PIN,PIN_LOW)   }
                                                 
#if  (CONFIG_CS_PIN == 1)                        
    #define  OLED_CS_H()                          { __OLED_PIN_SET(OLED_CS_PORT_PIN,PIN_HIGH)  }
    #define  OLED_CS_L()                          { __OLED_PIN_SET(OLED_CS_PORT_PIN,PIN_LOW)   }
#endif



static unsigned char GRAM[128][8] = {0};




/******************************************************************************
 * @brief      OLED软件延时函数
 *
 * @param[in]  Time  :  延时时间
 *
 * @return     无
 *
 ******************************************************************************/
static void OLED_Delay(uint32_t Time)
{
    while (Time--);
}




/******************************************************************************
 * @brief     初始化OLED使用的引脚
 *
 * @param[in]  无
 *
 * @return     无
 *
 ******************************************************************************/
static void OLED_IOInit(void)       //oled IO口初始化
{

    OLED_PIN_CLOCK_ENABLE();
    OLED_PIN_CONFIG(OLED_SCL_PORT_PIN, OLED_SDA_PORT_PIN, OLED_RST_PORT_PIN, OLED_DC_PORT_PIN, OLED_CS_PORT_PIN);

#if  (CONFIG_CS_PIN == 1)
    OLED_CS_PIN_CONFIG(OLED_CS_PORT_PIN);
    OLED_CS_L();   //cs使能
#endif
    
} 




/******************************************************************************
 * @brief      OLED写入命令
 *
 * @param[in]  Cmd  :  要写入的命令
 *
 * @return     无
 *
 ******************************************************************************/
static void OLED_WriteCmd(uint8_t Cmd)
{
    uint8_t n = 8;
    OLED_DC_L();
    OLED_SCL_L();

    while (n){
        
        if (Cmd & 0x80){
            OLED_SDA_H();
        } else {
            OLED_SDA_L();
        }

        OLED_SCL_H(); 
        OLED_SCL_L();

        Cmd <<= 1;
        n--;
    }
}




/******************************************************************************
 * @brief      OLED写入数据
 *
 * @param[in]  Data  :  要写入的数据
 *
 * @return     无
 *
 ******************************************************************************/
static void OLED_WriteData(uint8_t Data)
{
    uint8_t n = 8;

    OLED_DC_H();
    OLED_SCL_L();

    while (n){
        
        if (Data & 0x80) {
            OLED_SDA_H();
        } else {
            OLED_SDA_L();
        }

        OLED_SCL_H();
        OLED_SCL_L();

        Data <<= 1;
        n--;
    }
}




/******************************************************************************
 * @brief      设置OLED显示坐标
 *
 * @param[in]  x  :  x坐标
 * @param[in]  y  :  y坐标
 *
 * @return     无
 *
 ******************************************************************************/
static void OLED_SetPos(uint8_t x,uint8_t y)
{
    OLED_WriteCmd(0xb0 + y);
    OLED_WriteCmd(((x & 0xf0) >> 4) | 0x10);
    OLED_WriteCmd((x & 0x0f) | 0x01);
}




/******************************************************************************
 * @brief      初始化OLED内部寄存器
 *
 * @param[in]  无
 *
 * @return     无
 *
 ******************************************************************************/
static void OLED_RegInit(void)
{
    OLED_WriteCmd(0xae);//--turn off SSD1306 panel
    OLED_WriteCmd(0x00);//---set low column address
    OLED_WriteCmd(0x10);//---set high column address
    OLED_WriteCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_WriteCmd(0x81);//--set contrast control register
    OLED_WriteCmd(0xcf); // Set SEG Output Current Brightness
    OLED_WriteCmd(0xa1);//--Set SEG/Column Mapping     0xa0???? 0xa1??
    OLED_WriteCmd(0xc8);//Set COM/Row Scan Direction   0xc0???? 0xc8??
    OLED_WriteCmd(0xa6);//--set normal display
    OLED_WriteCmd(0xa8);//--set multiplex ratio(1 to 64)
    OLED_WriteCmd(0x3f);//--1/64 duty
    OLED_WriteCmd(0xd3);//-set display offset    Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WriteCmd(0x00);//-not offset
    OLED_WriteCmd(0xd5);//--set display clock divide ratio/oscillator frequency
    OLED_WriteCmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WriteCmd(0xd9);//--set pre-charge period
    OLED_WriteCmd(0xf1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_WriteCmd(0xda);//--set com pins hardware configuration
    OLED_WriteCmd(0x12);
    OLED_WriteCmd(0x40);//Set VCOM Deselect Level
    OLED_WriteCmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_WriteCmd(0x02);//
    OLED_WriteCmd(0x8d);//--set Charge Pump enable/disable
    OLED_WriteCmd(0x14);//--set(0x10) disable
    OLED_WriteCmd(0xa4);// Disable Entire Display On (0xa4/0xa5)
    OLED_WriteCmd(0xa6);// Disable Inverse Display On (0xa6/a7) 
    OLED_WriteCmd(0xaf);//--turn on SSD1306 panel
}




/******************************************************************************
 * @brief      刷新OLED的颜色值.OLED_ClearDispaly(0xf0)
 *
 * @param[in]  color  :  要刷新的颜色值
 *
 * @return     无
 *
 ******************************************************************************/
void OLED_ClearDispaly(uint8_t color)
{
    uint8_t x, y;

    for (y = 0; y < 8; y++) {
        for (x = 0; x < OLED_SIZE_X; x++) {
            GRAM[x][y] = color;
        }
    }

}




/******************************************************************************
 * @brief      刷新OLED
 *
 * @param[in]  无
 *
 * @return     无
 *
 ******************************************************************************/
void OLED_Refresh(void)
{
    uint8_t x, y;

    for (y = 0; y < 8; y++) {
        OLED_WriteCmd(0xb0 + y);
        OLED_WriteCmd(0x01);
        OLED_WriteCmd(0x10);
        for (x = 0; x < OLED_SIZE_X; x++){
            OLED_WriteData(GRAM[x][y]);
        }
    }

}




/******************************************************************************
 * @brief      初始化OLED
 *
 * @param[in]  无
 *
 * @return     无
 *
 ******************************************************************************/
void OLED_Init(void)
{
    OLED_IOInit();   //初始化oled所用的IO口

    OLED_RST_L();    //复位一段时间
    OLED_Delay(10000);
    OLED_RST_H();

    OLED_RegInit();   //oled寄存器配置

    OLED_ClearDispaly(0x00);
    OLED_Refresh();
    OLED_SetPos(0, 0);
}




/******************************************************************************
 * @brief      OLED画点函数
 *
 * @param[in]  x  :  画点的x坐标
 * @param[in]  y  :  画点的y坐标
 * @param[in]  c  :  画点的颜色值, 1画点, 0清除
 *
 * @return     无
 *
 ******************************************************************************/
void OLED_DrawPoint(unsigned char x,unsigned char y,unsigned char c)
{
    unsigned char pos,bx,temp=0;
    if (x > 127 || y > 63)
        return;//超出范围了

    pos = 7 - y / 8;
    bx = y % 8;
    temp = 1 << (7 - bx);

    if (c)
        GRAM[x][pos] |= temp;
    else 
        GRAM[x][pos] &= ~temp;        
}

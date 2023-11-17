#include "esp8266.h"

#define Esp8266DelayMs(t)         vTaskDelay(t)

#if defined (STM32F10X_HD)

#include "stm32f10x.h"
		
#endif


#if !defined (STM32F10X_MD) && !defined (STM32F40_41xxx)
int uart_send_byte(char ch)
{
    while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
    USART2->DR = (u8) ch; 
	return 0;
}

#endif

//指令最多接受三个参数
//[0]:表示发送的指令
//[1]:接收到的返回值，
//[2]:表示该指令有几个参数 = 0,表示没有参数  [3] [4] [5]不再使用
//[3]:第一个参数
//[4]:第二个参数
//[5]:第三个参数

typedef struct Esp8266_AtCommand_t {
    char*   at_command;                // AT指令
    char*   response;                  // 回应
    int     cmd_parameter_cnt;         // 参数个数
}Esp8266_AtCommand_t;


Esp8266_AtCommand_t ESP8266_AtCommand[] = {
    {"AT",                        "OK",                0},
    {"AT+RST",                    "ready",             0},
    {"AT+GMR",                    "OK",                0},
    {"AT+CWMODE",                 "OK",                1},
    //作为Ap		
    {"AT+CWSAP",                  "OK",                2},
    {"AT+CIPMUX",                 "OK",                1},
    {"AT+CIPSERVER",              "OK",                1},
    {"AT+CIPSEND",                 "OK",               2},
    //作为client		
    {"AT+CIFSR",                  "OK",                0},
    {"AT+CIPSTART",               "OK",                3},
    {"AT+CWJAP",                  "OK",                2},
    {"AT+CIPMODE",                "OK",                1},
    {"AT+CIPSEND",                ">",                 0},
};	


static int ESP8266_ReceiveByte(ESP8266_t *esp8266,char rx_data);


int ESP8266_Init(ESP8266_t *esp8266, ESP8266_USART_t usart, uint32_t baud) //IO初始化
{ 
	if (!esp8266) 
        return -1;

	//初始化串口和中断
#if defined (STM32F10X_MD)
    if (usart == USART2) {
        //GPIO端口设置
        GPIO_InitTypeDef    GPIO_InitStructure;
        USART_InitTypeDef   USART_InitStructure;
        NVIC_InitTypeDef    NVIC_InitStructure;
        
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能GPIOA时钟
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);//使能USART时钟
                    
        GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_2; //PA.2
        GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF_PP;	//复用推挽输出
        GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2
         
        //USART1_RX	 
        GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_3; //PA3
        GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IN_FLOATING;//浮空输入
        GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化GPIOA.3

        //Usart1 NVIC 配置
        NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
        NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
        
        //USART 初始化设置
        USART_InitStructure.USART_BaudRate            = baud;//串口波特率
        USART_InitStructure.USART_WordLength          = USART_WordLength_8b;//字长为8位数据格式
        USART_InitStructure.USART_StopBits            = USART_StopBits_1;//一个停止位
        USART_InitStructure.USART_Parity              = USART_Parity_No;//无奇偶校验位
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
        USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

        USART_Init(USART2, &USART_InitStructure); //初始化串口2
        USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
        USART_Cmd(USART2, ENABLE);                    //使能串口2 
    } else 
        printf("ESP8266_Init port no\r\n");
    
#endif	

	esp8266->usart                   =  usart;
	esp8266->init                    =  true;	
	esp8266->mode                    =  ESP8266_DefaultMode;
	esp8266->ESP8266_ReceiveByte     =  ESP8266_ReceiveByte;
	esp8266->rx_max_length           =  ESP8266_DefaultReceiveLength;
	esp8266->rx_length               =  0;
	memset(esp8266->rx_buffer, 0, esp8266->rx_max_length);
    
	return 0 ;
}			

//发送一个字节数据
static int ESP8266_SendByte(ESP8266_t *esp8266, char ch)
{
    if (!esp8266 || !esp8266->init) //初始化之后
        return -1;  
    
    while ((esp8266->usart->SR & 0X40) == 0);     //循环发送,直到发送完毕   
    esp8266->usart->DR = (u8)ch;
	return 0;
}

//发送指定长度字节的数据
//data          数据指针，
//data_length   数据长度
int ESP8266_SendData(ESP8266_t *esp8266, char* data, char data_length)
{
    if (!esp8266 || !esp8266->init)  
        return -1;  //初始化之后
    for (int i = 0; i < data_length; i++){
        ESP8266_SendByte(esp8266,data[i]);
    }
}


//接受一个字节数据
static int ESP8266_ReceiveByte(ESP8266_t *esp8266, char rx_data)
{
    if (!esp8266 || !esp8266->init) 
        return -1;  //初始化之后
     
    //超出缓冲区大小,数据可以需要接受
    esp8266->rx_buffer[esp8266->rx_length % esp8266->rx_max_length] = rx_data;
    esp8266->rx_length++;
    if (esp8266->rx_length >= esp8266->rx_max_length)
        printf("esp8266->rx_length: %d\r\n",esp8266->rx_length);
        
}

int ESP8266_RstParameter(ESP8266_t *esp8266)
{
    if (!esp8266 || !esp8266->init)
        return -1;
    memset(esp8266->rx_buffer, 0, esp8266->rx_max_length);
    esp8266->rx_length = 0; 	
}

int ESP8266_PrintReceiveData(ESP8266_t *esp8266)
{
    if (!esp8266 || !esp8266->init) 
        return -1;
    ESP8266_DefaultDebug("ReceiveData:\r\n%s\r\n", esp8266->rx_buffer);
}

//发送at指令
int ESP8266_SendAtCommand(ESP8266_t *esp8266, Esp8266AtCommand_t at)
{
	if (!esp8266 || !esp8266->init) 
        return -1;
    
	char at_cmd_buff[50];
	memset(at_cmd_buff, 0, sizeof(at_cmd_buff));
	ESP8266_RstParameter(esp8266);
	
    if(at ==  Esp8266AtCommand_AT_CWMODE) {  //携带一个参数
        sprintf(at_cmd_buff, "%s=%d\r\n", ESP8266_AtCommand[at].at_command, esp8266->mode);
        int at_len = strlen(at_cmd_buff);
        ESP8266_SendData(esp8266, at_cmd_buff, at_len);
	} else if (at ==  Esp8266AtCommand_AT_CIPSTART) {  //带3个参数
        //AT+CIPSTART="TCP", "192.168.101.12", 8080
        if(esp8266->mode == ESP8266_ModeClient) { //作为客户端时，去连接服务器
            sprintf(at_cmd_buff, "%s=\"%s\",\"%s\",%s\r\n", ESP8266_AtCommand[at].at_command, "TCP" , ESP8266_DefaultServerIp, ESP8266_DefaultServerPort);
            printf("send at cmd: %s\r\n", at_cmd_buff);
            int at_len = strlen(at_cmd_buff);
            ESP8266_SendData(esp8266, at_cmd_buff, at_len);
        }	
	} else if (at ==  Esp8266AtCommand_AT_CWSAP) { //打开热点
        //"AT+CWSAP=\"WIFI\",\"123456\",11,0,4,0"
        sprintf(at_cmd_buff, "%s=\"%s\",\"%s\",11,0,4,0\r\n", ESP8266_AtCommand[at].at_command, ESP8266_DefaultApName, ESP8266_DefaultApPassword);
        printf("send at cmd: %s\r\n", at_cmd_buff);
        int at_len = strlen(at_cmd_buff);			
        ESP8266_SendData(esp8266, at_cmd_buff, at_len);
	} else if (at ==  Esp8266AtCommand_AT_CIPMUX) { //设置透传模式
        //AT+CIPMUX=1
        sprintf(at_cmd_buff, "%s=%d\r\n", ESP8266_AtCommand[at].at_command, 1);
        printf("send at cmd: %s\r\n", at_cmd_buff);
        int at_len = strlen(at_cmd_buff);
        ESP8266_SendData(esp8266, at_cmd_buff,at_len);
	} else if (at ==  Esp8266AtCommand_AT_CIPSERVER) { //建立服务器
        //AT+CIPSERVER=1,8086
        sprintf(at_cmd_buff, "%s=%s,%s\r\n", ESP8266_AtCommand[at].at_command, "1", ESP8266_DefaultServerPort);
        printf("send at cmd: %s\r\n", at_cmd_buff);
        int at_len = strlen(at_cmd_buff);
        ESP8266_SendData(esp8266, at_cmd_buff, at_len);
	} else if (at ==  Esp8266AtCommand_AT_CWJAP) {  //连接热点
        //AT+CWJAP="ALIENTEK","15902020353"
        sprintf(at_cmd_buff, "%s=\"%s\",\"%s\"\r\n", ESP8266_AtCommand[at].at_command, ESP8266_DefaultWiFiName, ESP8266_DefaultWiFiPassword);
        printf("send at cmd: %s\r\n",at_cmd_buff);
        int at_len = strlen(at_cmd_buff);
        ESP8266_SendData(esp8266,at_cmd_buff,at_len);
	} else if(at ==  Esp8266AtCommand_AT_CIPMODE) {    //开启透传模式
        sprintf(at_cmd_buff, "%s=%d\r\n",ESP8266_AtCommand[at].at_command, 1);
        printf("send at cmd: %s\r\n", at_cmd_buff);
        int at_len = strlen(at_cmd_buff);
        ESP8266_SendData(esp8266, at_cmd_buff, at_len);
	} else if(at ==  Esp8266AtCommand_AT_CIPSEND) {  //开始透传
        //sprintf(at_cmd_buff,"%s\r\n",ESP8266_AtCommand[at].at_command);
        sprintf(at_cmd_buff, "AT+CIPSEND\r\n");
        printf("send at cmd: %s\r\n", at_cmd_buff);
        int at_len = strlen(at_cmd_buff);
        ESP8266_SendData(esp8266, at_cmd_buff, at_len);
	} else {
        sprintf(at_cmd_buff, "%s\r\n",ESP8266_AtCommand[at].at_command);
        int at_len = strlen(at_cmd_buff);
        ESP8266_SendData(esp8266, at_cmd_buff, at_len);  
	}
	
	return 0;
}	

//查找某个已发送指令的返回值是否存在
int ESP8266_FindString(ESP8266_t *esp8266,Esp8266AtCommand_t at)
{
	if (!esp8266 || !esp8266->init) 
        return -1;
	
	int i = 100;
	do {
		if(strstr(esp8266->rx_buffer, ESP8266_AtCommand[at].response) != NULL) //找到 
		   	return 0;   //找到
		else   //未找到匹配的字符
            Esp8266DelayMs(100);                 
		i--;
	}while(i > 0);
	
	return -2;
}	


/****************作为从机时********************/

//连接热点,并且连接上服务器，之后，可以直接向服务器发送数据
//ssid: WiFi用户名
//pd: WiFi密码
int ESP8266_ConnectWiFi(ESP8266_t *esp8266) 
{
    if (!esp8266 || !esp8266->init) 
        return -1;
    int res = 0;

    //检测有没有wifi模块
    ESP8266_SendAtCommand(esp8266,Esp8266AtCommand_AT);		
    res = ESP8266_FindString(esp8266,Esp8266AtCommand_AT);
    if(!res)
        printf("AT is ok\r\n");
    else
        printf("AT is fail\r\n");	

		
    //配置wifi模块模式
    ESP8266_SendAtCommand(esp8266, Esp8266AtCommand_AT_CWMODE);
//		delay_ms(1000);
//		ESP8266_PrintReceiveData(esp8266);	 
    res = ESP8266_FindString(esp8266, Esp8266AtCommand_AT_CWMODE);
    if(!res)
         printf("config mode is ok\r\n");
    else
          printf("config mode is fail\r\n");	

    //复位wifi模块
    ESP8266_SendAtCommand(esp8266, Esp8266AtCommand_AT_RST);		
    res = ESP8266_FindString(esp8266, Esp8266AtCommand_AT_RST);
    if(!res)
          printf("RST is ok\r\n");
    else
          printf("RST is fail\r\n");	
    
    RECONFIG_WIFI:;
    //连接wifi
    ESP8266_SendAtCommand(esp8266, Esp8266AtCommand_AT_CWJAP);
    res = ESP8266_FindString(esp8266, Esp8266AtCommand_AT_CWJAP);
    if(!res)
          printf("connect is ok\r\n");
    else {
          printf("connect is fail\r\n");
          ESP8266_PrintReceiveData(esp8266);		
    }
    //显示当前的wifi IP地址
    ESP8266_SendAtCommand(esp8266, Esp8266AtCommand_AT_CIFSR);	
    res = ESP8266_FindString(esp8266, Esp8266AtCommand_AT_CIFSR);	
    if(!res) {
        printf("get ip ok\r\n");
        // ESP8266_PrintReceiveData(esp8266);
        char *p = strstr(esp8266->rx_buffer, "STAIP");
        char *ip = p + 7;
        p = ip;
        p = strstr(p, "\"");
        p[0] = '\0';
        printf("ip: %s\r\n", ip);
        printf("p: %s\r\n", p);
        char buf[20];
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "ip: %s\r\nport: %s", ip, ESP8266_DefaultServerPort);
        printf("%s\r\n",buf);
    }
            
    //连接上服务器，显示IP                
    ESP8266_SendAtCommand(esp8266, Esp8266AtCommand_AT_CIPSTART);
    res = ESP8266_FindString(esp8266, Esp8266AtCommand_AT_CIPSTART);
    if(!res)
          printf("connect server is ok\r\n");
    else {
          printf("connect server is fail\r\n");
          ESP8266_PrintReceiveData(esp8266);	
    }

    //开启透传模式
    ESP8266_SendAtCommand(esp8266, Esp8266AtCommand_AT_CIPMODE);
    res = ESP8266_FindString(esp8266, Esp8266AtCommand_AT_CIPMODE);
    if(!res)
          printf("start mode ok\r\n");
    else{
          printf("start mode fail\r\n");
          ESP8266_PrintReceiveData(esp8266);
          goto RECONFIG_WIFI;            
    }
    
    //开始透传
    ESP8266_SendAtCommand(esp8266, Esp8266AtCommand_AT_CIPSEND);
    res = ESP8266_FindString(esp8266, Esp8266AtCommand_AT_CIPSEND);
    if(!res)
          printf("start ok\r\n");
    else {
          printf("start fail\r\n");
          ESP8266_PrintReceiveData(esp8266);	
    }
    
    return 0;
}

//必须先执行ESP8266_ConnectWiFi()函数
//从wifi模块想服务器发送一定量的数据
int ESP8266_SendDataToServer(ESP8266_t *esp8266, char* buffer, int len)
{
    if (!esp8266 || !esp8266->init) 
        return -1;
    ESP8266_SendData(esp8266, buffer, len);
}


#ifndef _MQTT_H_
#define _MQTT_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>


#ifndef uint1_t
typedef _Bool			uint1_t;
#endif


//=============================����==============================
//===========�����ṩRTOS���ڴ��������Ҳ����ʹ��C���=========
//RTOS
#include <stdlib.h>

#define MQTT_MallocBuffer	malloc

#define MQTT_FreeBuffer		free
//==========================================================


#define MOSQ_MSB(A)         (uint8_t)((A & 0xFF00) >> 8)
#define MOSQ_LSB(A)         (uint8_t)(A & 0x00FF)


/*--------------------------------�ڴ���䷽����־--------------------------------*/
#define MEM_FLAG_NULL		0
#define MEM_FLAG_ALLOC		1
#define MEM_FLAG_STATIC		2


typedef struct Buffer
{
	
	uint8_t	*_data;		//Э������
	
	uint32_t	_len;		//д������ݳ���
	
	uint32_t	_size;		//�����ܴ�С
	
	uint8_t	_memFlag;	//�ڴ�ʹ�õķ�����0-δ����	1-ʹ�õĶ�̬����		2-ʹ�õĹ̶��ڴ�
	
} MQTT_PACKET_STRUCTURE;


/*--------------------------------�̶�ͷ����Ϣ����--------------------------------*/
enum MqttPacketType
{
	
    MQTT_PKT_CONNECT = 1, /**< �����������ݰ� */
    MQTT_PKT_CONNACK,     /**< ����ȷ�����ݰ� */
    MQTT_PKT_PUBLISH,     /**< �����������ݰ� */
    MQTT_PKT_PUBACK,      /**< ����ȷ�����ݰ� */
    MQTT_PKT_PUBREC,      /**< ���������ѽ������ݰ���Qos 2ʱ���ظ�MQTT_PKT_PUBLISH */
    MQTT_PKT_PUBREL,      /**< ���������ͷ����ݰ��� Qos 2ʱ���ظ�MQTT_PKT_PUBREC */
    MQTT_PKT_PUBCOMP,     /**< ����������ݰ��� Qos 2ʱ���ظ�MQTT_PKT_PUBREL */
    MQTT_PKT_SUBSCRIBE,   /**< �������ݰ� */
    MQTT_PKT_SUBACK,      /**< ����ȷ�����ݰ� */
    MQTT_PKT_UNSUBSCRIBE, /**< ȡ���������ݰ� */
    MQTT_PKT_UNSUBACK,    /**< ȡ������ȷ�����ݰ� */
    MQTT_PKT_PINGREQ,     /**< ping ���ݰ� */
    MQTT_PKT_PINGRESP,    /**< ping ��Ӧ���ݰ� */
    MQTT_PKT_DISCONNECT,  /**< �Ͽ��������ݰ� */
	
	//����
	
	MQTT_PKT_CMD  		 /**< �����·����ݰ� */
	
};


/*--------------------------------MQTT QOS�ȼ�--------------------------------*/
enum MqttQosLevel
{
	
    MQTT_QOS_LEVEL0,  /**< ��෢��һ�� */
    MQTT_QOS_LEVEL1,  /**< ���ٷ���һ��  */
    MQTT_QOS_LEVEL2   /**< ֻ����һ�� */
	
};


/*--------------------------------MQTT ���������־λ���ڲ�ʹ��--------------------------------*/
enum MqttConnectFlag
{
	
    MQTT_CONNECT_CLEAN_SESSION  = 0x02,
    MQTT_CONNECT_WILL_FLAG      = 0x04,
    MQTT_CONNECT_WILL_QOS0      = 0x00,
    MQTT_CONNECT_WILL_QOS1      = 0x08,
    MQTT_CONNECT_WILL_QOS2      = 0x10,
    MQTT_CONNECT_WILL_RETAIN    = 0x20,
    MQTT_CONNECT_PASSORD        = 0x40,
    MQTT_CONNECT_USER_NAME      = 0x80
	
};


/*--------------------------------��Ϣ��packet ID�����Զ���--------------------------------*/
#define MQTT_PUBLISH_ID			10

#define MQTT_SUBSCRIBE_ID		20

#define MQTT_UNSUBSCRIBE_ID		30


/*--------------------------------ɾ��--------------------------------*/
void MQTT_DeleteBuffer(MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------���--------------------------------*/
uint8_t MQTT_UnPacketRecv(uint8_t *dataPtr);

/*--------------------------------��¼���--------------------------------*/
uint8_t MQTT_PacketConnect(const int8_t *user, const int8_t *password, const int8_t *devid,
						uint16_t cTime, uint1_t clean_session, uint1_t qos,
						const int8_t *will_topic, const int8_t *will_msg, int32_t will_retain,
						MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------�Ͽ��������--------------------------------*/
uint1_t MQTT_PacketDisConnect(MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------������Ӧ���--------------------------------*/
uint8_t MQTT_UnPacketConnectAck(uint8_t *rev_data);

/*--------------------------------���ݵ��ϴ����--------------------------------*/
uint1_t MQTT_PacketSaveData(const int8_t *devid, int16_t send_len, int8_t *type_bin_head, uint8_t type, MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------�������ļ��ϴ����--------------------------------*/
uint1_t MQTT_PacketSaveBinData(const int8_t *name, int16_t file_len, MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------�����·����--------------------------------*/
uint8_t MQTT_UnPacketCmd(uint8_t *rev_data, int8_t **cmdid, int8_t **req, uint16_t *req_len);

/*--------------------------------����ظ����--------------------------------*/
uint1_t MQTT_PacketCmdResp(const int8_t *cmdid, const int8_t *req, MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------�����������--------------------------------*/
uint8_t MQTT_PacketSubscribe(uint16_t pkt_id, enum MqttQosLevel qos, const int8_t *topics[], uint8_t topics_cnt, MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------��������ظ����--------------------------------*/
uint8_t MQTT_UnPacketSubscribe(uint8_t *rev_data);

/*--------------------------------ȡ���������--------------------------------*/
uint8_t MQTT_PacketUnSubscribe(uint16_t pkt_id, const int8_t *topics[], uint8_t topics_cnt, MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------ȡ�����Ļظ����--------------------------------*/
uint1_t MQTT_UnPacketUnSubscribe(uint8_t *rev_data);

/*--------------------------------�����������--------------------------------*/
uint8_t MQTT_PacketPublish(uint16_t pkt_id, const int8_t *topic,
						const int8_t *payload, uint32_t payload_len,
						enum MqttQosLevel qos, int32_t retain, int32_t own,
						MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------������Ϣ�ظ����--------------------------------*/
uint8_t MQTT_UnPacketPublish(uint8_t *rev_data, int8_t **topic, uint16_t *topic_len, int8_t **payload, uint16_t *payload_len, uint8_t *qos, uint16_t *pkt_id);

/*--------------------------------������Ϣ��Ack���--------------------------------*/
uint1_t MQTT_PacketPublishAck(uint16_t pkt_id, MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------������Ϣ��Ack���--------------------------------*/
uint1_t MQTT_UnPacketPublishAck(uint8_t *rev_data);

/*--------------------------------������Ϣ��Rec���--------------------------------*/
uint1_t MQTT_PacketPublishRec(uint16_t pkt_id, MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------������Ϣ��Rec���--------------------------------*/
uint1_t MQTT_UnPacketPublishRec(uint8_t *rev_data);

/*--------------------------------������Ϣ��Rel���--------------------------------*/
uint1_t MQTT_PacketPublishRel(uint16_t pkt_id, MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------������Ϣ��Rel���--------------------------------*/
uint1_t MQTT_UnPacketPublishRel(uint8_t *rev_data, uint16_t pkt_id);

/*--------------------------------������Ϣ��Comp���--------------------------------*/
uint1_t MQTT_PacketPublishComp(uint16_t pkt_id, MQTT_PACKET_STRUCTURE *mqttPacket);

/*--------------------------------������Ϣ��Comp���--------------------------------*/
uint1_t MQTT_UnPacketPublishComp(uint8_t *rev_data);

/*--------------------------------�����������--------------------------------*/
uint1_t MQTT_PacketPing(MQTT_PACKET_STRUCTURE *mqttPacket);


#endif

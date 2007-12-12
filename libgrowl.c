/*
 * 
 * Library implementing the Growl protocol
 * 
 * Author Antti Holvikari <anttih@gmail.com>
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "libgrowl.h"

int growl_register_app(char *name)
{
	GrowlPacketRegister *reg;
	unsigned char *data;
	
	char *list[] = { "Test0", "Test1" };
	
	reg = (GrowlPacketRegister *) malloc(sizeof(GrowlPacketRegister));
	
	data = growl_create_packet(reg, name, list, 2);
	
	/* send_packet(data);*/
	
	free(reg);
	return 1;
}

unsigned char
*growl_create_packet(GrowlPacketRegister *packet, char *app_name, char *notifications[], int notifications_num)
{
	int i;
	unsigned short len;
	unsigned short app_name_len;
	unsigned char *data;
	
	GrowlPacket gp;
	
	data = gp.data;
	
	packet->ver  = GROWL_PROTOCOL_VERSION;
	packet->type = GROWL_TYPE_REGISTRATION_NOAUTH;
	
	/* this is NULL terminated */
	packet->app_name_len = strlen(packet->app_name) - 1;
	
	memcpy(data, &packet->ver, sizeof(packet->ver));
	data += sizeof(packet->ver);
	
	memcpy(data, &packet->type, sizeof(packet->type));
	data += sizeof(packet->type);
	
	/* network byte-order */
	app_name_len = htons(packet->app_name_len);
	
	memcpy(data, &app_name_len, sizeof(unsigned short));
	data += sizeof(unsigned short);
	
	memcpy(data, packet->app_name, packet->app_name_len);
	data += packet->app_name_len;
	
	for (i = 0; i < notifications_num; i++) {
		len = strlen(notifications[i]);
		data = memcpy(data, &len, sizeof(unsigned short));
		data += sizeof(unsigned short);
		
		data = memcpy(data, notifications[i], len);
		data += sizeof(len);
	}
	
	return (unsigned char *) data;
}

int growl_send_packet(GrowlPacket *packet)
{
	/* actually send the packet */
	return 1;
}
/*
 * 
 * Library implementing the Growl protocol
 * 
 * @link http://growl.info/documentation/developer/protocol.php
 * 
 * Author Antti Holvikari <anttih@gmail.com>
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "libgrowl.h"

int growl_register_app(GrowlPacketRegister *reg)
{
	int bytes_sent;
	GrowlPacket *gp;
	
	gp = (GrowlPacket *) malloc(sizeof(GrowlPacket));
	
	growl_create_register_packet(reg, gp);
	bytes_sent = growl_send_packet(gp);
	
	free(gp->data);
	free(gp);
	
	return bytes_sent;
}

int growl_notify(GrowlPacketNtf *ntf)
{
	int bytes_sent;
	GrowlPacket *gp;
	
	gp = (GrowlPacket *) malloc(sizeof(GrowlPacket));
	
	growl_create_ntf_packet(ntf, gp);
	bytes_sent = growl_send_packet(gp);
	
	free(gp->data);
	free(gp);
	
	return bytes_sent;
}

int growl_create_register_packet(GrowlPacketRegister *packet, GrowlPacket *gp)
{
	int i;
	unsigned short len;
	unsigned short len_htons;
	unsigned short app_name_len;
	unsigned char *data;

	app_name_len = strlen(packet->app_name);
	
	/* this we know for sure */
	gp->len = 6;
	gp->len += app_name_len;
	
	/* Find out how much data to allocate for all notifications */
	for (i = 0; i < packet->notifications_num; i++) {
		gp->len += strlen(packet->notifications[i]);
		gp->len += sizeof(unsigned short);
	}
	
	/* TODO: defaults */
	for (i = 0; i < packet->notifications_num; i++) {
		gp->len += sizeof(unsigned char);
	}
	
	/* allocate for packet data */
	gp->data = (unsigned char *) malloc(gp->len);
	
	data = gp->data;
	
	/* Version */
	memcpy(data, &packet->ver, sizeof(packet->ver));
	data += sizeof(packet->ver);

	/* Type */
	memcpy(data, &packet->type, sizeof(packet->type));
	data += sizeof(packet->type);
	
	len_htons = htons(app_name_len);
	
	/* app name length */
	memcpy(data, &len_htons, sizeof(unsigned short));
	data += sizeof(unsigned short);
	
	/* nall (number of notifications in the list) */
	memcpy(data, (unsigned char *) &packet->notifications_num, sizeof(char));
	data += sizeof(char);
	
	/* ndef (number of notifications enabled by default) */
	memcpy(data, (unsigned char *) &packet->notifications_num, sizeof(char));
	data += sizeof(char);
	
	memcpy(data, packet->app_name, app_name_len);
	data += app_name_len;
	
	/* add each notification */
	for (i = 0; i < packet->notifications_num; i++) {
		len = strlen(packet->notifications[i]);
		len_htons = htons(len);
		
		memcpy(data, &len_htons, sizeof(unsigned short));
		data += sizeof(unsigned short);
		
		memcpy(data, packet->notifications[i], len);
		data += len;
	}
	
	/* defaults */
	for (i = 0; i < packet->notifications_num; i++) {
		memcpy(data, (unsigned char *) &i, sizeof(unsigned char));
		data += sizeof(unsigned char);
	}
	
	return 1;
}

int growl_create_ntf_packet(GrowlPacketNtf *packet, GrowlPacket *gp)
{
	unsigned short len;
	unsigned char *data;
	
	gp->len = sizeof(packet->ver)
			+ sizeof(packet->type)
			+ 2 
			+ sizeof(packet->notification_len)
			+ sizeof(packet->title_len)
			+ sizeof(packet->descr_len)
			+ sizeof(packet->app_name_len);
			
	gp->len += packet->notification_len
			+ packet->title_len
			+ packet->descr_len
			+ packet->app_name_len;
	gp->data = (unsigned char *) malloc(gp->len);
	
	data = gp->data;
	
	/* Version */
	data = (unsigned char *) memcpy(data, &packet->ver, sizeof(packet->ver));
	data += sizeof(packet->ver);

	/* Type */
	data = (unsigned char *) memcpy(data, &packet->type, sizeof(packet->type));
	data += sizeof(packet->type);
	
	/* Flags */
	packet->flags.reserved = 0;
	packet->flags.priority = 0;
	packet->flags.sticky = 0;
	
	data = (unsigned char *) memcpy(data, &packet->flags, 2);
	data += 2;
	
	/* network byte-order */
	len = htons(packet->notification_len);
	memcpy(data, &len, sizeof(unsigned short));
	data += sizeof(unsigned short);
	
	len = htons(packet->title_len);
	memcpy(data, &len, sizeof(unsigned short));
	data += sizeof(unsigned short);
	
	len = htons(packet->descr_len);
	memcpy(data, &len, sizeof(unsigned short));
	data += sizeof(unsigned short);
	
	len = htons(packet->app_name_len);
	memcpy(data, &len, sizeof(unsigned short));
	data += sizeof(unsigned short);
	
	/* actual data */
	memcpy(data, packet->notification, packet->notification_len);
	data += packet->notification_len;
	
	/* title */
	memcpy(data, packet->title, packet->title_len);
	data += packet->title_len;
	
	/* descriptions */
	memcpy(data, packet->descr, packet->descr_len);
	data += packet->descr_len;
	
	/* application name */
	memcpy(data, packet->app_name, packet->app_name_len);
	data += packet->app_name_len;
	
	return 1;
}

int growl_send_packet(GrowlPacket *packet)
{
	int bytes_sent;
	int sockfd;
	socklen_t to_len;
	struct sockaddr_in addr;
	
	/*
	int fromlen;
	void *recv_buf;
	int recv_buf_len = 512;
	*/
	sockfd = socket(PF_INET, SOCK_DGRAM, 0);

	addr.sin_family = AF_INET;                  /* host byte order */
	addr.sin_port = htons(GROWL_DEFAULT_PORT);  /* short, network byte order */
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(addr.sin_zero, 0, sizeof(addr.sin_zero));
	
	to_len = sizeof(struct sockaddr);
	
	/* actually send the packet */
	bytes_sent = sendto(sockfd, packet->data, packet->len, 0, (struct sockaddr *) &addr, to_len);
	
	/*
	if (recvfrom(sockfd, recv_buf, recv_buf_len, 0, (struct sockaddr *) &addr, (unsigned *) &fromlen) < 0) {
		printf("Error reading from remote host\n");
	}
	*/
	return bytes_sent;
}
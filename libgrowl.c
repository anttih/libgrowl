/****************************************************************
 *                                                              *
 * Library implementing the Growl protocol                      *
 *                                                              *
 * @link http://growl.info/documentation/developer/protocol.php *
 *                                                              *
 * Author Antti Holvikari <anttih@gmail.com>                    *
 *                                                              *
 ****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "md5.h"
#include "libgrowl.h"

unsigned char *
growl_create_register_packet(GrowlRegistration *rp, char *notifications[], int notifications_num, char *passwd, unsigned *packet_size)
{
	int i;
	unsigned short len;
	unsigned short len_nbo;
	unsigned short app_name_len;
	unsigned short app_name_len_nbo;
	unsigned char nall;
	unsigned char ndef;
	size_t length;
	unsigned char *data, *packet_data;
	
	app_name_len = strlen(rp->app_name);
	app_name_len_nbo = htons(app_name_len);
	
	nall = notifications_num;
	ndef = notifications_num;
	
	length = sizeof(rp->ver) + sizeof(rp->type) + sizeof(app_name_len_nbo) + sizeof(nall) + sizeof(ndef);
	
	length += app_name_len;
	
	/* Find out how much data to allocate for all notifications */
	for (i = 0; i < nall; i++) {
		length += strlen(notifications[i]);
		length += sizeof(unsigned short);
	}
	
	/* TODO: defaults */
	for (i = 0; i < ndef; i++) {
		length += sizeof(unsigned char);
	}
	
	if (rp->type == GROWL_TYPE_REGISTRATION) {
		/* MD5 used */
		length += 16;
	}
	
	/* allocate for packet data */
	packet_data = (unsigned char *) malloc(length);
	
	data = packet_data;
	
	/* -------------------------------------------------------
	 *
	 * Start with the actual data
	 *
	 */
	
	memcpy(data, &rp->ver, sizeof(rp->ver));
	data += sizeof(rp->ver);
	
	memcpy(data, &rp->type, sizeof(rp->type));
	data += sizeof(rp->type);

	memcpy(data, &app_name_len_nbo, sizeof(app_name_len_nbo));
	data += sizeof(app_name_len_nbo);
	
	memcpy(data, &nall, sizeof(nall));
	data += sizeof(nall);

	memcpy(data, &ndef, sizeof(ndef));
	data += sizeof(ndef);
	
	
	/* application name */
	memcpy(data, rp->app_name, app_name_len);
	data += app_name_len;
	
	/* add each notification */
	for (i = 0; i < nall; i++) {
		len = strlen(notifications[i]);
		len_nbo = htons(len);
		
		memcpy(data, &len_nbo, sizeof(unsigned short));
		data += sizeof(unsigned short);
		
		memcpy(data, notifications[i], len);
		data += len;
	}
	
	/* defaults */
	for (i = 0; i < ndef; i++) {
		memcpy(data, (unsigned char *) &i, sizeof(unsigned char));
		data += sizeof(unsigned char);
	}
	
	/* add checksum if needed */
	if (rp->type == GROWL_TYPE_REGISTRATION) {
		add_checksum(packet_data, length - 16, rp->type, passwd);
	}
	
	*packet_size = length;
	
	/* caller must free data */
	return packet_data;
}

unsigned char *
growl_create_notification_packet(GrowlNotification *ntf, char *passwd, unsigned *packet_size)
{
	/* packet size */
	size_t length;
	
	unsigned short notification_len;
	unsigned short title_len;
	unsigned short descr_len;
	unsigned short app_name_len;
	
	unsigned short notification_len_nbo;
	unsigned short title_len_nbo;
	unsigned short descr_len_nbo;
	unsigned short app_name_len_nbo;
	
	/* packet data */
	unsigned char *data, *packet_data;
	
	/* lengths */
	notification_len = strlen(ntf->notification);
	title_len        = strlen(ntf->title);
	descr_len        = strlen(ntf->descr);
	app_name_len     = strlen(ntf->app_name);
	
	/* lengths in network byte order */
	notification_len_nbo = htons(notification_len);
	title_len_nbo        = htons(title_len);
	descr_len_nbo        = htons(descr_len);
	app_name_len_nbo     = htons(app_name_len);
	
	/* packet size */
	length = sizeof(ntf->ver)
			+ sizeof(ntf->type)
			+ 2 /* two bytes of flags 12 + 3 + 1 bits */
			+ sizeof(notification_len_nbo)
			+ sizeof(title_len_nbo)
			+ sizeof(descr_len_nbo)
			+ sizeof(app_name_len_nbo)
			+ notification_len + title_len + descr_len + app_name_len;
	
	if (ntf->type == GROWL_TYPE_NOTIFICATION) {
		length += 16;
	}
	
	packet_data = (unsigned char *) malloc(length);
	
	data = packet_data;
	
	/* Version */
	memcpy(data, &ntf->ver, sizeof(ntf->ver));
	data += sizeof(ntf->ver);

	/* Type */
	memcpy(data, &ntf->type, sizeof(ntf->type));
	data += sizeof(ntf->type);
	
	/* flags */
	memcpy(data, &ntf->flags, 2);
	data += 2;
	
	/* network byte-order */
	memcpy(data, &notification_len_nbo, sizeof(unsigned short));
	data += sizeof(unsigned short);
	
	memcpy(data, &title_len_nbo, sizeof(unsigned short));
	data += sizeof(unsigned short);
	
	memcpy(data, &descr_len_nbo, sizeof(unsigned short));
	data += sizeof(unsigned short);
	
	memcpy(data, &app_name_len_nbo, sizeof(unsigned short));
	data += sizeof(unsigned short);
	
	
	/* actual data */
	memcpy(data, ntf->notification, notification_len);
	data += notification_len;
	
	/* title */
	memcpy(data, ntf->title, title_len);
	data += title_len;
	
	/* descriptions */
	memcpy(data, ntf->descr, descr_len);
	data += descr_len;
	
	/* application name */
	memcpy(data, ntf->app_name, app_name_len);
	data += app_name_len;
	
	if (ntf->type == GROWL_TYPE_NOTIFICATION) {
		add_checksum(packet_data, length - 16, ntf->type, passwd);
	}
	
	*packet_size = length;
	
	return packet_data;
}

int growl_send_packet(unsigned char *data, unsigned packet_size, char *ip, short port)
{
	int bytes_sent;
	int sockfd;
	socklen_t to_len;
	struct sockaddr_in addr;
	
	sockfd = socket(PF_INET, SOCK_DGRAM, 0);

	addr.sin_family = AF_INET;                  /* host byte order */
	addr.sin_port = htons(port);                /* short, network byte order */
	addr.sin_addr.s_addr = inet_addr(ip);
	memset(addr.sin_zero, 0, sizeof(addr.sin_zero));
	
	to_len = sizeof(struct sockaddr);
	
	/* actually send the packet */
	bytes_sent = sendto(sockfd, data, packet_size, 0, (struct sockaddr *) &addr, to_len);
	
	return bytes_sent;
}

void add_checksum(unsigned char *data, size_t length, enum GrowlAuthMethod type, char *passwd)
{
	unsigned passwd_len;
	unsigned char *chk_data;
	size_t chk_data_len;
	
	passwd_len = strlen(passwd);
	
	/* length of data from which the hash will be taken */
	chk_data_len = length + passwd_len;
	chk_data = (unsigned char *) malloc(chk_data_len);
	
	/* copy packet and password to the checksum data */
	memcpy(chk_data, data, length);
	chk_data += length;
	
	/* write password and reset pointer */
	memcpy(chk_data, passwd, passwd_len);
	chk_data -= length;
	
	if (type <= GROWL_TYPE_NOTIFICATION) {
		MD5_CTX md_context;
		
		MD5Init(&md_context);
		MD5Update(&md_context, chk_data, chk_data_len);
		MD5Final(&md_context);
		
		/* write checksum to the end of the packet */
		memcpy((data + length), &md_context.digest[0], 16);
	}
	
	free(chk_data);
}

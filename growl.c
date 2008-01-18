#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libgrowl.h"

static int bytes = 0;

int register_app(char *name);
int notify(char *notification, char *title, char *descr, char *app_name);

int main(int argc, char *argv[])
{
	register_app(argv[4]);
	/*notify(argv[1], argv[2], argv[3], argv[4]);*/
	
	printf("Sent %d bytes\n", bytes);
	return 1;
}

int register_app(char *name)
{
	char *notifications[] = { "Test1", "Test2" };
	unsigned packet_size;
	GrowlRegistration rp;
	unsigned char *data;
	
	rp.ver      = GROWL_PROTOCOL_VERSION;
	rp.type     = GROWL_TYPE_REGISTRATION;
	rp.app_name = name;
	
	data = growl_create_register_packet(&rp, notifications, 2, "kakka", &packet_size);
	
	bytes = growl_send_packet(data, packet_size, "127.0.0.1", GROWL_DEFAULT_PORT);
	
	free(data);
	
	return 1;
}

int notify(char *notification, char *title, char *descr, char *app_name)
{
	GrowlNotification ntf;
	unsigned packet_size;
	unsigned char *data;
	
	ntf.ver  = 1;
	ntf.type = GROWL_TYPE_NOTIFICATION_NOAUTH;
	
	ntf.notification = notification;
	ntf.title        = title;
	ntf.descr        = descr;
	ntf.app_name     = app_name;
	
	ntf.flags.sticky = 1;
	
	/*
	ntf->flags.reserved = 0;
	ntf->flags.priority = 0;
	*/
	
	data = growl_create_notification_packet(&ntf, &packet_size);
	bytes = growl_send_packet(data, packet_size, "127.0.0.1", GROWL_DEFAULT_PORT);
	
	free(data);
	
	return 1;
}
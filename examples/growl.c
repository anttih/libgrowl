#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libgrowl.h"

#define GROWL_PASSWORD ""
#define GROWL_HOST "127.0.0.1"

static int bytes = 0;

int register_app(const char *name);
int notify(const char *notification, const char *title, const char *descr, const char *app_name);

int main(int argc, char *argv[])
{
	register_app(argv[4]);
	notify(argv[1], argv[2], argv[3], argv[4]);
	
	printf("Sent %d bytes\n", bytes);
	return 1;
}

int register_app(const char *name)
{
	char *notifications[] = { "Test1", "Test2" };
	unsigned packet_size;
	GrowlRegistration rp;
	unsigned char *data;
	
	rp.ver      = GROWL_PROTOCOL_VERSION;
	rp.type     = GROWL_TYPE_REGISTRATION;
	rp.app_name = name;
	
	data = growl_create_register_packet(&rp, notifications, 2, GROWL_PASSWORD, &packet_size);
	
	if ((int)data < 0) {
		fprintf(stderr, "Error creating registration packet\n");
		exit(-1);
	}
	
	bytes = growl_send_packet(data, packet_size, GROWL_HOST, GROWL_DEFAULT_PORT);
	if (bytes < 0) {
		fprintf(stderr, "Error sending registration packet\n");
		free(data);
		exit(-1);
	}
	
	free(data);
	
	return 1;
}

int notify(const char *notification, const char *title, const char *descr, const char *app_name)
{
	GrowlNotification ntf;
	unsigned packet_size;
	unsigned char *data;
	
	ntf.ver  = 1;
	ntf.type = GROWL_TYPE_NOTIFICATION;
	
	ntf.notification = notification;
	ntf.title        = title;
	ntf.descr        = descr;
	ntf.app_name     = app_name;
	ntf.flags.sticky = 0;
	
	/*
	ntf->flags.reserved = 0;
	ntf->flags.priority = 0;
	*/
	
	data = growl_create_notification_packet(&ntf, GROWL_PASSWORD, &packet_size);
	if ((int)data < 0) {
		fprintf(stderr, "Error creating notification packet\n");
		exit(-1);
	}
	
	bytes = growl_send_packet(data, packet_size, GROWL_HOST, GROWL_DEFAULT_PORT);
	if (bytes < 0) {
		fprintf(stderr, "Error sending notification packet\n");
		free(data);
		exit(-1);
	}
	
	free(data);
	
	return 1;
}
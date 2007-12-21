#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libgrowl.h"

int register_app(char *name);
int notify(char *notification, char *title, char *descr, char *app_name);

int main(int argc, char *argv[])
{
	register_app(argv[4]);
	return notify(argv[1], argv[2], argv[3], argv[4]);
}

int register_app(char *name)
{
	GrowlPacketRegister *reg;
	
	reg = (GrowlPacketRegister *) malloc(sizeof(GrowlPacketRegister));
	
	reg->ver  = GROWL_PROTOCOL_VERSION;
	reg->type = GROWL_TYPE_REGISTRATION_NOAUTH;
	
	reg->app_name = name;
	
	reg->notifications[0] = "Test1";
	reg->notifications[1] = "Test2";
	
	reg->notifications_num = 2;
	
	growl_register_app(reg);
	
	free(reg);
	return 1;
}

int notify(char *notification, char *title, char *descr, char *app_name)
{
	GrowlPacketNtf *ntf;
	
	ntf = (GrowlPacketNtf *) malloc(sizeof(GrowlPacketNtf));
	
	ntf->ver  = GROWL_PROTOCOL_VERSION;
	ntf->type = GROWL_TYPE_NOTIFICATION_NOAUTH;
	
	ntf->notification_len = strlen(notification);
	ntf->title_len        = strlen(title);
	ntf->descr_len        = strlen(descr);
	ntf->app_name_len     = strlen(app_name);
	
	ntf->notification = notification;
	ntf->title        = title;
	ntf->descr        = descr;
	ntf->app_name     = app_name;
	
	growl_notify(ntf);
	
	free(ntf);
	
	return 1;
}
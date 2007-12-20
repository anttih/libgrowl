#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libgrowl.h"

int register_app(char *name);

int main(int argc, char *argv[])
{
	return register_app(argv[1]);
}

int register_app(char *name)
{
	GrowlPacketRegister *reg;
	char app_name[] = "libgrowl";
	
	reg = (GrowlPacketRegister *) malloc(sizeof(GrowlPacketRegister));
	
	reg->ver  = GROWL_PROTOCOL_VERSION;
	reg->type = GROWL_TYPE_REGISTRATION_NOAUTH;
	
	reg->app_name_len = (unsigned short) strlen(app_name);
	
	reg->app_name = (char *) malloc(reg->app_name_len + 1);
	strcpy(reg->app_name, app_name);
	
	reg->notifications[0] = "Test1";
	reg->notifications[1] = "Test2";
	
	reg->notifications_num = 2;
	
	growl_register_app(reg);
	
	free(reg->app_name);
	free(reg);
	
	return 1;
}
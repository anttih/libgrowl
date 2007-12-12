#include <stdio.h>
#include "libgrowl.h"

extern GrowlPacketRegister *packet;

void main(int argc, char *argv[])
{
	register(argv[1]);
}

int register(char *name)
{
	GrowlPacketRegister reg;
	
	char *list[] = {
		"Test0", "Test1"
	};
	
	
	
	reg = create_packet(reg, name, list, 2);
	
	send_packet(reg);
}
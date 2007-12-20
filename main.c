
#include <stdio.h>
#include "libgrowl.h"

int main(int argc, char *argv[])
{
	growl_register_app("App name");
	
	return 1;
}
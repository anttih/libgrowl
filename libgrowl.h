
/* protocol versions */
#define GROWL_PROTOCOL_VERSION 1
#define GROWL_PROTOCOL_VERSION_AES128 2

/* packet type */
#define GROWL_TYPE_REGISTRATION 0
#define GROWL_TYPE_NOTIFICATION 1

/* auth? */
#define GROWL_TYPE_REGISTRATION_NOAUTH 4
#define GROWL_TYPE_NOTIFICATION_NOAUTH 5


typedef struct _growl_registration_packet {
	unsigned char ver;
	unsigned char type;
	unsigned short app_name_len;
	unsigned char nall;
	char *md5; /* 16 bytes (md5), 0 (NOAUTH) */
	char *app_name;
	
} GrowlPacketRegister;

typedef struct {
	int len;
	unsigned char *data;
} GrowlPacket;

int register_app(char *);

unsigned char *growl_create_packet(GrowlPacketRegister *, char *, char *[], int);

int growl_send_packet(GrowlPacket *);


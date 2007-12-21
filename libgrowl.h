
/* protocol versions */
#define GROWL_PROTOCOL_VERSION 1
#define GROWL_PROTOCOL_VERSION_AES128 2

/* packet type */
#define GROWL_TYPE_REGISTRATION 0
#define GROWL_TYPE_NOTIFICATION 1

/* auth? */
#define GROWL_TYPE_REGISTRATION_NOAUTH 4
#define GROWL_TYPE_NOTIFICATION_NOAUTH 5


#define MAX_NOTIFICATION_NUM 10
#define GROWL_DEFAULT_PORT 9887

/*
 * Registration packet
 */
typedef struct _growl_registration_packet {
	unsigned char ver;
	unsigned char type;
	unsigned char nall;
	unsigned char ndef;
	
	int notifications_num;
	char *notifications[MAX_NOTIFICATION_NUM];
	
	char *md5; /* 16 bytes (md5), 0 (NOAUTH) */
	char *app_name;
	
} GrowlPacketRegister;


/*
 * Notification packet
 */
typedef struct _growl_notification_packet {
	unsigned char ver;
	unsigned char type;
	
	struct growl_notification_flags {
#ifdef __BIG_ENDIAN__
		unsigned reserved: 12;
		signed   priority: 3;
		unsigned sticky:   1;
#else
		unsigned sticky:   1;
		signed   priority: 3;
		unsigned reserved: 12;
#endif
	} flags;
	
	unsigned short notification_len;
	unsigned short title_len;
	unsigned short descr_len;
	unsigned short app_name_len;
	
	char *notification;
	char *title;
	char *descr;
	char *app_name;
	
	char *md5; /* 16 bytes (md5), 0 (NOAUTH) */
	
} GrowlPacketNtf;

typedef struct {
	int len;
	unsigned char *data;
} GrowlPacket;


int growl_register_app(GrowlPacketRegister *reg);

int growl_notify(GrowlPacketNtf *reg);

int growl_create_register_packet(GrowlPacketRegister *, GrowlPacket *gp);

int growl_create_ntf_packet(GrowlPacketNtf *packet, GrowlPacket *gp);

int growl_send_packet(GrowlPacket *gp);


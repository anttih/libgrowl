
#define MAX_NOTIFICATION_NUM 10
#define GROWL_DEFAULT_PORT 9887

enum GrowlProtocolVersion {
	GROWL_PROTOCOL_VERSION        = 1,
	GROWL_PROTOCOL_VERSION_AES128 = 2
};

enum GrowlAuthMethod {
	GROWL_TYPE_REGISTRATION,         /* MD5 auth */
	GROWL_TYPE_NOTIFICATION,         /* MD5 auth */
	GROWL_TYPE_REGISTRATION_SHA256,
	GROWL_TYPE_NOTIFICATION_SHA256,
	GROWL_TYPE_REGISTRATION_NOAUTH,
	GROWL_TYPE_NOTIFICATION_NOAUTH
};

/*
 * Error codes
 */
#define GROWL_ERR_MALLOC 1
#define GROWL_ERR_SEND   2

#define RETURN_ERR_PTR(err) return ((void *)-err)
#define RETURN_ERR_VAL(err) return (-err)

/*
 * Registration packet
 */
typedef struct growl_registration GrowlRegistration;
	
struct growl_registration {
	unsigned char ver;
	unsigned char type;
	
	const char *app_name;
	
};


/*
 * Notification packet
 */

typedef struct growl_notification GrowlNotification;

struct growl_notification {
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
	
	const char *notification;
	const char *title;
	const char *descr;
	const char *app_name;
	
};


unsigned char *
growl_create_register_packet(GrowlRegistration *gp,
							char *notifications[],
							int notifications_num,
							char *passwd,
							unsigned *packet_size);

unsigned char *
growl_create_notification_packet(GrowlNotification *np, char *passwd, unsigned *packet_size);

void add_checksum(unsigned char *data, size_t length, enum GrowlAuthMethod, char *passwd);

int growl_send_packet(unsigned char *data, unsigned packet_size, char *ip, short port);


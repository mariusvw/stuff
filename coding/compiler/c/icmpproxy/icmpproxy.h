#include <linux/ioctl.h>
#include <strings.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <linux/if.h>
#include <linux/if_tun.h>


#ifdef DEBUG
#define dbg printf
#else
#define dbg //
#endif

/* Maximum packet length, 2^16-20-8-4 */
#define TUNLEN 65503
#define PACKLEN 65536

#define MAX(a,b) ((a>b)?(a):(b))

/* Open TUN device with specified name ("it%d") */
int opentun(char*);

/* Find target for local ip */
int findtarget(int);

/* Set target for local ip */
int settarget(int, int);

/* routing table */
int* targets;



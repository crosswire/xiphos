/*
 * BibleSync library
 * biblesync.hh
 *
 * Karl Kleinpaste, May 2014
 *
 * All files related to implementation of BibleSync, including program
 * source, READMEs, manual pages, and related similar documents, are in
 * the public domain.  As a matter of simple decency, your social
 * obligations are to credit the source and to coordinate any changes you
 * make back to the origin repository.  These obligations are non-
 * binding for public domain software, but they are to be seriously
 * handled nonetheless.
 */

#ifndef __BIBLESYNC_HH__
#define __BIBLESYNC_HH__

// XIPHOS-SPECIFIC - THIS #INCLUDE TO BE REMOVED WHEN
// BIBLESYNC IS EXTRICATED TO AN INDEPENDENT LIBRARY.
#include <glib/gi18n.h>

//
// Bible Sync Protocol.
// http://biblesyncprotocol.wikispaces.com/
//
// BSP provides a classroom type of arrangement, where one person
// (speaker) is in charge of inducing others' (audience) Bible
// software to navigate as speaker requires.  The speaker only
// xmits and the audience only recvs.
// BSP also provides a personal mode which both xmits and recvs, where
// one user works with multiple programs across several devices,
// also suitable for small teams working together, such as translators.
// BSP is implemented using multicast UDP with small packets in a
// simple format employing a few bytes of packet control followed by a
// series of newline-terminated "name=value" pairs.
//
// * Application interface *
//
// - object creation.
//	BibleSync *YourBibleSyncObjectPtr = new BibleSync(app, version, user);
//		create exactly one.
//		identify the application, its version, and the user.
//
// - mode selection.
//	setMode(BSP_MODE_xyz, "passphrase", your_void_nav_func);
//		invoke a mode, including net.setup as needed.
//		xyz = { DISABLE, PERSONAL, SPEAKER, AUDIENCE }.
//		   DISABLE kills it, shuts off network access.
//		   PERSONAL is bidirectional.
//		   SPEAKER xmits only.
//		   AUDIENCE recvs only.
//	=> for any active mode, the application must then start polling using
//	   the receiver, BibleSync::Receive(), and stop polling when mode
//	   goes to DISABLE.  if Receive() is called while disabled, it will
//	   return FALSE to indicate its polled use should stop, otherwise TRUE.
//	=> interface for your_void_nav_func:
//		(char cmd,
//		 string bible, string ref, string alt,
//		 string group, string domain,
//		 string info,  string dump)
//		there are 4 your_void_nav_func() use cases, identified in cmd:
//		1. 'E' (error) for network errors & malformed packets.
//		   only info + dump are useful.
//		2. 'M' (mismatch) against passphrase
//		   info == "announce" or "sync" (+ user@ipaddr)
//			   sync:     bible, ref, alt, group, domain as arrived.
//			   announce: presence message in alt.
//			      also, individual elements are also available:
//			      parameter overload... bible  ref     group  domain
//						    user   ipaddr  app    version
//		   dump available.
//		3. 'A' (announce)
//		   presence message in alt.  dump available.
//		4. 'N' (navigation)
//		   bible, ref, alt, group, domain as arrived.
//		   info + dump available.
//
// - get current mode.
//	BibleSync_mode getMode().
//
// - get current passphrase, for default use when setting a new one.
//	string getPassphrase().
//
// - receive navigation.
//	BibleSync::Receive(YourBibleSyncObjPtr);	// *-* poll often *-*
//		see note below; calls your_void_nav_func().
//
// - send navigation.
//	BibleSync_xmit_status retval =
//		Transmit(BSP_SYNC,
//			 "NASB", "John.3.16", "some alt ref",
//			 "1", "BIBLE-VERSE");
//	  params: type (sync only), bible, ref, alt-ref, group, domain.
//	  all params have defaults.
//	=> it is the application's responsibility to send well-formed verse
//	   references.
//
// - set self as private
//	bool setPrivate(boolean);
//	  sets outgoing TTL to zero so no one hears you off-machine.
//
// Receive() USAGE NOTE:
// the application must call BibleSync::Receive(YourBibleSyncObjPtr)
// frequently.  For example:
//    g_timeout_add(2000,	// 2sec in msec.
//		    (GSourceFunc)BibleSync::Receive,
//		    biblesyncobj);	// of type (BibleSync *).
// g_timeout_add is a glib function for polled function calls.
// this will induce timed-interval calls to the function, as long
// as the function returns TRUE; upon returning FALSE, calls stop.
// other than with glib, accomplish the same thing somehow else.
// Receive is a static method accessible from C or C++.  it must be
// called with the pointer to your BibleSync object in order that
// object context be re-entered.  the internal receive routine
// is private.

using namespace std;

#include <map>
#include <string>

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#ifndef WIN32
#include <sys/utsname.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <uuid/uuid.h>
#else
#define	uuid_t	UUID
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#endif

typedef enum _BibleSync_mode {
    BSP_MODE_DISABLE,
    BSP_MODE_PERSONAL,
    BSP_MODE_SPEAKER,
    BSP_MODE_AUDIENCE,
    N_BSP_MODE
} BibleSync_mode;

typedef enum _BibleSync_xmit_status {
    BSP_XMIT_OK,
    BSP_XMIT_FAILED,
    BSP_XMIT_NO_SOCKET,
    BSP_XMIT_BAD_TYPE,
    BSP_XMIT_NO_AUDIENCE_XMIT,
    BSP_XMIT_RECEIVING,
    N_BSP_XMIT
} BibleSync_xmit_status;

// args: cmd, bible, verse, alt, group, domain, info, dump.
typedef void (*BibleSync_navigate)(char,
				   string, string, string,
				   string, string,
				   string, string);

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifndef min
#define	min(a,b)	(((a) < (b)) ? (a) : (b))
#endif

// message structure constants
#define	BSP_MULTICAST	"239.225.27.227"
#define	BSP_PORT	22272
#define	BSP_MAX_SIZE	1280	// in spec, it's 512.  experimenting.
#define	BSP_RES_SIZE	6	// unused bytes, fill out header to 32.
#define	BSP_HEADER_SIZE	32
#define	BSP_MAX_PAYLOAD	(BSP_MAX_SIZE - BSP_HEADER_SIZE)

// message indications
#define	BSP_MAGIC	htonl(0x409CAF11)
#define	BSP_PROTOCOL	2

// message types
#define	BSP_ANNOUNCE	1
#define	BSP_SYNC	2

// message content names.
#define BSP_APP_NAME			"app.name"		// req'd
#define BSP_APP_VERSION			"app.version"		// opt
#define BSP_APP_INSTANCE_UUID		"app.inst.uuid"		// req'd
#define BSP_APP_OS			"app.os"		// opt
#define BSP_APP_DEVICE			"app.device"		// opt
#define BSP_APP_USER			"app.user"		// req'd
#define BSP_MSG_SYNC_DOMAIN		"msg.sync.domain"	// req'd
#define BSP_MSG_SYNC_VERSE		"msg.sync.verse"	// req'd
#define BSP_MSG_SYNC_ALTVERSE		"msg.sync.altVerse"	// opt
#define BSP_MSG_SYNC_BIBLEABBREV	"msg.sync.bibleAbbrev"	// req'd
#define BSP_MSG_SYNC_GROUP		"msg.sync.group"	// req'd
#define BSP_MSG_PASSPHRASE		"msg.sync.passPhrase"	// req'd

// required number of fields to send (out) or verify (in).
#define	BSP_FIELDS_RECV_ANNOUNCE	4
#define	BSP_FIELDS_RECV_SYNC		8
#define	BSP_FIELDS_XMIT_ANNOUNCE	7
#define	BSP_FIELDS_XMIT_SYNC		12

#ifndef WIN32
#define	BSP_OS	"UNIX/Linux"
#else
#define	BSP_OS	"Windows"
#endif

#define	BSP_UUID_PRINT_LENGTH		37	// actually 36, plus '\0'.

class BibleSync {

private:

    // simple name/value pairs.
    typedef std::map < string, string > BibleSyncContent;

    typedef struct _BibleSyncMessage {
	uint32_t  magic;
	uint8_t   version;
	uint8_t   msg_type;
	uint16_t  num_packets;
	uint16_t  index_packet;
	uint8_t   reserved[BSP_RES_SIZE];
	uuid_t    uuid;
	char      body[BSP_MAX_PAYLOAD+1];	// +1 for stuffing '\0'.
    } BibleSyncMessage;

    // application identifiers.
    string application;
    string version;
    string user;
    string device;

    // currently processing received navigation.
    // prevents use of Transmit.
    bool receiving;

    // what operational mode we're in.
    BibleSync_mode mode;

    // callback by which Receive induces navigation.
    BibleSync_navigate nav_func;

    // privacy
    string passphrase;

    // network access
    struct sockaddr_in server, client;
    int server_fd, client_fd;
    struct ip_mreq multicast_req;

    // default address discoverer, for multicast configuration.
    void InterfaceAddress();
    struct in_addr interface_addr;

    // unique identification.
    uuid_t uuid;
    char uuid_string[BSP_UUID_PRINT_LENGTH];	// printable

    // socket init and listener start, called from setMode().
    string Setup();

    // dispose of network access.
    void Shutdown();

    // real receiver.
    int ReceiveInternal();		// C++ object context.
    int InitSelectRead(char *, struct sockaddr_in *, BibleSyncMessage *);

    // uuid dumper;
    void uuid_dump(uuid_t &u, char *destination);
    char uuid_dump_string[BSP_UUID_PRINT_LENGTH];
    void uuid_gen(uuid_t &u);		// differentiates linux/win32.

#ifndef WIN32
    // network self-analysis, borrowed from the net.
    int get_default_if_name(char *name, socklen_t size);
    int parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo);
    int readNlSock(int sockFd, char *bufPtr, size_t buf_size,
		   unsigned int seqNum, unsigned int pId);
#endif /* !WIN32 */

public:
    BibleSync(string a, string v, string u);
    ~BibleSync();

    // operation.
    BibleSync_mode setMode(BibleSync_mode m,
			   BibleSync_navigate n = NULL,
			   string p = "");
    inline BibleSync_mode getMode(void) { return mode; };

    // obtain passphrase, for default choice.
    inline string getPassphrase(void) { return passphrase; };

    // audience receiver
    static int Receive(void *);	// C context, polled, e.g. from glib timeout

    // speaker transmitter
    BibleSync_xmit_status Transmit(char message_type = BSP_SYNC,
				   string bible  = "KJV",
				   string ref    = "Gen.1.1",
				   string alt    = "",
				   string group  = "1",
				   string domain = "BIBLE-VERSE");

    // set privacy using TTL 0 in personal mode.
    bool setPrivate(bool privacy);
};

#endif // __BIBLESYNC_HH__

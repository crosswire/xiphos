/*
 * BibleSync library
 * biblesync.cc
 *
 * Copyright © 2014 Karl Kleinpaste
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

//
// BibleSync - class implementation.
//

using namespace std;

#include <biblesync.hh>

// sync is a proper superset of announce,
// both inbound as well as outbound.
// in these 2 arrays of strings, sync-specific
// fields are placed after announce fields.
// see field_count in ReceiveInternal and Transmit.
static string inbound_required[] = {
    BSP_APP_NAME,
    BSP_APP_INSTANCE_UUID,
    BSP_APP_USER,
    BSP_MSG_PASSPHRASE,
    BSP_MSG_SYNC_DOMAIN,
    BSP_MSG_SYNC_VERSE,
    BSP_MSG_SYNC_BIBLEABBREV,
    BSP_MSG_SYNC_GROUP
};

static string outbound_fill[] = {
    BSP_APP_NAME,
    BSP_APP_VERSION,
    BSP_APP_INSTANCE_UUID,
    BSP_APP_OS,
    BSP_APP_DEVICE,
    BSP_APP_USER,
    BSP_MSG_PASSPHRASE,
    BSP_MSG_SYNC_DOMAIN,
    BSP_MSG_SYNC_VERSE,
    BSP_MSG_SYNC_ALTVERSE,
    BSP_MSG_SYNC_BIBLEABBREV,
    BSP_MSG_SYNC_GROUP
};

// BibleSync class constructor.
// args identify the user of the class, by application, version, and user.
BibleSync::BibleSync(string a, string v, string u)
    : application(a),
      version(v),
      user(u),
      receiving(false),
      mode(BSP_MODE_DISABLE),
      nav_func(NULL),
      passphrase("BibleSync"),
      server_fd(-1),
      client_fd(-1)
{
    // learn the address to which to assign for multicast.
    InterfaceAddress();

#ifndef WIN32
    // cobble together a description of this machine.
    struct utsname uts;
    uname(&uts);
    device = (string)uts.machine
	+ " running "
	+ uts.sysname
	+ " "
	+ uts.release
	+ " on "
	+ uts.nodename;
#else
    device = "Windows PC";
#endif
}

#define	BSP		(string)"BibleSync: "
#define	EMPTY		(string)""

// BibleSync class destructor.
// kill it all off.
BibleSync::~BibleSync()
{
    nav_func = NULL;
    mode = BSP_MODE_DISABLE;
    if ((client_fd >= 0) || (server_fd >= 0))
	Shutdown();
}

// mode choice and setup invocation.
void BibleSync::setMode(BibleSync_mode m,
			string p,
			BibleSync_navigate n)
{
    if ((mode == BSP_MODE_DISABLE) ||
	((mode != BSP_MODE_DISABLE) &&
	 (n != NULL)))		// oops.
    {
	mode = m;
	passphrase = p;
	nav_func = n;
	if (mode == BSP_MODE_DISABLE)
	    Shutdown();
    }
    else
    {
	mode = BSP_MODE_DISABLE;
	nav_func = NULL;
	Shutdown();
    }

    string result = Setup();
    if (result != "")
    {
	if (nav_func != NULL)
	    (*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
			BSP "network setup errors.", result);
	Shutdown();
    }
}

// network init w/listener start.
string BibleSync::Setup()
{
    string retval = "";
    bool ok_so_far = true;

    if (mode == BSP_MODE_DISABLE)
	Shutdown();

    else
    {
	// prepare both xmitter and recvr, even though one or the other might
	// be not generally in use in classroom setting (viz. announce).

	// in "personal" mode, user is both server and client, because he
	// receives nav from other programs, and sends nav to them, as peers.

	// instructor == "client" insofar as he xmits nav to students.
	if (client_fd < 0)
	{
	    if ((client_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	    {
		ok_so_far = false;
		retval += "client socket";
	    }
	    else
	    {
		// basic xmit socket initialization.
		memset((char *) &client, 0, sizeof(client));
		client.sin_family = AF_INET;
		client.sin_port = htons(BSP_PORT);
		client.sin_addr.s_addr = inet_addr(BSP_MULTICAST);

		// disable listening to our own multicast via loopback.
		char loop=0;			/* ...or 1 enables */
		if (setsockopt(client_fd, IPPROTO_IP, IP_MULTICAST_LOOP,
			       (char *)&loop, sizeof(loop)) < 0)
		{
		    ok_so_far = false;
		    retval += " IP_MULTICAST_LOOP " + loop;
		}
		else
		{
		    // multicast join.
		    if (setsockopt(client_fd, IPPROTO_IP, IP_MULTICAST_IF,
				   (char *)&interface_addr,
				   sizeof(interface_addr)) < 0)
		    {
			ok_so_far = false;
			retval += " IP_MULTICAST_IF " + (int)interface_addr.s_addr;
		    }
		}
		// client is now ready for sendto(2) calls.
	    }
	}

	// student == "server" insofar as he recvs nav from instructor.
	if (ok_so_far && (server_fd < 0))
	{
	    if ((server_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	    {
		ok_so_far = false;
		retval += " server socket";
	    }
	    else
	    {
		int reuse = 1;
		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
			       &reuse, sizeof(reuse)) < 0)
		{
		    ok_so_far = false;
		    retval += " SO_REUSEADDR";
		}

		memset((char *) &server, 0, sizeof(server));
		server.sin_family = AF_INET;
		server.sin_port = htons(BSP_PORT);
		server.sin_addr.s_addr = INADDR_ANY;

		// make it receive-ready.
		if (bind(server_fd, (struct sockaddr*)&server,
			 sizeof(server)) == -1)
		{
		    ok_so_far = false;
		    retval += " bind";
		}

		// multicast join.
		multicast_req.imr_multiaddr.s_addr = inet_addr(BSP_MULTICAST);
		multicast_req.imr_interface.s_addr = interface_addr.s_addr;
		if (setsockopt(server_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,
			       &multicast_req, sizeof(multicast_req)) < 0)
		{
		    ok_so_far = false;
		    retval += " IP_ADD_MEMBERSHIP";
		}
		// bind(2) leaves us ready for recvfrom(2) calls.
	    }
	}

	// identify ourselves uniquely.
	uuid_gen(uuid);
	uuid_dump(uuid, uuid_string);

	// now that we're alive, tell the network world that we're here.
	Transmit(BSP_ANNOUNCE);
    }

    return retval;
}

// disposal of network access.
void BibleSync::Shutdown()
{
    close(server_fd);
    close(client_fd);
    server_fd = client_fd = -1;
}

// local hack version of uuid_generate(),
// which doesn't exist in win32 mingw.
void BibleSync::uuid_gen(uuid_t u)
{
    char *p = (char *)u;
    long int x;

    // srandom(time(NULL));
    srandom((unsigned int)interface_addr.s_addr); // "randomly" driven by address.
    for (int i = 0; i < 4; ++i)
    {
	x = random();
	memcpy(p+(i*4), &x, 4);
    }
}

// conversion of UUID to printable form.
void BibleSync::uuid_dump(uuid_t u, char *destination)
{
    unsigned char *s = (unsigned char *)u;
    snprintf((char *)destination, BSP_UUID_PRINT_LENGTH,
	     "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
	      s[0],  s[1],  s[2],  s[3],  s[4],  s[5],  s[6],  s[7],
	      s[8],  s[9], s[10], s[11], s[12], s[13], s[14], s[15]);
}

// receiver, generally from C.
// to be called with "myself" as userdata.
// this exists so as to be able to dive from object-less C context
// into C++ context with reference to "this", the BibleSync object.
int BibleSync::Receive(void *myself)
{
    // dive back into object context.
    return ((BibleSync *)myself)->ReceiveInternal();
}

// receiver, in C++ object context.
// note that, in expected usage, and per usage by glib's g_timeout_add(),
// return TRUE means the function is prepared to be called again,
// and return FALSE means the function calls should stop until something
// changes.  we return FALSE only when mode has been set to BSP_MODE_DISABLE.
// ==> THEREFORE...
// whenever the application sets any enabled mode, it must also arrange for
// Receive() to begin being called regularly.

#define	DEBUG_LENGTH	(4*BSP_MAX_SIZE)	// print is bigger than raw

int BibleSync::ReceiveInternal()
{
    if (mode == BSP_MODE_DISABLE)
	return FALSE;				// done: un-schedule polling.

    // nav_func unset => no point trying; no network => just leave.
    if ((nav_func == NULL) || (server_fd < 0))
	return TRUE;
    
    char dump[DEBUG_LENGTH];
    struct sockaddr_in source;
    BibleSyncMessage bsp;
    int recv_size = 0;

    // anything non-empty here is at least legitimate network traffic.
    // whether it passes muster for BibleSync is another matter.
    while ((recv_size = InitSelectRead(dump, &source, &bsp)) > 0)
    {
	((char*)&bsp)[recv_size] = '\0';	// as an ordinary C string

	// dump content into something humanly useful.
	uuid_dump(bsp.uuid, uuid_dump_string);
	snprintf(dump, DEBUG_LENGTH-1,
		 "magic: 0x%08x\nversion: 0x%02x\ntype: 0x%02x (%s)\n"
		 "uuid: %s\n#pkt: %d\npkt index: %d\n\n-*- body -*-\n%s",
		 bsp.magic, bsp.version,
		 bsp.msg_type, ((bsp.msg_type == BSP_ANNOUNCE)
				   ? "announce"
				   : ((bsp.msg_type == BSP_SYNC)
				      ? "sync"
				      : "*???*")),
		 uuid_dump_string,
		 bsp.num_packets, bsp.index_packet,
		 bsp.body);

	// validate message: fixed values.
	if (bsp.magic != BSP_MAGIC)
	    (*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
			BSP "bad magic", (string)dump);
	else if (bsp.version != BSP_PROTOCOL)
	    (*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
			BSP "bad protocol version", (string)dump);
	else if ((bsp.msg_type != BSP_ANNOUNCE) &&
		 (bsp.msg_type != BSP_SYNC))
	    (*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
			BSP "bad msg type", (string)dump);
	else if (bsp.num_packets != 1)
	    (*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
			BSP "bad packet count", (string)dump);
	else if (bsp.index_packet != 0)
	    (*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
			BSP "bad packet index", (string)dump);

	// basic header sanity tests passed.  now parse body content.
	else
	{
	    bool ok_so_far(true);
	    char *name, *value;
	    BibleSyncContent content;

	    // structure test and content retrieval
	    // "name=value\n" for each.
	    for (char *s = bsp.body; ok_so_far && *s; ++s)
	    {
		name = s;
		// newline terminator of name/value pair.
		if ((s = strchr(s, '\n')) == NULL)
		{
		    ok_so_far = false;
		    break;
		}
		else
		{
		    *s = '\0';	// NUL-terminate assignment (end of value).

		    // separator ('=') between name and value.
		    if ((value = strchr(name, '=')) == NULL)
		    {
			ok_so_far = false;
			break;
		    }
		    else
		    {
			*(value++) = '\0';	// NUL-terminate name.

			// valid content.
			content[name] = value;
		    }
		}
	    }

	    if (!ok_so_far)
	    {
		(*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
			    BSP "bad body format", (string)dump);
	    }
	    else
	    {
		// verify minimum body content.
		int field_count = ((bsp.msg_type == BSP_ANNOUNCE)
				   ? BSP_FIELDS_RECV_ANNOUNCE
				   : BSP_FIELDS_RECV_SYNC);

		for (int i = 0; i < field_count; ++i)
		{
		    if (content.find(inbound_required[i]) == content.end())
		    {
			ok_so_far = false;
			string info = BSP "missing required header "
			    + inbound_required[i]
			    + ".";
			(*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
				    (char *)info.c_str(), (string)dump);
			// don't break -- find all missing.
		    }
		}

		if (ok_so_far)
		{
		    // give reference items initial filler content.
		    string bible = "<>", ref = "<>", alt = "<>",
			group = "<>", domain = "<>",
			info = "<>";
		    char cmd;

		    // generally good, so extract interesting content.
		    if (bsp.msg_type == BSP_SYNC)
		    {
			// regular synchronized navigation
			bible  = content.find(BSP_MSG_SYNC_BIBLEABBREV)->second;
			ref    = content.find(BSP_MSG_SYNC_VERSE)->second;
			alt    = content.find(BSP_MSG_SYNC_ALTVERSE)->second;
			group  = content.find(BSP_MSG_SYNC_GROUP)->second;
			domain = content.find(BSP_MSG_SYNC_DOMAIN)->second;

			if ((passphrase ==
			     content.find(BSP_MSG_PASSPHRASE)->second) &&
			    ((mode == BSP_MODE_PERSONAL) ||
			     (mode == BSP_MODE_STUDENT)))
			    // instructor accepts no navigation.
			{
			    cmd = 'N';	// navigation
			}
			else
			{
			    cmd = 'M';	// mismatch
			    info = "navigation";
			}
		    }
		    else
		    {
			// construct user's presence announcement
			string version = content.find(BSP_APP_VERSION)->second;

			alt = BSP
			    + content.find(BSP_APP_USER)->second
			    + " present at ["
			    + (string)inet_ntoa(source.sin_addr)
			    + "], using "
			    + content.find(BSP_APP_NAME)->second
			    + " "
			    + ((version != "") ? version : (string)"(version?)")
			    + ".";
			if (passphrase ==
			    content.find(BSP_MSG_PASSPHRASE)->second)
			{
			    cmd = 'A';	// presence announcement
			}
			else
			{
			    cmd = 'M';	// mismatch
			    info = "announce";
			}
		    }

		    // delivery to application.
		    receiving = true;			// re-xmit lock.
		    (*nav_func)(cmd,
				bible, ref, alt, group, domain,
				info, (string)dump);
		    receiving = false;			// re-xmit unlock.
		}
	    }
	}

    }

    return TRUE;
}

// network read access.
// do full initialization, no-wait select, and no-wait recvfrom
// to get potential nav data.  returns size acquired.
// controls 'while' in ReceiveInternal().
int BibleSync::InitSelectRead(char *dump,
			      struct sockaddr_in *source,
			      BibleSyncMessage *buffer)
{
    struct timeval tv = { 0, 0 };	// select returns immediately
    fd_set read_set;
    int recv_size = 0;
    unsigned int source_length = sizeof(*source);

    strcpy(dump, "[no dump ready]");	// initial, pre-read filler

    FD_ZERO(&read_set);
    FD_SET(server_fd, &read_set);
    if (select(server_fd+1, &read_set, NULL, NULL, &tv) < 0)
    {
	(*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
		    BSP "select < 0", (string)dump);
	return -1;
    }

    if ((FD_ISSET(server_fd, &read_set)) &&
	(recv_size = (recvfrom(server_fd, (char *)buffer, BSP_MAX_SIZE,
			       MSG_DONTWAIT, (sockaddr *)source,
			       &source_length) < 0)))
    {
	(*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
		    BSP "recvfrom < 0", (string)dump);
	return -1;
    }
    return recv_size;
}

// instructor transmitter
// sanity checks for permission to xmit,
// then format and ship it.
BibleSync_xmit_status BibleSync::Transmit(char message_type,
					  string bible,
					  string ref,
					  string alt,
					  string group,
					  string domain)
{
    if (receiving)
	return BSP_XMIT_RECEIVING;	// if this occurs, app re-xmit'd. *NO*.
    
    if (client_fd < 0)
	return BSP_XMIT_NO_SOCKET;

    if ((message_type != BSP_ANNOUNCE) && (message_type != BSP_SYNC))
	return BSP_XMIT_BAD_TYPE;
    
    if ((mode == BSP_MODE_STUDENT) && (message_type == BSP_SYNC))
	return BSP_XMIT_NO_STUDENT_XMIT;
    
    BibleSyncContent content;
    BibleSyncMessage bsp;
    string body = "";

    // all name/value pairs.
    content[BSP_APP_NAME]             = application;
    content[BSP_APP_VERSION]          = version;
    content[BSP_APP_USER]             = user;
    content[BSP_APP_DEVICE]           = device;
    content[BSP_APP_OS]               = BSP_OS;
    content[BSP_APP_INSTANCE_UUID]    = uuid_string;
    content[BSP_MSG_SYNC_BIBLEABBREV] = bible;
    content[BSP_MSG_SYNC_VERSE]       = ref;
    content[BSP_MSG_SYNC_ALTVERSE]    = alt;
    content[BSP_MSG_SYNC_GROUP]       = group;
    content[BSP_MSG_SYNC_DOMAIN]      = domain;
    content[BSP_MSG_PASSPHRASE]       = passphrase;

    // header.
    bsp.magic = BSP_MAGIC;
    bsp.version = BSP_PROTOCOL;
    bsp.msg_type = message_type;
    bsp.num_packets = 1;
    bsp.index_packet = 0;
    memcpy((void *)&bsp.uuid, (const void *)uuid, sizeof(uuid_t));
    for (int i = 0; i < BSP_RES_SIZE; ++i)
    {
	bsp.reserved[i] = '\0';	// or 0xff?
    }

    // body prep.
    int field_count = ((message_type == BSP_ANNOUNCE)
		       ? BSP_FIELDS_XMIT_ANNOUNCE
		       : BSP_FIELDS_XMIT_SYNC);

    for (int i = 0; i < field_count; ++i)
    {
	body += outbound_fill[i] + "=" + content[outbound_fill[i]] + "\n";
    }

    // ship it.
    strncpy(bsp.body, body.c_str(), BSP_MAX_PAYLOAD);
    unsigned int xmit_size = min(BSP_MAX_SIZE,
				 BSP_HEADER_SIZE + body.length());

#if 0
    char dump[DEBUG_LENGTH];
    uuid_dump(bsp.uuid, uuid_dump_string);
    snprintf(dump, DEBUG_LENGTH-1,
	     "magic: 0x%08x\nversion: 0x%02x\ntype: 0x%02x (%s)\n"
	     "uuid: %s\n#pkt: %d\npkt index: %d\n\n-*- body -*-\n%s",
	     bsp.magic, bsp.version,
	     bsp.msg_type, ((bsp.msg_type == BSP_ANNOUNCE)
			    ? "announce"
			    : ((bsp.msg_type == BSP_SYNC)
			       ? "sync"
			       : "*???*")),
	     uuid_dump_string,
	     bsp.num_packets, bsp.index_packet,
	     bsp.body);
    (*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
		BSP "TRANSMIT:", dump);
#endif /* 0 */

    return ((sendto(client_fd, (char *)&bsp, xmit_size, 0,
	       (struct sockaddr *)&client, sizeof(client)) >= 0)
	    ? BSP_XMIT_OK
	    : BSP_XMIT_FAILED);
}

// routines below imported from the net as workable examples.
// in order to do multicast setup, we require the address
// of the interface that has our default route.  code below
// finds the name of that interface.  then getifaddrs(3)
// code (taken from its man page) lets us match that name
// against an entry that has the address we need.

// extra includes needed only for the
// route- & gateway-discovery code below.

#include <sys/socket.h>
#include <net/if.h>
#include <linux/rtnetlink.h>
#include <unistd.h>
#include <arpa/inet.h>

struct route_info
{
    struct in_addr dstAddr;
    struct in_addr srcAddr;
    struct in_addr gateWay;
    char ifName[IF_NAMESIZE];
};

int BibleSync::readNlSock(int sockFd,
			  char *bufPtr,
			  size_t buf_size,
			  unsigned int seqNum,
			  unsigned int pId)
{
#ifndef WIN32
    struct nlmsghdr *nlHdr;
    int readLen = 0, msgLen = 0;

    do
    {
	if((readLen = recv(sockFd, bufPtr, buf_size - msgLen, 0)) < 0)
	{
	    perror("SOCK READ: ");
	    return -1;
	}

	nlHdr = (struct nlmsghdr *)bufPtr;

	if((NLMSG_OK(nlHdr, readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR))
	{
	    perror("Error in recieved packet");
	    return -1;
	}

	/* check if last message */
	if(nlHdr->nlmsg_type == NLMSG_DONE)
	{
	    break;
	}
	else
	{
	    /* else move pointer to buffer appropriately */
	    bufPtr += readLen;
	    msgLen += readLen;
	}

	/* check if multi part message */
	if((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0)
	{
	    /* return if its not */
	    break;
	}
    }
    while((nlHdr->nlmsg_seq != seqNum) || (nlHdr->nlmsg_pid != pId));

    return msgLen;
#else
    return 0;
#endif
}

int BibleSync::parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo)
{
#ifndef WIN32
    struct rtmsg *rtMsg;
    struct rtattr *rtAttr;
    int rtLen;

    rtMsg = (struct rtmsg *)NLMSG_DATA(nlHdr);

    /* if route is not AF_INET or not in main table, return. */
    if((rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN))
	return -1;

    rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
    rtLen = RTM_PAYLOAD(nlHdr);

    for(; RTA_OK(rtAttr,rtLen); rtAttr = RTA_NEXT(rtAttr,rtLen))
    {
	switch(rtAttr->rta_type)
	{
	case RTA_OIF:
	    if_indextoname(*(int *)RTA_DATA(rtAttr), rtInfo->ifName);
	    break;

	case RTA_GATEWAY:
	    memcpy(&rtInfo->gateWay, RTA_DATA(rtAttr), sizeof(rtInfo->gateWay));
	    break;

	case RTA_PREFSRC:
	    memcpy(&rtInfo->srcAddr, RTA_DATA(rtAttr), sizeof(rtInfo->srcAddr));
	    break;

	case RTA_DST:
	    memcpy(&rtInfo->dstAddr, RTA_DATA(rtAttr), sizeof(rtInfo->dstAddr));
	    break;
	}
    }

#endif
    return 0;
}

int BibleSync::get_default_if_name(char *name, socklen_t size)
{
#ifndef WIN32
    int found_default = 0;

    struct nlmsghdr *nlMsg;
    //struct rtmsg *rtMsg;
    struct route_info route_info;
    char msgBuf[4096];

    int sock, len, msgSeq = 0;

    name[1] = '\0';	// pre-set, in case of error.

    if((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0)
    {
	perror("socket creation: ");
	name[0] = 'x';
	return -1;
    }

    memset(msgBuf, 0, sizeof(msgBuf));

    nlMsg = (struct nlmsghdr *)msgBuf;
    //rtMsg = (struct rtmsg *)NLMSG_DATA(nlMsg);

    nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    nlMsg->nlmsg_type = RTM_GETROUTE;

    nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST;
    nlMsg->nlmsg_seq = msgSeq++;
    nlMsg->nlmsg_pid = getpid();

    if(send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0)
    {
	perror("write to socket failed: ");
	name[0] = 'y';
	return -1;
    }

    if((len = readNlSock(sock, msgBuf, sizeof(msgBuf), msgSeq, getpid())) < 0)
    {
	perror("read from socket failed: ");
	name[0] = 'z';
	return -1;
    }

    for(; NLMSG_OK(nlMsg,len); nlMsg = NLMSG_NEXT(nlMsg,len))
    {
	memset(&route_info, 0, sizeof(route_info));
	if ( parseRoutes(nlMsg, &route_info) < 0 )
	    continue;			// don't check: not set up

	if (strstr((char *)inet_ntoa(route_info.dstAddr), "0.0.0.0"))
	{
	    // copy it over
	    strcpy(name, route_info.ifName);
	    found_default = 1;
	    break;
	}
    }

    close(sock);
    return found_default;
#else
    return 0;
#endif
}

#include <netdb.h>
#include <ifaddrs.h>

void BibleSync::InterfaceAddress()
{
#ifndef WIN32
    char gw_if[IF_NAMESIZE];	// default gateway interface.

    (void)get_default_if_name(gw_if, 100);

    // if no error, search the interface list for that address.
    if (gw_if[1] != '\0')
    {
	struct ifaddrs *ifaddr, *ifa;

	if (getifaddrs(&ifaddr) == -1) {
	    perror("getifaddrs");
	    return;
	}

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
	    if (ifa->ifa_addr == NULL)
		continue;

	    if ((ifa->ifa_addr->sa_family == AF_INET) &&
		(strcmp(gw_if, ifa->ifa_name) == 0)) {
		interface_addr.s_addr =
		    ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr.s_addr;
		break;
	    }
	}
	freeifaddrs(ifaddr);
    }
#else
    interface_addr = htonl(0xff000001);	// 127.0.0.1
#endif /* WIN32 */
    return;
}

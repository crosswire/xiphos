/*
 * BibleSync library
 * biblesync.cc
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
    BSP_MSG_SYNC_GROUP,
    BSP_MSG_SYNC_BIBLEABBREV,
    BSP_MSG_SYNC_ALTVERSE,
    BSP_MSG_SYNC_VERSE		// last: could go overly long, risk cutoff.
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
#ifndef WIN32
    // cobble together a description of this machine.
    struct utsname uts;
    uname(&uts);
    device = (string)uts.machine
	+ ": "
	+ uts.sysname
	+ " @ "
	+ uts.nodename;
#else
    device = "Windows PC";
#endif

    interface_addr.s_addr = htonl(0x7f000001);	// 127.0.0.1
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
BibleSync_mode BibleSync::setMode(BibleSync_mode m,
				  BibleSync_navigate n,
				  string p)
{
    if ((mode == BSP_MODE_DISABLE) ||
	((mode != BSP_MODE_DISABLE) &&
	 (n != NULL)))		// oops.
    {
	mode = m;
	if (p != "")
	{
	    passphrase = p;			// use existing.
	}
	nav_func = n;
	if (mode == BSP_MODE_DISABLE)
	    Shutdown();
    }
    else
    {
	Shutdown();
    }

    string result = Setup();
    if (result != "")
    {
	if (nav_func != NULL)
	    (*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
			BSP + _("network setup errors."), result);
	Shutdown();
    }
    return mode;
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
	// learn the address to which to assign for multicast.
	InterfaceAddress();

	// prepare both xmitter and recvr, even though one or the other might
	// be not generally in use in classroom setting (viz. announce).

	// in "personal" mode, user is both server and client, because he
	// receives nav from other programs, and sends nav to them, as peers.

	// speaker == "client" insofar as he xmits nav to audience.
	if (client_fd < 0)
	{
	    if ((client_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	    {
		ok_so_far = false;
		retval += _(" client socket");
	    }
	    else
	    {
		// basic xmit socket initialization.
		memset((char *) &client, 0, sizeof(client));
		client.sin_family = AF_INET;
		client.sin_port = htons(BSP_PORT);
		client.sin_addr.s_addr = inet_addr(BSP_MULTICAST);

		// enable listening to our own multicast via loopback.
		char loop=1;
		if (setsockopt(client_fd, IPPROTO_IP, IP_MULTICAST_LOOP,
			       (char *)&loop, sizeof(loop)) < 0)
		{
		    ok_so_far = false;
		    retval += " IP_MULTICAST_LOOP";
		}
		else
		{
		    // multicast join.
		    if (setsockopt(client_fd, IPPROTO_IP, IP_MULTICAST_IF,
				   (char *)&interface_addr,
				   sizeof(interface_addr)) < 0)
		    {
			ok_so_far = false;
			retval += (string)" IP_MULTICAST_IF ["
			    + inet_ntoa(interface_addr)
			    + "]";
		    }
		}
		// client is now ready for sendto(2) calls.
	    }
	}

	// one way or another, if we got this far with a valid socket,
	// and the app is in a public mode, "TTL 0" privacy makes no sense.
	if ((client_fd >= 0) &&
	    ((mode == BSP_MODE_SPEAKER) ||
	     (mode == BSP_MODE_AUDIENCE)))
	{
	    setPrivate(false);
	}

	// audience == "server" insofar as he recvs nav from speaker.
	if (ok_so_far && (server_fd < 0))
	{
	    if ((server_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	    {
		ok_so_far = false;
		retval += _(" server socket");
	    }
	    else
	    {
		int reuse = 1;
		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
			       (char *)&reuse, sizeof(reuse)) < 0)
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
			       (char *)&multicast_req, sizeof(multicast_req))
		    < 0)
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
	if (retval == "")
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
    mode = BSP_MODE_DISABLE;
    nav_func = NULL;
}

// pick the OS' generation flavor.
void BibleSync::uuid_gen(uuid_t &u)
{
#ifndef WIN32
    uuid_generate(u);
#else
    UuidCreate(&u);
#endif /* WIN32 */
}

// conversion of UUID to printable form.
void BibleSync::uuid_dump(uuid_t &u, char *destination)
{
    unsigned char *s = (unsigned char *)&u;
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
		 "[%s]\nmagic: 0x%08x\nversion: 0x%02x\ntype: 0x%02x (%s)\n"
		 "uuid: %s\n#pkt: %d\npkt index: %d\n\n-*- body -*-\n%s",
		 inet_ntoa(source.sin_addr),
		 ntohl(bsp.magic), bsp.version,
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
			BSP + _("bad magic"), (string)dump);
	else if (bsp.version != BSP_PROTOCOL)
	    (*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
			BSP + _("bad protocol version"), (string)dump);
	else if ((bsp.msg_type != BSP_ANNOUNCE) &&
		 (bsp.msg_type != BSP_SYNC))
	    (*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
			BSP + _("bad msg type"), (string)dump);
	else if (bsp.num_packets != 1)
	    (*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
			BSP + _("bad packet count"), (string)dump);
	else if (bsp.index_packet != 0)
	    (*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
			BSP + _("bad packet index"), (string)dump);

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
			    BSP + _("bad body format"), (string)dump);
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
			string info = BSP + _("missing required header ")
			    + inbound_required[i]
			    + ".";
			(*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
				    (char *)info.c_str(), (string)dump);
			// don't break -- find all missing.
		    }
		}

		if (ok_so_far)
		{
		    // loopback is enabled: reject self-uuid packets.
		    unsigned int i;
		    unsigned char *incoming = (unsigned char *)&uuid;
		    unsigned char *mine     = (unsigned char *)&bsp.uuid;
		    for (i = 0; i < sizeof(uuid_t); ++i)
		    {
			if (incoming[i] != mine[i])
			    break;	// not ourselves.
		    }
		    // if we end the loop without early break,
		    // then we matched UUID for ourselves.
		    // i.e. we're hearing an echo of ourselves.  ignore.
		    if (i == sizeof(uuid_t))
		    {
			(*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
				    BSP + _("Ignoring echo."), (string)dump);
			continue;
		    }
		    
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

			if (domain != "BIBLE-VERSE")
			{
			    cmd = 'E';
			    info = BSP
				+ _("Domain not 'BIBLE-VERSE': ")
				+ domain;
			} else if ((group.length() != 1) ||
				   (group.c_str()[0] < '1') ||
				   (group.c_str()[0] > '9'))
			{
			    cmd = 'E';
			    info = BSP
				+ _("Invalid group: ")
				+ group;
			}
			else if ((passphrase ==
			     content.find(BSP_MSG_PASSPHRASE)->second) &&
			    ((mode == BSP_MODE_PERSONAL) ||
			     (mode == BSP_MODE_AUDIENCE)))
			    // speaker accepts no navigation.
			{
			    cmd = 'N';	// navigation
			}
			else
			{
			    cmd = 'M';	// mismatch
			    info = (string)"sync: "
				+ content.find(BSP_APP_USER)->second
				+ " @ ["
				+ inet_ntoa(source.sin_addr)
				+ "]";
			}
		    }
		    else
		    {
			// construct user's presence announcement
			string version = content.find(BSP_APP_VERSION)->second;

			alt = BSP
			    + content.find(BSP_APP_USER)->second
			    + _("\npresent at [")
			    + (string)inet_ntoa(source.sin_addr)
			    + _("]\nusing ")
			    + content.find(BSP_APP_NAME)->second
			    + " "
			    + ((version != "") ? version : (string)"(version?)")
			    + ".";
			bible  = content.find(BSP_APP_USER)->second;
			ref    = (string)"[" + inet_ntoa(source.sin_addr) + "]";
			group  = content.find(BSP_APP_NAME)->second;
			domain = ((version != "") ? version : (string)"(version?)");
			if (passphrase ==
			    content.find(BSP_MSG_PASSPHRASE)->second)
			{
			    cmd = 'A';	// presence announcement
			}
			else
			{
			    cmd = 'M';	// mismatch
			    info = (string)"announce: "
				+ content.find(BSP_APP_USER)->second
				+ " @ ["
				+ inet_ntoa(source.sin_addr)
				+ "]";
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
#ifndef WIN32
    // yes, really:
    // linux insists on unsigned int, win32 insists on int.
    // each complains bitterly of invalid conversion if wrongly used.
    unsigned
#endif
    int source_length = sizeof(*source);

    strcpy(dump, _("[no dump ready]"));	// initial, pre-read filler

    FD_ZERO(&read_set);
    FD_SET(server_fd, &read_set);
    if (select(server_fd+1, &read_set, NULL, NULL, &tv) < 0)
    {
	(*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
		    BSP + _("select < 0"), (string)dump);
	return -1;
    }

    if ((FD_ISSET(server_fd, &read_set)) &&
	((recv_size = recvfrom(server_fd, (char *)buffer, BSP_MAX_SIZE,
			       0, (sockaddr *)source,
			       &source_length)) < 0))
    {
	(*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
		    BSP + _("recvfrom < 0"), (string)dump);
	return -1;
    }
    return recv_size;
}

// speaker transmitter
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
    
    if ((mode == BSP_MODE_AUDIENCE) && (message_type == BSP_SYNC))
	return BSP_XMIT_NO_AUDIENCE_XMIT;
    
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
    memcpy((void *)&bsp.uuid, (const void *)&uuid, sizeof(uuid_t));
    memset((void *)&bsp.reserved, 0, BSP_RES_SIZE);

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
	     ntohl(bsp.magic), bsp.version,
	     bsp.msg_type, ((bsp.msg_type == BSP_ANNOUNCE)
			    ? "announce"
			    : ((bsp.msg_type == BSP_SYNC)
			       ? "sync"
			       : "*???*")),
	     uuid_dump_string,
	     bsp.num_packets, bsp.index_packet,
	     bsp.body);
    (*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
		BSP + _("TRANSMIT:"), dump);
#endif /* 0 */

    // force last == newline: attempt to preserve body format when long.
    ((unsigned char*)&bsp)[BSP_MAX_SIZE-1] = '\n';

    BibleSync_xmit_status retval;
    if (sendto(client_fd, (char *)&bsp, xmit_size, 0,
	       (struct sockaddr *)&client, sizeof(client)) >= 0)
    {
	retval = BSP_XMIT_OK;
    }
    else
    {
	retval = BSP_XMIT_FAILED;
	(*nav_func)('E', EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
		    BSP + _("Transmit failed.\n"),
		    _("Unable to multicast; BibleSync is now disabled.\n"
		      "If your network connection changed while this program\n"
		      "was active, it may be sufficient to re-enable."));
	Shutdown();
    }
    return retval;
}

//
// privacy setting.
// only when in personal mode, allow setting TTL 0
// so that packets do not leave this box.  only programs
// running right here will hear.
//
bool BibleSync::setPrivate(bool privacy)
{
    if (mode != BSP_MODE_PERSONAL)
	privacy = false;		// regardless of caller intent.

    int ttl = (privacy ? 0 : 1);

    return (setsockopt(client_fd, IPPROTO_IP, IP_MULTICAST_TTL,
		       (char *)&ttl, sizeof(ttl)) >= 0);
}


#ifndef WIN32

// routines below imported from the net as workable examples.
// in order to do multicast setup, we require the address
// of the interface that has our default route.  code below
// finds the name of that interface.  then getifaddrs(3)
// code (taken from its man page) lets us match that name
// against an entry that has the address we need.

// extra includes needed only for the
// route & gateway discovery code below.

#include <net/if.h>
#include <linux/rtnetlink.h>

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
    struct nlmsghdr *nlHdr;
    int readLen = 0, msgLen = 0;

    do
    {
	if((readLen = recv(sockFd, bufPtr, buf_size - msgLen, 0)) < 0)
	{
	    return -1;
	}

	nlHdr = (struct nlmsghdr *)bufPtr;

	if((NLMSG_OK(nlHdr, readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR))
	{
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
}

int BibleSync::parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo)
{
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

    return 0;
}

int BibleSync::get_default_if_name(char *name, socklen_t size)
{
    int found_default = 0;

    struct nlmsghdr *nlMsg;
    //struct rtmsg *rtMsg;
    struct route_info route_info;
    char msgBuf[4096];

    int sock, len, msgSeq = 0;

    name[1] = '\0';	// pre-set, in case of error.

    if((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0)
    {
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
	name[0] = 'y';
	return -1;
    }

    if((len = readNlSock(sock, msgBuf, sizeof(msgBuf), msgSeq, getpid())) < 0)
    {
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
}

#include <netdb.h>
#include <ifaddrs.h>

void BibleSync::InterfaceAddress()
{
    // cancel any old interface value.
    // we must fail with current info, if at all.
    interface_addr.s_addr = htonl(0x7f000001);	// 127.0.0.1 fallback

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
    return;
}

#else	/* WIN32 */

void BibleSync::InterfaceAddress()
{
    // cancel any old interface value.
    // we must fail with current info, if at all.
    interface_addr.s_addr = htonl(0x7f000001);	// 127.0.0.1 fallback

    // this code is rudely derived from, and courtesy of,
    // http://tangentsoft.net/wskfaq/examples/getifaces.html
    // to whom we are grateful.

    // this is more simplistic than the linux/unix case.
    // here, we simply find a functioning multicast-capable interface.

    WSADATA WinsockData;
    if (WSAStartup(MAKEWORD(2, 2), &WinsockData) != 0) {
        return;
    }

    SOCKET sd = WSASocket(AF_INET, SOCK_DGRAM, 0, 0, 0, 0);
    if (sd == SOCKET_ERROR) {
	return;
    }

    INTERFACE_INFO InterfaceList[20];
    unsigned long nBytesReturned;
    if (WSAIoctl(sd, SIO_GET_INTERFACE_LIST, 0, 0, &InterfaceList,
			sizeof(InterfaceList), &nBytesReturned, 0, 0)
	== SOCKET_ERROR) {
	return;
    }

    int nNumInterfaces = nBytesReturned / sizeof(INTERFACE_INFO);
    for (int i = 0; i < nNumInterfaces; ++i) {

        u_long nFlags = InterfaceList[i].iiFlags;

        if ((nFlags & IFF_UP)		&&	// alive.
	    !(nFlags & IFF_LOOPBACK)	&&	// not local.
	    (nFlags & IFF_MULTICAST))		// multicast-capable.
	{
	    sockaddr_in *pAddress;
	    pAddress = (sockaddr_in *) & (InterfaceList[i].iiAddress);

	    interface_addr.s_addr = pAddress->sin_addr.s_addr;
	    break;
	}
    }
    return;
}
#endif /* WIN32 */

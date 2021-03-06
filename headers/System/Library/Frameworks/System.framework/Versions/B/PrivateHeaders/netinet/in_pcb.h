/*
 * Copyright (c) 2000-2011 Apple Inc. All rights reserved.
 *
 * @APPLE_OSREFERENCE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. The rights granted to you under the License
 * may not be used to create, or enable the creation or redistribution of,
 * unlawful or unlicensed copies of an Apple operating system, or to
 * circumvent, violate, or enable the circumvention or violation of, any
 * terms of an Apple operating system software license agreement.
 * 
 * Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_OSREFERENCE_LICENSE_HEADER_END@
 */
/*
 * Copyright (c) 1982, 1986, 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)in_pcb.h	8.1 (Berkeley) 6/10/93
 * $FreeBSD: src/sys/netinet/in_pcb.h,v 1.32.2.4 2001/08/13 16:26:17 ume Exp $
 */
/*
 * NOTICE: This file was modified by SPARTA, Inc. in 2007 to introduce
 * support for mandatory and extensible security protections.  This notice
 * is included in support of clause 2.2 (b) of the Apple Public License,
 * Version 2.0.
 */

#ifndef _NETINET_IN_PCB_H_
#define _NETINET_IN_PCB_H_
#include <sys/appleapiopts.h>

#include <sys/types.h>
#include <sys/queue.h>

#include <netinet6/ipsec.h> /* for IPSEC */

typedef	u_quad_t	inp_gen_t;

/*
 * PCB with AF_INET6 null bind'ed laddr can receive AF_INET input packet.
 * So, AF_INET6 null laddr is also used as AF_INET null laddr, by utilizing
 * the following structure.
 */
struct in_addr_4in6 {
	u_int32_t	ia46_pad32[3];
	struct	in_addr	ia46_addr4;
};


/*
 * The range of the generation count, as used in this implementation,
 * is 9e19.  We would have to create 300 billion connections per
 * second for this number to roll over in a year.  This seems sufficiently
 * unlikely that we simply don't concern ourselves with that possibility.
 */

/*
 * Interface exported to userland by various protocols which use
 * inpcbs.  Hack alert -- only define if struct xsocket is in scope.
 */

/*
 * This is a copy of the inpcb as it shipped in Panther. This structure
 * is filled out in a copy function. This allows the inpcb to change
 * without breaking userland tools.
 * 
 * CAUTION: Many fields may not be filled out. Fewer may be filled out
 * in the future. Code defensively.
 */

#pragma pack(4)

#if defined(__LP64__)
struct _inpcb_list_entry {
    u_int32_t	le_next;
    u_int32_t	le_prev;
};
#define _INPCB_PTR(x)		u_int32_t
#define _INPCB_LIST_ENTRY(x)	struct _inpcb_list_entry
#else
#define _INPCB_PTR(x)		x
#define _INPCB_LIST_ENTRY(x)	LIST_ENTRY(x)	
#endif

struct inpcbinfo;
struct inpcbport;
struct mbuf;
struct ip6_pktopts;
struct ip6_moptions;
struct icmp6_filter;
struct inpcbpolicy;

struct inpcb {
	_INPCB_LIST_ENTRY(inpcb) inp_hash;	/* hash list */
	struct	in_addr reserved1;	/* APPLE reserved: inp_faddr defined in protcol indep. part */
	struct	in_addr reserved2; /* APPLE reserved */
	u_short	inp_fport;		/* foreign port */
	u_short	inp_lport;		/* local port */
	_INPCB_LIST_ENTRY(inpcb) inp_list;	/* list for all PCBs of this proto */
	_INPCB_PTR(caddr_t)	inp_ppcb;	/* pointer to per-protocol pcb */
	_INPCB_PTR(struct inpcbinfo *)	inp_pcbinfo;	/* PCB list info */
	_INPCB_PTR(void *)	inp_socket;	/* back pointer to socket */
	u_char	nat_owner;		/* Used to NAT TCP/UDP traffic */
	u_int32_t nat_cookie;		/* Cookie stored and returned to NAT */
	_INPCB_LIST_ENTRY(inpcb) inp_portlist;	/* list for this PCB's local port */
	_INPCB_PTR(struct inpcbport *)	inp_phd;		/* head of this list */
	inp_gen_t inp_gencnt;		/* generation count of this instance */
	int	inp_flags;		/* generic IP/datagram flags */
	u_int32_t inp_flow;

	u_char	inp_vflag;

	u_char inp_ip_ttl;		/* time to live proto */
	u_char inp_ip_p;		/* protocol proto */
	/* protocol dependent part */
	union {
		/* foreign host table entry */
		struct	in_addr_4in6 inp46_foreign;
		struct	in6_addr inp6_foreign;
	} inp_dependfaddr;
	union {
		/* local host table entry */
		struct	in_addr_4in6 inp46_local;
		struct	in6_addr inp6_local;
	} inp_dependladdr;
	union {
		/* placeholder for routing entry */
		u_char	inp4_route[20];
		u_char	inp6_route[32];
	} inp_dependroute;
	struct {
		/* type of service proto */
		u_char inp4_ip_tos;
		/* IP options */
		_INPCB_PTR(struct mbuf *) inp4_options;
		/* IP multicast options */
		_INPCB_PTR(struct ip_moptions *) inp4_moptions;
	} inp_depend4;

	struct {
		/* IP options */
		_INPCB_PTR(struct mbuf *)	inp6_options;
		u_int8_t	inp6_hlim;
		u_int8_t	unused_uint8_1;
		ushort	unused_uint16_1;
		/* IP6 options for outgoing packets */
		_INPCB_PTR(struct ip6_pktopts *)	inp6_outputopts;
		/* IP multicast options */
		_INPCB_PTR(struct ip6_moptions *)	inp6_moptions;
		/* ICMPv6 code type filter */
		_INPCB_PTR(struct icmp6_filter *)	inp6_icmp6filt;
		/* IPV6_CHECKSUM setsockopt */
		int	inp6_cksum;
		u_short	inp6_ifindex;
		short	inp6_hops;
	} inp_depend6;

	int	hash_element;           /* Array index of pcb's hash list    */
	_INPCB_PTR(caddr_t)	inp_saved_ppcb;	/* place to save pointer while cached */
	_INPCB_PTR(struct inpcbpolicy *)	inp_sp;
	u_int32_t	reserved[3];	/* For future use */
};

struct	xinpcb {
	u_int32_t	xi_len;		/* length of this structure */
	struct	inpcb xi_inp;
	struct	xsocket xi_socket;
	u_quad_t	xi_alignment_hack;
};

#if !CONFIG_EMBEDDED

struct inpcb64_list_entry {
    u_int64_t   le_next;
    u_int64_t   le_prev;
};

struct	xinpcb64 {
	u_int64_t		xi_len;		/* length of this structure */
	u_int64_t		xi_inpp;
	u_short 		inp_fport;	/* foreign port */
	u_short			inp_lport;	/* local port */
	struct inpcb64_list_entry	
				inp_list;	/* list for all PCBs of this proto */
	u_int64_t		inp_ppcb;	/* pointer to per-protocol pcb */
	u_int64_t		inp_pcbinfo;	/* PCB list info */
	struct inpcb64_list_entry	
				inp_portlist;	/* list for this PCB's local port */
	u_int64_t		inp_phd;	/* head of this list */
	inp_gen_t		inp_gencnt;	/* generation count of this instance */
	int			inp_flags;	/* generic IP/datagram flags */
	u_int32_t		inp_flow;
	u_char			inp_vflag;
	u_char			inp_ip_ttl;	/* time to live */
	u_char			inp_ip_p;	/* protocol */
	union {					/* foreign host table entry */
			struct  in_addr_4in6	inp46_foreign;
			struct  in6_addr	inp6_foreign;
	}			inp_dependfaddr;
	union {					/* local host table entry */
			struct  in_addr_4in6	inp46_local;
			struct  in6_addr	inp6_local;
	}			inp_dependladdr;
	struct {
			u_char		inp4_ip_tos;	/* type of service */
	}			inp_depend4;
	struct {
			u_int8_t        inp6_hlim;
	int		inp6_cksum;
			u_short		inp6_ifindex;
			short   	inp6_hops;
	}			inp_depend6;
	struct  xsocket64       xi_socket;
	u_quad_t		xi_alignment_hack;
};

#endif /* !CONFIG_EMBEDDED */


struct xinpcb_list_entry {
    u_int64_t   le_next;
    u_int64_t   le_prev;
};

struct	xinpcb_n {
	u_int32_t		xi_len;		/* length of this structure */
	u_int32_t		xi_kind;		/* XSO_INPCB */
	u_int64_t		xi_inpp;
	u_short 		inp_fport;	/* foreign port */
	u_short			inp_lport;	/* local port */
	u_int64_t		inp_ppcb;	/* pointer to per-protocol pcb */
	inp_gen_t		inp_gencnt;	/* generation count of this instance */
	int				inp_flags;	/* generic IP/datagram flags */
	u_int32_t		inp_flow;
	u_char			inp_vflag;
	u_char			inp_ip_ttl;	/* time to live */
	u_char			inp_ip_p;	/* protocol */
	union {					/* foreign host table entry */
		struct  in_addr_4in6	inp46_foreign;
		struct  in6_addr	inp6_foreign;
	}				inp_dependfaddr;
	union {					/* local host table entry */
		struct  in_addr_4in6	inp46_local;
		struct  in6_addr	inp6_local;
	}				inp_dependladdr;
	struct {
		u_char		inp4_ip_tos;	/* type of service */
	}				inp_depend4;
	struct {
		u_int8_t	inp6_hlim;
		int			inp6_cksum;
		u_short		inp6_ifindex;
		short		inp6_hops;
	}				inp_depend6;
};


struct	xinpgen {
	u_int32_t xig_len;	/* length of this structure */
	u_int	xig_count;	/* number of PCBs at this time */
	inp_gen_t xig_gen;	/* generation count at this time */
	so_gen_t xig_sogen;	/* socket generation count at this time */
};

#pragma pack()

/*
 * These defines are for use with the inpcb.
 */
#define INP_IPV4	0x1
#define INP_IPV6	0x2
#define	inp_faddr	inp_dependfaddr.inp46_foreign.ia46_addr4
#define	inp_laddr	inp_dependladdr.inp46_local.ia46_addr4
#define	inp_route	inp_dependroute.inp4_route
#define	inp_ip_tos	inp_depend4.inp4_ip_tos
#define	inp_options	inp_depend4.inp4_options
#define	inp_moptions	inp_depend4.inp4_moptions
#define	in6p_faddr	inp_dependfaddr.inp6_foreign
#define	in6p_laddr	inp_dependladdr.inp6_local
#define	in6p_route	inp_dependroute.inp6_route
#define	in6p_ip6_hlim	inp_depend6.inp6_hlim
#define	in6p_hops	inp_depend6.inp6_hops	/* default hop limit */
#define	in6p_ip6_nxt	inp_ip_p
#define	in6p_flowinfo	inp_flow
#define	in6p_vflag	inp_vflag
#define	in6p_options	inp_depend6.inp6_options
#define	in6p_outputopts	inp_depend6.inp6_outputopts
#define	in6p_moptions	inp_depend6.inp6_moptions
#define	in6p_icmp6filt	inp_depend6.inp6_icmp6filt
#define	in6p_cksum	inp_depend6.inp6_cksum
#define	in6p_ifindex	inp_depend6.inp6_ifindex
#define	in6p_flags	inp_flags  /* for KAME src sync over BSD*'s */
#define	in6p_socket	inp_socket  /* for KAME src sync over BSD*'s */
#define	in6p_lport	inp_lport  /* for KAME src sync over BSD*'s */
#define	in6p_fport	inp_fport  /* for KAME src sync over BSD*'s */
#define	in6p_ppcb	inp_ppcb  /* for KAME src sync over BSD*'s */
#define	in6p_state	inp_state
#define	in6p_wantcnt	inp_wantcnt
#define	in6p_last_outif	inp_last_outif


/* flags in inp_flags: */
#define	INP_RECVOPTS		0x01	/* receive incoming IP options */
#define	INP_RECVRETOPTS		0x02	/* receive IP options for reply */
#define	INP_RECVDSTADDR		0x04	/* receive IP dst address */
#define	INP_HDRINCL		0x08	/* user supplies entire IP header */
#define	INP_HIGHPORT		0x10	/* user wants "high" port binding */
#define	INP_LOWPORT		0x20	/* user wants "low" port binding */
#define	INP_ANONPORT		0x40	/* port chosen for user */
#define	INP_RECVIF		0x80	/* receive incoming interface */
#define	INP_MTUDISC		0x100	/* user can do MTU discovery */
#ifdef __APPLE__
#define INP_STRIPHDR		0x200	/* Strip headers in raw_ip, for OT support */
#endif
#define  INP_FAITH		0x400   /* accept FAITH'ed connections */
#define  INP_INADDR_ANY 	0x800   /* local address wasn't specified */

#define INP_RECVTTL		0x1000
#define	INP_UDP_NOCKSUM		0x2000	/* Turn off outbound UDP checksum */
#define	INP_BOUND_IF		0x4000	/* bind socket to an ifindex */

#define IN6P_IPV6_V6ONLY	0x8000 /* restrict AF_INET6 socket for v6 */
#define	IN6P_PKTINFO		0x10000 /* receive IP6 dst and I/F */
#define	IN6P_HOPLIMIT		0x20000 /* receive hoplimit */
#define	IN6P_HOPOPTS		0x40000 /* receive hop-by-hop options */
#define	IN6P_DSTOPTS		0x80000 /* receive dst options after rthdr */
#define	IN6P_RTHDR		0x100000 /* receive routing header */
#define	IN6P_RTHDRDSTOPTS	0x200000 /* receive dstoptions before rthdr */
#define	IN6P_TCLASS		0x400000 /* receive traffic class value */
#define	IN6P_AUTOFLOWLABEL	0x800000 /* attach flowlabel automatically */
#define	IN6P_BINDV6ONLY		0x1000000 /* do not grab IPv4 traffic */
#define	IN6P_RFC2292		0x2000000 /* used RFC2292 API on the socket */
#define	IN6P_MTU		0x4000000 /* receive path MTU */
#define	INP_PKTINFO		0x8000000 /* receive and send PKTINFO for IPv4 */

#define	INP_NO_IFT_CELLULAR	0x20000000 /* do not use IFT_CELLULAR route */


#endif /* !_NETINET_IN_PCB_H_ */

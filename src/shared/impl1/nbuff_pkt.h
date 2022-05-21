/*
    Copyright (c) 2017 Broadcom
    All Rights Reserved

    <:label-BRCM:2017:DUAL/GPL:standard
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License, version 2, as published by
    the Free Software Foundation (the "GPL").
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    
    A copy of the GPL is available at http://www.broadcom.com/licenses/GPLv2.php, or by
    writing to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
    
    :>
*/

#ifndef _nbuff_pkt_h_
#define _nbuff_pkt_h_

#if defined(BCM_NBUFF_PKT)

#include <typedefs.h>
#include <linux/bcm_skb_defines.h>

#if (defined(CONFIG_BCM_BPM) || defined(CONFIG_BCM_BPM_MODULE))
#include <linux/gbpm.h>
#endif

#if !defined(BCM_EAPFWD)
extern void *nbuff_pktdup(osl_t *osh, void *pkt);
extern void *nbuff_pktdup_cpy(osl_t *osh, void *pkt);
extern void nbuff_pktfree(osl_t *osh, void *pkt, bool send);
extern uchar *nbuff_pktdata(osl_t *osh, void *pkt);
extern uint nbuff_pktlen(osl_t *osh, void *pkt);
extern uint nbuff_pktheadroom(osl_t *osh, void *pkt);
extern uint nbuff_pkttailroom(osl_t *osh, void *pkt);
extern void nbuff_pktsetnext(osl_t *osh, void *pkt, void *x);
extern void *nbuff_pktnext(osl_t *osh, void *pkt);
extern uchar *nbuff_pktpush(osl_t *osh, void *pkt, int bytes);
extern uchar *nbuff_pktpull(osl_t *osh, void *pkt, int bytes);
extern void nbuff_pktsetlen(osl_t *osh, void *pkt, uint len);
extern bool nbuff_pktshared(void *pkt);
extern void *nbuff_pkt_frmnative(osl_t *osh, void *pkt);
extern struct sk_buff *nbuff_pkt_tonative(osl_t *osh, void *pkt);
extern void *nbuff_pktlink(void *pkt);
extern void nbuff_pktsetlink(void *pkt, void *x);
extern uint nbuff_pktprio(void *pkt);
extern void nbuff_pktsetprio(void *pkt, uint x);
extern void *nbuff_pkt_get_tag(void *pkt);
extern int nbuff_pkttag_attach(void *osh, void *pkt);
extern uint nbuff_pktflowid(void *pkt);
extern void nbuff_pktsetflowid(void *pkt, uint x);

#endif /* !BCM_EAPFWD */
/* packet primitives */

#if defined(BCMDONGLEHOST)
#define NBUFF_FKBUFF_OR_SKBUFF
#else
#define NBUFF_IS_SKBUFF /* Use sk_buff directly */
#endif

#ifdef NBUFF_FKBUFF_OR_SKBUFF

#define PKTATTACHTAG(osh, pkt)  nbuff_pkttag_attach((osh), (pkt))
#define PKTFLOWID(pkt)          nbuff_pktflowid((pkt))
#define PKTSETFLOWID(pkt, x)    nbuff_pktsetflowid((pkt), (x))
#define PKTGET(osh, len, send)  linux_pktget((osh), (len))
#define PKTDUP(osh, pkt)        nbuff_pktdup((osh), (pkt))
#define PKTDUP_CPY(osh, pkt)    nbuff_pktdup_cpy((osh), (pkt))
#define PKTFREE(osh, pkt, send) nbuff_pktfree((osh), (pkt), (send))
#define PKTLIST_DUMP(osh, buf)  BCM_REFERENCE(osh)
#define PKTSETPOOL(osh, pkt, x, y)  BCM_REFERENCE(osh)
#define PKTPOOL(osh, pkt)       ({BCM_REFERENCE(osh); BCM_REFERENCE(pkt); FALSE;})

#define PKTDATA(osh, pkt)       nbuff_pktdata((osh), (pkt))
#define PKTLEN(osh, pkt)        nbuff_pktlen((osh), (pkt))
#define PKTHEADROOM(osh, pkt)   nbuff_pktheadroom((osh), (pkt))
#define PKTTAILROOM(osh, pkt)   nbuff_pkttailroom((osh), (pkt))
#define PKTNEXT(osh, pkt)       nbuff_pktnext((osh), (pkt))
#define PKTSETNEXT(osh, pkt, x)     nbuff_pktsetnext((osh), (pkt), (x))
#define PKTPUSH(osh, pkt, bytes)    nbuff_pktpush((osh), (pkt), (bytes))
#define PKTPULL(osh, pkt, bytes)    nbuff_pktpull((osh), (pkt), (bytes))
#define PKTSETLEN(osh, pkt, len)    nbuff_pktsetlen((osh), (pkt), (len))
#define PKTTAG(pkt)             nbuff_pkt_get_tag((pkt))
#define PKTFREELIST(pkt)        PKTLINK(pkt)
#define PKTSETFREELIST(pkt, x)  PKTSETLINK((pkt), (x))
#define PKTFRMNATIVE(osh, pkt)  nbuff_pkt_frmnative((osh), (pkt))
#define PKTTONATIVE(osh, pkt)   nbuff_pkt_tonative((osl_t *)(osh), (pkt))
#define PKTLINK(pkt)            nbuff_pktlink((pkt))
#define PKTSETLINK(pkt, x)      nbuff_pktsetlink((pkt), (x))
#define PKTPRIO(pkt)            nbuff_pktprio((pkt))
#define PKTSETPRIO(pkt, x)      nbuff_pktsetprio((pkt), (x))
#define PKTSHARED(pkt)          nbuff_pktshared((pkt))

/* PKTPOOLGET PKTDIRTYP PKTDATAPRISTINE and PKTTAINTED are not supported */
#else /* NBUFF_IS_SKBUFF */

#define PKTGET(osh, len, send)  linux_pktget((osh), (len))

/* Get a packet buffer from BPM */
#define PKTPOOLGET(osh, len)                            ({                     \
        struct sk_buff *bpm_skb;                                               \
        bpm_skb = gbpm_alloc_buf_skb_attach((len));                            \
        if (bpm_skb) {                                                         \
	       	SKB_BPM_TAINTED(bpm_skb);                                      \
                PKTACCOUNT(osh, 1, TRUE);                                      \
        }                                                                      \
        bpm_skb;                                        })

#define PKTDUP(osh, skb)                                ({                     \
        void *skb_dup;                                                         \
        SKB_BPM_TAINTED((struct sk_buff*)(skb));                               \
        skb_dup = osl_pktdup(osh, skb);                                        \
        skb_dup;                                        })

#define PKTDUP_CPY(osh, skb);                           ({                     \
        void *skb_cpy;                                                         \
        SKB_BPM_TAINTED((struct sk_buff*)(skb));                               \
        skb_cpy = osl_pktdup_cpy(osh, skb);                                    \
        skb_cpy;                                        })

#define PKTFREE(osh, skb, send)                                                \
        linux_pktfree((osh), (skb), (send))

#define PKTLIST_DUMP(osh, buf)                          BCM_REFERENCE(osh)

#define PKTSETPOOL(osh, skb, x, y)                      BCM_REFERENCE(osh)

#define PKTPOOL(osh, skb)                               ({ BCM_REFERENCE(osh); \
                                                           BCM_REFERENCE(skb); \
        FALSE;                                          })

#define PKTDATA(osh, skb)                               ({ BCM_REFERENCE(osh); \
        (((struct sk_buff*)(skb))->data);               })

#define PKTLEN(osh, skb)                                ({ BCM_REFERENCE(osh); \
        (((struct sk_buff*)(skb))->len);                })

#define PKTHEADROOM(osh, skb)                           ({ BCM_REFERENCE(osh); \
        skb_headroom((struct sk_buff*)(skb));           })

#define PKTTAILROOM(osh, skb)                           ({ BCM_REFERENCE(osh); \
        skb_tailroom((struct sk_buff*)(skb));           })

#define PKTNEXT(osh, skb)                               ({ BCM_REFERENCE(osh); \
        (((struct sk_buff*)(skb))->next);               })

#define PKTSETNEXT(osh, skb, skb_next)                  ({ BCM_REFERENCE(osh); \
        (((struct sk_buff*)(skb))->next = (struct sk_buff*)(skb_next)); })

#define PKTPUSH(osh, skb, bytes)                        ({ BCM_REFERENCE(osh); \
        skb_push((struct sk_buff*)(skb), (bytes));      })

#define PKTPULL(osh, skb, bytes)                        ({ BCM_REFERENCE(osh); \
        skb_pull((struct sk_buff*)(skb), (bytes));      })

#define PKTSETLEN(osh, skb, len)                        ({ BCM_REFERENCE(osh); \
        __pskb_trim((struct sk_buff*)(skb), (len));     })

#define PKTTAG(skb) \
        ((void *)(((struct sk_buff*)(skb))->cb))

#define PKTFREELIST(skb)                                                       \
        PKTLINK(skb)

#define PKTSETFREELIST(skb, x)                                                 \
        PKTSETLINK((skb), (x))

#define PKTFRMNATIVE(osh, skb)                          ({                     \
        struct sk_buff *_skb;                                                  \
        _skb = osl_pkt_frmnative((osh), (skb));                                \
        _skb;                                           })

#define PKTTONATIVE(osh, skb)                           ({                     \
        struct sk_buff *_skb;                                                  \
        _skb = osl_pkt_tonative((osh), (skb));                                 \
        _skb;                                           })

#define PKTLINK(skb)                                                           \
        (((struct sk_buff*)(skb))->prev)

#define PKTSETLINK(skb, x)                                                     \
        (((struct sk_buff*)(skb))->prev = (struct sk_buff*)(x))

#define PKTPRIO(skb)                                    ({                     \
        uint32 prio = ((struct sk_buff*)(skb))->mark >> PRIO_LOC_NFMARK & 0x7; \
        if (prio > 7) prio = 0;                                                \
        prio;                                           })

#define PKTSETPRIO(skb, x)                              ({                     \
        ((struct sk_buff*)(skb))->mark &= ~(0x7 << PRIO_LOC_NFMARK);           \
        ((struct sk_buff*)(skb))->mark |= ((x) & 0x7) << PRIO_LOC_NFMARK;      \
                                                        })
#define PKTSHARED(skb)                                                         \
        (((struct sk_buff*)(skb))->cloned)

#define PKTDIRTYP(osh, skb)                             ({                     \
        BCM_REFERENCE(osh); skb_shinfo(skb)->dirty_p;   })

#define PKTTAINTED(osh, skb)                            ({                     \
        BCM_REFERENCE(osh); SKB_BPM_TAINTED(skb);       })

#define PKTDATAPRISTINE(osh, skb)                       ({                     \
        BCM_REFERENCE(osh); SKB_DATA_PRISTINE(skb);     })

#define PKTDATATAINTED(osh, skb)   PKTTAINTED((osh), (skb))
        
#endif /* NBUFF_IS_SKBUFF */

/* MACROS NOT COMMON TO FKB and SKB */

#ifdef BCMDBG_CTRACE
#define PKTCALLER(zskb)	({BCM_REFERENCE(zskb);})
#endif /* BCMDBG_CTRACE */

#define PKTSETFWDERBUF(osh, pkt)  ({ BCM_REFERENCE(osh); BCM_REFERENCE(pkt); })
#define PKTCLRFWDERBUF(osh, pkt)  ({ BCM_REFERENCE(osh); BCM_REFERENCE(pkt); })
#define PKTISFWDERBUF(osh, pkt)   ({ BCM_REFERENCE(osh); BCM_REFERENCE(pkt); FALSE;})
#define PKTORPHAN(pkt)            ({BCM_REFERENCE(pkt); 0;})


#ifdef PKTC
#define CHAINED (1 << 3)
#define PKTSETCHAINED(osh, skb) \
    (IS_SKBUFF_PTR(skb) ? (skbuff_bcm_ext_wlan_get((struct sk_buff*)(skb), pktc_flags) |= CHAINED) : BCM_REFERENCE(skb))
#define PKTCLRCHAINED(osh, skb) \
    (IS_SKBUFF_PTR(skb) ? (skbuff_bcm_ext_wlan_get((struct sk_buff*)(skb), pktc_flags) &= (~CHAINED)) : BCM_REFERENCE(skb))
#define PKTISCHAINED(skb) \
    (IS_SKBUFF_PTR(skb) ? (skbuff_bcm_ext_wlan_get((struct sk_buff*)(skb), pktc_flags) & CHAINED) : FALSE)

/* Use 8 bytes of skb tstamp field to store below info */
struct chain_node {
        struct sk_buff  *link;
        unsigned int    flags:3, pkts:9, bytes:20;
};

#define CHAIN_NODE(skb)         ((struct chain_node*)(skbuff_bcm_ext_wlan_get((struct sk_buff*)skb, pktc_cb)))
#define PKTCSETATTR(skb, f, p, b) (IS_SKBUFF_PTR(skb) ? ({CHAIN_NODE(skb)->flags = (f); CHAIN_NODE(skb)->pkts = (p); \
                                 CHAIN_NODE(skb)->bytes = (b);}) : BCM_REFERENCE(skb))
#define PKTCCLRATTR(skb)        (IS_SKBUFF_PTR(skb) ? ({CHAIN_NODE(skb)->flags = CHAIN_NODE(skb)->pkts = \
                                 CHAIN_NODE(skb)->bytes = 0;}) : BCM_REFERENCE(skb))
#define PKTCGETATTR(skb)        (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->flags << 29 | CHAIN_NODE(skb)->pkts << 20 | \
                                 CHAIN_NODE(skb)->bytes) : ({BCM_REFERENCE(skb); 0;}))
#define PKTCCNT(skb)            (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->pkts) : 1)
#define PKTCLEN(skb)            (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->bytes) : PKTLEN(skb))
#define PKTCGETFLAGS(skb)       (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->flags) : ({BCM_REFERENCE(skb); 0;}))
#define PKTCSETFLAGS(skb, f)    (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->flags = (f)) : BCM_REFERENCE(skb))
#define PKTCCLRFLAGS(skb)       (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->flags = 0) : BCM_REFERENCE(skb))
#define PKTCFLAGS(skb)          (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->flags) : ({BCM_REFERENCE(skb); 0;}))
#define PKTCSETCNT(skb, c)      (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->pkts = (c)) : BCM_REFERENCE(skb))
#define PKTCINCRCNT(skb)        (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->pkts++) : BCM_REFERENCE(skb))
#define PKTCADDCNT(skb, c)      (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->pkts += (c)) : BCM_REFERENCE(skb))
#define PKTCSETLEN(skb, l)      (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->bytes = (l)) : BCM_REFERENCE(skb))
#define PKTCADDLEN(skb, l)      (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->bytes += (l)) : BCM_REFERENCE(skb))
#define PKTCSETFLAG(skb, fb)    (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->flags |= (fb)) : BCM_REFERENCE(skb))
#define PKTCCLRFLAG(skb, fb)    (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->flags &= ~(fb)) : BCM_REFERENCE(skb))
#define PKTCLINK(skb)           (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->link) : NULL)
#define PKTSETCLINK(skb, x)     (IS_SKBUFF_PTR(skb) ? (CHAIN_NODE(skb)->link = (struct sk_buff*)(x)) : BCM_REFERENCE(skb))
#define FOREACH_CHAINED_PKT(skb, nskb) \
	for (; (skb) != NULL; (skb) = (nskb)) \
		if ((nskb) = (PKTISCHAINED(skb) ? PKTCLINK(skb) : NULL), \
			PKTSETCLINK((skb), NULL), 1)
		
#define PKTCFREE(osh, skb, send) \
	(IS_SKBUFF_PTR(skb) ? \
		({do { \
			void *nskb; \
			ASSERT((skb) != NULL); \
			FOREACH_CHAINED_PKT((skb), nskb) { \
				PKTCLRCHAINED((osh), (skb)); \
				PKTCCLRFLAGS((skb)); \
				PKTFREE((osh), (skb), (send)); \
			} \
		} while (0); }) : \
		PKTFREE(osh, skb, send))

#define PKTCENQTAIL(h, t, p) \
	(IS_SKBUFF_PTR(p) ? \
		({do { \
			if ((t) == NULL) { \
				(h) = (t) = (p); \
			} else { \
				PKTSETCLINK((t), (p)); \
				(t) = (p); \
			} \
		} while (0); }) : BCM_REFERENCE(p))

#endif /* PKTC */

#ifdef PKTC_TBL
#define PKTCENQCHAINTAIL(h, t, h1, t1) \
	do { \
		if (((h1) == NULL) || ((t1) == NULL)) \
			break;  \
		if ((t) == NULL) { \
			(h) = (h1); \
			(t) = (t1); \
		} else { \
			PKTSETCLINK((t), (h1)); \
			(t) = (t1); \
		} \
	} while (0)
#endif /* PKTC_TBL */

#if defined(WLCFP)

/** Cache Flow Processing Packet Macros */
#define SKB_CFP_PKT     (1 << 6)

#define PKTISCFP(pkt)   (skbuff_bcm_ext_wlan_get((struct sk_buff*)(pkt), pktc_flags) & SKB_CFP_PKT)

#define PKTGETCFP(pkt)  PKTISCFP(pkt)
#define PKTSETCFP(pkt) \
({ skbuff_bcm_ext_wlan_get((struct sk_buff*)(pkt), pktc_flags) |= SKB_CFP_PKT; })
#define PKTCLRCFP(pkt) \
({ skbuff_bcm_ext_wlan_get((struct sk_buff*)(pkt), pktc_flags) &= ~SKB_CFP_PKT; })

#define PKTGETCFPFLOWID(pkt) \
({  skbuff_bcm_ext_wlan_get((struct sk_buff*)(pkt), wl_flowid); })
#define PKTSETCFPFLOWID(pkt, cfp_flowid) \
({  PKTSETCFP(pkt); skbuff_bcm_ext_wlan_get((struct sk_buff*)(pkt), wl_flowid) = (cfp_flowid); })
#define PKTCLRCFPFLOWID(pkt, cfp_flowid) \
({  PKTCLRCFP(pkt); skbuff_bcm_ext_wlan_get((struct sk_buff*)(pkt), wl_flowid) = (cfp_flowid); })

#endif /* WLCFP */

/**  IDENTIFY IF SKB need to be handled with blog_emit */
#define SKB_BLOG_TOHANDLE_PKT     (1 << 1)
#define PKTISBLOG_TOHANDLE(pkt)   (skbuff_bcm_ext_wlan_get((struct sk_buff*)(pkt), wl_flag1) & SKB_BLOG_TOHANDLE_PKT)
#define PKTGETBLOG_TOHANDLE(pkt)  PKTISBLOG_TOHANDLE(pkt)
#define PKTSETBLOG_TOHANDLE(pkt) \
({ skbuff_bcm_ext_wlan_get((struct sk_buff*)(pkt), wl_flag1) |= SKB_BLOG_TOHANDLE_PKT; })
#define PKTCLRBLOG_TOHANDLE(pkt) \
({ skbuff_bcm_ext_wlan_get((struct sk_buff*)(pkt), wl_flag1) &= ~SKB_BLOG_TOHANDLE_PKT; })

/**  IDENTIFY IF SKB is WFM handled */
#define SKB_WMF_HANDLED_PKT     (1 << 2)
#define PKTISWMF_HANDLED(pkt)   (skbuff_bcm_ext_wlan_get((struct sk_buff*)(pkt), wl_flag1) & SKB_WMF_HANDLED_PKT)
#define PKTGETWMF_HANDLED(pkt)  PKTISWMF_HANDLED(pkt)
#define PKTSETWMF_HANDLED(pkt) \
({ skbuff_bcm_ext_wlan_get((struct sk_buff*)(pkt), wl_flag1) |= SKB_WMF_HANDLED_PKT; })
#define PKTCLRWMF_HANDLED(pkt) \
({ skbuff_bcm_ext_wlan_get((struct sk_buff*)(pkt), wl_flag1) &= ~SKB_WMF_HANDLED_PKT; })

/**  IDENTIFY IF SKB is RX */
#define SKB_RX_PKT     (1 << 3)
#define PKTISRX(pkt)   (skbuff_bcm_ext_wlan_get((struct sk_buff*)(pkt), wl_flag1) & SKB_RX_PKT)
#define PKTGETRX(pkt)  PKTISRX(pkt)
#define PKTSETRX(pkt) \
({ skbuff_bcm_ext_wlan_get((struct sk_buff*)(pkt), wl_flag1) |= SKB_RX_PKT; })
#define PKTCLRRX(pkt) \
({ skbuff_bcm_ext_wlan_get((struct sk_buff*)(pkt), wl_flag1) &= ~SKB_RX_PKT; })

/**  IDENTIFY IF SKB is to be forward back to BSS */
#define SKB_INTRABSS_FWD_PKT     (1 << 4)
#define PKTISINTRABSS_FWD(pkt)   (skbuff_bcm_ext_wlan_get((struct sk_buff*)(pkt), wl_flag1) & SKB_INTRABSS_FWD_PKT)
#define PKTGETINTRABSS_FWD(pkt)  PKTISINTRABSS_FWD(pkt)
#define PKTSETINTRABSS_FWD(pkt) \
({ skbuff_bcm_ext_wlan_get((struct sk_buff*)(pkt), wl_flag1) |= SKB_INTRABSS_FWD_PKT; })
#define PKTCLRINTRABSS_FWD(pkt) \
({ skbuff_bcm_ext_wlan_get((struct sk_buff*)(pkt), wl_flag1) &= ~SKB_INTRABSS_FWD_PKT; })
						  
#endif /* BCM_NBUFF_PKT */

#endif	/* _nbuff_pkt_h_ */
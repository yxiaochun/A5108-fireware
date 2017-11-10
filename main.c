
int rx_count = 0;
int down_count = 0;



/* addresses of static structs */
#define SDIO_INFO_ADDR			((void *) 0x0025C7AC) //xxxx
#define WLC_INFO_ADDR			((void *) 0x00259E94) //xxxx

#define NEXMON_MONITOR_CHANNEL 14

typedef struct sk_buff {
	struct sk_buff *next;       /* 0x00 */
	struct sk_buff *prev;       /* 0x04 */
	void *data;                 /* 0x08 - data head pointer */
	short len;                  /* 0x0c */
	short pad1;                 /* 0x0e */
} __attribute__((packed)) sk_buff;

typedef struct wlc_info {
	int PAD0;                    
	void *osh;                    
	//int PAD1[0x46C/4-2]; 
	//short chanspec;			    /* 0x46C */
	//short pad0;                   
	//int PAD2[0x5D0/4-0x46C/4-1];   
	int PAD2[0x5D0/4-2];                 
	void *active_queue;                 /* 0X5D0 */
} __attribute__((packed)) wlc_info;

typedef void * (*wlc_bsscfg_find_by_wlcif_func)(void *wlc, int wlcif);

wlc_bsscfg_find_by_wlcif_func wlc_bsscfg_find_by_wlcif = (wlc_bsscfg_find_by_wlcif_func)(0x1C1334 + 1);

typedef sk_buff * (*pkt_buf_get_skb_func)(void *osh, unsigned int len) ;

pkt_buf_get_skb_func pkt_buf_get_skb = (pkt_buf_get_skb_func)(0x19E9AC + 1);

typedef void (*pkt_buf_free_func)(void *osh, unsigned int len, int r2) ;

pkt_buf_free_func pkt_buf_free = (pkt_buf_free_func)(0x19E9F0 + 1);

typedef int (*memcpy_func)(void *dst, void *src, int len) ;

memcpy_func memcpyx = (memcpy_func)(0x19A098 + 1);

typedef void * (*__wlc_scb_lookup_func)(void *wlc, void *bsscfg, unsigned char *pkt, int n) ;

__wlc_scb_lookup_func __wlc_scb_lookup = (__wlc_scb_lookup_func)(0x1E4DB4 + 1);

typedef int (*wlc_scb_set_bsscfg_func)(void *scb, void *bsscfg);

wlc_scb_set_bsscfg_func wlc_scb_set_bsscfg = (wlc_scb_set_bsscfg_func)(0x1E4A46 + 1);

typedef int (*wlc_sendctl_func)(void *wlc, void *p, void *qi, void *scb, unsigned int fifo, unsigned int rate_override, char enq_only);

wlc_sendctl_func wlc_sendctl = (wlc_sendctl_func)(0x31CE8 + 1);

typedef int (*dngl_sendpkt_func)(void * r0, sk_buff *p, int channel);

dngl_sendpkt_func dngl_sendpkt = (dngl_sendpkt_func)(0x19C71C + 1); 

typedef int (*sdpcmd_tx_func)(void * r0, sk_buff *p, int channel);

sdpcmd_tx_func sdpcmd_tx = (sdpcmd_tx_func)(0x19B2D0 + 1); 

typedef int (*wlc_recv_func)(struct wlc_info *wlc, unsigned int *p);

wlc_recv_func wlc_recv = (wlc_recv_func)(0x1B6B30 + 1); //xxxx




typedef int (*console_dump_func)(char * fmt, unsigned int);

console_dump_func console_dump = (console_dump_func)(0x3834 + 1); //xxxx

typedef int (*console_dump2_func)(char * fmt, unsigned int, unsigned int);

console_dump2_func console_dump2 = (console_dump2_func)(0x3834 + 1);

typedef int (*console_dump8_func)(char * fmt, int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);

console_dump8_func console_dump8 = (console_dump8_func)(0x3834 + 1);

void dump_var(char *title, unsigned int *data, unsigned int len)
{
	unsigned int i;
	int ll = (len+31)/32;
	
	i = 2;
	for (i = 0; i < ll; i++)
	{
		{
			console_dump8("[%04x] %08x %08x %08x %08x - %08x %08x %08x %08x\n", 
				i*32,
				data[8*i], data[8*i+1], data[8*i+2], data[8*i+3], data[8*i+4], data[8*i+5], data[8*i+6], data[8*i+7]);
		}
		if (i == 2)
			break;
	}

//	console_dump("\n", 0);
}

/**
 *	add data to the start of a buffer
 */
void *
skb_push(sk_buff *p, unsigned int len)
{
	p->data -= len;
	p->len += len;

	return p->data;
}

/**
 *	remove data from the start of a buffer
 */
void *
skb_pull(sk_buff *p, unsigned int len)
{
	p->data += len;
	p->len -= len;

	return p->data;
}

int txcount = 0;

void send_pkt(void);

//  192.168.31.1          f0-b4-29-f3-6f-99
//  192.168.31.38         a8-5b-78-88-b6-33

//000051.967 [0000] 00000070 15002806 8080d2b0 00000080 - 8106008a 1006493b 03174968 d20000d2
//000051.967 [0020] 9a6ff329 89b858e4 0f006e07 428800f0 - 58e40024 2a4789b8 f329b4f0 b4f09a6f
//000051.967 [0040] 996ff329 000040a0 20000416 00000000 - 0003aaaa 00080000 34000045 00000fe0

// 00011.084 [0000] 000000c1 00000000 8080b600 00000480 - 8d060004 100aa683 00a7ac5f b60000b6
//000011.084 [0020] 00000000 00000000 000a0a0a 73b705c8 - 00000080 ffffffff b430ffff fc68459e
//000011.084 [0040] 459eb430 7bd0fc68 97e21181 0000000c - 04310064 6f530600 50417466 84820801
//000011.187 [0000] 000000c1 00000000 8080b400 00000080 - 8f060004 100a3683 00a93c5f b40000b4
//000011.187 [0020] 02000000 00000080 000a0a0a 73b705c8 - 00000080 ffffffff b430ffff fc68459e
//000011.187 [0040] 459eb430 7be0fc68 97e3a181 0000000c - 04310064 6f530600 50417466 84820801
//000011.289 [0000] 000000c1 00000000 8080b400 00000080 - 91060004 100ac682 00aacc5f b40000b4
//000011.289 [0020] 00000000 00000000 000a0a0a 73b705c8 - 00000080 ffffffff b430ffff fc68459e
//000011.289 [0040] 459eb430 7bf0fc68 97e53181 0000000c - 04310064 6f530600 50417466 84820801

//000019.548 [0000] 880f0524 b4f0d5b8 9a6ff329 d4322257 - 5de09a6f 1e121d50 000000a5 0c110064
//24-05-0F-88-B8-D5
int count = 0;
void forward_pkt(struct wlc_info *wlc, sk_buff *p)
{
	unsigned int *pU = (unsigned int *)(p->data);
	unsigned char *pData = (unsigned char *)(p->data);

	if ((int)pU == 0)
		return;
	
	//if (pU[0x34/4] == 0x880f0524)
	//if (pU[0x34/4] == 0x88785ba8  || (pU[0x3C/4] & 0xFEFFFFFF) == 0x32b68878 || pU[0x40/4] == 0x88785ba8 ||
	//    (pU[0x34/4] & 0xFEFFFFFF) == 0x32b68878  || pU[0x3C/4] == 0x88785ba8 || (pU[0x40/4] & 0xFEFFFFFF) == 0x32b68878)
	
		//dump_var("data", p->data, p->len);
	if (pU[0x3C/4] == 0xADAC5DE5 && pU[0x3C/4+1] == 0x00450008 && pU[0x3C/4+3] == 0x01400040 && pData[98] == 0x11)
	//if (pU[0x3C/4] == 0xCCCCCCCC)
	{
		sk_buff *p2;
		
		//int i;

		//dump_var("data", pU+0x34/4, 0x20);

		//for (i = 0; i < 100; i++)
		{
			p2 = pkt_buf_get_skb(wlc->osh, p->len);
			
			// copy the example frame to the buffer
			memcpyx(p2->data, p->data, p->len);
			console_dump2("seq %02x%02x\n", pData[94], pData[95]);
			count ++;
			console_dump("count = %d\n", count);
			dump_var("data", p->data, p->len);
			
			dngl_sendpkt(0x0025abf0, 0x001fbd84, p2);
		}
		
		rx_count++;
	}
	
	//sk_buff *p2;
	//
	//p2 = pkt_buf_get_skb(wlc->osh, p->len+202);
	//
	//skb_pull(p2, 202);
        //
	//// copy the example frame to the buffer
	//memcpyx(p2->data, p->data, p->len);
        //
	//dngl_sendpkt(0x0025abf0, 0x001fbd84, p2);

}


char pkt[] = {
	0x80, 0x00, 0x00, 0x00, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 			/* destination address */
	0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc,			/* source address */
	0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 0xdd, 			/* BSS address */
	0x10, 0x00, 						/* sequence and fragment numbers */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,		/* timestamp */
	0x64, 0x00, 0x21, 0x05, 				
	0x00, 							/* tag number: SSID parameter set */
	0x06, 							/* tag length */
	'N', 'E', 'X', 'M', 'O', 'N',		 		/* SSID */
};


void send_pkt(void)
{
	sk_buff *p;
	struct wlc_info *wlc = WLC_INFO_ADDR;
	void *bsscfg = wlc_bsscfg_find_by_wlcif(wlc, 0);
	void *scb;

	// create a new sk_buff to hold the beacon frame and additional headers
	p = pkt_buf_get_skb(wlc->osh, sizeof(pkt) + 202);

	// shift the beginning of the frame to leave space for additional headers
	skb_pull(p, 202);

	// copy the example frame to the buffer
	memcpyx(p->data, pkt, sizeof(pkt));

	// get station control block (scb) for given mac address
	scb = __wlc_scb_lookup(wlc, bsscfg, pkt, 0);

	// set the scb's bsscfg entry
	wlc_scb_set_bsscfg(scb, bsscfg);

	//wlc_bmac_suspend_mac_and_wait_wrapper(wlc);
	//wlc_set_chanspec(wlc, 0x1904);
	//wlc_enable_mac(wlc);

	// send the frame with the lowest possible rate
	//wlc_sendctl(wlc, p, wlc->active_queue, scb, 1, 0x81020001, 0);
	wlc_sendctl(wlc, p, wlc->active_queue, scb, 1, 0, 0);
	
	//dump_var("pkt", pkt, 0x40);
	
	pkt[4+6+6]++;
	txcount++;
}

/* logical channel numbers */
#define SDPCM_CONTROL_CHANNEL	0	/* Control Request/Response Channel Id */
#define SDPCM_EVENT_CHANNEL	1	/* Asyc Event Indication Channel Id */
#define SDPCM_DATA_CHANNEL	2	/* Data Xmit/Recv Channel Id */

typedef int (*proto_pkt_header_pull_func)(unsigned int *r0, sk_buff *r1);

proto_pkt_header_pull_func proto_pkt_header_pull = (proto_pkt_header_pull_func)(0x19C5C0 + 1); //XXXX


int dump_txdata = 0;
int dump_rxdata = 0;

int ex_proto_pkt_header_pull(unsigned int *r0, sk_buff *r1)
{
	unsigned char *data;
	unsigned short len;
	int channel;
	int ret;
		
	data = r1->data;
	len = r1->len;

	if (dump_txdata)
	{
		dump_var("r1", r1, 0x20);
		dump_var("data", data, len);
	}

	ret = proto_pkt_header_pull(r0, r1);
	
	//dump_var("r1", r1, 0x20);
	
	return ret;
}

typedef int (*show_string_func)() ;

show_string_func show_string = (show_string_func)(0x8C74 + 1);

void ex_hook_show_string()
{
	char *str = (char *)0x0025DED8; //	char		cbuf[CBUF_LEN];

	console_dump(str, 0);
	//console_dump("\n", 0);
	//dump_var("r0", r0, 0x20);
	//dump_var("r1", r1, 0x20);
	//dump_var("data", data-0x40, 0xA0);
	
	console_dump("rx %d ", rx_count);
	console_dump("down %d\n", down_count);

	if (str[0] == 's')
	{
		send_pkt();
	}
	if (str[0] == 't')
	{
		dump_txdata = 1;
	}
	if (str[0] == 'r')
	{
		dump_rxdata = 1;
	}
	
	show_string();
}

/* maccontrol register */
#define MCTL_GMODE              (1U << 31)
#define MCTL_DISCARD_PMQ        (1 << 30)
#define MCTL_WAKE               (1 << 26)
#define MCTL_HPS                (1 << 25)
#define MCTL_PROMISC            (1 << 24)
#define MCTL_KEEPBADFCS         (1 << 23)
#define MCTL_KEEPCONTROL        (1 << 22)
#define MCTL_PHYLOCK            (1 << 21)
#define MCTL_BCNS_PROMISC       (1 << 20)
#define MCTL_LOCK_RADIO         (1 << 19)
#define MCTL_AP                 (1 << 18)
#define MCTL_INFRA              (1 << 17)
#define MCTL_BIGEND             (1 << 16)
#define MCTL_GPOUT_SEL_MASK     (3 << 14)
#define MCTL_GPOUT_SEL_SHIFT    14
#define MCTL_EN_PSMDBG          (1 << 13)
#define MCTL_IHR_EN             (1 << 10)
#define MCTL_SHM_UPPER          (1 <<  9)
#define MCTL_SHM_EN             (1 <<  8)
#define MCTL_PSM_JMP_0          (1 <<  2)
#define MCTL_PSM_RUN            (1 <<  1)
#define MCTL_EN_MAC             (1 <<  0)

typedef int (*wlc_bmac_mctrl_func)(void *wlc_hw, unsigned int mask, unsigned int val); //struct wlc_hw_info *wlc_hw

wlc_bmac_mctrl_func wlc_bmac_mctrl = (wlc_bmac_mctrl_func)(0x49780 + 1); 



#if 0
469 #define MCTL_GMODE              (1U << 31)
470 #define MCTL_DISCARD_PMQ        (1 << 30) 0x40000000  set
471 #define MCTL_WAKE               (1 << 26)
472 #define MCTL_HPS                (1 << 25)
473 #define MCTL_PROMISC            (1 << 24) 0x01000000  set
474 #define MCTL_KEEPBADFCS         (1 << 23) 0x00800000  clear
475 #define MCTL_KEEPCONTROL        (1 << 22) 0x00400000  set
476 #define MCTL_PHYLOCK            (1 << 21) 
477 #define MCTL_BCNS_PROMISC       (1 << 20) 0x00100000  set
478 #define MCTL_LOCK_RADIO         (1 << 19) 
479 #define MCTL_AP                 (1 << 18) 0x00040000  clear
480 #define MCTL_INFRA              (1 << 17) 0x00020000  set
481 #define MCTL_BIGEND             (1 << 16)
482 #define MCTL_GPOUT_SEL_MASK     (3 << 14)
483 #define MCTL_GPOUT_SEL_SHIFT    14
484 #define MCTL_EN_PSMDBG          (1 << 13)
485 #define MCTL_IHR_EN             (1 << 10)
486 #define MCTL_SHM_UPPER          (1 <<  9)
487 #define MCTL_SHM_EN             (1 <<  8)
488 #define MCTL_PSM_JMP_0          (1 <<  2)
489 #define MCTL_PSM_RUN            (1 <<  1)
490 #define MCTL_EN_MAC             (1 <<  0)


    // _NO_ KEEP BAD FCS
    wlc_bmac_mctrl(wlc_hw, 0x41d60000, 0x41520000);
#endif

    
void ex_hook_wlc_recv(struct wlc_info *wlc, sk_buff *p, void *wlc_hw)
{
	unsigned char *data;
	unsigned short len;
	int channel;
	int ret;
		
	data = p->data;
	len = p->len;

	if (dump_rxdata)
	{
		dump_var("data", data, len);
	}
	//	if (data)
	//	{
	//		unsigned int *pU = (unsigned int *)data;
	//		
	//		console_dump2("rx %d %08x\n", (unsigned int)rxcount, pU[0x3C/4]);
	//		if (pU[0x3C/4] == 0xCCCCCCCC)
	//		{
	//			dump_var("data", data, 0x60);
	//		}
	
	//console_dump("ch %04x\n", (unsigned int)wlc->chanspec);
	forward_pkt(wlc, (sk_buff *)p);
	
	//	}
	//}
	wlc_recv(wlc, p);
	
	//wlc_bmac_mctrl(wlc_hw, MCTL_PROMISC, MCTL_PROMISC);

	wlc_bmac_mctrl(wlc_hw, 
			MCTL_DISCARD_PMQ | MCTL_PROMISC | MCTL_KEEPBADFCS | MCTL_KEEPCONTROL | MCTL_BCNS_PROMISC | MCTL_AP | MCTL_INFRA, 
			MCTL_DISCARD_PMQ | MCTL_PROMISC                   | MCTL_KEEPCONTROL | MCTL_BCNS_PROMISC           | MCTL_INFRA);
	
	//send_pkt();

}

void ex_dngl_sendpkt(int r0, int r1, sk_buff * p)
{
	//int r=0;
	//console_dump("r0 %08x ", r0);
	//console_dump("r1 %08x ", r1);
	//console_dump("r2 %08x\n", r2);
	//dump_var("&r", ((&r)+8), 0x80);
	
	//dump_var("data", p->data, p->len);
}

void ex_wlc_set_chanspec(void *wlc, int chanspec)
{
	//console_dump("wlc %08x ", (int)wlc);
	//console_dump("ch %04x\n", (unsigned int)chanspec);
}

void ex_wlc_down()
{
	int r=0x55555555;
	
	dump_var("wlc_down",&r+8, 0x80);
	
	down_count++;
}
/*

adb push fw_bcmdhd.bin /data/local/tmp
adb shell "su -c mount -o remount,rw /system"
adb shell "su -c cp /data/local/tmp/fw_bcmdhd.bin /system/vendor/firmware"
adb shell "su -c chmod 644 /system/vendor/firmware/fw_bcmdhd.bin"


adb shell "su -c ifconfig wlan0 down"
adb shell "su -c ifconfig wlan0 up"

adb shell "su -c /data/local/tmp/bin/dhdutil -i wlan0 consoledump"

/data/local/tmp/dhdutil -i wlan0 membytes -r 0x180000 0xC0000 >/data/local/tmp/dhd_0x180000
adb shell "su -c chmod 777 /data/local/tmp/dhd_0x180000"
adb pull /data/local/tmp/dhd_0x180000


/data/local/tmp/dhdutil cons abcdefghijk
/data/local/tmp/bin/dhdutil -i wlan0 membytes -r 0x180000 0xC0000 >/data/local/tmp/dhd_0x180000


-- scan the WLAN (turn on WiFi, scan will not finish)
*/


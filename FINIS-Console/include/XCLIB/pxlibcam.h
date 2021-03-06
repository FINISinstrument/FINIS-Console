/*
 *
 *	pxlibcam.h	External	14-Dec-2017
 *
 *	Copyright (C)  1999-2015  EPIX, Inc.  All rights reserved.
 *
 *	Frame Grabber Library: Camera Control Definitions
 *
 */


#if !defined(__EPIX_PXLIBCAM_DEFINED)
#define __EPIX_PXLIBCAM_DEFINED
#include "cext_hps.h"     

#ifdef  __cplusplus
extern "C" {
#endif


/*
 * Generic Command Verbs
 */
#define PXCam_MapCameraStateToVideoState	    0x0001
#define PXCam_MapVideoStateToCameraState	    0x0002
#define PXCam_SetDefaultValuesInCameraState	    0x0004
#define PXCam_SetCorrectValuesInCameraState	    0x0005
#define PXCam_SetMinimumValuesInCameraState	    0x0006
#define PXCam_SetMaximumValuesInCameraState	    0x0007
#define PXCam_SetAtomicValuesInCameraState	    0x0008
#define PXCam_InitCameraStateViaHandle		    0x0009
#define PXCam_ResetCameraStateViaHandle 	    0x000A
#define PXCam_UploadCameraStateViaHandle	    0x000B
#define PXCam_UploadCameraStateDeltaViaHandle	    0x000C
#define PXCam_DownloadCameraStateViaHandle	    0x000D
#define PXCam_DownloadCameraIDStateViaHandle	    0x000E
#define PXCam_MapVideoStateSerialToCameraState	    0x000F
#define PXCam_DownloadCameraStatusViaHandle	    0x0010
#define PXCam_ExecuteCameraCommandViaHandle	    0x0011
#define PXCam_SetDefaultVideoValuesInCameraState    0x0012
#define PXCam_SetDefaultAllValuesInCameraState	    0x0013
#define PXCam_SetupLogging			    0x0014
#define PXCam_DownloadCameraID2StateViaHandle	    0x0015
#define PXCam_ExportCameraStateViaPathName	    0x0016
#define PXCam_ImportCameraStateViaPathName	    0x0017

/*
 * Generic Port Selection Verbs
 */
#define PXCam_PortUnused		0x000
#define PXCam_PortIspxdevservice	0x001
#define PXCam_PortIsFILE		0x002	// not used
#define PXCam_PortIsHANDLE		0x003	// not used
#define PXCam_PortIsComHandle		0x004
#define PXCam_PortIsTtysFd		0x005
#define PXCam_PortIsPathName		0x006


/*
 * SILICON VIDEO 2112 State & Control
 */
struct PXCam_SV2112State {
    struct  pxddch  ddch;

    int 	id;		    /* sensor ID as reported	    */
    int 	frameMode;	    /* 's': still frame, 'v': video */
    int 	aoiLeft;	    /* AOI. these values are ..     */
    int 	aoiTop; 	    /* .. always relative to ..     */
    int 	aoiWidth;	    /* .. full resolution and ..    */
    int 	aoiHeight;	    /* .. independent of decimation */
    int 	testRamp;	    /* 0: off, 1: on		    */
    int 	pixelRegistration;  /* 0: off, 1: on		    */
    int 	scanDirection;	    /* CC('L', 'T'): L-R/T-B	    */
				    /* CC('R', 'T'): R-L/T-B	    */
				    /* CC('L', 'B'): L-R/B-T	    */
				    /* CC('R', 'B'): R-L/B-T	    */
    int 	decimation;	    /* encoded as 0xHHVV	    */
				    /* 0x0101: none		    */
				    /* 0x0202: 2x2		    */
				    /* 0x0404: 4x4		    */
    double	gain[3];	    /* for R,G&B in db		    */
    double	exposure;	    /* in milliseconds		    */
    int 	rgbGainLock;	    /* 0: off, 1: on		    */
    double	extendedLineTime;   /* in microseconds		    */
    double	pixelClkFreq;	    /* in MHz. Info only which must */
				    /* be supplied		    */

    struct {
				    /* controlled mode: 	*/
	int	mode;		    /* 's', 'c' 		*/
	double	framerate;	    /* rate in 'c' mode, Hz.	*/
				    /* min. retrigger period	*/
				    /* in 's' mode (1/Hz).	*/
	double	exposure;	    /* millisec 		*/
	int	trigger;	    /* 'n', '+', '-'		*/
	int	strobe; 	    /* 'n', 'p' 		*/
	int	bits;		    /* 8, 10			*/
	double	linepulses;	    /* internal use		*/
	int	minrnl; 	    /* internal use		*/
    } adj;
};
typedef struct PXCam_SV2112State PXCam_SV2112State_s;
#define PXMOS_CAMSV2112STATE	(PXMOS_DDCH+24)

_cDcl(_dllpxlib,_cfunfcc, int)
PXCam_SILICONVIDEO2112(int cmnd, int rsvd, int unitmap, int porttype,
		       void *port, PXCam_SV2112State_s *camstatep,
		       PXCam_SV2112State_s *camstate2p, pxvidstate_s *vidstatep);



/*
 * SILICON VIDEO 1310 State & Control
 */
struct PXCam_SV1310State {
    struct  pxddch  ddch;

    int 	id;		    /* sensor ID as reported		*/
    int 	negADCRef;	    /* mV				*/
    int 	posADCRef;	    /* mV				*/
    int 	power;		    /* 1 or 0				*/
    int 	videoMode;	    /* 'c': free-run, 's': controlled	*/
    int 	subsampleMode;	    /* 0: default: 'm': mono, 'b': bayer*/
    int 	subsample;	    /* encoded as 0xHHVV		*/
				    /* 0x0101: none	 0x0204:	*/
				    /* 0x0202: 2x2	 0x0208:	*/
				    /* 0x0404: 4x4	 0x0401:	*/
				    /* 0x0808: 8x8	 0x0402:	*/
				    /* 0x0102: etc	 0x0408:	*/
				    /* 0x0104:		 0x0801:	*/
				    /* 0x0108:		 0x0802:	*/
				    /* 0x0201:		 0x0804:	*/
    int 	aoiLeft;	    /* AOI. these values are .. 	*/
    int 	aoiTop; 	    /* .. always relative to .. 	*/
    int 	aoiWidth;	    /* .. full resolution and ..	*/
    int 	aoiHeight;	    /* .. independent of decimation	*/
    int 	readoutDirection;   /* CC('L', 'T'): L-R/T-B		*/
				    /* CC('R', 'T'): R-L/T-B		*/
				    /* CC('L', 'B'): L-R/B-T		*/
				    /* CC('R', 'B'): R-L/B-T		*/
    int 	offsetCorrection[64+1]; /* mV * 10			*/
    int 	frameWidth;
    int 	frameHeight;
    int 	expGainMode;	    /* 0: raw, 1: lin1, 2: lin2 	*/
    int 	clrGainMode;	    /* 0: raw, 1: lin1			*/
    int 	analogOffset;	    /* mV				*/
    double	colorGain[4];	    /* x  Gr, R, B, Gb			*/
    double	gain;		    /* x				*/
    double	exposure;	    /* msec				*/
    double	framePeriod;	    /* msec				*/
    int 	aoiFramePeriodMin;  /* rsvd				*/
    int 	strobeMode;	    /* 0:  disabled			*/
				    /* '1': One Line			*/
				    /* 'e': exposure			*/
    int 	shA;		    /* see chip specs			*/
    int 	shB;		    /* see chip specs			*/
    int 	sofDelay;	    /* see chip specs			*/
    int 	frameClampHeight;   /* internal use			*/
    int 	frameClampTop;	    /* internal use			*/
    int 	frameClamp;	    /* 1: auto frame clamp on, 0: off	*/

    double	pixelClkFreq;	    /* in MHz. Info only which must	*/
				    /* be supplied			*/

    struct {
				    /* controlled mode: 	*/
	int	mode;		    /* 's', 'c' 		*/
	double	framerate;	    /* rate in 'c' mode, Hz.	*/
				    /* min. retrigger period	*/
				    /* in 's' mode (1/Hz).	*/
	double	maxframerate;	    /* internal use		*/
	int	trigger;	    /* 'n', '+', '-', 'b'	*/
	int	strobe; 	    /* 'n', 'p' 		*/
	int	bits;		    /* 8, 10			*/
	double	linepulses;	    /* internal use		*/
	int	minrnl; 	    /* internal use		*/
    } adj;
};

typedef struct PXCam_SV1310State PXCam_SV1310State_s;
#define PXMOS_CAMSV1310STATE	(PXMOS_DDCH+96+8)

_cDcl(_dllpxlib,_cfunfcc, int)
PXCam_SILICONVIDEO1310(int cmnd, int rsvd, int unitmap, int porttype,
		       void *port, PXCam_SV1310State_s *camstatep,
		       PXCam_SV1310State_s *camstate2p, pxvidstate_s *vidstatep);


/*
 * SILICON VIDEO 1281 State & Control
 */
struct PXCam_SV1281State {
    struct  pxddch  ddch;

    int 	productID1;	    /* as reported			*/
    int 	productID2;	    /* as reported			*/
    int 	productID3;	    /* as reported			*/
    int 	blacklevel;	    /* black level, 0 thru 62		*/
    int 	sleep;		    /* sleep mode. 0: off, 1: on	*/
    int 	videomode;	    /* CC('v','i'): video mode		*/
				    /* CC('s','x'): controlled mode	*/

    int 	aoiLeft;	    /* capture AOI			*/
    int 	aoiTop; 	    /*	..				*/
    int 	aoiWidth;	    /*	..				*/
    int 	aoiHeight;	    /*	..				*/

    int 	extendHBlank;	    /* lengthen H blanking		*/
    int 	extendVBlank;	    /* lengthen V blanking		*/
				    /* (no effect on current chips)	*/

    double	gain;		    /* dB. log(1.5) thru log(39.375)	*/
    double	exposure;	    /* msec				*/
    double	frameperiod;	    /* rsvd				*/
    double	pixelClkFreq;	    /* in MHz. Info only which must	*/
				    /* be supplied			*/

    /*
     * Chip tweaks which are almost never changed.
     */
    int 	bandgap;	    /* 0: internal, 1: external. reference  */
    int 	clkSchmitt;	    /* Schmitt trigger. 0: off, 1: on	    */
    int 	autoblack;	    /* 0: internal, 1: external reference   */
    int 	_1stcolamp_rst_cnt; /* see chip specs			    */
    int 	_pre_int_rst_cnt;   /* see chip specs			    */
    int 	_ds_rst_cnt;	    /* see chip specs			    */
    int 	_row_sel_wait_cnt;  /* see chip specs			    */
    int 	_feed_thru_cnt;     /* see chip specs			    */
    int 	rsvd[8];

    struct {
				    /* controlled mode: 	*/
	int	mode;		    /* 's', 'c' 		*/
	int	trigger;	    /* 'n', '+', '-', 'b'	*/
	double	framerate;	    /* rate in 'c' mode, Hz.	*/
				    /* min. retrigger period	*/
				    /* in 's' mode (1/Hz).	*/
	double	exposure;	    /* msec. not used		*/
	int	minrnl; 	    /* internal use		*/
	double	maxframerate;	    /* internal use		*/
	double	readout;	    /* internal use		*/
	double	linetime;	    /* internal use		*/
	double	pixcistep;	    /* internal use		*/
	double	rsvd2[4];
	int	rsvd1[4];
    } adj;
};

typedef struct PXCam_SV1281State PXCam_SV1281State_s;
#define PXMOS_CAMSV1281STATE	(PXMOS_DDCH+24+8+9+4+4)

_cDcl(_dllpxlib,_cfunfcc, int)
PXCam_SILICONVIDEO1281(int cmnd, int rsvd, int unitmap, int porttype,
		       void *port, PXCam_SV1281State_s *camstatep,
		       PXCam_SV1281State_s *camstate2p, pxvidstate_s *vidstatep);


/*
 * SILICON VIDEO 9M001 & 9T001 & 5C10/5M10 & 9C10
 * & 10C6/10C6 & 10C-CL/10M-CL & 15C5 State & Control
 */
struct PXCam_SV9M001State {
    struct  pxddch  ddch;

    int 	ID;		    /* chip ID as reported			*/
    int 	aoiLeft;	    /* capture AOI				*/
    int 	aoiTop; 	    /*	..					*/
    int 	aoiWidth;	    /*	..					*/
    int 	aoiHeight;	    /*	..					*/
    int 	subsample;	    /* encoded as 0xHHVV			*/
				    /* 0x0101: none  0x0204: 2x4		*/
				    /* 0x0202: 2x2   0x0208: 2x8		*/
				    /* 0x0404: 4x4   0x0401: 4x1		*/
				    /* 0x0808: 8x8   0x0402: 4x2		*/
				    /* 0x0102: 1x2   0x0408: 4x8		*/
				    /* 0x0104: 1x4   0x0801: 8x1		*/
				    /* 0x0108: 1x8   0x0802: 8x2		*/
				    /* 0x0201: 2x1   0x0804: 8x4		*/
				    /* 0x0110: 1x16  0x0210: 2x16		*/
				    /* 0x0120: 1x32  0x0220: 2x32		*/
				    /* 0x0410: 4x16				*/
				    /* 0x0420: 4x32				*/
				    /* SV9M001/SV9T001: no x__10, x__20 	*/
				    /* SV5x10: no x08__, x__10, x__20		*/
				    /* SV9C10: no x08__ 			*/
				    /* SV10x6/SV10x-CL: no x08__		*/
				    /* SV15C5: no x08__ 			*/
    int 	subbinning;	    /* Bit 0=0: subsample is subsampling	*/
				    /* Bit 0=1: subsample is binning		*/
				    /* SV9T001/SV5x10/SV10x6/SV10x-CL/SV15C5 only */
				    /* Bit 1=0: binning is averaging		*/
				    /* Bit 1=1: binning is summing		*/
				    /* SV5x10/SV15C5 only			*/
    int 	scanDirection;	    /* CC('R','T'): L-R/T-B			*/
				    /* CC('R','B'): L-R/B-T			*/
				    /* SV9M001/SV5x10/SV9C10/SV10x6/SV10x-CL only */
				    /* CC('L','T'): R-L/T-B			*/
				    /* CC('L','B'): R-L/B-T			*/
				    /* SV5x10/SV9C10/SV10x6/SV10x-CL/SV15C5 only*/
    int 	testdata;	    /* 0: off, 1: on				*/
				    /* SV9M001/SV9T001				*/
				    /* 0: off, 1 thru 9: on-various		*/
				    /* SV5x10					*/
				    /* 0: off, 1-4, 256-258: on-various 	*/
				    /* SV9x10					*/
				    /* 0: off, 1-4, 256-257: on-various 	*/
				    /* SV10x6/SV10x-CL/SV15C5			*/
    int 	chipenable;	    /* 1: on, 0: off				*/
				    /* SV9M001/SV9T001 only			*/
    int 	videoMode;	    /* 'c': free-run, 's': controlled		*/
				    /* SV5x10: 'g'^'s': controlledglobal	*/
				    /* SV9T001:'r'^'s': controlledglobal	*/
				    /* SV9C10: 'g'^'s': controlledglobal	*/
				    /* SV10x6: 'g'^'s': controlledglobal	*/
				    /* SV10x-CL: 'g'^'s': controlledglobal	*/
				    /* SV15C5: 'g'^'s': controlledglobal	*/
				    /* SV9C10/SV10x6/SV10x-CL/SV15C5: no 's'	*/
    int 	hBlank; 	    /* lengthen H blanking				*/
				    /* SV9C10/SV10x6/SV10x-CL/SV15C5: actual line len	*/
    int 	vBlank; 	    /* lengthen V blanking				*/
				    /* SV9C10/SV10x6/SV10x-CL/SV15C5: actual frame len	*/
    int 	blackLevelMode;     /* 'n': auto/ADC, 'c': auto 		*/
				    /* 'm': manual, 'd': disabled		*/
				    /* 'c' and 'd' are SV9M001 only		*/
    int 	strobeMode;	    /*	0 : disabled				*/
				    /* 'e': exposure				*/
				    /* '1': one line				*/
				    /* SV9M001/SV9T001 only			*/
				    /*	 0 : disabled				*/
				    /*	'e': enabled				*/
				    /*	'i': inverted				*/
				    /* SV5x10 only				*/
				    /* 'p': programmed				*/
				    /* 'p'|0x80: programmed inverted		*/
				    /* SV9C10/SV10x6/SV10x-CL/SV15C5 only	*/
    int 	microExposure;	    /* 1: allow short exposure less than	*/
				    /* one line, 0: don't                       */
				    /* SV9M001 only				*/
				    /* (SV9T001/SV5x10 future?) 		*/
    int 	macroExposure;	    /* 1: allow long exposure			*/
				    /* 0: don't                                 */
				    /* SV9T001/SV5x10/SV9C10/SV10x6		*/
				    /* /SV10x-CL/SV15C5 only			*/
    int 	blackrgb[4];	    /* rsvd. 0					*/
    int 	blacklevel;	    /* black level: SV9T001 only		*/

    int 	strobeStart;	    /* 0/1/2/3: select start timing		*/
				    /* SV5x10 only				*/
    int 	strobeEnd;	    /* 0/1/2/3: select strobe duration		*/
				    /* SV5x10 only				*/
    int 	rsvd1[4];

    int 	ID2;		    /* internal use				*/

    uint16	lastreg[0x80];	    /* internal use				*/

    double	gainrgb[4];	    /* Gb/B/R/Gr dB				*/
    double	exposure;	    /* exposure period, msec			*/
    double	shutterDelay;	    /* rsvd					*/
    double	framePeriod;	    /* frame period, msec			*/

    double	pixelClkFreq;	    /* in MHz. Info only which must		*/
				    /* be supplied				*/

    double	digitalgain;	    /* digital gain, dB 			*/
				    /* SV9M001: not used			*/
				    /* SV9T001: deprecated			*/
				    /* SV5x10:	not used			*/
				    /* SV9C10:	used				*/
				    /* SV10x6/SV10x-CL:  used			*/
				    /* SV15C5:	used				*/
    double	strobePeriod;	    /* strobe period, msec			*/
				    /* SV9C10/SV10x6/SV10x-CL/SV15C5 only	*/
    double	subscale;	    /* reserved 				*/
    double	rsvd2[5];

    struct {
				    /* controlled mode: 			*/
	int	mode;		    /* 's', 'c' 				*/
	int	trigger;	    /* 'n', '+', '-', 'b'			*/
	double	framerate;	    /* rate in 'c' mode, Hz.			*/
				    /* min. retrigger period			*/
				    /* in 's' mode (1/Hz).			*/
	double	exposure;	    /* msec. not used				*/
	double	maxframerate;	    /* internal use				*/


	int	bits;		    /* 8, 10 (SV9M001/SV9T001)			    */
				    /* 8, 12 (SV5x10/SV9C10/SV10x6/SV10x-CL,SV15C5  */
	int	bitpack;	    /* 0: off, 1: on w. bits!=8 		    */
				    /* only w. PIXCI(R) SI1,SI2,SI4		    */
				    /* only w. PIXCI(R) EB1-PoCL (for SV10x-CL)     */

      /*uint	 strobepolarity: 1; */
	int	strobepolarity;     /* Bit 0: 0=normal, 1=invert strobe 		 */
				    /* SV5x10/SV9C10/SV10x6/SV10x-CL: not needed    */

	int	smaoutSI4;	    /* sma trig output connector is		    */
				    /* sma trig in or camera N expose		    */

	int	rsvd1[1];
	double	rsvd2[4];
    } adj;
};

typedef struct PXCam_SV9M001State PXCam_SV9M001State_s;
#define PXMOS_CAMSV9M001STATE	 (PXMOS_DDCH+21+8+0x80+8+8+6+4+4)

_cDcl(_dllpxlib,_cfunfcc, int)
PXCam_SILICONVIDEO9M001(int cmnd, int rsvd, int unitmap, int porttype,
		       void *port, PXCam_SV9M001State_s *camstatep,
		       PXCam_SV9M001State_s *camstate2p, pxvidstate_s *vidstatep);



/*
 * SILICON VIDEO 642 State & Control
 */
struct PXCam_SV642State {
    struct  pxddch  ddch;

    int 	id;		    /* sensor ID as reported		*/
    int 	aoiLeft;	    /* capture AOI			*/
    int 	aoiTop; 	    /*	..				*/
    int 	aoiWidth;	    /*	..				*/
    int 	aoiHeight;	    /*	..				*/
    int 	hBlank; 	    /* lengthen H blanking		*/
    int 	vBlank; 	    /* lengthen V blanking		*/
    int 	gainrange;	    /* 10: high range, 5: low range	*/
    int 	videoMode;	    /* CCC('m','c','i'): free-run	*/
				    /* CCC('s','s','e'): controlled	*/
				    /* CCC('m','c','e'): master/Cont/Seq*/
				    /* CCC('s','c','e'): slave/Cont/Seq */
    int 	interlaceMode;	    /* 0: progressive			*/
				    /* 1: interlaced			*/
				    /* 2: one field			*/
    int 	macroExposure;	    /* 1: allow long exposure		*/
				    /* 0: don't                         */

    int 	VLNAMP; 	    /* voltage references and internal	*/
    int 	VLNOUT; 	    /* default versus explicit selection*/
    int 	VLN1;		    /*	..				*/
    int 	VLN2;		    /*	..				*/
    int 	VLN2BOOST;	    /*	..				*/
    int 	VLP;		    /*	..				*/
    int 	VREF;		    /*	..				*/
    int 	VOFF;		    /*	..				*/
    int 	VREF1;		    /*	..				*/
    int 	VREF2;		    /*	..				*/
    int 	vref;		    /*	..				*/
    int 	vref2;		    /*	..				*/
    int 	vref1;		    /*	..				*/
    int 	voff;		    /*	..				*/
    int 	rsvd1[8];

    double	exposure;	    /* exposure period, msec		*/
    double	framePeriod;	    /* frame period, msec		*/
    double	gainrgb[4];	    /* Gb/B/R/Gr dB			*/
    double	pixelClkFreq;	    /* in MHz. Info only which must	*/
				    /* be supplied			*/
    double	rsvd2[8];

    struct {
				    /* controlled mode: 	*/
	int	mode;		    /* 's', 'c' 		*/
	int	trigger;	    /* 'n', '+', '-', 'b'	*/
	int	bits;		    /* 8, 10			*/
	int	rsvd1[4];
	double	framerate;	    /* rate in 'c' mode, Hz.	*/
				    /* min. retrigger period	*/
				    /* in 's' mode (1/Hz).	*/
	double	exposure;	    /* msec. not used		*/
	double	maxframerate;	    /* internal use		*/
	double	rsvd2[4];
    } adj;
};
typedef struct PXCam_SV642State     PXCam_SV642State_s;
#define PXMOS_CAMSV642STATE	    (PXMOS_DDCH+25+8+7+8+7+7)

_cDcl(_dllpxlib,_cfunfcc, int)
PXCam_SILICONVIDEO642(int cmnd, int rsvd, int unitmap, int porttype,
		       void *port, PXCam_SV642State_s *camstatep,
		       PXCam_SV642State_s *camstate2p, pxvidstate_s *vidstatep);


/*
 * SILICON VIDEO 643 State & Control
 */
struct PXCam_SV643State {
    struct  pxddch  ddch;

    int 	aoiLeft;	    /* capture AOI			*/
    int 	aoiTop; 	    /*	..				*/
    int 	aoiWidth;	    /*	..				*/
    int 	aoiHeight;	    /*	..				*/
    int 	scanDirection;	    /* CC('R', 'B'): L-R/T-B		*/
				    /* CC('R', 'T'): L-R/B-T		*/
    int 	subsample;	    /* 0x0101: none			*/
				    /* 0x0102: 1x2			*/
    int 	videoMode;	    /* 'c': free-run			*/
				    /* 's': controlled			*/
    int 	pixelClkGran;	    /* internal use			*/
    int 	frameTransfer;	    /* internal use			*/
    int 	macroExposure;	    /* 1: allow long exposure		*/
				    /* 0: don't                         */
    int 	expmode;	    /* 1: normal			*/
				    /* 2: dual slope			*/
				    /* 3: triple slope			*/
    int 	rsvd1[15];

    double	pixelClkFreq;	    /* in MHz. Info only which must	*/
				    /* be supplied			*/
    double	gain;		    /* dB.   20*log10( 1.00)		*/
				    /* thru  20*log10(15.38)		*/
    double	framePeriod;	    /* frame period, msec		*/
    double	exposure;	    /* free-run exposure period, msec	*/
    double	Vcal;		    /* A-D voltage. See Cypress data sheet */
    double	Vblack; 	    /* A-D voltage. See Cypress data sheet */
    double	Voffset;	    /* A-D voltage. See Cypress data sheet */

    double	exposure2;	    /* dual slope exposure/reset	*/
    double	exposure3;	    /* triple slope exposure/reset	*/
    double	rsvd2[14];

    struct {
				    /* controlled mode: 	    */
	int	mode;		    /* 's', 'c' 		    */
	int	trigger;	    /* 'n', '+', '-', 'b'	    */

	int	bits;		    /* 8, 10			    */
	int	strobepolarity;     /* 'p', 'n' 		    */
	int	bitpack;	    /* 0: off, 1: on w. bits!=8     */
				    /* only w. PIXCI(R) SI1,SI2,SI4 */
	int	rsvd1[7];
	double	framerate;	    /* rate in 'c' mode, Hz.	    */
				    /* min. retrigger period	    */
				    /* in 's' mode (1/Hz).	    */
	double	exposure;	    /* controlled exposure, msec    */
	double	maxframerate;	    /* internal use		    */
	double	rsvd2[8];

    } adj;

};

typedef struct PXCam_SV643State     PXCam_SV643State_s;
#define PXMOS_CAMSV643STATE	    (PXMOS_DDCH+12+16+7+16+4+8+3+8)

_cDcl(_dllpxlib,_cfunfcc, int)
PXCam_SILICONVIDEO643(int cmnd, int rsvd, int unitmap, int porttype,
		       void *port, PXCam_SV643State_s *camstatep,
		       PXCam_SV643State_s *camstate2p, pxvidstate_s *vidstatep);


/*
 * SILICON VIDEO WGA State & Control
 */
struct PXCam_SV032State {
    struct  pxddch  ddch;

    int 	id;		    /* sensor ID as reported		    */
    int 	aoiLeft;	    /* capture AOI			    */
    int 	aoiTop; 	    /*	..				    */
    int 	aoiWidth;	    /*	..				    */
    int 	aoiHeight;	    /*	..				    */
    int 	scanDirection;	    /* CC('R', 'T'): L-R/T-B		    */
				    /* CC('R', 'B'): L-R/B-T		    */
				    /* CC('L', 'T'): R-L/T-B		    */
				    /* CC('L', 'B'): R-L/B-T		    */
    int 	subsample;	    /* 0x0101: none			    */
				    /* 0x0102: 1x2			    */
				    /* 0x0104: 1x4			    */
    int 	videoMode;	    /* 'c': free-run			    */
				    /* 's': controlled			    */
    int 	hBlank; 	    /* lengthen H blanking		    */
    int 	vBlank; 	    /* lengthen V blanking		    */
    int 	adcMode;	    /* 10: linear, 12: companded	    */
    int 	adcModeColor;	    /* 0: neutral, 1: warmer		    */
    int 	testPattern;	    /* 0: off, 1-3: various		    */
    int 	agc;		    /* 0: off, 1: auto gain on		    */
    int 	aec;		    /* 0: off, 1: auto exposure on	    */
    int 	strobeMode;	    /* 0: disabled, 'e': enabled,	    */
				    /* 'i': inverted			    */
    int 	readoutMode;	    /* 'i': overlaps exposure		    */
				    /* 'e': follows exposure (not fully     */
				    /*	    supported)			    */
    int 	exposureMode;	    /* 'l': linear			    */
				    /* 'x': high dynamic range (HDR)	    */
    int 	macroExposure;	    /* 1: allow long exposure		    */
				    /* 0: don't                             */
    int 	aecMaxmax;
    int 	agcUpdates;	    /* 1-16: agc update period		    */
    int 	aecUpdates;	    /* 1-16: aec update period		    */
    int 	aegcTarget;	    /* 16-1008: agc/aec image target value  */
    int 	aecLpf; 	    /* 0-2: aec low pass filtering	    */
    int 	agcLpf; 	    /* 0-2: agc low pass filtering	    */
    int 	aegcCount;	    /* 0-65535: pixels analyzed for aec/agc */
    int 	hdrMode;	    /* 'a': auto HDR, 'm': manual HDR	    */
    int 	hdrKnee;	    /* 1-2: # of HDR knees		    */
    int 	ecl;		    /* 0: off, 1: anti-eclipse on	    */
    int 	rsvd1[16];

    double	gain;		    /* in dB.				    */
    double	exposure;	    /* in milliseconds			    */
    double	framePeriod;	    /* frame period, msec		    */
    double	agcMax; 	    /* max gain set by AGC, in dB.	    */
    double	aecMax; 	    /* max exposure set by AEC, in msec.    */
    double	digitalGain;	    /* in dB.				    */
    double	vref;		    /* ADC Ref., V.			    */
    double	v1;		    /* ADC V1, V.			    */
    double	v2;		    /* ADC V2, V.			    */
    double	v3;		    /* ADC V3, V.			    */
    double	v4;		    /* ADC V4, V.			    */
    double	hdrT2;		    /* HDR T2 & T3 ratio ..		    */
    double	hdrT3;		    /* .. pow(.5, 15) - pow(.5, 0)	    */
    double	hdrExp1;	    /* HDR T1 exposure, msec.		    */
    double	hdrExp2;	    /* HDR T1 exposure, msec.		    */
    double	eclRef; 	    /* anti-eclipse reference, V.	    */
    double	rsvd2[15];

    struct {
	int	    isColor;	    /* 0: sensor is mono, 1: is color	*/
				    /* Info only which must be supplied */
	double	    pixelClkFreq;   /* in MHz. Info only which must	*/
				    /* be supplied			*/
    } adi;

    struct {
				    /* controlled mode: 	    */
	int	mode;		    /* 's', 'c' 		    */
	int	trigger;	    /* 'n', '+', '-', 'b'	    */

	int	bits;		    /* 8, 10			    */
	int	bitpack;	    /* 0: off, 1: on w. bits!=8     */
				    /* only w. PIXCI(R) SI1,SI2,SI4 */
	int	strobePolarity;     /* not used 		    */
	int	rsvd1[8];

	double	frameRate;	    /* rate in 'c' mode, Hz.	    */
				    /* min. retrigger period	    */
				    /* in 's' mode (1/Hz).	    */
	double	exposure;	    /* msec. not used		    */
	double	maxFrameRate;	    /* internal use		    */
	double	resetDelay;	    /* internal use		    */
	double	rsvd2[8];

    } adj;

};

typedef struct PXCam_SV032State     PXCam_SV032State_s;
#define PXMOS_CAMSV032STATE	    (PXMOS_DDCH+29+16  +16+15 +2 +5+8+4+8)

_cDcl(_dllpxlib,_cfunfcc, int)
PXCam_SILICONVIDEO032(int cmnd, int rsvd, int unitmap, int porttype,
		       void *port, PXCam_SV032State_s *camstatep,
		       PXCam_SV032State_s *camstate2p, pxvidstate_s *vidstatep);


/*
 * SILICON VIDEO HAS21 State & Control
 */
struct PXCam_SVHAS2State {
    struct  pxddch  ddch;

    int 	id;		    /* sensor ID as reported		    */
    int 	aoiLeft;	    /* capture AOI			    */
    int 	aoiTop; 	    /*	..				    */
    int 	aoiWidth;	    /*	..				    */
    int 	aoiHeight;	    /*	..				    */

    int 	videoMode;	    /*	'c': Free-run Max Rate		    */
				    /*	'C': Free-run Pgm Rate		    */
				    /*	's': Ext. Triggered		    */
    int 	black;		    /* not used 			    */
    int 	readout;	    /* 'n': CDS/NDR Non-Destructive Readout */
				    /* 'd': DS/DR   Destructive Readout     */
    int 	aps;		    /*	1: Active, 0: Standby		    */
    int 	rsvd1[30];
    double	pgaOffset;	    /* PGA Offset, 0.3 to 1.3 V.	    */
    double	pgaGain;	    /* PGA Gain, 0.0 to 18.1 dB.	    */
    double	framePeriod;	    /* frame period, msec		    */
    double	exposure;	    /* in milliseconds			    */
    double	ndrBlack;	    /* NDR black level, 0.4 to 2.9 V.	    */
    double	ndrOffset;	    /* NDR even/odd offset, -17.5 to 17.5mV.*/
    double	rsvd2[6];

    struct {
	double	    pixelClkFreq;   /* in MHz. Info only which must	*/
				    /* be supplied			*/
    } adi;

    struct {
				    /* controlled mode: 	    */
	int	mode;		    /* 's', 'c' 		    */
	int	trigger;	    /* 'n', '+', '-', 'b'	    */

	int	bits;		    /* 8, 12			    */
	int	bitpack;	    /* 0: off, 1: on w. bits!=8     */
				    /* only w. PIXCI(R) SI1,SI2,SI4 */
	int	strobePolarity;     /* not used 		    */
	int	rsvd1[8];

	double	frameRate;	    /* rate in 'c' mode, Hz.	    */
				    /* min. retrigger period	    */
				    /* in 's' mode (1/Hz).	    */
	double	exposure;	    /* msec. not used		    */
	double	maxFrameRate;	    /* internal use		    */
	double	resetDelay;	    /* internal use		    */
	double	rsvd2[8];

    } adj;
};

typedef struct PXCam_SVHAS2State     PXCam_SVHAS2State_s;
#define PXMOS_CAMSVHAS2STATE	     (PXMOS_DDCH+9+32+2+8+1+5+8+4+8)

_cDcl(_dllpxlib,_cfunfcc, int)
PXCam_SILICONVIDEOHAS2(int cmnd, int rsvd, int unitmap, int porttype,
		       void *port, PXCam_SVHAS2State_s *camstatep,
		       PXCam_SVHAS2State_s *camstate2p, pxvidstate_s *vidstatep);



/*
 * SILICON VIDEO 2KS State & Control
 */
struct PXCam_SV2000State {
    struct  pxddch  ddch;

    int 	id;		    /* sensor ID as reported		    */
    int 	aoiLeft;	    /* capture AOI			    */
    int 	aoiTop; 	    /*	..				    */
    int 	aoiWidth;	    /*	..				    */
    int 	aoiHeight;	    /*	..				    */
    int 	aoiTops[8];	    /*	..				    */
    int 	aoiHeights[8];	    /*	..				    */
    int 	aoiN;

    int 	adcGain;
    int 	adcGainDefault;
    int 	darkLevel;
    int 	bitMode;
    int 	outputMode;
    int 	adcResolution;
    int 	scanDirection;
    int 	Vramp;
    int 	videoMode;	    /* 's': controlled */
    int 	expMode;
    int 	subsample;
    int 	hdrMode;
    int 	hdrFrameCount;
    int 	power;
    int 	reg73;
    int 	Vlow2;
    int 	Vlow3;
    int 	rsvd1[30];

    double	analogGain;
    double	exposure;
    double	exposure2;
    double	exposure3;
    double	exposure4;
    double	rsvd2[14];

    struct {
	int	    isColor;
	double	    pixelClkFreq;   /* in MHz. Info only which must	*/
				    /* be supplied			*/
    } adi;

    struct {
				    /* controlled mode: 	    */
	int	mode;		    /* 's', 'c' 		    */
	int	trigger;	    /* 'n', '+', '-', 'b'	    */

	int	bits;
	int	bitpack;

	int	strobePolarity;
	int	rsvd1[8];

	double	frameRate;
	double	exposure;
	double	maxFrameRate;
	double	rsvd2[9];

    } adj;
};

typedef struct PXCam_SV2000State     PXCam_SV2000State_s;
#define PXMOS_CAMSV2000STATE	     (PXMOS_DDCH+5+8+8+1+15+32+3+16+(2)+(5+8+4+8))

_cDcl(_dllpxlib,_cfunfcc, int)
PXCam_SILICONVIDEO2000(int cmnd, int rsvd, int unitmap, int porttype,
		       void *port, PXCam_SV2000State_s *camstatep,
		       PXCam_SV2000State_s *camstate2p, pxvidstate_s *vidstatep);



/*
 * SILICON VIDEO 1C45/1M45 State & Control
 */
struct PXCam_SV1CM45State {
    struct  pxddch  ddch;

    int 	id;		    /* sensor ID as reported		    */
    int 	revision;	    /* sensor ID as reported		    */
    int 	aoiLeft;	    /* capture AOI			    */
    int 	aoiTop; 	    /*	..				    */
    int 	aoiWidth;	    /*	..				    */
    int 	aoiHeight;	    /*	..				    */

    int 	outputSlewRate;     /* reserved 			    */
    int 	pixclkSlewRate;     /* reserved 			    */
    int 	pixclkPhase;	    /* reserved 			    */
    int 	analogGain;	    /* 1, 2, 4, 8 (multiplicative factor)   */
    int 	offset; 	    /* pixel offset value		    */
    int 	testdata;	    /* 0: off, 1, 2, 3, 256: on-various     */
    int 	subsample;	    /* encoded as 0xHHVV		    */
				    /* 0x0101: none			    */
				    /* 0x0102: 1x2 (HxV)		    */
				    /* 0x0104: 1x4			    */
				    /* 0x0108: 1x8			    */
				    /* 0x0110: 1x16			    */
				    /* 0x0120: 1x32			    */
    int 	subbinning;	    /* Bit 0=0: subsample is subsampling    */
				    /* Bit 0=1: subsample is binning	    */
    int 	scanDirection;	    /* CC('R', 'T'): L-R/T-B		    */
				    /* reserved 			    */
    int 	hBlank; 	    /* lengthen H blanking		    */
    int 	vBlank; 	    /* lengthen V blanking		    */
    int 	microExposure;	    /* 1: allow short exposure less than    */
				    /* one line, 0: don't                   */
    int 	macroExposure;	    /* 1: allow long exposure		    */
				    /* 0: don't                             */
    int 	videoMode;	    /* 'c': Free-run			    */
				    /* 's': Ext. Triggered		    */
    int 	strobe; 	    /* 0: disabled, 'e': enabled,	    */
				    /* 'i': inverted			    */
    int 	aec;		    /* 1: enable auto exposure, 0: disable	*/
    int 	agc;		    /* 1: enable auto digital gain, 0: disable	*/
    int 	aac;		    /* 1: enable auto analog gain, 0: disable	*/
    int 	aegcTarget;	    /* aec, agc, aac target brightness value	*/
    int 	aecMaxmax;	    /* 1: force the aecMax to be the maximum*/
				    /* 0: don't                             */
    int 	aacMin; 	    /* auto analog gain, min settable value */
    int 	aegcLeft;	    /* aec/agc/aac AOI			    */
    int 	aegcTop;	    /*	..				    */
    int 	aegcWidth;	    /*	..				    */
    int 	aegcHeight;	    /*	..				    */
    int 	aegcAoiMax;	    /* 1: force aec/agc/aac AOI to max	    */
				    /* 0: don't                             */
    int 	rowNoiseCorrection;	/* 1: enable row noise correction, 0: disable	*/
    int 	columnCorrection;	/* 1: enable column correction, 0: disable	*/
    int 	blackLevelCorrection;	/* 1: enable black level correction, 0: disable */
    int 	embeddedData;		/* reserved					*/
    int 	rsvd1[27];



    double	gainrgb[4];	    /* digital gain Gb/B/R/Gr dB	*/
    double	exposure;	    /* in milliseconds			*/
    double	framePeriod;	    /* frame period, msec		*/
    double	aecMax; 	    /* auto exp. max settable value, msec.  */
    double	aecMin; 	    /* auto exp. min settable value, msec.  */
    double	aegcRate;	    /* aec, agc, aac rate; values > 1.0 */
				    /* may cause overshoot		*/
    double	rsvd2[14];

    struct {
	int	aegcMean;	    /* status: computed image mean	*/
				    /* brightness for aec, agc, aac	*/

	int	aacAnalogGain;	    /* status: aac analog gain		*/
	int	rsvd1[2];
	double	agcGainrgb[4];	    /* status: agc digital gain, dB	*/
	double	aecExposure;	    /* status: aec exposure, msec	*/
	double	sensorTemp;	    /* status: sensor temp. in deg C	*/
	double	rsvd2[3];
    } ads;

    struct {
	int	calibrateColumnCorrection;  /* Bit0=1: execute column calibration	*/
					    /* bit0 reset after execution		*/
	int	rsvd1[2];
    } ade;

    struct {
	int	    isColor;	    /* 0: sensor is mono, 1: is color	*/
				    /* Info only which must be supplied */
	double	    pixelClkFreq;   /* in MHz. Info only which must	*/
				    /* be supplied			*/
    } adi;

    struct {
				    /* controlled mode: 	    */
	int	mode;		    /* 's', 'c' 		    */
	int	trigger;	    /* 'n', '+', '-', 'b'	    */

	int	bits;		    /* 8, 12			    */
	int	bitpack;	    /* 0: off, 1: on w. bits!=8     */
				    /* only w. PIXCI(R) SI1,SI2,SI4 */
	int	strobePolarity;     /* not used 		    */

	int	smaoutSI4;	    /* sma trig output connector is	*/
				    /* sma trig in or camera N expose	*/
	int	rsvd1[7];

	double	frameRate;	    /* rate in 'c' mode, Hz.	    */
				    /* min. retrigger period	    */
				    /* in 's' mode (1/Hz).	    */
	double	exposure;	    /* msec. not used		    */
	double	maxFrameRate;	    /* internal use		    */
	double	rsvd2[9];

    } adj;
};

typedef struct PXCam_SV1CM45State     PXCam_SV1CM45State_s;
#define PXMOS_CAMSV1CM45STATE	      (PXMOS_DDCH+6+35+27+4+5+14+(2+2+4+2+3)+(3)+(2)+(6+7+3+9))

_cDcl(_dllpxlib,_cfunfcc, int)
PXCam_SILICONVIDEO1CM45(int cmnd, int rsvd, int unitmap, int porttype,
		       void *port, PXCam_SV1CM45State_s *camstatep,
		       PXCam_SV1CM45State_s *camstate2p, pxvidstate_s *vidstatep);




/*
 * Common for base for various cameras
 */
struct PXCam_SVClass15State {

    struct {
	int	timeout;		/* reserved. 0			    */
	int	retries;		/* reserved. 0			    */
	int	rsvd[6];		/* reserved. 0			    */
    } adp;

    struct {				/* info 			    */
	char	infoName[64];		/* ID as reported. \0 terminated    */
	char	infoVersion[32];	/* ID as reported. \0 terminated    */
	char	infoSerial[32]; 	/* ID as reported. \0 terminated    */
	char	infoSensorSerial[32];	/* ID as reported. \0 terminated    */
	char	infoFpgaVersion[32];	/* current page.   \0 terminated    */
	char	infoFpgaInfo[32];	/* reserved. 0			    */
	char	infoFpgaStat[32];	/* reserved. 0			    */
    } add;

    struct {				/* info 			    */
	double	    sensorTemp; 	/* sensor degC, 		    */
					/* or DBL_MIN if unavailable	    */
	double	    sensorClkFreq;	/* sensor ext clk, MHz		    */
	double	    sensorClkFreq0;	/* sensor ext clk, page 0, MHz	    */
	double	    interfaceClkFreq;	/* camera link clk, MHz 	    */
	double	    pixelClkFreq;	/* sensor pixel clk, MHZ	    */
	double	    rsvd2[3];
	int	    infoFpgaVersN;	/* fpga version 		    */
	int	    infoFpgaVers0N;	/* fpga version, page0		    */
	int	    interfaceConfig;	/* 0: unspecified		    */
					/* 'B': Camera Link Base	    */
					/* 'F': Camera Link Full	    */
					/* 'D': Camera Link Deca/80Bit	    */
	int	    fpgaPage;		/* rsvd 			    */
	int	    rsvd1[3];
    } ads;

    struct {
	int	    dataMode;
	int	    videoMode;		/* 'c': free-run mode		    */
	int	    aoiLeft;		/* AOI. these values are ..	    */
	int	    aoiTop;		/* .. always relative to ..	    */
	int	    aoiWidth;		/* .. full resolution and ..	    */
	int	    aoiHeight;		/* .. independent of decimation     */
	int	    subsample;		/* encoded as 0xHHVV		    */
					/* 0x0101: none 		    */
					/* 0x0102: 1x2 (HxV)		    */
					/* ...				    */
	int	    subbinning;
	int	    triggerPolarity;	/* '+': rising edge trigger	    */
					/* '-': falling edge trigger	    */

	int	    triggerSource;
	int	    scanDirection;	/* CC('R', 'T'): L-R/T-B	    */
					/* CC('R', 'B'): L-R/B-T	    */
					/* CC('L', 'T'): R-L/T-B	    */
					/* CC('L', 'B'): R-L/B-T	    */

	int	    anablackrgb[4];	/* analog Gb/B/R/Gr		    */
	int	    digblackrgb[4];	/* digital Gb/B/R/Gr		    */
	int	    aoiMode;
	int	    anagainMode;
	int	    diggainMode;
	int	    anablackMode;
	int	    digblackMode;
	int	    expMode;
	int	    testData;		/* 0: disabled			    */
	int	    strobeMode;
	int	    framePeriodMode;
	int	    lutEnable;
	int	    lutLoadType;
	int	    lutLoadMode;
	uchar	    microExposure;
	uchar	    macroExposure;
	uchar	    rsvd2[5];

	double	    scaleWidth;
	double	    scaleHeight;
	double	    exposure;		/* in milliseconds		*/
	double	    framePeriod;	/* in milliseconds		*/
	double	    strobePeriod;	/* in milliseconds		*/
	double	    anagainrgb[4];	/* analog gain Gb/B/R/Gr dB	*/
	double	    diggainrgb[4];	/* digital gain Gb/B/R/Gr dB	*/
	double	    lutKneePos[6];
	double	    lutKneeValue[6];
    } ad1;

    struct {
	int	    isColor;
    } adi;

    struct {
				/* controlled mode:		*/
	int	mode;		/* 's', 'c'			*/
	int	trigger;	/* 'n', '+', '-', 'b'		*/
	int	bitpack;	/* 0: off, 1: on w. bits!=8	*/
	int	rsvd1[3];

	double	exposure;	/* controlled exposure, msec	*/
	double	frameRate;	/* rate in 'c' mode, Hz.	*/
				/* min. retrigger period	*/
				/* in 's' mode (1/Hz).		*/
	double	maxframeRate;	/* internal use 		*/
	double	rsvd2[2];
    } adj;
};
typedef struct PXCam_SVClass15State	PXCam_SVClass15State_s;
#define PXMOS_CAMSVCLASS15STATE 	((2+6)+(64+6*32)+(5+3+4+3)+(11+4+4+11+4+3+5+5+4+6+6)+(1)+(3+3+3+2))



/*
 * SILICON VIDEO 20M-CL/20C-CL State & Control
 */
struct PXCam_SV20MCLState {
    struct  pxddch  ddch;

    struct  PXCam_SVClass15State c15;	    /* common stuff			*/

    struct {
	int	dpcFactColumns[16];	    /* info as queried			*/
	int	dpcFactRows[16];	    /* info as queried			*/
	int	dpcFactColumnsN;	    /* info as queried			*/
	int	dpcFactRowsN;		    /* info as queried			*/
	int	rsvd1[3];
	double	tempGain0;		    /* info as queried. rsvd		*/
	double	tempOffset0;		    /* info as queried. rsvd		*/
	double	rsvd2[3];
    } add;

    struct {
	int	aoiN;			    /* number of AOIs, 1 - 8		*/
	int	aoiTops[8];		    /* capture AOIs			*/
	int	aoiHeights[8];		    /*	...				*/
	int	dpcRowsN;		    /* number of dpcRows[]		*/
	int	dpcColumnsN;		    /* number of dpcColumns[]		*/
	int	dpcRows[16];		    /* bad rows to be replaced		*/
	int	dpcColumns[16]; 	    /* bad cols to be replaced		*/

	int	dpcMode;		    /* 0x01: replace bad rows		*/
					    /* 0x04: replace bad cols		*/
					    /* 0x02: rows: bayer, not grey, mode*/
					    /* 0x08: cols: bayer, not grey, mode*/
					    /* 0x10: rows: use factory list	*/
					    /* 0x20: cols: use factory list	*/

	int	hdrMode;		    /* high dynamic range mode		*/
					    /* 0: normal			*/
					    /* '2': 2 slope			*/
					    /* '3': 3 slope			*/


	int	outputMode;		    /* reserved 		    */
	int	rsvd1[16];

	uchar	blackSunLevel;		    /* 0: disabled		    */
					    /* 1 - 63: increased protection */
	uchar	clockGating;		    /* 0: lower noise, higher power */
					    /* 1: more noise, lower power   */
	uchar	fotMult;		    /* reserved 		    */
	uchar	power;			    /* reserved 		    */
	uchar	pinD3;			    /* reserved 		    */
	uchar	pinD1;			    /* reserved 		    */
	uchar	lvdsInput;		    /* reserved 		    */
	uchar	lvdsOutput;		    /* reserved 		    */
	uchar	rsvd2[16];

	double	Vlevel2;		    /* for 2nd slope HDR, DAC Knee, 0 to 100% */
	double	Vlevel3;		    /* for 3rd slope HDR, DAC Knee, 0 to 100% */
	double	exposure2;		    /* for 2nd slope HDR, msec	    */
	double	exposure3;		    /* for 2nd slope HDR, msec	    */
	double	exposure4;		    /* reserved 		    */
	double	rsvd3[16];
    } ad1;

    /*
     * Notes on allowable values for variables in PXCam_SVClass15State
     *
    int 	expMode;		// controlled mode exposure:	    //
					// 'p': programmed		    //
					// 'w': pulse width		    //

    int 	scanDirection;		// CC('R', 'T'): L-R/T-B	    //
					// CC('R', 'B'): L-R/B-T	    //

    int 	dataMode;		// (2<<8) | 8):    8 bit  2 tap    Base     //
					// (8<<8) | 8):    8 bit  8 tap    Full     //
					// (2<<8) | 10):   10 bit 2 tap    Base     //
					// (8<<8) | 10):   10 bit 8 tap    80-Bit   //
					// (2<<8) | 12):   12 bit 2 tap    Base     //
					// (20<<8) | 12):  12 bit 20/3 tap 80-Bit   //

    int 	videoMode;		// 'c': free-run mode		    //
					// 's': controlled/trigger mode     //

    int 	subsample;		// encoded as 0xHHVV		    //
					// 0x0101: none 		    //
					// 0x0102: 1x2 (HxV)		    //
					// 0x0104: 1x4			    //
					// 0x0108: 1x8			    //
					// 0x0110: 1x16 		    //


    int 	digblackrgb;		// pixel offset 		    //
					// (1296-2048) thru (1296+2047)     //

    double	anagainrgb;		// 1.6x, 1.9x, 2.25x, 2.55x	    //

    double	diggainrgb;		// 0.55x thru 5.8x		    //
					// values < 2x not recommended	    //

    int 	testData;		// 0: disabled, 1: enabled	    //

    int 	lutEnable;		// 0: disabled			    //
					// 1: map pixel data thru lut	    //

    int 	lutLoadType;		// 0:	     disabled				//
					// 'l':      load linear lut			//
					// 'l'^0x80: load inverted linear lut		//
					// 'g':      load gamma lut			//
					//	     w. lutKneeValue[0] as gamma	//
					// 'g'^0x80: load inverted gamma lut		//
					//	     w. lutKneeValue[0] as gamma	//
					// 0x100|'k':load 1-knee lut using		//
					//	     lutKneePos[0] & lutKneeValue[0]	//
					//	     as pixel value & slope		//
					// 0x100|'k'^0x80:				//
					//	     load inverted 1-knee lut using	//
					//	     lutKneePos[0] & lutKneeValue[0]	//
					//	     as pixel value & slope		//
					// 0x200|'k':load 2-knee lut using		//
					//	     lutKneePos[0] & lutKneeValue[0]	//
					//	     lutKneePos[1] & lutKneeValue[1]	//
					//	     as pixel value & slope		//
					// 0x200|'k'^0x80:				//
					//	     load inverted 2-knee lut using	//
					//	     lutKneePos[0] & lutKneeValue[0]	//
					//	     lutKneePos[1] & lutKneeValue[1]	//
					//	     as pixel value & slope		//
    *
    */
};

typedef struct PXCam_SV20MCLState     PXCam_SV20MCLState_s;
#define PXMOS_CAMSV20MCLSTATE	      (PXMOS_DDCH+PXMOS_CAMSVCLASS15STATE+(16+16+2+3+2+3)+(1+8+8+2+16+16+3+16+8+16+8+16+5+16))

_cDcl(_dllpxlib,_cfunfcc, int)
PXCam_SILICONVIDEO20MCL(int cmnd, int rsvd, int unitmap, int porttype,
		       void *port, PXCam_SV20MCLState_s *camstatep,
		       PXCam_SV20MCLState_s *camstate2p, pxvidstate_s *vidstatep);



/*
 * SILICON VIDEO 1820CL State & Control
 */
struct PXCam_SV1820CLState {
    struct  pxddch  ddch;

    struct  PXCam_SVClass15State c15;

    struct {
	int	    sensorID;
	int	    sensorRev;
	int	    sensorCraCode;
	int	    rsvd1[3];
    } add;

    struct {
	int	    chipenable;     /* 0: stopped			    */
				    /* 1: normal			    */
	int	    hBlank;	    /* lengthen H blanking		    */
	int	    vBlank;	    /* lengthen V blanking		    */
	int	    hvBlankMode;    /* 0, rsvd				    */
	int	    rsvd1[3];
	double	    rsvd2[3];
    } ad1;

    struct {
	uint16	    pllRegs[7];
	int	    clkMode;
	double	    sensorClks[12];
    } adc;

    /*
     * Notes on allowable values for variables in PXCam_SVClass15State
     *
    int 	    dataMode;	    // (2<<8) | 8) :  8 bit 2 tap   Base    //
				    // (2<<8) | 10):  10 bit 2 tap  Base    //
				    // (2<<8) | 12):  12 bit 2 tap  Base    //
				    // (8<<8) | 8) :  8 bit 8 tap   Full    //
				    // (8<<8) | 10):  10 bit 8 tap  Full    //

    int 	    videoMode;	    // 'c': free-run mode		    //
				    // 'g'^'s': controlled global trigger   //

    int 	    testData;	    // 0: Disabled			    //
				    // 1: Solid Color			    //
				    // 2: 100% Color Bar		    //
				    // 3: Faded Color Bar		    //

    int 	    subsample;	    // encoded as 0xHHVV		    //
				    // 0x0101: none			    //
				    // 0x0102: 1x2 (HxV)		    //
				    // 0x0104: 1x4			    //
				    // 0x0108: 1x8			    //
				    // 0x0201: 2x1			    //
				    // 0x0202: 2x2			    //
				    // 0x0204: 2x4			    //
				    // 0x0208: 2x8			    //
				    // 0x0401: 4x1			    //
				    // 0x0402: 4x2			    //
				    // 0x0404: 4x4			    //
				    // 0x0408: 4x8			    //

    int 	    subbinning;     // Bit 0=1: enable vertical summing     //
				    // Bit 8=1: enable horizontal binning   //

    int 	    scanDirection;  // CC('L', 'T'): R-L/T-B		    //
				    // CC('L', 'B'): R-L/B-T		    //
				    // CC('R', 'T'): L-R/T-B		    //
				    // CC('R', 'B'): L-R/B-T		    //

    int 	    triggerSource;  // 't': 'Trigger In'		    //
				    // '1': CameraLink CC1		    //

    int 	    triggerPolarity;// '+': rising edge trigger 	    //
				    // '-': falling edge trigger	    //

    double	    anagainrgb[4];  // -6 dB thru 17.5 dB		    //
				    // values below 0 dB not recommended    //

    double	    diggainrgb[4];  // 0 dB thru 12 dB			    //
     *
     */

};

typedef struct PXCam_SV1820CLState     PXCam_SV1820CLState_s;
#define PXMOS_CAMSV1820CLSTATE	       (PXMOS_DDCH+PXMOS_CAMSVCLASS15STATE+(3+3)+(4+3+3)+(7+1+12))

_cDcl(_dllpxlib,_cfunfcc, int)
PXCam_SILICONVIDEO1820CL(int cmnd, int rsvd, int unitmap, int porttype,
		       void *port, PXCam_SV1820CLState_s *camstatep,
		       PXCam_SV1820CLState_s *camstate2p, pxvidstate_s *vidstatep);




#ifdef  __cplusplus
}
#endif

#include "cext_hpe.h"     
#endif				/* !defined(__EPIX_PXLIBCAM_DEFINED) */

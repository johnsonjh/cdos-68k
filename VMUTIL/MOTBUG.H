
/*	@(#)motbug.h	1.4		*/

/* buffer for VME/10 bootrecord */

#define VOLID	struct VolumeId

VOLID
{
	BYTE name[4];		/* The magic "CDOS" string */
	BYTE _vfill1[0x10];
	LONG vsector;		/* first versados sector to transfer */
	WORD vsects;		/* number of versados sectors */
	BYTE _vfill2[4];
	LONG dest;		/* destination address */
	BYTE _vfill5[4];
	BYTE description[20];	/* decriptive string */
	BYTE _vfill3[0x6];
	LONG diag[16];		/* diagnostic test pattern */
	BYTE _vfill3a[0x10];
	LONG dcasect;		/* address of disk configuration area */
	BYTE dcalen;		/* length of configuration area */
	BYTE _vfill4[0x63];
	BYTE exormacs[8];	/* exormacs string */
};

#define DCA	struct DataConfigurationAttr

DCA
{
	BYTE fill4dca[8];
	WORD dca_attr;		/* attributes */
	BYTE _fill5dca[14];
	BYTE dca_spt;		/* sectors per track */
	BYTE dca_heads;
	WORD dca_cylinders;	/* number of cylinders on medium */
	WORD _fill6dca;
	WORD dca_bps;		/* bytes per sector */
	WORD _fill7dca[2];
	WORD dca_precomp;	/* starting precompensation cylinder */
};


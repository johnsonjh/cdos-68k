/********************************************************************
*	LOAD68K.H - structures for CP/M load files
********************************************************************/

/********************************************************************
*
*  CP/M-68K load file header structure
*
********************************************************************/

#define	HEADER struct header
HEADER {
    WORD h_flag;	    /* 601A or 601C */
    LONG h_tsize;	    /* size of text segment */
    LONG h_dsize;	    /* size of data segment */
    LONG h_bsize;	    /* size of BSS segment */
    LONG h_ssize;	    /* size of symbol table */
    LONG h_stack;	    /* size of stack */
    LONG h_start;	    /* start of	text seg and program entry point */
    WORD h_reloc;	    /* relocation flag - 0 if relocatable */
};


/********************************************************************
*
*	CP/M-68K base page format
*
********************************************************************/

#define	BASPAG struct baspag
BASPAG {
    LONG b_lotpa;	    /* lowest address of TPA */
    LONG b_hitpa;	    /* highest address of TPA +	1 */
    LONG b_tstart;	    /* start of	text segment */
    LONG b_tsize;	    /* size of text segment */
    LONG b_dstart;	    /* start of	data segment */
    LONG b_dsize;	    /* size of data segment */
    LONG b_bstart;	    /* start of	bss segment */
    LONG b_bsize;	    /* length of bss */
    LONG b_free;	    /* length of free memory after bss */
    BYTE b_drive;	    /* drive from which	pgm was	loaded */
    BYTE b_junk1[3];	    /* filler to make stack on LONG boundary */
    LONG b_stack;	    /* start of	stack segment */
    BYTE b_junk2[12];	    /* more filler */
    BYTE b_fcb2[36];	    /* 2nd fcb from cmd	line */
    BYTE b_fcb1[36];	    /* 1st fcb from cmd	line */
    BYTE b_dma[128];	    /* command tail */
};

/*	coff.h - structures used in common object file format files
*
* WHEN		WHO	WHAT
* ====		===	====
* 12/17/85	MA	Added a_ssize (stack size) field to a.out header
*
*/

#define FILEHDR struct _filehdr		/* file header */
#define	AOUTHDR struct _aouthdr		/* optional a.out header */
#define SCNHDR	struct _scnhdr		/* section header */
#define RELOC	struct _reloc		/* relocation information */
#define SYMENT	struct _syment		/* symbol table entry */

#define	FHDRSIZE (LONG)sizeof(FILEHDR)
#define AHDRSIZE (LONG)sizeof(AOUTHDR)
#define SHDRSIZE (LONG)sizeof(SCNHDR)

FILEHDR {
	WORD	f_magic;	/* magic number */
	WORD	f_nscns;	/* number of sections */
	LONG	f_timdat;	/* time & date stamp */
	LONG	f_symptr;	/* file ptr to symtab */
	LONG	f_nsyms;	/* # of symtab entries */
	UWORD	f_opthdr;	/* sizeof (optional a.out header) */
	UWORD	f_flags;	/* flags */
} ;

AOUTHDR {
	WORD	a_magic;	/* magic number */
	WORD	a_vstamp;	/* version stamp */
	LONG	a_tsize;	/* text size in bytes, padded (.text) */
	LONG	a_dsize;	/* initialized data (.bss) */
	LONG	a_bsize;	/* uninitialized data (.bss) */
	LONG	a_entry;	/* entry point */
	LONG	a_tstart;	/* base of text used for this file */
	LONG	a_dstart;	/* base of data used for this file */
	LONG	a_relptr;	/* pointer to load-time relocation info */
	LONG	a_ssize;	/* stack size */
} ;

SCNHDR {
	BYTE	s_name[8];	/* section name */
	LONG	s_paddr;	/* physical address */
	LONG	s_vaddr;	/* virtual address */
	LONG	s_size;		/* section size */
	LONG	s_scnptr;	/* file ptr to raw data */
	LONG	s_relptr;	/* file ptr to relocation info */
	LONG	s_lnnoptr;	/* file ptr to line numbers */
	UWORD	s_nreloc;	/* no. of relocation entries */
	UWORD	s_nlnno;	/* no. of line number entries */
	LONG	s_flags;	/* flags */
} ;

/* flags in section header */

#define STYP_TEXT	0x20	/* section contains only executable text */
#define STYP_DATA	0x40	/* section contains only initialized data */
#define STYP_BSS	0x80	/* section contains only unitialized data */

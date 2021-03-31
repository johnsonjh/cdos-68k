/*  COFF (24 MAR 86) - convert CP/M-68K crunched .68k file to a COFF file */

/*  usage:
	coff <input file> <output file>
*/

#include "portab.h"
#include "load68k.h"
#include "coff.h"


/*  CDOS error codes */

#define	E_EOF	0x4003


/*  external routines in XIO.C */

EXTERN LONG xopen(),xcreat(),xread(),xwrite(),xseek();


/*  global variables */

HEADER hdr;			/* CP/M-68K command file header */

FILEHDR fhdr;			/* COFF file header */
AOUTHDR ahdr;			/* COFF optional a.out header */
SCNHDR	shdr;			/* COFF section header */
LONG	hdrsize;		/* sum of all coff headers (inc. 3 SCNHDRS) */
LONG	ret;			/* CDOS return code */

BYTE buffer[0x4000];		/* I/O buffer */

LONG
    actual,			/* actual no. of bytes read from file */
    fdin, fdout;		/* object file descriptors */


/* main	program	*/

main(argc,argv)
WORD argc;
BYTE *argv[];
{
    WORD i;

    if (argc !=	3)
	error("Usage: coff <input file> <output file>");

    fdin = xopen(argv[1],0);		/* open	the object file	for read */
    if ((ret = fdin) < 0)		/* failure to open file	*/
	lerror("Unable to open input file.");

    actual = xread(fdin,&hdr,(LONG)sizeof(hdr));

    if (hdr.h_flag != 0x601c)
	error("File is not in crunched .68K format.");
    if (hdr.h_flag == 0x150)
	error("File is already in COFF format.");
    if (hdr.h_reloc)
	error("File is not relocatable");

    fdout = xcreat(argv[2],0);		/* create output file */
    if ((ret = fdout) < 0)
	lerror("Unable to create output file.");

    makecoff();				/* write out the coff file */

    xclose(fdin);
    xclose(fdout);
    s_exit(0L);
}


/* error - print an error message and exit to shell */

error(s)
BYTE *s;
{
    printf(s);
    s_exit(1L);
}


/* lerror - print the global long variable "ret" in hex, then
	an error message, then exit to shell
*/

lerror(s)
BYTE *s;
{
    printf("Error %lx: ",ret);
    error(s);
}


/* makecoff - write the coff file */

makecoff()
{

/* set up the main header */

    fhdr.f_magic = 0x150;
    fhdr.f_nscns = 3;
    fhdr.f_timdat = 0;
    fhdr.f_symptr = 0;
    fhdr.f_nsyms = 0;
    fhdr.f_opthdr = sizeof(ahdr);

/* set the main header flags for no UNIX relocs, executable, no line nos.,
   no local symbols, and non-DEC host */

    fhdr.f_flags = 0x20f;
    if ((ret = xwrite(fdout,&fhdr,(LONG)sizeof(fhdr))) < sizeof(fhdr))
	lerror("Unable to write coff header.");

/* set up the a.out header and write it out */

    ahdr.a_magic = 0;
    ahdr.a_vstamp = 0;
    ahdr.a_tsize = hdr.h_tsize;
    ahdr.a_dsize = hdr.h_dsize;
    ahdr.a_bsize = hdr.h_bsize;
    ahdr.a_entry = 0;
    ahdr.a_tstart = 0;
    ahdr.a_dstart = hdr.h_tsize;
    hdrsize = FHDRSIZE + AHDRSIZE + 3*(SHDRSIZE);
    ahdr.a_relptr = hdrsize + hdr.h_tsize + hdr.h_dsize;
    ahdr.a_ssize = hdr.h_stack;
    if ((ret = xwrite(fdout,&ahdr,(LONG)sizeof(ahdr))) < sizeof(ahdr))
	lerror("Unable to write a.out header.");

/* set and write the text section header */

    clear(&shdr,sizeof(shdr));
    strcpy(shdr.s_name,".text");
    shdr.s_size = hdr.h_tsize;
    shdr.s_scnptr = hdrsize;
    shdr.s_flags = STYP_TEXT;
    if ((ret = xwrite(fdout,&shdr,(LONG)sizeof(shdr))) < sizeof(shdr))
	lerror("Unable to write text section header.");

/* set and write the data section header */

    clear(&shdr,sizeof(shdr));
    strcpy(shdr.s_name,".data");
    shdr.s_paddr = shdr.s_vaddr = hdr.h_tsize;
    shdr.s_size = hdr.h_dsize;
    shdr.s_scnptr = hdrsize + hdr.h_tsize;
    shdr.s_flags = STYP_DATA;
    if ((ret = xwrite(fdout,&shdr,(LONG)sizeof(shdr))) < sizeof(shdr))
	lerror("Unable to write data section header.");

/* set and write the bss section header */

    clear(&shdr,sizeof(shdr));
    strcpy(shdr.s_name,".bss");
    shdr.s_paddr = shdr.s_vaddr = hdr.h_tsize + hdr.h_dsize;
    shdr.s_size = hdr.h_bsize;
    shdr.s_flags = STYP_BSS;
    if ((ret = xwrite(fdout,&shdr,(LONG)sizeof(shdr))) < sizeof(shdr))
	lerror("Unable to write bss section header.");

/* copy the text and data sections */

    if (!copyfile(hdr.h_tsize + hdr.h_dsize))
	lerror("Unable to copy text and data sections.");

/* copy the crunched relocation information */

    if (!copyrel())
	lerror("Unable to copy relocation information.");
}


/* copyfile - copy data verbatim from input file to output file */

copyfile(nbytes)
LONG nbytes;
{
    LONG actual;

    while (nbytes)
    {
	if (nbytes < sizeof(buffer))
	    actual = nbytes;
	else actual = sizeof(buffer);
	actual = xread(fdin,buffer,actual);
	if ((ret = actual) < 0) return (FALSE);
	if ((ret = xwrite(fdout,buffer,actual)) < 0)
	    return (FALSE);
	nbytes -= actual;
    }
    return (TRUE);
}


/* copyrel - copy crunched reloc info from input file to output file */

copyrel()
{
    LONG actual;

    ret = xseek(fdin,(LONG)sizeof(hdr)+hdr.h_tsize+hdr.h_dsize+hdr.h_ssize,0);
    if (ret < 0)
	lerror("Unable to seek to reloc info in input file.");

    for (;;)
    {

    /* read some more reloc info */

	actual = xread(fdin,buffer,actual);
	if ((ret = actual) < 0)
	    return ((ret & 0xffffL) == E_EOF);
	if (actual == 0) return (TRUE);

    /* write the reloc info to the output file */

	if ((ret = xwrite(fdout,buffer,actual)) < actual) return (FALSE);
    }
    return (TRUE);
}


/* clear - zero out a block */

MLOCAL clear(s,size)
BYTE *s;
WORD size;
{
    while (size--)
	*s++ = 0;
}

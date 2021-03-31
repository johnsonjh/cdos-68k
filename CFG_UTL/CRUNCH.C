/*  CRUNCH (24 Mar 86) - shrink reloc info in .68k file */

/*  usage:
	crunch <input file> <output file>
*/

#include "portab.h"
#include "load68k.h"


/*  external functions in XIO.C */

EXTERN LONG xopen(),xcreat(),xread(),xwrite();


/*  global variables */

HEADER hdr;

BYTE buffer[0x4000];		/* I/O buffer */

WORD
    relitems,relbytes;		/* number of reloc items and reloc bytes */

LONG
    actual,			/* actual no. of bytes read from file */
    ret,			/* return code from CDOS */
    fdin, fdout;		/* object file descriptors */


/* main	program	*/

main(argc,argv)
WORD argc;
BYTE *argv[];
{
    WORD i;

    if (argc !=	3)
    {
	printf("Usage: crunch <input file> <output file>");
	s_exit(1L);
    }
    fdin = xopen(argv[1],0);		/* open	the object file	for read */
    if (fdin < 0)			/* failure to open file	*/
    {
	printf("Error %lx opening %s",fdin,argv[1]);
	s_exit(1L);
    }
    actual = xread(fdin,&hdr,(LONG)sizeof(hdr));
    if (hdr.h_flag != 0x601a)
    {
	printf("Header flag is not 601A");
	s_exit(1L);
    }
    if (hdr.h_reloc)
    {
	printf("File is not relocatable");
	s_exit(1L);
    }
    fdout = xcreat(argv[2],0);		/* create output file */
    if (fdout < 0)
    {
	printf("Error %lx creating %s",fdout,argv[2]);
	s_exit(1L);
    }
    hdr.h_flag = 0x601c;
    if ((actual = xwrite(fdout,&hdr,(LONG)sizeof(hdr))) < 0)
    {
	printf("Error %lx writing header to output file",actual);
	s_exit(1L);
    }
    if (!copyfile())
	s_exit(1L);
    if (!shrink()) s_exit(1L);
    xclose(fdin);
    xclose(fdout);
    printf("Code+Data size: %ld, Relocation items: %d, Relocation bytes: %d",
		hdr.h_tsize+hdr.h_dsize,relitems,relbytes);
    s_exit(0L);
}


/* copyfile - copy text, data, and symbols to output file */

copyfile()
{
    LONG nbytes;
    LONG actual;

    nbytes = hdr.h_tsize + hdr.h_dsize + hdr.h_ssize;
    while (nbytes)
    {
	if (nbytes < sizeof(buffer)) actual = nbytes;
	else actual = sizeof(buffer);
	actual = xread(fdin,buffer,actual);
	if (actual < 0)
	{
	    printf("Error %lx reading from input file",actual);
	    return (FALSE);
 	}
	actual = xwrite(fdout,buffer,actual);
	if (actual < 0)
	{
	    printf("Error %lx writing to output file",actual);
	    return (FALSE);
	}
	nbytes -= actual;
    }
    return (TRUE);
}


/* shrink - copy relocation info to output file, shrinking it */

shrink()
{
    LONG nbytes,addr,fixaddr,prev,actual;
    REG LONG i,di;
    BYTE wflag;
    union {
	BYTE dispb[4];
	LONG displ;
    } d;

    wflag = 0x80;
    addr = prev = 0;
    nbytes = hdr.h_tsize + hdr.h_dsize;
    while (nbytes)
    {
	if (nbytes < sizeof(buffer)) actual = nbytes;
	else actual = sizeof(buffer);
	actual = xread(fdin,buffer,actual);
	if (actual < 0)
	{
	    printf("Error %lx reading reloc info from input file",actual);
	    return (FALSE);
	}
	for (i = 1; i < actual; i += 2)		/* i must be odd to index */
	{					/*  low byte of reloc word */
	    switch(buffer[i] & 7)
	    {
		case 0:		/* absolute */
		case 7:		/* opcode */
		    wflag = 0x80;
		    break;
		case 1:		/* data */
		case 2:		/* text */
		case 3:		/* bss  */
		    if (wflag)
			fixaddr = addr;			/* word reloc */
		    else
			fixaddr = addr - 2;		/* long reloc */

		    if ((d.displ = fixaddr - prev) >= 0x10000L)
		    {					/* long extension */
			wflag |= 127;
			di = 0;
		    }
		    else if (d.displ >= 0x100)		/* word extension */
		    {
			wflag |= 126;
			di = 2;
		    }
		    else if (d.displ >= 125 || d.displ == 0)
		    {					/* byte extension */
			wflag |= 125;
			di = 3;
		    }
		    else				/* no extension */
		    {
			wflag |= d.dispb[3];
			di = 4;
		    }

		/* write relocation byte */
		    if ((ret = xwrite(fdout,&wflag,1L)) < 0)
		    {
			printf("Error %lx writing relocation byte",ret);
			return (FALSE);
		    }
		/* write extension bytes if necessary */
		    if (di < 4)
			if ((ret = xwrite(fdout,&d.dispb[di],4L-di)) < 0)
			{
			    printf("Error %lx writing extension bytes",ret);
			    return (FALSE);
			}

		    wflag = 0x80;
		    prev = fixaddr;
		    relitems += 1;
		    relbytes += 5-di;
		    break;
		case 5:		/* upper word of long */
		    wflag = 0;
		    break;
		case 4:
		    printf("Undefined symbol");
		    return(FALSE);
		case 6:
		    printf("PC relative reference");
		    return(FALSE);
	    }
	    addr += 2;
	}
	nbytes -= actual;
    }
    wflag = 0;			/* reloc info terminator */
    ret = xwrite(fdout,&wflag,1L);
    if (ret < 0)
    {
	printf("Error %lx writing reloc info terminator",ret);
	return (FALSE);
    }
    else return (TRUE);
}

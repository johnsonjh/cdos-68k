/*  ss.c - change or display stack size field in .68k file header */

/*  usage:
	ss [-s<stack size>] <filename> ...

    where
	<stack size> in hex to be inserted into file header
	<filename> is the name of a .68k program file

    If the stack size argument is missing, the current stack size is
    displayed but not changed.
*/

#include "portab.h"
#include "load68k.h"
#include "coff.h"


/*  external functions in XIO.C */

EXTERN LONG xopen(),xread(),xwrite(),xseek();


/* disk file table structure - see DISKFILE Table in Programmer's Guide */

MLOCAL struct {
    LONG d_key;
    BYTE d_name[18];
    WORD d_attrib;
    WORD d_recsize;
    BYTE d_user;
    BYTE d_group;
    WORD d_protect;
    WORD d_rsvd[3];
    LONG d_size;
    BYTE d_time[8];
} dftab;


/* disk table - used to find out cluster size on destination disk */
/*  global variables */

HEADER header;			/* CP/M file header */
FILEHDR fhdr;			/* COFF file header */
AOUTHDR ahdr;			/* COFF a.out header */

WORD
    soption,			/* s option specified */
    argnum;			/* current file index */

LONG
    ret,			/* return code from CDOS */
    actual,			/* actual no. of bytes read from file */
    fd;				/* object file descriptor */

BYTE
    pathname[128],		/* expanded pathname */
    **argvec;			/* copy of argv */

LONG
    stksize;			/* desired stack size */

/* main	program	*/

main(argc,argv)
WORD argc;
BYTE *argv[];
{
    argvec = argv;
    if (argc ==	1)
    {
	printf("Usage: ss [-s<stacksize>] <filename>...");
	s_exit(1L);
    }
    for	(argnum	= 1; argnum < argc && *argv[argnum] == '-'; argnum++)
	getarg(argv[argnum]+1);
    for	( ; argnum < argc; argnum++) dowild(argv[argnum]);
    s_exit(0L);
}


/*  GETARG - get a parameter from the command line, perform desired action

    Possible parameters:
	-s<stack size>
    where <stacksize> is a hexadecimal number.
*/

getarg(p)
BYTE *p;
{
    BYTE c;

    c =	*p++;
    if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
    switch (c)
    {
	case 's':
	    getsize(p);
	    break;
	default:
	    printf("Illegal option letter");
	    s_exit(1L);
    }
}


/*  GETSIZE - get a "s" stack size parameter from command line */
    
getsize(p)
BYTE *p;
{
    LONG c;

    stksize = 0;
    for	( ; *p;	++p)
    {
 	if ((c = *p & 0xff) >= 'a')
	    c -= 'a' - 'A';
	if (c >= '0' && c <= '9' )
	    c -= '0';
 	else if (c >= 'A' && c <= 'F')
	    c -= 'A' - 10;
	else
	{
	    printf("Illegal stack size specified");
	    s_exit(1L);
	}
	stksize = (stksize << 4) + c;
    }
    soption = 1;
}


/*  dowild - for each file in a wild card specification,
	call setsize for that file.
*/

dowild(nameptr)
BYTE *nameptr;
{
    for (;;)
    {

    /* separate the drive:path/ portion of the filename */

	getpath(pathname,nameptr);

    /* look up the file */

	ret = s_lookup(0x20,0,nameptr,&dftab,(LONG)sizeof(dftab),
			(LONG)sizeof(dftab),dftab.d_key);
	if (ret != 1)
	{
	    if (ret < 0)
		printf("S_LOOKUP error: %lx\r\n",ret);
	    return (FALSE);	/* no more files */
	}
	strcat(pathname,dftab.d_name);
	setsize(pathname);
    }
}


/*  getpath - copy the drive:/path/ portion of a filename */

getpath(dst,src)
BYTE *dst,*src;
{
    BYTE *end,c;

    end = dst;
    do			/* while not at end of string */
    {
	c = (*dst++ = *src++);
	if (c == ':' || c == '/' || c == '\\')
	    end = dst;	/* save location of last path separator */
    } while (c);
    *end = 0;
}


/*  setsize - open an object file, read the header, display the
	stack size, possible set new stack size.
*/

setsize(nameptr)
BYTE *nameptr;
{
    fd = xopen(nameptr,2);		/* open	the object file	for read */
    if (fd < 0)				/* failure to open file	*/
    {
	printf("Error %lx opening %s\r\n",fd,nameptr);
	return;
    }
    actual = xread(fd,&header,(LONG)sizeof(header));
    if (actual < 0 || actual != sizeof(header))
    {
	printf("Error %lx reading header of %s\r\n",actual,nameptr);
	return;
    }
    if (header.h_flag == 0x150)		/* COFF? */
	setcoff(fd,nameptr);
    else if (header.h_flag == 0x601a || header.h_flag == 0x601c)  /* CP/M */
    {
	if (soption)
	{
	    header.h_stack = stksize;
	    if ((ret = xseek(fd,0L,0)) < 0L)	/* seek	to start of header */
	    {
		xclose(fd);
		printf("Error %lx seeking to start of in %s\r\n",ret,nameptr);
		return;
	    }
	    if ((ret = xwrite(fd,&header,(LONG)sizeof(header))) < 0)
	    {
		xclose(fd);
		printf("Error %lx writing header of %s\r\n",ret,nameptr);
		return;
	    }
	    printf("%s: stack size set to %lx\r\n",nameptr,stksize);
	}
	else
	    printf("%s: stack size is %lx\r\n",nameptr,header.h_stack);
    }
    else
	printf("Invalid magic header word in %s\r\n",nameptr);
    xclose(fd);
}

setcoff(fd,nameptr)
LONG fd;
BYTE *nameptr;
{
    if ((ret = xseek(fd,0L,0)) < 0L)	/* seek	to start of header */
    {
	printf("Error %lx seeking to start of %s\r\n",ret,nameptr);
	return;
    }
    if ((ret = xread(fd,&fhdr,(LONG)sizeof(fhdr))) != sizeof(fhdr))
    {
	printf("Error %lx reading COFF file header in %s\r\n",ret,nameptr);
	return;
    }
    if (fhdr.f_opthdr != AHDRSIZE)
    {
	printf("%s: a.out header does not have stack size field\r\n",
	    nameptr);
	return;
    }
    if ((ret = xread(fd,&ahdr,(LONG)sizeof(ahdr))) != sizeof(ahdr))
    {
	printf("Error %lx reading a.out header in %s\r\n",ret,nameptr);
	return;
    }
    if (soption)
    {
	ahdr.a_ssize = stksize;
	if ((ret = xseek(fd,FHDRSIZE,0)) < FHDRSIZE)	/* seek to a.out header */
	{
	    printf("Error %lx seeking to a.out header in %s\r\n",ret,nameptr);
	    return;
	}
	if ((ret = xwrite(fd,&ahdr,(LONG)sizeof(ahdr))) != sizeof(ahdr))
	{
	    printf("Error %lx writing a.out header in %s\r\n",ret,nameptr);
	    return;
	}
	printf("%s: stack size set to %lx\r\n",nameptr,stksize);
    }
    else
	printf("%s: stack size is %lx\r\n",nameptr,ahdr.a_ssize);
}

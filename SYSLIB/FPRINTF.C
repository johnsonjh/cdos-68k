/*************** fprintf.c *********************************************
* WHEN		WHO	WHAT
* ====		===	====
* 2/13/85	MA	Added printf routine for front end.
* 5/15/85	LT	changed word parm to a long parm in doval
* 6/04/85	LT	Added code to fprintf to allow ".xx" digit
			string formatting.
***********************************************************************/

#include "portab.h"

EXTERN	LONG	s_write();

#define BUFFER struct _bufBYTE
BUFFER
{
	WORD b_type;
	WORD b_cnt;
	BYTE *b_buff;
	LONG b_fn;
};

LONG _goout(buf,cnt,opbuf)
    BYTE *buf;
    LONG cnt;
    BUFFER *opbuf;
{
    WORD i;

	i = (WORD)cnt;
	while (i--)
	{
	    (opbuf->b_buff)[opbuf->b_cnt++] = *buf++;
	    if ((opbuf->b_type) && (opbuf->b_cnt == 126))
	    {
		s_write(0x200,opbuf->b_fn,opbuf->b_buff,
					(LONG)(opbuf->b_cnt),0L);
		opbuf->b_cnt = 0;
	    }
	}
}

_goflush(opbuf)
    BUFFER *opbuf;
{
	if (opbuf->b_type)
	    s_write(0x200,opbuf->b_fn,opbuf->b_buff,(LONG)(opbuf->b_cnt),0L);
	else
	    (opbuf->b_buff)[opbuf->b_cnt] = 0;
	opbuf->b_cnt = 0;
}

BYTE	hexdig[] = { "0123456789ABCDEF" } ;
WORD	*doval();

sprintf( fn,f,parms)  /* print routine, format string, parms */
BYTE *fn;
BYTE *f;
WORD parms;
{
    BUFFER obuf;

	obuf.b_cnt = 0;
	obuf.b_type = 0;
	obuf.b_buff = fn;
	obuf.b_fn = 0L;

	ffprintf(&obuf,f,&parms);
}

fprintf( fn,f,parms)  /* print routine, format string, parms */
LONG fn;
BYTE *f;
WORD parms;
{
	BUFFER obuf;
	BYTE tbuff[126];

	obuf.b_cnt = 0;
	obuf.b_type = 1;
	obuf.b_buff = tbuff;
	obuf.b_fn = fn;

	ffprintf(&obuf,f,&parms);
}

printf(f,parms)  /* print routine, format string, parms */
BYTE *f;
WORD parms;
{
	BUFFER obuf;
	BYTE tbuff[126];

	obuf.b_cnt = 0;
	obuf.b_type = 1;
	obuf.b_buff = tbuff;
	obuf.b_fn = 1L;		/* standard output file */

	ffprintf(&obuf,f,&parms);
}

ffprintf(obuf,f,parms)
    BUFFER *obuf;
    BYTE *f;
    WORD *parms;
{
	BYTE c,*s,*s1,tc;
	WORD *pb;
	WORD ldigit,lldigit,ndigit,i;
	/* flags for zero fill, left justify, long op */
	WORD zf,jf,lf;

	pb = parms;
	while (c = *f++)
		if (c == '%')
		{
			ldigit = 0xffff;
			ndigit = lf = zf = jf = 0;
			c = *f++;
			if (c == '-') { jf = 1; c = *f++; }
			if (c == '0') { zf = 1; c = *f++; }
			while ((c >= '0') && (c <= '9'))
			{
				ndigit = ndigit*10 + (c - '0');
				c = *f++;
			}
			if ( c == '.' ) { ldigit = 0; c = *f++; }
			while (( c >= '0' ) && ( c <= '9' ))
			{
				ldigit = ldigit*10 + (c - '0');
				c = *f++;
			}
			lldigit = ldigit;
			if (c == 'l') { lf = 1; c = *f++; }
			switch (c)
			{
			  case 'x':
				pb = doval(pb,16,jf,zf,lf,ndigit,ldigit,obuf);
				break;
			  case 'o':
				pb = doval(pb, 8,jf,zf,lf,ndigit,ldigit,obuf);
				break;
			  case 'd':
				pb = doval(pb,10,jf,zf,lf,ndigit,ldigit,obuf);
				break;
			  case 'c':
				tc = (BYTE)(*pb++);
				if (!jf) prblk(ndigit-1,' ',obuf);
				_goout(&tc,1L,obuf);
				if (jf) prblk(ndigit-1,' ',obuf);
				break;
			  case 's':  
				s = s1 = *((BYTE **) pb);
				pb += sizeof(BYTE *)/2;
				for (i=0; *s1++ && ldigit--; i++);
				if (!jf) prblk(ndigit-i,' ',obuf);
/*	
				while (c = *s++) _goout(&c,1L,obuf);
	new printf below	*/
				while (( c = *s++) && (lldigit--))
					_goout(&c,1L,obuf);
				if (jf) prblk(ndigit-i,' ',obuf);
				break;
			}
		}
		else _goout(&c,1L,obuf);
	_goflush(obuf);
}

prblk(n,ch,obuf)
WORD n;
BYTE ch;
BUFFER *obuf;
{
	for( ; n > 0; n-- )
		_goout(&ch,1L,obuf);
}

WORD *doval(pb,bs,jf,zf,lf,ndigit,ldigit,obuf)
BYTE *pb;
WORD bs,jf,zf,lf,ndigit,ldigit;
BUFFER *obuf;
{
	WORD i,fillc,mf,nf;
	LONG lstak[20];
	LONG base;
	base = bs;
	mf = nf = 0; /* minus flag = 0 */
	if (lf)
	{
		lstak[0] = *((LONG *) pb);
		pb += sizeof(LONG);
	}
	else
	{
		lstak[0] = *((WORD *) pb);
		pb += sizeof(WORD);
	}
	if (lstak[0] < 0)
	{
		nf = 1;	/* negative flag */
		if (bs == 10)
		{
			mf = 1; 
			/* fix for bug in divide */
			lstak[0] = -lstak[0];
		}
		else lstak[0] = ~lstak[0];
	}
	for (i = 0; lstak[i]; i++) lstak[i+1] = lstak[i]/bs;
	if (!i) i++;
	fillc = ndigit - i;
	if (nf & !mf) prblk(fillc,hexdig[bs-1],obuf);
	else
	{
	  if (mf) { fillc--; if (zf) _goout("-",1L,obuf); }
	  if (!jf) prblk(fillc,(zf ? '0' : ' '),obuf);
	  if (mf && !zf) _goout("-",1L,obuf);
	}
	if ( !ldigit ) ldigit = i ;
	for ( ; i && ldigit; i--, ldigit--)
	  if (bs == 10) _goout(&hexdig[(WORD)(lstak[i-1] % 10)],1L,obuf);
	  else 
	  {
	      if (nf)
		_goout(&hexdig[(WORD)(bs-1-(lstak[i-1] % base))],1L,obuf);
	      else
		_goout(&hexdig[(WORD) (lstak[i-1] % base)],1L,obuf);
	  }		
	if (jf) prblk(fillc,' ',obuf);
	return(pb);
}
	      else
		_goout(&hexdig[(WORD) (lstak[i-1] % base)],1L,obuf);
	  }		
	if (jf) prblk(fillc,' ',obuf);
	return(p
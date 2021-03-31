
/*****************************************************************
 * "Copyright (C) 1985, Digital Research, Inc.  All Rights       *
 * Reserved.  The Software Code contained in this listing is     *
 * proprietary to Digital Research Inc., Monterey, California    *
 * and is covered by U.S. and other copyright protection.        *
 * Unauthorized copying, adaptation, distribution, use or        *
 * display is prohibited and may be subject to civil and         *
 * criminal penalties.  Disclosure to others is prohibited.  For *
 * the terms and conditions of software code use refer to the    *
 * appropriate Digital Research License Agreement."              *
 *****************************************************************/

/*===============================================================*
 *   Version 1.4        CHAR.H					 *
 *			16 bit Character Definitions		 *
 *---------------------------------------------------------------*
 *    VERSION   DATE    BY      CHANGE/COMMENTS                  *
 *---------------------------------------------------------------*
 *    1.0	3/26/85 jor    	original                         *
 *    1.1	6/11/85	jor	Added 16_bit input char CI_RUB.	 *
 *    1.2	6/20/85	jor	Added 16_bit output char CH_BELL.*
 *    1.3	7/24/85	jor	Added 16_bit input chars for 	 *
 *				numeric keypad.(SPR #382)	 *
 *    1.4	5/29/86 DR-K	define upcurs,delline,revindex	 *
 *===============================================================*/

#define CH_CTRLB	0x0002		/* CTRL B		*/
#define CH_CTRLC	0x0003		/* CTRL C		*/
#define CH_BS		0x0008		/* Backspace (CTRL H)	*/
#define CH_LF		0x000a		/* Line Feed		*/
#define CH_CR		0x000d		/* Carraige Return	*/
#define CH_CTRLQ	0x0011		/* CTRL Q		*/
#define CH_CTRLS	0x0013		/* CTRL S		*/
#define CH_CTRLX	0x0018		/* CTRL X		*/
#define CH_CTRLZ	0x001a		/* CTRL Z		*/
#define CH_ESC		0x001b		/* Escape		*/
#define CH_RUB		0x007f		/* Delete or Rubout	*/
#define	CH_BELL		0x0007		/* BELL			*/
	/* Input Character Set */

#define	CI_HELP		0x2000		/* Help Key		*/
#define CI_WINDOW	0x2001		/* Window Key		*/
#define CI_NEXT		0x2002		/* Next Key		*/
#define CI_PREV		0x2003		/* Previous Key		*/
#define CI_RUB		0x200a		/* DELETE/RUBOUT	*/
#define CI_LEFT    	0x2012		/* Left Cursor		*/
#define CI_RIGHT	0x2013		/* Right Cursor		*/

	/* Numeric Keypad Input Character Set */

#define CI_ZERO		0x2030		/* Numeric Zero		*/
#define CI_ONE		0x2031		/* Numeric One		*/
#define CI_TWO		0x2032		/* Numeric Two		*/
#define CI_THREE	0x2033		/* Numeric Three	*/
#define CI_FOUR		0x2034		/* Numeric Four		*/
#define CI_FIVE		0x2035		/* Numeric Five		*/
#define CI_SIX		0x2036		/* Numeric Six		*/
#define CI_SEVEN	0x2037		/* Numeric Seven	*/
#define CI_EIGHT	0x2038		/* Numeric Eight	*/
#define CI_NINE		0x2039		/* Numeric Nine		*/
#define CI_A		0x203A		/* 'A'			*/
#define CI_B		0x203B		/* 'B'			*/
#define CI_C		0x203C		/* 'C'			*/
#define CI_D		0x203D		/* 'D'			*/
#define CI_E		0x203E		/* 'E'			*/
#define CI_F		0x203F		/* 'F'			*/
#define CI_ENTER	0x2040		/* Enter 		*/
#define CI_COMMA	0x2041		/* Comma   (',')	*/
#define CI_MINUS	0x2042		/* Minus   ('-')	*/
#define CI_PERIOD	0x2043		/* Period  ('.')	*/
#define CI_PLUS		0x2044		/* Plus	   ('+')	*/
#define CI_DIVIDE	0x2045		/* Divide  ('/')	*/
#define CI_MULT		0x2046		/* Multiply('*')	*/
#define CI_EQUAL	0x2047		/* Equal   ('=')	*/

	/* Output Character Set */

#define CO_EICH		0x2040		/* Enter Insert Char	*/
#define CO_EEOL		0x204b		/* Erase to End of Line	*/
#define CO_DELCH	0x204e		/* Delete Character	*/
#define CO_XICH		0x204f		/* Exit Insert Char	*/
#define CO_MOVCC	0x4000		/* Move Cursor Column	*/
#define CO_REVINDX	0x2049		/* Reverse Index	*/
#define CO_UPCURS	0x2041		/* Cursor Up		*/
#define CO_DELLINE	0x204d		/* Delete Entire Line	*/
	0x2049		/* Reverse Index	*/
#define CO_UPCURS	0x2041		/* Cursor Up		*/
#define CO_DELLINE	0x204d		/* Delete Ent
 Line	*/
	0x2049		/* Reverse Index	*/
#define CO_UPCURS	0x2041		/* Cursor Up		*/
#define CO_DELLINE	0x204d		/* Delete Ent
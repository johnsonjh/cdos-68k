/* Utility Return Codes */

/****************************************************************/
/*								*/
/*	Utility Return codes follow the same basic format of    */
/*	DOS return codes, with the following exceptions:	*/
/*								*/
/*	   All error codes are positive numbers (LONGS) since   */
/*	   the high bit (31) is always zero (0).		*/
/*								*/
/*	   The error code field should use one of the following */
/*	   listed codes when possible.				*/
/*								*/
/*	Utility return code format:				*/
/*								*/
/*      31|30   24|23    16|15             0 (bits)		*/
/*	+-+-------+--------+----------------+			*/
/*	|0|   0   | module |   Error Code   |			*/
/*	+-+-------+--------+----------------+			*/
/*	(high byte)		(low word)			*/
/*								*/
/*	Error Code	16 bit number as described below	*/
/*			(NOTE: these are all cast as WORD)	*/
/*	Module		8 bit number indicating source Module	*/
/*			This value may be used to designate	*/
/*			certain modules within a utility.	*/
/*	High Byte	high bit is 0, low 7 bits are reserved	*/
/*								*/
/*	Applications return errors that they generated		*/
/*	by ORing the Module with the Error Code.		*/
/*	ie.							*/
/*		return( UR_SOURCE | UR_PARM );			*/ 
/* (The Utility is noting that a parameter error was detected)	*/
/*								*/
/*	ALSO:  DON'T OR Module Number with UR_SUCCESS...	*/
/*								*/
/****************************************************************/

#define	UR_SOURCE	(LONG)0			/* Utility Return Module ID  */

#define	UR_SUCCESS	(LONG)0			/* Successful completion     */

#define	UR_PARM		0x0001			/* Parameter error	     */
#define	UR_CONFLICT	0x0002			/* Contention conflict	     */
#define	UR_UTERM	0x0003			/* Terminated by user	     */
#define	UR_FORMAT	0x0004			/* Data structure format err */
#define	UR_INTERNAL	0x0005			/* Internal utility error    */
 */
#define	UR_FORMAT
/**
 * $Id: hash.h,v 1.1 2005/03/03 09:08:48 mihajlov Exp $
 *
 * (C) Copyright IBM Corp. 2004
 * 
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE COMMON PUBLIC LICENSE
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Common Public License from
 * http://oss.software.ibm.com/developerworks/opensource/license-cpl.html
 *
 * Author:       Viktor Mihajlovski <mihajlov@de.ibm.com>
 * Contributors: 
 *
 * Description: Hashtable for Parser Symbol Tables.
 *
 */
 
#if !defined HASHH
#define HASHH

# define RADIX   15
# define MAXHASH (( 1 << RADIX ) - 1)	/* 2^RADIX-1 elements in hash table */

  typedef struct{
  		char * he_key;
		enum { HE_FREE, HE_USED } he_state;
		union {
			void *   ptr;	/* for regular entry */
			int      number;	/* for table head */
		      } he;
		} hashentry;
  
  hashentry * htcreate( const char * );
  void	      htdelete( hashentry * );
  void 	    * htlookup( hashentry *, const char *, unsigned );
  int         htinsert( hashentry *, const char *, unsigned, void * );
  
  unsigned fold(const  char *, unsigned);/* folds string into machine word */
  unsigned primary( unsigned );		/* returns primary hash index */
  unsigned secondary( unsigned );	/* returns secondary hash index */

#endif /* HASHH */

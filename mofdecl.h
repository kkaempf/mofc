/**
 * $Id: mofdecl.h,v 1.1 2005/03/03 09:08:48 mihajlov Exp $
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
 * Description: Declarations needed by Parser and Scanner
 *
 */

#ifndef MOFDECL_H
#define MOFDECL_H

#include <hash.h>
#include <stdio.h>

extern hashentry * current_symtab;
extern hashentry * current_qualtab;
extern FILE * yyin;
extern int yylex(void);
int yyparse(void);
char * upstrdup( const char *, int );
int init_scanner(char * parsefiles[], int numfiles, const char * includedir, 
		 const char * extrafile, int verbose);
void stop_scanner(void);
FILE * try_open_file(const char * filename);
extern int line_number;
extern char * file_name;

#endif

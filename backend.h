/**
 * $Id: backend.h,v 1.1 2005/03/03 09:08:47 mihajlov Exp $
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
 * Description: Common MOF Compiler Backend Declarations
 *
 */

#ifndef BACKEND_H
#define BACKEND_H

#include "symtab.h"

#define BACKEND_DEFAULT   0x0000
#define BACKEND_VERBOSE   0x0001

typedef int (backend_type) (class_chain * cls_chain, const char * outfile, unsigned options, const char * extraopts);

#endif

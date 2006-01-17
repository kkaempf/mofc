/**
 * $Id: backend_sfcb.c,v 1.6 2006/01/17 17:51:05 a3schuur Exp $
 *
 * (C) Copyright IBM Corp. 2004
 * 
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE ECLIPSE PUBLIC LICENSE
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Eclipse Public License from
 * http://www.opensource.org/licenses/eclipse-1.0.php
 *
 * Author:       Viktor Mihajlovski <mihajlov@de.ibm.com>
 * Contributors: 
 *
 * Description: MOF Compiler sfcb Backend
 *              This backend produces Small Footprint CIM Broker
 *              objects for the parsed CIM classes.
 *
 */

#include "objectImpl.h"
#include "mofdecl.h"
#include "symtab.h"
#include "mofc.h"
#include "hash.h"
#include "backend.h"


/* Be aware that these are all entry points into SFCB's guts */
extern int ClClassAddQualifier(ClObjectHdr * hdr, ClSection * qlfs,
			       const char *id, CMPIData d);
extern int ClClassAddProperty(ClClass * cls, const char *id, CMPIData d);
extern void *ClObjectGetClSection(ClObjectHdr * hdr, ClSection * s);
extern int ClClassAddPropertyQualifier(ClObjectHdr * hdr, ClProperty * p,
				       const char *id, CMPIData d);
extern ClClass *ClClassNew(const char *cn, const char *pa);
extern ClClass *ClClassRebuildClass(ClClass * cls, void *area);
extern void ClClassFreeClass(ClClass * cls);
extern char *ClClassToString(ClClass * cls);
extern CMPIStatus simpleArrayAdd(CMPIArray * array, CMPIValue * val, CMPIType type);

extern long swapEndianClass(ClClass * cls);

extern CMPIBroker *Broker;

static unsigned sfcb_options = BACKEND_DEFAULT;
static int endianMode = SFCB_LOCAL_ENDIAN;

#define BACKEND_SFCB_NO_QUALIFIERS      0x0100
#define BACKEND_SFCB_REDUCED_QUALIFIERS 0x0200

CMPIType make_cmpi_type(type_type lextype, int arrayspec)
{
  int array_flag = arrayspec == -1 ? 0 : CMPI_ARRAY;
  switch (lextype.type_base) {
  case BaseTypeUINT8:
    return CMPI_uint8 | array_flag;
  case BaseTypeSINT8:
    return CMPI_sint8 | array_flag;
  case BaseTypeUINT16:
    return CMPI_uint16 | array_flag;
  case BaseTypeSINT16:
    return CMPI_sint16 | array_flag;
  case BaseTypeUINT32:
    return CMPI_uint32 | array_flag;
  case BaseTypeSINT32:
    return CMPI_sint32 | array_flag;
  case BaseTypeUINT64:
    return CMPI_uint64 | array_flag;
  case BaseTypeSINT64:
    return CMPI_sint64 | array_flag;
  case BaseTypeREAL32:
    return CMPI_real32 | array_flag;
  case BaseTypeREAL64:
    return CMPI_real64 | array_flag;
  case BaseTypeCHAR16:
    return CMPI_char16 | array_flag;
  case BaseTypeSTRING:
    return CMPI_string | array_flag;
  case BaseTypeBOOLEAN:
    return CMPI_boolean | array_flag;
  case BaseTypeDATETIME:
    return CMPI_dateTime | array_flag;
  default:
    return CMPI_ref | array_flag;
  }  
}

static CMPIData make_cmpi_data( type_type lextype, int arrayspec, 
				value_chain * vals )
{
  CMPIData data;
  CMPIData arr_data;
  int i = 0;
  CMPIStatus st;
  
  data.type = make_cmpi_type(lextype,arrayspec);
  data.value.uint64 = 0L;        /* set to binary zeros */
  
  if ( vals == NULL )  {
    data.state = CMPI_nullValue;
  } else if (vals -> val_value == NULL) {
    fprintf (stderr,"*** fatal error in backend: NULL value recieved.\n");
    abort();  /* paranoia */
  }else { 
    data.state = CMPI_goodValue;
  }    
  
  if ( data.state == CMPI_goodValue ) {
    if (data.type & CMPI_ARRAY) {
      /* process array entries */
      data.value.array = 
	CMNewArray(Broker,0,data.type&~CMPI_ARRAY,NULL);
      while (vals && vals -> val_value) {
	arr_data = make_cmpi_data(lextype,-1,vals);
     st=simpleArrayAdd(data.value.array, &arr_data.value, data.type&~CMPI_ARRAY);
	i++;
	vals = vals -> val_next;
      }
    } else {
      switch (data.type & ~CMPI_ARRAY) {
      case CMPI_uint8:
	sscanf(vals -> val_value, "%hhu", &data.value.uint8 );
	break;
      case CMPI_sint8:
	sscanf(vals -> val_value, "%hhd", &data.value.sint8 );
	break;
      case CMPI_uint16:
	sscanf(vals -> val_value, "%hu", &data.value.uint16 );
	break;
      case CMPI_sint16:
	sscanf(vals -> val_value, "%hd", &data.value.sint16 );
	break;
      case CMPI_uint32:
	sscanf(vals -> val_value, "%lu", &data.value.uint32 );
	break;
      case CMPI_sint32:
	sscanf(vals -> val_value, "%ld", &data.value.sint32 );
	break;
      case CMPI_uint64:
	sscanf(vals -> val_value, "%llu", &data.value.uint64);
	break;
      case CMPI_sint64:
	sscanf(vals -> val_value, "%lld", &data.value.uint64 );
	break;
      case CMPI_real32:
	sscanf(vals -> val_value, "%f", &data.value.real32 );
	break;
      case CMPI_real64:
	sscanf(vals -> val_value, "%lf", &data.value.real64 );
	break;
      case CMPI_char16:
	/* this one is suspect to produce garbage */
	sscanf(vals -> val_value, "%c", &data.value.uint8 );
	break;
      case CMPI_string:
	data.value.string = CMNewString(Broker,vals -> val_value,NULL);
	break;
      case CMPI_boolean:
	if (strcasecmp("true",vals -> val_value) == 0) {
	  data.value.boolean = 1;
	} else {
	  data.value.boolean = 0;
	}
	break;
      case CMPI_dateTime:
	data.value.dateTime = 
	  CMNewDateTimeFromChars(Broker,vals -> val_value,NULL);
	if (data.value.dateTime == NULL) {
	  fprintf(stderr,"failed to build datetime from %s", vals -> val_value);
	  data.state = CMPI_nullValue;
	}
	break;
      default:
	data.value.ref = CMNewObjectPath(Broker,NULL,
					 lextype.type_ref -> class_id,NULL);
      }
    }
  }
  return data;
}

static void sfcb_add_version(FILE * f, unsigned short opt, int endianMode)
{
   ClVersionRecord rec;
   long size;
   
   rec = ClBuildVersionRecord(opt, endianMode, &size);
   fwrite(&rec,size,1,f);
}

static int sfcb_add_class(FILE * f, hashentry * he, class_entry * ce, int endianMode)
{
  /* SFCB related */
  ClClass * sfcbClass;
  ClClass * sfcbClassRewritten;
  ClProperty * sfcbProp;
  int prop_id;
  int qual_id;
  long size;
  /* Symtab related */
  qual_chain * quals = ce -> class_quals;
  prop_chain * props = ce -> class_props;
  

  /* postfix processing - recursive */
  if ( ce -> class_parent) {
    sfcb_add_class( f, he, ce -> class_parent, endianMode ); 
  }
  if ( htlookup( he, 
		 upstrdup(ce -> class_id, 
			  strlen(ce -> class_id)), 
		 strlen(ce -> class_id))
       == NULL ) {
    if (sfcb_options & BACKEND_VERBOSE) {
      fprintf(stderr,"  adding class %s \n", ce -> class_id );
    }
    /* remember we did this class already */
    htinsert( he, 
	      upstrdup(ce -> class_id, 
		       strlen(ce -> class_id)), strlen(ce -> class_id),
	      (void *)1); 
    sfcbClass = ClClassNew( ce -> class_id, 
			    ce -> class_parent ? 
			    ce -> class_parent -> class_id : NULL );
    if (sfcbClass == NULL) {
      fprintf(stderr,"Error: could not create SFCB class for %s\n",ce -> class_id);
      return 1;
    }
    while (quals) {
      if (sfcb_options & BACKEND_VERBOSE) {
	fprintf(stderr,"  adding qualifier %s for class %s \n", 
		quals -> qual_id, ce -> class_id );
      }
      qual_id =  
	ClClassAddQualifier(&sfcbClass->hdr, &sfcbClass->qualifiers,
			    quals->qual_id, 
			    make_cmpi_data(quals->qual_qual->qual_type,
					   quals->qual_qual->qual_array,
					   quals->qual_vals));
      quals = quals -> qual_next;
    }
    while (props) {
      if (sfcb_options & BACKEND_VERBOSE) {
	fprintf(stderr,"  adding property %s for class %s \n", 
		props -> prop_id, ce -> class_id );
      }
      prop_id = ClClassAddProperty(sfcbClass,
				   props->prop_id, 
				   make_cmpi_data(props->prop_type,
						  props->prop_array,
						  props->prop_value));
      if (prop_id == 0) {
	fprintf(stderr,"Error: could not add SFCB class property %s for %s\n",
		props -> prop_id, ce -> class_id);
	return 1;
      }
      quals = props -> prop_quals;
      sfcbProp=((ClProperty*)ClObjectGetClSection(&sfcbClass->hdr,&sfcbClass->properties))+prop_id-1;
      while (quals) {
	if ( (sfcb_options & BACKEND_SFCB_REDUCED_QUALIFIERS) == 0 ||
	    (strcasecmp("description",quals->qual_id) &&
	     strcasecmp("valuemap",quals->qual_id) &&
	     strcasecmp("values",quals->qual_id) ) ) {
	  if (sfcb_options & BACKEND_VERBOSE) {
	    fprintf(stderr,"  adding qualifier %s for property %s in class %s\n", 
		    quals -> qual_id, props -> prop_id, ce -> class_id );
	  }
	  qual_id = 
	    ClClassAddPropertyQualifier(&sfcbClass->hdr, 
					sfcbProp,
					quals->qual_id, 
					make_cmpi_data(quals->qual_qual->qual_type,
						       quals->qual_qual->qual_array,
						       quals->qual_vals));
	}
	quals = quals -> qual_next;
      }
      props = props -> prop_next;
    }
    
    sfcbClassRewritten = ClClassRebuildClass(sfcbClass,NULL);
    size=sfcbClassRewritten->hdr.size;

    if (endianMode != SFCB_LOCAL_ENDIAN)
       swapEndianClass(sfcbClassRewritten);

    fwrite(sfcbClassRewritten,size,1,f);
    //ClClassFreeClass(sfcbClassRewritten);
    free(sfcbClassRewritten);
  }
  return 0;
}

int backend_sfcb(class_chain * cls_chain, const char * outfile, 
		 unsigned options, const char * extraopts)
{
  hashentry * classes_done = htcreate("SFCB");
  FILE      * class_file = fopen(outfile, "w");
  short test = 1;
  char *tp = (char*)&test;
    
  if (class_file == NULL) {
    return 1;
  }
  sfcb_options = options;
  
  if (strchr(extraopts,'Q')) {
    sfcb_options |= BACKEND_SFCB_NO_QUALIFIERS;
    sfcb_options |= BACKEND_SFCB_REDUCED_QUALIFIERS;
  } else if (strchr(extraopts,'q')) {
    sfcb_options |= BACKEND_SFCB_REDUCED_QUALIFIERS;
    if (sfcb_options & BACKEND_VERBOSE) {
      fprintf(stderr,"  information: omitting selected qualifiers.\n");
    }
  }
  
  if (strchr(extraopts,'L')) {
    endianMode = SFCB_LITTLE_ENDIAN;
  }
  if (strchr(extraopts,'B')) {
    endianMode = SFCB_BIG_ENDIAN;
  }
  
  if (tp[0]==1 && endianMode == SFCB_LITTLE_ENDIAN) endianMode=0;
  else if (tp[1]==1 && endianMode == SFCB_BIG_ENDIAN) endianMode=0;
   
  sfcb_add_version(class_file, ClTypeClassRep, endianMode);

  while (cls_chain && cls_chain->class_item) {
    if (sfcb_add_class(class_file, classes_done, cls_chain->class_item,endianMode)) {
      return 1;
    }
    cls_chain = cls_chain -> class_next;
  }
  
  fclose(class_file);
  return 0;
}

backend_type * backend_ptr = backend_sfcb;

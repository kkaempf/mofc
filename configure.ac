# $Id: configure.ac,v 1.1 2005/03/03 09:08:47 mihajlov Exp $

AC_INIT(MOF Compiler, 0.6.5a, mihajlov@de.ibm.com,mofc)
AC_CONFIG_SRCDIR([hash.c])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE

AC_CANONICAL_HOST
case $host_os in
     openedition*) CPPFLAGS=-W"c,SSCOMM" 
		CPPFLAGS="-D_OPEN_SOURCE=3 -D_ENHANCED_ASCII_EXT=0xFFFFFFFF $CPPFLAGS"
		CFLAGS=-W"c,ASCII,dll,expo,langlvl(extended),float"
		LDFLAGS=-W"l,XPLINK,dll,EDIT=NO"
		;;
     **)	CFLAGS=-Wall
		CFLAGS="-g $CFLAGS"
		;;
esac

AC_ARG_ENABLE(relax-mofsyntax,
	      [AC_HELP_STRING([--enable-relax-mofsyntax],
		              [be less strict when parsing MOFs.])])
AC_ARG_ENABLE(testscanner,
	      [AC_HELP_STRING([--enable-testscanner],
		              [produce a standalone scanner for testing.])])
AC_ARG_VAR(SFCB_DIR,
      [set SFCB_DIR to the directory containing the SFCB sources, 
       if configure fails to find it.])

AC_ARG_WITH(sfcb,
	    [AC_HELP_STRING([--with-sfcb],
		              [use sfcb backend. In that case this package must
			       be in a sub-directory of the sfcb package.])])

if test "$with_sfcb" != no; then
   if test x"$SFCB_DIR" = x; then
      SFCB_DIR=..
   fi
   BACKEND_OBJ='backend_sfcb.$(OBJEXT)'
   BACKEND_LIBS="-L $SFCB_DIR/.libs -lsfcBrokerCore"
   BACKEND_INCLUDES="-I $SFCB_DIR"
   AC_DEFINE(BACKEND_INFO,"SFCB Backend",[Backend Identification String])
else
   BACKEND_OBJ='backend_print.$(OBJEXT)'
   BACKEND_LIBS=
   BACKEND_INCLUDES=
   AC_DEFINE(BACKEND_INFO,"Default Backend",[Backend Identification String])
fi

if test "$CFLAGS" != ""; then
  user_cflags_set=yes
else
  user_cflags_set=no
fi


# Checks for programs.
AC_PROG_CC
AC_PROG_YACC
AC_PROG_LEX

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stddef.h stdlib.h string.h unistd.h])
if test "$with_sfcb" = yes; then
   save_CPPFLAGS=$CPPFLAGS
   CPPFLAGS="-I $SFCB_DIR $CPPFLAGS"
   AC_CHECK_HEADER(objectImpl.h,,
	[AC_MSG_ERROR([sorry, could not find SFCB include files. Hint: set SFCB_DIR environment variable.])])
   CPPFLAGS=$save_CPPFLAGS
fi;

# Checks for libraries
if test "$with_sfcb" = yes && test x"$sfcb_recursed" = x; then
   save_LDFLAGS=$LDFLAGS
   LDFLAGS="-L $SFCB_DIR/.libs $LDFLAGS"
   AC_CHECK_LIB(sfcBrokerCore,main,,
       [AC_MSG_ERROR([sorry, could not find SFCB libraries. Hint: set SFCB_DIR environment variable.])])
   LDFLAGS=$save_LDFLAGS
fi;

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([strcasecmp strchr strdup strrchr strstr])

# Defines
if test "$enable_relax_mofsyntax" = yes; then
  AC_DEFINE(RELAXED_MOF,,[Relaxed MOF Parsing])
fi

AM_CONDITIONAL(TESTSCANNER,[test "$enable_testscanner" = yes]) 

AC_SUBST(BACKEND_OBJ)
AC_SUBST(BACKEND_LIBS)
AC_SUBST(BACKEND_INCLUDES)

# Output control
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

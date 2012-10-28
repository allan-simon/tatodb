AC_INIT
AM_INIT_AUTOMAKE(libtato, 0.2)

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

AC_CHECK_LIB(expat, XML_ParserCreate, [], [
	echo "Error! You need libexpat."
	exit -1
])

AC_CONFIG_FILES([tato.pc])
AC_OUTPUT(Makefile tato/Makefile)



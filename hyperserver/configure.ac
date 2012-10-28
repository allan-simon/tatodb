AC_INIT
AM_INIT_AUTOMAKE(tatodbd, 0.1)

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

PKG_CHECK_MODULES(TATO, [tato])

AC_CHECK_LIB(event, evhttp_new, [], [
	echo "Error! You need libevent >= 1.4."
	exit -1
])

AC_CHECK_LIB(dl, dlopen, [], [
	echo "Error! You need libdl >= 1.4."
	exit -1
])

AC_ARG_WITH(plugin-swac, [  --with-plugin-swac	  Build swac plugin], [
	AM_CONDITIONAL(BUILD_PLUGIN_SWAC, true)
	sinclude(plugins/swac/plugin.ac)
], [
	AM_CONDITIONAL(BUILD_PLUGIN_SWAC, false)
])
AC_OUTPUT(plugins/swac/Makefile)

AC_ARG_WITH(plugin-verbiste, [  --with-plugin-verbiste  Build verbiste plugin], [
	AM_CONDITIONAL(BUILD_PLUGIN_VERBISTE, true)
	sinclude(plugins/verbiste/plugin.ac)
], [
	AM_CONDITIONAL(BUILD_PLUGIN_VERBISTE, false)
])
AC_OUTPUT(plugins/verbiste/Makefile)

AC_OUTPUT(Makefile src/Makefile doc/Makefile plugins/Makefile)


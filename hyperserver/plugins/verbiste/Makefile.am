libdir = @libdir@/tatodb

if BUILD_PLUGIN_VERBISTE
AM_CFLAGS = @VERBISTE_CFLAGS@ @TATO_CFLAGS@ -I../../src/

lib_LTLIBRARIES = libverbiste.la
libverbiste_la_SOURCES = \
	plugin_verbiste.c \
	plugin_verbiste.h

libverbiste_la_LIBADD = @VERBISTE_LIBS@ @TATO_LIBS@
endif

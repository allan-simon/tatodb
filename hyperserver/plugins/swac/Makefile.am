libdir = @libdir@/tatodb

if BUILD_PLUGIN_SWAC
AM_CFLAGS = @SQLITE3_CFLAGS@ @TATO_CFLAGS@ -I../../src/

lib_LTLIBRARIES = libswac.la
libswac_la_SOURCES = \
	plugin_swac.c \
	plugin_swac.h \
	swac.c \
	swac.h \
	sound.c \
	sound.h \
	lang.c \
	lang.h

libswac_la_LIBADD = @SQLITE3_LIBS@ @TATO_LIBS@
endif

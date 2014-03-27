ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = spatialite_gui

INCLUDES = @CFLAGS@
INCLUDES += -I$(top_srcdir)

spatialite_gui_SOURCES = Classdef.h BlobExplorer.cpp \
	Dialogs.cpp DialogsGraph.cpp Exif.cpp \
	Main.cpp MalformedGeoms.cpp Network.cpp \
	Objects.cpp QueryView.cpp QueryViewComposer.cpp \
	ResultSetView.cpp Shapefiles.cpp TableTree.cpp \
	TextCsv.cpp Wfs.cpp

LDADD = @WX_LIBS@ @LIBGAIAGRAPHICS_LIBS@ @LIBSPATIALITE_LIBS@ \
	@LIBFREEXL_LIBS@ @LIBXML2_LIBS@

EXTRA_DIST = Makefile-static-MinGW \
        Makefile-static-Linux \
        Makefile-static-MacOsX \
	indent_me

AUTOMAKE_OPTIONS = dist-zip

SUBDIRS = icons win_resource mac_resource gnome_resource


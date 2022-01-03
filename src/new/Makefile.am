bin_PROGRAMS = nip4

nip4_SOURCES = \
	app.c \
	app.h \
	gresources.c \
	gtkutil.c \
	gtkutil.h \
	main.c \
	main.h \
	nip.h \
	startup.c

AM_CPPFLAGS = @PKGFLAGS_CFLAGS@
LDADD = @PKGFLAGS_CFLAGS@ @PKGFLAGS_LIBS@ -lm
AM_LDFLAGS = @LDFLAGS@ 

resource_files = \
	gresources.xml 

gresources.c: $(resource_files)
	glib-compile-resources \
		--target=$@ \
		--sourcedir=$(top_srcdir)/src/gtk \
		--generate-source \
		$<

CLEANFILES = gresources.c

EXTRA_DIST = \
	gtk \
	$(resource_files) 



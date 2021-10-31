SUBDIRS = \
	src \
	test \
	man \
	share \
	po 

EXTRA_DIST = \
	autogen.sh \
	doc \
	proj \
	m4 \
	screenshot.png \
	nip4.desktop.in \
	nip4.xml \
	nip4.appdata.xml

nip4appdir = $(datadir)/applications

nip4mimedir = $(datadir)/mime/packages

nip4appdatadir = $(datadir)/appdata

nip4app_DATA = nip4.desktop

nip4mime_DATA = nip4.xml

nip4appdata_DATA = nip4.appdata.xml

install-exec-hook:
	-rm -rf ${DESTDIR}$(datadir)/doc/nip4 
	$(mkinstalldirs) ${DESTDIR}$(datadir)/doc/nip4
	-cp -r ${top_srcdir}/doc/html ${top_srcdir}/doc/pdf ${DESTDIR}$(datadir)/doc/nip4
	-rm -rf ${DESTDIR}$(datadir)/doc/nip4/html/CVS
	-rm -rf ${DESTDIR}$(datadir)/doc/nip4/pdf/CVS

if UPDATE_DESKTOP
install-data-hook:
	-$(UPDATE_MIME_DATABASE) ${DESTDIR}$(datadir)/mime
	-$(UPDATE_DESKTOP_DATABASE) ${DESTDIR}$(datadir)/applications
endif

dist-hook:
# make sure we don't get any .svn dirs from EXTRA_DIST
# also "fred" gets left around occasionally
	-find $(distdir) -name .svn -exec rm -rf {} \;
	-find $(distdir) -name fred -exec rm {} \;

uninstall-hook:
# make sure we have write permission for 'rm'
	-chmod -R u+w ${DESTDIR}$(datadir)/doc/nip4
	-rm -rf ${DESTDIR}$(datadir)/doc/nip4
	-$(UPDATE_MIME_DATABASE) ${DESTDIR}$(datadir)/mime
	-$(UPDATE_DESKTOP_DATABASE) ${DESTDIR}$(datadir)/applications

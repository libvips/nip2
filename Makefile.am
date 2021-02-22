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
	nip3.desktop.in \
	nip3.xml \
	nip3.appdata.xml

nip3appdir = $(datadir)/applications

nip3mimedir = $(datadir)/mime/packages

nip3appdatadir = $(datadir)/appdata

nip3app_DATA = nip3.desktop

nip3mime_DATA = nip3.xml

nip3appdata_DATA = nip3.appdata.xml

install-exec-hook:
	-rm -rf ${DESTDIR}$(datadir)/doc/nip3 
	$(mkinstalldirs) ${DESTDIR}$(datadir)/doc/nip3
	-cp -r ${top_srcdir}/doc/html ${top_srcdir}/doc/pdf ${DESTDIR}$(datadir)/doc/nip3
	-rm -rf ${DESTDIR}$(datadir)/doc/nip3/html/CVS
	-rm -rf ${DESTDIR}$(datadir)/doc/nip3/pdf/CVS

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
	-chmod -R u+w ${DESTDIR}$(datadir)/doc/nip3
	-rm -rf ${DESTDIR}$(datadir)/doc/nip3
	-$(UPDATE_MIME_DATABASE) ${DESTDIR}$(datadir)/mime
	-$(UPDATE_DESKTOP_DATABASE) ${DESTDIR}$(datadir)/applications

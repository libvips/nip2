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
	nip2.desktop.in \
	nip2.xml \
	nip2.appdata.xml

nip2appdir = $(datadir)/applications

nip2mimedir = $(datadir)/mime/packages

nip2appdatadir = $(datadir)/appdata

nip2app_DATA = nip2.desktop

nip2mime_DATA = nip2.xml

nip2appdata_DATA = nip2.appdata.xml

install-exec-hook:
	-rm -rf ${DESTDIR}$(datadir)/doc/nip2 
	$(mkinstalldirs) ${DESTDIR}$(datadir)/doc/nip2
	-cp -r ${top_srcdir}/doc/html ${top_srcdir}/doc/pdf ${DESTDIR}$(datadir)/doc/nip2
	-rm -rf ${DESTDIR}$(datadir)/doc/nip2/html/CVS
	-rm -rf ${DESTDIR}$(datadir)/doc/nip2/pdf/CVS

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
	-chmod -R u+w ${DESTDIR}$(datadir)/doc/nip2
	-rm -rf ${DESTDIR}$(datadir)/doc/nip2
	-$(UPDATE_MIME_DATABASE) ${DESTDIR}$(datadir)/mime
	-$(UPDATE_DESKTOP_DATABASE) ${DESTDIR}$(datadir)/applications

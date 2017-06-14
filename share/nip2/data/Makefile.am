nipdatadir = $(pkgdatadir)/data

nipdata_DATA = \
	rachel.con \
	AdobeRGB1998.icc \
	sRGB.icm \
	macbeth_lab_d65.mat \
	macbeth_lab_d50.mat \
	nip2.png \
	nip2-icon.ico \
	cmyk.icm \
	stock-tool-ink-22.png \
	stock-tool-path-22.png \
	stock-tool-text-22.png \
	stock-tool-smudge-22.png \
	stock-tool-bucket-fill-22.png \
	stock-tool-rect-select-22.png \
	stock-tool-select-22.png \
	stock-padlock-closed-22.png \
	stock-alert-22.png \
	nip-slider-16.png \
	stock-tool-move-22.png \
	stock-led-red-18.png \
	stock-led-green-18.png \
	stock-led-blue-18.png \
	stock-led-cyan-18.png \
	stock-led-yellow-18.png \
	stock-led-off-18.png

EXTRA_DIST = \
	$(nipdata_DATA) \
	examples

install-exec-hook:
	rm -rf $(DESTDIR)$(nipdatadir)/examples
	$(mkinstalldirs) $(DESTDIR)$(nipdatadir)/examples
	cp -r ${top_srcdir}/share/nip2/data/examples/* $(DESTDIR)$(nipdatadir)/examples

uninstall-hook:
# make sure we have write permission for 'rm'
	chmod -R u+w ${DESTDIR}$(nipdatadir)/examples
	${RM} -rf ${DESTDIR}$(nipdatadir)/examples


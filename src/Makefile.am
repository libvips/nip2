SUBDIRS = BITMAPS

bin_SCRIPTS = run-nip2.sh

# need this to keep autoconf quiet, but we don't actually use it ... see the
# bison rule below
YACC = bison

# windows resource files ... see .rc.o rule later
SUFFIXES = .rc

# only build the cli wrapper on win32
if OS_WIN32 
bin_PROGRAMS = nip2 nip2-cli
nip2_cli_SOURCES = nip2-cli.c
nip2_CFLAGS="-mwindows"
CLI_DIST = 
else
bin_PROGRAMS = nip2 
CLI_DIST = nip2-cli.c
endif

nip2_SOURCES = \
	plotmodel.c \
	plotmodel.h \
	plotpresent.c \
	plotpresent.h \
	plotstatus.c \
	plotstatus.h \
	floatwindow.c \
	floatwindow.h \
	vobject.c \
	vobject.h \
	action.c \
	action.h \
	boxes.c \
	boxes.h \
	browse.c \
	browse.h \
	builtin.c \
	builtin.h \
	icontainer.c \
	icontainer.h \
	iobject.c \
	iobject.h \
	class.c \
	class.h \
	classmodel.c \
	classmodel.h \
	colour.c \
	colour.h \
	colourdisplay.c \
	colourdisplay.h \
	colourview.c \
	colourview.h \
	column.c \
	column.h \
	columnview.c \
	columnview.h \
	compile.c \
	compile.h \
	conversion.c \
	conversion.h \
	conversionview.c \
	conversionview.h \
	doubleclick.c \
	doubleclick.h \
	dump.c \
	dump.h \
	expr.c \
	expr.h \
	filemodel.c \
	filemodel.h \
	pane.c \
	pane.h \
	pathname.c \
	pathname.h \
	fontname.c \
	fontname.h \
	group.c \
	group.h \
	pathnameview.c \
	pathnameview.h \
	fontnameview.c \
	fontnameview.h \
	filesel.c \
	filesel.h \
	graph.c \
	graph.h \
	graphicview.c \
	graphicview.h \
	gtkutil.c \
	gtkutil.h \
	gtksheet.c \
	gtksheet.h \
	gtkitementry.c \
	gtkitementry.h \
	gtkextra-marshal.c \
	gtkextra-marshal.h \
	gtkextra.c \
	gtkextrafeatures.h \
	gtkplotbar.c \
	gtkplotbar.h \
	gtkplotarray.c \
	gtkplotarray.h \
	gtkplot.c \
	gtkplot.h \
	gtkplotpc.c \
	gtkplotpc.h \
	gtkpsfont.c \
	gtkpsfont.h \
	gtkplotdata.c \
	gtkplotdata.h \
	gtkplotgdk.c \
	gtkplotgdk.h \
	gtkplot3d.c \
	gtkplot3d.h \
	gtkplotcanvas.c \
	gtkplotcanvasellipse.c \
	gtkplotcanvasellipse.h \
	gtkplotcanvas.h \
	gtkplotcanvasline.c \
	gtkplotcanvasline.h \
	gtkplotcanvaspixmap.c \
	gtkplotcanvaspixmap.h \
	gtkplotcanvasplot.c \
	gtkplotcanvasplot.h \
	gtkplotcanvasrectangle.c \
	gtkplotcanvasrectangle.h \
	gtkplotcanvastext.c \
	gtkplotcanvastext.h \
	gtkplotpolar.c \
	gtkplotpolar.h \
	gtkplotsurface.c \
	gtkplotsurface.h \
	gtkplotdt.c \
	gtkplotdt.h \
	gtkplotps.c \
	gtkplotps.h \
	heap.c \
	heap.h \
	heapmodel.c \
	heapmodel.h \
	helpindex.h \
	nipmarshal.h \
	nipmarshal.c \
	iarrow.c \
	iarrow.h \
	value.c \
	value.h \
	valueview.c \
	valueview.h \
	idialog.c \
	idialog.h \
	iimage.c \
	iimage.h \
	iimageview.c \
	iimageview.h \
	imagedisplay.c \
	imagedisplay.h \
	log.c \
	log.h \
	error.c \
	error.h \
	managed.c \
	managed.h \
	managedfile.c \
	managedfile.h \
	managedgvalue.c \
	managedgvalue.h \
	managedgobject.c \
	managedgobject.h \
	imageinfo.c \
	imageinfo.h \
	imagemodel.c \
	imagemodel.h \
	imagepresent.c \
	imagepresent.h \
	imageview.c \
	imageview.h \
	ip.h \
	iregion.c \
	iregion.h \
	iregiongroup.c \
	iregiongroup.h \
	iregiongroupview.c \
	iregiongroupview.h \
	iregionview.c \
	iregionview.h \
	itext.c \
	itext.h \
	itextview.c \
	itextview.h \
	iwindow.c \
	iwindow.h \
	parse.y \
	parser.h \
	prefs.c \
	prefs.h \
	prefworkspaceview.c \
	prefworkspaceview.h \
	prefcolumnview.c \
	prefcolumnview.h \
	lex.l \
	link.c \
	link.h \
	main.c \
	main.h \
	mainw.c \
	mainw.h \
	matrix.c \
	matrix.h \
	matrixview.c \
	matrixview.h \
	plot.c \
	plot.h \
	plotview.c \
	plotview.h \
	plotwindow.c \
	plotwindow.h \
	model.c \
	model.h \
	nip2-icon.rc \
	option.c \
	option.h \
	optionview.c \
	optionview.h \
	orderitem.c \
	orderitem.h \
	orderlist.c \
	orderlist.h \
	formula.c \
	formula.h \
	paintboxview.c \
	paintboxview.h \
	path.c \
	path.h \
	predicate.c \
	predicate.h \
	program.c \
	program.h \
	string.c \
	istring.h \
	number.c \
	number.h \
	expression.c \
	expression.h \
	expressionview.c \
	expressionview.h \
	stringview.c \
	stringview.h \
	editview.c \
	editview.h \
	numberview.c \
	numberview.h \
	real.c \
	real.h \
	vector.c \
	vector.h \
	reduce.c \
	reduce.h \
	regionview.c \
	regionview.h \
	rhs.c \
	rhs.h \
	rhsview.c \
	rhsview.h \
	row.c \
	row.h \
	rowview.c \
	rowview.h \
	secret.c \
	secret.h \
	slider.c \
	slider.h \
	sliderview.c \
	sliderview.h \
	clock.c \
	clock.h \
	spin.c \
	spin.h \
	statusview.c \
	statusview.h \
	subcolumn.c \
	subcolumn.h \
	subcolumnview.c \
	subcolumnview.h \
	symbol.c \
	symbol.h \
	toggle.c \
	toggle.h \
	toggleview.c \
	toggleview.h \
	tool.c \
	tool.h \
	toolkit.c \
	toolkit.h \
	toolkitgroup.c \
	toolkitgroup.h \
	toolkitgroupview.c \
	toolkitgroupview.h \
	toolkitview.c \
	toolkitview.h \
	toolkitbrowser.c \
	toolkitbrowser.h \
	toolview.c \
	toolview.h \
	trace.c \
	trace.h \
	tree.c \
	tree.h \
	tslider.c \
	tslider.h \
	util.c \
	util.h \
	view.c \
	view.h \
	vips_call.c \
	vips_call.h \
	call8.c \
	call8.h \
	watch.c \
	watch.h \
	wild.c \
	wild.h \
	workspace.c \
	workspace.h \
	workspacedefs.c \
	workspacedefs.h \
	workspacegroup.c \
	workspacegroup.h \
	workspaceview.c \
	workspaceview.h 

.PHONEY: helpindex.h
helpindex.h: 
	./makehelpindex.pl $(prefix) > helpindex.h
.PHONEY: nipmarshal.h
nipmarshal.h:
	glib-genmarshal --prefix=nip --header nipmarshal.list > nipmarshal.h
.PHONEY: nipmarshal.c
nipmarshal.c:
	echo "#include \"nipmarshal.h\"" > nipmarshal.c 
	glib-genmarshal --prefix=nip --body nipmarshal.list >> nipmarshal.c
.PHONEY: gtkextra-marshal.h
gtkextra-marshal.h:
	glib-genmarshal --prefix=gtkextra --header gtkextra-marshal.list > \
		gtkextra-marshal.h
.PHONEY: gtkextra-marshal.c
gtkextra-marshal.c:
	echo "#include \"gtkextra-marshal.h\"" > gtkextra-marshal.c 
	glib-genmarshal --prefix=gtkextra --body gtkextra-marshal.list >> \
		gtkextra-marshal.c

nip2-icon.rc:
	echo 1 ICON \"nip2-icon.ico\" > nip2-icon.rc

.rc.o:
if OS_WIN32 
	cp ${top_srcdir}/share/nip2/data/nip2-icon.ico .
	${WINDRES} $< -o $@ 
else
	echo "int poop () {}" >dummy.c 
	$(CC) -c dummy.c -o $@
endif

# we have to replace the standard .y.c rule: we are a bison-only GLR parser,
# so we can't use autoconf's preferred -y yacc-compatibility stuff
.y.c:
	$(BISON) --defines=$*.h -o $*.c $<

INCLUDES = @IP_CFLAGS@ 
LDADD = @IP_CFLAGS@ @IP_LIBS@
AM_LDFLAGS = @LDFLAGS@

dist-hook:
	${RM} ${distdir}/parse.c ${distdir}/lex.c

CLEANFILES = parse.c parse.h lex.c tags

EXTRA_DIST = makehelpindex.pl helpindex.h run-nip2.sh \
	nipmarshal.h nipmarshal.c nipmarshal.list \
	gtkextra-marshal.h gtkextra-marshal.c gtkextra-marshal.list \
	$(CLI_DIST)

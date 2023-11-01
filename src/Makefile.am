SUBDIRS = BITMAPS

# need this to keep autoconf quiet, but we don't actually use it ... see the
# bison rule below
YACC = bison

# windows resource files ... see .rc.o rule later
SUFFIXES = .rc

# only build the cli wrapper on win32
if OS_WIN32 
bin_PROGRAMS = nip4 nip4-cli
nip4_cli_SOURCES = nip4-cli.c
nip4_CFLAGS="-mwindows"
CLI_DIST = 
else
bin_PROGRAMS = nip4 
CLI_DIST = nip4-cli.c
endif

nip4_SOURCES = \
	vipsobject.c \
	vipsobject.h \
	plotmodel.c \
	plotmodel.h \
	progress.c \
	progress.h \
	panechild.h \
	plotpresent.c \
	plotpresent.h \
	plotstatus.h \
	floatwindow.h \
	vobject.c \
	vobject.h \
	action.c \
	action.h \
	boxes.h \
	popupbutton.h \
	imageheader.h \
	preview.h \
	builtin.c \
	builtin.h \
	cache.c \
	cache.h \
	call.c \
	call.h \
	class.c \
	class.h \
	classmodel.c \
	classmodel.h \
	clock.c \
	clock.h \
	colour.c \
	colourdisplay.c \
	colourdisplay.h \
	colour.h \
	colourview.c \
	colourview.h \
	column.c \
	column.h \
	columnview.c \
	columnview.h \
	compile.c \
	compile.h \
	defbrowser.c \
	defbrowser.h \
	doubleclick.c \
	doubleclick.h \
	dump.c \
	dump.h \
	editview.c \
	editview.h \
	error.c \
	error.h \
	expr.c \
	expression.c \
	expression.h \
	expressionview.c \
	expressionview.h \
	expr.h \
	filemodel.c \
	filemodel.h \
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
	floatwindow.c \
	floatwindow.h \
	fontname.c \
	fontname.h \
	fontnameview.c \
	fontnameview.h \
	formula.c \
	formula.h \
	graphicview.c \
	graphicview.h \
	gresources.c \
	group.c \
	group.h \
	gtkutil.c \
	gtkutil.h \
	heap.c \
	heap.h \
	heapmodel.c \
	heapmodel.h \
	helpindex.h \
	iarrow.c \
	iarrow.h \
	icontainer.c \
	icontainer.h \
	idialog.c \
	idialog.h \
	iimage.c \
	iimage.h \
	imagedisplay.c \
	imagedisplay.h \
	imageheader.c \
	imageheader.h \
	imageinfo.c \
	imageinfo.h \
	imagemodel.c \
	imagemodel.h \
	iobject.c \
	iobject.h \
	ip.h \
	iregion.c \
	iregiongroup.c \
	iregiongroup.h \
	iregiongroupview.c \
	iregiongroupview.h \
	iregion.h \
	iregionview.c \
	iregionview.h \
	istring.h \
	itext.c \
	itext.h \
	itextview.c \
	itextview.h \
	lex.l \
	link.c \
	link.h \
	main.c \
	main.h \
	managed.c \
	managedfile.c \
	managedfile.h \
	managedgobject.c \
	managedgobject.h \
	managedgvalue.c \
	managedgvalue.h \
	managed.h \
	managedstring.c \
	managedstring.h \
	matrix.c \
	matrix.h \
	matrixview.c \
	matrixview.h \
	model.c \
	model.h \
	nipmarshal.c \
	nipmarshal.h \
	number.c \
	number.h \
	numberview.c \
	numberview.h \
	option.c \
	option.h \
	optionview.c \
	optionview.h \
	paintboxview.c \
	paintboxview.h \
	pane.c \
	panechild.c \
	panechild.h \
	pane.h \
	parser.h \
	parse.y \
	path.c \
	path.h \
	pathname.c \
	pathname.h \
	pathnameview.c \
	pathnameview.h \
	plot.c \
	plot.h \
	plotmodel.c \
	plotmodel.h \
	plotpresent.c \
	plotpresent.h \
	plotstatus.c \
	plotstatus.h \
	plotview.c \
	plotview.h \
	popupbutton.c \
	popupbutton.h \
	predicate.c \
	predicate.h \
	prefcolumnview.c \
	prefcolumnview.h \
	prefs.c \
	prefs.h \
	prefworkspaceview.c \
	prefworkspaceview.h \
	preview.c \
	preview.h \
	progress.c \
	progress.h \
	real.c \
	real.h \
	reduce.c \
	reduce.h \
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
	spin.c \
	spin.h \
	statusview.c \
	statusview.h \
	string.c \
	stringview.c \
	stringview.h \
	subcolumn.c \
	subcolumn.h \
	subcolumnview.c \
	subcolumnview.h \
	symbol.c \
	symbol.h \
	tile.h \
	tile.c \
	tilesource.h \
	tilesource.c \
	tilecache.h \
	tilecache.c \
	toggle.c \
	toggle.h \
	toggleview.c \
	toggleview.h \
	tool.c \
	tool.h \
	toolkitbrowser.c \
	toolkitbrowser.h \
	toolkit.c \
	toolkitgroup.c \
	toolkitgroup.h \
	toolkitgroupview.c \
	toolkitgroupview.h \
	toolkit.h \
	toolkitview.c \
	toolkitview.h \
	toolview.c \
	toolview.h \
	tree.c \
	tree.h \
	tslider.c \
	tslider.h \
	util.c \
	util.h \
	value.c \
	value.h \
	valueview.c \
	valueview.h \
	vector.c \
	vector.h \
	view.c \
	view.h \
	vipsobject.c \
	vipsobject.h \
	vobject.c \
	vobject.h \
	watch.c \
	watch.h \
	workspace.c \
	workspacedefs.c \
	workspacedefs.h \
	workspacegroup.c \
	workspacegroup.h \
	workspacegroupview.c \
	workspacegroupview.h \
	workspace.h \
	workspaceroot.c \
	workspaceroot.h \
	workspaceview.c \
	workspaceview.h 

need_rewrite = \
	graphwindow.c \
	graphwindow.h \
	iimageview.c \
	iimageview.h \
	iwindow.c \
	iwindow.h \
	log.c \
	log.h \
	mainw.c \
	mainw.h \
	plotwindow.c \
	plotwindow.h \
	program.c \
	program.h \
	regionview.c \
	regionview.h

if OS_WIN32
nip4_SOURCES += \
	nip4-icon.rc
endif

helpindex.h: 
	./makehelpindex.pl $(prefix) > helpindex.h
nipmarshal.h:
	glib-genmarshal --prefix=nip --header nipmarshal.list > nipmarshal.h
nipmarshal.c:
	echo "#include \"nipmarshal.h\"" > nipmarshal.c 
	glib-genmarshal --prefix=nip --body nipmarshal.list >> nipmarshal.c

resource_files = \
	gresources.xml 

gresources.c: $(resource_files)
	glib-compile-resources \
		--target=$@ \
		--sourcedir=$(top_srcdir)/src/gtk \
		--generate-source \
		$<

.rc.o:
	cp ${top_srcdir}/share/nip4/data/nip4-icon.ico .
	${WINDRES} $< -o $@ 

# we have to replace the standard .y.c rule: we are a bison-only GLR parser,
# so we can't use autoconf's preferred -y yacc-compatibility stuff
.y.c:
	$(BISON) --defines=$*.h -o $*.c $<

nip4-model.o model.o: model.c parse.c

AM_CPPFLAGS = @IP_CFLAGS@ 
LDADD = @IP_CFLAGS@ @IP_LIBS@ 
AM_LDFLAGS = @LDFLAGS@

dist-hook:
	${RM} ${distdir}/parse.c ${distdir}/lex.c

CLEANFILES = parse.c parse.h lex.c tags gresources.c

EXTRA_DIST = makehelpindex.pl helpindex.h \
	nipmarshal.h nipmarshal.c nipmarshal.list \
	gresources.c \
	$(CLI_DIST)

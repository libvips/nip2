SUBDIRS = BITMAPS

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
	vipsobject.c \
	vipsobject.h \
	plotmodel.c \
	plotmodel.h \
	progress.c \
	progress.h \
	panechild.c \
	panechild.h \
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
	popupbutton.c \
	popupbutton.h \
	imageheader.c \
	imageheader.h \
	preview.c \
	preview.h \
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
	graphwindow.c \
	graphwindow.h \
	graphicview.c \
	graphicview.h \
	gtkutil.c \
	gtkutil.h \
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
	managedstring.c \
	managedstring.h \
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
	option.c \
	option.h \
	optionview.c \
	optionview.h \
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
	defbrowser.c \
	defbrowser.h \
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
	call.c \
	call.h \
	cache.c \
	cache.h \
	watch.c \
	watch.h \
	workspace.c \
	workspace.h \
	workspacegroup.c \
	workspacegroup.h \
	workspacegroupview.c \
	workspacegroupview.h \
	workspacedefs.c \
	workspacedefs.h \
	workspaceroot.c \
	workspaceroot.h \
	workspaceview.c \
	workspaceview.h 

if OS_WIN32
nip2_SOURCES += \
	nip2-icon.rc
endif

helpindex.h: 
	./makehelpindex.pl $(prefix) > helpindex.h
nipmarshal.h:
	glib-genmarshal --prefix=nip --header nipmarshal.list > nipmarshal.h
nipmarshal.c:
	echo "#include \"nipmarshal.h\"" > nipmarshal.c 
	glib-genmarshal --prefix=nip --body nipmarshal.list >> nipmarshal.c

.rc.o:
	cp ${top_srcdir}/share/nip2/data/nip2-icon.ico .
	${WINDRES} $< -o $@ 

# we have to replace the standard .y.c rule: we are a bison-only GLR parser,
# so we can't use autoconf's preferred -y yacc-compatibility stuff
.y.c:
	$(BISON) --defines=$*.h -o $*.c $<

nip2-model.o model.o: model.c parse.c

AM_CPPFLAGS = @IP_CFLAGS@ 
LDADD = @IP_CFLAGS@ @IP_LIBS@ 
AM_LDFLAGS = @LDFLAGS@

dist-hook:
	${RM} ${distdir}/parse.c ${distdir}/lex.c

CLEANFILES = parse.c parse.h lex.c tags

EXTRA_DIST = makehelpindex.pl helpindex.h \
	nipmarshal.h nipmarshal.c nipmarshal.list \
	$(CLI_DIST)

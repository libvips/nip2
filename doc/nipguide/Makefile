#

PDF = nipguide.pdf 
SRC = \
      	config.tex \
	infrared.tex \
	intro.tex \
	program.tex \
	reference.tex \
	menus.tex \
	tutorial.tex \
	mydefs.tex

mkinstalldirs = $(SHELL) ../../mkinstalldirs
destdir = ../

all: $(PDF) html

install: all $(PDF) html
	-rm -f ${destdir}/pdf/$(PDF)
	$(mkinstalldirs) ${destdir}/pdf
	-cp $(PDF) ${destdir}/pdf
	-rm -rf ${destdir}/html/figs
	-rm -f ${destdir}/html/*.html
	-rm -f ${destdir}/html/*.css
	$(mkinstalldirs) ${destdir}/html
	-cp -r nipguide/* ${destdir}/html

$(PDF): $(SRC) 
	pdflatex nipguide.tex
	pdflatex nipguide.tex

.PHONEY: html
html: 
	-rm -rf nipguide 
	mkdir nipguide
	htlatex nipguide.tex html.cfg,3 "" -dnipguide/
	cp -r figs nipguide
	-rm nipguide/*.jpg
	-rm nipguide/*.png

.PHONEY: clean 
clean:
	-rm -f *.4ct
	-rm -f *.4tc
	-rm -f *.log
	-rm -f *.xref
	-rm -f *.tmp
	-rm -f *.html
	-rm -f *.css
	-rm -f *.lg
	-rm -f *.idv
	-rm -f *.aux
	-rm -f *.dvi
	-rm -f *.lof
	-rm -f *.lot
	-rm -f *.toc
	-rm -f *.pdf
	-rm -rf nipguide

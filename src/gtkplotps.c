/* gtkplotps - postscript driver
 * Copyright 1999-2001  Adrian E. Feiguin <feiguin@ifir.edu.ar>
 *
 * Some few lines of code borrowed from
 * DiaCanvas -- a technical canvas widget
 * Copyright (C) 1999 Arjan Molenaar
 * Dia -- an diagram creation/manipulation program
 * Copyright (C) 1998 Alexander Larsson
 * ISO Latin encoding by
 * Przemek Klosowski
 * przemek@rrdbartok.nist.gov
 * (borrowed from XMGR)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <gtk/gtk.h>
#include <locale.h>

#include "gtkplot.h"
#include "gtkpsfont.h"
#include "gtkplotpc.h"
#include "gtkplotps.h"


static void gtk_plot_ps_class_init 		(GtkPlotPSClass *klass);
static void gtk_plot_ps_init 			(GtkPlotPS *ps);
static void gtk_plot_ps_destroy 		(GtkObject *object);
/*********************************************************************/
/* Postscript specific functions */
static gboolean psinit				(GtkPlotPC *pc); 
static void pssetviewport			(GtkPlotPC *pc, 
						 gdouble w, gdouble h); 
static void psleave				(GtkPlotPC *pc);
static void psgsave				(GtkPlotPC *pc);
static void psgrestore				(GtkPlotPC *pc);
static void psclip				(GtkPlotPC *pc,
						 const GdkRectangle *area);
static void psclipmask				(GtkPlotPC *pc,
						 gdouble x, gdouble y,
						 const GdkBitmap *mask);
static void psdrawlines				(GtkPlotPC *pc,
						 GtkPlotPoint *points, 
						 gint numpoints);
static void psdrawpoint				(GtkPlotPC *pc, 
                				 gdouble x, gdouble y); 
static void psdrawline				(GtkPlotPC *pc,
						 gdouble x0, gdouble y0, 
						 gdouble xf, gdouble yf);
static void psdrawpolygon			(GtkPlotPC *pc,
						 gboolean filled,
						 GtkPlotPoint *points, 
						 gint numpoints); 
static void psdrawrectangle			(GtkPlotPC *pc, 
						 gboolean filled, 
                				 gdouble x, gdouble y, 
						 gdouble width, gdouble height);
static void psdrawcircle			(GtkPlotPC *pc,
						 gboolean filled,
                                                 gdouble x, gdouble y, 
						 gdouble size);
static void psdrawellipse			(GtkPlotPC *pc, 
              					 gboolean filled,
						 gdouble x, gdouble y, 
						 gdouble width, gdouble height); 
static void pssetcolor				(GtkPlotPC *pc, 
						 const GdkColor *color); 
static void pssetlineattr			(GtkPlotPC *pc, 
                                                 gfloat line_width,
                                                 GdkLineStyle line_style,
                                                 GdkCapStyle cap_style,
                                                 GdkJoinStyle join_style);
static void psdrawstring			(GtkPlotPC *pc,
             					 gint x, gint y,
                                                 gint angle,
                                                 const GdkColor *fg,
                                                 const GdkColor *bg,
                                                 gboolean transparent,
                                                 gint border,
                                                 gint border_space,
                                                 gint border_width,
                                                 gint shadow_width,
                                                 const gchar *font,
                                                 gint height,
                                                 GtkJustification just,
                                                 const gchar *text);
static void pssetfont				(GtkPlotPC *pc, 
						 GtkPSFont *psfont,
						 gint height);
static void pssetdash				(GtkPlotPC *pc, 
						 gdouble offset,
						 gdouble *values,
						 gint num_values);
static void psdrawpixmap                        (GtkPlotPC *pc,
                                                 GdkPixmap *pixmap,
                                                 GdkBitmap *mask,
                                                 gint xsrc, gint ysrc,
                                                 gint xdest, gint ydest,
                                                 gint width, gint height,
                                                 gdouble sx, gdouble sy);

static void ps_reencode_font			(FILE *file, char *fontname);
static void color_to_hex                        (GdkColor color,
                                                 gchar string[7]);

/*********************************************************************/
static GtkPlotPCClass *parent_class = NULL;
static gchar *locale = NULL;

GtkType
gtk_plot_ps_get_type (void)
{
  static GtkType pc_type = 0;

  if (!pc_type)
    {
      GtkTypeInfo pc_info =
      {
        "GtkPlotPS",
        sizeof (GtkPlotPS),
        sizeof (GtkPlotPSClass),
        (GtkClassInitFunc) gtk_plot_ps_class_init,
        (GtkObjectInitFunc) gtk_plot_ps_init,
        /* reserved 1*/ NULL,
        /* reserved 2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      pc_type = gtk_type_unique (GTK_TYPE_PLOT_PC, &pc_info);
    }
  return pc_type;
}

static void
gtk_plot_ps_init (GtkPlotPS *ps)
{
  ps->psname = NULL;
  ps->gsaved = FALSE;
  GTK_PLOT_PC(ps)->use_pixmap = FALSE;
}


static void
gtk_plot_ps_class_init (GtkPlotPSClass *klass)
{
  GtkObjectClass *object_class;
  GObjectClass *gobject_class;
  GtkPlotPCClass *pc_class;

  parent_class = gtk_type_class (gtk_plot_pc_get_type ());

  object_class = (GtkObjectClass *) klass;
  gobject_class = (GObjectClass *) klass;
  pc_class = (GtkPlotPCClass *) klass;

  pc_class->init = psinit;
  pc_class->leave = psleave;
  pc_class->set_viewport = pssetviewport;
  pc_class->gsave = psgsave;
  pc_class->grestore = psgrestore;
  pc_class->clip = psclip;
  pc_class->clip_mask = psclipmask;
  pc_class->set_color = pssetcolor;
  pc_class->set_dash = pssetdash;
  pc_class->set_lineattr = pssetlineattr;
  pc_class->draw_point = psdrawpoint;
  pc_class->draw_line = psdrawline;
  pc_class->draw_lines = psdrawlines;
  pc_class->draw_rectangle = psdrawrectangle;
  pc_class->draw_polygon = psdrawpolygon;
  pc_class->draw_circle = psdrawcircle;
  pc_class->draw_ellipse = psdrawellipse;
  pc_class->set_font = pssetfont;
  pc_class->draw_string = psdrawstring;
  pc_class->draw_pixmap = psdrawpixmap;

  object_class->destroy = gtk_plot_ps_destroy;
}

static void
gtk_plot_ps_destroy(GtkObject *object)
{
  GtkPlotPS *ps;

  ps = GTK_PLOT_PS(object);

  if(ps->psname){
    g_free(ps->psname);
    ps->psname = NULL;
  }
}

GtkObject *
gtk_plot_ps_new                         (const gchar *psname,
                                         gint orientation,
                                         gint epsflag,
                                         gint page_size,
                                         gdouble scalex,
					 gdouble scaley)
{
  GtkObject *object;
  GtkPlotPS *ps;

  object = gtk_type_new(gtk_plot_ps_get_type());

  ps = GTK_PLOT_PS(object);

  gtk_plot_ps_construct(ps, psname, orientation, epsflag, page_size, scalex, scaley);

  return (object);
}

void
gtk_plot_ps_construct                   (GtkPlotPS *ps,
					 const gchar *psname,
                                         gint orientation,
                                         gint epsflag,
                                         gint page_size,
                                         gdouble scalex,
					 gdouble scaley)
{
  gint width, height;

  ps->psname = g_strdup(psname);
  ps->orientation = orientation;
  ps->epsflag = epsflag;
  ps->page_size = page_size;
  ps->scalex = scalex;
  ps->scaley = scaley;

  switch (page_size){
   case GTK_PLOT_LEGAL:
        width = GTK_PLOT_LEGAL_W;
        height = GTK_PLOT_LEGAL_H;
        break;
   case GTK_PLOT_A4:
        width = GTK_PLOT_A4_W;
        height = GTK_PLOT_A4_H;
        break;
   case GTK_PLOT_EXECUTIVE:
        width = GTK_PLOT_EXECUTIVE_W;
        height = GTK_PLOT_EXECUTIVE_H;
        break;
   case GTK_PLOT_LETTER:
   default:
        width = GTK_PLOT_LETTER_W;
        height = GTK_PLOT_LETTER_H;
  }

  gtk_plot_ps_set_size(ps, GTK_PLOT_PSPOINTS, width, height);
}

GtkObject *
gtk_plot_ps_new_with_size                       (const gchar *psname,
                                                 gint orientation,
                                                 gint epsflag,
                                                 gint units,
                                                 gdouble width, gdouble height,
						 gdouble scalex, gdouble scaley)
{
  GtkObject *object;
  GtkPlotPS *ps;

  object = gtk_type_new(gtk_plot_ps_get_type());

  ps = GTK_PLOT_PS(object);

  gtk_plot_ps_construct_with_size (ps, psname, orientation, epsflag, units, width, height, scalex, scaley);

  return object;
}

void
gtk_plot_ps_construct_with_size                 (GtkPlotPS *ps,
						 const gchar *psname,
                                                 gint orientation,
                                                 gint epsflag,
                                                 gint units,
                                                 gdouble width, gdouble height,
						 gdouble scalex, gdouble scaley)
{
  gtk_plot_ps_construct(ps, psname, orientation, epsflag, GTK_PLOT_CUSTOM, scalex, scaley);

  gtk_plot_ps_set_size(ps, units, width, height);
}

void
gtk_plot_ps_set_size                            (GtkPlotPS *ps,
                                                 gint units,
                                                 gdouble width,
                                                 gdouble height)
{
  ps->units = units;
  ps->width = width;
  ps->height = height;

  switch(units){
   case GTK_PLOT_MM:
        ps->page_width = (gdouble)width * 2.835;
        ps->page_height = (gdouble)height * 2.835;
        break;
   case GTK_PLOT_CM:
        ps->page_width = width * 28.35;
        ps->page_height = height * 28.35;
        break;
   case GTK_PLOT_INCHES:
        ps->page_width = width * 72;
        ps->page_height = height * 72;
        break;
   case GTK_PLOT_PSPOINTS:
   default:
        ps->page_width = width;
        ps->page_height = height;
   }

   if(ps->orientation == GTK_PLOT_PORTRAIT)
     gtk_plot_pc_set_viewport(GTK_PLOT_PC(ps), ps->page_width, ps->page_height);
   else
     gtk_plot_pc_set_viewport(GTK_PLOT_PC(ps), ps->page_height, ps->page_width);
}

void
gtk_plot_ps_set_scale                           (GtkPlotPS *ps,
                                                 gdouble scalex,
                                                 gdouble scaley)
{
  ps->scalex = scalex;
  ps->scaley = scaley; 
}

static void pssetviewport			(GtkPlotPC *pc,
						 gdouble w, gdouble h) 
{

}

static void pssetlineattr			(GtkPlotPC *pc, 
                                                 gfloat line_width,
                                                 GdkLineStyle line_style,
                                                 GdkCapStyle cap_style,
                                                 GdkJoinStyle join_style)
{
    FILE *psout = GTK_PLOT_PS(pc)->psfile;

    fprintf(psout, "%g slw\n", line_width);
    fprintf(psout, "%d slc\n", abs(cap_style - 1));
    fprintf(psout, "%d slj\n", join_style);

    if(line_style == 0)
            fprintf(psout,"[] 0 sd\n");  /* solid line */
}

static void 
pssetdash(GtkPlotPC *pc,
          gdouble offset, 
          gdouble *values,
          gint num_values)
{
    FILE *psout = GTK_PLOT_PS(pc)->psfile;

    switch(num_values){
      case 0:
        fprintf(psout,"[] 0 sd\n");
        break;
      case 2:
        fprintf(psout, "[%g %g] %g sd\n", values[0], values[1], offset);
        break;
      case 4:
        fprintf(psout, "[%g %g %g %g] %g sd\n", values[0], values[1],
                                                values[2], values[3], 
                                                offset);
        break;
      case 6:
        fprintf(psout, "[%g %g %g %g %g %g] %g sd\n",  values[0], values[1],
                                                       values[2], values[3], 
                                                       values[4], values[5], 
                                                       offset);
        break;
      default:
        break;
    }
}

static void 
psleave(GtkPlotPC *pc)
{
    fprintf(GTK_PLOT_PS(pc)->psfile, "showpage\n");
    fprintf(GTK_PLOT_PS(pc)->psfile, "%%%%Trailer\n");
    fprintf(GTK_PLOT_PS(pc)->psfile, "%%%%EOF\n");
    fclose(GTK_PLOT_PS(pc)->psfile);
    setlocale(LC_NUMERIC, locale);
    g_free(locale);
}

static gboolean 
psinit						(GtkPlotPC *pc)
{
    time_t now;
    FILE *psout;
    GtkPlotPS *ps;

    now = time(NULL);

    locale = g_strdup(setlocale(LC_NUMERIC, NULL));
    setlocale(LC_NUMERIC, "C");

    ps = GTK_PLOT_PS(pc);
    psout = ps->psfile;

    if ((psout = fopen(ps->psname, "w")) == NULL){
       g_warning("ERROR: Cannot open file: %s", ps->psname); 
       return FALSE;
    }

    ps->psfile = psout;

    if(ps->epsflag)
       fprintf (psout, "%%!PS-Adobe-2.0 PCF-2.0\n");
    else
       fprintf (psout, "%%!PS-Adobe-2.0\n");

    fprintf (psout,
             "%%%%Title: %s\n"
             "%%%%Creator: %s v%s Copyright (c) 1999 Adrian E. Feiguin\n"
             "%%%%CreationDate: %s"
             "%%%%Magnification: 1.0000\n",
             ps->psname,
             "GtkPlot", "3.x",
             ctime (&now));

    if(ps->orientation == GTK_PLOT_PORTRAIT)
             fprintf(psout,"%%%%Orientation: Portrait\n");
    else
             fprintf(psout,"%%%%Orientation: Landscape\n");

/*
    if(ps->epsflag)
*/
          fprintf (psout,
                   "%%%%BoundingBox: 0 0 %d %d\n"
                   "%%%%Pages: 1\n"
                   "%%%%EndComments\n",
                   ps->page_width,
                   ps->page_height);


    fprintf (psout,
             "/cp {closepath} bind def\n"
             "/c {curveto} bind def\n"
             "/f {fill} bind def\n"
             "/a {arc} bind def\n"
             "/ef {eofill} bind def\n"
             "/ex {exch} bind def\n"
             "/gr {grestore} bind def\n"
             "/gs {gsave} bind def\n"
             "/sa {save} bind def\n"
             "/rs {restore} bind def\n"
             "/l {lineto} bind def\n"
             "/m {moveto} bind def\n"
             "/rm {rmoveto} bind def\n"
             "/n {newpath} bind def\n"
             "/s {stroke} bind def\n"
             "/sh {show} bind def\n"
             "/slc {setlinecap} bind def\n"
             "/slj {setlinejoin} bind def\n"
             "/slw {setlinewidth} bind def\n"
             "/srgb {setrgbcolor} bind def\n"
             "/rot {rotate} bind def\n"
             "/sc {scale} bind def\n"
             "/sd {setdash} bind def\n"
             "/ff {findfont} bind def\n"
             "/sf {setfont} bind def\n"
             "/scf {scalefont} bind def\n"
             "/sw {stringwidth pop} bind def\n"
             "/tr {translate} bind def\n"

             "/JR {\n"
             " neg 0\n"
             " rmoveto\n"
             "} bind def\n"

             "/JC {\n"
             " 2 div neg 0\n"
             " rmoveto\n"
             "} bind def\n"
  
             "\n/ellipsedict 8 dict def\n"
             "ellipsedict /mtrx matrix put\n"
             "/ellipse\n"
             "{ ellipsedict begin\n"
             "   /endangle exch def\n"
             "   /startangle exch def\n"
             "   /yrad exch def\n"
             "   /xrad exch def\n"
             "   /y exch def\n"
             "   /x exch def"
             "   /savematrix mtrx currentmatrix def\n"
             "   x y tr xrad yrad sc\n"
             "   0 0 1 startangle endangle arc\n"
             "   savematrix setmatrix\n"
             "   end\n"
             "} def\n\n"
    ); 
  
    fprintf(psout,
          "[ /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n"
          "/.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n"
          "/.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n"
          "/.notdef /.notdef /space /exclam /quotedbl /numbersign /dollar /percent /ampersand /quoteright\n"
          "/parenleft /parenright /asterisk /plus /comma /hyphen /period /slash /zero /one\n"
          "/two /three /four /five /six /seven /eight /nine /colon /semicolon\n"          "/less /equal /greater /question /at /A /B /C /D /E\n"
          "/F /G /H /I /J /K /L /M /N /O\n"
          "/P /Q /R /S /T /U /V /W /X /Y\n"
          "/Z /bracketleft /backslash /bracketright /asciicircum /underscore /quoteleft /a /b /c\n"
          "/d /e /f /g /h /i /j /k /l /m\n"
          "/n /o /p /q /r /s /t /u /v /w\n"
          "/x /y /z /braceleft /bar /braceright /asciitilde /.notdef /.notdef /.notdef\n"
          "/.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n"
          "/.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n"
          "/.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n"
          "/space /exclamdown /cent /sterling /currency /yen /brokenbar /section /dieresis /copyright\n"
          "/ordfeminine /guillemotleft /logicalnot /hyphen /registered /macron /degree /plusminus /twosuperior /threesuperior\n"
          "/acute /mu /paragraph /periodcentered /cedilla /onesuperior /ordmasculine /guillemotright /onequarter /onehalf\n"
          "/threequarters /questiondown /Agrave /Aacute /Acircumflex /Atilde /Adieresis /Aring /AE /Ccedilla\n"
          "/Egrave /Eacute /Ecircumflex /Edieresis /Igrave /Iacute /Icircumflex /Idieresis /Eth /Ntilde\n"
          "/Ograve /Oacute /Ocircumflex /Otilde /Odieresis /multiply /Oslash /Ugrave /Uacute /Ucircumflex\n"
          "/Udieresis /Yacute /Thorn /germandbls /agrave /aacute /acircumflex /atilde /adieresis /aring\n"
          "/ae /ccedilla /egrave /eacute /ecircumflex /edieresis /igrave /iacute /icircumflex /idieresis\n"
          "/eth /ntilde /ograve /oacute /ocircumflex /otilde /odieresis /divide /oslash /ugrave\n"
          "/uacute /ucircumflex /udieresis /yacute /thorn /ydieresis] /isolatin1encoding exch def\n");
 
    ps_reencode_font(psout, "Times-Roman");
    ps_reencode_font(psout, "Times-Italic");
    ps_reencode_font(psout, "Times-Bold");
    ps_reencode_font(psout, "Times-BoldItalic");
    ps_reencode_font(psout, "AvantGarde-Book");
    ps_reencode_font(psout, "AvantGarde-BookOblique");
    ps_reencode_font(psout, "AvantGarde-Demi");
    ps_reencode_font(psout, "AvantGarde-DemiOblique");
    ps_reencode_font(psout, "Bookman-Light");
    ps_reencode_font(psout, "Bookman-LightItalic");
    ps_reencode_font(psout, "Bookman-Demi");
    ps_reencode_font(psout, "Bookman-DemiItalic");
    ps_reencode_font(psout, "Courier");
    ps_reencode_font(psout, "Courier-Oblique");
    ps_reencode_font(psout, "Courier-Bold");
    ps_reencode_font(psout, "Courier-BoldOblique");
    ps_reencode_font(psout, "Helvetica");
    ps_reencode_font(psout, "Helvetica-Oblique");
    ps_reencode_font(psout, "Helvetica-Bold");
    ps_reencode_font(psout, "Helvetica-BoldOblique");
    ps_reencode_font(psout, "Helvetica-Narrow");
    ps_reencode_font(psout, "Helvetica-Narrow-Oblique");
    ps_reencode_font(psout, "Helvetica-Narrow-Bold");
    ps_reencode_font(psout, "Helvetica-Narrow-BoldOblique");
    ps_reencode_font(psout, "NewCenturySchoolbook-Roman");
    ps_reencode_font(psout, "NewCenturySchoolbook-Italic");
    ps_reencode_font(psout, "NewCenturySchoolbook-Bold");
    ps_reencode_font(psout, "NewCenturySchoolbook-BoldItalic");
    ps_reencode_font(psout, "Palatino-Roman");
    ps_reencode_font(psout, "Palatino-Italic");
    ps_reencode_font(psout, "Palatino-Bold");
    ps_reencode_font(psout, "Palatino-BoldItalic");
    ps_reencode_font(psout, "Symbol");
    ps_reencode_font(psout, "ZapfChancery-MediumItalic");
    ps_reencode_font(psout, "ZapfDingbats");
    
    fprintf(psout,"%%%%EndProlog\n%%%%BeginSetup\n"
                "%%%%PageBoundingBox: 0 0 %d %d\n"
                "%%%%PageOrientation: %s\n"
                "%%%%PaperSize: %d %d\n",
                 ps->page_width,
                 ps->page_height, 
                (ps->orientation == GTK_PLOT_PORTRAIT) ? "Portrait":"Landscape",
                 ps->page_width,
                 ps->page_height); 

    if(ps->orientation == GTK_PLOT_PORTRAIT)
             fprintf(psout, "%g %g scale\n",
                            ps->scalex, ps->scaley);

    if(ps->orientation == GTK_PLOT_LANDSCAPE)
             fprintf(psout, "%g %g scale\n"
                            "90 rotate \n"
                            "0 %d translate\n",
                            ps->scalex, ps->scaley,
                            -ps->page_width);

    fprintf(psout,"%%%%EndSetup\n\n\n");

    return TRUE;
}

static void ps_reencode_font(FILE *file, char *fontname)
{
  /* Don't reencode the Symbol font, as it doesn't work in latin1 encoding.
   * Instead, just define Symbol-latin1 to be the same as Symbol. */
  if (!strcmp(fontname, "Symbol"))
    fprintf(file,
            "/%s-latin1\n"
            "    /%s findfont\n"
            "definefont pop\n", fontname, fontname);
  else
    fprintf(file,
            "/%s-latin1\n"
            "    /%s findfont\n"
            "    dup length dict begin\n"
            "   {1 index /FID ne {def} {pop pop} ifelse} forall\n"
            "   /Encoding isolatin1encoding def\n"
            "    currentdict end\n"
            "definefont pop\n", fontname, fontname);
}

static void pssetcolor(GtkPlotPC *pc, const GdkColor *color)
{
    FILE *psout = GTK_PLOT_PS(pc)->psfile;

    fprintf(psout, "%g %g %g setrgbcolor\n",
	    (gdouble) color->red / 65535.0,
	    (gdouble) color->green / 65535.0,
	    (gdouble) color->blue / 65535.0);
}

static void
psdrawpoint(GtkPlotPC *pc, gdouble x, gdouble y)
{
  FILE *psout = GTK_PLOT_PS(pc)->psfile;

  y = GTK_PLOT_PS(pc)->page_height - y;
  fprintf(psout, "n\n");
  fprintf(psout, "%g %g m\n", x, y);
  fprintf(psout, "%g %g l\n", x, y);
  fprintf(psout, "s\n");
}

static void
psdrawlines(GtkPlotPC *pc, GtkPlotPoint *points, gint numpoints)
{
  gint i, page_height = GTK_PLOT_PS(pc)->page_height;
  FILE *psout = GTK_PLOT_PS(pc)->psfile;
 
  fprintf(psout,"n\n");
  fprintf(psout,"%g %g m\n", points[0].x, page_height - points[0].y);
  for(i = 1; i < numpoints; i++)
        fprintf(psout,"%g %g l\n", points[i].x, page_height - points[i].y);

  fprintf(psout,"s\n");
}

static void
psdrawpolygon(GtkPlotPC *pc, gboolean filled, GtkPlotPoint *points, gint numpoints)
{
  gint i, page_height = GTK_PLOT_PS(pc)->page_height;
  FILE *psout = GTK_PLOT_PS(pc)->psfile;

  fprintf(psout,"n\n");
  fprintf(psout,"%g %g m\n", points[0].x, page_height - points[0].y);
  for(i = 1; i < numpoints; i++)
      fprintf(psout,"%g %g l\n", points[i].x, page_height - points[i].y);

  if(filled)
     fprintf(psout,"f\n");
  else
     fprintf(psout,"cp\n");

  fprintf(psout,"s\n");
}

static void psdrawline(GtkPlotPC *pc, gdouble x0, gdouble y0, gdouble xf, gdouble yf)
{
  FILE *psout = GTK_PLOT_PS(pc)->psfile;

  fprintf(psout, "%g %g m\n", x0, GTK_PLOT_PS(pc)->page_height - y0);
  fprintf(psout, "%g %g l\n", xf, GTK_PLOT_PS(pc)->page_height - yf);
  fprintf(psout, "s\n");
}

static void
psdrawrectangle(GtkPlotPC *pc, gboolean filled, 
                gdouble x, gdouble y, gdouble width, gdouble height)
{
  GtkPlotPoint point[4];

  point[0].x = x;
  point[0].y = y;
  point[1].x = x + width;
  point[1].y = y;
  point[2].x = x + width;
  point[2].y = y + height;
  point[3].x = x;
  point[3].y = y + height;

  psdrawpolygon(pc, filled, point, 4);
}

static void
psdrawcircle(GtkPlotPC *pc, gboolean filled, gdouble x, gdouble y, gdouble size)
{
  FILE *psout = GTK_PLOT_PS(pc)->psfile;

  y = GTK_PLOT_PS(pc)->page_height - y;
  fprintf(psout,"n %g %g %g %g 0 360 ellipse\n", 
          x, y, size / 2., size / 2.);

  if(filled)
     fprintf(psout,"f\n");

  fprintf(psout,"s\n");
}

static void
psdrawellipse(GtkPlotPC *pc, 
              gboolean filled, 
              gdouble x, gdouble y, 
              gdouble width, gdouble height)
{
  FILE *psout = GTK_PLOT_PS(pc)->psfile;

  y = GTK_PLOT_PS(pc)->page_height - y;
  fprintf(psout,"n %g %g %g %g 0 360 ellipse\n", 
          x+width/2., y-height/2., 
          width/2., height/2.);

  if(filled)
     fprintf(psout,"f\n");

  fprintf(psout,"s\n");
}

static void
psoutputstring (GtkPlotPC *pc,
		GtkPSFont *psfont,
		GtkPSFont *latin_psfont,
		gint height,
		const gchar *wstring,
		const gchar *addstring)
{
  const gchar *p;
  gint curcode = 0;
  gchar begin[] = { 0, '(', '<' };
  gchar end[] = { 0, ')', '>' };
  GtkPSFont *fonts[3];
  FILE *out = GTK_PLOT_PS(pc)->psfile;
  const gchar *c = NULL;
 
  fonts[0] = NULL;
  fonts[1] = latin_psfont;
  fonts[2] = psfont;

  p = wstring;
  
  if (psfont->i18n_latinfamily) {
/*    gint code; */ /* 0..neither 1..latin 2..i18n */
/*
    gchar wcs[2];
    while (*p) {
      code = (*p >= 0 && *p <= 0x7f) ? 1 : 2;
      if (curcode && curcode != code)
	fprintf(out, "%c %s\n", end[curcode], addstring);
      if (curcode != code) {
	pssetfont(pc, fonts[code], height);
	fputc(begin[code], out);
      }
      
      curcode = code;

      wcs[0] = *p++;
      wcs[1] = 0;
      c = wcs;
      
      if (code == 2) {
	while (*c)
	  fprintf(out, "%02x", (unsigned char)(*c++));
      } else {
	if (*c == '(' || *c == ')')
	  fputc('\\', out);
	fputc(*c, out);
      }
    }
*/
  } else {
    c = wstring;
    
    pssetfont(pc, psfont, height);
    fputc(begin[1], out);
    curcode = 1;

    while (*c) {
      const gchar *aux2 = c;
      if (*c == '(' || *c == ')')
	fputc('\\', out);
     
      if(++aux2 != g_utf8_next_char(c)){
        fprintf(out, ") show <%02x> show (", (unsigned char)(*++c));
        c++;
      } else {
        fputc(*c, out);
        c = g_utf8_next_char(c);
      } 
    }
  }

  if (curcode)
    fprintf(out, "%c %s\n", end[curcode], addstring);
}

static void
psdrawstring	(GtkPlotPC *pc,
             	 gint x, gint y,
                 gint angle,
                 const GdkColor *fg,
                 const GdkColor *bg,
                 gboolean transparent,
                 gint border,
                 gint border_space,
                 gint border_width,
                 gint shadow_width,
                 const gchar *font,
                 gint font_height,
                 GtkJustification justification,
                 const gchar *text)

{
  gchar *currfont;
  const gchar *c;
  GtkPSFont *psfont, *base_psfont, *latin_psfont = NULL;
  gint curcnt = 0, offset = 0;
  gint numf;
  gdouble scale;
  gboolean italic, bold;
  gboolean special = FALSE;
  GList *family;
  FILE *psout;
  gint twidth, theight, tdescent, tascent;
  gint tx, ty, width, height; 
  gint i;
  const gchar *aux, *xaux, *wtext, *lastchar = NULL;
  gchar *curstr, bkspchar[3];
  gchar num[4];

  if (text == NULL || strlen(text) == 0) return;

  psout = GTK_PLOT_PS(pc)->psfile;

  gtk_psfont_get_families(&family, &numf);
  base_psfont = psfont = gtk_psfont_get_by_name(font);
  italic = psfont->italic;
  bold = psfont->bold;

  currfont = psfont->psname;

  if (psfont->i18n_latinfamily) {
    latin_psfont = gtk_psfont_get_by_family(psfont->i18n_latinfamily, italic,
					    bold);
  }

  gtk_plot_text_get_area(text, angle, justification, font, font_height,
                         &tx, &ty, &width, &height);

  tx += x;
  ty += y;

  if(!transparent){
    pssetcolor(pc, bg);
    gtk_plot_pc_draw_rectangle(pc,
                         TRUE,
                         tx - border_space, ty - border_space,
                         width + 2*border_space, height + 2*border_space);
  }
/* border */

  pssetcolor(pc, fg);
  pssetdash(pc, 0, NULL, 0);
  pssetlineattr(pc, border_width, 0, 0, 0);
 
  switch(border){
    case GTK_PLOT_BORDER_SHADOW:
      psdrawrectangle(pc,
                         TRUE, 
                         tx - border_space + shadow_width,
                         ty + height + border_space, 
                         width + 2 * border_space, shadow_width);
      psdrawrectangle(pc,
                         TRUE, 
                         tx + width + border_space, 
                         ty - border_space + shadow_width, 
                         shadow_width, height + 2 * border_space);
    case GTK_PLOT_BORDER_LINE: 
      psdrawrectangle(pc,
                         FALSE, 
                         tx - border_space, ty - border_space, 
                         width + 2*border_space, height + 2*border_space);
    case GTK_PLOT_BORDER_NONE:
    default:
        break;
  }


  gtk_plot_text_get_size(text, angle, psfont->psname, font_height, 
                         &twidth, &theight, &tascent, &tdescent);

  psgsave(pc);
  fprintf(psout, "%d %d translate\n", x, GTK_PLOT_PS(pc)->page_height - y);
  fprintf(psout, "%d rotate\n", angle);

  fprintf(psout, "0 0 m\n");
  
  if (psfont->i18n_latinfamily)
    special = TRUE;

  c = text;
  while(c && *c != '\0' && *c != '\n') {
     if(*c == '\\'){
         c = g_utf8_next_char(c);
         switch(*c){
           case '0': case '1': case '2': case '3':
           case '4': case '5': case '6': case '7': case '9':
           case '8': case'g': case 'B': case 'b': case 'x': case 'N':
           case 's': case 'S': case 'i': case '-': case '+': case '^':
             special = TRUE;
             break;
           default:
             break;
         }
     } else {
         c = g_utf8_next_char(c);
     }
  }

  if(special){
    switch (justification) {
      case GTK_JUSTIFY_LEFT:
        break;
      case GTK_JUSTIFY_RIGHT:
        if(angle == 0 || angle == 180)
               fprintf(psout, "%d JR\n", twidth);
        else
               fprintf(psout, "%d JR\n", theight);
        break;
      case GTK_JUSTIFY_CENTER:
      default:
        if(angle == 0 || angle == 180)
               fprintf(psout, "%d JC\n", twidth);
        else
               fprintf(psout, "%d JC\n", theight);
        break;
    }
  } else {
    pssetfont(pc, psfont, font_height);
    
    switch (justification) {
      case GTK_JUSTIFY_LEFT:
        break;
      case GTK_JUSTIFY_RIGHT:
        fprintf(psout, "(%s) sw JR\n", text);
        break;
      case GTK_JUSTIFY_CENTER:
      default:
        fprintf(psout, "(%s) sw JC\n", text);
        break;
    }
    fprintf(psout, "(%s) show\n", text);

    psgrestore(pc);  
    fprintf(psout, "n\n");  
    return;
  }

  i = g_utf8_strlen(text, -1) + 2;
  curstr = g_malloc0(sizeof(gchar) * i);
  aux = wtext = text; 

  scale = font_height;
  curcnt = 0;

  while(aux && *aux != '\0' && *aux != '\n') {
     if(*aux == '\\'){
         aux = g_utf8_next_char(aux);
         switch(*aux){
           case '0': case '1': case '2': case '3':
           case '4': case '5': case '6': case '7': case '9':
                  curstr[curcnt] = 0;
		  psoutputstring(pc, psfont, latin_psfont, (gint)scale,
				 curstr, "show");
                  curcnt = 0;
                  psfont = gtk_psfont_get_by_family((gchar *)g_list_nth_data(family, *aux-'0'), italic, bold);
		  aux = g_utf8_next_char(aux);
                  break;
           case '8':case 'g':
                  curstr[curcnt] = 0;
                  psoutputstring(pc, psfont, latin_psfont, (gint)scale,
				 curstr, "show");
                  curcnt = 0;
                  psfont = gtk_psfont_get_by_family("Symbol", italic, bold);
                  aux = g_utf8_next_char(aux);
                  break;
           case 'B':
                  curstr[curcnt] = 0;
                  psoutputstring(pc, psfont, latin_psfont, (gint)scale,
				 curstr, "show");
                  curcnt = 0;
  		  bold = TRUE;
                  psfont = gtk_psfont_get_by_family(psfont->family, italic, bold);
		  if (psfont->i18n_latinfamily)
		    latin_psfont = gtk_psfont_get_by_family(psfont->i18n_latinfamily, italic, bold);
		  aux = g_utf8_next_char(aux);
                  break;
           case 'x':
                  xaux = aux + 1;
                  for (i=0; i<3; i++){
		    if (xaux[i] >= '0' && xaux[i] <= '9')
		      num[i] = xaux[i];
		    else
		      break;
                  }
                  if (i < 3){
                     aux = g_utf8_next_char(aux);
                     break;
                  }
                  num[3] = '\0';
		  
		  i = atoi(num);
		  g_snprintf(num, 4, "%o", i % (64 * 8));

		  curstr[curcnt++] = '\\';
		  i = 0;
		  while (num[i]) {
		    curstr[curcnt++] = num[i++];
		  }
		  
                  aux += 4;
                  break;
           case 'i':
	          curstr[curcnt] = 0;
                  psoutputstring(pc, psfont, latin_psfont, (gint)scale,
				 curstr, "show");
                  curcnt = 0;
		  italic = TRUE;
                  psfont = gtk_psfont_get_by_family(psfont->family, italic, bold);
		  if (psfont->i18n_latinfamily)
		    latin_psfont = gtk_psfont_get_by_family(psfont->i18n_latinfamily, italic, bold);
		  aux = g_utf8_next_char(aux);
                  break;
           case 's':case '_':
                  curstr[curcnt] = 0;
                  psoutputstring(pc, psfont, latin_psfont, (gint)scale,
				 curstr, "show");
                  curcnt = 0;
                  scale = 0.6 * font_height;
		  offset -= (gint)scale / 2;
                  fprintf(psout, "0 %d rmoveto\n", -((gint)scale / 2));
                  aux = g_utf8_next_char(aux);
                  break;
           case 'S':case '^':
                  curstr[curcnt] = 0;
                  psoutputstring(pc, psfont, latin_psfont, (gint)scale,
				 curstr, "show");
                  curcnt = 0;
                  scale = 0.6 * font_height;
		  offset += 0.5*font_height;
                  fprintf(psout, "0 %d rmoveto\n", (gint)(0.5*font_height));
                  aux = g_utf8_next_char(aux);
                  break;
           case 'N':
                  curstr[curcnt] = 0;
                  psoutputstring(pc, psfont, latin_psfont, (gint)scale,
				 curstr, "show");
                  curcnt = 0;
		  psfont = base_psfont;
		  italic = psfont->italic;
		  bold = psfont->bold;
		  if (psfont->i18n_latinfamily) {
		    latin_psfont = gtk_psfont_get_by_family(psfont->i18n_latinfamily,
							     italic, bold);
		  }
                  scale = font_height;
                  fprintf(psout, "0 %d rmoveto\n", -offset);
                  offset = 0;
                  aux = g_utf8_next_char(aux);
                  break;
           case 'b':
                  curstr[curcnt] = '\0';
                  psoutputstring(pc, psfont, latin_psfont, (gint)scale,
				 curstr, "show");
                  curcnt = 0;
                  if (lastchar) {
                      const gchar *aux2 = lastchar;
                      bkspchar[0] = *lastchar;
                      lastchar = g_utf8_prev_char(lastchar);
		      bkspchar[1] = 0;
                      if(--aux2 != lastchar){
                        bkspchar[1] = *lastchar;
                        lastchar = g_utf8_prev_char(lastchar);
		        bkspchar[2] = 0;
                      }
                  } else {
                      bkspchar[0] = 'X';
                      lastchar = NULL;
                  }
		  psoutputstring(pc, psfont, latin_psfont, (gint)scale,
				 bkspchar,
				 "stringwidth pop 0 exch neg exch rmoveto");
                  aux = g_utf8_next_char(aux);
                  break;
           case '-':
                  curstr[curcnt] = 0;
		  psoutputstring(pc, psfont, latin_psfont, (gint)scale,
				 curstr, "show");
                  curcnt = 0;
                  scale -= 3;
                  if (scale < 6) {
                      scale = 6;
                  }
                  aux = g_utf8_next_char(aux);
                  break;
           case '+':
                  curstr[curcnt] = 0;
		  psoutputstring(pc, psfont, latin_psfont, (gint)scale,
				 curstr, "show");
                  curcnt = 0;
                  scale += 3;
                  aux = g_utf8_next_char(aux);
                  break;
           default:
                  if(aux && *aux != '\0' && *aux != '\n'){
                    curstr[curcnt++] = *aux;
                    aux = g_utf8_next_char(aux);
                  }
                  break;
         }
     } else {
       if(aux && *aux != '\0' && *aux != '\n'){
                const gchar *aux2 = aux;
                if(g_utf8_next_char(aux) != ++aux2){
                  curstr[curcnt++] = *aux++;
//                  aux = g_utf8_next_char(aux);
                  curstr[curcnt++] = *aux++;
                } else {
                  curstr[curcnt++] = *aux;
		  lastchar = aux;
                  aux = g_utf8_next_char(aux);
                }
       }
     }
  }
  curstr[curcnt] = 0;
  psoutputstring(pc, psfont, latin_psfont, (gint)scale, curstr, "show");

  psgrestore(pc);  
  fprintf(psout, "n\n");  

  g_free(curstr);
}

static void
pssetfont(GtkPlotPC *pc, GtkPSFont *psfont, gint height)
{
  FILE *psout = GTK_PLOT_PS(pc)->psfile;

  if (psfont->i18n_latinfamily && psfont->vertical)
    fprintf(psout,
	    "/%s ff [0 1 -1 0 0 0.3] makefont [%d 0 0 %d 0 0] makefont sf\n",
	    psfont->psname, height, height);
  else
    fprintf(psout, "/%s-latin1 ff %g scf sf\n", psfont->psname, (double)height);

}


static void
psgsave(GtkPlotPC *pc)
{
  GtkPlotPS *ps;
  FILE *psout;

  ps = GTK_PLOT_PS(pc);

  psout = ps->psfile;

  fprintf(psout,"gsave\n");
  ps->gsaved = TRUE;
}

static void
psgrestore(GtkPlotPC *pc)
{
  GtkPlotPS *ps;
  FILE *psout;

  ps = GTK_PLOT_PS(pc);

  psout = ps->psfile;

/*
  if(!ps->gsaved) return;
*/

  fprintf(psout,"grestore\n");
  ps->gsaved = FALSE;
}

static void
psclipmask(GtkPlotPC *pc, gdouble x, gdouble y, const GdkBitmap *mask)
{
  FILE *psout = GTK_PLOT_PS(pc)->psfile;
  gint width, height;
  gint px, py;
  gint npoints = 0;
  gint i, page_height = GTK_PLOT_PS(pc)->page_height;
  GtkPlotVector *points;
  GdkImage *image;

  if(!mask){ 
    fprintf(psout,"grestore\n");
    return;
  }

  gdk_window_get_size((GdkWindow *)mask, &width, &height);
  image = gdk_image_get((GdkWindow *)mask, 0, 0, width, height);

  points = (GtkPlotVector *)g_malloc(width*height*sizeof(GtkPlotVector));

  for(px = 0; px < width; px++){
    for(py = 0; py < height; py++){
      if(gdk_image_get_pixel(image, px, py)){
        points[npoints].x = px; 
        points[npoints].y = py; 
        npoints++;
        break;
      }
    } 
  }
  for(py = points[npoints-1].y; py < height; py++){
    for(px = width - 1; px >= 0; px--){
      if(gdk_image_get_pixel(image, px, py)){
        points[npoints].x = px; 
        points[npoints].y = py; 
        npoints++;
        break;
      }
    } 
  }
  for(px = points[npoints-1].x; px >= 0; px--){
    for(py = height - 1; py >= 0; py--){
      if(gdk_image_get_pixel(image, px, py)){
        points[npoints].x = px; 
        points[npoints].y = py; 
        npoints++;
        break;
      }
    } 
  }
  for(py = points[npoints-1].y; py >= 0; py--){
    for(px = 0; px < width; px++){
      if(gdk_image_get_pixel(image, px, py)){
        points[npoints].x = px; 
        points[npoints].y = py; 
        npoints++;
        break;
      }
    } 
  }

  fprintf(psout,"gsave\n");

  fprintf(psout,"n\n");
  fprintf(psout,"%g %g m\n", x + points[0].x, page_height - y - points[0].y);
  for(i = 1; i < npoints; i++)
      fprintf(psout,"%g %g l\n", x + points[i].x, page_height - y - points[i].y);

  fprintf(psout,"cp\n");

  fprintf(psout,"clip\n");

  g_free(points);
  gdk_image_destroy(image);
}

static void
psclip(GtkPlotPC *pc, const GdkRectangle *clip)
{
  FILE *psout = GTK_PLOT_PS(pc)->psfile;

  if(!clip){ 
    fprintf(psout,"grestore\n");
    return;
  }

  fprintf(psout,"gsave\n");
  fprintf(psout,"%d %d %d %d rectclip\n", 
                  clip->x,
                  GTK_PLOT_PS(pc)->page_height - clip->y - clip->height,
                  clip->width,  
                  clip->height);
}

/* TODO: FIXME */

static void 
psdrawpixmap  (GtkPlotPC *pc,
               GdkPixmap *pixmap,
               GdkBitmap *mask,
               gint xsrc, gint ysrc,
               gint xdest, gint ydest,
               gint width, gint height,
               gdouble scale_x, gdouble scale_y)
{
  FILE *psout = GTK_PLOT_PS(pc)->psfile;
  GdkColormap *colormap;

  colormap = gdk_colormap_get_system ();

  fprintf(psout, "gsave\n");
  if(pixmap){
    GdkImage *image;
    gint x, y;

    image = gdk_image_get(pixmap,
                          xsrc, ysrc,
                          width, height);

    if(mask) gtk_plot_pc_clip_mask(pc, xdest, ydest, mask);
    ydest = GTK_PLOT_PS(pc)->page_height - ydest - height;

    fprintf(psout, "%d %g translate\n", xdest, ydest + height * scale_y);
    fprintf(psout, "%g %g scale\n",width * scale_x, height * scale_y);
    fprintf(psout, "%d %d 8 [%d 0 0 %d 0 %d]\n",width, height, width, height, height);
    fprintf(psout, "/scanline %d 3 mul string def\n", width);
    fprintf(psout, "{ currentfile scanline readhexstring pop } false 3\n");
    fprintf(psout, "colorimage\n");

    for(y = height - 1; y >= 0; y--){
      for(x = 0; x < width; x++){
        GdkColor color;
        gchar string[7];

        color.pixel = gdk_image_get_pixel(image, x, y);
	gdk_colormap_query_color(colormap, color.pixel, &color);
        color_to_hex(color, string);
        fprintf(psout,"%s",string);
        if(fmod(x + 1, 13) == 0) fprintf(psout, "\n");
      }
      fprintf(psout,"\n");
    }

    gdk_image_destroy(image);
    if(mask) gtk_plot_pc_clip_mask(pc, xdest, ydest, NULL);
  }

  fprintf(psout, "grestore\n");
}

static void
color_to_hex(GdkColor color, gchar string[7])
{
  gint n;
  gint aux;

  aux = color.red / 256;
  n=aux/16;
  aux-=n*16;
  if(n < 10)
    string[0]='0'+n;
  else
    string[0]='A'+n-10;
  n = aux;
  if(n < 10)
    string[1]='0'+n;
  else
    string[1]='A'+n-10;

  aux = color.green / 256;
  n=aux/16;
  aux-=n*16;
  if(n < 10)
    string[2]='0'+n;
  else
    string[2]='A'+n-10;
  n = aux;
  if(n < 10)
    string[3]='0'+n;
  else
    string[3]='A'+n-10;

  aux = color.blue / 256;
  n=aux/16;
  aux-=n*16;
  if(n < 10)
    string[4]='0'+n;
  else
    string[4]='A'+n-10;
  n = aux;
  if(n < 10)
    string[5]='0'+n;
  else
    string[5]='A'+n-10;

  string[6]='\0';
}



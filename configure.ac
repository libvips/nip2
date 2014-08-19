# Process this file with autoconf to produce a configure script.

AC_INIT([nip2], [7.40.4])

# foreign stops complaints about a missing README (we use README.md instead)
# and missing INSTALL (the standard Gnu INSTALL is not very useful)
AM_INIT_AUTOMAKE([-Wall foreign])

AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR([m4])

dnl
dnl We do the version number components as m4 macros
dnl so that we can base configure --help output off
dnl of them.
dnl

m4_define([nip_major_version], [7])
m4_define([nip_minor_version], [40])
m4_define([nip_micro_version], [4])
m4_define([nip_version],
          [nip_major_version.nip_minor_version.nip_micro_version])

MAJOR_VERSION=nip_major_version()
MINOR_VERSION=nip_minor_version()
MICRO_VERSION=nip_micro_version()

AC_DEFINE_UNQUOTED(MAJOR_VERSION, $MAJOR_VERSION, [Major version number])
AC_DEFINE_UNQUOTED(MINOR_VERSION, $MINOR_VERSION, [Minor version number])
AC_DEFINE_UNQUOTED(MICRO_VERSION, $MICRO_VERSION, [Micro version number])

AC_CANONICAL_HOST

AC_MSG_CHECKING([for native Win32])
case "$host" in
  *-*-mingw*)
    nip_os_win32=yes
    ;;
  *)
    nip_os_win32=no
    ;;
esac
AC_MSG_RESULT([$nip_os_win32])

if test x"$nip_os_win32" = "xyes"; then
  AC_DEFINE(OS_WIN32,1,[native win32])

  # makes gcc use win native alignment
  IP_CFLAGS="-mms-bitfields $IP_CFLAGS"
fi

# src/Makeile.am uses this to add an icon to the .exe
AM_CONDITIONAL(OS_WIN32, test x"$nip_os_win32" = "xyes")

AC_MSG_CHECKING([for Mac OS X])
case "$host" in
  *-*-darwin*)
    nip_os_darwin=yes
    ;;
  *)
    nip_os_darwin=no
    ;;
esac
AC_MSG_RESULT([$nip_os_darwin])
if test x"$nip_os_darwin" = "xyes"; then
  AC_DEFINE(OS_DARWIN,1,[native Mac OS X])
fi

# disable debugging by default for production releases
m4_define([debug_default],
  m4_if(m4_eval(nip_minor_version() % 2), [1], [yes], [no]))

AC_ARG_ENABLE(debug,
  AC_HELP_STRING([--enable-debug=@<:@no/minimum/yes@:>@],
    [turn on debugging @<:@default=debug_default()@:>@]),,
  enable_debug=debug_default())

if test "x$enable_debug" = "xyes"; then
  NIP_DEBUG_FLAGS="-DDEBUG_FATAL -DDEBUG_LEAK"
else
  NIP_DEBUG_FLAGS="-DG_DISABLE_CAST_CHECKS"

  if test "x$enable_debug" = "xno"; then
    NIP_DEBUG_FLAGS="$GLIB_DEBUG_FLAGS -DG_DISABLE_ASSERT -DG_DISABLE_CHECKS"
  fi
fi

IP_CFLAGS="$NIP_DEBUG_FLAGS $IP_CFLAGS"

# we want largefile support, if possible
AC_SYS_LARGEFILE

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LEX
# we must have flex or lex
if test x"$LEX" = x:; then 
  AC_MSG_ERROR([lex/flex not found: $PACKAGE requires one of these])
fi
IP_LIBS="$IP_LIBS $LEXLIB"
AC_PROG_INSTALL
AC_PROG_LN_S
AC_CHECK_TOOL(WINDRES, windres)
AC_CHECK_TOOL(DLLWRAP, dllwrap)
AC_CHECK_TOOL(DLLTOOL, dlltool)
AC_CHECK_TOOL(OBJDUMP, objdump)
AC_CHECK_TOOL(RANLIB, ranlib)
AC_CHECK_TOOL(STRIP, strip)
AC_CHECK_TOOL(BISON, bison)
# we have to have bison :-( maybe we could ship the generated .c/.h files? not
# clear on their portability
if test x"$BISON" = x; then 
  AC_MSG_ERROR([bison not found: $PACKAGE uses bison-only features])
fi
AC_CHECK_TOOL(AR, ar)
AC_CHECK_TOOL(AS, as)
AC_CHECK_TOOL(LD, ld)
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

# dmalloc option
AM_WITH_DMALLOC

# i18n
GETTEXT_PACKAGE=nip2
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE",
	[The prefix for our gettext translation domains.])
# ALL_LINGUAS="en_GB malkovich"
ALL_LINGUAS="en_GB"
AM_GLIB_GNU_GETTEXT

# check for flex ... nip needs to adjust itself a bit
if test "${LEX}" = "flex"; then
  AC_DEFINE(HAVE_FLEX,1,[using flex, rather than lex])
fi

# flex >= 2.5.36 uses a nonstandard type for yyleng
AC_MSG_CHECKING([whether yyleng is yy_size_t])
cat > conftest.l <<EOF
%%
%%
yy_size_t yyleng;
EOF
$LEX conftest.l
AC_COMPILE_IFELSE([AC_LANG_DEFINES_PROVIDED [`cat $LEX_OUTPUT_ROOT.c`]], [
  AC_MSG_RESULT(yes)
  AC_DEFINE(YYLENG_IS_YY_SIZE_T,1,
    [Define to 1 if lex declares yyleng to be yy_size_t.])
], [
  AC_MSG_RESULT(no)
])
rm -f conftest.l $LEX_OUTPUT_ROOT.c

# get packages we need
# gtk before 2.4.9 crashes with the way we use combobox :-(
# vips before 7.30 used "vips-x.y" as the pkg name
PKG_CHECK_MODULES(REQUIRED_PACKAGES, 
  gthread-2.0 gtk+-2.0 >= 2.4.9 libxml-2.0 vips >= 7.30)
IP_CFLAGS="$REQUIRED_PACKAGES_CFLAGS $IP_CFLAGS"
IP_LIBS="$REQUIRED_PACKAGES_LIBS $IP_LIBS"

# gdk_window_set_opacity() was added in gtk 2.12
PKG_CHECK_EXISTS(gtk+-2.0 >= 2.12,
  [nip_set_opacity=yes],
  [nip_set_opacity=no]
)

if test x"$nip_set_opacity" = x"yes"; then
  AC_DEFINE(HAVE_SET_OPACITY,1,[define if you have gdk_window_set_opacity()])
fi

# GtkInfoBar was added in gtk 2.18
PKG_CHECK_EXISTS(gtk+-2.0 >= 2.18,
  [nip_use_infobar=yes],
  [nip_use_infobar=no]
)

if test x"$nip_use_infobar" = x"yes"; then
  AC_DEFINE(USE_INFOBAR,1,[define if you have GtkInfoBar])
fi

# notebook action widgets came in 2.20
PKG_CHECK_EXISTS(gtk+-2.0 >= 2.20,
  [nip_use_notebook_action=yes],
  [nip_use_notebook_action=no]
)

if test x"$nip_use_notebook_action" = x"yes"; then
  AC_DEFINE(USE_NOTEBOOK_ACTION,1,[define if you have gtk_notebook_set_action_widget()])
fi

# notebook group names widgets came in 2.24
PKG_CHECK_EXISTS(gtk+-2.0 >= 2.24,
  [nip_use_notebook_group_name=yes],
  [nip_use_notebook_group_name=no]
)

if test x"$nip_use_notebook_group_name" = x"yes"; then
  AC_DEFINE(USE_NOTEBOOK_GROUP_NAME,1,[define if you have gtk_notebook_set_group_name()])
fi

# GRegex was added in glib-2.14
# we need it for regex searching in the program window
PKG_CHECK_EXISTS(glib-2.0 >= 2.14,
  [nip_use_gregex=yes],
  [nip_use_gregex=no]
)

if test x"$nip_use_gregex" = x"yes"; then
  AC_DEFINE(HAVE_GREGEX,1,[define if you have GRegex])
fi

# Check for the function strccpy in libgen
AC_CHECK_HEADER(libgen.h, 
  AC_CHECK_LIB(gen, strccpy, 
    AC_DEFINE(HAVE_STRCCPY,1,[have strccpy() in -lgen]) 
    IP_LIBS="$IP_LIBS -lgen" 
  ), 
)

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(limits.h pwd.h fnmatch.h sys/statvfs.h sys/vfs.h sys/mount.h sys/resource.h sys/wait.h malloc.h sys/time.h sys/param.h unistd.h)

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_FNMATCH
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(geteuid getcwd getpwnam getrlimit getpwent getwd putenv regcomp strcspn strspn strstr)

# need fftw so we load and unload wisdom on startup/shutdown
# no need to add to libs (pkg-config will do this for us when we ask for vips)
# we do need to add to CFLAGS though
AC_ARG_WITH([fftw3], AS_HELP_STRING([--without-fftw3], [build without fftw3 (default: test)]))

if test "x$with_fftw3" != "xno"; then
  PKG_CHECK_MODULES(FFTW3, fftw3,
    [AC_DEFINE(HAVE_FFTW3,1,[define if you have fftw3 installed.])
     with_fftw3=yes
    ],
    [AC_MSG_WARN([fftw3 not found; disabling fftw support])
     with_fftw3=no
    ])
  IP_CFLAGS="$FFTW_INCLUDES $FFTW3_CFLAGS $IP_CFLAGS"
fi

# optional ... use libgoffice to draw plots
# pretty basic functionality, really, but we need to be able to build without
# it for testing
# libgoffice 0.6.x is too buggy, build 0.7.x to get the 0.8 package
AC_ARG_WITH([libgoffice], AS_HELP_STRING([--without-libgoffice], [build without libgoffice (default: test)]))

if test "x$with_libgoffice" != "xno"; then
  PKG_CHECK_MODULES(LIBGOFFICE, libgoffice-0.8,
    [AC_DEFINE(HAVE_LIBGOFFICE,1,[define if you have libgoffice installed.])
     with_libgoffice=yes
    ],
    [AC_MSG_WARN([libgoffice not found; disabling plot display])
     with_libgoffice=no
    ])
  IP_CFLAGS="$LIBGOFFICE_CFLAGS $IP_CFLAGS"
  IP_LIBS="$LIBGOFFICE_LIBS $IP_LIBS"
fi

# use libgsf to save goffice plots to files
AC_ARG_WITH([libgsf], AS_HELP_STRING([--without-libgsf], [build without libgsf (default: test)]))

if test "x$with_libgsf" != "xno"; then
  PKG_CHECK_MODULES(LIBGSF, libgsf-1,
    [AC_DEFINE(HAVE_LIBGSF,1,[define if you have libgsf installed.])
     with_libgsf=yes
    ],
    [AC_MSG_WARN([libgsf not found; disabling save plot to file])
     with_libgsf=no
    ])
  IP_CFLAGS="$LIBGSF_CFLAGS $IP_CFLAGS"
  IP_LIBS="$LIBGSF_LIBS $IP_LIBS"
fi

# optional ... use libgvc to draw graphs of workspace dependencies
AC_ARG_WITH([libgvc], AS_HELP_STRING([--without-libgvc], [build without libgvc (default: test)]))

# gvc 2.30 is broken in a number of ways and we can't use it, see for example
# http://lists.research.att.com/pipermail/graphviz-devel/2012/001544.html

if test "x$with_libgvc" != "xno"; then
  PKG_CHECK_MODULES(LIBGVC, libgvc > 2.30,
    [AC_DEFINE(HAVE_LIBGVC,1,[define if you have libgvc installed.])
     with_libgvc=yes
    ],
    [AC_MSG_WARN([libgvc not found; disabling workspace dep graph display])
     with_libgvc=no
    ])
  IP_CFLAGS="$LIBGVC_CFLAGS $IP_CFLAGS"
  IP_LIBS="$LIBGVC_LIBS $IP_LIBS"
fi

# optional ... we add some gsl funcs as builtins if available
AC_ARG_WITH([gsl], AS_HELP_STRING([--without-gsl], [build without gsl (default: test)]))

if test "x$with_gsl" != "xno"; then
  PKG_CHECK_MODULES(GSL, gsl,
    [AC_DEFINE(HAVE_GSL,1,[define if you have gsl installed.])
     with_gsl=yes
    ],
    [AC_MSG_WARN([gsl not found; disabling extra numerical functions])
     with_gsl=no
    ])
  IP_CFLAGS="$GSL_CFLAGS $IP_CFLAGS"
  IP_LIBS="$GSL_LIBS $IP_LIBS"
fi

# optional ... use this to open the help browser, if available
AC_PATH_PROG(XDG_OPEN, xdg-open, no)

if test "x$XDG_OPEN" != "xno"; then
  AC_DEFINE(HAVE_XDG_OPEN,1,[define if you have xdg-open])
  AC_DEFINE_UNQUOTED(XDG_OPEN, "$XDG_OPEN", [path of xdg-open binary])
fi

# optional ... use these to update desktop after install
AC_PATH_PROG(UPDATE_MIME_DATABASE, update-mime-database, no)
AC_PATH_PROG(UPDATE_DESKTOP_DATABASE, update-desktop-database, no)

nip_desktop_update=no
if test "x$UPDATE_MIME_DATABASE" != "xno"; then
  if test "x$UPDATE_DESKTOP_DATABASE" != "xno"; then
    nip_desktop_update=yes
  fi
fi

# stop the DBs being updated: useful for packagers
AC_ARG_ENABLE(update-desktop,
  AC_HELP_STRING([--disable-update-desktop],
    [disable update of desktop database]),
  [nip_desktop_update=$enableval],)

if test x"$nip_desktop_update" = "xyes"; then
  AM_CONDITIONAL(UPDATE_DESKTOP, true)
else
  AM_CONDITIONAL(UPDATE_DESKTOP, false)
fi

AC_SUBST(IP_CFLAGS)
AC_SUBST(IP_LIBS)

# needed by test/test_all.sh
# :( what's a better way to do this, argh
TOP_SRCDIR=$ac_pwd
AC_SUBST(TOP_SRCDIR)

AC_OUTPUT([
	nip2.desktop
	Makefile
	man/Makefile
	man/man1/Makefile
	share/Makefile
	share/nip2/Makefile
	share/nip2/data/Makefile
	share/nip2/rc/Makefile
	share/nip2/start/Makefile
	share/nip2/compat/Makefile
	share/nip2/compat/7.8/Makefile
	share/nip2/compat/7.9/Makefile
	share/nip2/compat/7.10/Makefile
	share/nip2/compat/7.12/Makefile
	share/nip2/compat/7.14/Makefile
	share/nip2/compat/7.16/Makefile
	share/nip2/compat/7.24/Makefile
	share/nip2/compat/7.26/Makefile
	share/nip2/compat/7.28/Makefile
	share/nip2/compat/7.38/Makefile
	src/BITMAPS/Makefile 
	src/Makefile
	test/Makefile
	test/test_all.sh
	po/Makefile.in
	nip2.spec
])

# generated script needs to be executable
chmod +x test/test_all.sh

AC_MSG_RESULT([
* general build options
native win32:				$nip_os_win32
native os x:				$nip_os_darwin
update desktop after install: 		$nip_desktop_update
debug: 					$enable_debug

* optional packages and modules
use fftw3 for FFT: 			$with_fftw3
use gsl for numeric functions:		$with_gsl
use libgoffice to show plots:		$with_libgoffice
use libgsf to save plots to files:	$with_libgsf
use libgvc to show ws dep graphs:	$with_libgvc
  (requires gvc < 2.30)
use gtkinfobar to show messages:	$nip_use_infobar
  (requires gtk+-2.0 >= 2.18)
use notebook action widget:  		$nip_use_notebook_action
  (requires gtk+-2.0 >= 2.20)
use notebook group name:  		$nip_use_notebook_group_name
  (requires gtk+-2.0 >= 2.24)
allow regex searches: 			$nip_use_gregex
  (requires glib-2.0 >= 2.14)
display help files with xdg:		$XDG_OPEN
])

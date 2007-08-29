dnl AC_FUNC_MKDIR
dnl Check for mkdir.  
dnl Can define HAVE_MKDIR, HAVE__MKDIR and MKDIR_TAKES_ONE_ARG.
dnl
dnl #if HAVE_MKDIR
dnl # if MKDIR_TAKES_ONE_ARG
dnl    /* Mingw32 */
dnl #  define mkdir(a,b) mkdir(a)
dnl # endif
dnl #else
dnl # if HAVE__MKDIR
dnl    /* plain Win32 */
dnl #  define mkdir(a,b) _mkdir(a)
dnl # else
dnl #  error "Don't know how to create a directory on this system."
dnl # endif
dnl #endif
dnl
dnl Written by Alexandre Duret-Lutz <duret_g@epita.fr>.

AC_DEFUN([AC_FUNC_MKDIR],
[AC_CHECK_FUNCS([mkdir _mkdir])
AC_CACHE_CHECK([whether mkdir takes one argument],
                [ac_cv_mkdir_takes_one_arg],
[AC_TRY_COMPILE([
#include <sys/stat.h>
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
],[mkdir (".");],
[ac_cv_mkdir_takes_one_arg=yes],[ac_cv_mkdir_takes_one_arg=no])])
if test x"$ac_cv_mkdir_takes_one_arg" = xyes; then
  AC_DEFINE([MKDIR_TAKES_ONE_ARG],1,
            [Define if mkdir takes only one argument.])
fi
])

dnl From FIND_MOTIF and ACX_PTHREAD, without much understanding
dnl
dnl FIND_FFTW[ACTION-IF-FOUND[, ACTION-IF-NOT-FOUND]]
dnl ------------------------------------------------
dnl
dnl Find FFTW libraries and headers
dnl
dnl Put compile stuff in FFTW_INCLUDES
dnl Put link stuff in FFTW_LIBS
dnl
dnl Default ACTION-IF-FOUND defines HAVE_FFTW
dnl
AC_DEFUN([FIND_FFTW], [
AC_REQUIRE([AC_PATH_XTRA])

FFTW_INCLUDES=""
FFTW_LIBS=""

AC_ARG_WITH(fftw, 
[  --without-fftw		do not use libfftw])
# Treat --without-fftw like --without-fftw-includes --without-fftw-libraries.
if test "$with_fftw" = "no"; then
	FFTW_INCLUDES=no
	FFTW_LIBS=no
fi

AC_ARG_WITH(fftw-includes,
[  --with-fftw-includes=DIR	FFTW include files are in DIR],
FFTW_INCLUDES="-I$withval")
AC_ARG_WITH(fftw-libraries,
[  --with-fftw-libraries=DIR	FFTW libraries are in DIR],
FFTW_LIBS="-L$withval -lfftw")

AC_MSG_CHECKING(for FFTW)

# Look for fftw.h 
if test x"$FFTW_INCLUDES" = x""; then
	# Check the standard search path
	AC_TRY_COMPILE([
		#include <fftw.h>],[int a],[
		FFTW_INCLUDES=""
	], [
		# fftw.h is not in the standard search path.

		# A whole bunch of guesses
		for dir in \
			"${prefix}"/*/include \
			/usr/local/include \
			/usr/*/include \
			/usr/local/*/include /usr/*/include \
			"${prefix}"/include/* \
			/usr/include/* /usr/local/include/* /*/include; do
			if test -f "$dir/fftw.h"; then
				FFTW_INCLUDES="-I$dir"
				break
			fi
		done

		if test "$FFTW_INCLUDES" = ""; then
			FFTW_INCLUDES=no
		fi
	])
fi

# Now for the libraries
if test x"$FFTW_LIBS" = x""; then
	fftw_save_LIBS="$LIBS"
	fftw_save_INCLUDES="$INCLUDES"

	INCLUDES="$FFTW_INCLUDES $INCLUDES"

	# could be called fftw or dfftw ... depends on the distribution
	# we prefer the "d" version (means we're sure it's been built for
	# double precision)

	# Try the standard search path first
	LIBS="-ldrfftw -ldfftw -lm $fftw_save_LIBS"
	AC_TRY_LINK([
		#include <math.h>
		#include <fftw.h>],[fftw_die("")], [
		FFTW_LIBS="-ldrfftw -ldfftw -lm"
	], [
		# not in the standard search path.
		# A whole bunch of guesses
		for dir in \
			"${prefix}"/*/lib \
			/usr/local/lib \
			/usr/*/lib \
			"${prefix}"/lib/* /usr/lib/* \
			/usr/local/lib/* /*/lib; do
			if test -d "$dir" && test "`ls $dir/libdfftw.* 2> /dev/null`" != ""; then
				FFTW_LIBS="-L$dir -ldrfftw -ldfftw -lm"
				break
			fi
		done

		if test x"$FFTW_LIBS" = x""; then
			FFTW_LIBS=no
		fi
	])

	if test "$FFTW_LIBS" = "no"; then
		# Try the standard search path first
		LIBS="-lrfftw -lfftw -lm $fftw_save_LIBS"
		AC_TRY_LINK([
			#include <math.h>
			#include <fftw.h>],[fftw_die("")], [
			FFTW_LIBS="-lrfftw -lfftw -lm"
		], [
			# not in the standard search path.
			# A whole bunch of guesses
			for dir in \
				"${prefix}"/*/lib \
				/usr/local/lib \
				/usr/*/lib \
				"${prefix}"/lib/* /usr/lib/* \
				/usr/local/lib/* /*/lib; do
				if test -d "$dir" && test "`ls $dir/libdfftw.* 2> /dev/null`" != ""; then
					FFTW_LIBS="-L$dir -lrfftw -lfftw -lm"
					break
				fi
			done

			if test x"$FFTW_LIBS" = x""; then
				FFTW_LIBS=no
			fi
		])
	fi

	LIBS="$fftw_save_LIBS"
	INCLUDES="$fftw_save_INCLUDES"
fi

# Print a helpful message
fftw_libraries_result="$FFTW_LIBS"
fftw_includes_result="$FFTW_INCLUDES"

if test x"$fftw_libraries_result" = x""; then
	fftw_libraries_result="in default path"
fi
if test x"$fftw_includes_result" = x""; then
	fftw_includes_result="in default path"
fi

if test "$fftw_libraries_result" = "no"; then
	fftw_libraries_result="(none)"
fi
if test "$fftw_includes_result" = "no"; then
	fftw_includes_result="(none)"
fi

AC_SUBST(FFTW_LIBS)
AC_SUBST(FFTW_INCLUDES)

AC_MSG_RESULT(
  [libraries $fftw_libraries_result, headers $fftw_includes_result])

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test "$FFTW_INCLUDES" != "no" && test "$FFTW_LIBS" != "no"; then
        ifelse([$1],,AC_DEFINE(HAVE_FFTW,1,[Define if you have fftw libraries and header files.]),[$1])
        :
else
	FFTW_INCLUDES=""
	FFTW_LIBS=""
        $2
fi

])dnl


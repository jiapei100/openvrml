dnl @synopsis AX_CHECK_GLUT
dnl
dnl Check for GLUT.  If GLUT is found, the required compiler and linker flags
dnl are included in the output variables "GLUT_CFLAGS" and "GLUT_LIBS",
dnl respectively, and the shell variable "no_glut" is set to the empty
dnl string.  Also, if the header "GL/glut.h" is found, the symbol
dnl "HAVE_GL_GLUT_H" is defined; otherwise, if the header "GLUT/glut.h" is
dnl found, "HAVE_GLUT_GLUT_H" is defined. If GLUT is not found, "no_glut" is
dnl set to "yes".
dnl
dnl @copyright (C) 2003 Braden McDaniel
dnl @license GNU GPL
dnl @version $Id: ax_check_glut.m4,v 1.4 2003-07-25 23:43:20 braden Exp $
dnl @author Braden McDaniel <braden@endoframe.com>
dnl
AC_DEFUN([AX_CHECK_GLUT],
[AC_REQUIRE([AX_CHECK_GL])dnl
AC_REQUIRE([AC_PATH_XTRA])dnl

GLUT_CFLAGS=${GL_CFLAGS}

if test "X$with_apple_opengl_framework" = "Xyes"; then
  GLUT_CFLAGS="${GLUT_CFLAGS} -framework GLUT"
else
  GLUT_LIBS="${GL_LIBS}"

  #
  # If X is present, assume GLUT depends on it.
  #
  if test "X${no_x}" != "Xyes"; then
    GLUT_LIBS="${X_PRE_LIBS} -lXmu -lXi ${X_EXTRA_LIBS} ${GLUT_LIBS}"
  fi

  AC_LANG_PUSH(C)

  ax_save_CPPFLAGS="${CPPFLAGS}"
  CPPFLAGS="${GLUT_CFLAGS} ${CPPFLAGS}"

  AC_CACHE_CHECK([for GLUT library], [ax_cv_check_glut_libglut],
  [ax_cv_check_glut_libglut="no"
  ax_save_LIBS="${LIBS}"
  LIBS=""
  ax_check_libs="-lglut32 -lglut"
  for ax_lib in ${ax_check_libs}; do
    if test "X$CC" = "Xcl"; then
      ax_try_lib=`echo $ax_lib | sed -e 's/^-l//' -e 's/$/.lib/'`
    else
      ax_try_lib="${ax_lib}"
    fi
    LIBS="${ax_try_lib} ${GLUT_LIBS} ${ax_save_LIBS}"
    AC_TRY_LINK([
# if HAVE_WINDOWS_H && defined(_WIN32)
#   include <windows.h>
# endif
# include <GL/glut.h>
  ],
    [glutMainLoop()],
    [ax_cv_check_glut_libglut="${ax_try_lib}"; break])

  done
  LIBS=${ax_save_LIBS}
  ])
  CPPFLAGS="${ax_save_CPPFLAGS}"
  AC_LANG_POP(C)

  if test "X${ax_cv_check_glut_libglut}" = "Xno"; then
    no_glut="yes"
  else
    GLUT_LIBS="${ax_cv_check_glut_libglut} ${GLUT_LIBS}"
  fi
fi

AC_SUBST([GLUT_CFLAGS])
AC_SUBST([GLUT_LIBS])
])dnl

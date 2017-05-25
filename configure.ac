#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(crf_train, 1.0, yiwenwang@aliyun.com)
AM_INIT_AUTOMAKE(crf_train, 1.0)
AC_CONFIG_SRCDIR([crf_train_main.cpp])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
#AX_CXX_COMPILE_STDCXX_11
#AX_CHECK_COMPILE_FLAG([-std=gnu++11], [CXXFLAGS="$CXXFLAGS -std=gnu++11 -pthread"])

# Checks for libraries.

# Checks for header files.
#AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.
#AC_CHECK_HEADER_STDBOOL
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_HEADER_STDC
AC_CHECK_FUNCS([memset])

#AC_CONFIG_FILES([Makefile])
CXXFLAGS="$CXXFLAGS -pthread -std=gnu++11"
#CXX=/opt/compiler/gcc-4.8.2/bin/g++
AC_OUTPUT(Makefile)


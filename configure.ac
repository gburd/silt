AC_PREREQ(2.63)
AC_INIT([silt], [1.0.1], [fawn-dev@mailman.srv.cs.cmu.edu])
AM_INIT_AUTOMAKE([1.10 foreign])
LT_PREREQ([2.4.2])
LT_INIT([dlopen])

AC_CONFIG_HEADER([config.h])

# If CFLAGS was not set on entry and we are not debugging, default to -O3
silt_test_CFLAGS=${CFLAGS+set}
if test "$silt_test_CFLAGS" != set && test "$silt_cv_enable_debug" != "yes"; then
	CFLAGS="-O3 -march=native -mtune=native"
fi

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET

AC_PROG_LIBTOOL
AC_CONFIG_MACRO_DIR([m4])

# Add debugging flags if requested and not already in CFLAGS
if test "$silt_cv_enable_debug" = "yes"; then
	case "$CFLAGS" in
		*-g*)	;;
		*)	AM_CFLAGS="$AM_CFLAGS -g -O0 -fno-omit-frame-pointer" ;;
	esac
fi

AM_CONDITIONAL([DEBUG], [test "$silt_cv_enable_debug" = "yes"])

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lgtest':
AC_CHECK_LIB([gtest], [main], [LIBS="-lgtest $LIBS"; no_libgtest=false],
		      	      [echo "gtest unittesting framework not found."; no_libgtest=true])
AC_LANG_CPLUSPLUS
AC_CHECK_LIB([tbb], [main], [], [echo "libtbb not found.  Please install the Intel TBB before proceeding."; exit -1])
AC_CHECK_LIB([gtest_main], [main], [], [echo "gtest_main not found.  Please install google test library before proceeding"; exit -1])
AC_CHECK_LIB([pthread], [pthread_mutex_init], [], [echo "pthreads not found.  Please install pthread library before proceeding"; exit -1])
AC_CHECK_LIB([boost_thread], [main], [], [echo "boost library not found.  Please install boost library before proceeding"; exit -1])
AC_CHECK_LIB([crypto], [main], [], [echo "openssl crypto library not found.  Please install openssl library before proceeding"; exit -1])
AC_CHECK_LIB([xerces-c], [main], [], [echo "Xerces XML parsing library not found.  Please install xerces library before proceeding"; exit -1])
AC_CHECK_LIB([m], [cos], [], [echo "m library not found.  Please install m library before proceeding"; exit -1])
AC_CHECK_LIB([gslcblas], [cblas_dgemm], [], [echo "gsl library not found.  Please install gsl library before proceeding"; exit -1])
AC_CHECK_LIB([gsl], [gsl_blas_dgemm], [], [echo "gsl library not found.  Please install gsl library before proceeding"; exit -1])
AC_CHECK_LIB([nsort], [nsort_define], [], [echo "nsort library not found.  An inferior sorting will be used."])
AC_CHECK_LIB([db], [db_create], [], [echo "Berkeley Database library not found.  Support for BDB will be excluded."])
AC_CHECK_LIB([rt], [main], [], [echo "rt."])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stdlib.h string.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_HEADER_TIME
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_C_VOLATILE


# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([\
	bzero \
	ftruncate \
	gethostbyname \
	getpagesize \
	gettimeofday \
	inet_ntoa \
	memchr \
	memmove \
	memset \
	munmap \
	select \
	socket \
	strdup \
	strerror \
	strtol \
	strtoul \
	strtoull \
	])

CFLAGS="$CFLAGS -Wall -Wextra" # -Werror
CXXFLAGS="$CFLAGS"

AC_CONFIG_FILES([ Makefile
		  src/Makefile
		  utils/Makefile
		  test/Makefile
		])
AC_OUTPUT

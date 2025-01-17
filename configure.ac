AC_INIT([clog], [1.0], [mark@mystikos.org])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_SRCDIR([homepagecgi.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([mysqlclient], [mysql_init])

# Checks for header files.
AC_CHECK_HEADERS([inttypes.h stdlib.h stdio.h string.h strings.h sys/stat.h sys/types.h unistd.h mysql/mysql.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_TYPE([size_t], [unsigned])

# Checks for library functions.
AC_CHECK_FUNCS([malloc realloc strdup])

# Define the output files
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
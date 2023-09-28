PHP_ARG_ENABLE(hookevals, whether to enable hookevals support,
[ --enable-hookevals   Enable PHP eval hook support])
if test "$PHP_HOOKEVALS" = "yes"; then
  AC_DEFINE(HAVE_HOOKEVALS, 1, [Whether you have PHP HOOKEVALS])
  PHP_NEW_EXTENSION(hookevals, hookevals.c, $ext_shared)
fi
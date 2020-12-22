/* CMake config.h for Hatari */

/* Define if you have a PNG compatible library */
/* #undef HAVE_LIBPNG */

/* Define if you have a readline compatible library */
/* #undef HAVE_LIBREADLINE */

/* Define if you have the PortAudio library */
/* #undef HAVE_PORTAUDIO */

/* Define if you have the PortMidi library */
/* #undef HAVE_PORTMIDI */

/* Define if you have the capsimage library */
/* #undef HAVE_CAPSIMAGE */

/* Define if you have a X11 environment */
/* #undef HAVE_X11 */

/* Define to 1 if you have the `z' library (-lz). */
/* #undef HAVE_LIBZ */

/* Define to 1 if you have the <zlib.h> header file. */
/* #undef HAVE_ZLIB_H */

/* Define to 1 if you have the <termios.h> header file. */
/* #undef HAVE_TERMIOS_H */

/* Define to 1 if you have the <sys/ioctl.h> header file. */
/* #undef HAVE_SYS_IOCTL_H */

/* Define to 1 if you have the <glob.h> header file. */
/* #undef HAVE_GLOB_H */

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <SDL_config.h> header file. */
#define HAVE_SDL_CONFIG_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/times.h> header file. */
/* #undef HAVE_SYS_TIMES_H */

/* Define to 1 if you have the <utime.h> header file. */
#define HAVE_UTIME_H 1

/* Define to 1 if you have the <sys/utime.h> header file. */
#define HAVE_SYS_UTIME_H 1

/* Define to 1 if you have the `cfmakeraw' function. */
/* #undef HAVE_CFMAKERAW */

/* Define to 1 if you have the `tcsetattr' function. */
/* #undef HAVE_TCSETATTR */

/* Define to 1 if you have the 'setenv' function. */
/* #undef HAVE_SETENV */

/* Define to 1 if you have the `select' function. */
/* #undef HAVE_SELECT */

/* Define to 1 if you have unix domain sockets */
/* #undef HAVE_UNIX_DOMAIN_SOCKETS */

/* Define to 1 if you have the 'gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the 'nanosleep' function. */
/* #undef HAVE_NANOSLEEP */

/* Define to 1 if you have the 'alphasort' function. */
/* #undef HAVE_ALPHASORT */

/* Define to 1 if you have the 'scandir' function. */
/* #undef HAVE_SCANDIR */

/* Define to 1 if you have the 'statvfs' function. */
/* #undef HAVE_STATVFS */

/* Define to 1 if you have the 'fseeko' function. */
#define HAVE_FSEEKO 1

/* Define to 1 if you have the 'ftello' function. */
#define HAVE_FTELLO 1

/* Define to 1 if you have the 'flock' function. */
/* #undef HAVE_FLOCK */

/* Define to 1 if you have the 'strlcpy' function. */
/* #undef HAVE_LIBC_STRLCPY */

/* Define to 1 if you have the 'd_type' member in the 'dirent' struct */
/* #undef HAVE_DIRENT_D_TYPE */
#undef __BSD_VISIBLE

/* Relative path from bindir to datadir */
#define BIN2DATADIR "."

/* Define to 1 to enable DSP 56k emulation for Falcon mode */
#define ENABLE_DSP_EMU 1

/* Define to 1 to enable WINUAE cpu  */
#define ENABLE_WINUAE_CPU 1

/* Define to 1 to use less memory - at the expense of emulation speed */
/* #undef ENABLE_SMALL_MEM */

/* Define to 1 to enable trace logs - undefine to slightly increase speed */
#define ENABLE_TRACING 1

/* Define to 1 if udev support is available */
/* #undef HAVE_UDEV */

#define HAVE_WINSOCK_SOCKETS 1


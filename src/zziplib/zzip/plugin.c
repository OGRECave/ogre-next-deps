
/*
 * Author:
 *	Guido Draheim <guidod@gmx.de>
 *      Mike Nordell <tamlin-@-algonet-se>
 *
 * Copyright (c) 2002,2003 Guido Draheim
 * 	    All rights reserved,
 *	    use under the restrictions of the
 *	    Lesser GNU General Public License
 *          or alternatively the restrictions
 *          of the Mozilla Public License 1.1
 */

#include <zzip/lib.h>
#include <zzip/plugin.h>

#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#ifdef DEBUG
#include <stdio.h>
#endif

#include <zzip/file.h>
#include <zzip/format.h>

#if defined(__MINGW32__) && defined(_WIN64)
#include <limits.h>

/* Limit each transfer to INT_MAX:
 * zzip_size_t specifies the requested number of bytes and is 64-bit on Win64
 * (maximum about 16 EiB). zzip_ssize_t reports the number of bytes actually
 * transferred and is a 32-bit int (maximum about 2 GiB).
 * Therefore, limit each read/write operation to INT_MAX bytes
 * (i.e. 2 GiB - 1 byte).
 */
static zzip_ssize_t
zzip_mingw64_read(int fd, void *buf, zzip_size_t len)
{
    unsigned int count = len > (zzip_size_t)INT_MAX
        ? (unsigned int)INT_MAX
        : (unsigned int)len;

    return _read(fd, buf, count);
}

static zzip_ssize_t
zzip_mingw64_write(int fd, const void *buf, zzip_size_t len)
{
    unsigned int count = len > (zzip_size_t)INT_MAX
        ? (unsigned int)INT_MAX
        : (unsigned int)len;

    return _write(fd, buf, count);
}
#endif

zzip_off_t
zzip_filesize(int fd)
{
    struct stat st;

    if (fstat(fd, &st) < 0)
        return -1;

# if defined DEBUG && ! defined _WIN32
    if (! st.st_size && st.st_blocks > 1)        /* seen on some darwin 10.1 machines */
        fprintf(stderr, "broken fstat(2) ?? st_size=%ld st_blocks=%ld\n",
                (long) st.st_size, (long) st.st_blocks);
# endif

    return st.st_size;
}

static const struct zzip_plugin_io default_io = {
    &open,
    &close,
#if defined(__MINGW32__) && defined(_WIN64)
    &zzip_mingw64_read,
#else
    &_zzip_read,
#endif
    &_zzip_lseek,
    &zzip_filesize,
    1, 1,
#if defined(__MINGW32__) && defined(_WIN64)
    &zzip_mingw64_write
#else
    &_zzip_write
#endif
};

/** => zzip_init_io
 * This function returns a zzip_plugin_io_t handle to static defaults
 * wrapping the posix io file functions for actual file access. The
 * returned structure is shared by all threads in the system.
 */
zzip_plugin_io_t
zzip_get_default_io(void)
{
    return (zzip_plugin_io_t) & default_io;
}

/**
 * This function initializes the users handler struct to default values
 * being the posix io functions in default configured environments.
 *
 * Note that the target io_handlers_t structure should be static or
 * atleast it should be kept during the lifetime of zzip operations.
 */
int
zzip_init_io(zzip_plugin_io_handlers_t io, int flags)
{
    if (! io)
    {
        return ZZIP_ERROR;
    }
    memcpy(io, &default_io, sizeof(default_io));
    io->fd.sys = flags;
    return 0;
}

/*
 * Local variables:
 * c-file-style: "stroustrup"
 * End:
 */

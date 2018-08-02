/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * unistd (POSIX routines for file operations)
 *
 * Copyright 2017-2018 Phoenix Systems
 * Author: Aleksander Kaminski, Pawel Pisarczyk, Kamil Amanowicz
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "posix/utils.h"
#include "posixsrv/posixsrv.h"


extern int sys_open(const char *filename, int oflag, ...);
extern int sys_mkfifo(const char *filename, mode_t mode);
extern int sys_link(const char *path1, const char *path2);
extern int sys_unlink(const char *path);

WRAP_ERRNO_DEF(int, read, (int fildes, void *buf, size_t nbyte), (fildes, buf, nbyte))
WRAP_ERRNO_DEF(int, write, (int fildes, const void *buf, size_t nbyte), (fildes, buf, nbyte))
WRAP_ERRNO_DEF(int, close, (int fildes), (fildes))
WRAP_ERRNO_DEF(int, ftruncate, (int fildes, off_t length), (fildes, length))
WRAP_ERRNO_DEF(int, lseek, (int fildes, off_t offset, int whence), (fildes, offset, whence))
WRAP_ERRNO_DEF(int, dup, (int fildes), (fildes))
WRAP_ERRNO_DEF(int, dup2, (int fildes, int fildes2), (fildes, fildes2))
WRAP_ERRNO_DEF(int, pipe, (int fildes[2]), (fildes))
WRAP_ERRNO_DEF(int, fstat, (int fd, struct stat *buf), (fd, buf))

WRAP_ERRNO_DEF(int, grantpt, (int fd), (fd))
WRAP_ERRNO_DEF(int, unlockpt, (int fd), (fd))
WRAP_ERRNO_DEF(int, ptsname_r, (int fd, char *buf, size_t buflen), (fd, buf, buflen))


int link(const char *path1, const char *path2)
{
	char *canonical1, *canonical2;
	int err;
	canonical1 = canonicalize_file_name(path1);
	canonical2 = canonicalize_file_name(path2);
	err = sys_link(canonical1, canonical2);
	free(canonical1);
	free(canonical2);

	return SET_ERRNO(err);
}


int unlink(const char *path)
{
	char *canonical;
	int err;
	canonical = canonicalize_file_name(path);
	err = sys_unlink(canonical);
	free(canonical);
	return SET_ERRNO(err);
}


int open(const char *filename, int oflag, ...)
{
	va_list ap;
	mode_t mode = 0;
	int err;
	char *canonical;

	/* FIXME: handle varargs properly */
	va_start(ap, oflag);
	mode = va_arg(ap, mode_t);
	va_end(ap);

	canonical = canonicalize_file_name(filename);
	err = sys_open(canonical, oflag, mode);
	free(canonical);
	return SET_ERRNO(err);
}


int mkfifo(const char *filename, mode_t mode)
{
	int err;
	char *canonical;

	canonical = canonicalize_file_name(filename);
	err = sys_mkfifo(canonical, mode);
	free(canonical);
	return SET_ERRNO(err);
}


int symlink(const char *path1, const char *path2)
{
	return -1;
}


int isatty(int fildes)
{
	return fildes == 0 || fildes == 1;
}


int access(const char *path, int amode)
{
	return 0;
}


int create_dev(oid_t *oid, const char *path)
{
	oid_t odir;
	msg_t msg = { 0 };
	char *canonical_path, *dir, *name;

	if (path == NULL)
		return -1;

	if ((canonical_path = canonicalize_file_name(path)) == NULL)
		return -1;

	splitname(canonical_path, &name, &dir);

	if (lookup(dir, NULL, &odir) < 0) {
		free(canonical_path);
		return -1;
	}

	msg.type = mtCreate;
	memcpy(&msg.i.create.dir, &odir, sizeof(odir));
	memcpy(&msg.i.create.dev, oid, sizeof(*oid));

	msg.i.create.type = otDev;
	msg.i.create.mode = 0;

	msg.i.data = name;
	msg.i.size = strlen(name) + 1;

	if (msgSend(odir.port, &msg) != EOK) {
		free(canonical_path);
		return -1;
	}

	free(canonical_path);

	if (msg.o.io.err < 0)
		return -1;

	return 0;
}


extern int sys_fcntl(int fd, int cmd, unsigned val);


int fcntl(int fd, int cmd, ...)
{
	va_list ap;
	unsigned val;

	/* FIXME: handle varargs properly */
	va_start(ap, cmd);
	val = va_arg(ap, unsigned);
	va_end(ap);

	return SET_ERRNO(sys_fcntl(fd, cmd, val));
}


extern int sys_ioctl(int fildes, unsigned long request, void *val);


int ioctl(int fildes, unsigned long request, ...)
{
	va_list ap;
	void * val;

	/* FIXME: handle varargs properly */
	va_start(ap, request);
	val = va_arg(ap, void *);
	va_end(ap);

	return SET_ERRNO(sys_ioctl(fildes, request, val));
}

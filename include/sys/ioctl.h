/*
 * Phoenix-RTOS
 *
 * libphoenix
 *
 * ioctl.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */


#ifndef _LIBPHOENIX_IOCTL_H_
#define _LIBPHOENIX_IOCTL_H_

#include <sys/msg.h>
#include <sys/types.h>
#include <phoenix/ioctl.h>

#define _IOC(inout,group,num,len)	((unsigned long) (inout | ((len & IOCPARM_MASK) << 16) | ((group) << 8) | (num)))
#define _IO(g,n)			_IOC(IOC_VOID, (g), (n), 0)
#define _IOV(g,n,t)			_IOC(IOC_VOID, (g), (n), sizeof(t))     /* IOW with passing by value */
#define _IOR(g,n,t)			_IOC(IOC_OUT, (g), (n), sizeof(t))
#define _IOW(g,n,t)			_IOC(IOC_IN, (g), (n), sizeof(t))
#define _IOWR(g,n,t)		_IOC(IOC_INOUT, (g), (n), sizeof(t))


#define TIOCGPTN	_IOR('T', 0x30, unsigned int)
#define TIOCSPTLCK	_IOW('T', 0x31, int)


#define	FIONREAD		_IOR('f', 127, int)


int ioctl(int fd, unsigned long cmd, ... );


const void * ioctl_unpack(const msg_t *msg, unsigned long *request, id_t *id);

const void * ioctl_unpackEx(const msg_t *msg, unsigned long *request, id_t *id, void** response_buf);

pid_t ioctl_getSenderPid(const msg_t *msg);


void ioctl_setResponse(msg_t *msg, unsigned long request, int err, const void *data);

void ioctl_setResponseErr(msg_t *msg, unsigned long request, int err);


#endif
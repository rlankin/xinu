/**
 * @file stddef.h
 *
 * Includes standard type definitions and primary constants used across
 * all aspects of Xinu.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _STDDEF_H_
#define _STDDEF_H_

/* macro to get offset to struct members */
#define offsetof(type, member) ((size_t)&(((type *)0)->member))

/* Base type definitions */
typedef unsigned char uchar;    /**< unsigned char type                 */
typedef unsigned short ushort;  /**< unsigned short type                */
typedef unsigned int uint;      /**< unsigned int type                  */
typedef unsigned long ulong;    /**< unsigned long type                 */
typedef char bool;              /**< boolean type                       */
typedef unsigned long size_t;   /**< size type                          */

typedef	signed char	int8;
typedef	unsigned char	byte;
typedef	unsigned char	uint8;
typedef	int		int32;
typedef	short		int16;
typedef	unsigned int	uint32;
typedef uint32		uintn;
typedef	unsigned short	uint16;
typedef	unsigned long long uint64;

/* Xinu-specific types */
typedef	int32	sid32;		/* semaphore ID				*/
typedef	int16	qid16;		/* queue ID				*/
typedef	int32	pid32;		/* process ID				*/
typedef	int32	did32;		/* device ID				*/
typedef	int16	pri16;		/* process priority			*/
typedef	uint32	umsg32;		/* message passed among processes	*/
typedef	int32	bpid32;		/* buffer pool ID			*/
typedef	byte	bool8;		/* Boolean type				*/
typedef	uint32	intmask;	/* saved interrupt mask			*/
typedef	int32	ibid32;		/* index block ID (used in file system)	*/
typedef	int32	dbid32;		/* data block ID (used in file system)	*/
typedef	int32	uid32;		/* ID for UDP table descriptor		*/

/* Function declaration return types */
typedef int syscall;            /**< system call declaration            */
typedef int devcall;            /**< device call declaration            */
typedef int shellcmd;           /**< shell command declaration          */
typedef int thread;             /**< thread declaration                 */
typedef void interrupt;         /**< interrupt procedure                */
typedef void exchandler;        /**< exception procedure                */
typedef int message;            /**< message passing content            */

typedef int tid_typ;            /**< thread ID type                     */

/* Boolean type and constants */
#define FALSE        0          /**< boolean false                      */
#define TRUE         1          /**< boolean true                       */

/* Universal return constants */
#define OK        1             /**< system call ok                     */
#define NULL      0             /**< null pointer for linked lists      */
#define SYSERR   (-1)           /**< system call failed                 */
#define EOF      (-2)           /**< End-of-file (usually from read)    */
#define TIMEOUT  (-3)           /**< system call timed out              */
#define NOMSG    (-4)           /**< no message to receive              */

#endif                          /* _STDDEF_H_ */

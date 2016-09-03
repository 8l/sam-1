/* Copyright (c) 1998 Lucent Technologies - All rights reserved. */
#include <u.h>
#include <libc.h>
#include "errors.h"
#include "../config.h"

/*
 * BLOCKSIZE is relatively small to keep memory consumption down.
 */

#define	BLOCKSIZE	2048
#define	RUNESIZE	sizeof(Rune)
#define	NDISC		5
#define	NBUFFILES	3+2*NDISC	/* plan 9+undo+snarf+NDISC*(transcript+buf) */
#define NSUBEXP	10

#define	TRUE		1
#define	FALSE		0

#define	INFINITY	0x7FFFFFFFL
#define	INCR		25
#define	STRSIZE		(2*BLOCKSIZE)

typedef long		Posn;		/* file position or address */
typedef	ushort		Mod;		/* modification number */

typedef struct Address	Address;
typedef struct Buffer	Buffer;
typedef struct File	File;
typedef struct List	List;
typedef struct Mark	Mark;
typedef struct Range	Range;
typedef struct Rangeset	Rangeset;
typedef struct String	String;

enum State
{
	Clean =		' ',
	Dirty =		'\'',
	Unread =	'-',
	Readerr =	'~'
};

struct Range
{
	Posn	p1, p2;
};

struct Rangeset
{
	Range	p[NSUBEXP];
};

struct Address
{
	Range	r;
	File	*f;
};

struct List	/* code depends on a long being able to hold a pointer */
{
	int	nalloc;
	int	nused;
	union{
		void	*listp;
		long	*longp;
		uchar*	*ucharp;
		String*	*stringp;
		File*	*filep;
		long	listv;
	}g;
};

#define	listptr		g.listp
#define	longptr		g.longp
#define	ucharpptr	g.ucharp
#define	stringpptr	g.stringp
#define	filepptr	g.filep
#define	listval		g.listv

struct String
{
	short	n;
	short	size;
	Rune	*s;
};

typedef struct Gapbuffer Gapbuffer;
struct Buffer{
    Posn nrunes;
    Gapbuffer *gb;
};

#define	NGETC	128

struct File
{
	Buffer	*buf;		/* cached disc storage */
	Buffer	*transcript;	/* what's been done */
	Posn	markp;		/* file pointer to start of latest change */
	Mod	mod;		/* modification stamp */
	Posn	nrunes;		/* total length of file */
	Posn	hiposn;		/* highest address touched this Mod */
	Address	dot;		/* current position */
	Address	ndot;		/* new current position after update */
	Range	tdot;		/* what terminal thinks is current range */
	Range	mark;		/* tagged spot in text (don't confuse;
};

#define	NGETC	128

struct File
{
	Buffer	*buf;		/* cached disc storage */
	Buffer	*transcript;	/* what's been done */
	Posn	markp;		/* file pointer to start of latest change */
	Mod	mod;		/* modification stamp */
	Posn	nrunes;		/* total length of file */
	Posn	hiposn;		/* highest address touched this Mod */
	Address	dot;		/* current position */
	Address	ndot;		/* new current position after update */
	Range	tdot;		/* what terminal thinks is current range */
	Range	mark;		/* tagged spot in text (don't confuse with Mark) */
	List	*rasp;		/* map of what terminal's got */
	String	name;		/* file name */
	short	tag;		/* for communicating with terminal */
	char	state;		/* Clean, Dirty, Unread, or Readerr*/
	char	closeok;	/* ok to close file? */
	char	deleted;	/* delete at completion of command */
	char	marked;		/* file has been Fmarked at least once; once
				 * set, this will never go off as undo doesn't
				 * revert to the dawn of time */
	long	dev;		/* file system from which it was read */
	long	qid;		/* file from which it was read */
	long	date;		/* time stamp of plan9 file */
	Posn	cp1, cp2;	/* Write-behind cache positions and */
	String	cache;		/* string */
	Rune	getcbuf[NGETC];
	int	ngetc;
	int	getci;
	Posn	getcp;
};

struct Mark
{
	Posn	p;
	Range	dot;
	Range	mark;
	Mod	m;
	short	s1;
};

/*
 * The precedent to any message in the transcript.
 * The component structures must be an integral number of Runes long.
 */
union Hdr
{
	struct _csl
	{
		short	c;
		short	s;
		long	l;
	}csl;
	struct _cs
	{
		short	c;
		short	s;
	}cs;
	struct _cll
	{
		short	c;
		long	l;
		long	l1;
	}cll;
	Mark	mark;
};

#define	Fgetc(f)  ((--(f)->ngetc<0)? Fgetcload(f, (f)->getcp) : (f)->getcbuf[(f)->getcp++, (f)->getci++])
#define	Fbgetc(f) (((f)->getci<=0)? Fbgetcload(f, (f)->getcp) : (f)->getcbuf[--(f)->getcp, --(f)->getci])

int	alnum(int);
void	Bterm(Buffer*);
void	Bdelete(Buffer*, Posn, Posn);
void	Binsert(Buffer*, String*, Posn);
Buffer	*Bopen(void);
int	Bread(Buffer*, Rune*, int, Posn);
int	Fbgetcload(File*, Posn);
int	Fbgetcset;
};

#define	NGETC	128

struct File
{
	Buffer	*buf;		/* cached disc storage */
	Buffer	*transcript;	/* what's been done */
	Posn	markp;		/* file pointer to start of latest change */
	Mod	mod;		/* modification stamp */
	Posn	nrunes;		/* total length of file */
	Posn	hiposn;		/* highest address touched this Mod */
	Address	dot;		/* current position */
	Address	ndot;		/* new current position after update */
	Range	tdot;		/* what terminal thinks is current range */
	Range	mark;		/* tagged spot in text (don't confuse with Mark) */
	List	*rasp;		/* map of what terminal's got */
	String	name;		/* file name */
	short	tag;		/* for communicating with terminal */
	char	state;		/* Clean, Dirty, Unread, or Readerr*/
	char	closeok;	/* ok to close file? */
	char	deleted;	/* delete at completion of command */
	char	marked;		/* file has been Fmarked at least once; once
				 * set, this will never go off as undo doesn't
				 * revert to the dawn of time */
	long	dev;		/* file system from which it was read */
	long	qid;		/* file from which it was read */
	long	date;		/* time stamp of plan9 file */
	Posn	cp1, cp2;	/* Write-behind cache positions and */
	String	cache;		/* string */
	Rune	getcbuf[NGETC];
	int	ngetc;
	int	getci;
	Posn	getcp;
};

struct Mark
{
	Posn	p;
	Range	dot;
	Range	mark;
	Mod	m;
	short	s1;
};

/*
 * The precedent to any message in the transcript.
 * The component structures must be an integral number of Runes long.
 */
union Hdr
{
	struct _csl
	{
		short	c;
		short	s;
		long	l;
	}csl;
	struct _cs
	{
		short	c;
		short	s;
	}cs;
	struct _cll
	{
		short	c;
		long	l;
		long	l1;
	}cll;
	Mark	mark;
};

#define	Fgetc(f)  ((--(f)->ngetc<0)? Fgetcload(f, (f)->getcp) : (f)->getcbuf[(f)->getcp++, (f)->getci++])
#define	Fbgetc(f) (((f)->getci<=0)? Fbgetcload(f, (f)->getcp) : (f)->getcbuf[--(f)->getcp, --(f)->getci])

int	alnum(int);
void	Bterm(Buffer*);
void	Bdelete(Buffer*, Posn, Posn);
void	Binsert(Buffer*, String*, Posn);
Buffer	*Bopen(void);
int	Bread(Buffer*, Rune*, int, Posn);
int	Fbgetcload(File*, Posn);
int	Fbgetcset;
};

#define	NGETC	128

struct File
{
	Buffer	*buf;		/* cached disc storage */
	Buffer	*transcript;	/* what's been done */
	Posn	markp;		/* file pointer to start of latest change */
	Mod	mod;		/* modification stamp */
	Posn	nrunes;		/* total length of file */
	Posn	hiposn;		/* highest address touched this Mod */
	Address	dot;		/* current position */
	Address	ndot;		/* new current position after update */
	Range	tdot;		/* what terminal thinks is current range */
	Range	mark;		/* tagged spot in text (don't confuse with Mark) */
	List	*rasp;		/* map of what terminal's got */
	String	name;		/* file name */
	short	tag;		/* for communicating with terminal */
	char	state;		/* Clean, Dirty, Unread, or Readerr*/
	char	closeok;	/* ok to close file? */
	char	deleted;	/* delete at completion of command */
	char	marked;		/* file has been Fmarked at least once; once
				 * set, this will never go off as undo doesn't
				 * revert to the dawn of time */
	long	dev;		/* file system from which it was read */
	long	qid;		/* file from which it was read */
	long	date;		/* time stamp of plan9 file */
	Posn	cp1, cp2;	/* Write-behind cache positions and */
	String	cache;		/* string */
	Rune	getcbuf[NGETC];
	int	ngetc;
	int	getci;
	Posn	getcp;
};

struct Mark
{
	Posn	p;
	Range	dot;
	Range	mark;
	Mod	m;
	short	s1;
};

/*
 * The precedent to any message in the transcript.
 * The component structures must be an integral number of Runes long.
 */
union Hdr
{
	struct _csl
	{
		short	c;
		short	s;
		long	l;
	}csl;
	struct _cs
	{
		short	c;
		short	s;
	}cs;
	struct _cll
	{
		short	c;
		long	l;
		long	l1;
	}cll;
	Mark	mark;
};

#define	Fgetc(f)  ((--
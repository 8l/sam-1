/* Copyright 2016 Rob King */

#include "sam.h"

#define BUFFER_MIN 65535
#define GAPSIZE(b) ((b)->ge - (b)->gs)
#define NBYTES(b) ((b)->size - GAPSIZE(b))

typedef size_t pos_t;
typedef struct Gapbuffer Gapbuffer;
struct Gapbuffer{
    size_t size;
    pos_t gs;
    pos_t ge;
    char *buf;
};

static void
movegap(Gapbuffer *b, pos_t p)
{
    if (p == b->gs)
        return;
    else if (p < b->gs){
        size_t d = b->gs - p;
        b->gs -= d;
        b->ge -= d;
        memmove(b->buf + b->ge, b->buf + b->gs, d);
    } else{
        size_t d = p - b->gs;
        b->gs += d;
        b->ge += d;
        memmove(b->buf + b->gs - d, b->buf + b->ge - d, d);
    }
}

static void
ensuregap(Gapbuffer *b, size_t l)
{
    size_t ns = b->size + l + BUFFER_MIN;
    size_t es = b->size - b->ge;

    if (GAPSIZE(b) >= l)
        return;

    b->buf = realloc(b->buf, ns);
    if (!b->buf)
        panic("out of memory");

    memmove(b->buf + (ns - es), b->buf + b->ge, es);
    b->ge = ns - es;
    b->size = ns;
}

static void
deletebuffer(Gapbuffer *b, pos_t p, size_t l)
{
    movegap(b, p);
    b->ge += l;
}

static size_t
readbuffer(Gapbuffer *b, pos_t p, size_t l, char *c)
{
    size_t r = 0;

    if (p < b->gs){
        size_t d = b->gs - p;
        size_t t = l > d ? d : l;

        memcpy(c, b->buf + p, t);
        c += t;
        l -= t;
        r += t;

        memcpy(c, b->buf + b->ge, l);
        r += l;
    } else{
        p += GAPSIZE(b);

        memcpy(c, b->buf + p, l);
        r = l;
    }

    return r;
}

static void
insertbuffer(Gapbuffer *b, pos_t p, const char *s, size_t l)
{
    ensuregap(b, l);
    movegap(b, p);
    memcpy(b->buf + b->gs, s, l);
    b->gs += l;
}

Buffer *
Bopen(void)
{
    Buffer *b = calloc(1, sizeof(Buffer));
    if (!b)
        panic("out of memory");

    b->gb = calloc(1, sizeof(Gapbuffer));
    if (!b->gb)
        panic("out of memory");

    b->gb->buf = calloc(1, BUFFER_MIN);
    if (!b->gb->buf)
        panic("out of memory");

    b->gb->size = BUFFER_MIN;
    b->gb->gs = 0;
    b->gb->ge = BUFFER_MIN;

    return b;
}

void
Bterm(Buffer *b)
{
    if (b){
        free(b->gb->buf);
        free(b->gb);
        free(b);
    }
}

int
Bread(Buffer *b, Rune *c, int l, Posn p)
{
    if (p + l >= b->nrunes)
        l = b->nrunes - p;

    if (l == 0)
        return 0;

    size_t r = readbuffer(b->gb, p * RUNESIZE, l * RUNESIZE, (char *)c);
    return (int)(r / RUNESIZE);
}

void
Binsert(Buffer *b, String *s, Posn p)
{
    if (s->n > 0){
        insertbuffer(b->gb, (size_t)(p * RUNESIZE), (char *)s->s, s->n * RUNESIZE);
        b->nrunes += s->n;
    }
}

void
Bdelete(Buffer *b, Posn p1, Posn p2)
{
    size_t l = p2 - p1;
    deletebuffer(b->gb, p1 * RUNESIZE, l * RUNESIZE);
    b->nrunes -= l;
}

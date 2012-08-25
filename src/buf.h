
#ifndef __BUF_H
#define __BUF_H

#include <stdint.h>
#include <stdlib.h>

#define SEQ_GT(s1, s2) ((int)((s1) - (s2)) > 0)
#define SEQ_LT(s1, s2) ((int)((s1) - (s2)) < 0)
#define SEQ_GE(s1, s2) ((int)((s1) - (s2)) >= 0)
#define SEQ_LE(s1, s2) ((int)((s1) - (s2)) <= 0)

struct mb {
	unsigned char	*head;
	unsigned char	*data;
	uint32_t	seq;
	uint32_t	len;
	struct mb	*next;
};

static inline void mb_free(struct mb *m)
{
	free(m->head);
	free(m);
}

struct buf {
	uint32_t	seq;
	struct mb	*first;
};

static inline void buf_init(struct buf *b, uint32_t seq)
{
	b->seq = seq;
	b->first = NULL;
}

int buf_add(struct buf *h, uint32_t seq, const unsigned char *data,
		uint32_t len);

static inline struct mb *buf_del(struct buf *b)
{
	if (b->first && b->first->seq == b->seq) {
		struct mb *m = b->first;

		b->first = m->next;
		b->seq = m->seq + m->len;
		return m;
	}

	return NULL;
}

void buf_drain_to(struct buf *h, uint32_t seq);
void buf_drain(struct buf *b);

#endif /* __BUF_H */

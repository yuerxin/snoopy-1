/**
 * Snoopy - A lightweight bypass censorship system for HTTP
 * Copyright (C) 2012- Changli Gao <xiaosuo@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "utils.h"
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

static inline unsigned char hexval(unsigned char v)
{
	switch (v) {
	case '0'...'9':
		return v - '0';
	case 'a'...'f':
		return v - 'a' + 10;
	case 'A'...'F':
		return v - 'A' + 10;
	default:
		return 0;
	}
}

int url_decode(unsigned char *buf, int len)
{
	unsigned char *dec = buf;
	int dec_len = 0;
	unsigned char c;

	while (len-- > 0) {
		c = *buf++;
		if (c == '%') {
			unsigned char l;

			if (!isxdigit((c = *buf++)) || !isxdigit((l = *buf++)))
				return -1;
			c = (hexval(c) << 4) | hexval(l);
			len -= 2;
		}
		*dec++ = c;
		dec_len++;
	}
	*dec = '\0';

	return dec_len;
}

void *xmemdup(const void *data, int len)
{
	void *r = malloc(len + 1);

	if (r) {
		memcpy(r, data, len);
		((char *)r)[len] = '\0';
	}

	return r;
}

int if_get_mtu(const char *name)
{
	int s = socket(AF_INET, SOCK_DGRAM, 0);
	struct ifreq ifr;

	if (s < 0)
		goto err;
	strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
	if (ioctl(s, SIOCGIFMTU, &ifr))
		goto err2;
	close(s);

	return ifr.ifr_mtu;
err2:
	close(s);
err:
	return -1;
}

#ifndef __GLIBC__
void *memmem(const void *haystack, size_t haystacklen,
		const void *needle, size_t needlelen)
{
	assert(needlelen > 0);

	while (haystacklen >= needlelen) {
		if (memcmp(haystack, needle, needlelen) == 0)
			return (void *)haystack;
		haystack++;
		haystacklen--;
	}

	return NULL;
}
#endif

#ifdef TEST
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
	char buf[] = "you%20are%20good%3b%3b";

	assert(url_decode(buf, strlen(buf)) == 14);
	printf("%s\n", buf);
	printf("%d\n", if_get_mtu("en1"));

	return 0;
}
#endif

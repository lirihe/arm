/*
 *  bytesize.c
 *
 *  Created by Jeppe Ledet-Pedersen on 14/09/10.
 *  Copyright 2010 GomSpace ApS. All rights reserved.
 *
 */

/* Pretty print bytes sizes */

#include <util/log.h>

void bytesize(char *buf, int len, long n) {
    char postfix;
    double size;
    if (n >= 1048576) {
        size = n/1048576.0;
        postfix = 'M';
    } else if (n >= 1024) {
        size = n/1024.;
        postfix = 'K';
    } else {
        size = n;
        postfix = 'B';
    }
    snprintf(buf, len, "%.1f%c", size, postfix);
}

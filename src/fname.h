#ifndef FNAME_H
#define FNAME_H

#include <stdint.h>

#define INVALID_FNAME 0xFFFFFFFFFFFFFFFF

int encoded_fname_len(uint64_t cname);
uint64_t encoded_fname(char *fname);

#endif

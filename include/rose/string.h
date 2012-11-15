#ifndef ROSE_STRING_H
#define ROSE_STRING_H

#include <rose/stdint.h>

#define NULL ((void *) 0)

void *memset(void *s, int c, size_t n);
void *memchr(const void *, int, size_t);
int memcmp(const void *, const void *, size_t);
void *memcpy(void *, const void *, size_t);
void *memmove(void *, const void *, size_t);
char *strcat(char *, const char *);
char *strchr(const char *, int);
int strcmp(const char *, const char *);
char *strcpy(char *, const char *);
size_t strlen(const char *);
char *strncat(char *, const char *, size_t);
int strncmp(const char *, const char *, size_t);
char *strncpy(char *, const char *, size_t);
char *strrchr(const char *, int);
char *strstr(const char *, const char *);

#endif

/*fort_record.h: Header file for fortran record file input/output. */
#ifndef FORT_RECORD_H
#define FORT_RECORD_H
#include <stdio.h>
#include <stddef.h>

int fread_fort_record(FILE *fp, void *buffer, size_t size);
int fwrite_fort_record(FILE *fp, const void *buffer, size_t size);
void fort_str_to_c(char *fort_str, unsigned int len);

#endif /* FORT_RECORD_H */
/* End of fort_record.h */
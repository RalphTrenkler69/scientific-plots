/* fort_record.c: Functions for reading and writing 
   of unformatted fortran record in binary. */
#include <stdlib.h>
#include <stdio.h>

int fread_fort_record(FILE *fp, void *buffer, size_t size) {
    unsigned int record_size;
    if (fread(&record_size, sizeof(record_size), 1, fp) != 1) {
        perror("Error reading record size");
        return -1;
    }
    if (record_size != size) {
        fprintf(stderr, "Record size mismatch: expected %zu, got %u\n",
            size, record_size);
        return -1;
    }
    if (fread(buffer, size, 1, fp) != 1) {
        perror("Error reading record");
        return -1;
    }
    if (fread(&record_size, sizeof(record_size), 1, fp) != 1) {
        perror("Error reading end of record size");
        return -1;
    }
    if (record_size != size) {
        fprintf(stderr, "End of record size mismatch: expected %zu, got %u\n",
            size, record_size);
        return -1;
    }
    return 0;
}

int fwrite_fort_record(FILE *fp, const void *buffer, size_t size) {
    unsigned int record_size = (unsigned int)size;
    if (fwrite(&record_size, sizeof(record_size), 1, fp) != 1) {
        perror("Error writing record size");
        return -1;
    }
    if (fwrite(buffer, size, 1, fp) != 1) {
        perror("Error writing record");
        return -1;
    }
    if (fwrite(&record_size, sizeof(record_size), 1, fp) != 1) {
        perror("Error writing end of record size");
        return -1;
    }
    return 0;
}

void fort_str_to_c(char *fort_str, unsigned int len) {
    int i;
    if (len == 0) {
        return; // No conversion needed for zero length
    }
    if (fort_str[len-1] != ' ') 
        fort_str[len-1] = '\0'; // Null-terminate
    else {
        for (i = len - 1; i >= 0 && fort_str[i] == ' '; i--) {
            // Trim trailing spaces
        }
        fort_str[i + 1] = '\0'; // Null-terminate after trimming
    }

}
/* End of fort_record.c */
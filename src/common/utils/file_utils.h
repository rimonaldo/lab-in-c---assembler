#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdio.h>

#define PATH_MAX 4096

/* Extracts the file basename (without extension) from a full path */
/* input_path: the full file path (e.g., "input/prog1.as") */
/* output_basename: output string (at least PATH_MAX characters) */
/* Returns 0 on success, negative on error */
int extract_basename_no_ext(const char *input_path, char *output_basename, size_t max_len);

/* Creates an output path in the output directory based on the base filename only */
/* output_basename: the filename without directories or extension (e.g., "prog1") */
/* output_path: output string for the full path (e.g., "output/prog1.am") */
/* max_len: size of the output_path string */
void make_output_path(const char *output_basename, char *output_path, size_t max_len);

/* Ensures that a directory named directory_path exists, creates it if it does not */
/* Returns 0 on success, negative on error */
int ensure_directory_exists(const char *directory_path);

#endif /* FILE_UTILS_H */
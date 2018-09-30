#include <stdlib.h>
#include <stdio.h>

#include <polyglot_api.h>

/*
 * 'slurp' reads the file identified by 'path' into a character buffer
 * pointed at by 'buf', optionally adding a terminating NUL if
 * 'add_nul' is true. On success, the size of the file is returned; on
 * failure, -1 is returned and ERRNO is set by the underlying system
 * or library call that failed.
 *
 * WARNING: 'slurp' malloc()s memory to '*buf' which must be freed by
 * the caller.
 */
long slurp(char const* path, char **buf, bool add_nul)
{
    FILE  *fp;
    size_t fsz;
    long   off_end;
    int    rc;

    /* Open the file */
    fp = fopen(path, "rb");
    if( NULL == fp ) {
        return -1L;
    }

    /* Seek to the end of the file */
    rc = fseek(fp, 0L, SEEK_END);
    if( 0 != rc ) {
        return -1L;
    }

    /* Byte offset to the end of the file (size) */
    if( 0 > (off_end = ftell(fp)) ) {
        return -1L;
    }
    fsz = (size_t)off_end;

    /* Allocate a buffer to hold the whole file */
    *buf = malloc( fsz+(int)add_nul );
    if( NULL == *buf ) {
        return -1L;
    }

    /* Rewind file pointer to start of file */
    rewind(fp);

    /* Slurp file into buffer */
    if( fsz != fread(*buf, 1, fsz, fp) ) {
        free(*buf);
        return -1L;
    }

    /* Close the file */
    if( EOF == fclose(fp) ) {
        free(*buf);
        return -1L;
    }

    if( add_nul ) {
        /* Make sure the buffer is NUL-terminated, just in case */
        buf[fsz] = '\0';
    }

    /* Return the file size */
    return (long)fsz;
}

int main(int argc, char **argv) {
  graal_isolate_t *isolate = NULL;
  graal_isolatethread_t *thread = NULL;
  
  if (graal_create_isolate(NULL, &isolate) != 0 || (thread = graal_current_thread(isolate)) == NULL) {
    fprintf(stderr, "initialization error\n");
    return 1;
  }
  
  poly_context context = NULL;
  
  if (poly_create_context(thread, NULL, 0, &context) != poly_ok) {
    fprintf(stderr, "initialization error\n");
    return 1;
  }
  
  char* language = "js";
  
  for (int n = 1; n < argc; n++) {
    if (argv[n][0] == '-') {
      language = &argv[n][1];
    } else {
      printf("language: %s\n", language);
      poly_value result = NULL;
      // TODO: write a function for reading the full file
      char *source;
      size_t size = slurp(argv[n], &source, false);

      if (poly_context_eval(thread, context, language, "unicalc", source, &result) != poly_ok) {
        fprintf(stderr, "eval error\n");
        return 1;
      }
      free(source);
      char buffer[1024];
      size_t length;
      
      if (poly_value_to_string_utf8(thread, result, buffer, sizeof(buffer), &length) != poly_ok) {
        fprintf(stderr, "to string error\n");
        return 1;
      }
      
      buffer[length] = '\0';
      printf("%s\n", buffer);
      
      poly_destroy_handle(thread, result);
    }
  }
  
  return 0;
}

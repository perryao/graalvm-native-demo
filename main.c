#include <stdlib.h>
#include <stdio.h>

#include <polyglot_api.h>


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
      FILE *infile;
      char *source;
      long numbytes;
      infile = fopen(argv[n], "r");
      /* quit if the file does not exist */
      if(infile == NULL)
        return 1;
      /* Get the number of bytes */
      fseek(infile, 0L, SEEK_END);
      numbytes = ftell(infile);
      printf("numbytes: %d\n", numbytes);
      /* reset the file position indicator to 
         the beginning of the file */
      fseek(infile, 0L, SEEK_SET);	

      /* grab sufficient memory for the 
         buffer to hold the text */
      source = (char*)calloc(numbytes, sizeof(char));	
      /* memory error */
      if(source == NULL)
        return 1;

      /* copy all the text into the buffer */
      fread(source, sizeof(char), numbytes, infile);

      fclose(infile);

      if (poly_context_eval(thread, context, language, "unicalc", source, &result) != poly_ok) {
        fprintf(stderr, "eval error\n");
        return 1;
      }
      /* free(source); */
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

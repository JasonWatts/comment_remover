// by Jason Watts, Trevor Kirkby, and Payton Dugas

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  if (argc < 2) {
    printf("Please specify a filename as an argument.\n");
    return 1; //By convention, the main() function should return 0 if it exited normally, and a nonzero value otherwise.
  }
  FILE *codefile = fopen(argv[1],"r");
  if (codefile == NULL) {
    printf("File does not exist.\n");
    return 2;
  }
  fseek(codefile, 0 , SEEK_END); //Point to the end of the file so we can get its length.
  long filesize = ftell(codefile); //Get the size of the file to create a correctly sized buffer.
  fseek(codefile, 0 , SEEK_SET); //Point back to the beginning of the file so we can read through it.
  char *buffer = malloc(sizeof(char) * filesize); //Allocate a buffer to hold the de-commented contents of the file.
  long index = 0; //Track where we are in the buffer, without changing the buffer pointer itself since we need to free() it later.
  short comment = 0; //0 if not in a comment, 1 if currently iterating inside of a "//" comment, 2 if currently iterating inside of a "/*" comment, 3 if we just ended a comment.
  int byte;
  char c;
  char prev = '\0'; //Keep track of two characters at a time, since that is what is needed to detect comment start and delimiters.
  while ((byte = fgetc(codefile)) != EOF) { //Make sure it's not an EOF before casting it is a char.
    c = (char) byte;
    if (prev) { //Only start parsing if we have 2 chars loaded already.
      if (comment) {
        if (comment == 1) {
          if (c == '\n') {comment = 0;} //Double slash comments are delimited by newlines.
        }
        else if (comment == 2) {
          if (prev == '*' && c == '/') {comment=3;} //Multiline comments are delimited by the sequence of chars "*/"
        }
        else if (comment == 3) {comment=0;} //If we just ended a comment (case 3), skip over copying the next char (which will be part of the comment delimiter) but then get back to copying the file contents straight into the buffer.
        else {
          printf("Invalid comment value: %d\n", comment);
          return 3;
        }
      } else if (prev == '/' && c == '/') {
        comment = 1;
      } else if (prev == '/' && c == '*') {
        comment = 2;
      } else {
        buffer[index] = prev; //Place the last char it into the buffer at the next spot we are pointing to.
        index++;
      }
    }
    prev = c;
  }
  if (!comment) {
    buffer[index] = prev;
  }
  fclose(codefile); //Close the file on read mode.
  codefile = fopen(argv[1],"w"); //Open it again on write mode.
  fprintf(codefile,"%s",buffer); //Add our uncommented code back into the file.
  fclose(codefile); //Close the file again.
  free(buffer); //The memory is probably freed up automatically once the process ends anyhow, but it's still very good practice to free() dynamically allocated memory.
  return 0;
}

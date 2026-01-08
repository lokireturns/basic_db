#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct
{
  char *buffer;
  size_t buffer_length;
  ssize_t input_length;
} InputBuffer;

InputBuffer *newInputBuffer()
{
  InputBuffer *input_buffer = (InputBuffer *)malloc(sizeof(InputBuffer));
  input_buffer->buffer = NULL; // Give it a known safe value
  input_buffer->buffer_length = 0;
  input_buffer->input_length = 0;
  return input_buffer;
}

void print_prompt() { printf("db > "); };

void read_input(InputBuffer *inputBuffer)
{
  ssize_t bytes_read = getline(&(inputBuffer->buffer), &(inputBuffer->buffer_length), stdin);

  if (bytes_read <= 0)
  {
    perror("Error reading input ");
    exit(EXIT_FAILURE);
  }

  // Ignore trailing newline
  inputBuffer->buffer_length = bytes_read - 1;
  inputBuffer->buffer[bytes_read - 1] = 0; // Convert '\n' to '\0' (null terminator)
}

// Free the memory of input buffer
// (getline allocates memory for input_buffer->buffer in read_input)
void close_input_buffer(InputBuffer *input_buffer)
{
  free(input_buffer->buffer);
  free(input_buffer);
}

typedef enum {
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum {
  PREPARE_SUCCESS,
  PREPARE_UNRECOGNISED_STATEMENT
} PrepareResult;

typedef enum {
  STATEMENT_INSERT,
  STATEMENT_SELECT
} StatementType;

typedef struct {
  StatementType type; 
} Statement;

MetaCommandResult do_meta_command(InputBuffer * input_buffer) {
  if(strcasecmp(input_buffer->buffer, ".exit") == 0) {
    exit(EXIT_SUCCESS);
  } else {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}

// Our proverbial SQL compiler


int main(int argc, char *argv[])
{
  InputBuffer *input_buffer = newInputBuffer();

  while (true)
  {
    print_prompt();
    read_input(input_buffer);

    if(input_buffer->buffer[0] == '.') {
      switch(do_meta_command(input_buffer)) {
        case (META_COMMAND_SUCCESS):
          continue;
        case (META_COMMAND_UNRECOGNIZED_COMMAND):
          printf("Command unregornised %s \n",  input_buffer->buffer);
          continue;
      }
    }

    Statement statement;
    switch (prepare_statement(input_buffer->buffer, &statement)) {
      case (PREPARE_SUCCESS):
        break;
      case (PREPARE_UNRECOGNISED_STATEMENT):
        printf("Unrecognised keyword at start of %s. \n", input_buffer->buffer);
        continue; 
    }

    // Our proverbial virtual machine
    execute_statement(&statement);
    printf("Statement executed. \n");
  }
      return 0;
}

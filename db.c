#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "page.h"

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

typedef enum
{
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum
{
  PREPARE_SUCCESS,
  PREPARE_UNRECOGNISED_STATEMENT,
  PREPARE_SYNTAX_ERROR
} PrepareResult;

typedef enum
{
  STATEMENT_INSERT,
  STATEMENT_SELECT
} StatementType;

typedef struct
{
  StatementType type;
  Row row_to_insert;
} Statement;

MetaCommandResult do_meta_command(InputBuffer *input_buffer)
{
  if (strcasecmp(input_buffer->buffer, ".exit") == 0)
  {
    exit(EXIT_SUCCESS);
  }
  else
  {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}

// Our proverbial SQL compiler
PrepareResult prepare_statement(InputBuffer *input_buffer, Statement *statement)
{
  // Using strncmp as insert will be followed by data
  if (strncmp(input_buffer->buffer, "insert", 6) == 0)
  {
    statement->type = STATEMENT_INSERT;
    int args_assigned = sscanf(input_buffer->buffer, "insert %d %s %s", &(statement->row_to_insert.id), statement->row_to_insert.username, statement->row_to_insert.email);
    if (args_assigned < 3)
    {
      return PREPARE_SYNTAX_ERROR;
    }
    return PREPARE_SUCCESS;
  }
  if (strcmp(input_buffer->buffer, "select") == 0)
  {
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }

  return PREPARE_UNRECOGNISED_STATEMENT;
}

ExecuteResult execute_insert(Statement* statement, Table* table) {
  if(table->num_rows >= TABLE_MAX_ROWS){
    return EXECUTE_TABLE_FULL;
  }

  Row *row_to_insert = &(statement->row_to_insert);
  print_row(row_to_insert);
  serialise_row(row_to_insert,row_slot(table, table->num_rows)); // Insert the next row
  table->num_rows += 1;
  return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement* statemene, Table* table) {
  for(uint32_t j = 0; j < table->num_rows; j++){
    Row *row;
    void* location_in_table = row_slot(table,j);
    deserialise_row(location_in_table, row);
    print_row(row);
  }
  return EXECUTE_SUCCESS;
}

Table* new_table(){
  Table* table = (Table*)malloc(sizeof(Table));
  for(uint32_t i = 0; i <= TABLE_MAX_PAGES; i++) {
    table->pages[i] = NULL;
  }
  table->num_rows = 0;
  return table;
}

void free_table(Table* table) {
  for(uint32_t i = 0; i <= table->num_rows; i++){
    free(table->pages[i]);
  }
  free(table);
}

ExecuteResult execute_statement(Table* table, Statement *statement)
{
  switch (statement->type)
  {
  case (STATEMENT_INSERT):
    return execute_insert(statement, table);
  case (STATEMENT_SELECT):
    return execute_select(statement, table);
  }
}

int main(int argc, char *argv[])
{
  InputBuffer *input_buffer = newInputBuffer();
  Table* table = new_table();

  // REPL
  while (true)
  {
    print_prompt();
    read_input(input_buffer);

    if (input_buffer->buffer[0] == '.')
    {
      switch (do_meta_command(input_buffer))
      {
      case (META_COMMAND_SUCCESS):
        continue;
      case (META_COMMAND_UNRECOGNIZED_COMMAND):
        printf("Command unrecognised %s \n", input_buffer->buffer);
        continue;
      }
    }

    Statement statement;
    switch (prepare_statement(input_buffer, &statement))
    {
    case (PREPARE_SUCCESS):
      break;
    case (PREPARE_UNRECOGNISED_STATEMENT):
      printf("Unrecognised keyword at start of %s. \n", input_buffer->buffer);
      continue;
    case (PREPARE_SYNTAX_ERROR):
      printf("Could not parse statement \n");
      continue;
    }

    // Our proverbial virtual machine
    switch(execute_statement(table, &statement)){
      case (EXECUTE_SUCCESS):
        printf("Executed \n");
        break;
      case (EXECUTE_TABLE_FULL):
        printf("Error: Table full");
        break;
      }
  }
  return 0;
}

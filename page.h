#pragma once

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

typedef struct
{
  uint32_t id;
  char username[COLUMN_USERNAME_SIZE];
  char email[COLUMN_EMAIL_SIZE];
} Row;

#define ID_SIZE sizeof(((Row*)0) -> id)
// Different syntax both cost zero at compile time
// Since C11 you can use compound literal - still prefer pointer syntax
#define USERNAME_SIZE sizeof(((Row){}.username))
#define EMAIL_SIZE sizeof(((Row){}.email))

#define ID_OFFSET offsetof(Row, id)
#define USERNAME_OFFSET offsetof(Row, username)
#define EMAIL_OFFSET offsetof(Row, email)

extern const uint32_t ROW_SIZE;
extern const uint32_t PAGE_SIZE;
extern const uint32_t ROWS_PER_PAGE;
extern const uint32_t TABLE_MAX_ROWS;

#define TABLE_MAX_PAGES 100

void print_row(Row* row);

typedef struct {
  uint32_t num_rows;
  void *pages[TABLE_MAX_PAGES];  
} Table;

typedef enum {
  EXECUTE_SUCCESS,
  EXECUTE_TABLE_FULL
} ExecuteResult;

void serialise_row(Row *source, void *destination);
void* row_slot(Table *table, uint32_t row_num);
void deserialise_row(void *source, Row *destination);
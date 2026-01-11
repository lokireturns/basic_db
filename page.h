#pragma once

#include <stdlib.h>
#include <stddef.h>

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
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

#define ID_OFFSET offsetof(Row, id)
#define USERNAME_OFFSET offsetof(Row, username)
#define EMAIL_OFFSET offsetof(Row, email)

const uint32_t PAGE_SIZE = 4096;
#define TABLE_MAX_PAGES 100
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
TABLE_MAX_ROWS = TABLE_MAX_PAGES * ROWS_PER_PAGE;

typedef struct {
  uint32_t num_rows;
  void *pages[TABLE_MAX_PAGES];  
} Table;
#include "page.h"
#include <string.h>

void serialise_row(Row *source, void *destination) {
    memcpy(destination + ID_OFFSET,  &(source->id), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, source->username, USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, source->email, EMAIL_SIZE);
}

void deserialise_row(void *source, Row *destination) {
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

// Where to read/write in memory for a row
void* row_slot(Table *table, uint32_t row_num){
    uint32_t page_num = row_num / ROWS_PER_PAGE; // Remember: Integer division in C
    void* page = table->pages[page_num];
    if(page == NULL) { // If valid memory addr
        void *allocated_memory = malloc(PAGE_SIZE);
        table->pages[page_num] = allocated_memory;
        page = allocated_memory;
    }
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE; // Offset within the page
    return page + byte_offset;
}

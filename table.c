#include "table.h"

void tableInit(Table *table) {
	table->count = 0;
	table->used = 0;
	table->entries = NULL;
}

void tableFree(Table *table) {
	FREE_ARRAY(Entry, table->entries, table->count);
	tableInit(table);
}

Entry *findEntry(Entry *entries, size_t size, ObjString *key) {
	uint32_t index = key->hash % size;
	while(1) {
		Entry *entry = &entries[index];
		if(entry->key == key || entry->key == NULL) {
			return entry;
		}
		index = (index+1) % size;
	}
}

void adjustCapacity(Table *table, size_t capacity) {
	Entry *entries = ALLOCATE(Entry, capacity);
	for(size_t i=0;i<capacity;i++) {
		entries[i].value = makeNil();
		entries[i].key   = NULL;
	}
	for(size_t i=0;i<capacity;i++) {
		Entry *entry = &(table->entries[i]);
		if(entry->key == NULL) continue;
		Entry *dest = findEntry(entries, capacity, entry->key);
		dest->key = entry->key;
		dest->value = entry->value;
	}
	FREE_ARRAY(Entry, table->entries, table->count);
	table->entries = entries;
	table->count = capacity;
}

void tableAddAll(Table *src, Table *dest) {
	for(size_t i=0;i<src->count;i++) {
		Entry *entry = &(src->entries[i]);
		if(entry->key != NULL) {
			tableSet(dest, entry->key, entry->value);
		}
	}
}

uint8_t tableSet(Table *table, ObjString *key, Value value) {
	if((double)table->used >= (double)(table->count) * TABLE_MAX_LOAD) {
		size_t capacity = GROW_CAPACITY(table->count);
		adjustCapacity(table, capacity);
	}
	Entry *entry = findEntry(Entry *entries, table->count, key);
	uint8_t isNewkey = entry->key == NULL;
	if(isNewKey) table->used++;
	entry->key = key;
	entry->value = value;
	return isNewKey;
}

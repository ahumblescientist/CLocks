#include "table.h"
#include <string.h>

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
	if(size==0) return NULL;
	uint32_t index = key->hash % size;
	Entry *tombstone = NULL;
	while(1) {
		Entry *entry = &entries[index];
		if(entry->key == NULL) {
			if(isNil(entry->value)) {
				return (tombstone != NULL ? tombstone : entry);
			} else {
				tombstone = entry;
			}
		} else if(entry->key == key) {
			return entry;
		}
		index = (index+1) % size;
	}
}

void adjustCapacity(Table *table, size_t capacity) {
	Entry *entries = ALLOCATE(Entry, capacity);
	table->used = 0;
	for(size_t i=0;i<capacity;i++) {
		entries[i].value = makeNil();
		entries[i].key   = NULL;
	}
	for(size_t i=0;i<table->count;i++) {
		Entry *entry = &(table->entries[i]);
		if(entry->key == NULL) continue;
		Entry *dest = findEntry(entries, capacity, entry->key);
		dest->key = entry->key;
		dest->value = entry->value;
		table->used++;
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
	Entry *entry = findEntry(table->entries, table->count, key);
	uint8_t isNewKey = entry->key == NULL;
	if(isNewKey && isNil(entry->value)) table->used++;
	entry->key = key;
	entry->value = value;
	return isNewKey;
}

uint8_t tableGet(Table *table, ObjString *key, Value *value) {
	if(table->count == 0) return 0;
	Entry *entry = findEntry(table->entries, table->count, key);
	if(entry->key == NULL) return 0;
	*value = entry->value;
	return 1;
}

uint8_t tableDel(Table *table, ObjString *key) {
	if(table->count == 0) return 0;
	Entry *entry = findEntry(table->entries, table->count, key);
	if(entry->key == NULL) return 0;
	entry->key = NULL;
	entry->value = makeBool(1);
	return 1;
}

ObjString *tableFindString(Table *table, char *start, size_t length, uint32_t hash) {
	if(table->used == 0) return NULL;
	uint32_t index = hash % table->count;
	while(1) {
		Entry *entry = &table->entries[index];
		if(entry->key == NULL) {
			if(isNil(entry->value)) return NULL;
		} else if(entry->key->length == length && entry->key->hash == hash && !memcmp(entry->key->cstr, start, length)) {
			return entry->key;
		}
		index = (index + 1) % table->count;
	}
}

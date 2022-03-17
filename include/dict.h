//
// Created by wangxz on 3/17/22.
//
#pragma once

#include "utils.h"

struct DictEntry {
  struct DictEntry *next;
  hash_t hash;
};

struct Dict {
  struct DictEntry **entries;
  ssize_t size;
  ssize_t n_free;
  ssize_t n_used;
};

struct Dict *Dict_Create();
void Dict_Destroy(struct Dict *dict);
bool Dict_Insert(struct Dict *dict, hash_t hash);
bool Dict_InsertEntry(struct Dict *dict, struct DictEntry *entry);
bool Dict_Remove(struct Dict *dict, hash_t hash);
bool Dict_RemoveEntry(struct Dict *dict, struct DictEntry *entry);

//
// Created by wangxz on 3/17/22.
//

#include "dict.h"
#include "memory_pool.h"

static ssize_t PRIMES[] = {211, 431, 863, 1733, 3467, 6947,
                           13901, 27803, 55609, 111227, 222461,
                           444929, 889871, 1779761, 3559537, 7119103,
                           14238221, 28476473, 56952947, 113905901};

static const ssize_t N_PRIMES = sizeof(PRIMES) / sizeof(ssize_t);

CREATE_MEMORY_POOL(DictEntry, 1024)

struct Dict *Dict_Create() {
  struct Dict *ret = malloc(sizeof(struct Dict));
  if (ret == NULL) {
    return NULL;
  }

  memset(ret, 0, sizeof(*ret));

  ret->entries = malloc(PRIMES[0] * sizeof(struct DictEntry*));
  if (ret->entries == NULL) {
    goto error;
  }

  ret->size = PRIMES[0];
  ret->n_free = PRIMES[0];
  return ret;

  error:
  free(ret);
  return NULL;
}

void Dict_Destroy(struct Dict *dict) {
  free(dict->entries);
  free(dict);
}

static bool Dict_Expand(struct Dict *dict) {
  ssize_t expand_size = 0;
  for (int i = 0; i < N_PRIMES; ++i) {
    if (dict->size < PRIMES[i]) {
      expand_size = PRIMES[i];
      break;
    }
  }

  if (expand_size == 0) {
    return false;
  }

  struct DictEntry **new_entries = malloc(expand_size * sizeof(struct DictEntry*));
  if (new_entries == NULL) {
    return false;
  }

  //rehash
  for (int i = 0; i < dict->size; ++i) {
    if (dict->entries[i] != NULL) {
      struct DictEntry *tmp = dict->entries[i];
      while (tmp != NULL) {
        struct DictEntry **new_pos = &new_entries[tmp->hash % expand_size];
        while (*new_pos != NULL) {
          new_pos = &(*new_pos)->next;
        }
        *new_pos = tmp;
        tmp = tmp->next;
        (*new_pos)->next = NULL;
      }
    }
  }

  free(dict->entries);
  dict->entries = new_entries;
  dict->size = expand_size;
  dict->n_free = expand_size - dict->n_used;
  return true;
}

bool Dict_Insert(struct Dict *dict, hash_t hash) {
  if (dict->n_used > dict->n_free && !Dict_Expand(dict)) {
    return false;
  }

  struct DictEntry **pos = &dict->entries[hash % dict->size];
  while (*pos != NULL) {
    if ((*pos)->hash == hash) {
      return false;
    }
    pos = &(*pos)->next;
  }

  struct DictEntry *ent = DictEntry_Malloc(&DictEntry_pool);
  if (ent == NULL) {
    return false;
  }

  ent->next = NULL;
  ent->hash = hash;
  (*pos) = ent;
  ++dict->n_used;
  --dict->n_free;
  return true;
}

bool Dict_InsertEntry(struct Dict *dict, struct DictEntry *entry) {
  if (dict->n_used > dict->n_free && !Dict_Expand(dict)) {
    return false;
  }

  struct DictEntry **pos = &dict->entries[entry->hash % dict->size];
  while (*pos != NULL) {
    if ((*pos)->hash == entry->hash) {
      return false;
    }
    pos = &(*pos)->next;
  }

  *pos = entry;
  entry->next = NULL;
  ++dict->n_used;
  --dict->n_free;
  return true;
}

bool Dict_Remove(struct Dict *dict, hash_t hash) {
  struct DictEntry **pos = &dict->entries[hash % dict->size];
  struct DictEntry *prev, *cur;
  if (*pos == NULL) {
    return false;
  }

  if ((*pos)->hash == hash) {
    DictEntry_Free(&DictEntry_pool, *pos);
    *pos = NULL;
    goto done;
  }

  prev = *pos;
  cur = prev->next;
  while (cur != NULL) {
    if (cur->hash == hash) {
      prev->next = cur->next;
      DictEntry_Free(&DictEntry_pool, cur);
      goto done;
    }
    prev = cur;
    cur = cur->next;
  }

  return false;

  done:
  ++dict->n_free;
  --dict->n_used;
  return true;
}

bool Dict_RemoveEntry(struct Dict *dict, struct DictEntry *entry) {
  struct DictEntry **pos = &dict->entries[entry->hash % dict->size];
  struct DictEntry *prev, *cur;
  if (*pos == NULL) {
    return false;
  }

  if ((*pos) == entry) {
    *pos = NULL;
    goto done;
  }

  prev = *pos;
  cur = prev->next;
  while (cur != NULL) {
    if (cur == entry) {
      prev->next = cur->next;
      goto done;
    }
    prev = cur;
    cur = cur->next;
  }

  return false;

  done:
  ++dict->n_free;
  --dict->n_used;
  return true;
}
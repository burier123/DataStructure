//
// Created by wangxz on 3/18/22.
//

#include "heap.h"
#include "memory_pool.h"

CREATE_MEMORY_POOL(HeapEntry, 1024)

static const ssize_t HEAP_INIT_SIZE = 1024;
const int HEAP_D = 2;

static inline void SwapPtr(void **p1, void **p2) {
  void *tmp = *p1;
  *p1 = *p2;
  *p2 = tmp;
}

static bool Heap_Expand(struct Heap *heap) {
  ssize_t new_size = heap->capacity * 2;
  struct HeapEntry **new_entries = malloc(new_size * sizeof(struct HeapEntry*));
  if (new_entries == NULL) {
    return false;
  }

  memset(new_entries, 0, new_size * sizeof(struct HeapEntry*));
  memcpy(new_entries, heap->entries, heap->size * sizeof(struct HeapEntry*));
  free(heap->entries);
  heap->entries = new_entries;
  heap->capacity = new_size;
  return true;
}

struct Heap *Heap_Create() {
  struct Heap *ret = malloc(sizeof(struct Heap));
  if (ret == NULL) {
    return NULL;
  }

  ret->entries = malloc(HEAP_INIT_SIZE * sizeof(struct HeapEntry*));
  if (ret->entries == NULL) {
    free(ret);
    return NULL;
  }

  memset(ret->entries, 0, HEAP_INIT_SIZE * sizeof(struct HeapEntry*));
  ret->size = 0;
  ret->capacity = HEAP_INIT_SIZE;
  return ret;
}

void Heap_Destroy(struct Heap *heap) {
  free(heap->entries);
  free(heap);
}

struct HeapEntry *MaxHeap_Push(struct Heap *heap, hash_t hash) {
  if (heap == NULL || (heap->size == heap->capacity && !Heap_Expand(heap))) {
    return NULL;
  }

  struct HeapEntry *new_entry = HeapEntry_Malloc(&HeapEntry_pool);
  if (new_entry == NULL) {
    return NULL;
  }
  new_entry->hash = hash;

  ssize_t cur_idx = heap->size;
  heap->entries[cur_idx] = new_entry;
  while (cur_idx != 0) {
    ssize_t up_idx = cur_idx / HEAP_D - (cur_idx % HEAP_D == 0 ? 1 : 0);
    if (heap->entries[up_idx]->hash < heap->entries[cur_idx]->hash) {
      SwapPtr(&heap->entries[up_idx], &heap->entries[cur_idx]);
    } else {
      break;
    }
    cur_idx = up_idx;
  }

  ++heap->size;
  return new_entry;
}

struct HeapEntry *MinHeap_Push(struct Heap *heap, hash_t hash) {
  if (heap == NULL || (heap->size == heap->capacity && !Heap_Expand(heap))) {
    return NULL;
  }

  struct HeapEntry *new_entry = HeapEntry_Malloc(&HeapEntry_pool);
  if (new_entry == NULL) {
    return NULL;
  }
  new_entry->hash = hash;

  ssize_t cur_idx = heap->size;
  heap->entries[cur_idx] = new_entry;
  while (cur_idx != 0) {
    ssize_t up_idx = cur_idx / HEAP_D - (cur_idx % HEAP_D == 0 ? 1 : 0);
    if (heap->entries[up_idx]->hash > heap->entries[cur_idx]->hash) {
      SwapPtr(&heap->entries[up_idx], &heap->entries[cur_idx]);
    } else {
      break;
    }
    cur_idx = up_idx;
  }

  ++heap->size;
  return new_entry;
}

bool MaxHeap_EntryPush(struct Heap *heap, struct HeapEntry *entry) {
  if (heap == NULL || (heap->size == heap->capacity && !Heap_Expand(heap))) {
    return false;
  }

  ssize_t cur_idx = heap->size;
  heap->entries[cur_idx] = entry;
  while (cur_idx != 0) {
    ssize_t up_idx = cur_idx / HEAP_D - (cur_idx % HEAP_D == 0 ? 1 : 0);
    if (heap->entries[up_idx]->hash < heap->entries[cur_idx]->hash) {
      SwapPtr(&heap->entries[up_idx], &heap->entries[cur_idx]);
    } else {
      break;
    }
    cur_idx = up_idx;
  }

  ++heap->size;
  return true;
}

bool MinHeap_EntryPush(struct Heap *heap, struct HeapEntry *entry) {
  if (heap == NULL || (heap->size == heap->capacity && !Heap_Expand(heap))) {
    return false;
  }

  ssize_t cur_idx = heap->size;
  heap->entries[cur_idx] = entry;
  while (cur_idx != 0) {
    ssize_t up_idx = cur_idx / HEAP_D - (cur_idx % HEAP_D == 0 ? 1 : 0);
    if (heap->entries[up_idx]->hash > heap->entries[cur_idx]->hash) {
      SwapPtr(&heap->entries[up_idx], &heap->entries[cur_idx]);
    } else {
      break;
    }
    cur_idx = up_idx;
  }

  ++heap->size;
  return true;
}

struct HeapEntry *MaxHeap_Pop(struct Heap *heap) {
  if (heap == NULL || heap->size == 0) {
    return NULL;
  }

  struct HeapEntry *ret = heap->entries[0];
  if (heap->size == 1) {
    goto done;
  }

  struct HeapEntry *hole = heap->entries[heap->size - 1];
  heap->entries[heap->size - 1] = NULL;
  ssize_t cur_idx = 0, down_idx = 1;
  while (down_idx < heap->size - 1) {
    ssize_t max_idx = down_idx;
    for (int i = 1; i < HEAP_D; ++i) {
      if (heap->entries[down_idx + i] == NULL) {
        break;
      }
      if (heap->entries[down_idx + i]->hash > heap->entries[max_idx]->hash) {
        max_idx = down_idx + i;
      }
    }

    if (heap->entries[max_idx]->hash > hole->hash) {
      SwapPtr(&heap->entries[cur_idx], &heap->entries[max_idx]);
    } else {
      break;
    }

    cur_idx = max_idx;
    down_idx = cur_idx * HEAP_D + 1;
  }

  heap->entries[cur_idx] = hole;

  done:
  heap->entries[heap->size - 1] = NULL;
  --heap->size;
  return ret;
}

struct HeapEntry *MinHeap_Pop(struct Heap *heap) {
  if (heap == NULL || heap->size == 0) {
    return NULL;
  }

  struct HeapEntry *ret = heap->entries[0];
  if (heap->size == 1) {
    goto done;
  }

  struct HeapEntry *hole = heap->entries[heap->size - 1];
  heap->entries[heap->size - 1] = NULL;
  ssize_t cur_idx = 0, down_idx = 1;
  while (down_idx < heap->size - 1) {
    ssize_t min_idx = down_idx;
    for (int i = 1; i < HEAP_D; ++i) {
      if (heap->entries[down_idx + i] == NULL) {
        break;
      }
      if (heap->entries[down_idx + i]->hash < heap->entries[min_idx]->hash) {
        min_idx = down_idx + i;
      }
    }

    if (heap->entries[min_idx]->hash < hole->hash) {
      SwapPtr(&heap->entries[cur_idx], &heap->entries[min_idx]);
    } else {
      break;
    }

    cur_idx = min_idx;
    down_idx = cur_idx * HEAP_D + 1;
  }

  heap->entries[cur_idx] = hole;

  done:
  heap->entries[heap->size - 1] = NULL;
  --heap->size;
  return ret;
}

struct HeapEntry *MaxHeap_GetMax(struct Heap *heap) {
  if (heap == NULL || heap->size == 0) {
    return NULL;
  }

  return heap->entries[0];
}

struct HeapEntry *MinHeap_GetMin(struct Heap *heap) {
  if (heap == NULL || heap->size == 0) {
    return NULL;
  }

  return heap->entries[0];
}

void PrintHeap(const struct Heap *heap) {
  if (heap == NULL || heap->size == 0) {
    return;
  }

  for (int i = 0; i < heap->size; ++i) {
    printf("%ld ", heap->entries[i]->hash);
  }
  printf("\n");
}
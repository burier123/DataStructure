//
// Created by wangxz on 2021/12/24.
//

#pragma once

#define CREATE_MEMORY_POOL(TYPE, BATCH_SIZE) \
static const ssize_t TYPE##_MemoryPool_SINGLE_SIZE = sizeof(struct TYPE) > sizeof(void*) ? \
sizeof(struct TYPE) : sizeof(void*); \
struct TYPE##_MemoryBatch { \
  struct TYPE##_MemoryBatch *next; \
  union { \
    struct TYPE item; \
    struct TYPE *ptr; \
  } data[BATCH_SIZE]; \
}; \
struct TYPE##_MemoryPool { \
  ssize_t n_used; \
  ssize_t n_free; \
  ssize_t capacity; \
  struct TYPE *free_list_head; \
  struct TYPE##_MemoryBatch *batch_list; \
}; \
static struct TYPE##_MemoryPool TYPE##_pool = {0, 0, 0, NULL, NULL}; \
bool TYPE##_MemoryBatch_Init(struct TYPE##_MemoryBatch *block) { \
  if (block == NULL) { \
    return false; \
  } \
  block->next = NULL; \
  for (int i = 0; i < BATCH_SIZE - 1; ++i) { \
    block->data[i].ptr = block->data + i + 1; \
  } \
  block->data[BATCH_SIZE - 1].ptr = NULL; \
  return true; \
} \
bool TYPE##_MemoryPool_Expand(struct TYPE##_MemoryPool *pool) { \
  if (pool == NULL) { \
    return false; \
  } \
  struct TYPE##_MemoryBatch *new_block = malloc(sizeof(struct TYPE##_MemoryBatch)); \
  if (new_block == NULL) { \
    return false; \
  } \
  if (!TYPE##_MemoryBatch_Init(new_block)) { \
    free(new_block); \
    return false; \
  } \
  new_block->data[BATCH_SIZE - 1].ptr = pool->free_list_head; \
  pool->free_list_head = new_block->data;    \
  struct TYPE##_MemoryBatch *tmp = pool->batch_list; \
  if (tmp == NULL) { \
    pool->batch_list = new_block; \
  } else { \
    while (tmp->next != NULL) tmp = tmp->next; \
    tmp->next = new_block; \
  } \
  pool->n_free += BATCH_SIZE; \
  pool->capacity += BATCH_SIZE; \
  return true; \
} \
struct TYPE##_MemoryPool *TYPE##_MemoryPool_Create() { \
  struct TYPE##_MemoryPool *ret = malloc(sizeof(struct TYPE##_MemoryPool)); \
  if (ret == NULL) { \
    return NULL; \
  } \
  memset(ret, 0, sizeof(struct TYPE##_MemoryPool)); \
  if (!TYPE##_MemoryPool_Expand(ret)) { \
    free(ret); \
    return NULL; \
  } \
  return ret; \
} \
struct TYPE *TYPE##_Malloc(struct TYPE##_MemoryPool *pool) { \
  if (pool->free_list_head == NULL && !TYPE##_MemoryPool_Expand(pool)) { \
    return NULL; \
  } \
  struct TYPE *ret = pool->free_list_head; \
  pool->free_list_head = *(struct TYPE **)pool->free_list_head; \
  --pool->n_free; \
  ++pool->n_used; \
  return ret; \
} \
void TYPE##_Free(struct TYPE##_MemoryPool *pool, struct TYPE *del) { \
  *(struct TYPE **)del = pool->free_list_head; \
  pool->free_list_head = del; \
  ++pool->n_free; \
  --pool->n_used; \
}

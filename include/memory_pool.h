//
// Created by wangxz on 2021/12/24.
//

#pragma once

#define MEMORY_POOL_BATCH_NUM 4092

#define CREATE_MEMORY_POOL(TYPE, BATCH_SIZE) \
static const ssize_t TYPE##_MemoryPool_SINGLE_SIZE = sizeof(struct TYPE) > sizeof(void*) ? \
sizeof(struct TYPE) : sizeof(void*); \
struct TYPE##_MemoryBatch { \
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
  struct TYPE##_MemoryBatch *batch_list[MEMORY_POOL_BATCH_NUM]; \
}; \
static struct TYPE##_MemoryPool TYPE##_pool; \
bool TYPE##_MemoryBatch_Init(struct TYPE##_MemoryBatch *block) { \
  if (block == NULL) { \
    return false; \
  } \
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
  struct TYPE##_MemoryBatch **tmp = NULL; \
  for (int i = 0; i < MEMORY_POOL_BATCH_NUM; ++i) { \
    if (pool->batch_list[i] == NULL) { \
      tmp = &pool->batch_list[i]; \
      break; \
    } \
  } \
  if (tmp == NULL) { \
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
  *tmp = new_block; \
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

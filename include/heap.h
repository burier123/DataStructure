//
// Created by wangxz on 3/18/22.
//
#pragma once

#include "utils.h"

struct HeapEntry {
  hash_t hash;
};

struct Heap {
  struct HeapEntry **entries;
  ssize_t size;
  ssize_t capacity;
};

const int HEAP_D;

struct Heap *Heap_Create();
void Heap_Destroy(struct Heap *heap);
struct HeapEntry *MaxHeap_Push(struct Heap *heap, hash_t hash);
struct HeapEntry *MinHeap_Push(struct Heap *heap, hash_t hash);
bool MaxHeap_EntryPush(struct Heap *heap, struct HeapEntry *entry);
bool MinHeap_EntryPush(struct Heap *heap, struct HeapEntry *entry);
struct HeapEntry *MaxHeap_Pop(struct Heap *heap);
struct HeapEntry *MinHeap_Pop(struct Heap *heap);
struct HeapEntry *MaxHeap_GetMax(struct Heap *heap);
struct HeapEntry *MinHeap_GetMin(struct Heap *heap);

void PrintHeap(const struct Heap *heap);

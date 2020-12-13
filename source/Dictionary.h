#pragma once
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief A container, which stores its data in a value, which is associated with a key. There is no limit to the number of elements, although the main capacity should be considered carefully.
 */
typedef struct Dictionary Dictionary;

Dictionary* DictionaryNew(const size_t keySize, const size_t valueSize, const uint64_t capacity);
void* DictionaryAdd(Dictionary* dict, const void* key, const void* value);
void DictionaryRemove(Dictionary* dict, const void* key);
void* DictionaryGet(Dictionary* dict, const void* key);
void DictionaryFree(Dictionary* dict);
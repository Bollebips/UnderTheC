#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
* @brief A container, which stores its data in a value, which is associated with a key. There is no limit to the number of elements, although the main capacity should be considered carefully.
*/
typedef struct Dictionary Dictionary;

Dictionary* DictionaryNew(const size_t keySize, const size_t valueSize);
void* DictionaryAdd(Dictionary* dict, const void* key, const void* value);
void DictionaryRemove(Dictionary* dict, const void* key);
void* DictionaryGet(const Dictionary* dict, const void* key);
void DictionaryResize(Dictionary* dict, const uint64_t newCapacity);
void DictionaryFree(Dictionary* dict);

uint64_t DictionaryNum(const Dictionary* dict);

#endif
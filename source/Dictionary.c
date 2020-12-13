#pragma once
#include "Dictionary.h"

#include <stdio.h>
#include "BucketArray.c"

/**
 * @brief An element, part of a dictionary.
 */
typedef struct Element
{
    struct Element* nextElement;    // Pointer to the next element, used in case of hash collision.
    bool isOccupied;                // Wether or not this element has data stored in it.
    char keyValuePair[];            // Array of bytes, containing the key and value.
} Element;

/**
 * @brief A container, which stores its data in a value, which is associated with a key. There is no limit to the number of elements, although the main capacity should be considered carefully.
 */
struct Dictionary
{
    uint64_t capacity;                  // The number of elements in the main array of elements.
    size_t keySize;                     // Memory footprint of the key data.
    size_t valueSize;                   // Memory footprint of the value data.
    BucketArray collisionElements;      // Array of elements that collided with other elements in the main array.
    Element elements[];                 // The main array of elements.
};

static const uint64_t HASH_PRIME = 1099511628211U; // 2^40 + 2^8 + 0xb3
static const uint64_t HASH_OFFSET = 14695981039346656037U;
static const float COLLISION_BUCKET_SIZE_PER_MAIN_ARRAY_SIZE = 0.3f;

static void DictionaryInit(Dictionary* dict, size_t keySize, size_t valueSize, const uint64_t capacity);
static Element* AddCollidingElement(Dictionary* dict, Element* prevElement, const void* key, const void* value);
static void RemoveCollidingElement(Dictionary* dict, Element* prevElement, const void* key);
static Element* GetCollidingElement(Dictionary* dict, Element* prevElement, const void* key);
static uint64_t HashKey(const void* key, const size_t keySize);

static size_t ElementSize(Dictionary* dict);
static Element* ElementGetNextElement(const Element* element);
static bool ElementGetIsOccupied(const Element* element);
static void* ElementGetKey(const Element* element);
static void* ElementGetValue(const Element* element, const size_t keySize);
static void ElementSet(Element* element, Element* nextElement, bool isOccupied, const void* key, const void* value, size_t keySize, size_t valueSize);

/**
 * @brief  Creates a new dictionary, and initializes it.
 * @param keySize The memory footprint of the key.
 * @param valueSize The memory footprint of the value.
 * @param capacity The number of elements available in the main array. Although the total amount of elements in the dictionary can be limitless, the capacity of the dictionary should be large enough for performace reasons.
 * @return Dictionary* A pointer to the newly created dictionary.
 */
Dictionary* DictionaryNew(size_t keySize, size_t valueSize, const uint64_t capacity)
{
    Dictionary* newDictionary = malloc(sizeof(Dictionary) + capacity * (sizeof(Element*) + sizeof(bool) + keySize + valueSize));

    assert(keySize > 0);
    assert(valueSize > 0);
    assert(capacity > 0);
    assert(newDictionary != NULL);

    DictionaryInit(newDictionary, keySize, valueSize, capacity);

    return newDictionary;
}

/**
 * @brief Add a new element to the dictionary. If the given key is already present in the dictionary, the element will not be added, and NULL will be returned.
 * @param dict The dictionary to add the element to.
 * @param key The key of the element.
 * @param value The value of the element.
 * @return void* A pointer to the value associated with the given key, if added successfully. NULL if the given key was already present in the dictionary.
 */
void* DictionaryAdd(Dictionary* dict, const void* key, const void* value)
{
    uint64_t hash = HashKey(key, dict->keySize);
    uint64_t index = hash % dict->capacity;

    Element* newElement = (void*) &(dict->elements) + (index * ElementSize(dict));

    if(ElementGetIsOccupied(newElement))
    {
        if(memcmp(key, ElementGetKey(newElement), dict->keySize) == 0) // Key is already present in the dictionary.
        {
            return NULL;
        }
        else // The key present is different from the currently insterted key.
        {
            newElement = AddCollidingElement(dict, newElement, key, value);
            if(newElement == NULL)
            {
                return NULL;
            }
            else
            {
                return ElementGetValue(newElement, dict->keySize);
            }
        }
    }
    else
    {
        ElementSet(newElement, NULL, true, key, value, dict->keySize, dict->valueSize);
        return ElementGetValue(newElement, dict->keySize);
    }
}

// TODO: Add a removedElement parameter, to retrieve the removed data.

/**
 * @brief Remove the element, associated with this key, from the dictionary,
 * @param dict The dictionary to remove the element from.
 * @param key The key of the element to be removed. If this key is not present in the dictionary, nothing will be removed.
 */
void DictionaryRemove(Dictionary* dict, const void* key)
{
    assert(dict != NULL);
    assert(key != NULL);

    uint64_t hash = HashKey(key, dict->keySize);
    uint64_t index = hash % dict->capacity;

    Element* elementToRemove = (void*) &(dict->elements) + (index * ElementSize(dict));

    if(ElementGetIsOccupied(elementToRemove))
    {
        if(memcmp(key, ElementGetKey(elementToRemove), dict->keySize) == 0)
        {
            Element* nextElement = ElementGetNextElement(elementToRemove);
            if(nextElement != NULL)
            {
                memcpy(elementToRemove, nextElement, ElementSize(dict));
                memset(nextElement, 0, ElementSize(dict));
            }
            else
            {
                memset(elementToRemove, 0, ElementSize(dict));
            }
        }
        else
        {
            RemoveCollidingElement(dict, elementToRemove, key);
        }
    }

    // The requested key is not present in the dictionary.
}

/**
 * @brief Retrieve the element, associated with this key, from the dictionary.
 * @param dict The dictionary to retrieve this element from.
 * @param key The key of the element to be retrieved.
 * @return void* A pointer to the value associated with this key. NULL if this key is not present in the dictionary.
 */
void* DictionaryGet(Dictionary* dict, const void* key)
{
    assert(dict != NULL);
    assert(key != NULL);

    uint64_t hash = HashKey(key, dict->keySize);
    uint64_t index = hash % dict->capacity;

    Element* elementLocation = (void*) &(dict->elements) + (index * ElementSize(dict));

    if(ElementGetIsOccupied(elementLocation))
    {
        if(memcmp(key, ElementGetKey(elementLocation), dict->keySize) == 0)
        {
            return ElementGetValue(elementLocation, dict->keySize);
        }
        else
        {
            elementLocation = GetCollidingElement(dict, elementLocation, key);
            return ElementGetValue(elementLocation, dict->keySize);
        }
    }

    // The requested key is not present in the dictionary.
    return NULL;
}

/**
 * @brief Free the dictionary.
 * @param dict The dictionary to free.
 */
void DictionaryFree(Dictionary* dict)
{
    BucketArrayFree(&(dict->collisionElements));
    free(dict);
}

/* ------------------------------------------------------------------------------------------------------------------ */

/**
 * @brief Initialize an existing dictionary. Only used internally. When calling DictionaryNew, the dictionary will already be initialized.
 * @param dict The dictionary to be initalized.
 * @param keySize The memory footprint of the key.
 * @param valueSize The memory footprint of the value.
 * @param capacity The number of elements available in the main array. Although the total amount of elements in the dictionary can be limitless, the capacity of the dictionary should be large enough for performace reasons.
 */
static void DictionaryInit(Dictionary* dict, size_t keySize, size_t valueSize, const uint64_t capacity)
{
    dict->keySize = keySize;
    dict->valueSize = valueSize;
    dict->capacity = capacity;

    memset(&(dict->elements), 0, ElementSize(dict) * capacity);

    BucketArrayInit(&(dict->collisionElements), ElementSize(dict), ceil(capacity * COLLISION_BUCKET_SIZE_PER_MAIN_ARRAY_SIZE));
}

/**
 * @brief Add an element, whose hash has collided with another element. This element will be added in a linked list struture.
 * @param dict The dictionary to add the element to.
 * @param prevElement The previous element to be added, most likely the element which the newly added element collided with.
 * @param key The key of the element to be added.
 * @param value The value of the element to be added.
 * @return Element* A pointer to the newly added element.
 */
static Element* AddCollidingElement(Dictionary* dict, Element* prevElement, const void* key, const void* value)
{
    Element* nextElement = ElementGetNextElement(prevElement);
    if(nextElement == NULL)
    {
        void* newElement = malloc(ElementSize(dict));

        ElementSet(newElement, NULL, true, key, value, dict->keySize, dict->valueSize);

        newElement = BucketArrayAdd(&(dict->collisionElements), newElement);

        prevElement->nextElement = newElement;

        return newElement;
    }
    else
    {
        void* presentKey = ElementGetKey(nextElement);
        if(memcmp(key, presentKey, dict->keySize) == 0)
        {
            // Key is already present in the dictionary.
            return NULL;
        }
        else
        {
            return AddCollidingElement(dict, nextElement, key, value);
        }
    }
}

/**
 * @brief Remove an element, whose hash has collided with another element.
 * @param dict The dictionary to remove the element from.
 * @param prevElement The previous element to be removed, most likely the element which the newly added element collided with.
 * @param key The key of the element to be removed.
 */
static void RemoveCollidingElement(Dictionary* dict, Element* prevElement, const void* key)
{
    assert(dict != NULL);
    assert(prevElement != NULL);
    assert(key != NULL);

    Element* elementToRemove = ElementGetNextElement(prevElement);

    if(elementToRemove == NULL)
    {
        // The requested key is not present in the dictionary.
        return;
    }

    if(ElementGetIsOccupied(elementToRemove))
    {
        if(memcmp(key, elementToRemove, dict->keySize) == 0)
        {
            Element* nextElement = ElementGetNextElement(elementToRemove);

            prevElement->nextElement = nextElement;
            memset(elementToRemove, 0, ElementSize(dict));
        }
        else
        {
            RemoveCollidingElement(dict, elementToRemove, key);
        }
    }
    else
    {
        // !! THIS SHOULD NEVER HAPPEN !!
        // The requested key is not present in the dictionary.
        return;
    }
}

/**
 * @brief Retrieve an element, whose hash has collided with another element.
 * @param dict The dictionary to retrieve the element from.
 * @param prevElement The previous element to be retrieved, most likely the element which the newly added element collided with.
 * @param key The key of the element to be retrieved.
 * @return Element* A pointer to the value associated with the given key, if added successfully. NULL if the given key was already present in the dictionary.
 */
static Element* GetCollidingElement(Dictionary* dict, Element* prevElement, const void* key)
{
    assert(dict);
    assert(key);

    Element* nextElement = ElementGetNextElement(prevElement);

    if(nextElement == NULL)
    {
        // The requested key is not present in the dictionary.
        return NULL;
    }

    if(ElementGetIsOccupied(nextElement))
    {
        if(memcmp(key, ElementGetKey(nextElement), dict->keySize) == 0)
        {
            return nextElement;
        }
    }
    else
    {
        // The requested key is not present in the dictionary.
        return NULL;
    }

    return GetCollidingElement(dict, nextElement, key);
}

/**
 * @brief Returns a hash, computed from the given key.
 * @param key The key data to be hashed.
 * @param keySize The memory footprint of the key.
 * @return uint64_t A semi-unique hash, computed from the given key.
 */
static uint64_t HashKey(const void* key, const size_t keySize)
{
    uint8_t* keyBytes = (uint8_t*) key; // The data gets converted to an array of uint8.

    uint64_t hash = HASH_OFFSET;

    for(int i = 0; i < keySize; ++i) // FNV-1a hashing algorithm
    {
        hash = hash ^ keyBytes[i];
        hash *= HASH_PRIME;
    }

    return hash;
}

/**
 * @brief Get the memory footprint of 1 element in the dictionary.
 * @param dict The dictionary the element belongs to.
 * @return size_t The memory footprint of 1 element in the dictionary.
 */
static size_t ElementSize(Dictionary* dict)
{
    return sizeof(Element*) + sizeof(bool) + dict->keySize + dict->valueSize; // DictElement: key, value, next_element (in case of collision), wether or not the element is used.
}

/**
 * @brief Get a pointer to the next element, linked to a given element.
 * @param element The element to get the next element from.
 * @return Element* The pointer to the next element. NULL if there is no next element linked to the given element.
 */
static Element* ElementGetNextElement(const Element* element)
{
    assert(element != NULL);

    return element->nextElement;
}
 /**
  * @brief Get wether or not there is data stored in this element.
  * @param element The element to be checked if there is data present.
  * @return Wether or not there is data stored in this element.
  */
static bool ElementGetIsOccupied(const Element* element)
{
    assert(element != NULL);

    return element->isOccupied;
}

/**
 * @brief Get the key, stored in the given element.
 * @param element The element to get the key from.
 * @return void* A pointer to the key, stored in the given element.
 */
static void* ElementGetKey(const Element* element)
{
    assert(element != NULL);

    return (void*) &(element->keyValuePair);
}

/**
 * @brief Get the value, stored in the given element.
 * @param element The element to get the value from.
 * @param keySize The memory footprint of the key, stored in this element. This is necessary, because the value is stored after the key,
 * @return void* A pointer to the key, stored in the given element.
 */
static void* ElementGetValue(const Element* element, const size_t keySize)
{
    assert(element != NULL);
    assert(keySize > 0);

    return (void*) &(element->keyValuePair) + keySize;
}

/**
 * @brief Set the data of an element.
 * @param element The element to set the data from.
 * @param nextElement A pointer to the next element. Can be left NULL if no next element is linked to the given element.
 * @param isOccupied Wether or not there is data stored in this element.
 * @param key A pointer to the key data to be copied into this element.
 * @param value A pointer to the value data to be copied into this element.
 * @param keySize The memory footprint of the key data.
 * @param valueSize The memory footprint of the value data.
 */
static void ElementSet(Element* element, Element* nextElement, bool isOccupied, const void* key, const void* value, size_t keySize, size_t valueSize)
{
    assert(element != NULL);
    assert(keySize > 0);
    assert(valueSize > 0);

    element->nextElement = nextElement;
    element->isOccupied = isOccupied;
    memcpy(ElementGetKey(element), key, keySize);
    memcpy(ElementGetValue(element, keySize), value, valueSize);
}
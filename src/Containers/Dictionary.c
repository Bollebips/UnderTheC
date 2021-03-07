#include "Dictionary.h"

#include "Math/Math.h"
#include "Logger.h"
#include "Utils/Hash.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

static const int INITIAL_CAPACITY = 16;
static const float MAX_LOAD_FACTOR = 0.7f;

static Element* AddCollidingElement(Dictionary* dict, Element* prevElement, const void* key, const void* value);
static void RemoveCollidingElement(Dictionary* dict, Element* prevElement, const void* key);
static Element* GetCollidingElement(const Dictionary* dict, Element* prevElement, const void* key);

static size_t ElementSize(const Dictionary* dict);
static Element* ElementNextElement(const Element* element);
static bool ElementIsOccupied(const Element* element);
static void* ElementKey(const Element* element);
static void* ElementValue(const Element* element, const size_t keySize);
static void ElementSet(Element* element, Element* nextElement, bool isOccupied, const void* key, const void* value, size_t keySize, size_t valueSize);

/**
 * @brief  Creates a new dictionary, and initializes it.
 * @param keySize The memory footprint of the key.
 * @param valueSize The memory footprint of the value.
 * @param capacity The number of elements available in the main array. Although the total amount of elements in the dictionary can be limitless, the capacity of the dictionary should be large enough for performace reasons.
 * @return Dictionary* A pointer to the newly created dictionary.
 */
Dictionary* DictionaryNew(size_t keySize, size_t valueSize)
{
    LogAssert(keySize > 0);
    LogAssert(valueSize > 0);

    Dictionary* newDictionary = malloc(sizeof(Dictionary));

    LogAssert(keySize > 0);
    LogAssert(valueSize > 0);
    LogAssert(newDictionary != NULL);

    DictionaryInit(newDictionary, keySize, valueSize);

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
    LogAssert(dict != NULL);
    LogAssert(key != NULL);
    LogAssert(value != NULL);

    float loadFactor = DictionaryNum(dict) / ArrayCapacity(&(dict->elements));
    if(loadFactor >= MAX_LOAD_FACTOR)
    {
        DictionaryResize(dict, ArrayCapacity(&(dict->elements)) * GOLDEN_RATIO);
    }

    uint64_t hash = HashFNV1a64(key, dict->keySize);
    uint64_t index = hash % ArrayCapacity(&(dict->elements));

    Element* newElement = (Element*) ArrayGet(&(dict->elements), index);

    if(ElementIsOccupied(newElement))
    {
        if(memcmp(key, ElementKey(newElement), dict->keySize) == 0) // Key is already present in the dictionary.
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
                dict->num++;
                return ElementValue(newElement, dict->keySize);
            }
        }
    }
    else
    {
        ElementSet(newElement, NULL, true, key, value, dict->keySize, dict->valueSize);
        dict->num++;
        return ElementValue(newElement, dict->keySize);
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
    LogAssert(dict != NULL);
    LogAssert(key != NULL);

    uint64_t hash = HashFNV1a64(key, dict->keySize);
    uint64_t index = hash % ArrayCapacity(&(dict->elements));

    Element* elementToRemove = (Element*) ArrayGet(&(dict->elements), index);

    if(ElementIsOccupied(elementToRemove))
    {
        if(memcmp(key, ElementKey(elementToRemove), dict->keySize) == 0)
        {
            Element* nextElement = ElementNextElement(elementToRemove);
            if(nextElement != NULL)
            {
                memcpy(elementToRemove, nextElement, ElementSize(dict));
                memset(nextElement, 0, ElementSize(dict));
            }
            else
            {
                memset(elementToRemove, 0, ElementSize(dict));
            }

            dict->num--;
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
void* DictionaryGet(const Dictionary* dict, const void* key)
{
    LogAssert(dict != NULL);
    LogAssert(key != NULL);

    uint64_t hash = HashFNV1a64(key, dict->keySize);
    uint64_t index = hash % ArrayCapacity(&(dict->elements));

    Element* elementLocation = (Element*) ArrayGet(&(dict->elements), index);

    if(ElementIsOccupied(elementLocation))
    {
        if(memcmp(key, ElementKey(elementLocation), dict->keySize) == 0)
        {
            return ElementValue(elementLocation, dict->keySize);
        }
        else
        {
            elementLocation = GetCollidingElement(dict, elementLocation, key);
            if(elementLocation == NULL)
            {
                return NULL;
            }
            else
            {
                return ElementValue(elementLocation, dict->keySize);
            }
        }
    }

    // The requested key is not present in the dictionary.
    return NULL;
}

void DictionaryResize(Dictionary* dict, const uint64_t newCapacity)
{
    LogAssert(dict != NULL);

    if(newCapacity == dict->elements.capacity)
    {
        return;
    }

    char prevElements[ElementSize(dict) * DictionaryNum(dict)];

    void* prevElement = &prevElements;

    for(int i = 0; i < ArrayCapacity(&(dict->elements)); i++)
    {
        Element* e = (Element*) ArrayGet(&(dict->elements), i);

        if(e != NULL && ElementIsOccupied(e))
        {
            memcpy(prevElement, e, ElementSize(dict));
            prevElement += ElementSize(dict);
        }
    }

    for(int i = 0; i < BucketArrayNum(&(dict->collisionElements)); i++)
    {
        Element* e = (Element*) BucketArrayGet(&(dict->collisionElements), i);

        if(e != NULL && ElementIsOccupied(e))
        {
            memcpy(prevElement, e, ElementSize(dict));
            prevElement += ElementSize(dict);
        }
    }

    ArrayClear(&(dict->elements));
    ArrayResize(&(dict->elements), newCapacity);
    Element emptyElement;
    memset(&emptyElement, 0, ElementSize(dict));
    ArrayFill(&(dict->elements), &emptyElement);

    BucketArrayClear(&(dict->collisionElements));
    BucketArrayResize(&(dict->collisionElements), ceil(newCapacity * (1.0f - MAX_LOAD_FACTOR)));

    uint64_t prevDictNum = dict->num;
    dict->num = 0;

    for(int i = 0; i < prevDictNum; i++)
    {
        Element* e = (Element*) (prevElements + (ElementSize(dict) * i));
        DictionaryAdd(dict, ElementKey(e), ElementValue(e, dict->keySize));
    }

    dict->num = prevDictNum;
}

/**
 * @brief Free the dictionary.
 * @param dict The dictionary to free.
 */
void DictionaryFree(Dictionary* dict)
{
    LogAssert(dict != NULL);

    DictionaryDeinit(dict);
    free(dict);
}

uint64_t DictionaryNum(const Dictionary* dict)
{
    LogAssert(dict != NULL);
    return dict->num;
}

/* ---------------------------------------------------- INTERNALS --------------------------------------------------- */

/**
 * @brief Initialize an existing dictionary. Only used internally. When calling DictionaryNew, the dictionary will already be initialized.
 * @param dict The dictionary to be initalized.
 * @param keySize The memory footprint of the key.
 * @param valueSize The memory footprint of the value.
 */
void DictionaryInit(Dictionary* dict, size_t keySize, size_t valueSize)
{
    LogAssert(dict != NULL);
    LogAssert(keySize > 0);
    LogAssert(valueSize > 0);

    dict->keySize = keySize;
    dict->valueSize = valueSize;
    dict->num = 0;

    ArrayInit(&(dict->elements), ElementSize(dict), (uint64_t) (INITIAL_CAPACITY));
    Element* emptyElement = malloc(ElementSize(dict));
    memset(emptyElement, 0, ElementSize(dict));
    ArrayFill(&(dict->elements), emptyElement);

    BucketArrayInit(&(dict->collisionElements), ElementSize(dict), ceil(INITIAL_CAPACITY * (1.0f - MAX_LOAD_FACTOR)));
}

/**
 * @brief Deinitialize the dictionary. This does not free the dictionary pointer. Use this function instead of free if the dictionary is stack allocated or allocated locally as a struct member.
 * @param dict The dictionary to deinitialize.
 */
void DictionaryDeinit(Dictionary* dict)
{
    LogAssert(dict != NULL);

    ArrayDeinit(&(dict->elements));
    BucketArrayDeinit(&(dict->collisionElements));
}

/* ----------------------------------------------------- STATICS ---------------------------------------------------- */

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
    LogAssert(dict != NULL);
    LogAssert(prevElement != NULL);
    LogAssert(key != NULL);
    LogAssert(value != NULL);

    Element* nextElement = ElementNextElement(prevElement);
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
        void* presentKey = ElementKey(nextElement);
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
    LogAssert(dict != NULL);
    LogAssert(prevElement != NULL);
    LogAssert(key != NULL);

    Element* elementToRemove = ElementNextElement(prevElement);

    if(elementToRemove == NULL)
    {
        // The requested key is not present in the dictionary.
        return;
    }

    if(ElementIsOccupied(elementToRemove))
    {
        if(memcmp(key, elementToRemove, dict->keySize) == 0)
        {
            Element* nextElement = ElementNextElement(elementToRemove);

            prevElement->nextElement = nextElement;
            memset(elementToRemove, 0, ElementSize(dict));

            dict->num--;
        }
        else
        {
            RemoveCollidingElement(dict, elementToRemove, key);
        }
    }
    else
    {
        LogError("Element has pointer to next element, but the next element is not occupied.");
    }
}

/**
 * @brief Retrieve an element, whose hash has collided with another element.
 * @param dict The dictionary to retrieve the element from.
 * @param prevElement The previous element to be retrieved, most likely the element which the newly added element collided with.
 * @param key The key of the element to be retrieved.
 * @return Element* A pointer to the value associated with the given key, if added successfully. NULL if the given key was already present in the dictionary.
 */
static Element* GetCollidingElement(const Dictionary* dict, Element* prevElement, const void* key)
{
    LogAssert(dict);
    LogAssert(prevElement);
    LogAssert(key);

    Element* nextElement = ElementNextElement(prevElement);

    if(nextElement == NULL)
    {
        // The requested key is not present in the dictionary.
        return NULL;
    }

    if(ElementIsOccupied(nextElement))
    {
        if(memcmp(key, ElementKey(nextElement), dict->keySize) == 0)
        {
            return nextElement;
        }
    }
    else
    {
        LogError("Element has pointer to next element, but the next element is not occupied.");
        return NULL;
    }

    return GetCollidingElement(dict, nextElement, key);
}

/**
 * @brief Get the memory footprint of 1 element in the dictionary.
 * @param dict The dictionary the element belongs to.
 * @return size_t The memory footprint of 1 element in the dictionary.
 */
static size_t ElementSize(const Dictionary* dict)
{
    LogAssert(dict != NULL);
    return sizeof(Element*) + sizeof(bool) + dict->keySize + dict->valueSize; // DictElement: next_element (in case of collision), bool (wether or not the element is used), key, value
}

/**
 * @brief Get a pointer to the next element, linked to a given element.
 * @param element The element to get the next element from.
 * @return Element* The pointer to the next element. NULL if there is no next element linked to the given element.
 */
static Element* ElementNextElement(const Element* element)
{
    LogAssert(element != NULL);
    return element->nextElement;
}
 /**
  * @brief Get wether or not there is data stored in this element.
  * @param element The element to be checked if there is data present.
  * @return Wether or not there is data stored in this element.
  */
static bool ElementIsOccupied(const Element* element)
{
    LogAssert(element != NULL);
    return element->isOccupied;
}

/**
 * @brief Get the key, stored in the given element.
 * @param element The element to get the key from.
 * @return void* A pointer to the key, stored in the given element.
 */
static void* ElementKey(const Element* element)
{
    LogAssert(element != NULL);
    return (void*) &(element->keyValuePair);
}

/**
 * @brief Get the value, stored in the given element.
 * @param element The element to get the value from.
 * @param keySize The memory footprint of the key, stored in this element. This is necessary, because the value is stored after the key,
 * @return void* A pointer to the key, stored in the given element.
 */
static void* ElementValue(const Element* element, const size_t keySize)
{
    LogAssert(element != NULL);
    LogAssert(keySize > 0);

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
    LogAssert(element != NULL);
    LogAssert(keySize > 0);
    LogAssert(valueSize > 0);

    element->nextElement = nextElement;
    element->isOccupied = isOccupied;
    memcpy(ElementKey(element), key, keySize);
    memcpy(ElementValue(element, keySize), value, valueSize);
}
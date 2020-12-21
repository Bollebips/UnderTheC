#ifndef DICTIONARY_I
#define DICTIONARY_I

#include "../../include/Containers/Dictionary.h"
#include "BucketArray.h"

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

#endif
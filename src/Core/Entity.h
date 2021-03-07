#ifndef ENTITY_I
#define ENTITY_I

#include <math.h>
#include <stdint.h>

// const uint8_t MAX_COMPONENT_TYPES = 64;
// const uint8_t MAX_SYSTEM_TYPES = 64;

// const int COMPONENT_BITFIELD_SIZE = (int) (MAX_COMPONENT_TYPES / 8) + ((MAX_COMPONENT_TYPES % 8 > 0) ? 1 : 0);
// const int SYSTEM_BITFIELD_SIZE = (int) (MAX_SYSTEM_TYPES / 8) + ((MAX_SYSTEM_TYPES % 8 > 0) ? 1 : 0);

// typedef uint64_t EntityID;
typedef uint64_t Entity;

uint64_t EntityGetID(const void* entity);

// struct Entity
// {
//     EntityID id;
//     // TODO: These 2 arrays should be a red black tree, so checking if an entity has a specific component is faster.
//     Array systems; // array of 64 bit hashes of the systems;
// };

#endif
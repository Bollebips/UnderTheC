#ifndef SPARSE_VOXEL_OCTREE_H
#define SPARSE_VOXEL_OCTREE_H

#include <GLFW/glfw3.h>

#include "Containers/Array.h"
#include <Defines.h>
#include <stddef.h>

#define VOXEL_CHUNK_SIZE 4

// octree element: [octree index of first child][child occupancy mask]
// has to be 32 bit to
typedef Array(u32) SparseVoxelOctree;

typedef struct
{
    SparseVoxelOctree Octree;
} VoxelChunk;

int CalculateOctreeSize(int denseDataSize);
void PopulateVoxelChunk(SparseVoxelOctree* result, u8 denseData[]);

#endif // SPARSE_VOXEL_OCTREE_H

#ifndef SPARSE_VOXEL_OCTREE_H
#define SPARSE_VOXEL_OCTREE_H

#include "Containers/Array.h"
#include <Defines.h>
#include <stddef.h>

#define VOXEL_CHUNK_SIZE 4

typedef Array(u32) SparseVoxelOctree;

typedef struct
{
    SparseVoxelOctree Octree;
} VoxelChunk;

int CalculateOctreeSize(int denseDataSize);
void PopulateVoxelChunk(SparseVoxelOctree* result, u32 denseData[]);

#endif // SPARSE_VOXEL_OCTREE_H

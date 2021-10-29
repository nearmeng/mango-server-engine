#pragma once
#include <stddef.h>

#define ONLY_MSPACES 1

#undef HAVE_MMAP
#define HAVE_MMAP 0

#undef HAVE_MORECORE
#define HAVE_MORECORE 0

#undef USE_LOCKS
#define USE_LOCKS 0

#undef USE_SPIN_LOCKS
#define USE_SPIN_LOCKS 0

#undef FOOTERS
#define FOOTERS 0

#undef INSECURE
#define INSECURE 0

#undef DEBUG
#define DEBUG 1

#undef ABORT_ON_ASSERT_FAILURE
#define ABORT_ON_ASSERT_FAILURE 0

/*******************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void* mspace;

size_t destroy_mspace(mspace msp);

mspace create_mspace_with_base(void* base, size_t capacity, int locked);

int mspace_track_large_chunks(mspace msp, int enable);

void* mspace_malloc(mspace msp, size_t bytes);

void mspace_free(mspace msp, void* mem);

void* mspace_realloc(mspace msp, void* mem, size_t newsize);

void* mspace_calloc(mspace msp, size_t n_elements, size_t elem_size);

void* mspace_memalign(mspace msp, size_t alignment, size_t bytes);

size_t mspace_footprint(mspace msp);

size_t mspace_max_footprint(mspace msp);

size_t mspace_usable_size(const void* mem);

int mspace_mallopt(int, int);

/*mspace_malloc_impl.h*/

void mspace_get_stats(mspace msp, size_t* out_maxfp, size_t* out_fp, size_t* out_used);

mspace base_to_mspace(void* base);

#ifdef __cplusplus
} /* end of extern "C" */
#endif /* __cplusplus */

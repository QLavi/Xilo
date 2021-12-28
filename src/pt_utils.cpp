#include <stdio.h>
#include <stdlib.h>

#define MEM_ALLOC(t, s) (t*)malloc(sizeof(t) * s)
#define MEM_FREE(x) free(x)

template <typename T>
struct Array
{
    void allocate(int s) { size = s; ix_ = 0; data = MEM_ALLOC(T, size); }
    void deallocate() { MEM_FREE(data); size = ix_ = 0; }

    T& operator [] (int idx) { 
#ifdef CHECK_BOUNDS
        if(idx >= size) { fprintf(stderr, "Index: %i, Larger then Array_Size: %i", idx, size); }
        return data[idx];
#endif
    }
    void push_back(T value) {
#ifdef CHECK_BOUNDS
        if(ix_ >= size) { fprintf(stderr, "Index out of range: %zu", ix_); }
#endif
        data[ix_++] = value;
    }

    int size;
    int ix_;
    T* data;
};

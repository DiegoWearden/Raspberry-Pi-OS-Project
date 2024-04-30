#include "stdint.h"  // Use cstdint for fixed-width integers
#include "heap.h"
#include "printf.h"

namespace gheith {

static int64_t *array;  // Use int64_t for array elements to ensure it works for 64-bit
static int len;
static int safe = 0;
static int avail = 0;
// static SpinLock *theLock = nullptr;

int64_t abs(int64_t x) {
    if (x < 0) return -x; else return x;
}

int64_t size(int i) {
    return abs(array[i]);
}

int headerFromFooter(int i) {
    return i - size(i) + 1;
}

int footerFromHeader(int i) {
    return i + size(i) - 1;
}

int sanity(int i) {
    if (safe) {
        if (i == 0) return 0;
        if ((i < 0) || (i >= len)) {
            panic("bad header index %d\n", i);
            return i;
        }
        int footer = footerFromHeader(i);
        if ((footer < 0) || (footer >= len)) {
            panic("bad footer index %d\n", footer);
            return i;
        }
        int64_t hv = array[i];
        int64_t fv = array[footer];
        if (hv != fv) {
            panic("bad block at %d, hv:%d fv:%d\n", i, hv, fv);
            return i;
        }
    }
    return i;
}

int left(int i) {
    return sanity(headerFromFooter(i - 1));
}

int right(int i) {
    return sanity(i + size(i));
}

int next(int i) {
    return sanity(array[i + 1]);
}

int prev(int i) {
    return sanity(array[i + 2]);
}

void setNext(int i, int x) {
    array[i + 1] = x;
}

void setPrev(int i, int x) {
    array[i + 2] = x;
}

void remove(int i) {
    int prevIndex = prev(i);
    int nextIndex = next(i);
    if (prevIndex == 0) {
        avail = nextIndex;
    } else {
        setNext(prevIndex, nextIndex);
    }
    if (nextIndex != 0) {
        setPrev(nextIndex, prevIndex);
    }
}

void makeAvail(int i, int64_t ints) {
    array[i] = ints;
    array[footerFromHeader(i)] = ints;
    setNext(i, avail);
    setPrev(i, 0);
    if (avail != 0) {
        setPrev(avail, i);
    }
    avail = i;
}

void makeTaken(int i, int64_t ints) {
    array[i] = -ints;
    array[footerFromHeader(i)] = -ints;
}

int isAvail(int i) {
    return array[i] > 0;
}

int isTaken(int i) {
    return array[i] < 0;
}
};

void heapInit(void* base, size_t bytes) {
    using namespace gheith;
    printf("| heap range 0x%lx 0x%lx\n", (uintptr_t)base, (uintptr_t)base + bytes);
    array = (int64_t*)base;
    len = bytes / sizeof(int64_t);
    makeTaken(0, 2);
    makeAvail(2, len - 4);
    makeTaken(len - 2, 2);
    // theLock = new SpinLock();
}

void* malloc(size_t bytes) {
    using namespace gheith;
    if (bytes == 0) return (void*)array;
    int64_t ints = ((bytes + sizeof(int64_t) - 1) / sizeof(int64_t)) + 2;
    if (ints < 4) ints = 4;
    // LockGuardP g{theLock};
    void* res = nullptr;
    int64_t mx = 0x7FFFFFFFFFFFFFFF;
    int it = 0;
    int countDown = 20;
    int p = avail;
    while (p != 0 && countDown > 0) {
        if (!isAvail(p)) {
            panic("block is not available in malloc %p\n", p);
        }
        int64_t sz = size(p);
        if (sz >= ints) {
            if (sz < mx) {
                mx = sz;
                it = p;
            }
            countDown--;
        }
        p = next(p);
    }
    if (it != 0) {
        remove(it);
        int64_t extra = mx - ints;
        if (extra >= 4) {
            makeTaken(it, ints);
            makeAvail(it + ints, extra);
        } else {
            makeTaken(it, mx);
        }
        res = &array[it + 1];
    }
    return res;
}

void free(void* p) {
    using namespace gheith;
    if (p == nullptr) return;
    if (p == (void*)array) return;
    // LockGuardP g{theLock};
    int idx = ((((uintptr_t)p) - ((uintptr_t)array)) / sizeof(int64_t)) - 1;
    sanity(idx);
    if (!isTaken(idx)) {
        panic("freeing free block, p:%lx idx:%d\n", (uintptr_t)p, idx);
        return;
    }
    int64_t sz = size(idx);
    int leftIndex = left(idx);
    int rightIndex = right(idx);
    if (isAvail(leftIndex)) {
        remove(leftIndex);
        idx = leftIndex;
        sz += size(leftIndex);
    }
    if (isAvail(rightIndex)) {
        remove(rightIndex);
        sz += size(rightIndex);
    }
    makeAvail(idx, sz);
}

/*****************/
/* C++ operators */
/*****************/

void* operator new(size_t size) {
    void* p = malloc(size);
    if (p == nullptr) panic("out of memory");
    return p;
}

void operator delete(void* p) noexcept {
    free(p);
}

void operator delete(void* p, size_t sz) {
    free(p);
}

void* operator new[](size_t size) {
    void* p = malloc(size);
    if (p == nullptr) panic("out of memory");
    return p;
}

void operator delete[](void* p) noexcept {
    free(p);
}

void operator delete[](void* p, size_t sz) {
    free(p);
}

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "lt.h"
#include "lt/lt_slot.h"
#include "system/error.h"

#define INITIAL_FRAME_BUFFER_SIZE 16

struct Lt
{
    Lt_slot **frames;
    size_t capacity;
    size_t size;
};

Lt *create_lt()
{
    Lt *lt = malloc(sizeof(Lt));
    if(lt == NULL) {
        throw_error(ERROR_TYPE_LIBC);
        goto malloc_lt_fail;
    }

    lt->frames = malloc(sizeof(Lt_slot*) * INITIAL_FRAME_BUFFER_SIZE);
    if (lt->frames == NULL) {
        throw_error(ERROR_TYPE_LIBC);
        goto malloc_lt_slots_fail;
    }

    lt->capacity = INITIAL_FRAME_BUFFER_SIZE;
    lt->size = 0;

    return lt;

malloc_lt_slots_fail:
    free(lt);
malloc_lt_fail:
    return NULL;
}

void destroy_lt(Lt *lt)
{
    assert(lt);

    while (lt->size-- > 0) {
        if (lt->frames[lt->size]) {
            destroy_lt_slot(lt->frames[lt->size]);
        }
    }

    free(lt->frames);
    free(lt);
}

void *lt_push(Lt *lt, void *resource, Lt_destroy resource_destroy)
{
    assert(lt);
    assert(resource_destroy);
    assert(lt != resource);

    if (resource == NULL) {
        return NULL;
    }

    if (lt->size >= lt->capacity) {
        lt->capacity *= 2;
        if ((lt->frames = realloc(lt->frames, sizeof(Lt_slot*) * lt->capacity)) == NULL) {
            throw_error(ERROR_TYPE_LIBC);
            return NULL;
        }
    }

    if ((lt->frames[lt->size++] = create_lt_slot(resource, resource_destroy)) == NULL) {
        return NULL;
    }

    return resource;
}

void* lt_reset(Lt *lt, void *old_resource, void *new_resource)
{
    assert(lt);
    assert(old_resource);
    assert(new_resource);
    assert(old_resource != new_resource);

    for (size_t i = 0; i < lt->size; ++i) {
        if (lt->frames[i] && lt_slot_contains_resource(lt->frames[i], old_resource)) {
            lt_slot_reset_resource(lt->frames[i], new_resource);
            return new_resource;
        }
    }

    return old_resource;
}

void *lt_release(Lt *lt, void *resource)
{
    assert(lt);
    assert(resource);

    for (size_t i = 0; i < lt->size; ++i) {
        if (lt->frames[i] && lt_slot_contains_resource(lt->frames[i], resource)) {
            release_lt_slot(lt->frames[i]);
            lt->frames[i] = NULL;
            return resource;
        }
    }

    return resource;
}

void *lt_replace(Lt *lt, void *old_resource, void *new_resource)
{
    assert(lt);
    assert(old_resource);
    assert(new_resource);

    for (size_t i = 0; i < lt->size; ++i) {
        if (lt->frames[i] && lt_slot_contains_resource(lt->frames[i], old_resource)) {
            lt_slot_replace_resource(lt->frames[i], new_resource);
            return new_resource;
        }
    }

    return old_resource;
}

#include "buffer_view.h"

void pop_front_buffer_view_init(pop_front_buffer_view* buffer, const uint8_t* data, size_t size) {
    buffer->data = data;
    buffer->size = size;
    buffer->offset = 0;
}

uint8_t pop_front_buffer_view_pop(pop_front_buffer_view* buffer) {
    return *(buffer->data + buffer->offset++);
}

const uint8_t* pop_front_buffer_view_pop_n(pop_front_buffer_view* buffer, size_t n) {
    const uint8_t* r = buffer->data + buffer->offset;
    buffer->offset += n;
    return r;
}

bool pop_front_buffer_view_empty(const pop_front_buffer_view* buffer) {
    return buffer->offset == buffer->size;
}

size_t pop_front_buffer_view_size(const pop_front_buffer_view* buffer) {
    return buffer->size - buffer->offset;
}

void push_back_buffer_view_init(push_back_buffer_view* buffer, uint8_t* data, size_t capacity) {
    buffer->data = data;
    buffer->capacity = capacity;
    buffer->size = 0;
}

void push_back_buffer_view_push(push_back_buffer_view* buffer, uint8_t b) {
    *(buffer->data + buffer->size++) = b;
}

uint8_t* push_back_buffer_view_reserve_n(push_back_buffer_view* buffer, size_t n) {
    uint8_t* r = buffer->data + buffer->size;
    buffer->size += n;
    return r;
}

bool push_back_buffer_view_full(const push_back_buffer_view* buffer) {
    return buffer->size == buffer->capacity;
}

size_t push_back_buffer_view_size(const push_back_buffer_view* buffer) {
    return buffer->size;
}

size_t push_back_buffer_view_capacity(const push_back_buffer_view* buffer) {
    return buffer->capacity;
}

size_t push_back_buffer_view_free_space(const push_back_buffer_view* buffer) {
    return buffer->capacity - buffer->size;
}

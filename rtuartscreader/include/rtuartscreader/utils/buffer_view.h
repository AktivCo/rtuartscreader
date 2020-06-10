#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


typedef struct {
    const uint8_t* data;
    size_t size;
    size_t offset;
} pop_front_buffer_view;

void pop_front_buffer_view_init(pop_front_buffer_view* buffer, const uint8_t* data, size_t length);

uint8_t pop_front_buffer_view_pop(pop_front_buffer_view* buffer);

const uint8_t* pop_front_buffer_view_pop_n(pop_front_buffer_view* buffer, size_t n);

bool pop_front_buffer_view_empty(const pop_front_buffer_view* buffer);

size_t pop_front_buffer_view_size(const pop_front_buffer_view* buffer);


typedef struct {
    uint8_t* data;
    size_t capacity;
    size_t size;
} push_back_buffer_view;

void push_back_buffer_view_init(push_back_buffer_view* buffer, uint8_t* data, size_t capacity);

void push_back_buffer_view_push(push_back_buffer_view* buffer, uint8_t b);

uint8_t* push_back_buffer_view_reserve_n(push_back_buffer_view* buffer, size_t n);

bool push_back_buffer_view_full(const push_back_buffer_view* buffer);

size_t push_back_buffer_view_size(const push_back_buffer_view* buffer);

size_t push_back_buffer_view_capacity(const push_back_buffer_view* buffer);

size_t push_back_buffer_view_free_space(const push_back_buffer_view* buffer);

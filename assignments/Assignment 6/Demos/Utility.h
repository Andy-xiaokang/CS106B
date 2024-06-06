#pragma once

/**
 * Macro: DISALLOW_COPYING_OF(Type)
 *
 * Disables copying / assignment of the specified type.
 */
#define DISALLOW_COPYING_OF(Type)                                           \
    Type(const Type &) = delete;                                            \
    Type(Type &&) = delete;                                                 \
    void operator= (Type) = delete

// Eclipse tries to optimize the <SOF> #ifndef #define ... #endif <EOF> pattern
// into a "#pragma once"-like attribute. This file appears to fit the pattern
// at a glance, but the macro actually gets undefined right before the #endif,
// allowing for multiple includes but not recursive includes. Hence Eclipse
// ignores subsequent includes, causing the indexer to complain about undefined
// variables that are, in fact, defined! Fortunately, this #define #undef
// pair at the beginning is enough to break the heuristic.
#define XENUM_H_ECLIPSE_PREPROCESSOR_BUSTER_
#undef XENUM_H_ECLIPSE_PREPROCESSOR_BUSTER_

// Don't allow nested includes. We'll be including the struct.*.h file that
// included this file, but when we hit that same include statement, we need to
// avoid infinitely recursing.
#ifndef XSTRUCT_H_NESTED_
#define XSTRUCT_H_NESTED_

// This usually won't be defined at this point, but just in case.
#ifdef X
#undef X
#endif

////////// Struct options.

// XGROUP: enable grouping of struct members.
// Usage:
//  #define XGROUP 1
//  ...
//  X(type, member) GROUP(1)
#if XGROUP
#define GROUP(g)
#endif

// XARRAY: enable array syntax in members.
// Usage:
//  #define XARRAY 1
//  ...
//  X(type, member) ARRAY([5]) // yields "type member[5];"
#if XARRAY
#define ARRAY(a)
#endif

#ifndef XSTRUCT_H_
#define XSTRUCT_H_
// Everything in this block should only show up once per compilation unit.

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Internal macros.
#define XSTRUCT_STR_EXPANDED(identifier) #identifier
#define XSTRUCT_STR(identifier) XSTRUCT_STR_EXPANDED(identifier)
#define XSTRUCT_GLUE_EXPANDED(prefix, suffix) prefix ## _ ## suffix
#define XSTRUCT_GLUE(prefix, suffix) XSTRUCT_GLUE_EXPANDED(prefix, suffix)
#define XSTRUCT_FILE(name) XENUM_STR(struct.name.h)

#define XSTRUCT_PRINT_LENGTH 32

const char *xstruct_format(char *typestr);

#ifdef XDATA_OWNER
// Everything in this block should only show up in a single compilation unit.

// Retrieves the printf format string associated with the given type string.
#define X(type, identifier)             \
    if (strcmp(#type, typestr) == 0) {  \
        return #identifier;             \
    }
const char *xstruct_format(char *typestr) {
    #include "xtypes.h"
    return "p";
}
#undef X

#endif // XDATA_OWNER

#endif // XSTRUCT_H_

// Everything below this point runs once per struct. Refer to struct.pixel.h
// for the examples in the comments, as well as enum.pixelgroup.h for those
// that deal with groups.

////////// Static data structure declarations.

// Create the struct for this macro.
// Example:
//  typedef struct pixel { int x; int y; color *color; float alpha; }
#ifdef XARRAY
#define X(type, identifier) type identifier
#undef ARRAY
#define ARRAY(a) a;
#else // XARRAY
#define X(type, identifier) type identifier;
#endif // XARRAY
typedef struct XNAME {
    #include XSTRUCT_FILE(XNAME)
} XNAME;
#ifdef XARRAY
#undef ARRAY
#define ARRAY(a)
#endif // XARRAY
#undef X

// Create the enum for this macro's members.
// enum { pixel_member_x, pixel_member_y, pixel_member_color, pixel_member_alpha, pixel_members };
#define X(type, identifier) XSTRUCT_GLUE(XSTRUCT_GLUE(XNAME, member), identifier),
typedef enum {
    #include XSTRUCT_FILE(XNAME)
    XSTRUCT_GLUE(XNAME, members)
} XSTRUCT_GLUE(XNAME, enum);
#undef X

// Holds the string name of each member in an array.
// Example:
//  char *pixel_strs[4];
char *XSTRUCT_GLUE(XNAME, strs)[XSTRUCT_GLUE(XNAME, members)];

// Holds the string name of each member's type in an array.
// Example:
//  char *pixel_type_strs[4];
char *XSTRUCT_GLUE(XNAME, type_strs)[XSTRUCT_GLUE(XNAME, members)];

#if XGROUP

// Holds the group of each member in an array.
// Example:
//  int pixel_groups[4];
int XENUM_GLUE(XNAME, groups)[XSTRUCT_GLUE(XNAME, members)];

#endif // XGROUP

////////// Function declarations. Refer to their definitions for documentation.

int XSTRUCT_GLUE(XNAME, index)(XNAME *structure, void *member);
void *XSTRUCT_GLUE(XNAME, member)(XNAME *structure, int index);
char *XSTRUCT_GLUE(XNAME, str)(XNAME *structure, void *member);
char *XSTRUCT_GLUE(XNAME, type_str)(XNAME *structure, void *member);
void XSTRUCT_GLUE(XNAME, iter)(XNAME *structure, int callback(void *));
char *XSTRUCT_GLUE(XNAME, print_member)(XNAME *structure, void *member, const char *format);
char *XSTRUCT_GLUE(XNAME, print)(XNAME *structure, const char *format, const char *sep);

#if XGROUP
int XSTRUCT_GLUE(XNAME, group)(XNAME *structure, void *member);
void XSTRUCT_GLUE(XNAME, group_iter)(XNAME *structure, int group, int callback(void *));
#endif

#ifdef XDATA_OWNER
// Everything in this block should only show up in a single compilation unit
// for each enum.

////////// Variable definitions.

// Create the string array of member names.
// Example:
//  char *pixel_members[] = { "x", "y", "color", "alpha" };
#define X(type, identifier) #identifier,
char *XSTRUCT_GLUE(XNAME, strs)[] = {
    #include XSTRUCT_FILE(XNAME)
};
#undef X

// Create the string array of types.
// char *pixel_members = { "int", "int", "color *", "float" };
#define X(type, identifier) #type,
char *XSTRUCT_GLUE(XNAME, type_strs)[] = {
    #include XSTRUCT_FILE(XNAME)
};
#undef X

#if XGROUP

// Create the group array.
// Example:
//  int pixel_groups[] = { pix_pos, pix_pos, pix_other, pix_other };
#define X(type, identifier)
#undef GROUP
#define GROUP(g) g,
int XSTRUCT_GLUE(XNAME, groups)[] = {
    #include XSTRUCT_FILE(XNAME)
};
#undef GROUP
#define GROUP(g)
#undef X

#endif // XGROUP

////////// Function definitions.

// Get the index of the member, or -1 for invalid input.
// Example:
//  void *pixel_index(pixel *structure, int index) { ... }
#define X(type, identifier)                 \
    if (member == &structure->identifier) { \
        return i;                           \
    }                                       \
    i++;
int XSTRUCT_GLUE(XNAME, index)(XNAME *structure, void *member) {
    int i = 0;
    #include XSTRUCT_FILE(XNAME)
    return -1;
}
#undef X

// Get a pointer to the index-th member of the struct, or NULL for invalid
// input.
// Example:
//  void *pixel_member(pixel *structure, int index) { ... }
#define X(type, identifier)             \
    if (i == index) {                   \
        return &structure->identifier;  \
    }                                   \
    i++;
void *XSTRUCT_GLUE(XNAME, member)(XNAME *structure, int index) {
    int i = 0;
    #include XSTRUCT_FILE(XNAME)
    return NULL;
}
#undef X

// Get the stringified name of a member, or NULL for invalid input.
// Example:
//  char *pixel_str(pixel *structure, void *member) { ... }
char *XSTRUCT_GLUE(XNAME, str)(XNAME *structure, void *member) {
    int i = XSTRUCT_GLUE(XNAME, index)(structure, member);
    if (i >= 0) {
        return XSTRUCT_GLUE(XNAME, strs)[i];
    }
    return NULL;
}

// Get the stringified type of a member, or NULL for invalid input.
// Example:
//  char *pixel_type_str(pixel *structure, void *member) { ... }
char *XSTRUCT_GLUE(XNAME, type_str)(XNAME *structure, void *member) {
    int i = XSTRUCT_GLUE(XNAME, index)(structure, member);
    if (i >= 0) {
        return XSTRUCT_GLUE(XNAME, type_strs)[i];
    }
    return NULL;
}

// Call the given function with a pointer to each member until it returns
// nonzero.
// Example:
//  void pixel_iter(pixel *structure, int group, int callback(void *)) { ... }
void XSTRUCT_GLUE(XNAME, iter)(XNAME *structure, int callback(void *)) {
    int i;
    for (i = 0; i < XSTRUCT_GLUE(XNAME, members); i++) {
        if (callback(XSTRUCT_GLUE(XNAME, member)(structure, i))) {
            return;
        }
    }
}

// Allocate and return a formatted string containing the member's name and
// value. The format string should contain a "%%s" for the name and a "%%%s"
// for the value, in that order. XData tries to guess how to print the value
// according to the type but falls back to the member's location in memory.
// Example:
//  char *pixel_print_member(pixel *structure, void *member, const char *format) { ... }
#define X(type, identifier)                                                     \
    if (strcmp(#type, member_type) XSTRUCT_CMP 0) {                             \
        int formatted_length = snprintf(formatted,                              \
                    formatter_length + XSTRUCT_PRINT_LENGTH, formatter,         \
                    member_name, XSTRUCT_DEREF(type*)member);                   \
        if (formatted_length >= formatter_length + XSTRUCT_PRINT_LENGTH) {      \
            formatted = realloc(formatted, formatted_length + 1);               \
            snprintf(formatted, formatted_length + 1, formatter, member_name,   \
                        XSTRUCT_DEREF(type*)member);                            \
        }                                                                       \
        return formatted;                                                       \
    }
char *XSTRUCT_GLUE(XNAME, print_member)(XNAME *structure, void *member, const char *format) {
    char *member_name = XSTRUCT_GLUE(XNAME, str)(structure, member);
    char *member_type = XSTRUCT_GLUE(XNAME, type_str)(structure, member);
    const char *member_format = xstruct_format(member_type);
    char *formatter = malloc(XSTRUCT_PRINT_LENGTH);
    int formatter_length = snprintf(formatter, XSTRUCT_PRINT_LENGTH, format, member_format);
    if (formatter_length >= XSTRUCT_PRINT_LENGTH) {
        formatter = realloc(formatter, formatter_length + 1);
        snprintf(formatter, formatter_length + 1, format, member_format);
    }
    char *formatted = malloc(formatter_length + XSTRUCT_PRINT_LENGTH);
    #define XSTRUCT_CMP ==
    #define XSTRUCT_DEREF *
    #include "xtypes.h"
    #undef XSTRUCT_CMP
    #undef XSTRUCT_DEREF
    #define XSTRUCT_CMP !=
    #define XSTRUCT_DEREF
    X(void, )
    #undef XSTRUCT_CMP
    #undef XSTRUCT_DEREF
    return NULL;
}
#undef X

// Allocate and return a formatted string containing each member's name and
// value, concatenated by the given separator.
// Example:
//  char *pixel_print(pixel *structure, const char *format, const char *sep) { ... }
char *XSTRUCT_GLUE(XNAME, print)(XNAME *structure, const char *format, const char *sep) {
    int i;
    int n_members = XSTRUCT_GLUE(XNAME, members);
    int formatted_size = XSTRUCT_PRINT_LENGTH * n_members;
    char *member;
    char *formatted = malloc(formatted_size);
    formatted[0] = '\0';
    for (i = 0; i < n_members; i++) {
        member = XSTRUCT_GLUE(XNAME, print_member)(structure,
                XSTRUCT_GLUE(XNAME, member)(structure, i), format);
        if (strlen(formatted) + strlen(member) > formatted_size) {
            formatted_size = strlen(formatted) + strlen(member) + strlen(sep) + 1;
            formatted = realloc(formatted, formatted_size);
        }
        strcat(formatted, member);
        if (i != n_members - 1) {
            strcat(formatted, sep);
        }
        free(member);
    }
    return formatted;
}

#if XGROUP

// Get the group of a member, or -1 for invalid input.
// Example:
//  int pixel_group(pixel *structure, void *member) { ... }
int XSTRUCT_GLUE(XNAME, group)(XNAME *structure, void *member) {
    int i = XSTRUCT_GLUE(XNAME, index)(structure, member);
    if (i >= 0) {
        return XSTRUCT_GLUE(XNAME, groups)[i];
    }
    return -1;
}

// Call the given function with a pointer to each member in the group until it
// returns nonzero.
// Example:
//  void pixel_group_iter(pixel *structure, int group, int callback(void *)) { ... }
void XSTRUCT_GLUE(XNAME, group_iter)(XNAME *structure, int group, int callback(void *)) {
    int i;
    for (i = 0; i < XSTRUCT_GLUE(XNAME, members); i++) {
        if (XSTRUCT_GLUE(XNAME, groups)[i] == group) {
            if (callback(XSTRUCT_GLUE(XNAME, member)(structure, i))) {
                return;
            }
        }
    }
}

#endif // XGROUP

#endif // XDATA_OWNER

////////// Cleanup.

#define X(type, identifier)
#undef XNAME

#if XGROUP
#undef XGROUP
#undef GROUP
#define GROUP(g)
#endif // XGROUP

#if XARRAY
#undef XARRAY
#undef ARRAY
#define ARRAY(a)
#endif // XARRAY

#undef XSTRUCT_H_NESTED_
#endif // XSTRUCT_H_NESTED_

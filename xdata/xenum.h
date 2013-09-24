// Eclipse tries to optimize the <SOF> #ifndef #define ... #endif <EOF> pattern
// into a "#pragma once"-like attribute. This file appears to fit the pattern
// at a glance, but the macro actually gets undefined right before the #endif,
// allowing for multiple includes but not recursive includes. Hence Eclipse
// ignores subsequent includes, causing the indexer to complain about undefined
// variables that are, in fact, defined! Fortunately, this #define #undef
// pair at the beginning is enough to break the heuristic.
#define XENUM_H_ECLIPSE_PREPROCESSOR_BUSTER_
#undef XENUM_H_ECLIPSE_PREPROCESSOR_BUSTER_

// Don't allow nested includes. We'll be including the enum.*.h file that
// included this file, but when we hit that same include statement, we need to
// avoid infinitely recursing.
#ifndef XENUM_H_NESTED_
#define XENUM_H_NESTED_

// This usually won't be defined at this point, but just in case.
#ifdef X
#undef X
#endif

////////// Enum options.

// XGROUP: enable grouping of enum identifiers.
// Usage:
//  #define XGROUP
//     X(identifer) GROUP(1)
#if XGROUP
#define GROUP(g)
#endif // XGROUP

// XVALUE: enable assigning values to enum identifiers.
// Usage:
//  #define XVALUE
//  X(identifier1) VALUE(=10)
//  X(identifier2) VALUE()        // implicitly set to 11
#if XVALUE
#define VALUE(v)
#endif // XVALUE

// XPREFIX: add a prefix before each identifier.
// Usage:
//  #define XPREFIX foo_
//  X(identifier)                // becomes foo_identifier
#ifndef XPREFIX
#define XPREFIX
#endif
// I don't know why I need 2 expansions this time, but it's rather annoying.
#define XENUM_ID__(prefix, identifier) prefix ## identifier
#define XENUM_ID_(prefix, identifier) XENUM_ID__(prefix, identifier)
#define XENUM_ID(identifier) XENUM_ID_(XPREFIX, identifier)

#ifndef XENUM_H_
#define XENUM_H_
// Everything in this block should only show up once per compilation unit.

#include <ctype.h>
#include <string.h>

// Internal macros.
#define XENUM_STR_(identifier) #identifier
#define XENUM_STR(identifier) XENUM_STR_(identifier)
#define XENUM_GLUE_(prefix, suffix) prefix ## _ ## suffix
#define XENUM_GLUE(prefix, suffix) XENUM_GLUE_(prefix, suffix)
#define XENUM_FILE(name) XENUM_STR(enum.name.h)

#ifdef XDATA_OWNER
// Everything in this block should only show up in a single compilation unit.
// There are no internal functions or variables here yet.
#endif // XDATA_OWNER

#endif // XENUM_H_

// Everything below this point runs once per enum. Refer to enum.color.h for
// the examples in the comments, as well as enum.colorgroup.h for those that
// deal with groups.

////////// Static data structure declarations.

// Create the enum itself.
// Example:
//  typedef enum { Red, Green, Blue, White=10, Black, color_max } color;
#if XVALUE
#define X(identifier) XENUM_ID(identifier)
#undef VALUE
#define VALUE(v) v,
#else
#define X(identifier) XENUM_ID(identifier),
#endif // XVALUE
typedef enum {
    #include XENUM_FILE(XNAME)
    XENUM_GLUE(XNAME, max)
} XNAME;
#if XVALUE
#undef VALUE
#define VALUE(v)
#endif // XVALUE
#undef X

////////// Variable declarations.

// Contains the number of identifiers in the enum. This can differ from the
// "max" identifier at the end if any values are explicitly set.
// Example:
//  int color_count;
int XENUM_GLUE(XNAME, count);

#define X(identifier) 1+
// Holds the value of each identifier in an array.
// Example:
//  color color_values[5];
XNAME XENUM_GLUE(XNAME, values)[
    #include XENUM_FILE(XNAME)
    0];

// Holds the string name of each identifier in an array.
// Example:
//  char *color_strs[5];
char *XENUM_GLUE(XNAME, strs)[
    #include XENUM_FILE(XNAME)
    0];
#undef X

#if XGROUP

// Holds the group of each identifier in an array.
// Example:
//  int color_groups[5];
#define X(identifier) 1+
int XENUM_GLUE(XNAME, groups)[
    #include XENUM_FILE(XNAME)
    0];
#undef X

#endif // XGROUP

////////// Function declarations. Refer to their definitions for documentation.

unsigned int XENUM_GLUE(XNAME, index)(XNAME value);
char *XENUM_GLUE(XNAME, str)(XNAME value);
void XENUM_GLUE(XNAME, iter)(int callback(XNAME));

#if XGROUP
int XENUM_GLUE(XNAME, group)(XNAME value);
void XENUM_GLUE(XNAME, group_iter)(int group, int callback(XNAME));
#endif

#ifdef XDATA_OWNER
// Everything in this block should only show up in a single compilation unit
// for each enum.

////////// Variable definitions.

// Create the enum count.
// Example:
//  int color_count = 5;
#define X(identifier) 1+
int XENUM_GLUE(XNAME, count) =
        #include XENUM_FILE(XNAME)
        0;
#undef X

// Create the value array.
// Example:
//  color color_values[] = { Red, Green, Blue, White, Black };
#define X(identifier) XENUM_ID(identifier),
XNAME XENUM_GLUE(XNAME, values)[] = {
    #include XENUM_FILE(XNAME)
};
#undef X

// Create the string array.
// Example:
//  char *color_strs[] = { "Red", "Green", "Blue", "White", "Black" };
#define X(identifier) XENUM_STR(XENUM_ID(identifier)),
char *XENUM_GLUE(XNAME, strs)[] = {
    #include XENUM_FILE(XNAME)
};
#undef X

#if XGROUP

// Create the group array.
// Example:
//  int color_groups[] = { Color, Color, Color, Grayscale, Grayscale };
#define X(identifier)
#undef GROUP
#define GROUP(g) g,
int XENUM_GLUE(XNAME, groups)[] = {
    #include XENUM_FILE(XNAME)
};
#undef GROUP
#define GROUP(g)
#undef X

#endif // XGROUP

////////// Function definitions.

// Get the index of the given value. Returns -1 for invalid values.
// Example:
//  unsigned int color_index(color value) { ... }
unsigned int XENUM_GLUE(XNAME, index)(XNAME value) {
    int i;
    for (i = 0; i < XENUM_GLUE(XNAME, count); i++) {
        if (value == XENUM_GLUE(XNAME, values)[i]) {
            return i;
        }
    }
    return -1;
}

// Get the string name of the given value.
// Example:
//  char *color_str(color value) { ... }
char *XENUM_GLUE(XNAME, str)(XNAME value) {
    return XENUM_GLUE(XNAME, strs)[XENUM_GLUE(XNAME, index)(value)];
}

// Call the given function with each value until it returns nonzero.
// Example:
//  void color_iter(int callback(color)) { ... }
void XENUM_GLUE(XNAME, iter)(int callback(XNAME)) {
    int i;
    for (i = 0; i < XENUM_GLUE(XNAME, count); i++) {
        if (callback(XENUM_GLUE(XNAME, values)[i])) {
            return;
        }
    }
}

#if XGROUP
// Get the group of the given value.
// Example:
//  int color_group(color value) { ... }
int XENUM_GLUE(XNAME, group)(XNAME value) {
    return XENUM_GLUE(XNAME, groups)[XENUM_GLUE(XNAME, index)(value)];
}

// Call the function with each value in the group until it returns nonzero.
// Example:
//  void color_group_iter(int group, int callback(color)) { ... }
void XENUM_GLUE(XNAME, group_iter)(int group, int callback(XNAME)) {
    int i;
    for (i = 0; i < XENUM_GLUE(XNAME, count); i++) {
        if (XENUM_GLUE(XNAME, groups)[i] == group) {
            if (callback(XENUM_GLUE(XNAME, values)[i])) {
                return;
            }
        }
    }
}
#endif // XGROUP

#endif // XDATA_OWNER

////////// Cleanup.

#define X(identifier)
#undef XNAME

#if XGROUP
#undef XGROUP
#undef GROUP
#define GROUP(g)
#endif // XGROUP

#if XVALUE
#undef XVALUE
#undef VALUE
#define VALUE(v)
#endif // XVALUE

#undef XPREFIX
#undef XENUM_ID

#undef XENUM_H_NESTED_
#endif // XENUM_H_NESTED_

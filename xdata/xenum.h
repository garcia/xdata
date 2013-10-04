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
//  typedef enum { Red, Green, Blue, White=10, Black } color;
#define X(identifier, ...) XENUM_ID(identifier) __VA_ARGS__,
typedef enum XNAME {
    #include XENUM_FILE(XNAME)
} XNAME;
#undef X

// Create the index enum. This is used for determining the number of enum
// members; if any members have their values explicitly set, the value of the
// last member can not be relied upon for this purpose.
// Example:
//  enum color_indices { color_index_Red, color_index_Green, color_index_Blue,
//                       color_index_White, color_index_Black, color_count };
#define X(identifier, ...) XENUM_GLUE(XENUM_GLUE(XNAME, index), identifier),
enum XENUM_GLUE(XNAME, indices) {
	#include XENUM_FILE(XNAME)
	XENUM_GLUE(XNAME, count)
};
#undef X

////////// Variable declarations.

// Holds the value of each identifier in an array.
// Example:
//  color color_values[5];
XNAME XENUM_GLUE(XNAME, values)[XENUM_GLUE(XNAME, count)];

// Holds the string name of each identifier in an array.
// Example:
//  char *color_strs[5];
char *XENUM_GLUE(XNAME, strs)[XENUM_GLUE(XNAME, count)];

#if XGROUP

// Holds the group of each identifier in an array.
// Example:
//  int color_groups[5];
int XENUM_GLUE(XNAME, groups)[XENUM_GLUE(XNAME, count)];

#endif // XGROUP

////////// Function declarations. Refer to their definitions for documentation.

unsigned int XENUM_GLUE(XNAME, index)(XNAME value);
char *XENUM_GLUE(XNAME, str)(XNAME value);
int XENUM_GLUE(XNAME, iter)(int callback(XNAME, void *), void *data);

#if XGROUP
int XENUM_GLUE(XNAME, group)(XNAME value);
int XENUM_GLUE(XNAME, group_iter)(int group, int callback(XNAME, void *), void *data);
#endif

#ifdef XDATA_OWNER
// Everything in this block should only show up in a single compilation unit
// for each enum.

////////// Variable definitions.

// Create the value array.
// Example:
//  color color_values[] = { Red, Green, Blue, White, Black };
#define X(identifier, ...) XENUM_ID(identifier),
XNAME XENUM_GLUE(XNAME, values)[] = {
    #include XENUM_FILE(XNAME)
};
#undef X

// Create the string array.
// Example:
//  char *color_strs[] = { "Red", "Green", "Blue", "White", "Black" };
#define X(identifier, ...) XENUM_STR(XENUM_ID(identifier)),
char *XENUM_GLUE(XNAME, strs)[] = {
    #include XENUM_FILE(XNAME)
};
#undef X

#if XGROUP

// Create the group array.
// Example:
//  int color_groups[] = { Color, Color, Color, Grayscale, Grayscale };
#define X(identifier, ...)
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

// Get the index of the given value, or -1 for invalid input.
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

// Get the string name of the given value, or NULL for invalid input.
// Example:
//  char *color_str(color value) { ... }
char *XENUM_GLUE(XNAME, str)(XNAME value) {
    int i = XENUM_GLUE(XNAME, index)(value);
    if (i >= 0) {
        return XENUM_GLUE(XNAME, strs)[XENUM_GLUE(XNAME, index)(value)];
    }
    return NULL;
}

// Call the given function with each value until it returns nonzero. If the
//  callback returns nonzero, this function returns the same value. Otherwise
//  it returns zero after the callback has been executed for each member.
// Example:
//  int color_iter(int callback(color)) { ... }
int XENUM_GLUE(XNAME, iter)(int callback(XNAME, void *), void *data) {
    int i;
    int rtn;
    for (i = 0; i < XENUM_GLUE(XNAME, count); i++) {
        if ((rtn = callback(XENUM_GLUE(XNAME, values)[i], data))) {
            return rtn;
        }
    }
    return 0;
}

#if XGROUP
// Get the group of the given value, or -1 for invalid input.
// Example:
//  int color_group(color value) { ... }
int XENUM_GLUE(XNAME, group)(XNAME value) {
    int i = XENUM_GLUE(XNAME, index)(value);
    if (i >= 0) {
        return XENUM_GLUE(XNAME, groups)[XENUM_GLUE(XNAME, index)(value)];
    }
    return -1;
}

// Call the function with each value in the group until it returns nonzero. If
//  the callback returns nonzero, this function returns the same value.
//  Otherwise it returns zero after the callback has been executed for each
//  member.
// Example:
//  int color_group_iter(int group, int callback(color)) { ... }
int XENUM_GLUE(XNAME, group_iter)(int group, int callback(XNAME, void *), void *data) {
    int i;
    int rtn;
    for (i = 0; i < XENUM_GLUE(XNAME, count); i++) {
        if (XENUM_GLUE(XNAME, groups)[i] == group) {
            if ((rtn = callback(XENUM_GLUE(XNAME, values)[i], data))) {
                return rtn;
            }
        }
    }
    return 0;
}
#endif // XGROUP

#endif // XDATA_OWNER

////////// Cleanup.

#define X(identifier, ...)
#undef XNAME

#if XGROUP
#undef XGROUP
#undef GROUP
#define GROUP(g)
#endif // XGROUP

#undef XPREFIX
#undef XENUM_ID

#undef XENUM_H_NESTED_
#endif // XENUM_H_NESTED_

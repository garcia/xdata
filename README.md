XData
=====


**XData** is a collection of preprocessor hacks that provide extra methods for
navigating and manipulating data structures, all while adhering to DRY
principles to the extent that the C preprocessor will permit.


Rationale
---------


In C, certain metadata such as enum names and struct member names are not
preserved. Iterating over an enum's values cannot be done in a non-redundant
manner if any members have their values explicitly set. Iterating over a
struct's members simply cannot be done without completely repeating the
structure in a different format. In such scenarios, it can be tempting to
resort to redundancy or string typing, but neither of these are desirable
choices. X Macros were devised to work around these issues, but they lack
standardization and often suffer from the same redundancy issues when multiple
data structures are involved. XData solves these problems by defining
X Macro-like preprocessor directives that can be reused for an indefinite
number of data structures. XData also features optional fields that bring extra
functionality to data structures when they are desired.


Usage
-----


Each data structure is defined in its own file, in the same directory as XData.
Datafiles should be named `{datatype}.{name}.h`, placed in the same directory
as xdata, and are generally of the following form (curly braces denote
mandatory components, square braces denote optional components):

    #define XNAME {name}
    [options]
    #include "x{datatype}.h"
    X({mandatory-fields}) [optional-fields]
    X({mandatory-fields}) [optional-fields]
    [...]

* `{datatype}`: either `enum` or `struct`.
* `{name}`: an arbitrary name for the data structure. Must be a valid C
	identifier, preferably without underscores.
* `[options]`: a line-separated list of macro definitions that enable options.
* `{mandatory-fields}`: a comma-separated list of fields; datatype specific.
* `[optional-fields]`: a series of option macros of the form `OPTION(value)`.

When using options that define macros to be placed in the `[optional-fields]`
component, the macro must be present after *every* `X(...)` in that datafile.
Empty values may be permitted by passing no arguments to the macro, but this
depends on the option in question.

Datafiles can be included directly, but care must be taken to prevent multiple
inclusion (the datafile cannot accomplish this on its own). For this reason, it
is generally easier to include the datafile in `xdata/xdata.h`, then include
that file from elsewhere in the project.

There must be one compilation unit in which `XDATA_OWNER` is defined before
each datafile is processed. This can be done by compiling `xdata/xdata.c` into
the project.


### Enums


#### Mandatory fields

* `X({identifier})`
    -  `{identifier}`: a C identifier to be placed into the enum.

#### Optional fields

* `XVALUE`: Enables specified values for enum members.
	- `[options]` component: `#define XVALUE 1`
	- `[optional-fields]` component: `VALUE([={value}])`, where `{value}` is an
        integer and is prefixed by an equal sign. For example, `VALUE()` and
        `VALUE(=5)` are both valid, but not `VALUE(5)`.
* `XGROUP`: Enables grouping of enum members.
	- `[options]` component: `#define XGROUP 1`
	- `[optional-fields]` component: `GROUP({group})`, where `{group}` is an
        integer.
* `XPREFIX`: Prefixes each enum member with a partial identifier.
	- `[options]` component: `#define XPREFIX prefix`
	- `[optional-fields]` component: none.

#### Variables

* `typedef enum {name} {
		[prefix]{identifier} [value], [
		[prefix]{identifier} [value], [ ... ] ]
		} {name}`
	- Defines the enum itself. If `XPREFIX` is not enabled, `[prefix]` has no
		effect. Likewise for `XVALUE` / `[value]`.
* `enum {name}_indices {
		{name}_index_{identifier}, [
		{name}_index_{identifier}, [ ... ] ]
		{name}_count }`
	- Used to calculate the number of identifiers defined by the enum. Note
		the absence of `[value]` in this enum.
* `int {name}_values[{name}_count]`
	- Holds the value of each identifier in an array.
* `char *{name}_strs[{name}_count]`
	- Holds the string name of each identifier in an array.
* `int {name}_groups[{name}_count]`
	- Holds the group of each identifier in an array. Only available if
		`XGROUP` is enabled.

#### Methods

* `unsigned int {name}_index(int value)`
	- Get the index of the given value, or -1 for invalid input. This is mostly
		useful for enums that have specified values (i.e. those with the
        `XVALUE` option enabled).
* `char *{name}_str(int value)`
	- Get the string name of the given value, or NULL for invalid input.
* `void {name}_iter(int callback(int, void *), void *data)`
	- Call the given function with each value and a user-specified pointer
		until it returns nonzero.
* `int {name}_group(int value)`
	- Get the group of the given value, or -1 for invalid input. Only available
        if `XGROUP` is enabled.
* `void {name}_group_iter(int group, int callback(int))`
	- Call the given function with each value in the group and a user-specified
		pointer until it returns nonzero.


### Structs


#### Mandatory fields

* `X({type}, {member})`
    - `type`: a C type specifier to be applied to this member.
    - `member`: a C identifier to be used as the member's name.

#### Optional fields

* `XGROUP`: Enables grouping of struct members.
	- `[options]` component: `#define XGROUP 1`
	- `[optional-fields]` component: `GROUP({group})`, where `{group}` is an
        integer.
* `XSUFFIX`: Enables suffixes for struct members. This can be used to create
	arrays and bitfields.
	- `[options]` component: `#define XARRAY 1`
	- `[optional-fields]` component: `ARRAY([suffix])`, where `[suffix]` is
		any syntactically valid C code when placed after a struct member
		declaration. `SUFFIX([5])` can be used to make an array of size 5;
		`SUFFIX(:1)` can be used to make a bitfield of size 1. `SUFFIX()` with
		no arguments must be used for other members in the same struct that do
		not require a suffix.

#### Variables

* `typedef struct {name} {
		{type} {member} [suffix]; [
		{type} {member} [suffix]; [ ... ] ]
		} {name}`
	- Defines the struct itself. If `XSUFFIX` is not enabled, `[suffix]` has no
		effect.
* `enum {name}_enum {
		{name}_member_{member}, [
		{name}_member_{member}, [ ... ] ]
	    {name}_members }`
	- Defines an enum corresponding to the members of the struct. Used
		primarily to calculate the number of members.
* `char *{name}_strs[{name}_members]`
	- Holds the string name of each member in an array.
* `char *{name}_type_strs[{name}_members]`
	- Holds the string name of each member's type in an array.
* `int {name}_groups[{name}_members]`
	- Holds the group of each member in an array. Only available if `XGROUP` is
		enabled.

#### Methods

* `int {name}_index({name} *structure, void *member)`
	- Get the index of the member, or -1 for invalid input.
		The first argument should be an instance of the struct; the second
		argument should be a member of that instance. Both are required to
		determine what member the pointer refers to.
* `void *{name}_member({name} *structure, int index)`
	- Get a pointer to the index-th member of the struct, or NULL for invalid
        input.
* `char *{name}_str({name} *structure, void *member)`
	- Get the stringified name of a member, or NULL for invalid input.
* `char *{name}_type_str({name} *structure, void *member)`
	- Get the stringified type of a member, or NULL for invalid input.
* `void {name}_iter({name} *structure, int callback(void *, void *),
		void *data)`
	- Call the given function with a pointer to each member and a
		user-specified pointer until it returns nonzero.
* `char *{name}_print_member({name} *structure, void *member,
		const char *format)`
	- Allocate and return a formatted string containing the member's name and
		value. The format string should contain a `%%s` for the name and a
		`"%%%s` for the value, in that order. XData tries to guess how to print
		the value according to the type but falls back to the member's location
		in memory.
* `char *{name}_print({name} *structure, const char *format, const char *sep)`
	- Allocate and return a formatted string containing each member's name and
		value, concatenated by the given separator.
* `int {name}_group({name} *structure, void *member)`
	- Get the group of a member, or -1 for invalid input. Only available if
		`XGROUP` is enabled.
* `void {name}_group_iter({name} *structure, int group,
		int callback(void *, void *), void *data)`
	- Call the given function with a pointer to each member in the group and a
		user-specified pointer until it returns nonzero.


Examples
--------


See [example.c](example.c) and the files referenced in
[xdata.h](xdata/xdata.h) for examples of XData in action.

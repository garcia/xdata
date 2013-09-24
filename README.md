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
as xdata, and are generally of the following form:

    #define XNAME {name}
    [options]
    #include "x{datatype}.h"
    X({mandatory-fields}) [optional-fields]
    X({mandatory-fields}) [optional-fields]
    [...]

* `{datatype}`: either `enum` or `struct`.
* `{name}`: an arbitrary name for the data structure. Must be a valid C
	identifier, preferably without underscores.


### Enums


The `enum` datatype can be used to create enums. There is only one mandatory
field: `identifier`, a C identifier to be placed into the enum.

#### Optional fields

* `XVALUE`: Enables specified values for enum members.
	- `[options]` component: `#define XVALUE 1`
	- `[optional-fields]` component: `VALUE([=value])`
* `XGROUP`: Enables grouping of enum members.
	- `[options]` component: `#define XGROUP 1`
	- `[optional-fields]` component: `GROUP(group)`
* `XPREFIX`: Prefixes each enum member with a partial identifier.
	- `[options]` component: `#define XPREFIX prefix`
	- `[optional-fields]` component: none.

#### Variables

* `typedef enum { [prefix]*, [ [prefix]*, [ ... ] ] {name}_max } {name}`
	- Defines the enum itself. If `XPREFIX` is not enabled, `[prefix]` has no
		effect.
* `int {name}_count`
	- Contains the number of identifiers defined by the enum. This can differ
		from `{name}_max` if `XVALUE` is enabled.
* `int {name}_values[{name}_count]`
	- Holds the value of each identifier in an array.
* `char *{name}_strs[{name}_count]`
	- Holds the string name of each identifier in an array.
* `int {name}_groups[{name}_count]`
	- Holds the group of each identifier in an array. Only available if
		`XGROUP` is enabled.

#### Methods

* `unsigned int {name}_index(int value)`
	- Get the index of the given value. Returns -1 for invalid values.
		This is mostly useful for enums that have specified values (i.e. those
		with the `XVALUE` option enabled).
* `char *{name}_str(int value)`
	- Get the string name of the given value.
* `void {name}_iter(int callback(int))`
	- Call the given function with each value until it returns nonzero.
* `int {name}_group(int value)`
	- Get the group of the given value. Only available if `XGROUP` is enabled.
* `void {name}_group_iter(int group, int callback(int))`
	- Call the given function with each value until it returns nonzero.


### Structs


The `struct` datatype can be used to create structs. There are two mandatory
fields: `type`, a C type specifier, and `member`, a C identifier to be used as
the member's name.

#### Optional fields

* `XGROUP`: Enables grouping of struct members.
	- `[options]` component: `#define XGROUP 1`
	- `[optional-fields]` component: `GROUP(group)`

#### Variables

* `typedef struct {name} { *; [ *; [ ... ] ] } {name}`
	- Defines the struct itself.
* `typedef enum { {name}_member_*, [ {name}_member_*, [ ... ] ]
	{name}_members } {name}_enum`
	- Defines an enum corresponding to the members of the struct. This enum
		cannot be altered, e.g. to have specified values.
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
	- Get a pointer to the index-th member of the struct.
* `char *{name}_str({name} *structure, void *member)`
	- Get the stringified name of a member, or NULL for invalid input.
* `char *{name}_type_str({name} *structure, void *member)`
	- Get the stringified type of a member, or NULL for invalid input.
* `void {name}_iter({name} *structure, int callback(void *))`
	- Call the given function with a pointer to each member until it returns
		nonzero.
* `char *{name}_print_member({name} *structure, void *member, const char *format)`
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
* `void {name}_group_iter({name} *structure, int group, int callback(void *))`
	- Call the given function with a pointer to each member in the group until
		it returns nonzero.


Examples
--------


See [example.c](example.c) and the files referenced in
[xdata.h](xdata.h) for examples of XData in action.
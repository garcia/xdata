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

* `X({identifier}[, ={value}])`
    - `{identifier}`: a C identifier to be placed into the enum.
    - `{value}`: a value to explicitly assign to the member. For example,
    	`X(foo)` and `X(foo, =5)` are both valid, but `X(foo, 5)` is not.

#### Optional fields

* `XGROUP`: Enables grouping of enum members.
	- `[options]` component: `#define XGROUP 1`
	- `[optional-fields]` component: `GROUP({group})`, where `{group}` is an
        integer.
* `XPREFIX`: Prefixes each enum member with a partial identifier.
	- `[options]` component: `#define XPREFIX prefix`
	- `[optional-fields]` component: none.
* `XASSOC`: Creates an arbitrarily typed associative array.
	- `[options]` components: `#define XASSOC 1`,
		`#define XASSOC_NAME {assoc_name}`, `#define XASSOC_TYPE {assoc_type}`,
		and optionally `#define XASSOC_INVALID {assoc_invalid}`, which sets the
		return value for invalid input to the associated function (this
		defaults to NULL).
	- `[optional-fields]` component: `ASSOC({data})`, where `{data}` is a value
		of type `{assoc_type}`.

#### Variables

* `typedef enum {name} {
		[prefix]{identifier} [={value}], [
		[prefix]{identifier} [={value}], [ ... ] ]
		} {name}`
	- Defines the enum itself. `[prefix]` is only present if `XPREFIX` is
		enabled. `[={value}]` is only present if the member has an explicitly
		assigned value.
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
* `{assoc_type} {name}_assoc_{assoc_name}[{name}_count]`
	- Holds the data associated with each identifier in an array. Only
		available if `XASSOC` is enabled.

#### Methods

* `unsigned int {name}_index(int value)`
	- Get the index of the given value, or -1 for invalid input. This is mostly
		useful for enums that have members with explicitly specified values.
* `char *{name}_str(int value)`
	- Get the string name of the given value, or NULL for invalid input.
* `int {name}_iter(int callback(int, void *), void *data)`
	- Call the given function with each value and a user-specified pointer
		until it returns nonzero. If the callback returns nonzero, this
		function returns the same value. Otherwise it returns zero after the
		callback has been executed for each member.
* `int {name}_group(int value)`
	- Get the group of the given value, or -1 for invalid input. Only available
        if `XGROUP` is enabled.
* `int {name}_group_iter(int group, int callback(int))`
	- Call the given function with each value in the group and a user-specified
		pointer until it returns nonzero. If the callback returns nonzero, this
		function returns the same value. Otherwise it returns zero after the
		callback has been executed for each member. Only available if `XGROUP`
		is enabled.
* `{assoc_type} {name}_{assoc_name}(int value)`
	- Get the data associated with the given value, or `{assoc_invalid}` for
		invalid input. Only available if `XASSOC` is enabled.


### Structs


#### Mandatory fields

* `X({type}, {member}[, {suffix}])`
    - `{type}`: a C type specifier to be applied to this member.
    - `{member}`: a C identifier to be used as the member's name.
    - `{suffix}`: a suffix to be placed after the member's declaration. This
    	can be used to create arrays and bitfields. For example,
    	`X(int, foo, [10])`, `X(int, foo, :1)`, and `X(int, foo)` are all valid
    	and specify an array, a one-bit bitfield, and a regular int,
    	respectively.

#### Optional fields

* `XGROUP`: Enables grouping of struct members.
	- `[options]` component: `#define XGROUP 1`
	- `[optional-fields]` component: `GROUP({group})`, where `{group}` is an
        integer.

#### Variables

* `typedef struct {name} {
		{type} {member} [suffix]; [
		{type} {member} [suffix]; [ ... ] ]
		} {name}`
	- Defines the struct itself. `[suffix]` is only present if the member has a
		suffix assigned.
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
* `int {name}_iter({name} *structure, int callback(void *, void *),
		void *data)`
	- Call the given function with a pointer to each member and a
		user-specified pointer until it returns nonzero. If the callback
		returns nonzero, this function returns the same value. Otherwise it
		returns zero after the callback has been executed for each member.
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
* `int {name}_group_iter({name} *structure, int group,
		int callback(void *, void *), void *data)`
	- Call the given function with a pointer to each member in the group and a
		user-specified pointer until it returns nonzero. If the callback
		returns nonzero, this function returns the same value. Otherwise it
		returns zero after the callback has been executed for each member.


Examples
--------


See [example.c](example.c) and the files referenced in
[xdata.h](xdata/xdata.h) for examples of XData in action.

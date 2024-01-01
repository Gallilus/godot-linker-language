#include "linker_index_get.h"

void LinkerIndexGet::_bind_methods() {}

Variant LinkerIndexGet::get_placeholder_value() const {
	// get the object index value
	return Variant();
}

Dictionary LinkerIndexGet::get_placeholder_info() const {
	// prefix dictionary with type PropertyInfo
	// keys (type, value)
	return Dictionary(/*get the object index info*/);
}
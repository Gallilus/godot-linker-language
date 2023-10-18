#include "register_types.h"

#include "core/object/class_db.h"
#include "LinkerScript.h"
#include "summator.h"

void initialize_linker_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
			return;
	}
	ClassDB::register_class<LinkerScript>();
}

void uninitialize_linker_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
			return;
	}
   // Nothing to do here in this example.
}
#include "register_types.h"

#include "core/config/engine.h"
#include "core/object/class_db.h"

#include "language/linker_language.h"
#include "language/linker_script.h"

using namespace godot;

LinkerLanguage *linker_language = nullptr;
//LinkerLoader *linker_loader = nullptr;
//LinkerSaver *linker_saver = nullptr;

void initialize_linker_module(ModuleInitializationLevel p_level) {
	if (p_level == GDEXTENSION_INITIALIZATION_CORE) {
		// not accesable by extensions
	}
	if (p_level == GDEXTENSION_INITIALIZATION_SERVERS) {
		GDREGISTER_CLASS(LinkerScript);

		linker_language = memnew(LinkerLanguage);
		ScriptServer::register_language(linker_language);

		//		ClassDB::register_class<LinkerLoader>();
		//		ClassDB::register_class<LinkerSaver>();
	}
	if (p_level == GDEXTENSION_INITIALIZATION_SCENE) {
		//		linker_saver = memnew(LinkerSaver);
		//		ResourceSaver::get_singleton()->add_resource_format_saver(linker_saver);
		//		linker_loader = memnew(LinkerLoader);
		//		ResourceLoader::get_singleton()->add_resource_format_loader(linker_loader);
	}
	if (p_level == GDEXTENSION_INITIALIZATION_EDITOR) {
	}
	if (p_level == GDEXTENSION_MAX_INITIALIZATION_LEVEL) {
	}
}

void uninitialize_linker_module(ModuleInitializationLevel p_level) {
	if (p_level == GDEXTENSION_INITIALIZATION_CORE) {
	}
	if (p_level == GDEXTENSION_INITIALIZATION_SERVERS) {
	}
	if (p_level == GDEXTENSION_INITIALIZATION_SCENE) {
		if (linker_language) {
			memdelete(linker_language);
		}
		/*if (linker_saver) {
			memdelete(linker_saver);
		}
		if (linker_loader) {
			memdelete(linker_loader);
		}*/
	}
	if (p_level == GDEXTENSION_INITIALIZATION_EDITOR) {
	}
	if (p_level == GDEXTENSION_MAX_INITIALIZATION_LEVEL) {
		//ERR_PRINT("this never prints");
	}
}
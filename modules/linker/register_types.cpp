#include "register_types.h"

#include "core/config/engine.h"
#include "core/object/class_db.h"

#include "editor/linker_editor.h"
#include "language/linker_function.h"
#include "language/linker_index_get.h"
#include "language/linker_language.h"
#include "language/linker_link.h"
#include "language/linker_loader.h"
#include "language/linker_saver.h"
#include "language/linker_scene_refrence.h"
#include "language/linker_script.h"

using namespace godot;

LinkerLanguage *linker_language = nullptr;
Ref<LinkerSaver> linker_saver;
Ref<LinkerLoader> linker_loader;

void initialize_linker_module(ModuleInitializationLevel p_level) {
	if (p_level == GDEXTENSION_INITIALIZATION_CORE) {
		// not accesable by extensions
	}
	if (p_level == GDEXTENSION_INITIALIZATION_SERVERS) {
		GDREGISTER_CLASS(LinkerScript);
		GDREGISTER_ABSTRACT_CLASS(LinkerLink);
		GDREGISTER_CLASS(LinkerSceneRefrence);
		GDREGISTER_CLASS(LinkerIndexGet);
		GDREGISTER_CLASS(LinkerFunction);
		// When Saving and loading no longer works on a rebase https://github.com/godotengine/godot/pull/84611
		// GDREGISTER_ calls _bind_methods() which is not needed for this class or getting things lige signals to work on base classes

		linker_language = memnew(LinkerLanguage);
		ScriptServer::register_language(linker_language);

		linker_loader.instantiate();
		ResourceLoader::add_resource_format_loader(linker_loader);

		linker_saver.instantiate();
		ResourceSaver::add_resource_format_saver(linker_saver);
	}
	if (p_level == GDEXTENSION_INITIALIZATION_SCENE) {
	}
	if (p_level == GDEXTENSION_INITIALIZATION_EDITOR) {
		LinkerEditor::register_editor();
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
	}
	if (p_level == GDEXTENSION_INITIALIZATION_EDITOR) {
	}
}
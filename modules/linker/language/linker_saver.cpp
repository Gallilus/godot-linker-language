#include "linker_saver.h"

Error LinkerSaver::save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags) {
	Ref<LinkerScript> sqscr = p_resource;
	ERR_FAIL_COND_V(sqscr.is_null(), ERR_INVALID_PARAMETER);

	String source = sqscr->get_source_code();

	{
		Error err;
		Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::WRITE, &err);

		ERR_FAIL_COND_V_MSG(err, err, "Cannot save LinkerScript file '" + p_path + "'.");

		file->store_string(source);
		if (file->get_error() != OK && file->get_error() != ERR_FILE_EOF) {
			return ERR_CANT_CREATE;
		}
	}
	// ToDo if reload on save is enabled, reload the script
	return OK;
}

bool LinkerSaver::recognize(const Ref<Resource> &p_resource) const {
	return Object::cast_to<LinkerScript>(*p_resource) != nullptr;
}

void LinkerSaver::get_recognized_extensions(const Ref<Resource> &p_resource, List<String> *p_extensions) const {
	LinkerLanguage::get_singleton()->get_recognized_extensions(p_extensions);
}

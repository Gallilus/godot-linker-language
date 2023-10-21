#include "linker_loader.h"

#include "core/config/engine.h"
#include "core/io/file_access.h"

Ref<Resource> LinkerLoader::load(const String &p_path, const String &p_original_path, Error *r_error, bool p_use_sub_threads, float *r_progress, CacheMode p_cache_mode) {
	if (p_use_sub_threads) {
		ERR_PRINT("sub threads not supported");
	}

	Error err;
	Ref<LinkerScript> scr = LinkerLanguage::get_singleton()->create_script();

	{
		Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::READ, &err);

		scr->set_path(p_path);
		scr->set_source_code(file->get_as_utf8_string());
	}

	if (err && scr.is_valid()) {
		// If !scr.is_valid(), the error was likely from scr->load_source_code(), which already generates an error.
		ERR_PRINT_ED(vformat(R"(Failed to load script "%s" with error "%s".)", p_path, error_names[err]));
	}

	if (r_error) {
		// Don't fail loading because of parsing error.
		*r_error = scr.is_valid() ? OK : err;
	}

	return scr;
}

void LinkerLoader::get_recognized_extensions(List<String> *p_extensions) const {
	LinkerLanguage::get_singleton()->get_recognized_extensions(p_extensions);
}

bool LinkerLoader::handles_type(const String &p_type) const {
	return p_type == LinkerLanguage::get_singleton()->get_type() || p_type == "Script";
}

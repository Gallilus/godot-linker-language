#include "linker_loader.h"

Ref<Resource> LinkerLoader::load(const String &p_path, const String &p_original_path, Error *r_error, bool p_use_sub_threads, float *r_progress, CacheMode p_cache_mode) {
	if (p_use_sub_threads) {
		ERR_PRINT("sub threads not supported");
	}

	ConfigFile confif_file = ConfigFile();

	Error err;
	err = confif_file.load(p_path);
	ERR_FAIL_COND_V_MSG(err, Ref<Resource>(), "Cannot load LinkerScript file '" + p_path + "'.");

	Ref<LinkerScript> linker_script = memnew(LinkerScript);
	linker_script->_set_script_loading();

	read_script_settings(linker_script, confif_file);
	read_script_values(linker_script, confif_file);
	linker_script->init_links_refrences();

	linker_script->_set_script_loaded();
	return linker_script;
}

void LinkerLoader::get_recognized_extensions(List<String> *p_extensions) const {
	LinkerLanguage::get_singleton()->get_recognized_extensions(p_extensions);
}

bool LinkerLoader::handles_type(const String &p_type) const {
	return p_type == LinkerLanguage::get_singleton()->get_type() || p_type == "Script";
}

void LinkerLoader::read_script_settings(const Ref<LinkerScript> &p_script, ConfigFile &p_config_file) {
	String section = "script_settings";
#ifdef TOOLS_ENABLED
	p_script->tool = p_config_file.get_value(section, "tool", p_script->tool);
#endif
	p_script->valid = p_config_file.get_value(section, "valid", p_script->valid);
	p_script->abstract = p_config_file.get_value(section, "abstract", p_script->abstract);
	p_script->reloading = p_config_file.get_value(section, "reloading", p_script->reloading);
	p_script->resize_links(p_config_file.get_value(section, "link_count", p_script->get_link_count()));
}

void LinkerLoader::read_script_values(const Ref<LinkerScript> &p_script, ConfigFile &p_config_file) {
	String section = "script_values";
	p_script->set_function_list(p_config_file.get_value(section, "methods", p_script->get_function_list()));
	p_script->set_property_list(p_config_file.get_value(section, "properties", p_script->get_property_list()));
	p_script->set_constants(p_config_file.get_value(section, "constants", p_script->get_constants()));
	p_script->set_signal_list(p_config_file.get_value(section, "signals", p_script->get_signal_list()));
	p_script->set_links(p_config_file.get_value(section, "links", p_script->get_links()));
}

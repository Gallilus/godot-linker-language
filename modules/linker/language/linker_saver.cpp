#include "linker_saver.h"

Error LinkerSaver::save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags) {
	Ref<LinkerScript> linker_script = p_resource;
	ERR_FAIL_COND_V(linker_script.is_null(), ERR_INVALID_PARAMETER);

	ConfigFile confif_file = ConfigFile();

	write_script_settings(linker_script, confif_file);
	write_script_values(linker_script, confif_file);

	Error err = confif_file.save(p_path);
	ERR_FAIL_COND_V_MSG(err, err, "Cannot save LinkerScript file '" + p_path + "'.");

#ifdef TOOLS_ENABLED
	linker_script->set_saved(true);
#endif
	// refresh editor
	return err;
}

bool LinkerSaver::recognize(const Ref<Resource> &p_resource) const {
	return Object::cast_to<LinkerScript>(*p_resource) != nullptr;
}

void LinkerSaver::get_recognized_extensions(const Ref<Resource> &p_resource, List<String> *p_extensions) const {
	LinkerLanguage::get_singleton()->get_recognized_extensions(p_extensions);
}

void LinkerSaver::write_script_settings(const Ref<LinkerScript> &p_script, ConfigFile &p_config_file) {
	String section = "script_settings";
#ifdef TOOLS_ENABLED
	p_config_file.set_value(section, "tool", p_script->tool);
#endif
	p_config_file.set_value(section, "valid", p_script->valid);
	p_config_file.set_value(section, "abstract", p_script->abstract);
	p_config_file.set_value(section, "reloading", p_script->reloading);
	p_config_file.set_value(section, "link_count", p_script->get_link_count());
}

void LinkerSaver::write_script_values(const Ref<LinkerScript> &p_script, ConfigFile &p_config_file) {
	String section = "script_values";
	p_config_file.set_value(section, "members", p_script->get_members());
	p_config_file.set_value(section, "methods", p_script->get_method_list());
	p_config_file.set_value(section, "properties", p_script->get_property_list());
	p_config_file.set_value(section, "constants", p_script->get_constants());
	p_config_file.set_value(section, "signals", p_script->get_signal_list());
	p_config_file.set_value(section, "links", p_script->get_links());
}
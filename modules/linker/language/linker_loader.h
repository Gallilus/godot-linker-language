#ifndef LINKER_LOADER_H
#define LINKER_LOADER_H

#include "core/io/config_file.h"
#include "core/io/resource_loader.h"

#include "linker_language.h"
#include "linker_script.h"

class LinkerLoader : public ResourceFormatLoader {
	GDCLASS(LinkerLoader, ResourceFormatLoader)

public:
	virtual Ref<Resource> load(const String &p_path, const String &p_original_path = "", Error *r_error = nullptr, bool p_use_sub_threads = false, float *r_progress = nullptr, CacheMode p_cache_mode = CACHE_MODE_REUSE) override;
	virtual void get_recognized_extensions(List<String> *p_extensions) const override;
	virtual bool handles_type(const String &p_type) const override;

	void read_script_settings(const Ref<LinkerScript> &p_script, ConfigFile &p_config_file);
	void read_script_values(const Ref<LinkerScript> &p_script, ConfigFile &p_config_file);

	void set_script_loaded();

protected:
	static void _bind_methods() {}
};
#endif // LINKER_LOADER_H
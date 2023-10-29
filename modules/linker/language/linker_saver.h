#ifndef LINKER_SAVER_H
#define LINKER_SAVER_H

#include "core/io/config_file.h"
#include "core/io/resource_saver.h"

#include "linker_language.h"
#include "linker_script.h"

class LinkerSaver : public ResourceFormatSaver {
	GDCLASS(LinkerSaver, ResourceFormatSaver)
	friend class LinkerScript;

public:
	virtual Error save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags = 0) override;
	virtual bool recognize(const Ref<Resource> &p_resource) const override;
	virtual void get_recognized_extensions(const Ref<Resource> &p_resource, List<String> *p_extensions) const override;

	void write_script_settings(const Ref<LinkerScript> &p_script, ConfigFile &p_config_file);
	void write_script_values(const Ref<LinkerScript> &p_script, ConfigFile &p_config_file);

protected:
	static void _bind_methods() {}
};
#endif // LINKER_SAVER_H
#ifndef LINKER_SAVER_H
#define LINKER_SAVER_H

#include "core/io/resource_saver.h"

#include "linker_language.h"
#include "linker_script.h"

class LinkerSaver : public ResourceFormatSaver {
	GDCLASS(LinkerSaver, ResourceFormatSaver)

public:
	virtual Error save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags = 0) override;
	virtual bool recognize(const Ref<Resource> &p_resource) const override;
	virtual void get_recognized_extensions(const Ref<Resource> &p_resource, List<String> *p_extensions) const override;

protected:
	static void _bind_methods() {}
};
#endif // LINKER_SAVER_H
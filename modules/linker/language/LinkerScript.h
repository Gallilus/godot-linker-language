#ifndef LINKER_SCRIPT_H
#define LINKER_SCRIPT_H

#include "core/object/script_language.h"

class LinkerScript : public Script {
	GDCLASS(LinkerScript, Script);

	int count;

protected:
	static void _bind_methods();

public:
	LinkerScript();
};

#endif // LINKER_SCRIPT_H
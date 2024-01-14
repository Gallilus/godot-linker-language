#ifndef LINKER_FUNCTION_INSTANCE_H
#define LINKER_FUNCTION_INSTANCE_H

#include "linker_function.h"
#include "linker_link_instance.h"

class LinkerFunctionInstance : public LinkerLinkInstance {
	friend class LinkerFunction;

public:
	void LinkessassarFunctionInstance();
};

#endif // LINKER_FUNCTION_INSTANCE_H
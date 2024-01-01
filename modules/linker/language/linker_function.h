#ifndef LINKER_FUNCTION_H
#define LINKER_FUNCTION_H

#include "editor/editor_node.h"
#include "linker_link.h"

class LinkerFunction : public LinkerLink {
	GDCLASS(LinkerFunction, LinkerLink);

private:
protected:
	static void _bind_methods() {}
	virtual void _set_owned_links() override;

public:
	virtual Variant get_placeholder_value() const override;
	virtual Dictionary get_placeholder_info() const override;
	virtual String get_graph_category() const override { return "graph_output"; }
};

#endif // LINKER_FUNCTION_H
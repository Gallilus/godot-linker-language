#ifndef LINKER_FUNCTION_H
#define LINKER_FUNCTION_H

#include "editor/editor_node.h"
#include "linker_link.h"

class LinkerLinkInstance;

class LinkerFunction : public LinkerLink {
	GDCLASS(LinkerFunction, LinkerLink);

private:
protected:
	static void _bind_methods() {}
	virtual void _set_owned_links() override;

	virtual void _initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_start_mode, int p_stack_size) override;

public:
	virtual Variant get_placeholder_value() const override;
	virtual Dictionary get_placeholder_info() const override;
	virtual String get_graph_category() const override { return "graph_output"; }

	virtual bool can_drop(Ref<LinkerLink> drag_link) const override { return false; }

	virtual LinkerLinkInstance *get_instance(LinkerScriptInstance *p_host, int p_start_mode, int p_stack_size) override;
};

#endif // LINKER_FUNCTION_H
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

	virtual void _initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_stack_size) override;

public:
	virtual Variant get_placeholder_value() const override;
	virtual Dictionary get_placeholder_info() const override;
	virtual String get_graph_category() const override { return "graph_output"; }
	virtual bool controler_at_object() const override { return false; }
	virtual int get_argument_count() const override { return 1; }
	virtual PropertyInfo get_input_value_port_info(int p_idx) const override;

	virtual bool can_drop_on_link(Ref<LinkerLink> drag_link) const override;

	virtual LinkerLinkInstance *get_instance(LinkerScriptInstance *p_host, int p_stack_size) override;
	virtual void remove_instance(LinkerScriptInstance *p_host, int p_stack_size) override;
};

////////////////////////////////////////////////////////////////////////
//////////////////////////////  INSTANCE  //////////////////////////////
////////////////////////////////////////////////////////////////////////

class LinkerFunctionInstance : public LinkerLinkInstance {
	friend class LinkerFunction;

protected:
	virtual int _step(StartMode p_start_mode, Callable::CallError &r_error, String &r_error_str) override;

public:
};

#endif // LINKER_FUNCTION_H
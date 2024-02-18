#ifndef LINKER_INDEX_CALL_H
#define LINKER_INDEX_CALL_H

#include "editor/editor_node.h"
#include "linker_link.h"
#include "linker_script.h"

class LinkerIndexCall : public LinkerLink {
	GDCLASS(LinkerIndexCall, LinkerLink);

private:
protected:
	static void _bind_methods() {}
	virtual void _set_owned_links() override {}
	virtual void _initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_stack_size) override;

public:
	MethodInfo get_method_info() const;

	virtual Variant get_placeholder_value() const override { return Variant(); }
	virtual Dictionary get_placeholder_info() const override;

	virtual Variant get_drag_data() const override;
	virtual bool can_drop_on_link(Ref<LinkerLink> drag_link) const override;
	virtual bool can_drop_on_arg(Ref<LinkerLink> drag_link) const override;
	virtual bool can_drop_on_object(Ref<LinkerLink> drag_link) const override { return true; }
	virtual bool controler_at_object() const override { return false; }
	virtual int get_argument_count() const override;

	virtual LinkerLinkInstance *get_instance(LinkerScriptInstance *p_host, int p_stack_size) override;
	virtual void remove_instance(LinkerScriptInstance *p_host, int p_stack_size) override;
};

////////////////////////////////////////////////////////////////////////
//////////////////////////////  INSTANCE  //////////////////////////////
////////////////////////////////////////////////////////////////////////

class LinkerIndexCallInstance : public LinkerLinkInstance {
	friend class LinkerIndexCall;

	Vector<Variant> input_args;

protected:
	virtual int _step(StartMode p_start_mode, Callable::CallError &r_error, String &r_error_str) override;

public:
	~LinkerIndexCallInstance();
};

#endif // LINKER_INDEX_CALL_H
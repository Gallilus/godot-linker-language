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
	virtual Variant get_placeholder_value() const override { return Variant(); }
	virtual Dictionary get_placeholder_info() const override;

	virtual bool can_drop(Ref<LinkerLink> drag_link) const override;
	virtual bool can_drop_argument(Ref<LinkerLink> drag_link) const override { return true; }
	virtual bool can_drop_source(Ref<LinkerLink> drag_link) const override { return true; }
	virtual void drop_data(Ref<LinkerLink> dropped_link) override;
	virtual bool controler_at_source() const override { return false; }

	virtual LinkerLinkInstance *get_instance(LinkerScriptInstance *p_host, int p_stack_size) override;
	virtual void remove_instance(LinkerScriptInstance *p_host, int p_stack_size) override;

	LinkerIndexCall();
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
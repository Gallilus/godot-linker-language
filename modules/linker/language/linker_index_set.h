#ifndef LINKER_INDEX_SET_H
#define LINKER_INDEX_SET_H

#include "editor/editor_node.h"
#include "linker_link.h"
#include "linker_script.h"

class LinkerIndexSet : public LinkerLink {
	GDCLASS(LinkerIndexSet, LinkerLink);

private:
protected:
	static void _bind_methods();
	virtual void _set_owned_links() override {}
	virtual void _initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_stack_size) override;

public:
	virtual Variant get_placeholder_value() const override;
	virtual Dictionary get_placeholder_info() const override;

	virtual bool can_drop_on_link(Ref<LinkerLink> drag_link) const override;
	virtual bool can_drop_on_object(Ref<LinkerLink> drag_link) const override { return true; }
	virtual bool controler_at_object() const override { return false; }
	virtual int get_argument_count() const override { return 1; }

	virtual LinkerLinkInstance *get_instance(LinkerScriptInstance *p_host, int p_stack_size) override;
	virtual void remove_instance(LinkerScriptInstance *p_host, int p_stack_size) override;
};

////////////////////////////////////////////////////////////////////////
//////////////////////////////  INSTANCE  //////////////////////////////
////////////////////////////////////////////////////////////////////////

class LinkerIndexSetInstance : public LinkerLinkInstance {
	friend class LinkerIndexSet;

protected:
	virtual int _step(StartMode p_start_mode, Callable::CallError &r_error, String &r_error_str) override;

public:
};

#endif // LINKER_INDEX_SET_H
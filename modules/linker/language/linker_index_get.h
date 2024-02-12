#ifndef LINKER_INDEX_GET_H
#define LINKER_INDEX_GET_H

#include "editor/editor_node.h"
#include "linker_link.h"
#include "linker_script.h"

class LinkerIndexGet : public LinkerLink {
	GDCLASS(LinkerIndexGet, LinkerLink);

private:
protected:
	static void _bind_methods();
	virtual void _set_owned_links() override {}
	virtual void _initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_stack_size) override;

public:
	virtual Variant get_placeholder_value() const override;
	virtual Dictionary get_placeholder_info() const override;

	virtual bool can_drop_on_link(Ref<LinkerLink> drag_link) const override;
	virtual bool can_drop_on_source(Ref<LinkerLink> drag_link) const override { return true; }
	virtual bool controler_at_source() const override { return true; }
	virtual bool use_argument() const override { return false; }

	virtual LinkerLinkInstance *get_instance(LinkerScriptInstance *p_host, int p_stack_size) override;
	virtual void remove_instance(LinkerScriptInstance *p_host, int p_stack_size) override;
};

////////////////////////////////////////////////////////////////////////
//////////////////////////////  INSTANCE  //////////////////////////////
////////////////////////////////////////////////////////////////////////

class LinkerIndexGetInstance : public LinkerLinkInstance {
	friend class LinkerIndexGet;

protected:
	virtual int _step(StartMode p_start_mode, Callable::CallError &r_error, String &r_error_str) override;

public:
};

#endif // LINKER_INDEX_GET_H
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
	virtual void _initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_start_mode, int p_stack_size) override {}

public:
	virtual Variant get_placeholder_value() const override { return Variant(); }
	virtual Dictionary get_placeholder_info() const override { return Dictionary(); }

	virtual bool can_drop(Ref<LinkerLink> drag_link) const override;
	virtual void drop_data(Ref<LinkerLink> dropped_link) override;

	virtual LinkerLinkInstance *get_instance(LinkerScriptInstance *p_host, int p_start_mode, int p_stack_size) override { return nullptr; }
};

#endif // LINKER_INDEX_CALL_H
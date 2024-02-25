#ifndef LINKER_VATIANT_OPERATOR_H
#define LINKER_VATIANT_OPERATOR_H

#include "editor/editor_node.h"
#include "linker_link.h"
#include "linker_script.h"

class LinkerVariantOperator : public LinkerLink {
	GDCLASS(LinkerVariantOperator, LinkerLink);

private:
	Variant::Operator optr;

protected:
	static void _bind_methods() {}
	virtual void _set_owned_links() override {}

	virtual void _initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_stack_size) override;

public:
	void set_operator(Variant::Operator p_optr);
	virtual void set_index(StringName p_index) override;

	virtual Variant get_placeholder_value() const override { return Variant(); }
	virtual Dictionary get_placeholder_info() const override;

	virtual bool can_drop_on_link(Ref<LinkerLink> drag_link) const override;
	virtual bool can_drop_on_arg(Ref<LinkerLink> drag_link) const override;
	virtual bool can_drop_on_object(Ref<LinkerLink> drag_link) const override { return false; }
	virtual bool controler_at_object() const override { return false; }
	virtual int get_argument_count() const override;

	virtual LinkerLinkInstance *get_instance(LinkerScriptInstance *p_host, int p_stack_size) override;
	virtual void remove_instance(LinkerScriptInstance *p_host, int p_stack_size) override;
};

////////////////////////////////////////////////////////////////////////
//////////////////////////////  INSTANCE  //////////////////////////////
////////////////////////////////////////////////////////////////////////

class LinkerVariantOperatorInstance : public LinkerLinkInstance {
	friend class LinkerVariantOperator;

	Variant::Operator optr;
	Vector<Variant> input_args;

protected:
	virtual int _step(StartMode p_start_mode, Callable::CallError &r_error, String &r_error_str) override;

public:
	~LinkerVariantOperatorInstance();
};

#endif // LINKER_VATIANT_OPERATOR_H
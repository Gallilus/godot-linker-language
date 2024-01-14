#ifndef LINKER_SCRIPT_INSTANCE_H
#define LINKER_SCRIPT_INSTANCE_H

#include "linker_language.h"
#include "linker_link.h"
#include "linker_link_instance.h"
#include "linker_script.h"

#include "core/object/script_instance.h"

class LinkerScriptInstance : public ScriptInstance {
	friend class LinkerScript;

	ObjectID owner_id;
	Object *owner = nullptr;
	Ref<LinkerScript> script;

	bool initialized = false;

	Vector<Variant> members;
	bool base_ref_counted;
	Vector<LinkerLinkInstance> link_instances;

protected:
public:
	virtual bool set(const StringName &p_name, const Variant &p_value) override;
	virtual bool get(const StringName &p_name, Variant &r_ret) const override;
	virtual void get_property_list(List<PropertyInfo> *p_properties) const override;
	virtual Variant::Type get_property_type(const StringName &p_name, bool *r_is_valid = nullptr) const override;
	virtual void validate_property(PropertyInfo &p_property) const override;

	virtual bool property_can_revert(const StringName &p_name) const override;
	virtual bool property_get_revert(const StringName &p_name, Variant &r_ret) const override;

	virtual Object *get_owner() { return owner; }

	virtual void get_method_list(List<MethodInfo> *p_list) const override;
	virtual bool has_method(const StringName &p_method) const override;

	virtual Variant callp(const StringName &p_method, const Variant **p_args, int p_argcount, Callable::CallError &r_error) override;
	Variant _call_internal(const StringName &p_method, int p_stack_size, int p_flow_stack_pos, int p_pass, bool p_resuming_yield, Callable::CallError &r_error);

	virtual void notification(int p_notification, bool p_reversed = false) override {}

	virtual Ref<Script> get_script() const override { return script; }

	virtual ScriptLanguage *get_language() override { return LinkerLanguage::get_singleton(); }

	LinkerLinkInstance *add_link_instance(LinkerLinkInstance p_link_instance);
	void initialize();

	LinkerScriptInstance();
	~LinkerScriptInstance();
};

#endif // LINKER_SCRIPT_INSTANCE_H
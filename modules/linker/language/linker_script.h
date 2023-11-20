#ifndef LINKER_SCRIPT_H
#define LINKER_SCRIPT_H

#include "linker_language.h"

#include "core/object/script_language.h"
#include "core/os/mutex.h"
#include "core/templates/hash_map.h"
#include "core/templates/rb_set.h"
#include "core/templates/self_list.h"
#include "core/variant/variant.h"

class LinkerScript : public Script {
	GDCLASS(LinkerScript, Script);

	//RES_BASE_EXTENSION("ls");

	friend class LinkerLanguage;
	friend class LinkerSaver;
	friend class LinkerLoader;
	friend class LinkerScriptInstance;

	bool tool = false;
	bool valid = false;
	bool abstract = false;
	bool reloading = false;

	Ref<Script> base;
	LinkerScript *_base = nullptr; //fast pointer access
	LinkerScript *_owner = nullptr; //for subclasses
public:
	struct NodeInfo {
		StringName node_class_name;
		StringName node_name;
		String node_script_file_path;
		NodePath node_scene_path;
		StringName node_scene_relative_path;

		operator Dictionary() const {
			Dictionary d;
			d["node_class_name"] = node_class_name;
			d["node_name"] = node_name;
			d["node_script_file_path"] = node_script_file_path;
			d["node_scene_path"] = node_scene_path;
			d["node_scene_relative_path"] = node_scene_relative_path;
			return d;
		}
		static NodeInfo from_dict(const Dictionary &p_dict) {
			NodeInfo node_info;
			node_info.node_class_name = StringName(p_dict["node_class_name"]);
			node_info.node_name = StringName(p_dict["node_name"]);
			node_info.node_script_file_path = String(p_dict["node_script_file_path"]);
			node_info.node_scene_path = NodePath(p_dict["node_scene_path"]);
			node_info.node_scene_relative_path = StringName(p_dict["node_scene_relative_path"]);
			return node_info;
		}
	};

	struct VariableInfo {
		int index = 0;
		PropertyInfo info;
		Variant default_value;
		StringName setter;
		StringName getter;
		bool is_export = false;
		bool is_constant = false;
		bool is_private = false; // variables are private properties are public

		VariableInfo() {}

		VariableInfo(const PropertyInfo &p_info) :
				info(p_info) {}

		VariableInfo(const PropertyInfo &p_info, const Variant &p_default_value) :
				info(p_info), default_value(p_default_value) {}

		VariableInfo(const PropertyInfo &p_info, const Variant &p_default_value, const StringName &p_setter, const StringName &p_getter, bool p_is_export, bool p_is_constant, bool p_is_private) :
				info(p_info), default_value(p_default_value), setter(p_setter), getter(p_getter), is_export(p_is_export), is_constant(p_is_constant), is_private(p_is_private) {}

		operator Dictionary() const {
			Dictionary d;
			d["index"] = index;
			d["info"] = Dictionary(info);
			d["default_value"] = default_value;
			d["setter"] = setter;
			d["getter"] = getter;
			d["is_export"] = is_export;
			d["is_constant"] = is_constant;
			d["is_private"] = is_private;
			return d;
		}
		static VariableInfo from_dict(const Dictionary &p_dict) {
			VariableInfo vi;
			vi.index = p_dict["index"];
			vi.info = PropertyInfo(p_dict["info"]);
			vi.default_value = p_dict["default_value"];
			vi.setter = p_dict["setter"];
			vi.getter = p_dict["getter"];
			vi.is_export = p_dict["is_export"];
			vi.is_constant = p_dict["is_constant"];
			vi.is_private = p_dict["is_private"];
			return vi;
		}
	};

private:
	HashSet<StringName> members;

	HashMap<StringName, MethodInfo> member_functions;
	HashMap<StringName, VariableInfo> member_properties;
	HashMap<StringName, Variant> constants;
	HashMap<StringName, MethodInfo> signals;
	HashMap<StringName, NodeInfo> scene_refrences;
	Dictionary rpc_config;

#ifdef TOOLS_ENABLED
	bool is_saved = false;
	Vector<DocData::ClassDoc> docs;
#endif // TOOLS_ENABLED

	String source;
	String path;
	StringName global_name; // `class_name`.
	String simplified_icon_path;

	RBSet<Object *> instances;
	SelfList<LinkerScript> script_list;

	ScriptInstance *_create_instance(const Variant **p_args, int p_argcount, Object *p_owner, bool p_is_ref_counted, Callable::CallError &r_error);

protected:
	virtual bool editor_can_reload_from_file() override { return false; } // this is handled by editor better
	static void _bind_methods() {}
	virtual void _placeholder_erased(PlaceHolderScriptInstance *p_placeholder) override {}

public:
	virtual bool can_instantiate() const override { return !abstract; }

	virtual Ref<Script> get_base_script() const override;
	virtual StringName get_global_name() const override;
	virtual bool inherits_script(const Ref<Script> &p_script) const override;

	virtual StringName get_instance_base_type() const override; // this may not work in all scripts, will return empty if so
	virtual ScriptInstance *instance_create(Object *p_this) override;
	virtual PlaceHolderScriptInstance *placeholder_instance_create(Object *p_this) override { return nullptr; }
	virtual bool instance_has(const Object *p_this) const override;

	virtual bool has_source_code() const override;
	virtual String get_source_code() const override;
	virtual void set_source_code(const String &p_code) override;
	virtual Error reload(bool p_keep_state = false) override;

#ifdef TOOLS_ENABLED
	virtual Vector<DocData::ClassDoc> get_documentation() const override { return docs; }
	virtual String get_class_icon_path() const override { return simplified_icon_path; }
	virtual PropertyInfo get_class_category() const override;
	void set_saved(bool p_saved) { is_saved = p_saved; }
	bool is_unsaved() const { return !is_saved; }
#endif // TOOLS_ENABLED

	// TODO: In the next compat breakage rename to `*_script_*` to disambiguate from `Object::has_method()`.
	virtual bool has_method(const StringName &p_method) const override;
	virtual bool has_static_method(const StringName &p_method) const override;

	virtual MethodInfo get_method_info(const StringName &p_method) const override;

	virtual bool is_tool() const override { return tool; }
	virtual bool is_valid() const override { return valid; }
	virtual bool is_abstract() const override { return abstract; }

	virtual ScriptLanguage *get_language() const override;

	virtual bool has_script_signal(const StringName &p_signal) const override;
	virtual void get_script_signal_list(List<MethodInfo> *r_signals) const override;

	virtual bool get_property_default_value(const StringName &p_property, Variant &r_value) const override;

	virtual void update_exports() {} //editor tool
	virtual void get_script_method_list(List<MethodInfo> *p_list) const override;
	virtual void get_script_property_list(List<PropertyInfo> *p_list) const override;

	virtual int get_member_line(const StringName &p_member) const override { return -1; }

	virtual void get_constants(HashMap<StringName, Variant> *p_constants) override;
	virtual void get_members(HashSet<StringName> *p_constants) override;

	virtual bool is_placeholder_fallback_enabled() const { return false; }

	virtual const Variant get_rpc_config() const override { return rpc_config; }

	TypedArray<StringName> get_members() const;

	Dictionary get_method_list() const;
	void set_method_list(const Dictionary &p_methods);
	void set_method(const MethodInfo &p_info);

	Dictionary get_property_list() const;
	void set_property_list(const Dictionary &p_properties);
	void set_member_variable(const StringName &p_name, const PropertyInfo &p_value, Variant *p_default_value = nullptr); // /!!!!!!!!
	void set_property(const VariableInfo &p_info);

	Dictionary get_constants() const;
	void set_constants(const Dictionary &p_constants);
	void set_constant(const StringName &p_name, const Variant &p_value);

	Dictionary get_signal_list() const;
	void set_signal_list(const Dictionary &p_signals);
	void set_signal(const MethodInfo &p_info);

	TypedArray<LinkerSceneRefrence> get_scene_refrences() const;
	void set_scene_refrences(TypedArray<LinkerSceneRefrence> p_scene_refrences);
	void add_scene_refrence(Ref<LinkerSceneRefrence> p_node_info);
	void remove_scene_refrence(StringName relative_path);

	LinkerScript();
	~LinkerScript() {}
};

#endif // LINKER_SCRIPT_H
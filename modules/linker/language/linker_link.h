#ifndef LINKER_LINK_H
#define LINKER_LINK_H

#include "core/io/resource.h"
#include "editor/editor_node.h"
#include "editor/editor_string_names.h"

class LinkerScript;
class LinkerScriptInstance;
class LinkerLinkInstance;

class LinkerLink : public Resource {
	GDCLASS(LinkerLink, Resource);
	friend class LinkerScript;

private:
	int saved_links_idx = -1; // used temporarily for loading from file

	Array pull_links_idx;
	Array push_links_idx;
	int owner_links_idx = -1;

protected:
	LinkerScript *host = nullptr;
	StringName index;
	int link_component_mask = LINK_COMPONENT_DEFAULT;

	static void _bind_methods();

	HashMap<int, LinkerLinkInstance *> link_instances;

	Vector<Ref<LinkerLink>> pull_links;
	Vector<Ref<LinkerLink>> push_links;

	LinkerLink *owner = nullptr;

	virtual void _set_owned_links() = 0; // when creating a new Linkerlink set the default links like get_value for return
	void _add_owned_link(Ref<LinkerLink> p_link, bool p_is_push = false);
	virtual void _initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_stack_size) = 0;

public:
	enum LinkComponents {
		LINK_COMPONENT_PULL = 1 << 0,
		LINK_COMPONENT_PUSH = 1 << 1,
		LINK_COMPONENT_SOURCE = 1 << 2,
		LINK_COMPONENT_INPUTS = 1 << 3,
		LINK_COMPONENT_OUTPUT = 1 << 4,
		LINK_COMPONENT_EDIT_VALUE = 1 << 5,
		LINK_COMPONENT_DEFAULT = LINK_COMPONENT_SOURCE | LINK_COMPONENT_OUTPUT,
	};

	void set_host(LinkerScript *p_host);
	LinkerScript *get_host() const { return host; }
	virtual void set_source(Ref<LinkerLink> p_source);
	Ref<LinkerLink> get_source() const;
	bool has_source() const { return !pull_links.is_empty(); }
	void set_index(StringName p_index) { index = p_index; }
	StringName get_index() const { return index; }
	void set_owner(LinkerLink *p_link);
	bool is_pushed() const;
	Ref<LinkerLink> get_push_link() const;
	void disconnect_pushed_link(Ref<LinkerLink> p_link);

	int get_link_idx() const;
	void set_to_idx(int p_idx) { saved_links_idx = p_idx; }
	Array get_pull_links() const;
	Vector<Ref<LinkerLink>> get_pull_link_refs() const { return pull_links; }
	void set_pull_links(Array p_links) { pull_links_idx = p_links; }
	Array get_push_links() const;
	Vector<Ref<LinkerLink>> get_push_link_refs() const { return push_links; }
	void add_push_link_ref(Ref<LinkerLink> p_link);
	void set_push_links(Array p_links) { push_links_idx = p_links; }
	int get_owner_idx() const;
	void set_owner_idx(int p_idx) { owner_links_idx = p_idx; }
	void set_link_refrences(); // called when loading file after all links are loaded
	void add_link_ref_to_script(Ref<LinkerLink> p_link);

	virtual Variant get_placeholder_value() const = 0;
	virtual Dictionary get_placeholder_info() const = 0;
	PropertyInfo get_output_info();
	virtual String get_graph_category() const { return "graph_data"; }
	virtual bool controler_at_source() const = 0;
	Variant get_drag_data() const;
	virtual bool can_drop(Ref<LinkerLink> drag_link) const;
	virtual bool can_drop_argument(Ref<LinkerLink> drag_link) const;
	virtual bool can_drop_source(Ref<LinkerLink> drag_link) const;
	virtual void drop_data(Ref<LinkerLink> dropped_link) {}
	virtual bool use_argument() const;
	virtual bool use_source() const;

	virtual LinkerLinkInstance *get_instance(LinkerScriptInstance *p_host, int p_stack_size) = 0;
	virtual void remove_instance(LinkerScriptInstance *p_host, int p_stack_size) = 0;

	void set_link_component_mask(int p_mask) { link_component_mask = p_mask; }
	int get_link_component_mask() const { return link_component_mask; }

	void remove_from_script(bool p_force = false);
};

////////////////////////////////////////////////////////////////////////
//////////////////////////////  INSTANCE  //////////////////////////////
////////////////////////////////////////////////////////////////////////

class LinkerLinkInstance {
public:
	enum StartMode {
		START_MODE_BEGIN,
		START_MODE_CONTINUE,
		START_MODE_RESUME_YIELD,
	};

	enum StepResult {
		STEP_OK = 1 << 0,
		STEP_ERROR = 1 << 1,
		STEP_COMPLETE = 1 << 2,
		STEP_BREAKPOINT = 1 << 3,
		STEP_RESULT_YIELD = 1 << 4,
	};

protected:
	LinkerScriptInstance *host = nullptr;
	StringName index;

	Vector<LinkerLinkInstance *> pull_links;
	Vector<LinkerLinkInstance *> push_links;

	Variant value;
	int pull_count = 0;
	int push_count = 0;
	int step_count = 0;

	bool stepped = false;
	bool running = false;

	virtual int _step(StartMode p_start_mode, Callable::CallError &r_error, String &r_error_str) = 0; // link specific step

public:
	int step(StartMode p_start_mode, Callable::CallError &r_error, String &r_error_str); // full step
	Variant get_value() const { return value; }

	~LinkerLinkInstance();
};

#endif // LINKER_LINK_H
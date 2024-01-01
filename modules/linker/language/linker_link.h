#ifndef LINKER_LINK_H
#define LINKER_LINK_H

#include "core/io/resource.h"
#include "editor/editor_node.h"
#include "editor/editor_string_names.h"

class LinkerScript;

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
	StringName index = "";

	static void _bind_methods();

	Vector<Ref<LinkerLink>> pull_links;
	Vector<Ref<LinkerLink>> push_links;

	LinkerLink *owner = nullptr;

	virtual void _set_owned_links() = 0; // when creating a new Linkerlink set the default links like get_value for return
	void _add_owned_link(Ref<LinkerLink> p_link, bool p_is_push = false);

	LinkerScript *_get_host() const { return host; }

public:
	void set_host(LinkerScript *p_host);
	virtual void set_source(Ref<LinkerLink> p_source);
	void set_index(StringName p_index) { index = p_index; }
	StringName get_index() const { return index; }
	void set_owner(LinkerLink *p_link);

	int get_link_idx() const;
	void set_to_idx(int p_idx) { saved_links_idx = p_idx; }
	Array get_pull_links() const;
	Vector<Ref<LinkerLink>> get_pull_link_refs() const { return pull_links; }
	void set_pull_links(Array p_links) { pull_links_idx = p_links; }
	Array get_push_links() const;
	Vector<Ref<LinkerLink>> get_push_link_refs() const { return push_links; }
	void set_push_links(Array p_links) { push_links_idx = p_links; }
	int get_owner_idx() const;
	void set_owner_idx(int p_idx) { owner_links_idx = p_idx; }
	void set_link_refrences(); // called when loading file after all links are loaded

	virtual Variant get_placeholder_value() const = 0;
	virtual Dictionary get_placeholder_info() const = 0;
	PropertyInfo get_output_info();
	virtual String get_graph_category() const { return "graph_data"; }

	void remove_from_script(bool p_force = false);
};

#endif // LINKER_LINK_H

// function examle
// object_resource is this get_value "return" link
// the link logic is the destination

// get_value examle
// object_resource is the other link
// the link logic Object::get()
// the destination is the resource triggering this instance

// set_value examle							// set_value examle
// is triggered after the logic of the function
// object_resource is the source of the link (always a other link)
// the link logic Object::set()
// the destination is the resource triggering this instance

// a link needs to trigger its arguments
// a link needs to trigger its popstprocessing links (call_func set_value etc)
// then it is able to return its value

// the post_processing needs to be deterministic in order of operation
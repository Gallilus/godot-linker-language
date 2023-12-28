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

protected:
	LinkerScript *host = nullptr;
	static void _bind_methods();

public:
	void set_host(LinkerScript *p_host);
	LinkerScript *get_host() const;

	int get_link_idx() const;
	void set_to_idx(int p_idx);
	virtual void set_link_refrences() {} // called when loading file after all links are loaded
	virtual Vector<Ref<LinkerLink>> get_arg_links() { return Vector<Ref<LinkerLink>>(); } // a list of all links that are represented as arguments
	virtual void set_member_name(StringName p_name) {}
	virtual StringName get_member_name() const { return StringName(); }

	virtual StringName get_caption() const = 0;
	virtual StringName get_category() const = 0;
	virtual StringName get_tooltip() const = 0;

	virtual int get_arg_count() const = 0;
	virtual PropertyInfo get_arg_info(int p_idx) const = 0;
	virtual int get_default_arg_count() const = 0;
	virtual Variant get_default_arg(int p_idx) const = 0;
	virtual int get_output_count() const = 0;
	virtual PropertyInfo get_output_info(int p_idx) const = 0;

	virtual Ref<Texture2D> get_icon() const = 0;

	void remove_from_script();

	//virtual Variant get_output_refrence() const = 0; // editortime

	//virtual VisualScriptNodeInstance *instantiate(VisualScriptInstance *p_instance) = 0;
	//virtual Variant get_output() const = 0; // runtime on instance
};

#endif // LINKER_LINK_H
#ifndef LINKER_INDEX_GET_H
#define LINKER_INDEX_GET_H

#include "editor/editor_node.h"
#include "linker_link.h"
#include "linker_script.h"

class LinkerIndexGet : public LinkerLink {
	GDCLASS(LinkerIndexGet, LinkerLink);

private:
	StringName index = "";
	Ref<LinkerLink> source;
	int source_links_idx = -1; // used temporarily for loading from file

	void _set_source_links_idx(int p_source_links_idx) { source_links_idx = p_source_links_idx; }
	int _get_source_links_idx() const;

protected:
	static void _bind_methods();

public:
	virtual StringName get_member_name() const { return ""; }

	virtual void set_link_refrences(); // called when loading file after all links are loaded
	virtual Vector<Ref<LinkerLink>> get_arg_links() override; // a list of all links that are represented as arguments

	virtual StringName get_caption() const override { return "get"; };
	virtual StringName get_category() const override { return "get"; }
	virtual StringName get_tooltip() const override { return "get " + index; }

	virtual int get_arg_count() const override;
	virtual PropertyInfo get_arg_info(int p_idx) const override;
	virtual int get_default_arg_count() const override;
	virtual Variant get_default_arg(int p_idx) const override;
	virtual int get_output_count() const override { return 1; }
	virtual PropertyInfo get_output_info(int p_idx) const override;

	virtual Ref<Texture2D> get_icon() const override;

	void set_index(const StringName &p_index);
	StringName get_index() const;
	void set_source(const Ref<LinkerLink> &p_source);
	Ref<LinkerLink> get_source() const;
};

#endif // LINKER_INDEX_GET_H
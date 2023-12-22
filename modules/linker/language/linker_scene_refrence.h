#ifndef LINKER_SCENE_REFRENCE_H
#define LINKER_SCENE_REFRENCE_H

#include "editor/editor_interface.h"
#include "editor/editor_node.h"
#include "linker_link.h"
#include "scene/main/node.h"

class LinkerSceneRefrence : public LinkerLink {
	GDCLASS(LinkerSceneRefrence, LinkerLink);

private:
	StringName node_class_name;
	StringName node_name;
	String node_script_file_path;
	NodePath node_scene_path;
	StringName node_scene_relative_path;

protected:
	static void _bind_methods();

public:
	virtual StringName get_member_name() const { return get_node_scene_relative_path(); };

	virtual StringName get_caption() const override { return node_scene_relative_path; };
	virtual StringName get_category() const override { return "graph_input"; }
	virtual StringName get_tooltip() const override { return get_caption(); }

	virtual int get_arg_count() const override;
	virtual PropertyInfo get_arg_info(int p_idx) const override;
	virtual int get_default_arg_count() const override;
	virtual Variant get_default_arg(int p_idx) const override;
	virtual PropertyInfo get_output_info() const override;

	virtual Ref<Texture2D> get_icon() const override;

	void set_refrence(Node *p_ref_node, Node *p_scripted_node);

	void set_node_class_name(const StringName &p_node_class_name) { node_class_name = p_node_class_name; }
	StringName get_node_class_name() const { return node_class_name; }
	void set_node_name(const StringName &p_node_name) { node_name = p_node_name; }
	StringName get_node_name() const { return node_name; }
	void set_node_script_file_path(const String &p_node_script_file_path) { node_script_file_path = p_node_script_file_path; }
	String get_node_script_file_path() const { return node_script_file_path; }
	void set_node_scene_path(const NodePath &p_node_scene_path) { node_scene_path = p_node_scene_path; }
	NodePath get_node_scene_path() const { return node_scene_path; }
	void set_node_scene_relative_path(const StringName &p_node_scene_relative_path) { node_scene_relative_path = p_node_scene_relative_path; }
	StringName get_node_scene_relative_path() const { return node_scene_relative_path; }
};

#endif // LINKER_SCENE_REFRENCE_H
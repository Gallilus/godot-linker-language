#ifndef LINKER_SCENE_REFRENCE_H
#define LINKER_SCENE_REFRENCE_H

#include "editor/editor_interface.h"
#include "editor/editor_node.h"
#include "scene/main/node.h"

#include "linker_link.h"

class LinkerSceneRefrence : public LinkerLink {
	GDCLASS(LinkerSceneRefrence, LinkerLink);

private:
	// set_member_name
	// scene relative path is index

	StringName node_class_name;
	StringName node_name;
	String node_script_file_path;
	NodePath node_scene_path;
	StringName node_scene_relative_path;

protected:
	static void _bind_methods();
	virtual void _set_owned_links() override {}
	virtual void _initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_start_mode, int p_stack_size) override {}

public:
	virtual Variant get_placeholder_value() const override;
	virtual Dictionary get_placeholder_info() const override;
	virtual String get_graph_category() const override { return "graph_input"; }

	void set_refrence(Node *p_ref_node, Node *p_scripted_node);

	void set_node_class_name(const StringName &p_node_class_name) { node_class_name = p_node_class_name; }
	StringName get_node_class_name() const { return node_class_name; }
	void set_node_name(const StringName &p_node_name) { node_name = p_node_name; }
	StringName get_node_name() const { return node_name; }
	void set_node_script_file_path(const String &p_node_script_file_path) { node_script_file_path = p_node_script_file_path; }
	String get_node_script_file_path() const { return node_script_file_path; }
	void set_node_scene_path(const NodePath &p_node_scene_path) { node_scene_path = p_node_scene_path; }
	NodePath get_node_scene_path() const { return node_scene_path; }
	void set_node_scene_relative_path(const StringName &p_node_scene_relative_path);
	StringName get_node_scene_relative_path() const { return node_scene_relative_path; }

	virtual bool can_drop(Ref<LinkerLink> drag_link) const override;
	virtual void drop_data(Ref<LinkerLink> dropped_link) override;

	virtual LinkerLinkInstance *get_instance(LinkerScriptInstance *p_host, int p_start_mode, int p_stack_size) override { return nullptr; }
};

#endif // LINKER_SCENE_REFRENCE_H
#ifndef LINK_CONTROLER_H
#define LINK_CONTROLER_H

#include "../language/linker_link.h"
#include "../language/linker_script.h"

#include "editor/editor_node.h"
#include "scene/gui/button.h"
#include "scene/gui/control.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/margin_container.h"
#include "scene/main/viewport.h"

class LinkerEditorLayout;
class LinkConnection;

class LinkControler : public MarginContainer {
	GDCLASS(LinkControler, MarginContainer);

	const Vector2 CELL_SIZE = Vector2(16, 16);

	bool debug_node_id = false;
	bool debug_ref_id = false;
	bool debug_controler_id = false;
	bool debug_drag_and_drop = true;
	bool mouse_inside = false;
	uint64_t last_mouse_enter = 0;
	bool dragging = false;

	Rect2 rect_this() const;
	Rect2 rect_source() const;
	Rect2 rect_arg() const;
	Rect2 rect_set() const;
	Rect2 rect_index() const;
	Rect2 rect_icon() const;
	Rect2 rect_component() const;
	Rect2 rect_output() const;
	Rect2 rect_push() const;
	Rect2 rect_next() const;

	Ref<LinkerLink> link;

	VBoxContainer *controler = nullptr;
	HBoxContainer *link_rects = nullptr;
	HBoxContainer *value_rects = nullptr;
	VBoxContainer *controler_inputs = nullptr;
	VBoxContainer *controler_core = nullptr;
	VBoxContainer *controler_outputs = nullptr;
	Label *label = nullptr;
	LineEdit *edit_index = nullptr;
	TextureRect *icon = nullptr;
	Control *source_rect = nullptr;
	Control *arg_rect = nullptr;
	Control *set_rect = nullptr;
	Control *component_rect = nullptr;
	Control *output_rect = nullptr;
	Control *push_rect = nullptr;
	Control *component_output_rect = nullptr;

	bool dragging_from = false;
	bool edit_mode_line_edit = false;

protected:
	static void _bind_methods();
	void _notification(int p_what);
	
	int link_components = LinkerLink::LINK_COMPONENT_PULL;

	virtual void _instantiate();

	virtual void on_size_changed();

	Ref<Texture2D> _get_link_icon(LinkerLink *p_link) const;
	HorizontalAlignment _get_icon_h_alignement(LinkerLink *p_link) const;
	String _get_text(LinkerLink *p_link) const;
	void _set_margin(HorizontalAlignment p_align);
	void _update_margin();

	void _draw_debug();
	

public:
	bool edit_mode = false;
	// void set_layout(LinkerEditorLayout *p_layout) { layout = p_layout; }

	virtual void gui_input(const Ref<InputEvent> &p_event) override;

	virtual Variant get_drag_data(const Point2 &p_point) override;
	virtual bool can_drop_data(const Point2 &p_point, const Variant &p_data) const override;
	virtual void drop_data(const Point2 &p_point, const Variant &p_data) override;
	void set_link(Ref<LinkerLink> p_link);
	LinkerLink *get_link() const { return link.ptr(); }
	int get_link_idx() const { return link->get_link_idx(); }

	Vector2 get_connection_point_top() const;
	Vector2 get_connection_point_bottom() const;
	Vector2 get_connection_point_left() const;
	Vector2 get_connection_point_right() const;

	void start_edit_mode();
	void end_edit_mode();

	LinkControler();
	~LinkControler();
};

#endif // LINK_CONTROLER_H
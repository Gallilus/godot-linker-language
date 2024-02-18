#ifndef LINK_CONTROLER_H
#define LINK_CONTROLER_H

#include "../language/linker_index_set.h"
#include "../language/linker_link.h"
#include "../language/linker_script.h"
#include "linker_inspector.h"
#include "linker_inspector_2.h"

#include "editor/editor_node.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/control.h"
#include "scene/gui/line_edit.h"
#include "scene/main/viewport.h"
#include "scene/resources/label_settings.h"

class LinkerEditorLayout;
class LinkConnection;

class LinkControler : public VBoxContainer {
	GDCLASS(LinkControler, VBoxContainer);

	const Vector2 CELL_SIZE = Vector2(16, 16);
	const Vector2 CELL_SIZE_Y = Vector2(0, CELL_SIZE.y);
	const Vector2 CELL_SIZE_X = Vector2(CELL_SIZE.x, 0);
	const Vector2 CELL_SIZE_SMALL = Vector2(12, 12);
	const Vector2 CELL_SIZE_SMALL_Y = Vector2(0, CELL_SIZE_SMALL.y);
	const Vector2 CELL_SIZE_SMALL_X = Vector2(CELL_SIZE_SMALL.x, 0);
	const Vector2 CELL_SIZE_COLAPSED = CELL_SIZE / 4; // for components visible but not active like connected arguments
	const Vector2 CELL_SIZE_HIDDEN = Vector2(0, 0);

	bool debug_node_id = false;
	bool debug_ref_id = false;
	bool debug_controler_id = false;
	bool debug_drag_and_drop = true;
	bool mouse_inside = false;
	uint64_t last_mouse_enter = 0;
	bool dragging = false;

	Rect2 rect_this() const;
	Rect2 rect_object() const;
	Rect2 rect_arg() const;
	Rect2 rect_value_1() const;
	Rect2 rect_index() const;
	Rect2 rect_icon() const;
	Rect2 rect_push() const;
	Rect2 rect_value_2() const;

	Ref<LinkerLink> link;

	HBoxContainer *banner = nullptr;
	HBoxContainer *components = nullptr;
	VBoxContainer *arguments = nullptr;
	Label *label = nullptr;
	LineEdit *edit_index = nullptr;
	TextureRect *icon = nullptr;
	Control *object_rect = nullptr;
	Control *value_rect_1 = nullptr;
	Control *push_rect = nullptr;
	Control *value_rect_2 = nullptr; // if value rect is to small

	bool dragging_from = false;
	bool lmb_down = false;
	Dictionary drag_data;
	bool edit_mode_line_edit = false;

	void resize_font_of(Label *p_label, int p_font_size);

protected:
	static void _bind_methods();
	void _notification(int p_what);

	virtual void _instantiate();
	void set_component_visibility();

	virtual void on_size_changed();

	Ref<Texture2D> _get_link_icon(LinkerLink *p_link) const;
	HorizontalAlignment _get_icon_h_alignement(LinkerLink *p_link) const;
	String _get_text(LinkerLink *p_link) const;
	void _set_margin(HorizontalAlignment p_align);
	void _update_margin();

	void _draw_debug();

	static Ref<LinkerLink> create_index_set(const String &index);

public:
	bool edit_mode = false;
	// void set_layout(LinkerEditorLayout *p_layout) { layout = p_layout; }

	virtual void gui_input(const Ref<InputEvent> &p_event) override;

	void prep_drag_data();
	virtual Variant get_drag_data(const Point2 &p_point) override;
	virtual bool can_drop_data(const Point2 &p_point, const Variant &p_data) const override;
	virtual void drop_data(const Point2 &p_point, const Variant &p_data) override;
	void edit_property(const String &p_property);
	void set_link(Ref<LinkerLink> p_link);
	LinkerLink *get_link() const { return link.ptr(); }
	int get_link_idx() const { return link->get_link_idx(); }

	Vector2 get_connection_point_top() const;
	Vector2 get_connection_point_bottom() const;
	Vector2 get_connection_point_left() const;
	Vector2 get_connection_point_right() const;

	void start_edit_mode();
	void update_edit_mode();
	void end_edit_mode();

	LinkControler();
	~LinkControler();
};

#endif // LINK_CONTROLER_H
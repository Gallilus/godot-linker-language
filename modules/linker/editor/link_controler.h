#ifndef LINK_CONTROLER_H
#define LINK_CONTROLER_H

#include "../language/linker_link.h"
#include "../language/linker_script.h"

#include "editor/editor_node.h"
#include "scene/gui/button.h"
#include "scene/gui/control.h"
#include "scene/gui/margin_container.h"
#include "scene/main/viewport.h"

class LinkerEditorLayout;
class LinkConnection;

class LinkControler : public MarginContainer {
	GDCLASS(LinkControler, MarginContainer);

	LinkerEditorLayout *layout = nullptr;
	LinkConnection *connection = nullptr;

	Ref<LinkerLink> link;

	Button *button = nullptr;

	int margin_top = 0;
	int margin_bottom = 0;
	int margin_left = 0;
	int margin_right = 0;

protected:
	static void _bind_methods() {}
	void _notification(int p_what);

	virtual void _instantiate(); // use on_ready() and  update controle-SORT_CHILDREN
	virtual void update_connection();

	virtual void on_size_changed();

	Ref<Texture2D> _get_link_icon(LinkerLink *p_link) const;
	HorizontalAlignment _get_icon_h_alignement(LinkerLink *p_link) const;
	String _get_text(LinkerLink *p_link) const;
	void _set_margin(HorizontalAlignment p_align);

public:
	void set_layout(LinkerEditorLayout *p_layout) { layout = p_layout; }

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

	LinkControler();
	~LinkControler();
};

#endif // LINK_CONTROLER_H
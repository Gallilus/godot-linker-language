#include "tree_droppable.h"

Variant TreeDroppable::get_drag_data(const Point2 &p_point) {
	int item_id = get_button_id_at_position(p_point);
	int column = get_column_at_position(p_point);
	if (item_id != -1) {
		return Variant();
	}

	TreeItem *ti = get_item_at_position(p_point);
	set_selected(ti);

	return ti->get_metadata(column);
}
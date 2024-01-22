#include "link_connection.h"
#include "link_controler.h"

void LinkConnection::_update_connection() {
	if (!start || !end) {
		return;
	}
	curve.clear_points();
	Vector2 start_pos = start->get_global_position();
	Vector2 end_pos = end->get_global_position();
	Vector2 start_size = start->get_size();
	Vector2 end_size = end->get_size();
	curve.add_point(Vector2(start_pos.x + start_size.x, start_pos.y + start_size.y / 2));
	curve.add_point(Vector2(end_pos.x, end_pos.y + end_size.y / 2));
	set_width(2);
	set_points(curve.get_baked_points());
}

void LinkConnection::set_start(LinkControler *p_source) {
	start = p_source;
	start->connect("item_rect_changed", callable_mp(this, &LinkConnection::_update_connection));
	if (end) {
		_update_connection();
	}
}
void LinkConnection::set_end(LinkControler *p_target) {
	end = p_target;
	end->connect("item_rect_changed", callable_mp(this, &LinkConnection::_update_connection));
	// select_virtual_method->connect("id_pressed", callable_mp(this, &MembersSection::_on_virtual_method_selected));
	if (start) {
		_update_connection();
	}
}
#include "link_connection.h"
#include "link_controler.h"

void LinkConnection::_update_connection() {
	if (!start || !end) {
		return;
	}
	switch (connection_type) {
		case CONNECTION_TYPE_SOURCE: {
			curve.clear_points();
			Vector2 start_pos = start->get_connection_point_right();
			Vector2 end_pos = end->get_connection_point_left();
			int distance_x = end_pos.x - start_pos.x;
			if (distance_x < 0) {
				distance_x = distance_x * -1;
			}
			distance_x = distance_x / 3;
			curve.add_point(start_pos, Vector2(-distance_x, 0), Vector2(distance_x, 0));
			curve.add_point(end_pos, Vector2(-distance_x, 0), Vector2(distance_x, 0));
			set_default_color(Color(0.1, 0.9, 0.1, 0.9));
			set_width(2);
			break;
		}
		case CONNECTION_TYPE_SEQUENCE: {
			curve.clear_points();
			Vector2 start_pos = start->get_connection_point_bottom();
			Vector2 end_pos = end->get_connection_point_top();
			int distance_y = end_pos.y - start_pos.y;
			if (distance_y < 0) {
				distance_y = distance_y * -1;
			}
			distance_y = distance_y * 0.75;
			curve.add_point(start_pos, Vector2(0, -distance_y), Vector2(0, distance_y));
			curve.add_point(end_pos, Vector2(0, -distance_y), Vector2(0, distance_y));
			set_default_color(Color(0.9, 0.9, 0.9, 0.3));
			set_width(4);
			break;
		}
		case CONNECTION_TYPE_REFRENCE: {
			break;
		}
	}
	set_points(curve.get_baked_points());
}

void LinkConnection::set_start(LinkControler *p_source) {
	if (!p_source) {
		return;
	}
	start = p_source;
	start->connect("item_rect_changed", callable_mp(this, &LinkConnection::_update_connection));
	if (end) {
		_update_connection();
	}
}
void LinkConnection::set_end(LinkControler *p_target) {
	if (!p_target) {
		return;
	}
	end = p_target;
	end->connect("item_rect_changed", callable_mp(this, &LinkConnection::_update_connection));
	// select_virtual_method->connect("id_pressed", callable_mp(this, &MembersSection::_on_virtual_method_selected));
	if (start) {
		_update_connection();
	}
}
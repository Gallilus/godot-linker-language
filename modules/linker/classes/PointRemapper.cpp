#include "PointRemapper.h"

void PointRemapper::add_point(Vector2 p_point) {
	points.push_back(p_point);
	sizes.push_back(Vector2(0, 0));
	names.push_back("");
	ids.push_back(-1);
}

void PointRemapper::add_point(Vector2 p_point, Vector2 p_size) {
	points.push_back(p_point);
	sizes.push_back(p_size);
	names.push_back("");
	ids.push_back(-1);
}

void PointRemapper::add_point(Vector2 p_point, Vector2 p_size, String p_name) {
	points.push_back(p_point);
	sizes.push_back(p_size);
	names.push_back(p_name);
	ids.push_back(-1);
}

void PointRemapper::add_point(Vector2 p_point, Vector2 p_size, String p_name, int p_id) {
	points.push_back(p_point);
	sizes.push_back(p_size);
	names.push_back(p_name);
	ids.push_back(p_id);
}

void PointRemapper::add_point(Vector2 p_point, Vector2 p_size, int p_id) {
	points.push_back(p_point);
	sizes.push_back(p_size);
	names.push_back("");
	ids.push_back(p_id);
}

void PointRemapper::clear() {
	points.clear();
	sizes.clear();
	names.clear();
	ids.clear();
	point_map.clear();
}

void PointRemapper::remove_point(int p_index) {
	int i = ids.find(p_index);
	points.remove_at(i);
	sizes.remove_at(i);
	names.remove_at(i);
	ids.remove_at(i);
}

void PointRemapper::remove_point(String p_name) {
	int i = names.find(p_name);
	points.remove_at(i);
	sizes.remove_at(i);
	names.remove_at(i);
	ids.remove_at(i);
}

void PointRemapper::bake() {
	ERR_FAIL_COND(points.size() != sizes.size());
	ERR_FAIL_COND(points.size() != names.size());
	ERR_FAIL_COND(points.size() != ids.size());
}

PackedInt32Array PointRemapper::get_order_of_by(PackedVector2Array p_sizes, int p_axis) {
	PackedInt32Array result;
	bool inserted = false;
	for (int i = 0; i < p_sizes.size(); i++) {
		for (int j = 0; j < result.size(); j++) {
			if (p_sizes[i][p_axis] < p_sizes[result[j]][p_axis]) {
				result.insert(j, i);
				inserted = true;
				break;
			}
		}
		if (!inserted) {
			result.append(i);
		}
		inserted = false;
	}
	return result;
}

HashMap<float, int> PointRemapper::get_largest(PackedInt32Array p_order, int p_axis) {
	int orientation = p_axis == X ? 0 : 1;
	float cel_id = -1.0;
	int cel_size = 0;
	HashMap<float, int> result;

	for (int i = 0; i < p_order.size(); i++) {
		int idx = p_order[i];
		if (cel_id != points[idx][p_axis]) {
			cel_id = points[idx][p_axis];
			cel_size = sizes[idx][orientation];
		} else if (sizes[idx][orientation] > cel_size) {
			cel_size = sizes[idx][orientation];
		}
		result[cel_id] = cel_size;
	}
	return result;
}

HashMap<float, int> PointRemapper::strech_out_points(HashMap<float, int> cel_sizes, int p_space) {
	HashMap<float, int> result;
	int last_value = 0;
	for (const KeyValue<float, int> &E : cel_sizes) {
		result[E.key] = last_value;
		last_value += E.value + p_space;
	}
	return result;
}

HashMap<Vector2, Vector2> PointRemapper::get_point_map() {
	point_map.clear();
	HashMap<float, int> cel_sizes_x = get_largest(get_order_of_by(points, X), X);
	HashMap<float, int> cel_points_x = strech_out_points(cel_sizes_x, spacing.x);
	HashMap<float, int> cel_sizes_y = get_largest(get_order_of_by(points, Y), Y);
	HashMap<float, int> cel_points_y = strech_out_points(cel_sizes_y, spacing.y);
	for (const KeyValue<float, int> &E : cel_points_x) {
		for (const KeyValue<float, int> &F : cel_points_y) {
			point_map[Vector2(E.key, F.key)] = Vector2(E.value, F.value);
		}
	}
	return point_map;
}

void PointRemapper::print_size_order_by_x() {
	print_line("orderd by x ");
	print_orderd(get_order_of_by(points, X));
}

void PointRemapper::print_size_order_by_y() {
	print_line("orderd by y");
	print_orderd(get_order_of_by(points, Y));
}

void PointRemapper::print_orderd(PackedInt32Array p_order) {
	for (int i = 0; i < p_order.size(); i++) {
		print_line("Point: " + String(points[p_order[i]]) + " Size: " + String(sizes[p_order[i]]) + " " + names[p_order[i]] + " " + itos(ids[p_order[i]]));
	}
}

void PointRemapper::print_cel_sizes_x() {
	HashMap<float, int> cel_sizes = get_largest(get_order_of_by(points, X), X);
	print_cel_sizes(cel_sizes, X);
}

void PointRemapper::print_cel_sizes_y() {
	HashMap<float, int> cel_sizes = get_largest(get_order_of_by(points, Y), Y);
	print_cel_sizes(cel_sizes, Y);
}

void PointRemapper::print_cel_sizes(HashMap<float, int> p_cel_sizes, int p_axis) {
	String axis = p_axis == X ? "X" : "Y";
	print_line("Cel sizes " + axis);
	for (const KeyValue<float, int> &E : p_cel_sizes) {
		print_line("Cel: " + rtos(E.key) + " Size: " + itos(E.value));
	}
}

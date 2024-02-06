#ifndef POINT_REMAPPER_H
#define POINT_REMAPPER_H

#include "core/string/print_string.h"
#include "core/templates/hash_map.h"
#include "core/variant/variant.h"

class PointRemapper {
	struct XCompare {
		bool operator()(const Vector2 &a, const Vector2 &b) const {
			return a.x < b.x;
		}
	};

	PackedVector2Array points;
	PackedVector2Array sizes;
	PackedStringArray names;
	PackedInt32Array ids;
	HashMap<Vector2, Vector2> point_map;

protected:
public:
	const int X = 0;
	const int Y = 1;

	Vector2 spacing = Vector2(5, 5);

	void add_point(Vector2 p_point);
	void add_point(Vector2 p_point, Vector2 p_size);
	void add_point(Vector2 p_point, Vector2 p_size, String p_name);
	void add_point(Vector2 p_point, Vector2 p_size, String p_name, int p_id);
	void add_point(Vector2 p_point, Vector2 p_size, int p_id);
	void clear();
	void remove_point(int p_index);
	void remove_point(String p_name);

	void bake();

	PackedInt32Array get_order_of_by(PackedVector2Array p_sizes, int p_axis);
	HashMap<float, int> get_largest(PackedInt32Array p_order, int p_axis);
	HashMap<float, int> strech_out_points(HashMap<float, int> cel_sizes);
	HashMap<Vector2, Vector2> get_point_map();

	void print_size_order_by_x();
	void print_size_order_by_y();
	void print_orderd(PackedInt32Array p_order);
	void print_cel_sizes_x();
	void print_cel_sizes_y();
	void print_cel_sizes(HashMap<float, int> p_cel_sizes, int p_axis);

	PointRemapper() {}
	~PointRemapper() {}
};

#endif // POINT_REMAPPER_H

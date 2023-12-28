#ifndef TREE_DROPPABLE_H
#define TREE_DROPPABLE_H

#include "scene/gui/tree.h"

class TreeDroppable : public Tree {
	GDCLASS(TreeDroppable, Tree);

protected:
	static void _bind_methods() {}

public:
	virtual Variant get_drag_data(const Point2 &p_point) override;

	TreeDroppable() {}
	~TreeDroppable() {}
};

#endif // TREE_DROPPABLE_H
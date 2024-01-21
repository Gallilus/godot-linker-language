#ifndef LINK_CONNECTION_H
#define LINK_CONNECTION_H

#include "scene/2d/line_2d.h"
#include "scene/gui/control.h"
#include "scene/resources/curve.h"

class LinkConnection : public Line2D {
	GDCLASS(LinkConnection, Line2D);

protected:
	static void _bind_methods() {}

	Control *start = nullptr;
	Control *end = nullptr;
	Curve2D curve;

	void _update_connection();

public:
	void set_start(Control *p_source);
	void set_end(Control *p_target);

	LinkConnection() {}
	~LinkConnection() {}
};

#endif // LINK_CONNECTION_H
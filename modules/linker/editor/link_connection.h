#ifndef LINK_CONNECTION_H
#define LINK_CONNECTION_H

#include "scene/2d/line_2d.h"
#include "scene/gui/control.h"
#include "scene/resources/curve.h"

class LinkControler;

class LinkConnection : public Line2D {
	GDCLASS(LinkConnection, Line2D);

protected:
	static void _bind_methods() {}

	LinkControler *start = nullptr;
	LinkControler *end = nullptr;
	Curve2D curve;

	bool valid = true;
	void _update_connection();

public:
	enum ConnectionType {
		CONNECTION_TYPE_OBJECT_REF,
		CONNECTION_TYPE_SEQUENCE,
		CONNECTION_TYPE_REFRENCE,
	};

	ConnectionType connection_type;

	void set_start(LinkControler *p_source);
	void set_end(LinkControler *p_target);
	void check_validity();

	LinkConnection() {}
	~LinkConnection() {}
};

#endif // LINK_CONNECTION_H
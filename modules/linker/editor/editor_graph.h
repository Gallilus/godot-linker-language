#ifndef EDITOR_GRAPH_H
#define CLASSEDITOR_GRAPH_H_NAME_H

#include "..\language\linker_link.h"
#include "core/object/ref_counted.h"
#include <igraph.h>

class EditorGraph : public RefCounted {
	GDCLASS(EditorGraph, RefCounted);

	igraph_attribute_table_t graph_attributes;
	igraph_t graph;

	HashMap<int, Ref<LinkerLink>> links; // key: igraph vertex id

protected:
	static void _bind_methods();

	int get_vertex_id(Ref<LinkerLink> p_linker_link) const;

public:
	Error add_linker_link(Ref<LinkerLink> p_linker_link);

	HashMap<LinkerLink *, Vector2> get_linker_link_positions();

	EditorGraph();
	~EditorGraph();
};

#endif // EDITOR_GRAPH_H

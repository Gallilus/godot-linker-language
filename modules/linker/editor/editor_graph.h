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
	void clear();
	HashMap<Ref<LinkerLink>, Vector2> get_linker_link_positions();
	void add_vertex(Ref<LinkerLink> p_link);
	void add_edge(Ref<LinkerLink> p_link_from, Ref<LinkerLink> p_link_to, String p_edge_category);
	void add_pull_edge(Ref<LinkerLink> pulled_link, Ref<LinkerLink> owner_link);
	void add_sequence_edge(Ref<LinkerLink> source_link, Ref<LinkerLink> destination_link);

	EditorGraph();
	~EditorGraph();
};

#endif // EDITOR_GRAPH_H

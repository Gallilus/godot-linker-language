#include "editor_graph.h"

void EditorGraph::_bind_methods() {
}

int EditorGraph::get_vertex_id(Ref<LinkerLink> p_linker_link) const {
	if (p_linker_link == nullptr) {
		return -1;
	}
	for (const KeyValue<int, Ref<LinkerLink>> &E : links) {
		if (E.value.is_valid() && E.value->get_link_idx() == p_linker_link->get_link_idx()) {
			return E.key;
		}
	}
	return -1;
}

HashMap<Ref<LinkerLink>, PackedVector2Array> EditorGraph::get_linker_link_positions() {
	HashMap<Ref<LinkerLink>, PackedVector2Array> positions;
	igraph_matrix_t pos_matrix;
	igraph_matrix_init(&pos_matrix, 0, 0);
	igraph_t extended_graph;
	igraph_vector_int_t extd_to_orig_eids;
	igraph_vector_int_init(&extd_to_orig_eids, 0);

	igraph_layout_sugiyama(&graph, &pos_matrix, &extended_graph, &extd_to_orig_eids, NULL, 1.0, 1.0, 100, NULL);

	for (igraph_integer_t i = 0; i < igraph_matrix_nrow(&pos_matrix); i++) {
		igraph_real_t y = igraph_matrix_get(&pos_matrix, i, 0);
		igraph_real_t x = igraph_matrix_get(&pos_matrix, i, 1);
		if (links.has(i)) {
			positions[links[i]].push_back(Vector2(x, y));
		} else {
			positions[links[VECTOR(extd_to_orig_eids)[i]]].push_back(Vector2(x, y)); // create the dummy routs
		}
	}

	igraph_matrix_destroy(&pos_matrix);
	igraph_destroy(&extended_graph);
	igraph_vector_int_destroy(&extd_to_orig_eids);
	return positions;
}

void EditorGraph::add_vertex(Ref<LinkerLink> p_link) {
	if (!p_link.is_valid()) {
		return;
	}
	if (get_vertex_id(p_link) != -1) {
		return;
	}
	igraph_add_vertices(&graph, 1, NULL);
	igraph_integer_t vertex_id = igraph_vcount(&graph) - 1;
	links[vertex_id] = p_link;

	const char *category = p_link->get_graph_category().utf8();
	SETVAS(&graph, "category", vertex_id, category);
}

void EditorGraph::add_edge(Ref<LinkerLink> p_link_from, Ref<LinkerLink> p_link_to, String p_edge_category) {
	int from_vertex_id = get_vertex_id(p_link_from);
	int to_vertex_id = get_vertex_id(p_link_to);
	if (from_vertex_id == -1 || to_vertex_id == -1) {
		return;
	}
	igraph_add_edge(&graph, from_vertex_id, to_vertex_id);

	igraph_integer_t edge_id = igraph_ecount(&graph) - 1;
	const char *category = p_edge_category.utf8();
	SETEAS(&graph, "category", edge_id, category);
}

void EditorGraph::add_arg_edge(Ref<LinkerLink> arg_link, Ref<LinkerLink> owner_link) {
	add_edge(arg_link, owner_link, "edge_data");
}

void EditorGraph::add_sequence_edge(Ref<LinkerLink> object_link, Ref<LinkerLink> destination_link) {
	add_edge(object_link, destination_link, "edge_sequence");
}

EditorGraph::EditorGraph() {
	//graph_attributes = igraph_attribute_table_t();
	igraph_set_attribute_table(&igraph_cattribute_table);
	//igraph_set_attribute_table(&graph_attributes);
	//graph = igraph_t();
	//	graph_attributes.init(&graph, 0);
	igraph_error_t err = igraph_empty(&graph, 0, IGRAPH_DIRECTED);

	//igraph_attribute_table_init(&graph_attributes);
	//graph_attributes.init(&graph, 0);
}

EditorGraph::~EditorGraph() {
	igraph_destroy(&graph);
}

#include "editor_graph.h"

void EditorGraph::_bind_methods() {
}

int EditorGraph::get_vertex_id(Ref<LinkerLink> p_linker_link) const {
	if (p_linker_link == nullptr) {
		return -1;
	}
	for (const KeyValue<int, Ref<LinkerLink>> &E : links) {
		if (E.value->get_link_idx() == p_linker_link->get_link_idx()) {
			return E.key;
		}
	}
	return -1;
}

Error EditorGraph::add_linker_link(Ref<LinkerLink> p_linker_link) {
	if (p_linker_link == nullptr) {
		return ERR_INVALID_PARAMETER;
	}
	if (get_vertex_id(p_linker_link) != -1) {
		return ERR_ALREADY_EXISTS;
	}
	igraph_add_vertices(&graph, 1, NULL);
	igraph_integer_t vertex_id = igraph_vcount(&graph) - 1;
	links[vertex_id] = p_linker_link;

	const char *category = String(p_linker_link->get_category()).utf8();
	SETVAS(&graph, "category", vertex_id, category);

	return OK;
}

HashMap<LinkerLink *, Vector2> EditorGraph::get_linker_link_positions() {
	HashMap<LinkerLink *, Vector2> positions;
	igraph_matrix_t pos_matrix;
	igraph_matrix_init(&pos_matrix, 0, 0);
	igraph_layout_sugiyama(&graph, &pos_matrix, NULL, NULL, NULL, 30.0, 150.0, 100, NULL);

	for (igraph_integer_t i = 0; i < igraph_matrix_nrow(&pos_matrix); i++) {
		igraph_real_t y = igraph_matrix_get(&pos_matrix, i, 0);
		igraph_real_t x = igraph_matrix_get(&pos_matrix, i, 1);
		positions[links[i].ptr()] = Vector2(x, y);
	}

	igraph_matrix_destroy(&pos_matrix);
	return positions;
}

void EditorGraph::add_edge(Ref<LinkerLink> p_linker_link_from, Ref<LinkerLink> p_linker_link_to, String p_edge_category) {
	int from_vertex_id = get_vertex_id(p_linker_link_from);
	int to_vertex_id = get_vertex_id(p_linker_link_to);
	if (from_vertex_id == -1 || to_vertex_id == -1) {
		return;
	}
	igraph_add_edge(&graph, from_vertex_id, to_vertex_id);

	igraph_integer_t edge_id = igraph_ecount(&graph) - 1;
	const char *category = p_edge_category.utf8();
	SETEAS(&graph, "category", edge_id, category);
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

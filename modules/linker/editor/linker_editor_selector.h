#ifndef LINK_EDITOR_SELECTOR_H
#define LINK_EDITOR_SELECTOR_H

#include "../language/linker_index_call.h"
#include "../language/linker_script.h"

#include "scene/gui/dialogs.h"
// #include "scene/gui/control.h"
// #include "scene/gui/margin_container.h"

class LinkerEditorSelector : public ConfirmationDialog {
	GDCLASS(LinkerEditorSelector, ConfirmationDialog);
	Ref<LinkerScript> script;

	enum ScopeFlags {
		SCOPE_BASE = 1 << 0,
		SCOPE_INHERITERS = 1 << 1,
		SCOPE_UNRELATED = 1 << 2,
		SCOPE_RELATED = SCOPE_BASE | SCOPE_INHERITERS,
		SCOPE_ALL = SCOPE_BASE | SCOPE_INHERITERS | SCOPE_UNRELATED
	};

	enum SearchFlags {
		SEARCH_CLASSES = 1 << 0,
		SEARCH_CONSTRUCTORS = 1 << 1,
		SEARCH_METHODS = 1 << 2,
		SEARCH_OPERATORS = 1 << 3,
		SEARCH_SIGNALS = 1 << 4,
		SEARCH_CONSTANTS = 1 << 5,
		SEARCH_PROPERTIES = 1 << 6,
		SEARCH_THEME_ITEMS = 1 << 7,
		SEARCH_ALL = SEARCH_CLASSES | SEARCH_CONSTRUCTORS | SEARCH_METHODS |
				SEARCH_OPERATORS | SEARCH_SIGNALS | SEARCH_CONSTANTS |
				SEARCH_PROPERTIES | SEARCH_THEME_ITEMS,
		SEARCH_CASE_SENSITIVE = 1 << 29,
		SEARCH_SHOW_HIERARCHY = 1 << 30,
	};

protected:
	static void _bind_methods() {}
	// void _popup(Vector2 p_pos);

public:
	void set_script(Ref<LinkerScript> p_script) { script = p_script; }
	void inspect_links(const Array &p_links, const Vector2 &p_pos);

	LinkerEditorSelector();
	~LinkerEditorSelector() {}
};

#endif // LINK_EDITOR_SELECTOR_H
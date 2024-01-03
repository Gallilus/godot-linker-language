#ifndef LINKER_INDEX_GET_H
#define LINKER_INDEX_GET_H

#include "editor/editor_node.h"
#include "linker_link.h"
#include "linker_script.h"

class LinkerIndexGet : public LinkerLink {
	GDCLASS(LinkerIndexGet, LinkerLink);

private:
protected:
	static void _bind_methods();
	virtual void _set_owned_links() override {}

public:
	virtual Variant get_placeholder_value() const override;
	virtual Dictionary get_placeholder_info() const override;

	virtual bool can_drop(Ref<LinkerLink> drag_link) const override;
	virtual void drop_data(Ref<LinkerLink> dropped_link) override;
};

#endif // LINKER_INDEX_GET_H
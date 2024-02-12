#include "linker_index_call.h"

void LinkerIndexCall::_initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_stack_size) {
	LinkerIndexCallInstance *instance = static_cast<LinkerIndexCallInstance *>(link);

	instance->host = p_host;
	instance->index = index;

	instance->pull_count = pull_links.size();
	instance->push_count = push_links.size();

	for (int i = 0; i < instance->pull_count; i++) {
		LinkerLinkInstance *_link = pull_links[i]->get_instance(p_host, p_stack_size);
		if (_link) {
			instance->pull_links.push_back(_link);
		} else {
			ERR_PRINT(String(pull_links[i]->get_class_name()) + ": instance is null");
		}
	}

	for (int i = 0; i < instance->push_count; i++) {
		LinkerLinkInstance *_link = push_links[i]->get_instance(p_host, p_stack_size);
		if (_link) {
			instance->push_links.push_back(_link);
		} else {
			ERR_PRINT(String(push_links[i]->get_class_name()) + ": instance is null");
		}
	}
}

MethodInfo LinkerIndexCall::get_method_info() const {
	MethodInfo mi;
	PropertyInfo pi;
	if (!get_source().is_valid()) {
		return mi;
	}
	pi = get_source()->get_output_info();

	ClassDB::get_method_info(pi.class_name, index, &mi);
	return mi;
}

Dictionary LinkerIndexCall::get_placeholder_info() const {
	MethodInfo mi = get_method_info();
	Dictionary d;
	d["type"] = "PropertyInfo";
	d["value"] = Dictionary(mi.return_val);
	return d;
}

Variant LinkerIndexCall::get_drag_arg_data(int p_index) const {
	MethodInfo mi = get_method_info();
	Dictionary d;
	d["type"] = "PropertyInfo";
	d["value"] = Dictionary(mi.arguments[p_index]);
	return d;
}

bool LinkerIndexCall::can_drop_on_link(Ref<LinkerLink> drag_link) const {
	if (drag_link.is_null() ||
			drag_link == this) {
		return false;
	}
	if (drag_link->get_class() == "LinkerIndexGet") {
		return true;
	}
	return true; // for now allow any link
}

bool LinkerIndexCall::can_drop_on_arg(Ref<LinkerLink> drag_link) const {
	MethodInfo mi = get_method_info();
	PropertyInfo pi = mi.arguments[0];
	PropertyInfo pi2 = drag_link->get_output_info();
	if (pi2.type == Variant::NIL){
		return true;
	}
	return pi.type == pi2.type;
}

void LinkerIndexCall::drop_data_on_link(Ref<LinkerLink> dropped_link) {
	// momentarly only compatible with one argument
	set_source(dropped_link);
}

LinkerLinkInstance *LinkerIndexCall::get_instance(LinkerScriptInstance *p_host, int p_stack_size) {
	if (!link_instances.has(p_stack_size)) {
		LinkerIndexCallInstance *instance = new LinkerIndexCallInstance();
		link_instances.insert(p_stack_size, p_host->add_link_instance(instance));
		_initialize_instance(link_instances[p_stack_size], p_host, p_stack_size);
	}
	return link_instances[p_stack_size];
}

void LinkerIndexCall::remove_instance(LinkerScriptInstance *p_host, int p_stack_size) {
	if (link_instances.has(p_stack_size)) {
		p_host->remove_link_instance(link_instances[p_stack_size]);
		link_instances.erase(p_stack_size);
	}
}

////////////////////////////////////////////////////////////////////////
//////////////////////////////  INSTANCE  //////////////////////////////
////////////////////////////////////////////////////////////////////////

int LinkerIndexCallInstance::_step(StartMode p_start_mode, Callable::CallError &r_error, String &r_error_str) {
	input_args.clear();
	Vector<const Variant *> argp;
	input_args.resize(pull_count);
	argp.resize(pull_count);

	for (int i = 0; i < pull_count; i++) {
		input_args.write[i] = pull_links[i]->get_value();
		argp.write[i] = &input_args[i];
	}

	//value = host->get_owner()->callp(index, (const Variant **)argp.ptr(), argp.size(), r_error);
	if (pull_links.is_empty()) {
		print_error("there is a call without arguments"); // temp code
	} else {
		print_line(pull_links[0]->get_value()); // built in functions
	}

	if (r_error.error != Callable::CallError::CALL_OK) {
		return STEP_ERROR;
	}
	return STEP_COMPLETE;
}

LinkerIndexCallInstance::~LinkerIndexCallInstance() {
}

// if (!Engine::get_singleton()->is_editor_hint()) { //only update cache if editor exists, it's pointless otherwise
// 	return;
// }
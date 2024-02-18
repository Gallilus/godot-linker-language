#include "linker_index_call.h"

void LinkerIndexCall::_initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_stack_size) {
	LinkerIndexCallInstance *instance = static_cast<LinkerIndexCallInstance *>(link);

	instance->host = p_host;
	instance->index = index;

	instance->arg_count = arg_links.size();
	instance->push_count = push_links.size();

	if (source_link.is_valid()) {
		instance->source_link = source_link->get_instance(p_host, p_stack_size);
	}

	for (int i = 0; i < instance->arg_count; i++) {
		LinkerLinkInstance *_link = arg_links[i]->get_instance(p_host, p_stack_size);
		if (_link) {
			instance->arg_links.push_back(_link);
		} else {
			ERR_PRINT(String(arg_links[i]->get_class_name()) + ": instance is null");
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

Variant LinkerIndexCall::get_drag_data() const {
	MethodInfo mi = get_method_info();
	Dictionary drag_data;
	drag_data["type"] = "linker_link";
	drag_data["link_idx"] = get_link_idx();
	drag_data["value_type"] = "MethodInfo";
	drag_data["value"] = Dictionary(mi);
	return drag_data;
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
	if (pi2.type == Variant::NIL) {
		return true;
	}
	return pi.type == pi2.type;
}

int LinkerIndexCall::get_argument_count() const {
	return get_method_info().arguments.size();
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
	List<MethodInfo> info_list;
	MethodInfo mi;
	Object *object = nullptr;

	// Check the argument and source conditions
	if (source_link != nullptr) {
		object = Object::cast_to<Object>(source_link->get_value());
		if (!source_link->get_value().has_method(index)) {
			r_error.error = Callable::CallError::CALL_ERROR_INVALID_METHOD;
			r_error_str = "LinkerIndexCall source " + index + "not found in class " + String(object->get_class_name());
			return STEP_ERROR;
		}
		ERR_PRINT(String(object->get_class_name()));
		ClassDB::get_method_info(object->get_class_name(), index, &mi);
	} else if (host->has_method(index)) {
		object = host->get_owner();
		mi = host->get_script()->get_method_info(index);
		ERR_PRINT(String(object->get_class_name()));
	} else {
		ERR_PRINT("String(object->get_class_name())");
		r_error.error = Callable::CallError::CALL_ERROR_INVALID_METHOD;
		r_error_str = "LinkerIndexCall " + index + "no source found";
		return STEP_ERROR;
	}

	if (mi.arguments.size() != arg_count) {
		r_error.expected = mi.arguments.size();
		if (r_error.expected < arg_count) {
			r_error.error = Callable::CallError::CALL_ERROR_TOO_MANY_ARGUMENTS;
		} else {
			r_error.error = Callable::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS;
		}
		r_error_str = "LinkerBuiltinFunction " + String(index);
		return STEP_ERROR;
	}

	input_args.clear();
	Vector<const Variant *> argp;
	input_args.resize(arg_count);
	argp.resize(arg_count);

	// get the arguments
	for (int i = 0; i < arg_count; i++) {
		input_args.write[i] = arg_links[i]->get_value();
		argp.write[i] = &input_args[i];
	}
	// call the function
	object->callp(index, (const Variant **)argp.ptr(), argp.size(), r_error);

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
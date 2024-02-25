#include "linker_variant_operator.h"

void LinkerVariantOperator::_initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_stack_size) {
	LinkerVariantOperatorInstance *instance = static_cast<LinkerVariantOperatorInstance *>(link);

	instance->host = p_host;
	instance->index = index;

	instance->arg_count = arg_links.size();
	instance->push_count = push_links.size();

	if (object_link.is_valid()) {
		instance->object_link = object_link->get_instance(p_host, p_stack_size);
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

	instance->optr = optr;
}

void LinkerVariantOperator::set_operator(Variant::Operator p_optr) {
	optr = p_optr;
	set_index(String(Variant::get_operator_name(p_optr)));
}

void LinkerVariantOperator::set_index(StringName p_index) {
	for (int i = 0; i < Variant::OP_MAX; i++) {
		if (p_index == Variant::get_operator_name(Variant::Operator(i))) {
			optr = Variant::Operator(i);
			index = p_index;
			return;
		}
	}
	ERR_PRINT("Invalid operator: " + p_index);
}

Dictionary LinkerVariantOperator::get_placeholder_info() const {
	return Dictionary();
}

bool LinkerVariantOperator::can_drop_on_link(Ref<LinkerLink> drag_link) const {
	return false;
}

bool LinkerVariantOperator::can_drop_on_arg(Ref<LinkerLink> drag_link) const {
	return false;
}

int LinkerVariantOperator::get_argument_count() const {
	return 2;
}

LinkerLinkInstance *LinkerVariantOperator::get_instance(LinkerScriptInstance *p_host, int p_stack_size) {
	if (!link_instances.has(p_stack_size)) {
		LinkerVariantOperatorInstance *instance = new LinkerVariantOperatorInstance();
		link_instances.insert(p_stack_size, p_host->add_link_instance(instance));
		_initialize_instance(link_instances[p_stack_size], p_host, p_stack_size);
	}
	return link_instances[p_stack_size];
}

void LinkerVariantOperator::remove_instance(LinkerScriptInstance *p_host, int p_stack_size) {
	if (link_instances.has(p_stack_size)) {
		p_host->remove_link_instance(link_instances[p_stack_size]);
		link_instances.erase(p_stack_size);
	}
}

int LinkerVariantOperatorInstance::_step(StartMode p_start_mode, Callable::CallError &r_error, String &r_error_str) {
	bool valid;
	input_args.clear();
	input_args.resize(arg_count);

	// get the arguments
	for (int i = 0; i < arg_count; i++) {
		input_args.write[i] = arg_links[i]->get_value();
	}

	// 	if (unary) {
	// 		Variant::evaluate(op, *p_inputs[0], Variant(), *p_outputs[0], valid);
	// 	} else {
	Variant::evaluate(optr, input_args[0], input_args[1], value, valid);

	if (!valid) {
		r_error.error = Callable::CallError::CALL_ERROR_INVALID_METHOD;
		if (value.get_type() == Variant::STRING) {
			r_error_str = value;
		} else {
			// if (unary) {
			// 	r_error_str = String(Variant::get_operator_name(optr)) + ": " + RTR("Invalid argument of type:") + " " + Variant::get_type_name(input_args[0].get_type());
			// } else {
			r_error_str = String(Variant::get_operator_name(optr)) + ": " + RTR("Invalid arguments:") + " A: " + Variant::get_type_name(input_args[0].get_type()) + ", B: " + Variant::get_type_name(input_args[1].get_type());
		}
		value = Variant();
		return STEP_ERROR;
	}
	return STEP_COMPLETE;
}

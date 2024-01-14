#include "linker_script_instance.h"
#include "linker_function_instance.h"

bool LinkerScriptInstance::set(const StringName &p_name, const Variant &p_value) {
	return false;
}

bool LinkerScriptInstance::get(const StringName &p_name, Variant &r_ret) const {
	return false;
}

void LinkerScriptInstance::get_property_list(List<PropertyInfo> *p_properties) const {
}

Variant::Type LinkerScriptInstance::get_property_type(const StringName &p_name, bool *r_is_valid) const {
	return Variant::Type();
}

void LinkerScriptInstance::validate_property(PropertyInfo &p_property) const {
}

bool LinkerScriptInstance::property_can_revert(const StringName &p_name) const {
	return false;
}

bool LinkerScriptInstance::property_get_revert(const StringName &p_name, Variant &r_ret) const {
	return false;
}

void LinkerScriptInstance::get_method_list(List<MethodInfo> *p_list) const {
}

bool LinkerScriptInstance::has_method(const StringName &p_method) const {
	return false;
}

Variant LinkerScriptInstance::callp(const StringName &p_method, const Variant **p_args, int p_argcount, Callable::CallError &r_error) {
	r_error.error = Callable::CallError::CALL_OK; //ok by default

	if (!script->has_method(p_method)) {
		r_error.error = Callable::CallError::CALL_ERROR_INVALID_METHOD;
		return Variant();
	}
	int total_stack_size = 0;
	void *stack = alloca(total_stack_size);

	return _call_internal(p_method, stack, total_stack_size, 0, 0, false, r_error);
	//return Variant();
}

Variant LinkerScriptInstance::_call_internal(const StringName &p_method, void *p_stack, int p_stack_size, int p_flow_stack_pos, int p_pass, bool p_resuming_yield, Callable::CallError &r_error) {
	bool error = false;
	String error_str;
	Variant return_value;

	int current_node_id;

	while (true) {
		p_pass++;

		LinkerLinkInstance::StartMode start_mode;
		{
			if (p_resuming_yield) {
				start_mode = LinkerLinkInstance::START_MODE_RESUME_YIELD;
				p_resuming_yield = false; // Should resume only the first time.
				// } else ToDo = LinkerLinkInstance::START_MODE_CONTINUE_SEQUENCE;
			} else {
				start_mode = LinkerLinkInstance::START_MODE_BEGIN_SEQUENCE;
			}
		}
		LinkerFunctionInstance lfi = LinkerFunctionInstance();
		current_node_id = script->function_refrences[p_method]->get_link_idx();
		script->function_refrences[p_method]->initialize_instance(&lfi, this, start_mode, p_stack, p_stack_size);

		LinkerLinkInstance::StepResultMask ret; // ToDo = link->step(start_mode, r_error, error_str);

		if (r_error.error != Callable::CallError::CALL_OK) {
			// error from previus step.
			error = true;
			break;
		}

		LinkerLinkInstance::StepResultMask output = static_cast<LinkerLinkInstance::StepResultMask>(ret & LinkerLinkInstance::STEP_MASK);

		if (ret & LinkerLinkInstance::STEP_EXIT_FUNCTION_BIT) {
			return_value; // = ToDo link->get_return_value();
		}
		break;
	}
	if (error) {
		// Error
		// Function, file, line, error, explanation.
		String err_file = script->get_path();
		String err_func = p_method;
		int err_line = current_node_id; // Not a line but it works as one.

		if (r_error.error != Callable::CallError::CALL_ERROR_INVALID_METHOD || error_str.is_empty()) {
			if (!error_str.is_empty()) {
				error_str += " ";
			}

			if (r_error.error == Callable::CallError::CALL_ERROR_INVALID_ARGUMENT) {
				int errorarg = r_error.argument;
				error_str += "Cannot convert argument " + itos(errorarg + 1) + " to " + Variant::get_type_name(Variant::Type(r_error.expected)) + ".";
			} else if (r_error.error == Callable::CallError::CALL_ERROR_TOO_MANY_ARGUMENTS) {
				error_str += "Expected " + itos(r_error.argument) + " arguments.";
			} else if (r_error.error == Callable::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS) {
				error_str += "Expected " + itos(r_error.argument) + " arguments.";
			} else if (r_error.error == Callable::CallError::CALL_ERROR_INVALID_METHOD) {
				error_str += "Invalid Call.";
			} else if (r_error.error == Callable::CallError::CALL_ERROR_METHOD_NOT_CONST) {
				error_str += "Method not const in a const instance.";
			} else if (r_error.error == Callable::CallError::CALL_ERROR_INSTANCE_IS_NULL) {
				error_str += "Base Instance is null";
			}
		}

		// ToDo if (!LinkerLanguage::singleton->debug_break(error_str, false)) {
		_err_print_error(err_func.utf8().get_data(), err_file.utf8().get_data(), err_line, error_str.utf8().get_data(), false, ERR_HANDLER_SCRIPT);
	}

	return return_value;
}

void LinkerScriptInstance::initialize() {
	if (initialized) {
		return;
	}
	initialized = true;
}

LinkerScriptInstance::LinkerScriptInstance() {
	owner = nullptr;
	base_ref_counted = false;
}

LinkerScriptInstance::~LinkerScriptInstance() {
	MutexLock lock(LinkerLanguage::get_singleton()->mutex);

	if (script.is_valid() && owner) {
		script->instances.erase(owner);
	}
}

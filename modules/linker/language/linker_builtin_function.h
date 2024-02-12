#ifndef LINKER_BUILTIN_FUNCTION_H
#define LINKER_BUILTIN_FUNCTION_H

#include "editor/editor_node.h"
#include "linker_link.h"
#include "linker_script.h"

class LinkerLinkInstance;
class LinkerBuiltinFunction : public LinkerLink {
	GDCLASS(LinkerBuiltinFunction, LinkerLink);

public:
	enum BuiltinFunc {
		MATH_SIN,
		MATH_COS,
		MATH_TAN,
		MATH_SINH,
		MATH_COSH,
		MATH_TANH,
		MATH_ASIN,
		MATH_ACOS,
		MATH_ATAN,
		MATH_ATAN2,
		MATH_SQRT,
		MATH_FMOD,
		MATH_FPOSMOD,
		MATH_FLOOR,
		MATH_CEIL,
		MATH_ROUND,
		MATH_ABS,
		MATH_SIGN,
		MATH_POW,
		MATH_LOG,
		MATH_EXP,
		MATH_ISNAN,
		MATH_ISINF,
		MATH_EASE,
		MATH_STEP_DECIMALS,
		MATH_SNAPPED,
		MATH_LERP,
		MATH_CUBIC_INTERPOLATE,
		MATH_INVERSE_LERP,
		MATH_REMAP,
		MATH_MOVE_TOWARD,
		MATH_RANDOMIZE,
		MATH_RANDI,
		MATH_RANDF,
		MATH_RANDI_RANGE,
		MATH_RANDF_RANGE,
		MATH_RANDFN,
		MATH_SEED,
		MATH_RANDSEED,
		MATH_DEG_TO_RAD,
		MATH_RAD_TO_DEG,
		MATH_LINEAR_TO_DB,
		MATH_DB_TO_LINEAR,
		MATH_WRAP,
		MATH_WRAPF,
		MATH_PINGPONG,
		LOGIC_MAX,
		LOGIC_MIN,
		LOGIC_CLAMP,
		LOGIC_NEAREST_PO2,
		OBJ_WEAKREF,
		TYPE_CONVERT,
		TYPE_OF,
		TYPE_EXISTS,
		TEXT_CHAR,
		TEXT_STR,
		TEXT_PRINT,
		TEXT_PRINTERR,
		TEXT_PRINTRAW,
		TEXT_PRINT_VERBOSE,
		VAR_TO_STR,
		STR_TO_VAR,
		VAR_TO_BYTES,
		BYTES_TO_VAR,
		MATH_SMOOTHSTEP,
		MATH_POSMOD,
		MATH_LERP_ANGLE,
		TEXT_ORD,
		FUNC_MAX
	};

	static int get_func_argument_count(BuiltinFunc p_func);
	static String get_func_name(BuiltinFunc p_func);
	static void exec_func(BuiltinFunc p_func, const Variant **p_inputs, Variant *r_return, Callable::CallError &r_error, String &r_error_str);
	static BuiltinFunc find_function(const String &p_string);

private:
	static const char *func_name[FUNC_MAX];

protected:
	static void _bind_methods() {}
	virtual void _set_owned_links() override;

	virtual void _initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_stack_size) override;

public:
	static const MethodInfo _execute;
	// String name;
	// PropertyInfo return_val;
	// uint32_t flags = METHOD_FLAGS_DEFAULT;
	// int id = 0;
	// List<PropertyInfo> arguments;
	// Vector<Variant> default_arguments;
	// int return_val_metadata = 0;
	// Vector<int> arguments_metadata;

	virtual Variant get_placeholder_value() const override;
	virtual Dictionary get_placeholder_info() const override;
	virtual String get_graph_category() const override { return "graph_output"; }
	virtual bool controler_at_source() const override { return false; }

	virtual bool can_drop_on_link(Ref<LinkerLink> drag_link) const override;

	virtual LinkerLinkInstance *get_instance(LinkerScriptInstance *p_host, int p_stack_size) override;
	virtual void remove_instance(LinkerScriptInstance *p_host, int p_stack_size) override;
};

////////////////////////////////////////////////////////////////////////
//////////////////////////////  INSTANCE  //////////////////////////////
////////////////////////////////////////////////////////////////////////

class LinkerBuiltInFunctionInstance : public LinkerLinkInstance {
	friend class LinkerBuiltinFunction;

	LinkerBuiltinFunction::BuiltinFunc func;

protected:
	virtual int _step(StartMode p_start_mode, Callable::CallError &r_error, String &r_error_str) override;

public:
};

#endif // LINKER_BUILTIN_FUNCTION_H
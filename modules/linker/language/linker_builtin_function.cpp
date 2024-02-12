#include "linker_builtin_function.h"

#include "core/io/marshalls.h"
#include "core/math/math_funcs.h"
#include "core/object/class_db.h"
#include "core/object/ref_counted.h"
#include "core/os/os.h"
#include "core/variant/variant_parser.h"

void LinkerBuiltinFunction::_set_owned_links() {
}

void LinkerBuiltinFunction::_initialize_instance(LinkerLinkInstance *link, LinkerScriptInstance *p_host, int p_stack_size) {
	LinkerBuiltInFunctionInstance *instance = static_cast<LinkerBuiltInFunctionInstance *>(link);

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

	instance->func = find_function(instance->index);
}

const char *LinkerBuiltinFunction::func_name[LinkerBuiltinFunction::FUNC_MAX] = {
	"sin",
	"cos",
	"tan",
	"sinh",
	"cosh",
	"tanh",
	"asin",
	"acos",
	"atan",
	"atan2",
	"sqrt",
	"fmod",
	"fposmod",
	"floor",
	"ceil",
	"round",
	"abs",
	"sign",
	"pow",
	"log",
	"exp",
	"is_nan",
	"is_inf",
	"ease",
	"step_decimals",
	"snapped",
	"lerp",
	"cubic_interpolate",
	"inverse_lerp",
	"range_lerp",
	"move_toward",
	"randomize",
	"randi",
	"randf",
	"randi_range",
	"randf_range",
	"randfn",
	"seed",
	"rand_seed",
	"deg2rad",
	"rad2deg",
	"linear2db",
	"db2linear",
	"wrapi",
	"wrapf",
	"pingpong",
	"max",
	"min",
	"clamp",
	"nearest_po2",
	"weakref",
	"convert",
	"typeof",
	"type_exists",
	"char",
	"str",
	"print",
	"printerr",
	"printraw",
	"print_verbose",
	"var2str",
	"str2var",
	"var2bytes",
	"bytes2var",
	"smoothstep",
	"posmod",
	"lerp_angle",
	"ord",
};

int LinkerBuiltinFunction::get_func_argument_count(BuiltinFunc p_func) {
	switch (p_func) {
		case MATH_RANDOMIZE:
		case MATH_RANDI:
		case MATH_RANDF:
			return 0;
		case MATH_SIN:
		case MATH_COS:
		case MATH_TAN:
		case MATH_SINH:
		case MATH_COSH:
		case MATH_TANH:
		case MATH_ASIN:
		case MATH_ACOS:
		case MATH_ATAN:
		case MATH_SQRT:
		case MATH_FLOOR:
		case MATH_CEIL:
		case MATH_ROUND:
		case MATH_ABS:
		case MATH_SIGN:
		case MATH_LOG:
		case MATH_EXP:
		case MATH_ISNAN:
		case MATH_ISINF:
		case MATH_STEP_DECIMALS:
		case MATH_SEED:
		case MATH_RANDSEED:
		case MATH_DEG_TO_RAD:
		case MATH_RAD_TO_DEG:
		case MATH_LINEAR_TO_DB:
		case MATH_DB_TO_LINEAR:
		case LOGIC_NEAREST_PO2:
		case OBJ_WEAKREF:
		case TYPE_OF:
		case TEXT_CHAR:
		case TEXT_ORD:
		case TEXT_STR:
		case TEXT_PRINT:
		case TEXT_PRINTERR:
		case TEXT_PRINTRAW:
		case TEXT_PRINT_VERBOSE:
		case VAR_TO_STR:
		case STR_TO_VAR:
		case TYPE_EXISTS:
			return 1;
		case VAR_TO_BYTES:
		case BYTES_TO_VAR:
		case MATH_ATAN2:
		case MATH_FMOD:
		case MATH_FPOSMOD:
		case MATH_POSMOD:
		case MATH_PINGPONG:
		case MATH_POW:
		case MATH_EASE:
		case MATH_SNAPPED:
		case MATH_RANDI_RANGE:
		case MATH_RANDF_RANGE:
		case MATH_RANDFN:
		case LOGIC_MAX:
		case LOGIC_MIN:
		case TYPE_CONVERT:
			return 2;
		case MATH_LERP:
		case MATH_LERP_ANGLE:
		case MATH_INVERSE_LERP:
		case MATH_SMOOTHSTEP:
		case MATH_MOVE_TOWARD:
		case MATH_WRAP:
		case MATH_WRAPF:
		case LOGIC_CLAMP:
			return 3;
		case MATH_CUBIC_INTERPOLATE:
		case MATH_REMAP:
			return 5;
		case FUNC_MAX: {
		}
	}
	return 0;
}

String LinkerBuiltinFunction::get_func_name(BuiltinFunc p_func) {
	ERR_FAIL_INDEX_V(p_func, FUNC_MAX, String());
	return func_name[p_func];
}

#define VALIDATE_ARG_NUM(m_arg)                                           \
	if (!p_inputs[m_arg]->is_num()) {                                     \
		r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT; \
		r_error.argument = m_arg;                                         \
		r_error.expected = Variant::FLOAT;                                \
		return;                                                           \
	}

void LinkerBuiltinFunction::exec_func(BuiltinFunc p_func, const Variant **p_inputs, Variant *r_return, Callable::CallError &r_error, String &r_error_str) {
	switch (p_func) {
		case LinkerBuiltinFunction::MATH_SIN: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::sin((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_COS: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::cos((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_TAN: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::tan((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_SINH: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::sinh((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_COSH: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::cosh((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_TANH: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::tanh((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_ASIN: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::asin((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_ACOS: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::acos((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_ATAN: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::atan((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_ATAN2: {
			VALIDATE_ARG_NUM(0);
			VALIDATE_ARG_NUM(1);
			*r_return = Math::atan2((double)*p_inputs[0], (double)*p_inputs[1]);
		} break;
		case LinkerBuiltinFunction::MATH_SQRT: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::sqrt((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_FMOD: {
			VALIDATE_ARG_NUM(0);
			VALIDATE_ARG_NUM(1);
			*r_return = Math::fmod((double)*p_inputs[0], (double)*p_inputs[1]);
		} break;
		case LinkerBuiltinFunction::MATH_FPOSMOD: {
			VALIDATE_ARG_NUM(0);
			VALIDATE_ARG_NUM(1);
			*r_return = Math::fposmod((double)*p_inputs[0], (double)*p_inputs[1]);
		} break;
		case LinkerBuiltinFunction::MATH_POSMOD: {
			VALIDATE_ARG_NUM(0);
			VALIDATE_ARG_NUM(1);
			*r_return = Math::posmod((int64_t)*p_inputs[0], (int64_t)*p_inputs[1]);
		} break;
		case LinkerBuiltinFunction::MATH_FLOOR: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::floor((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_CEIL: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::ceil((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_ROUND: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::round((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_ABS: {
			if (p_inputs[0]->get_type() == Variant::INT) {
				int64_t i = *p_inputs[0];
				*r_return = ABS(i);
			} else if (p_inputs[0]->get_type() == Variant::FLOAT) {
				real_t r = *p_inputs[0];
				*r_return = Math::abs(r);
			} else {
				r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT;
				r_error.argument = 0;
				r_error.expected = Variant::FLOAT;
			}
		} break;
		case LinkerBuiltinFunction::MATH_SIGN: {
			if (p_inputs[0]->get_type() == Variant::INT) {
				int64_t i = *p_inputs[0];
				*r_return = i < 0 ? -1 : (i > 0 ? +1 : 0);
			} else if (p_inputs[0]->get_type() == Variant::FLOAT) {
				real_t r = *p_inputs[0];
				*r_return = r < 0.0 ? -1.0 : (r > 0.0 ? +1.0 : 0.0);
			} else {
				r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT;
				r_error.argument = 0;
				r_error.expected = Variant::FLOAT;
			}
		} break;
		case LinkerBuiltinFunction::MATH_POW: {
			VALIDATE_ARG_NUM(0);
			VALIDATE_ARG_NUM(1);
			*r_return = Math::pow((double)*p_inputs[0], (double)*p_inputs[1]);
		} break;
		case LinkerBuiltinFunction::MATH_LOG: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::log((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_EXP: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::exp((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_ISNAN: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::is_nan((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_ISINF: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::is_inf((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_EASE: {
			VALIDATE_ARG_NUM(0);
			VALIDATE_ARG_NUM(1);
			*r_return = Math::ease((double)*p_inputs[0], (double)*p_inputs[1]);
		} break;
		case LinkerBuiltinFunction::MATH_STEP_DECIMALS: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::step_decimals((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_SNAPPED: {
			VALIDATE_ARG_NUM(0);
			VALIDATE_ARG_NUM(1);
			*r_return = Math::snapped((double)*p_inputs[0], (double)*p_inputs[1]);
		} break;
		case LinkerBuiltinFunction::MATH_LERP: {
			VALIDATE_ARG_NUM(0);
			VALIDATE_ARG_NUM(1);
			VALIDATE_ARG_NUM(2);
			*r_return = Math::lerp((double)*p_inputs[0], (double)*p_inputs[1], (double)*p_inputs[2]);
		} break;
		case LinkerBuiltinFunction::MATH_CUBIC_INTERPOLATE: {
			VALIDATE_ARG_NUM(0);
			VALIDATE_ARG_NUM(1);
			VALIDATE_ARG_NUM(2);
			VALIDATE_ARG_NUM(3);
			VALIDATE_ARG_NUM(4);
			*r_return = Math::cubic_interpolate((double)*p_inputs[0], (double)*p_inputs[1], (double)*p_inputs[2], (double)*p_inputs[3], (double)*p_inputs[4]);
		} break;
		case LinkerBuiltinFunction::MATH_LERP_ANGLE: {
			VALIDATE_ARG_NUM(0);
			VALIDATE_ARG_NUM(1);
			VALIDATE_ARG_NUM(2);
			*r_return = Math::lerp_angle((double)*p_inputs[0], (double)*p_inputs[1], (double)*p_inputs[2]);
		} break;
		case LinkerBuiltinFunction::MATH_INVERSE_LERP: {
			VALIDATE_ARG_NUM(0);
			VALIDATE_ARG_NUM(1);
			VALIDATE_ARG_NUM(2);
			*r_return = Math::inverse_lerp((double)*p_inputs[0], (double)*p_inputs[1], (double)*p_inputs[2]);
		} break;
		case LinkerBuiltinFunction::MATH_REMAP: {
			VALIDATE_ARG_NUM(0);
			VALIDATE_ARG_NUM(1);
			VALIDATE_ARG_NUM(2);
			VALIDATE_ARG_NUM(3);
			VALIDATE_ARG_NUM(4);
			*r_return = Math::remap((double)*p_inputs[0], (double)*p_inputs[1], (double)*p_inputs[2], (double)*p_inputs[3], (double)*p_inputs[4]);
		} break;
		case LinkerBuiltinFunction::MATH_SMOOTHSTEP: {
			VALIDATE_ARG_NUM(0);
			VALIDATE_ARG_NUM(1);
			VALIDATE_ARG_NUM(2);
			*r_return = Math::smoothstep((double)*p_inputs[0], (double)*p_inputs[1], (double)*p_inputs[2]);
		} break;
		case LinkerBuiltinFunction::MATH_MOVE_TOWARD: {
			VALIDATE_ARG_NUM(0);
			VALIDATE_ARG_NUM(1);
			VALIDATE_ARG_NUM(2);
			*r_return = Math::move_toward((double)*p_inputs[0], (double)*p_inputs[1], (double)*p_inputs[2]);
		} break;
		case LinkerBuiltinFunction::MATH_RANDOMIZE: {
			Math::randomize();

		} break;
		case LinkerBuiltinFunction::MATH_RANDI: {
			*r_return = Math::rand();
		} break;
		case LinkerBuiltinFunction::MATH_RANDF: {
			*r_return = Math::randf();
		} break;
		case LinkerBuiltinFunction::MATH_RANDI_RANGE: {
			VALIDATE_ARG_NUM(0);
			VALIDATE_ARG_NUM(1);
			*r_return = Math::random((int)*p_inputs[0], (int)*p_inputs[1]);
		} break;
		case LinkerBuiltinFunction::MATH_RANDF_RANGE: {
			VALIDATE_ARG_NUM(0);
			VALIDATE_ARG_NUM(1);
			*r_return = Math::random((double)*p_inputs[0], (double)*p_inputs[1]);
		} break;
		case LinkerBuiltinFunction::MATH_RANDFN: {
			VALIDATE_ARG_NUM(0);
			VALIDATE_ARG_NUM(1);
			*r_return = Math::randfn((double)*p_inputs[0], (double)*p_inputs[1]);
		} break;
		case LinkerBuiltinFunction::MATH_SEED: {
			VALIDATE_ARG_NUM(0);
			uint64_t seed = *p_inputs[0];
			Math::seed(seed);

		} break;
		case LinkerBuiltinFunction::MATH_RANDSEED: {
			VALIDATE_ARG_NUM(0);
			uint64_t seed = *p_inputs[0];
			int ret = Math::rand_from_seed(&seed);
			Array reta;
			reta.push_back(ret);
			reta.push_back(seed);
			*r_return = reta;

		} break;
		case LinkerBuiltinFunction::MATH_DEG_TO_RAD: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::deg_to_rad((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_RAD_TO_DEG: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::rad_to_deg((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_LINEAR_TO_DB: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::linear_to_db((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_DB_TO_LINEAR: {
			VALIDATE_ARG_NUM(0);
			*r_return = Math::db_to_linear((double)*p_inputs[0]);
		} break;
		case LinkerBuiltinFunction::MATH_PINGPONG: {
			VALIDATE_ARG_NUM(0);
			VALIDATE_ARG_NUM(1);
			*r_return = Math::pingpong((double)*p_inputs[0], (double)*p_inputs[1]);
		} break;
		case LinkerBuiltinFunction::MATH_WRAP: {
			VALIDATE_ARG_NUM(0);
			VALIDATE_ARG_NUM(1);
			VALIDATE_ARG_NUM(2);
			*r_return = Math::wrapi((int64_t)*p_inputs[0], (int64_t)*p_inputs[1], (int64_t)*p_inputs[2]);
		} break;
		case LinkerBuiltinFunction::MATH_WRAPF: {
			VALIDATE_ARG_NUM(0);
			VALIDATE_ARG_NUM(1);
			VALIDATE_ARG_NUM(2);
			*r_return = Math::wrapf((double)*p_inputs[0], (double)*p_inputs[1], (double)*p_inputs[2]);
		} break;
		case LinkerBuiltinFunction::LOGIC_MAX: {
			if (p_inputs[0]->get_type() == Variant::INT && p_inputs[1]->get_type() == Variant::INT) {
				int64_t a = *p_inputs[0];
				int64_t b = *p_inputs[1];
				*r_return = MAX(a, b);
			} else {
				VALIDATE_ARG_NUM(0);
				VALIDATE_ARG_NUM(1);

				real_t a = *p_inputs[0];
				real_t b = *p_inputs[1];

				*r_return = MAX(a, b);
			}

		} break;
		case LinkerBuiltinFunction::LOGIC_MIN: {
			if (p_inputs[0]->get_type() == Variant::INT && p_inputs[1]->get_type() == Variant::INT) {
				int64_t a = *p_inputs[0];
				int64_t b = *p_inputs[1];
				*r_return = MIN(a, b);
			} else {
				VALIDATE_ARG_NUM(0);
				VALIDATE_ARG_NUM(1);

				real_t a = *p_inputs[0];
				real_t b = *p_inputs[1];

				*r_return = MIN(a, b);
			}
		} break;
		case LinkerBuiltinFunction::LOGIC_CLAMP: {
			if (p_inputs[0]->get_type() == Variant::INT && p_inputs[1]->get_type() == Variant::INT && p_inputs[2]->get_type() == Variant::INT) {
				int64_t a = *p_inputs[0];
				int64_t b = *p_inputs[1];
				int64_t c = *p_inputs[2];
				*r_return = CLAMP(a, b, c);
			} else {
				VALIDATE_ARG_NUM(0);
				VALIDATE_ARG_NUM(1);
				VALIDATE_ARG_NUM(2);

				real_t a = *p_inputs[0];
				real_t b = *p_inputs[1];
				real_t c = *p_inputs[2];

				*r_return = CLAMP(a, b, c);
			}
		} break;
		case LinkerBuiltinFunction::LOGIC_NEAREST_PO2: {
			VALIDATE_ARG_NUM(0);
			int64_t num = *p_inputs[0];
			*r_return = next_power_of_2(num);
		} break;
		case LinkerBuiltinFunction::OBJ_WEAKREF: {
			if (p_inputs[0]->get_type() != Variant::OBJECT) {
				r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT;
				r_error.argument = 0;
				r_error.expected = Variant::OBJECT;

				return;
			}

			if (p_inputs[0]->is_ref_counted()) {
				Ref<RefCounted> r = *p_inputs[0];
				if (!r.is_valid()) {
					return;
				}

				Ref<WeakRef> wref = memnew(WeakRef);
				wref->set_ref(r);
				*r_return = wref;
			} else {
				Object *obj = *p_inputs[0];
				if (!obj) {
					return;
				}
				Ref<WeakRef> wref = memnew(WeakRef);
				wref->set_obj(obj);
				*r_return = wref;
			}

		} break;
		case LinkerBuiltinFunction::TYPE_CONVERT: {
			VALIDATE_ARG_NUM(1);
			int type = *p_inputs[1];
			if (type < 0 || type >= Variant::VARIANT_MAX) {
				r_error_str = RTR("Invalid type argument to convert(), use TYPE_* constants.");
				r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT;
				r_error.argument = 0;
				r_error.expected = Variant::INT;
				return;

			} else {
				Variant::construct(Variant::Type(type), *r_return, p_inputs, 1, r_error);
			}
		} break;
		case LinkerBuiltinFunction::TYPE_OF: {
			*r_return = p_inputs[0]->get_type();

		} break;
		case LinkerBuiltinFunction::TYPE_EXISTS: {
			*r_return = ClassDB::class_exists(*p_inputs[0]);

		} break;
		case LinkerBuiltinFunction::TEXT_CHAR: {
			char32_t result[2] = { *p_inputs[0], 0 };

			*r_return = String(result);

		} break;
		case LinkerBuiltinFunction::TEXT_ORD: {
			if (p_inputs[0]->get_type() != Variant::STRING) {
				r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT;
				r_error.argument = 0;
				r_error.expected = Variant::STRING;

				return;
			}

			String str = p_inputs[0]->operator String();

			if (str.length() != 1) {
				r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT;
				r_error.argument = 0;
				r_error.expected = Variant::STRING;
				*r_return = "Expected a string of length 1 (a character).";

				return;
			}

			*r_return = str.get(0);

		} break;
		case LinkerBuiltinFunction::TEXT_STR: {
			String str = *p_inputs[0];

			*r_return = str;

		} break;
		case LinkerBuiltinFunction::TEXT_PRINT: {
			String str = *p_inputs[0];
			print_line(str);

		} break;

		case LinkerBuiltinFunction::TEXT_PRINTERR: {
			String str = *p_inputs[0];
			print_error(str);

		} break;
		case LinkerBuiltinFunction::TEXT_PRINTRAW: {
			String str = *p_inputs[0];
			OS::get_singleton()->print("%s", str.utf8().get_data());

		} break;
		case LinkerBuiltinFunction::TEXT_PRINT_VERBOSE: {
			String str = *p_inputs[0];
			print_verbose(str);
		} break;
		case LinkerBuiltinFunction::VAR_TO_STR: {
			String vars;
			VariantWriter::write_to_string(*p_inputs[0], vars);
			*r_return = vars;
		} break;
		case LinkerBuiltinFunction::STR_TO_VAR: {
			if (p_inputs[0]->get_type() != Variant::STRING) {
				r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT;
				r_error.argument = 0;
				r_error.expected = Variant::STRING;

				return;
			}

			VariantParser::StreamString ss;
			ss.s = *p_inputs[0];

			String errs;
			int line;
			Error err = VariantParser::parse(&ss, *r_return, errs, line);

			if (err != OK) {
				r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT;
				r_error.argument = 0;
				r_error.expected = Variant::STRING;
				*r_return = "Parse error at line " + itos(line) + ": " + errs;
				return;
			}

		} break;
		case LinkerBuiltinFunction::VAR_TO_BYTES: {
			if (p_inputs[1]->get_type() != Variant::BOOL) {
				r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT;
				r_error.argument = 1;
				r_error.expected = Variant::BOOL;
				return;
			}
			PackedByteArray barr;
			int len;
			bool full_objects = *p_inputs[1];
			Error err = encode_variant(*p_inputs[0], nullptr, len, full_objects);
			if (err) {
				r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT;
				r_error.argument = 0;
				r_error.expected = Variant::NIL;
				r_error_str = "Unexpected error encoding variable to bytes, likely unserializable type found (Object or RID).";
				return;
			}

			barr.resize(len);
			{
				uint8_t *w = barr.ptrw();
				encode_variant(*p_inputs[0], w, len, full_objects);
			}
			*r_return = barr;
		} break;
		case LinkerBuiltinFunction::BYTES_TO_VAR: {
			if (p_inputs[0]->get_type() != Variant::PACKED_BYTE_ARRAY) {
				r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT;
				r_error.argument = 0;
				r_error.expected = Variant::PACKED_BYTE_ARRAY;
				return;
			}
			if (p_inputs[1]->get_type() != Variant::BOOL) {
				r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT;
				r_error.argument = 1;
				r_error.expected = Variant::BOOL;
				return;
			}

			PackedByteArray varr = *p_inputs[0];
			bool allow_objects = *p_inputs[1];
			Variant ret;
			{
				const uint8_t *r = varr.ptr();
				Error err = decode_variant(ret, r, varr.size(), nullptr, allow_objects);
				if (err != OK) {
					r_error_str = RTR("Not enough bytes for decoding bytes, or invalid format.");
					r_error.error = Callable::CallError::CALL_ERROR_INVALID_ARGUMENT;
					r_error.argument = 0;
					r_error.expected = Variant::PACKED_BYTE_ARRAY;
					return;
				}
			}

			*r_return = ret;

		} break;
		default: {
		}
	}
}

LinkerBuiltinFunction::BuiltinFunc LinkerBuiltinFunction::find_function(const String &p_string) {
	for (int i = 0; i < FUNC_MAX; i++) {
		if (p_string == func_name[i]) {
			return BuiltinFunc(i);
		}
	}

	return FUNC_MAX;
}

Variant LinkerBuiltinFunction::get_placeholder_value() const {
	return Variant();
}

Dictionary LinkerBuiltinFunction::get_placeholder_info() const {
	return Dictionary();
}

bool LinkerBuiltinFunction::can_drop_on_link(Ref<LinkerLink> drag_link) const {
	return false;
}

LinkerLinkInstance *LinkerBuiltinFunction::get_instance(LinkerScriptInstance *p_host, int p_stack_size) {
	if (!link_instances.has(p_stack_size)) {
		LinkerBuiltInFunctionInstance *instance = new LinkerBuiltInFunctionInstance();
		link_instances.insert(p_stack_size, p_host->add_link_instance(instance));
		_initialize_instance(link_instances[p_stack_size], p_host, p_stack_size);
	}
	return link_instances[p_stack_size];
}

void LinkerBuiltinFunction::remove_instance(LinkerScriptInstance *p_host, int p_stack_size) {
	if (link_instances.has(p_stack_size)) {
		// need to remove the instance sublinks
		p_host->remove_link_instance(link_instances[p_stack_size]);
		link_instances.erase(p_stack_size);
	}
}

int LinkerBuiltInFunctionInstance::_step(StartMode p_start_mode, Callable::CallError &r_error, String &r_error_str) {
	if (pull_count > 0) {
		value = pull_links[0]->get_value();
	}

	const Variant **p_inputs;
	Variant **p_outputs;

	LinkerBuiltinFunction::exec_func(func, p_inputs, p_outputs[0], r_error, r_error_str);
	return STEP_COMPLETE;
}

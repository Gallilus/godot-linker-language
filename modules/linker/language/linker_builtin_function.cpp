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

	if (source_link.is_valid()) {
		instance->source_link = source_link->get_instance(p_host, p_stack_size);
	}

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

	instance->func = func;
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

void LinkerBuiltinFunction::set_index(StringName p_index) {
	if (find_function(p_index) == FUNC_MAX) {
		ERR_PRINT("Built in function " + p_index + " not found");
		return;
	}
	index = p_index;
	func = find_function(p_index);
}

Variant LinkerBuiltinFunction::get_placeholder_value() const {
	return Variant();
}

Dictionary LinkerBuiltinFunction::get_placeholder_info() const {
	return Dictionary();
}

int LinkerBuiltinFunction::get_argument_count() const {
	return get_func_argument_count(func);
}

PropertyInfo LinkerBuiltinFunction::get_input_value_port_info(int p_idx) const {
	switch (func) {
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
		case MATH_ISINF: {
			return PropertyInfo(Variant::FLOAT, "s");
		} break;
		case MATH_ATAN2: {
			if (p_idx == 0) {
				return PropertyInfo(Variant::FLOAT, "y");
			} else {
				return PropertyInfo(Variant::FLOAT, "x");
			}
		} break;
		case MATH_FMOD:
		case MATH_FPOSMOD:
		case LOGIC_MAX:
		case LOGIC_MIN: {
			if (p_idx == 0) {
				return PropertyInfo(Variant::FLOAT, "a");
			} else {
				return PropertyInfo(Variant::FLOAT, "b");
			}
		} break;
		case MATH_POSMOD: {
			if (p_idx == 0) {
				return PropertyInfo(Variant::INT, "a");
			} else {
				return PropertyInfo(Variant::INT, "b");
			}
		} break;
		case MATH_POW: {
			if (p_idx == 0) {
				return PropertyInfo(Variant::FLOAT, "base");
			} else {
				return PropertyInfo(Variant::FLOAT, "exp");
			}
		} break;
		case MATH_EASE: {
			if (p_idx == 0) {
				return PropertyInfo(Variant::FLOAT, "s");
			} else {
				return PropertyInfo(Variant::FLOAT, "curve");
			}
		} break;
		case MATH_STEP_DECIMALS: {
			return PropertyInfo(Variant::FLOAT, "step");
		} break;
		case MATH_SNAPPED: {
			if (p_idx == 0) {
				return PropertyInfo(Variant::FLOAT, "s");
			} else {
				return PropertyInfo(Variant::FLOAT, "steps");
			}
		} break;
		case MATH_LERP:
		case MATH_LERP_ANGLE:
		case MATH_INVERSE_LERP:
		case MATH_SMOOTHSTEP: {
			if (p_idx == 0) {
				return PropertyInfo(Variant::FLOAT, "from");
			} else if (p_idx == 1) {
				return PropertyInfo(Variant::FLOAT, "to");
			} else {
				return PropertyInfo(Variant::FLOAT, "weight");
			}
		} break;
		case MATH_CUBIC_INTERPOLATE: {
			if (p_idx == 0) {
				return PropertyInfo(Variant::FLOAT, "from");
			} else if (p_idx == 1) {
				return PropertyInfo(Variant::FLOAT, "to");
			} else if (p_idx == 2) {
				return PropertyInfo(Variant::FLOAT, "pre");
			} else if (p_idx == 3) {
				return PropertyInfo(Variant::FLOAT, "post");
			} else {
				return PropertyInfo(Variant::FLOAT, "weight");
			}
		} break;
		case MATH_REMAP: {
			if (p_idx == 0) {
				return PropertyInfo(Variant::FLOAT, "value");
			} else if (p_idx == 1) {
				return PropertyInfo(Variant::FLOAT, "istart");
			} else if (p_idx == 2) {
				return PropertyInfo(Variant::FLOAT, "istop");
			} else if (p_idx == 3) {
				return PropertyInfo(Variant::FLOAT, "ostart");
			} else {
				return PropertyInfo(Variant::FLOAT, "ostop");
			}
		} break;
		case MATH_MOVE_TOWARD: {
			if (p_idx == 0) {
				return PropertyInfo(Variant::FLOAT, "from");
			} else if (p_idx == 1) {
				return PropertyInfo(Variant::FLOAT, "to");
			} else {
				return PropertyInfo(Variant::FLOAT, "delta");
			}
		} break;
		case MATH_RANDOMIZE:
		case MATH_RANDI:
		case MATH_RANDF: {
		} break;
		case MATH_RANDI_RANGE: {
			if (p_idx == 0) {
				return PropertyInfo(Variant::INT, "from");
			} else {
				return PropertyInfo(Variant::INT, "to");
			}
		} break;
		case MATH_RANDF_RANGE: {
			if (p_idx == 0) {
				return PropertyInfo(Variant::FLOAT, "from");
			} else {
				return PropertyInfo(Variant::FLOAT, "to");
			}
		} break;
		case MATH_RANDFN: {
			if (p_idx == 0) {
				return PropertyInfo(Variant::FLOAT, "mean");
			} else {
				return PropertyInfo(Variant::FLOAT, "deviation");
			}
		} break;
		case MATH_SEED:
		case MATH_RANDSEED: {
			return PropertyInfo(Variant::INT, "seed");
		} break;
		case MATH_DEG_TO_RAD: {
			return PropertyInfo(Variant::FLOAT, "deg");
		} break;
		case MATH_RAD_TO_DEG: {
			return PropertyInfo(Variant::FLOAT, "rad");
		} break;
		case MATH_LINEAR_TO_DB: {
			return PropertyInfo(Variant::FLOAT, "nrg");
		} break;
		case MATH_DB_TO_LINEAR: {
			return PropertyInfo(Variant::FLOAT, "db");
		} break;
		case MATH_PINGPONG: {
			if (p_idx == 0) {
				return PropertyInfo(Variant::FLOAT, "value");
			} else {
				return PropertyInfo(Variant::FLOAT, "length");
			}
		} break;
		case MATH_WRAP: {
			if (p_idx == 0) {
				return PropertyInfo(Variant::INT, "value");
			} else if (p_idx == 1) {
				return PropertyInfo(Variant::INT, "min");
			} else {
				return PropertyInfo(Variant::INT, "max");
			}
		} break;
		case MATH_WRAPF:
		case LOGIC_CLAMP: {
			if (p_idx == 0) {
				return PropertyInfo(Variant::FLOAT, "value");
			} else if (p_idx == 1) {
				return PropertyInfo(Variant::FLOAT, "min");
			} else {
				return PropertyInfo(Variant::FLOAT, "max");
			}
		} break;
		case LOGIC_NEAREST_PO2: {
			return PropertyInfo(Variant::INT, "value");
		} break;
		case OBJ_WEAKREF: {
			return PropertyInfo(Variant::OBJECT, "source");
		} break;
		case TYPE_CONVERT: {
			if (p_idx == 0) {
				return PropertyInfo(Variant::NIL, "what");
			} else {
				return PropertyInfo(Variant::STRING, "type");
			}
		} break;
		case TYPE_OF: {
			return PropertyInfo(Variant::NIL, "what");
		} break;
		case TYPE_EXISTS: {
			return PropertyInfo(Variant::STRING, "type");
		} break;
		case TEXT_ORD: {
			return PropertyInfo(Variant::STRING, "character");
		} break;
		case TEXT_CHAR: {
			return PropertyInfo(Variant::INT, "ascii");
		} break;
		case TEXT_STR:
		case TEXT_PRINT:
		case TEXT_PRINTERR:
		case TEXT_PRINTRAW:
		case TEXT_PRINT_VERBOSE: {
			return PropertyInfo(Variant::NIL, "value");
		} break;
		case STR_TO_VAR: {
			return PropertyInfo(Variant::STRING, "string");
		} break;
		case VAR_TO_STR:
		case VAR_TO_BYTES: {
			if (p_idx == 0) {
				return PropertyInfo(Variant::NIL, "var");
			} else {
				return PropertyInfo(Variant::BOOL, "full_objects");
			}

		} break;
		case BYTES_TO_VAR: {
			if (p_idx == 0) {
				return PropertyInfo(Variant::PACKED_BYTE_ARRAY, "bytes");
			} else {
				return PropertyInfo(Variant::BOOL, "allow_objects");
			}
		} break;
		case FUNC_MAX: {
		}
	}

	return PropertyInfo();
}

bool LinkerBuiltinFunction::can_drop_on_link(Ref<LinkerLink> drag_link) const {
	return false;
}

bool LinkerBuiltinFunction::can_drop_on_source(Ref<LinkerLink> drag_link) const {
	return drag_link.is_valid();
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

LinkerBuiltinFunction::LinkerBuiltinFunction(LinkerBuiltinFunction::BuiltinFunc p_func) {
	func = p_func;
	index = func_name[func];
}

LinkerBuiltinFunction::LinkerBuiltinFunction() {
	func = MATH_SIN;
	index = func_name[func];
}

template <LinkerBuiltinFunction::BuiltinFunc func>
static Ref<LinkerLink> create_builtin_func_node(const String &p_name) {
	Ref<LinkerBuiltinFunction> link = memnew(LinkerBuiltinFunction(func));
	return link;
}

void register_visual_script_builtin_func_node() {
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/sin", create_builtin_func_node<LinkerBuiltinFunction::MATH_SIN>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/cos", create_builtin_func_node<LinkerBuiltinFunction::MATH_COS>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/tan", create_builtin_func_node<LinkerBuiltinFunction::MATH_TAN>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/sinh", create_builtin_func_node<LinkerBuiltinFunction::MATH_SINH>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/cosh", create_builtin_func_node<LinkerBuiltinFunction::MATH_COSH>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/tanh", create_builtin_func_node<LinkerBuiltinFunction::MATH_TANH>);

	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/asin", create_builtin_func_node<LinkerBuiltinFunction::MATH_ASIN>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/acos", create_builtin_func_node<LinkerBuiltinFunction::MATH_ACOS>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/atan", create_builtin_func_node<LinkerBuiltinFunction::MATH_ATAN>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/atan2", create_builtin_func_node<LinkerBuiltinFunction::MATH_ATAN2>);

	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/sqrt", create_builtin_func_node<LinkerBuiltinFunction::MATH_SQRT>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/fmod", create_builtin_func_node<LinkerBuiltinFunction::MATH_FMOD>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/fposmod", create_builtin_func_node<LinkerBuiltinFunction::MATH_FPOSMOD>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/posmod", create_builtin_func_node<LinkerBuiltinFunction::MATH_POSMOD>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/floor", create_builtin_func_node<LinkerBuiltinFunction::MATH_FLOOR>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/ceil", create_builtin_func_node<LinkerBuiltinFunction::MATH_CEIL>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/round", create_builtin_func_node<LinkerBuiltinFunction::MATH_ROUND>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/abs", create_builtin_func_node<LinkerBuiltinFunction::MATH_ABS>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/sign", create_builtin_func_node<LinkerBuiltinFunction::MATH_SIGN>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/pow", create_builtin_func_node<LinkerBuiltinFunction::MATH_POW>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/log", create_builtin_func_node<LinkerBuiltinFunction::MATH_LOG>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/exp", create_builtin_func_node<LinkerBuiltinFunction::MATH_EXP>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/isnan", create_builtin_func_node<LinkerBuiltinFunction::MATH_ISNAN>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/isinf", create_builtin_func_node<LinkerBuiltinFunction::MATH_ISINF>);

	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/ease", create_builtin_func_node<LinkerBuiltinFunction::MATH_EASE>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/step_decimals", create_builtin_func_node<LinkerBuiltinFunction::MATH_STEP_DECIMALS>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/snapped", create_builtin_func_node<LinkerBuiltinFunction::MATH_SNAPPED>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/lerp", create_builtin_func_node<LinkerBuiltinFunction::MATH_LERP>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/cubic_interpolate", create_builtin_func_node<LinkerBuiltinFunction::MATH_CUBIC_INTERPOLATE>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/lerp_angle", create_builtin_func_node<LinkerBuiltinFunction::MATH_LERP_ANGLE>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/inverse_lerp", create_builtin_func_node<LinkerBuiltinFunction::MATH_INVERSE_LERP>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/range_lerp", create_builtin_func_node<LinkerBuiltinFunction::MATH_REMAP>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/smoothstep", create_builtin_func_node<LinkerBuiltinFunction::MATH_SMOOTHSTEP>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/move_toward", create_builtin_func_node<LinkerBuiltinFunction::MATH_MOVE_TOWARD>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/randomize", create_builtin_func_node<LinkerBuiltinFunction::MATH_RANDOMIZE>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/randi", create_builtin_func_node<LinkerBuiltinFunction::MATH_RANDI>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/randf", create_builtin_func_node<LinkerBuiltinFunction::MATH_RANDF>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/randi_range", create_builtin_func_node<LinkerBuiltinFunction::MATH_RANDI_RANGE>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/randf_range", create_builtin_func_node<LinkerBuiltinFunction::MATH_RANDF_RANGE>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/randfn", create_builtin_func_node<LinkerBuiltinFunction::MATH_RANDFN>);

	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/seed", create_builtin_func_node<LinkerBuiltinFunction::MATH_SEED>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/randseed", create_builtin_func_node<LinkerBuiltinFunction::MATH_RANDSEED>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/deg2rad", create_builtin_func_node<LinkerBuiltinFunction::MATH_DEG_TO_RAD>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/rad2deg", create_builtin_func_node<LinkerBuiltinFunction::MATH_RAD_TO_DEG>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/linear2db", create_builtin_func_node<LinkerBuiltinFunction::MATH_LINEAR_TO_DB>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/db2linear", create_builtin_func_node<LinkerBuiltinFunction::MATH_DB_TO_LINEAR>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/wrapi", create_builtin_func_node<LinkerBuiltinFunction::MATH_WRAP>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/wrapf", create_builtin_func_node<LinkerBuiltinFunction::MATH_WRAPF>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/pingpong", create_builtin_func_node<LinkerBuiltinFunction::MATH_PINGPONG>);

	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/max", create_builtin_func_node<LinkerBuiltinFunction::LOGIC_MAX>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/min", create_builtin_func_node<LinkerBuiltinFunction::LOGIC_MIN>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/clamp", create_builtin_func_node<LinkerBuiltinFunction::LOGIC_CLAMP>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/nearest_po2", create_builtin_func_node<LinkerBuiltinFunction::LOGIC_NEAREST_PO2>);

	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/weakref", create_builtin_func_node<LinkerBuiltinFunction::OBJ_WEAKREF>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/convert", create_builtin_func_node<LinkerBuiltinFunction::TYPE_CONVERT>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/typeof", create_builtin_func_node<LinkerBuiltinFunction::TYPE_OF>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/type_exists", create_builtin_func_node<LinkerBuiltinFunction::TYPE_EXISTS>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/char", create_builtin_func_node<LinkerBuiltinFunction::TEXT_CHAR>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/ord", create_builtin_func_node<LinkerBuiltinFunction::TEXT_ORD>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/str", create_builtin_func_node<LinkerBuiltinFunction::TEXT_STR>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/print", create_builtin_func_node<LinkerBuiltinFunction::TEXT_PRINT>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/printerr", create_builtin_func_node<LinkerBuiltinFunction::TEXT_PRINTERR>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/printraw", create_builtin_func_node<LinkerBuiltinFunction::TEXT_PRINTRAW>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/print_verbose", create_builtin_func_node<LinkerBuiltinFunction::TEXT_PRINT_VERBOSE>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/var2str", create_builtin_func_node<LinkerBuiltinFunction::VAR_TO_STR>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/str2var", create_builtin_func_node<LinkerBuiltinFunction::STR_TO_VAR>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/var2bytes", create_builtin_func_node<LinkerBuiltinFunction::VAR_TO_BYTES>);
	LinkerLanguage::get_singleton()->add_register_func("functions/built_in/bytes2var", create_builtin_func_node<LinkerBuiltinFunction::BYTES_TO_VAR>);
}

int LinkerBuiltInFunctionInstance::_step(StartMode p_start_mode, Callable::CallError &r_error, String &r_error_str) {
	if (LinkerBuiltinFunction::get_func_argument_count(func) != pull_count) {
		r_error.expected = LinkerBuiltinFunction::get_func_argument_count(func);
		if (r_error.expected < pull_count) {
			r_error.error = Callable::CallError::CALL_ERROR_TOO_MANY_ARGUMENTS;
		} else {
			r_error.error = Callable::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS;
		}
		r_error_str = "LinkerBuiltinFunction " + String(index);
		return STEP_ERROR;
	}

	input_args.clear();
	Vector<const Variant *> argp;
	input_args.resize(pull_count);
	argp.resize(pull_count);

	for (int i = 0; i < pull_count; i++) {
		input_args.write[i] = pull_links[i]->get_value();
		argp.write[i] = &input_args[i];
	}

	// if (!Engine::get_singleton()->is_editor_hint()) {
	LinkerBuiltinFunction::exec_func(func, (const Variant **)argp.ptr(), &value, r_error, r_error_str);
	// }

	if (r_error.error != Callable::CallError::CALL_OK) {
		return STEP_ERROR;
	}
	return STEP_COMPLETE;
}
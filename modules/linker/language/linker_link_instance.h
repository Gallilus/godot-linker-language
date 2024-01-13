#ifndef LINKER_LINK_INSTANCE_H
#define LINKER_LINK_INSTANCE_H

class LinkerLinkInstance {
public:
	enum StartMode {
		START_MODE_BEGIN_SEQUENCE,
		START_MODE_CONTINUE_SEQUENCE,
		START_MODE_RESUME_YIELD
	};

	enum StepResultMask {
		STEP_SHIFT = 1 << 24,
		STEP_MASK = STEP_SHIFT - 1,
		STEP_FLAG_PUSH_STACK_BIT = STEP_SHIFT, // push bit to stack
		STEP_FLAG_GO_BACK_BIT = STEP_SHIFT << 1, // go back to previous node
		STEP_NO_ADVANCE_BIT = STEP_SHIFT << 2, // do not advance past this node
		STEP_EXIT_FUNCTION_BIT = STEP_SHIFT << 3, // return from function
		STEP_YIELD_BIT = STEP_SHIFT << 4, // yield (will find VisualScriptFunctionState state in first working memory)

		FLOW_STACK_PUSHED_BIT = 1 << 30, // in flow stack, means bit was pushed (must go back here if end of sequence)
		FLOW_STACK_MASK = FLOW_STACK_PUSHED_BIT - 1
	};

	LinkerLinkInstance();
};

#endif // LINKER_LINK_INSTANCE_H
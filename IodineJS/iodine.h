#pragma once

#ifdef LIB

#include "interpreter.hpp"

#else

class JSInterpreter;
class JSValue;

extern "C" {
	JSInterpreter* JSCreate();

	JSValue* JSEvaluate(JSInterpreter*, char*);
	
	JSValue* JSEvaluateFile(JSInterpreter* intrp, std::string filename);
}

#endif
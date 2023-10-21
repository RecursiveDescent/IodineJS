// C bindings to make usage from other languages easier

#include <iostream>
#include <fstream>

#define LIB
#include "iodine.h"

extern "C" {
	JSInterpreter* JSCreate() {
		JSInterpreter* intrp = new JSInterpreter();

		intrp->LoadGlobals();

		return intrp;
	}

	JSValue* JSEvaluate(JSInterpreter* intrp, char* str) {
		return intrp->Evaluate(string(str));
	}

	JSValue* JSEvaluateFile(JSInterpreter* intrp, string filename) {
		return intrp->EvaluateFile(filename);
	}
}
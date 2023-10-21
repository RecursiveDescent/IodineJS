#include <iostream>
#include "iodine.h"

using std::string;

int main() {
	JSInterpreter* intrp = JSCreate();

	JSEvaluateFile(intrp, "test.js");

	return 0;
}
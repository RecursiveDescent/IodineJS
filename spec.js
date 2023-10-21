// UNUSED

// Standard types available in every context.

// FIXME: NaN and Infinity here cause floating point exceptions and abort the program
// NOTE: it is UB to divide by zero in C/C++, even with floating-point numbers ;)
// this likely needs te be a simple extra branch

// https://tc39.es/ecma262/multipage/global-object.html#sec-value-properties-of-the-global-object
/* https://tc39.es/ecma262/multipage/global-object.html#sec-value-properties-of-the-global-object-infinity

Infinity = 1.0 / 0.0;
[[Writable]]: false,
[[Enumerable]]: false,
[[Configurable]]: false
*/

/* https://tc39.es/ecma262/multipage/global-object.html#sec-value-properties-of-the-global-object-nan

NaN = 0.0 / 0.0;
[[Writable]]: false,
[[Enumerable]]: false,
[[Configurable]]: false
*/

// FIXME: This isn't supposed to be a keyword
/* https://tc39.es/ecma262/multipage/global-object.html#sec-undefined

undefined = void 0;
[[Writable]]: false,
[[Enumerable]]: false,
[[Configurable]]: false
*/

// FIXME: scripts should perform lookups against the global object, if the variable identifier cannot be resolved against any local variables

/* https://tc39.es/ecma262/multipage/global-object.html#sec-globalthis

globalThis = realm.[[GlobalEnv]].[[GlobalThisValue]];
[[Writable]]: true,
[[Enumerable]]: false,
[[Configurable]]: true
*/

// https://tc39.es/ecma262/multipage/global-object.html#sec-function-properties-of-the-global-object

/* https://tc39.es/ecma262/multipage/global-object.html#sec-eval-x
// NOTE: requires extensive built-in support for indirect vs direct eval, strict mode stuff, and variable naming {eval}

function eval(x) {
	1. Assert: The execution context stack has at least two elements.
	2. Let callerContext be the second to top element of the execution context stack.
	3. Let callerRealm be callerContext's Realm.
	4. Return ? PerformEval(x, callerRealm, false, false).
}
*/

/* https://tc39.es/ecma262/multipage/global-object.html#sec-isfinite-number

TODO: depends on Object.is, NaN, ToNumber

function isFinite(number) {
	1. Let num be ? ToNumber(number).
	const num = ToNumber(number);

	2. If num is NaN, +∞𝔽, or -∞𝔽, return false.
	3. Otherwise, return true.
	return !(isNaN(number) || Object.is(Infinity, number) || Object.is(-Infinity, number));
}*/

/* https://tc39.es/ecma262/multipage/global-object.html#sec-isnan-number

function isNaN(number) {
	1. Let num be ? ToNumber(number).
	const num = ToNumber(number);

	2. If num is NaN, return true.
	3. Otherwise, return false.

	return num !== num;
}
*/

/* https://tc39.es/ecma262/multipage/global-object.html#sec-parsefloat-string

function parseFloat(string) {}
	1. Let inputString be ? ToString(string).
	const inputString = ToString(string);
	2. Let trimmedString be ! TrimString(inputString, start).
	// what does this "!" mean?
	const trimmedString = TrimString(inputString, start);
	3. If neither trimmedString nor any prefix of trimmedString satisfies the syntax of a StrDecimalLiteral (see 7.1.4.1), return NaN.
	// https://tc39.es/ecma262/multipage/abstract-operations.html#sec-tonumber-applied-to-the-string-type
	if ... {
		return NaN;
	}
	4. Let numberString be the longest prefix of trimmedString, which might be trimmedString itself, that satisfies the syntax of a StrDecimalLiteral.
	5. Let parsedNumber be ParseText(! StringToCodePoints(numberString), StrDecimalLiteral).
	6. Assert: parsedNumber is a Parse Node.
	7. Return StringNumericValue of parsedNumber.
*/

/* https://tc39.es/ecma262/multipage/global-object.html#sec-parseint-string-radix

19.2.5 parseInt ( string, radix )

The parseInt function produces an integral Number dictated by interpretation of the contents of the string argument according to the specified radix. Leading white space in string is ignored. If radix is undefined or 0, it is assumed to be 10 except when the number begins with the code unit pairs 0x or 0X, in which case a radix of 16 is assumed. If radix is 16, the number may also optionally begin with the code unit pairs 0x or 0X.

The parseInt function is the %parseInt% intrinsic object. When the parseInt function is called, the following steps are taken:

    1. Let inputString be ? ToString(string).
    2. Let S be ! TrimString(inputString, start).
    3. Let sign be 1.
    4. If S is not empty and the first code unit of S is the code unit 0x002D (HYPHEN-MINUS), set sign to -1.
    5. If S is not empty and the first code unit of S is the code unit 0x002B (PLUS SIGN) or the code unit 0x002D (HYPHEN-MINUS), remove the first code unit from S.
    6. Let R be ℝ(? ToInt32(radix)).
    7. Let stripPrefix be true.
    8. If R ≠ 0, then
        a. If R < 2 or R > 36, return NaN.
        b. If R ≠ 16, set stripPrefix to false.
    9. Else,
        a. Set R to 10.
    10. If stripPrefix is true, then
        a. If the length of S is at least 2 and the first two code units of S are either "0x" or "0X", then
            i. Remove the first two code units from S.
            ii. Set R to 16.
    11. If S contains a code unit that is not a radix-R digit, let end be the index within S of the first such code unit; otherwise, let end be the length of S.
    12. Let Z be the substring of S from 0 to end.
    13. If Z is empty, return NaN.
    14. Let mathInt be the integer value that is represented by Z in radix-R notation, using the letters A-Z and a-z for digits with values 10 through 35. (However, if R is 10 and Z contains more than 20 significant digits, every significant digit after the 20th may be replaced by a 0 digit, at the option of the implementation; and if R is not 2, 4, 8, 10, 16, or 32, then mathInt may be an implementation-approximated value representing the integer value that is represented by Z in radix-R notation.)
    15. If mathInt = 0, then
        a. If sign = -1, return -0𝔽.
        b. Return +0𝔽.
    16. Return 𝔽(sign × mathInt).
*/
// encodeURI()
// encodeURIComponent()
// decodeURI()
// decodeURIComponent()

class Number {
	constructor() {

	}

	toString() {
		return "" + this.num;
	}
}

class Array {
	constructor(size, fill) {
		if (size == null) {
			size = 0;
		}
		
		if (fill == null) {
			fill = 0;
		}

		this.length = 0;

		for (let i = 0; i < size; i = i + 1) {
			this[i] = fill;
		}
	}

	push(element) {
		this[this.length] = element;

		this.length = this.length + 1;
	}

	pop() {
		let el = this[this.length - 1];

		this[this.length - 1] = undefined;

		this.length = this.length - 1;

		return el;
	}

	toString() {
		let str = "[";

		for (let i = 0; i < this.length; i = i + 1) {
			if (typeof(this[i]) == "string") {
				str = str + '"';
			}
			
			str = str + this[i];

			if (typeof(this[i]) == "string") {
				str = str + '"';
			}

			if (i + 1 != this.length) {
				str = str + ",";
			}
		}

		return str + "]";
	}
}

class String {
	constructor() {

	}

	startsWith(str) {
		for (let i = 0; i < str.length; i = i + 1) {
			if (this[i] != str[i]) {
				return new Boolean(false);
			}
		}

		return new Boolean(true);
	}

	endsWith(str) {
		for (let i = this.length - 1; i > -1 && i > str.length - 1; i = i - 1) {
			if (this[i] != str[str.length - (this.length - i)]) {
				return false;
			}
		}

		return true;
	}

	split(sep) {
		let result = [];
		let current = "";

		for (let i = 0; i < this.length; i = i + 1) {
			if (this[i] == sep) {
				result.push(current);

				current = "";
			}
			else {
				current = current + this[i];
			}
		}

		result.push(current);

		return result;
	}

	toString() {
		return this;
	}
}

class Stream {
	constructor () {
		this.pending = [];

		this.buffer = [];

		this.listeners = { flush: [], data: [], end: [] };

		this.ended = false;

		this.block = false;
	}

	write(data) {
		this.ended = false;

		this.pending.push(data);

		for (let i = 0; i < this.listeners["data"].length; i = i + 1) {
			this.listeners["data"][i](data);
		}
	}

	flush() {
		this.buffer = this.pending;

		this.pending = [];

		for (let i = 0; i < this.listeners["flush"].length; i = i + 1) {
			this.listeners["flush"][i](this);
		}
	}

	on(event, fn) {
		this.listeners[event].push(fn);
	}

	read() {
		if (this.buffer.length == 0) {
			this.ended = true;

			for (let i = 0; i < this.listeners["end"].length; i = i + 1) {
				this.listeners["end"][i](this);
			}

			if (this.block) {
				while (this.ended) {}

				this.flush();

				return read();
			}

			return null;
		}

		let data = this.buffer[0];

		for (let i = this.buffer.length - 1; i > 0; i = i - 1) {
			this.buffer[i - 1] = this.buffer[i]
		}

		this.buffer.pop();

		return data;
	}
}

class Console {
	constructor() {

	}

	// Arg doesn't go out of scope?
	log(arg) {
		if (arg == undefined) {
			stdout.write("undefined\n");

			stdout.flush();

			return;
		}

		stdout.write(arg.toString() + "\n");

		stdout.flush();
	}
}

const console = new Console();

// Auto initialized by interpreter
// Partially static, but this is unclear since the static keyword isn't implemented yet
class Object {
	constructor() {

	}

	keys(obj) {
		return this._meta(obj)["keys"];
	}

	values(obj) {
		return this._meta(obj)["values"];
	}

	toString() {
		return JSON.stringify(this)
	}
}

class JSONLexer {
	constructor(src) {
		this.position = 0;

		this.src = src;
	}

	next() {
		let c = "" + this.src[this.position];

		this.position = this.position + 1;

		return c;
	}

	peek() {
		return "" + this.src[this.position];
	}

	eatSpace() {
		while (this.peek() == " ") {
			this.next();
		}
	}

	getString() {
		this.next();

		let str = "";

		while (this.peek() != '"') {
			str = str + this.next();
		}

		this.next();

		return str;
	}

	getValue() {
		let str = "";

		while (this.peek() != '}' && this.peek() != ',' && this.peek() != ']' && this.peek() != undefined) {
			str = str + this.next();
		}

		return str;
	}

	getObject() {
		let str = this.next();

		while (this.peek() != '}' && this.peek() != ']' && this.peek() != undefined) {
			str = str + this.next();
		}

		str = str + this.next();

		return str;
	}

	getToken(nested) {
		this.eatSpace();

		if (this.peek() == "{" && nested) {
			return { type: "Object", value: this.getObject() };
		}

		if (this.peek() == "{") {
			return { type: "Symbol", value: this.next() };
		}

		if (this.peek() == "}") {
			return { type: "Symbol", value: this.next() };
		}

		if (this.peek() == ":") {
			return { type: "Symbol", value: this.next() };
		}

		if (this.peek() == ",") {
			return { type: "Symbol", value: this.next() };
		}

		if (this.peek() == '"') {
			let str = this.getString();

			return { type: "String", value: str };
		}
		
		return { type: "Value", value: this.getValue() };
	}

	peekToken(nested) {
		let pos = this.position;

		let tmp = this.getToken(nested);

		this.position = pos;

		return tmp;
	}
}

class Boolean {
	constructor(value) {
		this._bool = value;
	}

	toString() {
		if (this._bool) {
			return "true";
		}
		else {
			return "false"
		}
	}
}

class JSON {
	constructor() {

	}

	parse(str) {
		let lex = new JSONLexer(str);

		let root = lex.getToken();

		if (root.type == "String") {
			return root.value.split('"')[1];
		}

		if (root.type == "Value") {
			return root.value;
		}

		if (root.value == "{") {
			let obj = {};
			
			let key = lex.getToken().value;
			lex.getToken();
			let value = this.parse(lex.getToken(true).value);

			obj[key] = value;

			while (lex.peekToken().value == ",") {
				lex.getToken();

				let key = lex.getToken().value;
				lex.getToken();
				let value = this.parse(lex.getToken(true).value);

				obj[key] = value;
			}

			lex.getToken();

			return obj;
		}
	}

	stringify(obj) {
		let str = "";

		if (typeof(obj) == "string") {
			return '"' + obj + '"';
		}

		if (typeof(obj) == "number") {
			return obj.toString();
		}

		if (obj.length) {
			str = str + "[";

			for (let i = 0; i < obj.length; i = i + 1) {
				str = str + JSON.stringify(obj[i]);

				if (i + 1 != obj.length) {
					str = str + ",";
				}
			}

			str = str + "]";

			return str;
		}

		str = str + "{"

		let keys = Object.keys(obj);
		let values = Object.values(obj);

		for (let i = 0; i < keys.length; i = i + 1) {
			// Parenthesis required ???
			str = (str + '"' + keys[i]) + "\": " + this.stringify(values[i]);

			if (i + 1 != keys.length) {
				str = str + ", ";
			}
		}

		str = str + "}";

		return str;
	}
}

const JSON = new JSON();


// https://tc39.es/ecma262/multipage/numbers-and-dates.html#sec-value-properties-of-the-math-object
const Math = {
	// all { [[Writable]]: false, [[Enumerable]]: false, [[Configurable]]: false }
	E: 2.7182818284590452354,
	LN10: 2.302585092994046,
	LN2: 0.6931471805599453,
	LOG10E: 0.4342944819032518,
	LOG2E: 1.4426950408889634,
	PI: 3.1415926535897932,
	SQRT1_2: 0.7071067811865476
	SQRT2: 1.4142135623730951,
	// [Symbol.toStringTag]: "Math"
	// functions
	// all { [[Writable]]: true, [[Enumerable]]: false, [[Configurable]]: false }
	abs: function abs(x) {
		// Let n be ? ToNumber(x).
		const n = x;
		// 2. If n is NaN, return NaN.
		// 3. If n is -0𝔽, return +0𝔽.
		// 4. If n is -∞𝔽, return +∞𝔽.
		// 5. If n < +0𝔽, return -n.
		if (n < 0) { return n; }
		// 6. Return n.
		return n;
	},
	acos: function acos(x) {
		// 1. Let n be ? ToNumber(x).
		const n = x;
		// 2. If n is NaN, n > 1𝔽, or n < -1𝔽, return NaN.
		if (n > 1.0 || n < -1.0) { __abort__; }
		// 3. If n is 1𝔽, return +0𝔽.
		if (n === 1.0) { return 0.0; }
		// 4. Return an implementation-approximated value representing the result of the inverse cosine of ℝ(n).
		__abort__;
	},
	acosh: function acosh(x) {
		// Let n be ? ToNumber(x).
		const n = x;
		// 2. If n is NaN or n is +∞𝔽, return n.
		// 3. If n is 1𝔽, return +0𝔽.
		if (n === 1.0) { return 0.0; }
		// 4. If n < 1𝔽, return NaN.
	    // 5. Return an implementation-approximated value representing the result of the inverse hyperbolic cosine of ℝ(n).
		__abort__;
	},
	asin: function asin(x) {
		// Let n be ? ToNumber(x).
		const n = x;
		// 2. If n is NaN, n is +0𝔽, or n is -0𝔽, return n.
		if (n === 0.0) { return n; }
		// 3. If n > 1𝔽 or n < -1𝔽, return NaN.
		// 4. Return an implementation-approximated value representing the result of the inverse sine of ℝ(n).
		__abort__;
	},
	asinh: function asinh(x) {
		// 1. Let n be ? ToNumber(x).
		const n = x;
		// 2. If n is NaN, n is +0𝔽, n is -0𝔽, n is +∞𝔽, or n is -∞𝔽, return n.
		if (n === 0.0) { return n; }
		// 3. Return an implementation-approximated value representing the result of the inverse hyperbolic sine of ℝ(n).
		__abort__;
	},
	atan: function atan(x) {},
	atanh: function atanh(x) {},
	atan2: function atan2(y, x) {},
	cbrt: function cbrt(x) {},
	ceil: function ceil(x) {},
	clz32: function clz32(x) {},
	cos: function cos(x) {},
	cosh: function cosh(x) {},
	exp: function exp(x) {},
	expm1: function expm1(x) {},
	floor: function floor(x) {},
	fround: function fround(x) {},
	hypot: function hypot([x[, y[, …]]]) {},
	imul: function imul(x, y) {},
	log: function log(x) {},
	log1p: function log1p(x) {},
	log10: function log10(x) {},
	log2: function log2(x) {},
	max: function max([x[, y[, …]]]) {},
	min: function min([x[, y[, …]]]) {},
	pow: function pow(x, y) {},
	random: function random() {},
	round: function round(x) {},
	sign: function sign(x) {},
	sin: function sin(x) {},
	sinh: function sinh(x) {},
	sqrt: function sqrt(x) {},
	tan: function tan(x) {},
	tanh: function tanh(x),
	trunc: function trunc(x) {}
};
// Standard types available in every context.

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

// This technically extends Array but it's not annotated here since extends isn't implemented yet.
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

	log(arg) {
		if (arg == undefined) {
			stdout.write("undefined\n");

			stdout.flush();

			return;
		}

		if (arg.toString != undefined) {
			stdout.write(arg.toString() + "\n");

			stdout.flush();

			return;
		}

		stdout.write(JSON.stringify(arg) + "\n");

		stdout.flush();

		return;
	}
}

const console = new Console();

// Auto initialized by interpreter
// Partially static, but this is unclear since the static keyword isn't implemented yet.
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

// TODO: Function primitive.

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
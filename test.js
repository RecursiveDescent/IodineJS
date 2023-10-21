// Recursion
function fact(n) {
    if (n == 1) {
		return 1;
	}

	return n * fact(n - 1);
}

// Validate
if (fact(6) == 720) {
	console.log("fact(6) was correct! (" + fact(6) + ")");
}
else {
	console.log("fact(6) was incorrect! (" + fact(6) + ")");
}

// For loops
for (let i = 1; i < 6; i = i + 1) {
	console.log(i);
}

// Objects and nested objects
let obj = {
	nested: {
		func: function() { console.log("Nested function called!") }
	},

	i: 32
};

// Accessing nested members
obj.nested.func();

// Arrow functions
let test = (arg => { console.log(arg) })("Arrow functions!");

// JSON
console.log(JSON.stringify(obj));

// Classes
class MyClass {
	constructor() {
		this.property = "Some value";
	}

	toString() {
		return JSON.stringify(this);
	}
}

console.log(new MyClass());

const temp = {
	a: 1
};

console.log(temp);

// inserts new keys
temp.b = 2;

console.log(temp);

console.log("A,B,C".split(","))
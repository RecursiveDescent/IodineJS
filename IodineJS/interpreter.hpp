#pragma once

#include <map>

#include "parser.hpp"

using std::map;

enum class JSPrimitive {
	Object,
	Integer,
	String,
	Function,
	Bool,
	Null,
	Undefined
};

class JSInterpreter;

class JSReference;

class JSValue {
	public:

	void* Value;
	
	map<string, JSReference*> Properties = map<string, JSReference*>();

	JSPrimitive Type;

	bool Class = false;

	bool NativeStr = false;

	int ToInteger() {
		if (Type == JSPrimitive::String) {
			try {
				return std::stoi(Cast<string>());
			}
			catch (std::invalid_argument e) {
				return 0;
			}
		}

		if (Type == JSPrimitive::Null || Type == JSPrimitive::Undefined)
			return 0;

		return int(*((int*) Value));
	}

	virtual string ToString() {
		if (Type == JSPrimitive::Integer)
			return std::to_string(ToInteger());

		if (Type == JSPrimitive::Undefined)
			return "undefined";

		if (Type == JSPrimitive::Bool)
			return ToBool() ? "true" : "false";

		if (Type == JSPrimitive::Null)
			return "null";

		if (Type == JSPrimitive::Object)
			return "[Object]";

		if (Value != nullptr)
			return string(*((string*) Value));

		string str = "";

		for (JSValue* c : ToArray()) {
			str += c->ToString();
		}

		return str;
	}

	bool ToBool();

	template <class T2>
	T2 Cast() {
		if (typeid(T2) == typeid(int) && Type != JSPrimitive::Integer)
			return T2(); // I will probably be replacing these with JS's allow everything to be converted to everything way of doing things later

		return T2(*((T2*) Value));
	}

	virtual string GetType() {
		return Type == JSPrimitive::Object ? "object" : "value";
	}

	virtual JSValue* Get() {
		return this;
	}

	virtual JSReference* Get(string key) {
		return Properties[key];
	}

	virtual void Set(string key, JSReference* value) {
		Properties[key] = value;
	}

	vector<JSValue*> ToArray();

	JSValue* operator ==(JSValue& other) {
		if (Get()->Type == JSPrimitive::Undefined)
			return new JSValue(other.Get()->Type == JSPrimitive::Undefined);
		
		if (other.Get()->Type == JSPrimitive::Undefined)
			return new JSValue(Get()->Type == JSPrimitive::Undefined);
			
		// Might be a reference to a variable
		if (Get()->Type == JSPrimitive::Integer)
		    return new JSValue(Get()->ToInteger() == other.Get()->ToInteger());
            
        if (Get()->Type == JSPrimitive::String)
            return new JSValue(Get()->ToString() == other.Get()->ToString());
            
        return new JSValue(Get()->ToBool() == other.Get()->ToBool());
	}

	JSValue* operator +(JSValue& other) {
		/*if (other.Type == JSPrimitive::String) {
			return new JSValue(Get()->ToString() + other.Get()->ToString());
		}*/

		// Might be a reference to a variable
		if (Get()->Type == JSPrimitive::Integer)
		    return new JSValue(Get()->ToInteger() + other.Get()->ToInteger());
            
        if (Get()->Type == JSPrimitive::String) {
			if (NativeStr) {
				
			}

            return new JSValue(Get()->ToString() + other.Get()->ToString());
		}
            
        return new JSValue(Get()->ToString() + other.Get()->ToString());
	}

	JSValue* operator <(JSValue& other) {
		// Might be a reference to a variable
		if (Get()->Type == JSPrimitive::Integer)
		    return new JSValue(Get()->ToInteger() < other.Get()->ToInteger());
            
        if (Get()->Type == JSPrimitive::String)
            return new JSValue(Get()->ToString() < other.Get()->ToString());
            
        return new JSValue(Get()->ToInteger() < other.Get()->ToInteger());
	}

	JSValue* operator >(JSValue& other) {
		// Might be a reference to a variable
		if (Get()->Type == JSPrimitive::Integer)
		    return new JSValue(Get()->ToInteger() > other.Get()->ToInteger());
            
        if (Get()->Type == JSPrimitive::String)
            return new JSValue(Get()->ToString() > other.Get()->ToString());
            
        return new JSValue(Get()->ToInteger() > other.Get()->ToInteger());
	}

	JSValue* operator !() {
		if (Type != JSPrimitive::Bool)
			throw Error("Cannot invert non-bool");

		return new JSValue(! ToBool());
	}

	JSValue(JSValue* other) {
		Type = other->Type;

		Properties = other->Properties;

		Class = other->Class;

		NativeStr = other->NativeStr;

		Value = other->Value;
	}

	JSValue() {
		Type = JSPrimitive::Undefined;

		Value = nullptr;
	}

	JSValue(JSInterpreter* context, JSExpr* expr);

	JSValue(int value) {
		Type = JSPrimitive::Integer;

		Value = new int[1] { value };
	}

	JSValue(string value) {
		Type = JSPrimitive::String;

		NativeStr = true;

		Value = new string(value);
	}

	JSValue(bool value) {
		Type = JSPrimitive::Bool;

		Value = new bool(value);
	}
};

typedef JSValue* (*JSNativeFunction)(JSInterpreter* intrp, vector<JSValue*> args);

class JSScope;

class JSFunction : public JSValue {
	public:

	JSInterpreter* Owner;

	JSScope* ClosureScope;

	bool Closure = false;

	vector<JSStmt*> Block = vector<JSStmt*>();
	vector<string> ArgDefs = vector<string>();

	JSNativeFunction Native = nullptr;

	JSValue* This = nullptr;

	// Moved so it can use the interpreter members
	JSValue* Call(vector<JSExpr*> args, JSReference* thisval = nullptr);

	JSValue* Call(vector<JSValue*> args, JSReference* thisval = nullptr);

	virtual string GetType() {
		return "function";
	}

	virtual string ToString();

	JSFunction(JSInterpreter* owner, JSNativeFunction native) : JSValue(-1) {
		Type = JSPrimitive::Function;

		Owner = owner;

		Native = native;
	}

	JSFunction(JSInterpreter* owner) : JSValue(-1) {
		Type = JSPrimitive::Function;
		
		Owner = owner;
	}
};

class JSReference : public JSValue {
	JSValue* RefValue;

	public:

	void Set(JSValue* value) {
		RefValue = value;

		// Cheat because C++'s protected access modifier sucks. (Accessing Value member of base class)
		Value = ((JSReference*) value)->Value;
	}

	virtual JSValue* Get() {
		return RefValue;
	}

	virtual string GetType() {
		return "reference";
	}

	JSReference* Copy() {
		return new JSReference(new JSValue(RefValue));
	}

	JSReference(JSValue* value) : JSValue(-1) {
		RefValue = value;
	}

	JSReference(int value) : JSValue(value) {}
	JSReference(string value) : JSValue(value) {} 
};

bool JSValue::ToBool() {
	if (Type == JSPrimitive::Integer)
		return ToInteger() != 0;

	if (Type == JSPrimitive::Object)
		return true;
	
	if (Type != JSPrimitive::Bool && Get("_bool") == nullptr)
		return Type != JSPrimitive::Null && Type != JSPrimitive::Undefined;
	
	if (Properties.size() == 0)
		return *((bool*) Value);
	
	return Get()->Get("_bool")->Get()->ToBool();
}

string JSFunction::ToString() {
	return "[Function " + (Properties["name"] != nullptr ? Properties["name"]->Get()->ToString() : "anonymous") + "]";
}

vector<JSValue*> JSValue::ToArray() {
	int length = Get("length")->Get()->ToInteger();

	vector<JSValue*> elements = vector<JSValue*>();

	for (int i = 0; i < length; i++) {
		elements.push_back(Get(std::to_string(i))->Get());
	}

	return elements;
}

class JSObject : public JSValue {
	public:

	map<string, JSValue*> Properties = map<string, JSValue*>();

	JSObject() : JSValue(-1) {
		
	}

	JSObject(JSInterpreter* context, JSExpr* expr);
};

class JSScope {
	public:

	JSInterpreter* Owner;

	JSScope* Parent = nullptr;

	bool Closure = false;

	bool Returned = false;

	JSObject* Bound = nullptr;

	JSValue* ReturnValue;

	map<string, JSReference*> Defs = map<string, JSReference*>();

	map<string, map<string, JSStmt*>> Types = map<string, map<string, JSStmt*>>();

	JSScope* Copy() {
		JSScope* created = new JSScope(Owner, Parent);

		for (std::pair<string, JSReference*> def : Defs) {
			created->Define(def.first, def.second);
		}

		return created;
	}

	JSObject* ToObject() {
		JSObject* obj = new JSObject();

		for (std::pair<string, JSReference*> def : Defs) {
			obj->Set(def.first, def.second);
		}

		return obj;
	}

	void Define(string id, JSReference* value) {
		Defs[id] = value;
	}

	map<string, JSStmt*> GetType(string id) {
		if (Types.find(id) == Types.end() && Parent == nullptr)
			throw Error(id + " is not defined");
		else if (Types.find(id) == Types.end())
			return Parent->GetType(id);
		
		return Types[id];
	}

	JSReference* Get(string id) {
		if (id == "this" && Defs.find("this") == Defs.end()) {
			if (Bound != nullptr)
				return new JSReference(Bound);

			if (Parent != nullptr)
				return Parent->Get("this");
			else
				return Get("null");
		}

		if (Defs.find(id) == Defs.end() && Parent == nullptr)
			throw Error(id + " is not defined");
		else if (Defs.find(id) == Defs.end())
			return Parent->Get(id);
		
		return Defs[id];
	}

	JSScope(JSInterpreter* intrp) {
		Owner = intrp;

		JSValue* null = new JSValue();

		null->Type = JSPrimitive::Null;

		Define("null", new JSReference(null));

		Define("undefined", new JSReference(null));
	}

	JSScope(JSInterpreter* intrp, JSScope* parent) {
		Owner = intrp;

		Parent = parent;

		JSValue* null = new JSValue();

		null->Type = JSPrimitive::Null;
	}
};

#define JSCALL(thisval, func, args...) ((JSFunction*) func)->Call(vector<JSValue*> { args }, thisval)

#define JSBOOL(intrp, value) intrp->Construct("Boolean", vector<JSValue*> { new JSValue(value) })

class JSInterpreter {
	public:

	JSScope* Context = new JSScope(this);

	JSValue* Add(JSValue* a, JSValue* b) {
		if (a->Type == JSPrimitive::String || b->Type == JSPrimitive::String) {
			JSValue* str = Construct("String", vector<JSExpr*>());

			str->Type = JSPrimitive::String;

			int len = (a->NativeStr || a->Type != JSPrimitive::String) ? a->ToString().length() : a->Get("length")->Get()->ToInteger();

			// Copy string
			for (int i = 0; i < len; i++) {
				if (a->NativeStr || a->Type != JSPrimitive::String) {
					string c = "";

					c += a->ToString()[i];

					str->Set(std::to_string(i), new JSReference(new JSValue(c)));

					continue;
				}

				str->Set(std::to_string(i), a->Get(std::to_string(i)));
			}

			if (b->Type != JSPrimitive::String || b->NativeStr) {
				string data = b->ToString();

				for (int i = len; i < len + data.length(); i++) {
					string c = "";

					c += data[i - len];

					str->Set(std::to_string(i), new JSReference(new JSValue(c)));
				}

				str->Set("length", new JSReference(new JSValue((int) (len + data.length()))));

				return str;
			}

			// Append
			for (int i = len; i < len + b->Get("length")->Get()->ToInteger(); i++) {
				str->Set(std::to_string(i), b->Get(std::to_string(i - len)));
			}

			str->Set("length", new JSReference(new JSValue(len + b->Get("length")->Get()->ToInteger())));

			return str;
		}

		if (a->Type == JSPrimitive::Integer && b->Type == JSPrimitive::Integer) {
			return ConstructNumber((*a + *b)->ToInteger());
		}

		return *a + *b;
	}

	JSValue* Sub(JSValue* a, JSValue* b) {
		if (a->Type == JSPrimitive::Integer && b->Type == JSPrimitive::Integer) {
			return ConstructNumber(a->ToInteger() - b->ToInteger());
		}

		return new JSValue(a->ToInteger() - b->ToInteger());
	}

	JSValue* Mul(JSValue* a, JSValue* b) {
		if (a->Type == JSPrimitive::Integer && b->Type == JSPrimitive::Integer) {
			return ConstructNumber(a->ToInteger() * b->ToInteger());
		}

		return new JSValue(a->ToInteger() * b->ToInteger());
	}

	JSValue* Div(JSValue* a, JSValue* b) {
		if (a->Type == JSPrimitive::Integer && b->Type == JSPrimitive::Integer) {
			return ConstructNumber(a->ToInteger() / b->ToInteger());
		}

		return new JSValue(a->ToInteger() / b->ToInteger());
	}

	JSValue* Assign(JSReference* a, JSValue* b) {
		if (typeid(*a) != typeid(JSReference))
			throw Error("Invalid left-hand side in assignment");

		a->Set(b);

		return b;
	}

	JSValue* GT(JSValue* a, JSValue* b) {
		return *a > *b;
	}

	JSValue* LT(JSValue* a, JSValue* b) {
		return *a < *b;
	}

	JSValue* AND(JSValue* a, JSValue* b) {
		return new JSValue(a->ToBool() && b->ToBool());
	}

	JSValue* OR(JSValue* a, JSValue* b) {
		return new JSValue(a->ToBool() || b->ToBool());
	}

	JSValue* Compare(JSValue* a, JSValue* b) {
		return *a == *b;
	}

	JSValue* Construct(string type, vector<JSExpr*> args) {
		map<string, JSStmt*> methods = Context->GetType(type);

		JSValue* obj = new JSValue();

		obj->Type = JSPrimitive::Object;

		obj->Class = true;

		for (std::pair<string, JSStmt*> method : methods) {
			obj->Set(method.first, new JSReference(EvaluateStmt(method.second)));
		}

		if (obj->Get("constructor") != nullptr)
			((JSFunction*) obj->Get("constructor")->Get())->Call(args, new JSReference(obj));
		
		return obj;
	}

	JSValue* Construct(string type, vector<JSValue*> args) {
		map<string, JSStmt*> methods = Context->GetType(type);

		JSValue* obj = new JSValue();

		obj->Type = JSPrimitive::Object;

		obj->Class = true;

		for (std::pair<string, JSStmt*> method : methods) {
			obj->Set(method.first, new JSReference(EvaluateStmt(method.second)));
		}

		if (obj->Get("constructor") != nullptr)
			((JSFunction*) obj->Get("constructor")->Get())->Call(args, new JSReference(obj));
		
		return obj;
	}

	JSValue* ConstructString(string text) {
		vector<JSExpr*> args = vector<JSExpr*>();

		args.push_back(new JSExpr(JSExprType::Value, JSToken("String", text.c_str())));
		
		JSValue* str = Construct("String", args);

		for (int i = 0; i < text.length(); i++) {
			string c = "";

			c += text[i];

			str->Set(std::to_string(i), new JSReference(new JSValue(c)));
		}

		str->Set("length", new JSReference(new JSValue((int) text.length())));

		str->Type = JSPrimitive::String;

		return str;
	}

	JSValue* ConstructNumber(int value) {
		JSValue* num = new JSValue(value);

		map<string, JSStmt*> methods = Context->GetType("Number");

		num->Class = true;

		for (std::pair<string, JSStmt*> method : methods) {
			num->Set(method.first, new JSReference(EvaluateStmt(method.second)));
		}

		vector<JSExpr*> args = vector<JSExpr*>();

		num->Set("num", new JSReference(num));

		if (num->Get("constructor") != nullptr)
			((JSFunction*) num->Get("constructor")->Get())->Call(args, new JSReference(num));
		
		return num;
	}

	JSValue* ConstructArray(vector<JSValue*> elements) {
		JSValue* arr = Construct("Array", vector<JSExpr*>());

		for (int i = 0; i < elements.size(); i++) {
			arr->Set(std::to_string(i), new JSReference(elements[i]->Get()));
		}

		arr->Set("length", new JSReference(new JSValue((int) elements.size())));
		
		return arr;
	}

	// noresolve means it cant resolve identifiers
	JSValue* EvaluateExpr(JSExpr* expr, bool noresolve = false) {
		if (expr == nullptr)
			return new JSValue();
		
		if (expr->Type == JSExprType::Value) {
			if (expr->Value.Type == "String")
				return ConstructString(expr->Value.Convert<string>());

			if (expr->Value.Type == "Integer")
				return ConstructNumber(expr->Value.Convert<int>());
		}

		if (expr->Type == JSExprType::Unary) {
			if (expr->Value.Convert<string>() == "-") {
				JSValue* val = EvaluateExpr(expr->Right);

				return Mul(val, new JSValue(-1));
			}
		}

		if (expr->Type == JSExprType::Object) {
			JSValue* obj = Construct("Object", vector<JSExpr*>());

			for (std::pair<string, JSExpr*> member : expr->Properties) {
				obj->Set(member.first, new JSReference(EvaluateExpr(member.second)->Get()));
			}

			return obj;
		}

		if (expr->Type == JSExprType::Array) {
			JSValue* arr = Construct("Array", vector<JSExpr*>());

			for (int i = 0; i < expr->Array.size(); i++) {
				arr->Set(std::to_string(i), new JSReference(EvaluateExpr(expr->Array[i])->Get()));
			}

			arr->Set("length", new JSReference(new JSValue((int) expr->Array.size())));

			return arr;
		}

		if (expr->Type == JSExprType::New) {
			return Construct(expr->Value.Convert<string>(), expr->Args);
		}

		if (expr->Type == JSExprType::Identifier) {
			if (expr->Value.Convert<string>() == "null") {
				JSValue* val = new JSValue();

				val->Type = JSPrimitive::Null;

				return val;
			}

			if (expr->Value.Convert<string>() == "undefined") {
				return new JSValue();
			}

			if (noresolve)
				return new JSValue(expr->Value.Convert<string>());

			return Context->Get(expr->Value.Convert<string>());
		}

		if (expr->Type == JSExprType::Group)
			return EvaluateExpr(expr->Group, noresolve)->Get();

		if (expr->Type == JSExprType::AnonFunction) {
			JSFunction* func = new JSFunction(this);

			func->Block = expr->Block;

			for (JSExpr* arg : expr->Args) {
				func->ArgDefs.push_back(arg->Value.Convert<string>());
			}

			func->Closure = true;

			func->ClosureScope = Context->Copy();

			return func;
		}

		if (expr->Type == JSExprType::Call) {
			JSValue* fn = EvaluateExpr(expr->Fn)->Get();

			// Needs error handling, later
			if (fn->GetType() == "function")
				return ((JSFunction*) EvaluateExpr(expr->Fn)->Get())->Call(expr->Args);
			else {
				return ((JSFunction*) ((JSReference*) EvaluateExpr(expr->Fn)->Get())->Get())->Call(expr->Args);
			}
			
			return new JSValue(-1);
		}

		if (expr->Type == JSExprType::Access) {
			JSReference* left = (JSReference*) EvaluateExpr(expr->AccessPath[0]);

			vector<JSReference*> resolvepath = vector<JSReference*>();

			resolvepath.push_back(left);

			for (int i = 1; i < expr->AccessPath.size(); i++) {
				left = left->Get()->Get(expr->AccessPath[i]->Value.Convert<string>());

				if (expr->AccessPath[i]->Type == JSExprType::Call) {
					if (left == nullptr) {
						// std::cout << "Can't call null value [" + expr->AccessPath[0]->Value.Convert<string>() + "]" << "\n";
						throw Error("Can't call null or undefined value [" + expr->AccessPath[i - 1]->Value.Convert<string>() + "." + expr->AccessPath[i]->Value.Convert<string>() + "]");
					}

					JSFunction* fn = (JSFunction*) left->Get();

					// TODO Make a function to wrap values in references because a function might return a reference to something.
					left = (JSReference*) fn->Call(expr->AccessPath[i]->Args, resolvepath[i - 1]);

					resolvepath.push_back(left);

					continue;
				}

				if (left == nullptr) {
					// Set to undefined
					if (resolvepath[i - 1]->Get()->Get(expr->AccessPath[i]->Value.Convert<string>()) == nullptr)
						resolvepath[i - 1]->Get()->Set(expr->AccessPath[i]->Value.Convert<string>(), new JSReference(new JSValue()));

					// Return reference to new property
					return resolvepath[i - 1]->Get()->Get(expr->AccessPath[i]->Value.Convert<string>());
				}

				resolvepath.push_back(left);
			}

			return left;
		}

		if (expr->Type == JSExprType::Index) {
			JSValue* left = EvaluateExpr(expr->Left)->Get();
			JSValue* prop = EvaluateExpr(expr->Right)->Get();

			if (left == nullptr)
				throw Error("Cannot read property '" + (prop == nullptr ? "undefined" : prop->ToString()) + "' of " + left->ToString());

			if (left->Get(prop->ToString()) == nullptr) {
				left->Set(prop->ToString(), new JSReference(new JSValue()));

				return left->Get(prop->ToString());
			}
			
			return left->Get(prop->ToString());
		}

		if (expr->Type == JSExprType::Let) {
			Context->Define(expr->Left->Value.Convert<string>(), new JSReference(EvaluateExpr(expr->Right)->Get()));

			return new JSValue(-1);
		}

		if (expr->Type == JSExprType::Binary) {
			if (expr->Op.Convert<string>() == "+") {
				return Add(EvaluateExpr(expr->Left)->Get(), EvaluateExpr(expr->Right)->Get());
			}

			if (expr->Op.Convert<string>() == "-") {
				return Sub(EvaluateExpr(expr->Left)->Get(), EvaluateExpr(expr->Right)->Get());
			}

			if (expr->Op.Convert<string>() == "*") {
				return Mul(EvaluateExpr(expr->Left)->Get(), EvaluateExpr(expr->Right)->Get());
			}

			if (expr->Op.Convert<string>() == "/") {
				return Div(EvaluateExpr(expr->Left)->Get(), EvaluateExpr(expr->Right)->Get());
			}

			if (expr->Op.Convert<string>() == "=") {
				return Assign((JSReference*) EvaluateExpr(expr->Left), EvaluateExpr(expr->Right)->Get());
			}

			if (expr->Op.Convert<string>() == "==") {
				return Compare((JSReference*) EvaluateExpr(expr->Left), EvaluateExpr(expr->Right)->Get());
			}

			if (expr->Op.Convert<string>() == "!=") {
				return ! *Compare((JSReference*) EvaluateExpr(expr->Left), EvaluateExpr(expr->Right)->Get());
			}

			if (expr->Op.Convert<string>() == ">") {
				return GT((JSReference*) EvaluateExpr(expr->Left), EvaluateExpr(expr->Right)->Get());
			}

			if (expr->Op.Convert<string>() == "<") {
				return LT((JSReference*) EvaluateExpr(expr->Left), EvaluateExpr(expr->Right)->Get());
			}

			if (expr->Op.Convert<string>() == "&&") {
				return AND(EvaluateExpr(expr->Left)->Get(), EvaluateExpr(expr->Right)->Get());
			}

			if (expr->Op.Convert<string>() == "||") {
				return OR(EvaluateExpr(expr->Left)->Get(), EvaluateExpr(expr->Right)->Get());
			}
		}

		return new JSValue(-1);
	}

	JSValue* EvaluateStmt(JSStmt* stmt) {
		if (stmt->Type == JSStmtType::Expression) {
			return EvaluateExpr(stmt->Expr)->Get();
		}

		if (stmt->Type == JSStmtType::Class) {
			Context->Types.emplace(stmt->Id, stmt->Class);

			return new JSValue();
		}

		if (stmt->Type == JSStmtType::Let) {
			Context->Define(stmt->Id, new JSReference(EvaluateExpr(stmt->Expr)->Get()));
		}

		if (stmt->Type == JSStmtType::Return) {
			Context->Returned = true;
			Context->ReturnValue = EvaluateExpr(stmt->Expr);

			return Context->ReturnValue;
		}

		if (stmt->Type == JSStmtType::If) {
			if (EvaluateExpr(stmt->Expr)->Get()->ToBool()) {
				for (JSStmt* blockstmt : stmt->Block) {
					EvaluateStmt(blockstmt);
				}
			}
			else if (stmt->ElseBlock.size() > 0) {
				for (JSStmt* blockstmt : stmt->ElseBlock) {
					EvaluateStmt(blockstmt);
				}
			}
		}

		if (stmt->Type == JSStmtType::While) {
			while (EvaluateExpr(stmt->Expr)->Get()->ToBool()) {
				for (JSStmt* blockstmt : stmt->Block) {
					EvaluateStmt(blockstmt);
				}
			}
		}

		if (stmt->Type == JSStmtType::For) {
			for (EvaluateExpr(stmt->Initializer); EvaluateExpr(stmt->Condition)->Get()->ToBool(); EvaluateExpr(stmt->Modifier)) {
				for (JSStmt* blockstmt : stmt->Block) {
					EvaluateStmt(blockstmt);
				}
			}
		}

		if (stmt->Type == JSStmtType::Function) {
			JSFunction* func = new JSFunction(this);

			func->Set("name", new JSReference(new JSValue(stmt->Id)));

			func->Block = stmt->Expr->Block;

			for (JSExpr* arg : stmt->Expr->Args) {
				func->ArgDefs.push_back(arg->Value.Convert<string>());
			}

			Context->Define(stmt->Id, new JSReference(func));

			return func;
		}

		return new JSValue(-1);
	}

	JSValue* Evaluate(string src) {
		JSParser prs = JSParser(src);

		JSValue* last = new JSValue(-1);

		while (! prs.Finished()) {
			delete Context->Bound;
			Context->Bound = Context->ToObject();
			
			last = EvaluateStmt(prs.Statement());
		}

		return last->Get();
	}

	JSValue* EvaluateFile(string filename) {
		string js = "";

		std::ifstream file;

		file.open(filename);

		while (file.good()) {
			js += file.get();
		}

		return Evaluate(js);
	}

	static JSValue* write(JSInterpreter* intrp, vector<JSValue*> args) {
		JSValue* stdout = args[0];

		while (! stdout->Get("ended")->Get()->ToBool()) {
			JSValue* val = JSCALL(nullptr, stdout->Get("read")->Get())->Get();

			if (val->Type == JSPrimitive::Null)
				break;
			
			std::cout << val->ToString();
		}

		return new JSValue();
	}

	static JSValue* type(JSInterpreter* intrp, vector<JSValue*> args) {
		// Potential segfault

		if (args[0]->Type == JSPrimitive::Integer)
			return intrp->ConstructString("number");

		if (args[0]->Type == JSPrimitive::String)
			return intrp->ConstructString("string");

		return intrp->ConstructString("object");
	}

	// Returns a dictionary containing meta information on an object
	static JSValue* meta(JSInterpreter* intrp, vector<JSValue*> args) {
		JSValue* val = args[0];

		JSValue* info = intrp->Construct("Object", vector<JSExpr*>());

		vector<JSValue*> keys = vector<JSValue*>();

		vector<JSValue*> values = vector<JSValue*>();

		for (std::pair<string, JSValue*> pair : val->Properties) {
			if (pair.first == "constructor" || pair.first == "toString" || pair.first == "keys" || pair.first == "values" || pair.first == "num" || pair.first == "length")
				continue;
			
			keys.push_back(intrp->ConstructString(pair.first));

			values.push_back(pair.second);
		}

		info->Set("keys", new JSReference(intrp->ConstructArray(keys)));

		info->Set("values", new JSReference(intrp->ConstructArray(values)));

		return info;
	}

	void LoadGlobals() {
		// JSValue* console = new JSValue();

		// console->Set("log", new JSReference(new JSFunction(this, log)));
		// Context->Define("console", new JSReference(console));

		Context->Define("typeof", new JSReference(new JSFunction(this, type)));

		Context->Define("true", new JSReference(new JSValue(true)));
		Context->Define("false", new JSReference(new JSValue(false)));

		EvaluateFile("global.js");

		JSValue* trueobj = JSBOOL(this, true);
		JSValue* falseobj = JSBOOL(this, false);

		trueobj->Type = JSPrimitive::Bool;
		falseobj->Type = JSPrimitive::Bool;

		Context->Define("true", new JSReference(trueobj));
		Context->Define("false", new JSReference(falseobj));

		JSValue* object = Construct("Object", vector<JSExpr*>());

		object->Set("_meta", new JSReference(new JSFunction(this, meta)));

		Context->Define("Object", new JSReference(object));

		JSValue* stdout = Construct("Stream", vector<JSExpr*>());

		vector<JSValue*> args = vector<JSValue*>();

		args.push_back(ConstructString("flush"));
		args.push_back(new JSFunction(this, write));

		((JSFunction*) stdout->Get("on")->Get())->Call(args, new JSReference(stdout));

		Context->Define("stdout", new JSReference(stdout));
	}
};

JSValue* JSFunction::Call(vector<JSExpr*> args, JSReference* thisval) {
	if (this->Native != nullptr) {
		vector<JSValue*> values = vector<JSValue*>();

		for (int i = 0; i < args.size(); i++) {
			values.push_back(Owner->EvaluateExpr(args[i])->Get());
		}

		Owner->Context = new JSScope(Owner, Owner->Context);

		if (thisval != nullptr)
			Owner->Context->Define("this", thisval);
		
		JSValue* ret = this->Native(Owner, values);

		Owner->Context = Owner->Context->Parent;

		return ret;
	}

	if (Closure) {
		ClosureScope->Closure = true;

		ClosureScope->Parent = Owner->Context;

		Owner->Context = ClosureScope;
	}
	else {
		Owner->Context = new JSScope(Owner, Owner->Context);
	}

	int argc = 0;

	for (; argc < args.size(); argc++) {
		if (argc >= ArgDefs.size())
			break;
		
		Owner->Context->Define(ArgDefs[argc], new JSReference(Owner->EvaluateExpr(args[argc])->Get()));
	}

	for (int i = argc; i < ArgDefs.size(); i++) {
		Owner->Context->Define(ArgDefs[i], new JSReference(new JSValue()));
	}

	if (thisval != nullptr)
		Owner->Context->Define("this", thisval);
		
	for (JSStmt* stmt : Block) {
		if (Owner->Context->Returned)
			break;
		
		Owner->EvaluateStmt(stmt);
	}

	JSValue* value = Owner->Context->ReturnValue;

	if (value == nullptr)
		value = new JSValue();

	Owner->Context = Owner->Context->Parent;

	//if (Closure)
		//Owner->Context = Owner->Context->Parent;

	return value;
}

JSValue* JSFunction::Call(vector<JSValue*> args, JSReference* thisval) {
	if (this->Native != nullptr) {
		Owner->Context = new JSScope(Owner, Owner->Context);

		if (thisval != nullptr)
			Owner->Context->Define("this", thisval);
		
		JSValue* ret = this->Native(Owner, args);

		Owner->Context = Owner->Context->Parent;

		return ret;
	}

	if (Closure) {
		ClosureScope->Closure = true;

		ClosureScope->Parent = Owner->Context;

		Owner->Context = ClosureScope; // new JSScope(Owner, ClosureScope);
	}
	else {
		Owner->Context = new JSScope(Owner, Owner->Context);
	}

	int argc = 0;

	for (; argc < args.size(); argc++) {
		if (argc >= ArgDefs.size())
			break;
		
		Owner->Context->Define(ArgDefs[argc], new JSReference(args[argc]->Get()));
	}

	for (int i = argc; i < ArgDefs.size(); i++) {
		Owner->Context->Define(ArgDefs[i], new JSReference(new JSValue()));
	}

	if (thisval != nullptr)
		Owner->Context->Define("this", thisval);
		
	for (JSStmt* stmt : Block) {
		if (Owner->Context->Returned)
			break;
		
		Owner->EvaluateStmt(stmt);
	}

	JSValue* value = Owner->Context->ReturnValue;

	if (value == nullptr)
		value = new JSValue();

	Owner->Context = Owner->Context->Parent;

	// if (Closure)
		// Owner->Context = Owner->Context->Parent;

	return value;
}

JSValue::JSValue(JSInterpreter* context, JSExpr* expr) {
	Type = JSPrimitive::Object;

	Value = nullptr;

	for (std::pair<string, JSExpr*> member : expr->Properties) {
		Properties[member.first] = new JSReference(context->EvaluateExpr(member.second)->Get());
	}
}
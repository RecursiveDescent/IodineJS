#include <vector>
#include <map>

#include "lexer.hpp"

using std::vector;
using std::map;

enum class JSExprType {
	Identifier,
	Value,
	Object,
	Array,
	New,
	Unary,
	Binary,
	Group,
	Let,
	Call,
	Access,
	Index,
	AnonFunction,
	Function
};

enum class JSStmtType {
	Expression,
	Let,
	Class,
	If,
	While,
	For,
	Function,
	Return
};

class JSStmt;

class JSExpr {
	public:

	JSExprType Type;

	JSExpr* Left;
	JSToken Op;
	JSExpr* Right;

	JSExpr* Group;

	JSExpr* Fn;
	vector<JSExpr*> Args;

	vector<JSExpr*> Array;

	vector<JSStmt*> Block;

	vector<JSExpr*> AccessPath = vector<JSExpr*>();

	map<string, JSExpr*> Properties;

	JSToken Value;

	JSExpr(JSExprType type, JSToken value) : Op("null", nullptr), Value(value) {
		Type = type;
	}
};

class JSStmt {
	public:

	JSStmtType Type;

	string Id;

	// For loop
	JSExpr* Initializer;
	JSExpr* Condition;
	JSExpr* Modifier;

	JSExpr* Expr;

	map<string, JSStmt*> Class;

	vector<JSStmt*> Block;

	vector<JSStmt*> ElseBlock;

	JSStmt() {}
};

class JSParser {
	JSLexer Lex;

	public:

	bool Finished() {
		return Lex.AtEnd();
	}

	vector<JSExpr*> Args(bool noerr = false) {
		vector<JSExpr*> args = vector<JSExpr*>();

		if (Lex.PeekJSToken().Type != "LParen") {
			if (noerr)
				return vector<JSExpr*>();
			
			throw Error(Lex.PositionStr() + " Expected (");
		}

		Lex.GetJSToken();

		while (Lex.PeekJSToken().Type != "RParen") {
			args.push_back(Expression());

			if (Lex.PeekJSToken().Type == "RParen")
				break;

			// Maybe improve this later?
			if (Lex.PeekJSToken().Type != "Comma" && Lex.PeekJSToken(2).Type != "RParen")
				throw Error(Lex.PositionStr() + " missing ) after argument list");

			Lex.GetJSToken();
		}

		Lex.GetJSToken();

		return args;
	}

	vector<JSStmt*> Block() {
		if (Lex.PeekJSToken().Type != "LCurly")
			throw Error(Lex.PositionStr() + " Expected {");

		Lex.GetJSToken();

		vector<JSStmt*> block = vector<JSStmt*>();

		while (Lex.PeekJSToken().Type != "RCurly") {
			block.push_back(Statement());
		}

		Lex.GetJSToken();

		return block;
	}

	map<string, JSExpr*> Object() {
		if (Lex.PeekJSToken().Type != "LCurly")
			throw Error("Expected {");

		Lex.GetJSToken();

		map<string, JSExpr*> values = map<string, JSExpr*>();

		while (Lex.PeekJSToken().Type != "RCurly") {
			if (Lex.PeekJSToken().Type == "EOF")
				throw Error(Lex.PositionStr() + " Expected }");
			
			Lex.SkipLines();
			
			JSToken key = Lex.GetJSToken();

			if (key.Type != "Ident")
				throw Error(Lex.PositionStr() + " Expected identifier");

			if (Lex.GetJSToken().Type != "Colon")
				throw Error(Lex.PositionStr() + " Expected :");

			values[key.Convert<string>()] = Expression();

			Lex.SkipLines();

			if (Lex.PeekJSToken().Type != "Comma" && Lex.PeekJSToken().Type != "RCurly")
				throw Error(Lex.PositionStr() + " Expected : or }");

			if (Lex.PeekJSToken().Type == "Comma")
				Lex.GetJSToken();
		}

		Lex.GetJSToken();

		return values;
	}

	// Returns a map of class methods
	map<string, JSStmt*> Class() {
		map<string, JSStmt*> methods = map<string, JSStmt*>();

		if (Lex.GetJSToken().Type != "LCurly")
			throw Error("Expected {");

		while (Lex.PeekJSToken().Type != "RCurly") {
			/*while (Lex.PeekJSToken().Type == "Newline") {
				Lex.GetJSToken();

				continue;
			}*/
			
			if (Lex.PeekJSToken().Type == "EOF")
				throw Error("Expected }");

			// std::cout << Lex.Position << "\n";

			JSToken name = Lex.GetJSToken();

			if (name.Type != "Ident")
				throw Error("Expected identifier");

			JSStmt* func = new JSStmt();

			func->Id = name.Convert<string>();

			JSExpr* result = new JSExpr(JSExprType::Function, JSToken("null", nullptr));

			result->Args = Args();

			result->Block = Block();

			func->Type = JSStmtType::Function;

			func->Expr = result;

			methods.emplace(func->Id, func);
		}

		Lex.GetJSToken();

		return methods;
	}

	JSStmt* Statement() {
		JSStmt* stmt = new JSStmt();
		
		if (Lex.PeekJSToken().Compare<string>("Ident", "class")) {
			Lex.GetJSToken();

			if (Lex.PeekJSToken().Type != "Ident")
				throw Error("Expected identifier");

			stmt->Type = JSStmtType::Class;

			stmt->Id = Lex.GetJSToken().Convert<string>();

			stmt->Class = Class();

			if (Lex.PeekJSToken().Type == "Semicolon")
				Lex.GetJSToken();

			return stmt;
		}

		if (Lex.PeekJSToken().Compare<string>("Ident", "let") || Lex.PeekJSToken().Compare<string>("Ident", "const")) {
			Lex.GetJSToken();

			stmt->Type = JSStmtType::Let;

			stmt->Id = Lex.GetJSToken().Convert<string>();

			// Default to undefined later.
			// Segmentation fault if no definition provided.
			if (Lex.PeekJSToken().Compare<string>("Op", "=")) {
				Lex.GetJSToken();

				if (Lex.PeekJSToken().Type == "EOF")
					throw Error("Expected expression");

				stmt->Expr = Expression();
			}

			if (Lex.PeekJSToken().Type == "Semicolon")
				Lex.GetJSToken();

			return stmt;
		}

		if (Lex.PeekJSToken().Compare<string>("Ident", "return")) {
			Lex.GetJSToken();

			stmt->Type = JSStmtType::Return;

			// Check for RCurly instead?
			if (Lex.PeekJSToken(1, true).Type != "Newline" && Lex.PeekJSToken().Type != "Semicolon") {
				stmt->Expr = Expression();
			}
			else
				stmt->Expr = new JSExpr(JSExprType::Identifier, JSToken::Construct_String("Ident", "undefined"));

			if (Lex.PeekJSToken().Type == "Semicolon")
				Lex.GetJSToken();
			
			return stmt;
		}

		if (Lex.PeekJSToken().Compare<string>("Ident", "if")) {
			Lex.GetJSToken();

			stmt->Type = JSStmtType::If;

			Lex.GetJSToken(); // Error here if no (

			stmt->Expr = Expression();

			Lex.GetJSToken(); // Error here too.

			stmt->Block = Block();

			Lex.SkipLines();

			if (Lex.PeekJSToken().Compare<string>("Ident", "else")) {
				Lex.GetJSToken();

				stmt->ElseBlock = Block();
			}

			if (Lex.PeekJSToken().Type == "Semicolon")
				Lex.GetJSToken();

			return stmt;
		}

		if (Lex.PeekJSToken().Compare<string>("Ident", "while")) {
			Lex.GetJSToken();

			stmt->Type = JSStmtType::While;

			Lex.GetJSToken(); // Error here if no (

			stmt->Expr = Expression();

			Lex.GetJSToken(); // Error here too.

			stmt->Block = Block();

			if (Lex.PeekJSToken().Type == "Semicolon")
				Lex.GetJSToken();

			return stmt;
		}

		if (Lex.PeekJSToken().Compare<string>("Ident", "for")) {
			Lex.GetJSToken();

			stmt->Type = JSStmtType::For;

			Lex.GetJSToken(); // Error here if no (

			if (Lex.PeekJSToken().Type != "Semicolon")
				stmt->Initializer = Expression();
			else
				stmt->Initializer = nullptr;

			Lex.GetJSToken(); // Error here if no ;

			if (Lex.PeekJSToken().Type != "Semicolon")
				stmt->Condition = Expression();
			else
				stmt->Condition = nullptr;

			Lex.GetJSToken(); // Error here if no ;

			if (Lex.PeekJSToken().Type != "RParen")
				stmt->Modifier = Expression();
			else
				stmt->Modifier = nullptr;

			Lex.GetJSToken(); // Error here too.

			stmt->Block = Block();

			if (Lex.PeekJSToken().Type == "Semicolon")
				Lex.GetJSToken();

			return stmt;
		}

		if (Lex.PeekJSToken().Compare<string>("Ident", "function")) {
			Lex.GetJSToken();

			stmt->Id = Lex.GetJSToken().Convert<string>();

			JSExpr* result = new JSExpr(JSExprType::Function, JSToken("null", nullptr));

			result->Args = Args();

			result->Block = Block();

			stmt->Type = JSStmtType::Function;

			stmt->Expr = result;

			if (Lex.PeekJSToken().Type == "Semicolon")
				Lex.GetJSToken();

			return stmt;
		}

		stmt->Type = JSStmtType::Expression;

		stmt->Expr = Expression();

		if (Lex.PeekJSToken().Type == "Semicolon")
			Lex.GetJSToken();

		return stmt;
	}

	JSExpr* Expression() {
		if (Lex.PeekJSToken().Compare<string>("Ident", "function")) {
			Lex.GetJSToken();

			JSExpr* result = new JSExpr(JSExprType::AnonFunction, JSToken("null", nullptr));

			result->Args = Args();

			result->Block = Block();

			return result;
		}

		// Arrow functions
		int saved = Lex.Position;

		vector<JSExpr*> args;

		if (Lex.PeekJSToken().Type == "LParen") {
			args = Args(true);
		}
		else {
			args = vector<JSExpr*>();

			args.push_back(new JSExpr(JSExprType::Identifier, Lex.GetJSToken()));
		}

		if (Lex.PeekJSToken().Compare<string>("Op", "=>")) {
			Lex.GetJSToken();

			JSExpr* result = new JSExpr(JSExprType::AnonFunction, JSToken("null", nullptr));

			result->Args = args;

			if (Lex.PeekJSToken().Type == "LCurly") {
				result->Block = Block();
			}
			else {
				result->Block = vector<JSStmt*>();

				JSStmt* expr = new JSStmt();

				expr->Type = JSStmtType::Expression;

				expr->Expr = Expression();

				result->Block.push_back(expr);
			}

			return result;
		}

		Lex.Position = saved;

		if (Lex.PeekJSToken().Compare<string>("Ident", "let") || Lex.PeekJSToken().Compare<string>("Ident", "const")) {
			Lex.GetJSToken();

			JSExpr* result = new JSExpr(JSExprType::Let, JSToken("null", nullptr));

			result->Left = new JSExpr(JSExprType::Identifier, Lex.GetJSToken());

			// Default to undefined later.
			// Segmentation fault if no definition provided.
			if (Lex.PeekJSToken().Compare<string>("Op", "=")) {
				Lex.GetJSToken();

				if (Lex.PeekJSToken().Type == "EOF")
					throw Error("Expected expression");

				result->Right = Expression();
			}

			return result;
		}

		if (Lex.PeekJSToken().Type == "LCurly") {
			JSExpr* result = new JSExpr(JSExprType::Object, JSToken("null", nullptr));
		}

		return TopLogic();
	}

	JSExpr* TopLogic() {
		JSExpr* left = Logic();

		JSExpr* result = new JSExpr(JSExprType::Binary, JSToken("null", nullptr));

		if (Lex.PeekJSToken().Compare<string>("Op", "||") || Lex.PeekJSToken().Compare<string>("Op", "&&")) {
			JSToken op = Lex.GetJSToken();

			int pos = Lex.Position;

			JSExpr* right = Logic();

			if (Lex.PeekJSToken().Compare<string>("Op", "||") || Lex.PeekJSToken().Compare<string>("Op", "&&")) {
				Lex.Position = pos;

				right = TopLogic();
			}

			result->Left = left;

			result->Op = op;

			result->Right = right;

			return result; 
		}

		return left;
	}

	JSExpr* Logic() {
		JSExpr* left = Assign();

		JSExpr* result = new JSExpr(JSExprType::Binary, JSToken("null", nullptr));

		if (Lex.PeekJSToken().Compare<string>("Op", "==") || Lex.PeekJSToken().Compare<string>("Op", "!=") || Lex.PeekJSToken().Compare<string>("Op", ">") || Lex.PeekJSToken().Compare<string>("Op", "<")) {
			JSToken op = Lex.GetJSToken();

			JSExpr* right = (Lex.PeekJSToken(2).Compare<string>("Op", "==") || Lex.PeekJSToken(2).Compare<string>("Op", "!=") || Lex.PeekJSToken().Compare<string>("Op", ">") || Lex.PeekJSToken().Compare<string>("Op", "<")) ? Logic() : Assign();

			result->Left = left;

			result->Op = op;

			result->Right = right;

			return result; 
		}

		return left;
	}

	JSExpr* Assign() {
		JSExpr* left = Add();

		JSExpr* result = new JSExpr(JSExprType::Binary, JSToken("null", nullptr));

		if (Lex.PeekJSToken().Compare<string>("Op", "=")) {
			JSToken op = Lex.GetJSToken();

			JSExpr* right = Lex.PeekJSToken(2).Compare<string>("Op", "=") ? Assign() : Expression();

			result->Left = left;

			result->Op = op;

			result->Right = right;

			return result; 
		}

		return left;
	}

	JSExpr* Add() {
		JSExpr* left = Sub();

		JSExpr* result = new JSExpr(JSExprType::Binary, JSToken("null", nullptr));

		if (Lex.PeekJSToken().Compare<string>("Op", "+")) {
			JSToken op = Lex.GetJSToken();

			// A function call is 4 tokens instead of just one
			if (Lex.PeekJSToken(5).Compare<string>("Op", "+")) {
				JSExpr* right = Add();

				result->Left = left;

				result->Op = op;

				result->Right = right;

				return result; 
			}

			JSExpr* right = Lex.PeekJSToken(2).Compare<string>("Op", "+") ? Add() : Sub();

			result->Left = left;

			result->Op = op;

			result->Right = right;

			return result; 
		}

		return left;
	}

	JSExpr* Sub() {
		JSExpr* left = Mul();

		JSExpr* result = new JSExpr(JSExprType::Binary, JSToken("null", nullptr));

		if (Lex.PeekJSToken().Compare<string>("Op", "-")) {
			JSToken op = Lex.GetJSToken();

			// A function call is 4 tokens instead of just one
			if (Lex.PeekJSToken(5).Compare<string>("Op", "-")) {
				JSExpr* right = Sub();

				result->Left = left;

				result->Op = op;

				result->Right = right;

				return result; 
			}

			JSExpr* right = Lex.PeekJSToken(2).Compare<string>("Op", "-") ? Sub() : Mul();

			result->Left = left;

			result->Op = op;

			result->Right = right;

			return result; 
		}

		return left;
	}

	JSExpr* Mul() {
		JSExpr* left = Div();

		JSExpr* result = new JSExpr(JSExprType::Binary, JSToken("null", nullptr));

		if (Lex.PeekJSToken().Compare<string>("Op", "*")) {
			JSToken op = Lex.GetJSToken();

			// A function call is 4 tokens instead of just one
			if (Lex.PeekJSToken(5).Compare<string>("Op", "*")) {
				JSExpr* right = Mul();

				result->Left = left;

				result->Op = op;

				result->Right = right;

				return result; 
			}

			JSExpr* right = Lex.PeekJSToken(2).Compare<string>("Op", "*") ? Mul() : Div();

			result->Left = left;

			result->Op = op;
			
			result->Right = right;

			return result; 
		}

		return left;
	}

	JSExpr* Div() {
		JSExpr* left = Unary();

		JSExpr* result = new JSExpr(JSExprType::Binary, JSToken("null", nullptr));

		if (Lex.PeekJSToken().Compare<string>("Op", "/")) {
			JSToken op = Lex.GetJSToken();

			// A function call is 4 tokens instead of just one
			if (Lex.PeekJSToken(5).Compare<string>("Op", "/")) {
				JSExpr* right = Add();

				result->Left = left;

				result->Op = op;

				result->Right = right;

				return result; 
			}

			JSExpr* right = Unary(); // Lex.PeekJSToken(2).Compare<string>("Op", "/") ? Div() : Unary();

			while (Lex.PeekJSToken().Compare<string>("Op", "/")) {
				JSExpr* temp = new JSExpr(JSExprType::Binary, JSToken("null", nullptr));

				temp->Left = left;

				temp->Op = Lex.GetJSToken();

				temp->Right = right;

				left = temp;

				right = Unary();
			}

			result->Left = left;

			result->Op = op;
			
			result->Right = right;

			return result; 
		}

		return left;
	}

	JSExpr* AccessibleReturn(JSExpr* base, bool noaccess) {
		if (! noaccess && Lex.PeekJSToken().Compare<string>("Op", ".")) {
			JSExpr* access = new JSExpr(JSExprType::Access, JSToken("null", nullptr));

			access->AccessPath.push_back(base);

			while (Lex.PeekJSToken().Compare<string>("Op", ".")) {
				Lex.GetJSToken();

				access->AccessPath.push_back(Literal(false, true));
			}

			return AccessibleReturn(access, noaccess);
		}

		if (! noaccess && Lex.PeekJSToken().Type == "LBracket") {
			Lex.GetJSToken();

			JSExpr* index = new JSExpr(JSExprType::Index, JSToken("null", nullptr));

			index->Left = base;

			index->Right = Expression();

			if (Lex.GetJSToken().Type != "RBracket")
				throw Error("Expected ]");

			if (Lex.PeekJSToken().Type == "LParen") {
				JSExpr* call = new JSExpr(JSExprType::Call, JSToken("null", nullptr));

				call->Fn = index;
				call->Args = Args();

				return AccessibleReturn(call, noaccess);
			}

			return AccessibleReturn(index, noaccess);
		}

		return base;
	}

	JSExpr* Unary() {
		if (Lex.PeekJSToken().Compare<string>("Op", "-")) {
			JSExpr* expr = new JSExpr(JSExprType::Unary, Lex.GetJSToken());

			expr->Right = Literal();

			return expr;
		}

		return Literal();
	}

	JSExpr* Literal(bool nocall = false, bool noaccess = false) {
		if (Lex.PeekJSToken().Type == "LParen") {
			Lex.GetJSToken();

			JSExpr* result = new JSExpr(JSExprType::Group, JSToken("null", nullptr));

			if (Lex.PeekJSToken().Type == "RParen") {
				Lex.GetJSToken();

				result->Group = new JSExpr(JSExprType::Value, JSToken("null", nullptr));

				return AccessibleReturn(result, noaccess);
			}

			result->Group = Expression();

			if (Lex.GetJSToken().Type != "RParen")
				throw Error(Lex.PositionStr() + " Expected )");

			if (Lex.PeekJSToken().Type == "LParen" && ! nocall) {
				vector<JSExpr*> args = Args();

				JSExpr* callexpr = new JSExpr(JSExprType::Call, result->Group->Value);

				callexpr->Fn = result->Group;
				callexpr->Args = args;

				return AccessibleReturn(callexpr, noaccess);
			}

			return AccessibleReturn(result, noaccess);
		}

		if (Lex.PeekJSToken().Type == "LCurly") {
			JSExpr* result = new JSExpr(JSExprType::Object, JSToken("null", nullptr));

			result->Properties = Object();

			return result;
		}

		if (Lex.PeekJSToken().Type == "LBracket") {
			Lex.GetJSToken();

			JSExpr* result = new JSExpr(JSExprType::Array, JSToken("null", nullptr));

			vector<JSExpr*> elements = vector<JSExpr*>();

			while (Lex.PeekJSToken().Type != "RBracket") {
				elements.push_back(Expression());

				if (Lex.PeekJSToken().Type == "Comma")
					Lex.GetJSToken();
				
				if (Lex.PeekJSToken().Type == "EOF")
					throw Error("Expected ]");
			}

			Lex.GetJSToken();

			result->Array = elements;

			return result;
		}

		if (Lex.PeekJSToken().Type == "Newline")
			throw Error("");

		JSToken tok = Lex.GetJSToken();

		JSExpr* expr = new JSExpr(JSExprType::Identifier, tok);

		if (tok.Compare<string>("Ident", "new")) {
			expr->Type = JSExprType::New;

			if (Lex.PeekJSToken().Type != "Ident")
				throw Error("Expected identifier");

			expr->Value = Lex.GetJSToken();

			expr->Args = Args();
		}

		if (tok.Type == "String")
			expr->Type = JSExprType::Value;

		if (tok.Type == "Integer")
			expr->Type = JSExprType::Value;

		if (Lex.PeekJSToken().Type == "LBracket")
			return AccessibleReturn(expr, noaccess);

		if (! noaccess && Lex.PeekJSToken().Compare<string>("Op", "."))
			return AccessibleReturn(expr, noaccess);

		if (tok.Type != "Newline" && Lex.PeekJSToken().Type == "LParen" && ! nocall) {
			JSExpr* callexpr = new JSExpr(JSExprType::Call, tok);

			callexpr->Fn = expr;
			callexpr->Args = Args();

			return AccessibleReturn(callexpr, noaccess);
		}

		if (tok.Type != "Ident" && tok.Type != "String" && tok.Type != "Integer")
			throw Error("Unexpected token " + tok.Type);

		/*if (tok.Type != "Ident")
			expr->Type = JSExprType::Value;*/

		return expr;
	}

	JSParser(string src) : Lex(src){
		
	}
};
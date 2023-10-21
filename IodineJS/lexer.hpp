#include <iostream>
#include <string>
#include <memory>

using std::string;

class Error : public std::exception {
	public:

	string Message;

	const char* what() const noexcept {
		return Message.c_str();
	}

	Error(string message) {
		Message = message;
	}
};

class JSToken {
	void* Value;

	public:

	string Type;

	template <class T>
	T Convert() {
		return *((T*) Value);
	}

	template <class T>
	bool Compare(string type, T value) {
		return Type == type && *((T*) Value) == value;
	}

	// Makes a copy to avoid referencing stack memory, which would cause major problems, all constructors make a copy.
	JSToken(string value, bool str = false) {
		Type = str ? "String" : "Ident";

		Value = new string(value);
	}

	JSToken(int value) {
		Type = "Integer";

		Value = new int[1] { value };
	}

	JSToken(string type, void* value) {
		Type = type;

		Value = value;
	}

	JSToken() {
		
	}

	static JSToken Construct_String(string type, string value) {
		JSToken token = JSToken();

		token.Value = new string(value);

		return token;
	}

	void Free() {
		if (Type == "String")
			delete (string*) Value;

		if (Type == "Integer")
			delete (int*) Value;
	}
};

class JSLexer {
	public:

	string Source;

	int Position = 0;

	int Line = 1;
	int Column = 1;

	int LastLine;
	int LastColumn;
	int LastPosition;

	char Get() {
		if (Position >= Source.length())
			return '\0';

		Column++;

		if (Source[Position] == '\n') {
			Line++;
			Column = 1;
		}

		return Source[Position++];
	}

	char Peek() {
		if (Position >= Source.length())
			return '\0';

		if (Source[Position] < 0)
			return '\0';

		return Source[Position];
	}

	bool AtEnd() {
		return Position >= Source.length() || Peek() == '\0';
	}

	bool IsSpace(char c) {
		return c == ' ' || c == '\t' || c == '\r';
	}

	bool IsDigit(char c) {
		return c >= '0' && c <= '9';
	}

	bool IsAlpha(char c) {
		return (c >= 'a'  && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
	}

	bool IsOp(char c) {
		return c == '!' || c == '=' || c == '+' || c == '-' || c == '/' || c == '*' || c == '.' || c == '>' || c == '<' || c == '|' || c == '&';
	}

	void SkipSpace() {
		while (IsSpace(Peek())) {
			Get();
		}
	}

	void SkipLines() {
		SkipSpace();
		
		while (Peek() == '\n') {
			Get();
		}
	}

	string PositionStr() {
		return std::to_string(Line) + ":" + std::to_string(Column);
	}

	string GetIdent() {
		string text = "";

		while (IsAlpha(Peek()) || IsDigit(Peek())) {
			text += Get();
		}

		return text;
	}

	int GetInteger() {
		string text = "";

		while (IsDigit(Peek())) {
			text += Get();
		}

		return stoi(text);
	}

	JSToken GetJSToken(bool lines = false) {
		LastPosition = Position;
		LastLine = Line;
		LastColumn = Column;

		if (! lines)
			SkipLines();

		SkipSpace();

		if (! lines)
			SkipLines();

		if (Peek() == '\0')
			return JSToken("EOF", nullptr);

		if ((Peek() == '\n') && Get())
			return JSToken("Newline", nullptr);

		if ((Peek() == ';') && Get())
			return JSToken("Semicolon", nullptr);

		if (Peek() == '/' && Source[Position + 1] == '/') {
			while (Peek() != '\n')
				Get();
			
			return GetJSToken();
		}

		if (IsDigit(Peek()))
			return JSToken(GetInteger());

		// Get() only called if the condition is true
		if (Peek() == '{' && Get())
			return JSToken("LCurly", nullptr);

		if (Peek() == '}' && Get())
			 return JSToken("RCurly", nullptr);

		if (Peek() == '[' && Get())
			return JSToken("LBracket", nullptr);

		if (Peek() == ']' && Get())
			 return JSToken("RBracket", nullptr);

		if (Peek() == '(' && Get())
			return JSToken("LParen", nullptr);

		if (Peek() == ')' && Get())
			return JSToken("RParen", nullptr);

		if (Peek() == ',' && Get())
			return JSToken("Comma", nullptr);

		if (Peek() == ':' && Get())
			return JSToken("Colon", nullptr);

		if (Peek() == '"' || Peek() == '\'') {
			char qoute = Get();

			string text = "";

			while (Peek() != qoute) {
				if (Peek() == '\\') {
					Get();

					if (Peek() == 'n') {
						text += '\n';

						Get();
						
						continue;
					}
					
					text += Get();
					
					continue;
				}

				text += Get();
			}

			Get();

			return JSToken(text, true);
		}

		if (IsOp(Peek())) {
			string op = "";

			while (IsOp(Peek())) {
				op += Get();
			}

			JSToken tok = JSToken(op);

			tok.Type = "Op";

			return tok;
		}

		return JSToken(GetIdent());
	}

	JSToken PeekJSToken(int count = 1, bool lines = false) {
		int lp = Position;
		int ll = Line;
		int lc = Column;

		JSToken tok = GetJSToken(lines);

		for (int i = 1; i < count; i++) {
			tok = GetJSToken(lines);
		}

		LastPosition = lp;
		LastLine = ll;
		LastColumn = lc;

		Revert();

		return tok;
	}

	void Revert() {
		Position = LastPosition;
		Line = LastLine;
		Column = LastColumn;
	}

	JSLexer(string src) {
		Source = src;
	}
};
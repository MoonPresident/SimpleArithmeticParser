/***
	Component: A simple arithmetic parser in C++.
	Valid Inputs:
	Numbers: (-)[0-9]+
	Brackets: \(\)
	Operators: [+-/*]

	BOMDAS is not respected. Brackets are expanded first and then the expressions is
	evaluated from left to right.

	Whitespace will be ignored, invalid characters and unbalanced brackets will return false.

	Design Decisions
	An empty bracket will be considered invalid as its reasonable to expect a 
	token between brackets.
	As single number in brackets is silly but will be considered valid.
	Leading zeroes are considered valid and are ignored.
	Negating brackets is invalid.
	Aa single header file is used for simplicity.
	I compiled this on MinGW (on windows) so I'm assuming it will compile for clang and MSVC as well.

	State Machine:
	Start with State = A.
	ResultStr = ""
	Operator = Add (First move will to add the first number)
	Result = 0
	Stack = std::vector<pair<int, operator>> (Push and pop as brackets open and close)

	Init =>
		Result = 0
		Operator = Add
		ResultStr = ""
	ProcessNumber => 
		Result = Operator(Result, strtol(ResultStr))
		ResultStr = ""
	PushStack => 
		Stack.push_back(std::pair(Result, Operator))
		Init()
	PopStack => 
		Result = Operator(Result, Stack.back().0)
		ResultStr = Stack.back().1
		Stack.pop()

	State = A
	Init()

	Loop:
	State A: Expecting number or open bracket
	'[-|0-9]' => State = B, ResultStr.push_back(input)
	'(' => State = A, PushStack()
	Whitespace => State = A
	Invalid => return 0

	State B: Expecting operator, closing bracket or null terminator (closed bracket or end of string)
	'[+-/*]' => State = A, SetOperator(input)
	')' => State = B, PopStack()
	Whitespace => State = B
	Invalid => return 0

	State C: Expecting number
	'[0-9]' => State = C,
	'[+-/*]' => State = A, ProcessNumber()
	')' => State = B, ProcessNumber()
	Whitespace => State = B
	Invalid => return 0
***/

#ifndef SIMPLE_ARITHMETIC_PARSER
#define SIMPLE_ARITHMETIC_PARSER

#include <vector>

enum Op {
	Add,
	Minus,
	Multiply,
	Divide
};

enum State {
	Expression,
	Evaluation,
	Number
};

bool evaluate(const char *expression, int &result)
{
	std::string numberString = "";
	Op op;
	
	//This abstraction is over the top but codifying the initial state in code is sensible.
	auto reset = [&]() {
		result = 0;
		op = Add;
	};
	
	//Initialise arithmetic values.
	reset();
	
	//The stack will store results and operations when we are nesting our value.
	std::vector<std::pair<int, Op>> stack;
	State state = Expression;
	
	auto processNumber = [](int &a, int b, Op op) {
		switch(op) {
			case Add:
				a = a + b;
				break;
			case Minus:
				a = a - b;
				break;
			case Multiply:
				a = a * b;
				break;
			case Divide:
				if(b == 0) {
					throw std::invalid_argument("Don't divide by zero please.");
				}
				a = a / b;
				break;
		}
	};
	
	//Loop variables
	size_t i = 0;
	char c = expression[i++];
	
	while(true) {
		/* 
		 * I think this could be inverted to be terser (switch on chars and
		 * then on State) but the functionality would then be less clear.
		 */
		switch(state) {
			case State::Expression:
				switch(c) {
					//Numbers
					case '-':
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						numberString.push_back(c);
						state = State::Number;
						break;
					case '(':
						//State = State::Expression
						stack.push_back(std::pair(result, op));
						reset();
						break;
					case ' ':
					case '\t':
					case '\n':
					case '\r':
						//State = State::Expression
						break;
					default:
						return false;
				}
				break;
			case State::Evaluation:
				
				//parse number (scoped for readability)
				if(!numberString.empty()) {
					if(numberString == "-") {
						return false;
					}
					
					try {
						auto number = stoi(numberString);
						
						//While I could throw and catch, I think this is better practice.
						if(number == 0 && op == Op::Divide) {
							return false;
						}
						
						processNumber(result, number, op);
						numberString.clear();
					} catch (std::exception e) {
						return false;
					}
				}
				
				switch(c) {
					//Operators
					case '+':
						op = Op::Add;
						state = State::Expression;
						break;
					case '-':
						op = Op::Minus;
						state = State::Expression;
						break;
					case '*':
						op = Op::Multiply;
						state = State::Expression;
						break;
					case '/':
						op = Op::Divide;
						state = State::Expression;
						break;
					case ')':
						//State = State::Evaluation
						if(stack.empty()) {
							return false;
						}
						
						//Pop the last result and op and apply the latest expression to them.
						{
							int temp = result;
							result = stack.back().first;
							processNumber(result, temp, stack.back().second);
						}
						stack.pop_back();
						break;
					//terminate
					case '\0': 
						break;
					//Whitespace
					case ' ':
					case '\t':
					case '\n':
					case '\r':
						//State = State::Expression
						break;
					default:
						return false;
				}
				break;
			case State::Number:
				switch(c) {
					//Numbers
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						//state = State::Number;
						numberString.push_back(c);
						break;
					//Operators and null terminator
					case '+':
					case '-':
					case '*':
					case '/':
					case ')':
					case'\0':
						//Go back one to pass this through the evaluator.
						state = State::Evaluation;
						continue; //Don't grab next character, let the evaluation arm of the switch process this character.
					//Whitespace
					case ' ':
					case '\t':
					case '\n':
					case '\r':
						state = State::Evaluation;
						break;
					default:
						return false;
				}
				break;
			default:
				return false;
		}
		
		if(c == '\0') {
			break;
		}
		
		c = expression[i++];
	}
	
	//Stack should be empty if the correct number of braces were found.
	if(!stack.empty()) {
		return false;
	}
	
	return true;
}

#endif /*SIMPLE_ARITHMETIC_PARSER*/
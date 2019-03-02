#include <iostream>
#include <cmath>

#include "stack.hpp"

//Uncomment for debugging
// #define DEBUG

//Errors related to stack
enum StackError {
	EMPTY_STACK
};

//Errors that can be found during calculation
enum CalError {
	NO_OPERAND_UN,
	NO_OPERANDS_BIN,
	EXTRA_VALUES
};

int strlen(const char* str)
{
	int len = 0;
	while (str[len])
		len++;
	
	return len;
}

//Check for balanced brackets in expression
bool balanced(const char* str)
{
	int count = 0;

	//Closing brackets before opening brackets make expression unbalanced
	for (int i = 0; str[i] && count >= 0; i++)
	{
		if (str[i] == '(')
			count++;
		else if (str[i] == ')')
			count--;
	}

	//Negative means more ')'
	//Positive means more '('
	return count == 0;
}

//Get precedence for binary operator
//Returns -1 for unknown operator
const int getBinPrec(const char& ch)
{
	switch(ch)
	{
	case '&':
	case '|':
		return 5;

	case '<':
	case '>':
	case 'G':
	case 'g':
	case '=':
		return 4;

	case 'l':
		return 3;

	case '+':
	case '-':
		return 2;
	
	case '*':
	case '/':
		return 1;
	
	case '^':
		return 0;

	default:
		return -1;
	}
}

//Get precedence for unary operator
//Returns -1 for unknown operator
const int getUnPrec(const char& ch)
{
	switch(ch)
	{
	case '!':
		return 2;

	//Trigonometric functions
	case 's':
	case 'c':
	case 't':
	case 'C':
	case 'S':
	case 'T':
	//Natural Log
	case 'L':
		return 1;
	
	case 'n':
		return 0;
	
	default:
		return -1;
	}
}

//Check if binary operator exists
bool isBinaryOp(const char& op)
{
	return getBinPrec(op) != -1;
}

//Check if unary operator exists
bool isUnaryOp(const char& op)
{
	return getUnPrec(op) != -1;
}

//Check if constant exists
bool isConstant(const char& ch)
{
	return ch == 'P' || ch == 'E';
}

//Check for digits
bool isDigit(const char& ch)
{
	return (ch >= '0' && ch <= '9');
}

//Check for alphabets
bool isAlphabet(const char& ch)
{
	return (ch >= 'A' && ch <= 'Z')
		|| (ch >= 'a' && ch <= 'z');
}

bool isWhiteSpace(const char& ch)
{
	return ch == ' ' || ch == '\t' || ch == '\n';
}

//Compare subset of first string to given string
bool compareSubString(const char*& str1, int beg, int len, const char* str2)
{
	int idx = 0;
	while (idx < len)
	{	
		if (str1[beg + idx] != str2[idx])
			return false;
		
		idx++;
	}
	
	return true;
}

//Checks if a constant is at the given index and moves index to the end
//Constants are represneted by their first letter in capital
const char getConstCode(const char*& str, int& idx)
{
	switch(str[idx])
	{
	case 'p':
		if (compareSubString(str, idx, 2, "pi"))
		{
			idx += 2;
			return 'P';
		} else
			return '\0';
	
	case 'e':
		idx += 1;
		return 'E';
	
	default:
		return '\0';
	}
}

//Returns the value of a constant
//Before using this function, check if the character is a constant or not
double getConstValue(const char& ch)
{
	switch(ch)
	{
	case 'P':
		return M_PI;

	case 'E':
		return M_E;
	}
}

//Checks if a function is at the given index and moves index to the end
//Most functions are represented by their first letters
//Sec, Cosec and Cot are represented by the capital letters of their respective inverses
const char getOperatorCode(const char*& str, int& idx, const bool neg = false)
{
	switch(str[idx])
	{
	case 'l':
		if (compareSubString(str, idx, 2, "ln"))
		{
			idx += 2;
			return 'L';
		} else if (compareSubString(str, idx, 3, "log"))
		{
			idx += 3;
			return 'l';
		}
		else
			return '\0';
	
	case 's':
		if (compareSubString(str, idx, 3, "sin"))
		{
			idx += 3;
			return 's';
		}
		else if (compareSubString(str, idx, 3, "sec"))
		{
			idx += 3;
			return 'C';
		}
		else
			return '\0';
	
	case 'c':
		if (compareSubString(str, idx, 5, "cosec"))
		{
			idx += 5;
			return 'S';
		}
		else if (compareSubString(str, idx, 3, "cos"))
		{
			idx += 3;
			return 'c';
		}
		else if (compareSubString(str, idx, 3, "cot"))
		{
			idx += 3;
			return 'T';
		}
		else
			return '\0';
	
	case 't':
		if (compareSubString(str, idx, 3, "tan"))
		{
			idx += 3;
			return 't';
		}
		else
			return '\0';
	
	case '<':
		if (compareSubString(str, idx, 2, "<="))
		{
			idx += 2;
			return 'g';
		}
		else
			return str[idx++];

	case '>':
		if (compareSubString(str, idx, 2, ">="))
		{
			idx += 2;
			return 'G';
		}
		else
			return str[idx++];
	
	case '-':
		idx++;
		if (neg)
			return 'n';
		else
			return '-';

	case '&':
		if (compareSubString(str, idx, 2, "&&"))
		{
			idx += 2;
			return '&';
		}
		else
			return '\0';

	case '|':
		if (compareSubString(str, idx, 2, "||"))
		{
			idx += 2;
			return '|';
		}
		else
			return '\0';
	
	case '=':
		if (compareSubString(str, idx, 2, "=="))
		{
			idx += 2;
			return '=';
		}
		else
			return '\0';

	default:
		if (isBinaryOp(str[idx]) || isUnaryOp(str[idx]))
			return str[idx++];
		else
			return '\0';
	}
}

//Use character codes for operator to give the actual operator
//Only implemented for operators using more than one character
const char* getOperatorName(const char& op)
{
	switch(op)
	{
	case 'l':
		return "log";
	
	case 'L':
		return "ln";
	
	case 's':
		return "sin";
	
	case 'c':
		return "cos";
	
	case 't':
		return "tan";
	
	case 'C':
		return "sec";
	
	case 'S':
		return "cosec";
	
	case 'T':
		return "cot";
	
	case 'G':
		return ">=";
	
	case 'g':
		return "<=";
	
	case '&':
		return "&&";
	
	case '|':
		return "||";
	
	case '=':
		return "==";

	default:
		char* str = new char[2];
		str[0] = op;
		str[1] = '\0';
		return str;
	}
}

//Check if current operator has greater precedence than the one in stack
bool greaterPrec(const char& curOp, const char& opInStack)
{
	//Unary operators have greater precedence than binary operators

	if (isUnaryOp(curOp) && isBinaryOp(opInStack))
		return false;
	else if (isBinaryOp(curOp) && isUnaryOp(opInStack))
		return true;

	if (isUnaryOp(curOp) && isUnaryOp(opInStack))
		return getUnPrec(curOp) > getUnPrec(opInStack);
	else
		return getBinPrec(curOp) > getBinPrec(opInStack);
}

//Convert expression from infix to postfix
const char* in_post(const char*& exp)
{
    Stack<char> operators;
	
    //Size of postfix expression is larger to commpensate for additional spaces
    int size = strlen(exp);
    size *= 1.5;
    char* post = new char[size];

	int pIdx = 0;
    bool allow_neg = true;      //Tracks if '-' is unary or binary

    for (int idx = 0; exp[idx]; idx++)
    {
        
        //Numbers are directly entered into the postfix expression
        if (isDigit(exp[idx]) || exp[idx] == '.')
        {
            //If two digits are separated by whitspaces then they are separate numbers
            if (idx > 0 && !isDigit(exp[idx - 1]))
                post[pIdx++] = ' ';

            post[pIdx++] = exp[idx];
            allow_neg = false;
            continue;
        }

        //Check for constants
        char constCode = getConstCode(exp, idx);
        if (isConstant(constCode))
        {
            if (idx > 0 && (isDigit(post[pIdx - 1]) || isConstant(post[pIdx - 1])))
                post[pIdx++] = ' ';
            
            post[pIdx++] = constCode;
            allow_neg = false;
			idx--;
            continue;
        }

        char opCode = getOperatorCode(exp, idx, allow_neg);
        if (isUnaryOp(opCode) || isBinaryOp(opCode))
        {
			//Overwrite extra space if operator needs to be written
			if (post[pIdx - 1] == ' ')
				pIdx--;
			
            //Pop operators and add to string until an operator of greater precedence is found
			while(!operators.empty()
                && operators.top() != '('
                && greaterPrec(opCode, operators.top()))
			{

				post[pIdx++] = operators.top();
				operators.pop();
			}

			operators.push(opCode);

			//'-' after any operator will be unary
			allow_neg = true;
			idx--;
            continue;
        }

        //If brackets are encountered
        if (exp[idx] == '(')
        {
            if (isDigit(exp[idx - 1]))
                post[pIdx++] = ' ';

			operators.push('(');

			//'-' just after '(' is unary
			allow_neg = true;
        }
        else if (exp[idx] == ')')
        {
            while (operators.top() != '(')
			{
				post[pIdx++] = operators.top();
				operators.pop();
			}

			operators.pop();

			//'-' just after ')' is binary
			allow_neg = false;

			if (!operators.empty() && isUnaryOp(operators.top()))
			{
				post[pIdx++] = operators.top();
				operators.pop();
			}

			if (isDigit(post[pIdx - 1]))
				post[pIdx++] = ' ';
        }

    }

	//If any operators are left then all are added to the expression
	while (!operators.empty())
	{
		post[pIdx++] = operators.top();
		operators.pop();
	}
	post[pIdx] = '\0';

    //For debugging
	#ifdef DEBUG
	std::cout << post << std::endl;
	#endif

	return post;
}

//Calculate according to respective operator with given values
double operateBin(const double& val1, const char& op, const double& val2)
{
	switch (op)
	{
	case '&':
		return val1 && val2;
	
	case '|':
		return val1 || val2;

	case '=':
		//Using an epsilon to prevent errors
		return std::abs(val1 - val2) < 0.001;
	
	case '<':
		return val1 < val2;
	
	case '>':
		return val1 > val2;

	case 'G':
		return val1 >= val2;
	
	case 'g':
		return val1 <= val2;

	case 'l':
		return log(val2) / log(val1);

	case '+':
		return val1 + val2;
	
	case '-':
		return val1 - val2;
	
	case '*':
		return val1 * val2;
	
	case '/':
		return val1 / val2;
	
	case '^':
		return pow(val1, val2);
	}
}

double operateUn(const char& op, const double& val)
{
	switch(op)
	{
	case '!':
		return !(val);
	
	case 's':
		return sin(val);
	
	case 'c':
		return cos(val);
	
	case 't':
		return tan(val);

	case 'C':
		return 1 / cos(val);
	
	case 'S':
		return 1 / sin(val);
	
	case 'T':
		return 1 / tan(val);
	
	case 'L':
		return log(val);
	
	case 'n':
		return val * -1.0;
	}
}

//Performs corresponding operation
void operate(Stack<double>& values, const char* str, int idx)
{
	if (isUnaryOp(str[idx]))
	{
		//Throw exception if value is not in stack
		if (values.size() < 1)
			throw NO_OPERAND_UN;

		//If unary operator is encountered
		double val = values.top();
		values.pop();

		double res = operateUn(str[idx], val);
		values.push(res);

		#ifdef DEBUG
		std::cout << "operation : " << getOperatorName(str[idx]) << " " << val << " = " << res << std::endl;
		std::cout << "stack     : ";
		values.display();
		std::cout << std::endl;
		#endif

	} else if (isBinaryOp(str[idx]))
	{
		//Throw exception if stack has less than 2 values
		if (values.size() < 2)
			throw NO_OPERANDS_BIN;

		//If binary operator is encountered
		double val2 = values.top();
		values.pop();

		double val1 = values.top();
		values.pop();

		double res = operateBin(val1, str[idx], val2);
		values.push(res);

		//For debugging
		#ifdef DEBUG
		std::cout << "operation : " << val1 << " " << getOperatorName(str[idx]) << " " << val2 << " = " << res << std::endl;
		std::cout << "stack     : ";
		values.display();
		std::cout << std::endl;
		#endif

	}
}

//Solve postfix expression
double solve(const char* str)
{
	//If expression has no values, return 0
	if (str[0] == '\0')
		return 0.0;

	Stack<double> values;
	double  mult = 1.0;
	bool decimal = false, addValue = true;

	for (int idx = 0; str[idx]; idx++)
	{
		if (isDigit(str[idx]))
		{
			//Add new value to stack
			if (addValue)
			{
				values.push(0.0);
				addValue = false;
			}

			if (decimal)
				mult /= 10;
			else
				values.top() *= 10;
			
			values.top() += mult * (str[idx] - '0');
		} else if (str[idx] == '.') {
			decimal = true;
		} else if (isConstant(str[idx]))
		{
			values.push(getConstValue(str[idx]));
		} else
		{			
			//Reset variables for next number
			if (isDigit(str[idx - 1]))
			{
				mult = 1.0;
				decimal = false;
				addValue = true;

				#ifdef DEBUG
				if (!values.empty())
				{
					std::cout << "stack     : ";
					values.display();
					std::cout << std::endl;
				}
				#endif
			}

			try
			{
				//Calculate according to operator
				operate(values, str, idx);
			} catch (CalError& e)
			{
				switch (e)
				{
				case NO_OPERAND_UN:
					std::cerr << "Error: no operand for " << getOperatorName(str[idx]) << std::endl;
					break;
				
				case NO_OPERANDS_BIN:
					std::cerr << "Error: less than two operands for " << getOperatorName(str[idx]) << std::endl;
					break;
				}

				//Break loop if error is encountered
				break;
			}
		}
	}

	//Throw exception if stack is empty after calculation
	if (values.empty())
		throw EMPTY_STACK;
	else if (values.size() > 1)
		throw EXTRA_VALUES;

	//The only value left in the stack is the result
	double res = values.top();
	values.pop();

	return res;
}

int main(int argc, char const *argv[])
{
	#ifdef DEBUG
	std::cout << "COMMENT '#define DEBUG' BEFORE COMMITTING GODDAMNIT" << std::endl;
	#endif

	//If no string is given
	if (argc < 2)
		return 0;

	const char*& expr = argv[1];

	//Check for balanced brackets
	if (!balanced(expr))
	{
		std::cout << "Unbalanced Brackets" << std::endl;
		return 0;
	}

	//Change from infix to postfix
	const char* post = in_post(expr);

	try
	{
		//Solve the postfix expression
		double res = solve(post);

		#ifdef DEBUG
		std::cout << "result    : ";
		#endif

		//Show the solution to the expression
		std::cout << res << std::endl;
	} catch (StackError& e)
	{
		switch (e)
		{
		case EMPTY_STACK:
			std::cerr << "Calculation failed" << std::endl;
			break;
		}
	} catch (CalError& e)
	{
		switch (e)
		{
		case EXTRA_VALUES:
			std::cerr << "Not enough operators" << std::endl;
			break;
		}
	}

	delete[] post;
}
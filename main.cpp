#include <iostream>
#include <cmath>

#include "stack.hpp"

//Uncomment for debugging
#define DEBUG

//This is temporary
//Later size of input string will be calculated
#define MAX_SIZE 100

//Mathematical Constants
const double PI = M_PI;
const double E = M_E;

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
		return 6;

	case '<':
	case '>':
		return 5;
		
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
const char checkForConstants(const char*& str, int& idx)
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
		return PI;

	case 'E':
		return E;
	}
}

//Checks if a function is at the given index and moves index to the end
//Most functions are represented by their first letters
//Sec, Cosec and Cot are represented by the capital letters of their respective inverses
const char checkForFunctions(const char*& str, int& idx)
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
	
	default:
		return '\0';
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
		return getUnPrec(curOp) >= getUnPrec(opInStack);
	else
		return getBinPrec(curOp) >= getBinPrec(opInStack);
}

//Convert expression from infix to postfix
const char* in_post(const char*& str)
{
	Stack<char> op;
	char* new_str = new char[MAX_SIZE];

	int n_idx = 0;
	bool allow_neg = true;  //Tracks if '-' is unary or binary

	for (int idx = 0; str[idx]; idx++)
	{
		if (isDigit(str[idx]) || str[idx] == '.')
		{
			new_str[n_idx++] = str[idx];
			allow_neg = false;
		} else if (isAlphabet(str[idx]))
        {
			char constant = checkForConstants(str, idx);

			if (constant)
			{
				new_str[n_idx++] = constant;
				allow_neg = false;
			} else
			{
				char fn = checkForFunctions(str, idx);

				if (fn)
				{
					op.push(fn);
					allow_neg = true;
				}
			}
			//As idx is pushed to the end of the function,
			//It must be moved one idx back
			idx--;
        } else if (isBinaryOp(str[idx]))
		{
			//If unary '-' is encountered
			if (allow_neg && str[idx] == '-')
			{
				//Unary '-' is represented as 'n'
				op.push('n');
				allow_neg = false;

				continue;
			}

			//Add space after each number
			if (isDigit(new_str[n_idx - 1]))
			{
				new_str[n_idx++] = ' ';

				//'-' after a number is binary
				allow_neg = false;
			}

			//Pop operators and add to string until an operator of greater precedence is found
			while(!op.empty() && op.top() != '(' && greaterPrec(str[idx], op.top()))
			{
				//Overwrite extra space if operator needs to be written
				if (new_str[n_idx - 1] == ' ')
					n_idx--;

				new_str[n_idx++] = op.top();
				op.pop();
			}

			op.push(str[idx]);

			//'-' after any operator will be unary
			allow_neg = true;
		} else if (isUnaryOp(str[idx]))
		{
			//Pop operators and add to string until an operator of greater precedence is found
			while(!op.empty() && op.top() != '(' && greaterPrec(str[idx], op.top()))
			{
				//Overwrite extra space if operator needs to be written
				if (new_str[n_idx - 1] == ' ')
					n_idx--;

				new_str[n_idx++] = op.top();
				op.pop();
			}

			op.push(str[idx]);
		} else if (str[idx] == '(')
		{
            if (isDigit(str[idx - 1]))
                new_str[n_idx++] = ' ';

			op.push('(');

			//'-' just after '(' is unary
			allow_neg = true;
		} else if (str[idx] == ')')
		{
			while (op.top() != '(')
			{
				new_str[n_idx++] = op.top();
				op.pop();
			}

			op.pop();

			//'-' just after ')' is binary
			allow_neg = false;

			if (!op.empty() && isUnaryOp(op.top()))
			{
				new_str[n_idx++] = op.top();
				op.pop();
			}
		}
	}

	//Append remaining operators
	while (!op.empty())
	{
		new_str[n_idx++] = op.top();
		op.pop();
	}
	new_str[n_idx] = '\0';

	//For debugging
	#ifdef DEBUG
	std::cout << new_str << "\n";
	#endif

	return new_str;
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
		//If unary operator is encountered
		double val = values.top();
		values.pop();

		double res = operateUn(str[idx], val);
		values.push(res);

		#ifdef DEBUG
		std::cout << "operation : " << str[idx] << " " << val << " = " << res << std::endl;
		std::cout << "stack     : ";
		values.display();
		std::cout << std::endl;
		#endif
	} else if (isBinaryOp(str[idx]))
	{
		//If binary operator is encountered
		double val2 = values.top();
		values.pop();

		double val1 = values.top();
		values.pop();

		double res = operateBin(val1, str[idx], val2);
		values.push(res);

		//For debugging
		#ifdef DEBUG
		std::cout << "operation : " << val1 << " " << str[idx] << " " << val2 << " = " << res << std::endl;
		std::cout << "stack     : ";
		values.display();
		std::cout << std::endl;
		#endif

		//For <= and >=
		if (str[idx] == '=' && (str[idx + 1] == '<' || str[idx + 1] == '>'))
		{
			//If = is encountered followed by > or < then values are pushed back to re-evaluate
			values.push(val1);
			values.push(val2);
		} else if ((str[idx + 1] == '<' || str[idx + 1] == '>') && str[idx - 1] == '=')
		{
			val2 = values.top();
			values.pop();
			val1 = values.top();
			values.pop();
			
			//Both results are compared to get the result
			res = val1 || val2;
			values.push(res);
		
			#ifdef DEBUG
			std::cout << val1 << " | " << val2 << " = " << res << std::endl;
			#endif
		}
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

			//Calculate according to operator
			operate(values, str, idx);
		}
	}

	//The only value left in the stack is the result
	double res = values.top();
	values.pop();

	return res;
}

int main(int argc, char const *argv[])
{
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

	//Solve the postfix expression
	double res = solve(post);

	#ifdef DEBUG
	std::cout << "result    : ";
	#endif

	//Show the solution to the expression
	std::cout << res << std::endl;

	delete[] post;
}
#include <iostream>
#include <cmath>
#include "stack.hpp"

//Uncomment for debugging
// #define DEBUG

//This is temporary
//Later size of input string will be calculated
#define MAX_SIZE 100

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

//Get precedence for operator
//Returns -1 for unknown operator
const int getPrec(const char& ch)
{
	switch(ch)
	{
	case '&':
	case '|':
		return 7;

	case '!':
		return 6;

	case '<':
	case '>':
		return 5;
		
	case '=':
		return 4;

	case '+':
	case '-':
		return 3;
	
	case '*':
	case '/':
		return 2;
	
	case '^':
		return 1;
	
	case 'n':
		return 0;

	default:
		return -1;
	}
}

//Check for values
bool isValue(const char& ch)
{
	return (ch >= '0' && ch <= '9');
}

//Convert expression from infix to postfix
char* in_post(const char*& str)
{
	Stack<char> op;
	char* new_str = new char[MAX_SIZE];

	int n_idx = 0;
	bool allow_neg = true;  //Tracks if '-' is unary or binary

	for (int idx = 0; str[idx]; idx++)
	{
		if (isValue(str[idx]) || str[idx] == '.')
		{
			new_str[n_idx++] = str[idx];
		} else if (getPrec(str[idx]) > 0)
		{
			//Add space after each number
			if (isValue(new_str[n_idx - 1]))
			{
				new_str[n_idx++] = ' ';

				//'-' after a number is binary
				allow_neg = false;
			}

			const int current = getPrec(str[idx]);

			//If unary '-' is encountered
			if (allow_neg && str[idx] == '-')
			{
				//Unary '-' is represented as 'n'
				op.push('n');
				allow_neg = false;
			
				continue;
			}

			//Pop operators and add to string until an operator of greater precedence is found
			while(!op.empty() && op.top() != '(' && current >= getPrec(op.top()))
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
		} else if (str[idx] == '(')
		{
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
double calculate(const double& val1, const char& op, const double& val2)
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

//Performs corresponding operation
void operate(Stack<double>& values, const char* str, int idx)
{
	const int prec = getPrec(str[idx]);

	if (prec > 0 && str[idx] != '!')
	{
		double val2 = values.top();
		values.pop();

		double val1 = values.top();
		values.pop();

		double res = calculate(val1, str[idx], val2);
		values.push(res);

		//For debugging
		#ifdef DEBUG
		std::cout << "operation : " << val1 << " " << str[idx] << " " << val2 << " = " << res << std::endl;
		#endif
		
		//For >= and <=
		if (str[idx] == '=' && getPrec(str[idx + 1]) == 5)
		{
			//If = is encountered followed by > or < then values are pushed back to re-evaluate
			values.push(val1);
			values.push(val2);
		} else if (prec == 5 && str[idx - 1] == '=')
		{
			val2 = values.top();
			values.pop();
			val1 = values.top();
			values.pop();
			
			//Both results are compared to get the result
			res = val1 || val2;
			values.push(res);
		
			#ifdef DEBUG
			std::cout << val1 << " || " << val2 << " = " << res << std::endl;
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
		if (isValue(str[idx]))
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
		} else
		{
			//If negative operator is encountered
			if (str[idx] == 'n')
				values.top() *= -1.0;

			//If not operator is encountered
			if (str[idx] == '!')
			{
				#ifdef DEBUG
				std::cout << "operation : !(" << values.top() << ") = " << !(values.top()) << std::endl;
				#endif
				values.top() = !(values.top());
			}
			
			//Reset variables for next number
			if (isValue(str[idx - 1]))
			{
				mult = 1.0;
				decimal = false;
				addValue = true;

				#ifdef DEBUG
				if (!values.empty())
					std::cout << "stack     : ";
					values.display();
					std::cout << std::endl;
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
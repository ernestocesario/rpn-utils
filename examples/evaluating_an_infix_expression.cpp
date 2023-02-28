/**
 * @file evaluating_an_infix_expression.cpp
 * @brief Example file to try to convert infix expressions to RPN expressions and then evaluate them
 * @author ernestocesario
 * @date 2023-02-21
 */

#include <iostream>
#include <string>
#include <vector>
#include "rpn_utils.hpp"


int main()
{
    std::vector<std::string> rpn_expr;
    std::string infix_expr;

    std::cout << "Enter your infix expression: ";
    std::getline(std::cin, infix_expr);

    if(!rpn::infix_to_rpn(infix_expr, rpn_expr)){
        std::cout << "The infix expression is not valid!" << std::endl;
        return 1;
    }
    
    std::cout << "RPN expression: ";
    for(std::vector<std::string>::const_iterator it = rpn_expr.cbegin(); it != rpn_expr.cend(); ++it)
        std::cout << *it << ' ';
    std::cout << std::endl;

    std::pair<bool, double> result = rpn::evaluate(rpn_expr);
    if(result.first)
        std::cout << "Evaluation of RPN expression: " << result.second;
    else
        std::cout << "The expression cannot be evaluated mathematically!";
    std::cout << std::endl;

    return 0;
}
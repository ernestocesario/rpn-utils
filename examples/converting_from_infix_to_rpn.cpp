/**
 * @file converting_from_infix_to_rpn.cpp
 * @brief Sample file for trying to convert infix expressions to rpn expressions
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

    while(std::getline(std::cin, infix_expr)){
        bool valid;

        try
        {
            valid = rpn::infix_to_rpn(infix_expr, rpn_expr);
        }
        catch(const std::exception& e)
        {
            valid = false;
            std::cerr << e.what() << '\n';
        }
        
        if(valid){
            std::cout << "RPN expression: ";
            for(std::vector<std::string>::const_iterator it = rpn_expr.cbegin(); it != rpn_expr.cend(); ++it)
                std::cout << *it << ' ';
        }
        else
            std::cout << "The infix expression is not valid!";

        std::cout << std::endl << std::endl;
    }
}
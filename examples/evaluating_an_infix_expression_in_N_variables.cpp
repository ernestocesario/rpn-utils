/**
 * @file evaluating_an_infix_expression_in_N_variables.cpp
 * @brief Example file on how to evaluate infixed expressions with an indefinite number of variables
 * @author ernestocesario
 * @date 2023-02-21
 */

#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include "rpn_utils.hpp"


int main()
{
    unsigned n_var;

    std::cout << "Enter the number of variables to be used: ";
    std::cin >> n_var;

    for(unsigned i = 0; i < n_var; ++i){
        bool valid_name = false;

        do{
            std::string var_name;

            std::cout << "Enter the name of the variable " << i + 1 << ": ";
            std::cin >> var_name;

            if(rpn::add_operand(var_name, 0.0)){
                valid_name = true;
                std::cout << "Variable " << var_name << " correctly added!";
            }
            else
                std::cout << "Invalid name! Please try again";
            std::cout << std::endl;
        }while(!valid_name);
    }

    std::vector<std::string> rpn_expr;
    std::string infix_expr;

    std::cout << "Enter your infix expression: ";

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, infix_expr);

    if(!rpn::infix_to_rpn(infix_expr, rpn_expr)){
        std::cout << "The infix expression is not valid!" << std::endl;
        return 1;
    }
    
    std::cout << "RPN expression: ";
    for(std::vector<std::string>::const_iterator it = rpn_expr.cbegin(); it != rpn_expr.cend(); ++it)
        std::cout << *it << ' ';
    std::cout << std::endl;

    std::cout << "Enter a value for the variables:" << std::endl;
    const std::unordered_map<std::string, double> &my_operands = rpn::get_all_operands();
    for(std::unordered_map<std::string, double>::const_iterator it = my_operands.cbegin(); it != my_operands.cend(); ++it){
        bool valid_value = false;

        do{
            double var_value;
            const std::string &var_name = it -> first;

            std::cout << var_name << ": ";
            std::cin >> var_value;

            if(rpn::add_operand(var_name, var_value))
                valid_value = true;
            else
                std::cout << "Invalid name! Please try again" << std::endl;
        }while(!valid_value);
    }

    std::pair<bool, double> result = rpn::evaluate(rpn_expr);

    if(result.first)
        std::cout << "Evaluation of RPN expression: " << result.second;
    else
        std::cout << "The expression cannot be evaluated mathematically!";
    std::cout << std::endl;

    rpn::clear_all_operands();

    return 0;
}
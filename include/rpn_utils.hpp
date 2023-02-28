/**
 * @file rpn_utils.hpp
 * @brief Header file for RPN (Reverse Polish Notation) utility functions.
 * 
 * Utilities for converting mathematical expressions from infix notation to Reverse Polish Notation (RPN) and for evaluating RPN expressions
 * 
 * @author ernestocesario
 * @date 2023-02-21
 * @license Apache License 2.0
 */


#ifndef RPN_UTILS_HPP
#define RPN_UTILS_HPP

#include <string>
#include <vector>
#include <stack>
#include <cmath>
#include <cfenv>
#include <algorithm>
#include <stdexcept>
#include "additional_operators.hpp"

namespace rpn
{
    bool infix_to_rpn(const std::string &infix_expr, std::vector<std::string> &rpn_expr);  //convert an infix expression to postfix (rpn)
    std::pair<bool, double> evaluate(const std::vector<std::string> &rpn_expr);  //evaluate an expression rpn by substituting a value passed as an argument for the variable, return a <bool, double> pair, where the bool value indicates whether the expression is defined for that passed value and the double value is the result of the evaluation

    bool add_operand(const std::string &operand_name, double operand_value);  //adds an operand to the list of the additional operands. Returns true if the operation is successful
    bool remove_operand(const std::string &operand_name);  //removes an operand from the list of the additional operands. Returns true if the operation is successful, false if the operand isn't in the list
    double get_operand(const std::string &operand_name);  //returns the value (double) associated with the operand passed. If the operand does not exist it returns a default value
    const std::unordered_map<std::string, double> &get_all_operands();  //returns a constant reference to the std::unordered_map object where all additional operands are stored
    void clear_all_operands();  //deletes all the additional operands
}

#endif
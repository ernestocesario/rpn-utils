/**
 * @file additional_operators.hpp
 * @brief Header files for the internal library additional_operators
 * 
 * Internal library used by rpn_utils library to handle additional operators
 * 
 * @author ernestocesario
 * @date 2023-02-21
 * @license Apache License 2.0
 */


#ifndef ADDITIONAL_OPERATORS_HPP
#define ADDITIONAL_OPERATORS_HPP

#include <string>
#include <tuple>
#include <unordered_map>
#include <cmath>
#include <cfenv>

typedef double (*operator_func) (const double *argv);
extern const std::unordered_map<std::string, std::tuple<unsigned short, unsigned short, operator_func>> additional_operators;

#endif
/**
 * @file additional_operators.cpp
 * @brief Implementation files for the internal library additional_operators
 * @author ernestocesario
 * @date 2023-02-21
 * @license Apache License 2.0
 */


#include "additional_operators.hpp"

/*
    Here you can add or remove the function operators supported.
    To add a function operator, place in the unordered_map a string of lowercase
    only characters with which to represent the function operator and a tuple
    consisting of the number of operands the operator requires, the precedence
    value of the operator (>= 0) and a pointer to a function wrapper that takes
    a pointer to constant double as an argument, and returns a double.
    The body of the function wrapper must contain the code necessary to
    produce the result (double) intended to be produced by the chosen operator
    (appropriately using the cfenv library to throw exceptions in case the operator
    is not defined for the passed values).
*/



double wfunc_root(const double *argv)
{
    return pow(argv[1], 1.0/argv[0]);
}

double wfunc_sqrt(const double *argv)
{
    return pow(argv[0], 1.0/2.0);
}

double wfunc_cbrt(const double *argv)
{
    return pow(argv[0], 1.0/3.0);
}



double wfunc_pow(const double *argv)
{
    if(argv[0] == 0 && argv[1] == 0)  //because pow(0, 0) returns 1
        std::feraiseexcept(FE_INVALID);
    return pow(argv[0], argv[1]);
}

double wfunc_sqr(const double *argv)
{
    return pow(argv[0], 2.0);
}

double wfunc_cube(const double *argv)
{
    return pow(argv[0], 3.0);
}



double wfunc_logb(const double *argv)
{
    return log(argv[1]) / log(argv[0]);
}

double wfunc_log(const double *argv)
{
    return log10(argv[0]);
}

double wfunc_ln(const double *argv)
{
    return log(argv[0]);
}



double wfunc_sin(const double *argv)
{
    return sin(argv[0]);
}

double wfunc_cos(const double *argv)
{
    return cos(argv[0]);
}

double wfunc_tan(const double *argv)
{
    return tan(argv[0]);
}



double wfunc_asin(const double *argv)
{
    return asin(argv[0]);
}

double wfunc_acos(const double *argv)
{
    return acos(argv[0]);
}

double wfunc_atan(const double *argv)
{
    return atan(argv[0]);
}



double wfunc_sinh(const double *argv)
{
    return sinh(argv[0]);
}

double wfunc_cosh(const double *argv)
{
    return cosh(argv[0]);
}

double wfunc_tanh(const double *argv)
{
    return tanh(argv[0]);
}



double wfunc_asinh(const double *argv)
{
    return asinh(argv[0]);
}

double wfunc_acosh(const double *argv)
{
    return acosh(argv[0]);
}

double wfunc_atanh(const double *argv)
{
    return atanh(argv[0]);
}



const std::unordered_map<std::string, std::tuple<unsigned short, unsigned short, operator_func>> additional_operators = {  //acronym, number of operands, precedence value, function pointer
    {"root", {2, 1, wfunc_root}},
    {"sqrt", {1, 1, wfunc_sqrt}},
    {"cbrt", {1, 1, wfunc_cbrt}},

    {"^", {2, 0, wfunc_pow}},
    {"sqr", {1, 1, wfunc_sqr}},
    {"cube", {1, 1, wfunc_cube}},

    {"logb", {2, 1, wfunc_logb}},
    {"log", {1, 1, wfunc_log}},
    {"ln", {1, 1, wfunc_ln}},

    //trigonometric functions
    {"sin", {1, 1, wfunc_sin}},
    {"cos", {1, 1, wfunc_cos}},
    {"tan", {1, 1, wfunc_tan}},

    {"asin", {1, 1, wfunc_asin}},
    {"acos", {1, 1, wfunc_acos}},
    {"atan", {1, 1, wfunc_atan}},

    {"sinh", {1, 1, wfunc_sinh}},
    {"cosh", {1, 1, wfunc_cosh}},
    {"tanh", {1, 1, wfunc_tanh}},

    {"asin", {1, 1, wfunc_asinh}},
    {"acos", {1, 1, wfunc_acosh}},
    {"atan", {1, 1, wfunc_atanh}}
};
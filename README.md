# rpn_utils C++ Library

## 1. Overview

rpn_utils is a C++ library that provides utilities for converting any mathematical expressions from infix notation to Reverse Polish Notation (RPN) and for evaluating RPN expressions.


### The library consists of two additional modules:

#### Additional Operands *<sub> (internal library module)</sub>*
This module allows you to define additional operands (besides the classical numbers) by specifying:
- the _name of the operand_ (`std::string` that must contain __ONLY__ lowercase characters!)
- the _value of the operand_ (`double`).<br /><br />
##### Add/Remove additional operands:
To _add_ or _remove_ additional operands, two functions are available:
- __Add operand__:<br />
  `bool add_operand(const std::string &operand_name, double operand_value)`

  _returns_:
    - __true:__ if the operand has been added
    - __false:__ if the operand name does not consist of lowercase letters only or if the operand value is NaN or inf. In this case the operand is not added.<br />
    
  __Note:__ If the operand already exists only its associated value is changed.

- __Remove an operand__:<br />
  `bool remove_operand(const std::string &operand_name)`

  _returns_:
    - __true:__ if the operand has been removed.
    - __false:__ if the operand was not present before being removed.
<br />

##### Get/Clear all additional operands:
To _get_ or _delete_ all additional operands, two functions are available:
- __Get all operands__:<br />
  `const std::unordered_map<std::string, double> &get_all_operands()`
  
  _returns_ a constant reference to the std::unordered_map object where all additional operands are stored.
  
- __Clear all operands__:<br />
  `void clear_all_operands()`
<br /><br />
#### Additional Operators *<sub> (external library module)</sub>*
This module allows you to define additional operators (besides the four operations +, -, *, /) by specifying:
- the _name of the operator_ (`std::string` that must contain __ONLY__ lowercase characters!).
- the _number of operands_ it takes (`unsigned short`)
- the _precedence value_ with respect to the other additional operators (`unsigned short`, any value)
- A _function pointer_ with the following signature `double func_name (const double *argv)` that will perform a set of operations that we want our new operator to perform.

__Note:__ The function that the operator performs in case it does not call a `cmath` library function directly, must handle error checking (using the `std::feraiseexcept` function of the `cfenv` library) on the arguments passed, with respect to the domain of the function that the operator performs.<br />
In particular it is necessary to use the `std::feraiseexcept` function __ONLY__ with these flags: `FE_INVALID`, `FE_DIVBYZERO`, `FE_UNDERFLOW` `FE_OVERFLOW`

__Note:__ You cannot change the list of additional operators at runtime.
<br /><br />
Some examples on the use of these modules are in the __Advanced Examples__ section of this file.
<br />
## 2. Installation
To use the rpn_utils library, you need to copy all header files (.hpp) and all source files (.cpp) to the same folder.<br />Then include the `rpn_utils.hpp` file in your project.

## 3. Usage

### Converting from infix to RPN

To convert a mathematical infix expression to RPN, simply call the function<br />
`bool infix_to_rpn(const std::string infix_expr, std::vector<std::string> &rpn_expr)`<br />
passing it as arguments:
- The infix expression (`std::string`)
- A `std::vector<std::string>` that at the end of the operation will contain the RPN expression.

<br />The function returns a boolean value as follows:
- __true:__ if the conversion operation was successful and the generated RPN expression is correct.
- __false:__ if the generated RPN expression is invalid due to errors in the passed infix expression.<br /><br />

### Evaluating an RPN expression

To evaluate an RPN expression, simply call the function<br />
`std::pair<bool, double> evaluate(const std::vector<std::string> &rpn_expr)`<br />
passing it as arguments:

- The RPN expression (`std::vector<std::string>`).

The function returns a `std::pair<bool, double>` representing:
- __first__ (`bool`):

    - __true:__ if the evaluation of all operators was possible with the supplied operands.
    - __false:__ if the evaluation of the expression was not possible because the domain of some operator was not met.

- __second__ (`double`): Contains the result of the expression (`double`). It is possible to use this value ONLY if __first__ is __true__.

## 4. Basic Examples

### Examples 1: Converting from infix to RPN
```cpp
std::vector<std::string> rpn_expr;
std::string infix_expr = "sin 1.57 - sqrt (2.1 ^ 4)";
bool valid = rpn::infix_to_rpn(infix_expr, rpn_expr);

if(valid){
    //use your rpn expression
}
```

### Examples 2: Evaluating an RPN expression
```cpp
std::vector<std::string> rpn_expr = {"3", "2", "5", "*", "+"};
std::pair<bool, double> result = rpn::evaluate(rpn_expr);
if(result.first)
    std::cout << result.second << std::endl;  // 13
```

### Examples 3: Adding/Removing additional operands
```cpp
rpn::add_operand("pi", M_PI);  //adds an operand by calling it "pi" and giving it value M_PI (constant in the cmath library)
rpn::add_operand("e", M_E);  //adds an operand by calling it "e" and giving it value M_E (constant in the cmath library)
rpn::add_operand("foo", 2.4);  //adds an operand by calling it "foo" and giving it value 2.4
rpn::add_operand("bar", 3.1);  //adds an operand by calling it "bar" and giving it value 3.1

rpn::remove_operand("foo");  //removes the operand "foo"
rpn::remove_operand("e");  //removes the operand "e"

rpn::clear_all_operands();  //removes all the operands added
```

## 5. Advanced Examples

### Examples 1: Convert an infix expression (containing variables) to RPN
```cpp
std::vector<std::string> rpn_expr;

//first we add the variables present in the RPN expression with any value 
if(!rpn::add_operand("foo", 0.0) || !rpn::add_operand("bar", 0.0))  //in this case it's useless to do the check
    return 1;

std::string infix_expr = "foo * sin bar - log (foo / bar)";
bool valid = rpn::infix_to_rpn(infix_expr, rpn_expr);

//if we will no longer use these two variables we can remove them from the additional operands
rpn::remove_operand("foo");
rpn::remove_operand("bar");
//or use rpn::clear_all_operands();

if(valid){
    //use your rpn expression
}
```

### Examples 2: Evaluate an RPN expression (containing variables)
```cpp
std::vector<std::string> rpn_expr = {"foo", "bar", "sin", "*", "foo", "bar", "/", "log", "-"};

//first we choose the value to give to the variables for which to evaluate the expression
if(!rpn::add_operand("foo", 9.2) || !rpn::add_operand("bar", 4.7))  //in this case it's useless to do the check
    return 1;

std::pair<bool, double> result = rpn::evaluate(rpn_expr);

//if we will no longer use these two variables we can remove them from the additional operands
rpn::remove_operand("foo");
rpn::remove_operand("bar");
//or use rpn::clear_all_operands();

if(result.first)
    std::cout << result.second << std::endl;  // -9.49098
```

### Examples 3: Adding an additional operator
Let us pretend that we want to add a foobar operator that performs the following operation:<br />
`log x / x` (where _log_ means _log10_)<br />

To do this we must as first thing write our function that is to be executed when we want to evaluate our operator.<br />
The function has to have the following signature:<br />
`double func_name (const double *)`<br />

In this case when we write our function (since it is not just calling a cmath function but is performing operations chosen by us) we have to do error checking only on the part added by us, i.e. we have to check only if our argument is different from 0, to avoid a division by 0.<br />
_Note that we are not doing error checking on the logarithm calculation part because the cmath library takes care of that._

Our function for the `foobar` operator is:
```cpp
double wfunc_foobar(const double *argv)
{
    if(argv[0] == 0)
        std::feraiseexcept(FE_DIVBYZERO);
    return log10(argv[0]) / argv[0];
}
```
Since our function was taking as input only one operand we used only `argv[0]`. If our function had taken 2 operands as input, then we would also have had to use `argv[1]`.<br />
Note that when an additional operator is evaluated, all the operands needed will be found from `argv[0]` to `argv[number_of_operands_needed - 1]` in order.

Now that we have written our function we can add an element to the std::unordered_map as follows:<br />
`{operator_name, {n operands, precedence_over_other_additional_operators, pointer_to_the_function_to_be_executed}}`<br />
ie:<br />
`{"foobar", {1, 1, wfunc_foobar}}`

As a precedence value, we can choose any value greater equal than 0; in this case we wanted to give our foobar operator precedence equal to almost all other operators (the ^ (power-elevation) operator has precedence value 0), i.e., 1.<br />
If we had wanted the operator to have precedence over all others we would have had to put any value greater equal to 2.<br />

__At this point we can use our operator in both an infix expression and an RPN:__

##### Converting from infix to RPN
```cpp
std::vector<std::string> rpn_expr;

std::string infix_expr = "foobar 2.4";
bool valid = rpn::infix_to_rpn(infix_expr, rpn_expr);

if(valid){
    //use your rpn expression
}
```

##### Evaluating an RPN expression
```cpp
std::vector<std::string> rpn_expr = {"2.4", "foobar"};
std::pair<bool, double> result = rpn::evaluate(rpn_expr);

if(result.first)
    std::cout << result.second << std::endl;  // 0.158421
```
## 6. Full Examples
There are some complete examples in the [examples](https://github.com/ernestocesario/rpn-utils/blob/main/examples) folder showing the use of the library

## 7. License
See more in the [License](https://github.com/ernestocesario/rpn-utils/blob/main/LICENSE) file


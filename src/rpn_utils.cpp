/**
 * @file rpn_utils.cpp
 * @brief Implementation file for the rpn_utils library
 * @author ernestocesario
 * @date 2023-02-21
 * @license Apache License 2.0
 */


#include "rpn_utils.hpp"

namespace rpn
{
    namespace
    {
        //Additional Operands object
        std::unordered_map<std::string, double> additional_operands;

        //index constants to access the tuple of additional_operators
        const unsigned short IDX_N_OPERANDS = 0;
        const unsigned short IDX_PRECEDENCE_VAL = 1;
        const unsigned short IDX_FUNC_PTR = 2;

        const unsigned short PRECEDENCE_VAL_OTHER = 0;
        const unsigned short PRECEDENCE_VAL_SUM = PRECEDENCE_VAL_OTHER + 1;
        const unsigned short PRECEDENCE_VAL_MULTIPLICATION = PRECEDENCE_VAL_SUM + 1;
        const unsigned short PRECEDENCE_VAL_FUNC_OPERATOR = PRECEDENCE_VAL_MULTIPLICATION + 1;

        //Exceptions
        const std::string EXCP_GENERAL_ERROR = "Something went wrong!";
        const std::string EXCP_INVALID_OPERAND = " --> invalid operand!";
        const std::string EXCP_MISSING_FUNCTION_ARGUMENT = " --> function argument not found!";
        const std::string EXCP_UNKNOWN_COMPONENT = " --> unknown operand/operator!";

        enum class ObjType{NO_TYPE = 0, OPERAND, OPERATOR, OPEN_PARENTHESIS, CLOSE_PARENTHESIS};


        bool check_parenthesis(const std::string &infix_expr);  //check that each parenthesis has the corresponding one
        void adj_expr(std::string &infix_expr);  //removes unnecessary space and tab characters from an expression
        ObjType getOp(std::string &infix_expr, std::string &obj_value, std::string::size_type &index);  //gets the next operand/operator and updates the index to the next character
        unsigned short precedence(const std::string &obj_value);  //gets the operator precedence value.
        bool isAdditionalOperand(const std::string &obj_val); //returns true if the string is an additional operand
        bool isOperand(const std::string &obj_value);  //returns true if the string is an operand
        bool isSign(char);  //returns true if the character is a sign
        bool fetch_val(const std::string &infix_expr, std::string &obj_value, std::string::size_type &index); //fetchs the next value (operand, operator, sign, ...) from the infix expression
        ObjType getType(const std::string &obj_value); //Returns the type (operand, operator, ...) of the passed value
        void enclose_negative(std::string &infix_expr, std::string::size_type index, const std::string &next_value);  //modifies the expression to handle negative blocks
        void skipParenthesis(const std::string &infix_expr, std::string::size_type &index);  //given an index pointing to an open parenthesis, skips all characters until it finds a closed parenthesis
        bool isFuncOperator(const std::string &obj_value);  //returns true if the string is a function operator (ie additional operator)
        bool isBasicOperator(const std::string &obj_value);  //returns true if the string is an operator (+, -, *, /)
        unsigned short get_precedence_func_operator(const std::string &func_operator);  //returns the precedence value of the function operator
        unsigned short get_operands_func_operator(const std::string &func_operator);  //returns the number of operands requested by the operator
        bool checkRpn(const std::vector<std::string> &rpn_expr);  //checks if the rpn expression is valid
        double get_value_additionalOperand(const std::string &obj_val);  //returns the value (double) associated with an additional operand.
        std::pair<bool, double> eval_func_operator(const std::string &obj_val, const double *operands);  //Evaluates a function operator and returns a <bool, double> pair, where the bool value is true if the operator is defined for the passed operands and the double value is the result of the evaluation.


        bool check_parenthesis(const std::string &expr)
        {
            long open_brackets = 0;

            for(std::string::const_iterator it = expr.cbegin(); it != expr.cend(); ++it){
                if(*it == '(')
                    ++open_brackets;
                else if(*it == ')'){
                    if(!open_brackets)
                        return false;
                    --open_brackets;
                }
            }

            return open_brackets == 0;
        }

        void adj_expr(std::string &expr)
        {
            if(expr.empty())
                return;

            std::string::size_type i;

            bool need_space = false;
            for(i = 0; i < expr.size();){
                
                switch(expr[i]){
                    case '\t':
                        expr[i] = ' ';
                        break;

                    case '{': case '[':
                        expr[i] = '(';
                        break;
                        
                    case '}': case ']':
                        expr[i] = ')';
                        break;
                }

                if(expr[i] == ' '){
                    if(!need_space)
                        expr.erase(i, 1);
                    else{
                        need_space = false;
                        ++i;
                    }
                }
                else{
                    std::string::size_type j;
                    for(j = i + 1; j < expr.size() && expr[j] == ' '; ++j)
                        ;
                    std::string::size_type space_between = (j - i) - 1;
                    if(space_between > 0 && ((islower(expr[i]) && islower(expr[j])) || (isdigit(expr[i]) && (isdigit(expr[j]) || expr[j] == '.'))))
                        need_space = true;
                    std::string::size_type space_to_remove = space_between - ((need_space) ? 1 : 0);
                    if(space_to_remove > 0)
                        expr.erase(i + 1, space_to_remove);
                    ++i;
                }
            }
        }
        
        ObjType getOp(std::string &expr, std::string &obj_val, std::string::size_type &index)
        {
            static std::string actual_val("");  //to keep the current value for the next call
            obj_val.clear();

            const std::string prev_val(actual_val); //prev_val 
            
            bool res = fetch_val(expr, obj_val, index);  //fetchs the actual_value
            actual_val = obj_val;

            if(!res)
                return ObjType::NO_TYPE;

            if(isOperand(obj_val))
                return ObjType::OPERAND;
            else if(isSign(obj_val.front())){
                /*
                If it is a sign we must check if the previous value is an operator,
                in this case if the sign is positive we can ignore it, otherwise we
                must modify the expression in order to transform the:
                - something
                in
                (0 - something)
                */

                std::string next_val;
                std::string::size_type next_index = index;

                if(!fetch_val(expr, next_val, next_index))
                    return ObjType::NO_TYPE; //there is an error in the infix expression

                ObjType prev_val_type = getType(prev_val);

                if(isSign(next_val.front())){  //synthesizes subexpressions such as "-+---+"
                    char curr_sign = obj_val.front();

                    do{
                        char next_sign = next_val.front();
                        expr.erase(index - 1, 2);
                        if(curr_sign == next_sign)
                            expr.insert(index - 1, "+");
                        else
                            expr.insert(index - 1, "-");

                        next_index -= 1;
                        curr_sign = expr[index - 1];

                    }while(fetch_val(expr, next_val, next_index) && isSign(next_val.front()));
                    obj_val = curr_sign;
                }

                if(prev_val_type == ObjType::OPERATOR || prev_val_type == ObjType::OPEN_PARENTHESIS || prev_val_type == ObjType::NO_TYPE){
                    if(obj_val.front() == '+')
                        obj_val = next_val;
                    else{
                        if(isOperand(next_val))
                            obj_val.append(next_val);
                        else{
                            actual_val = "(";
                            enclose_negative(expr, index - 1, next_val);
                            return ObjType::OPEN_PARENTHESIS;
                        }
                    }
                    actual_val = obj_val;
                    index = next_index;
                }
                return getType(obj_val);
            }
            else if(isBasicOperator(obj_val) || isFuncOperator(obj_val))
                return ObjType::OPERATOR;
            
            switch(obj_val.front()){
                case '(':
                    return ObjType::OPEN_PARENTHESIS;
                    break;
                case ')':
                    return ObjType::CLOSE_PARENTHESIS;
                    break;
                default:
                    throw std::runtime_error(obj_val + EXCP_UNKNOWN_COMPONENT);
            }
        }

        unsigned short precedence(const std::string &obj_val)
        {

            const unsigned short DEFAULT_PRECEDENDE_FUNC_OPERATOR = 3;
            if(isFuncOperator(obj_val))
                return PRECEDENCE_VAL_FUNC_OPERATOR + get_precedence_func_operator(obj_val);
            
            switch(obj_val.front()){
                case '+': case '-':
                    return PRECEDENCE_VAL_SUM;
                    break;
                case '*': case '/':
                    return PRECEDENCE_VAL_MULTIPLICATION;
                    break;
                default:
                    return PRECEDENCE_VAL_OTHER;
                    break;
            }
        }

        bool isAdditionalOperand(const std::string &obj_val)
        {
            return additional_operands.find(obj_val) != additional_operands.cend();
        }

        bool isOperand(const std::string &obj_val)
        {
            if(obj_val.empty())
                return false;
            
            if(isAdditionalOperand(obj_val))
                return true;

            std::string::size_type i = 0;

            if(isSign(obj_val.front()) && obj_val.size() > 1)
                ++i;

            for(; i < obj_val.size(); ++i)
                if(!isdigit(obj_val[i]) && obj_val[i] != '.')
                    return false;
            
            return true;
        }

        bool isSign(char c)
        {
            return (c == '+' || c == '-') ? true : false;
        }

        bool fetch_val(const std::string &expr, std::string &curr_val, std::string::size_type &index)
        {   
            curr_val.clear();

            if(index < expr.size() && expr[index] == ' ')
                ++index;

            if(index >= expr.size())
                return false;


            if(islower(expr[index])){
                while(index < expr.size() && islower(expr[index]))
                    curr_val.push_back(expr[index++]);
            }
            else if(isdigit(expr[index]) || expr[index] == '.'){
                curr_val.push_back(expr[index]);
                ++index;

                while(index < expr.size() && (isdigit(expr[index]) || expr[index] == '.'))
                    curr_val.push_back(expr[index++]);
                if(std::count(curr_val.begin(), curr_val.end(), '.') > 1)
                    throw std::runtime_error(curr_val + EXCP_INVALID_OPERAND);
            }
            else{
                curr_val.push_back(expr[index++]);
            }

            return true;
        }

        ObjType getType(const std::string &obj_value)
        {
            if(obj_value.empty())
                return ObjType::NO_TYPE;
            else if(isOperand(obj_value))
                return ObjType::OPERAND;
            else if(isBasicOperator(obj_value) || isFuncOperator(obj_value))
                return ObjType::OPERATOR;
            else if(obj_value == "(")
                return ObjType::OPEN_PARENTHESIS;
            else if(obj_value == ")")
                return ObjType::CLOSE_PARENTHESIS;
            else
                return ObjType::NO_TYPE;
        }

        void enclose_negative(std::string &expr, std::string::size_type index, const std::string &next_val)
        {
            const std::string prefix = "(0";  //prefix to add to the block
            const std::string suffix = ")";  //suffix to append to the block

            expr.insert(index, prefix);  //Now index point to '('
            
            index += (prefix.size() + 1); //+1 so it points one character after the '-'

            ObjType next_val_type = getType(next_val);


            //May have only 2 cases either '(' or an operator (function).

            if(next_val_type == ObjType::OPEN_PARENTHESIS){
                skipParenthesis(expr, index);
            }
            else{  //operator (function) case
                unsigned short n_operands_required = get_operands_func_operator(next_val);
                std::string tmp_val;
                ObjType tmp_val_type; 

                for(unsigned operands_found = 0; operands_found < n_operands_required; ++operands_found){
                    bool found_operand = false;
                    while(!found_operand && fetch_val(expr, tmp_val, index)){
                        tmp_val_type = getType(tmp_val);
                        if(tmp_val_type == ObjType::OPERAND || tmp_val_type == ObjType::OPEN_PARENTHESIS)
                            found_operand = true;
                    }

                    if(!found_operand)  //if it doesn't find the function argument
                        throw std::runtime_error(next_val + EXCP_MISSING_FUNCTION_ARGUMENT);

                    switch(tmp_val_type){
                        case ObjType::OPEN_PARENTHESIS:  //the function argument is a block in parentheses
                            --index; //because index points to a character after '(' because of fetch_val
                            skipParenthesis(expr, index);
                            break;
                        case ObjType::OPERAND:  //the function argument is an operand
                            //nothing to do here, because fetch_val has already positioned index so that it points one character after the operand
                            break;
                    }
                }

            }
            expr.insert(index, suffix);
        }

        void skipParenthesis(const std::string &expr, std::string::size_type &index)
        {
            if(expr[index] != '(')
                return;
            
            ++index;

            for(unsigned inner_parenthesis = 0; index < expr.size() && (expr[index] != ')' || inner_parenthesis); ++index){
                if(expr[index] == '(')
                    ++inner_parenthesis;
                else if(expr[index] == ')')
                    --inner_parenthesis;
            }
        }

        bool isFuncOperator(const std::string &obj_val)
        {
            if(obj_val.empty())
                return false;
            
            if(additional_operators.find(obj_val) != additional_operators.cend())
                return true;
            
            return false;
        }

        bool isBasicOperator(const std::string &obj_val){
            if(obj_val.size() != 1)
                return false;
            
            switch(obj_val.front()){
                case '+': case '-': case '*': case '/':
                    return true;
                    break;
                default:
                    return false;
                    break;
            }
        }

        unsigned short get_precedence_func_operator(const std::string &obj_val)
        {
            if(additional_operators.find(obj_val) != additional_operators.cend())
                return std::get<IDX_PRECEDENCE_VAL>(additional_operators.at(obj_val));

            throw std::runtime_error(EXCP_GENERAL_ERROR);
        }

        unsigned short get_operands_func_operator(const std::string &obj_val)
        {
            if(additional_operators.find(obj_val) != additional_operators.cend())
                return std::get<IDX_N_OPERANDS>(additional_operators.at(obj_val));

            throw std::runtime_error(EXCP_GENERAL_ERROR);
        }

        bool checkRpn(const std::vector<std::string> &rpn_expr)
        {
            long checker = 0;

            for(std::vector<std::string>::const_iterator it = rpn_expr.cbegin(); it != rpn_expr.cend(); ++it){
                if(isOperand(*it))
                    ++checker;
                else if(isBasicOperator(*it))  //because all basic operators (+, -, *, /) take 2 operands
                    --checker;
                else if(isFuncOperator(*it))  //we should check how many operands a func operator take
                    checker -= (get_operands_func_operator(*it) - 1);
                else
                    return false;

                if(checker <= 0)
                    return false;
            }

            return checker == 1;
        }

        double get_value_additionalOperand(const std::string &obj_val)
        {
            if(!isAdditionalOperand(obj_val))
                throw std::runtime_error(EXCP_GENERAL_ERROR);

            return additional_operands.at(obj_val);
        }

        std::pair<bool, double> eval_func_operator(const std::string &obj_val, const double *operands)
        {
            if(additional_operators.find(obj_val) == additional_operators.cend())
                throw std::runtime_error(EXCP_GENERAL_ERROR);
            
            operator_func wfunc = std::get<IDX_FUNC_PTR>(additional_operators.at(obj_val));
            double result;
            bool defined = true;
            
            std::feclearexcept(FE_ALL_EXCEPT);
            result = (*wfunc)(operands);

            if(std::fetestexcept(FE_INVALID | FE_DIVBYZERO | FE_UNDERFLOW | FE_OVERFLOW))
                defined = false;
        
            return std::make_pair(defined, result);
        }
    }

    
    bool infix_to_rpn(const std::string &infix_expr, std::vector<std::string> &rpn_expr)
    {
        std::stack<std::string> op;
        rpn_expr.clear();
        std::string m_infix_expr(infix_expr);
        adj_expr(m_infix_expr);
        ObjType type;
        std::string obj_val;

        std::string::size_type index = 0;

        while((type = getOp(m_infix_expr, obj_val, index)) != ObjType::NO_TYPE){
            switch(type){
                case ObjType::OPERAND:
                    rpn_expr.push_back(obj_val);
                    break;
                
                case ObjType::OPERATOR:
                    while(!op.empty() && precedence(obj_val) < precedence(op.top())){
                        rpn_expr.push_back(op.top());
                        op.pop();
                    }
                    op.push(obj_val);
                    break;

                case ObjType::OPEN_PARENTHESIS:
                    op.push("(");
                    break;
                
                case ObjType::CLOSE_PARENTHESIS:
                    while(!op.empty() && op.top() != "("){
                        rpn_expr.push_back(op.top());
                        op.pop();
                    }

                    op.pop();
                    break;
            }
        }

        while(!op.empty()){
            rpn_expr.push_back(op.top());
            op.pop();
        }

        if(!checkRpn(rpn_expr)){
            rpn_expr.clear();
            return false;
        }
        return true;
    }

    std::pair<bool, double> evaluate(const std::vector<std::string> &expr)
    {
        if(!checkRpn(expr))
            return std::make_pair(false, 0.0);
        
        bool isDefined = true;
        std::stack<double> operands;

        for(std::vector<std::string>::const_iterator it = expr.cbegin(); isDefined && it != expr.cend(); ++it){
            if(isOperand(*it)){
                double tmp;
                if(isAdditionalOperand(*it))
                    tmp = get_value_additionalOperand(*it);
                else
                    tmp = std::stod(*it);
                
                operands.push(tmp);
            }
            else if(isBasicOperator(*it)){
                double op2 = operands.top();
                operands.pop();
                double op1 = operands.top();
                operands.pop();

                double result;

                switch(it->front()){
                    case '+':
                        result = op1 + op2;
                        break;

                    case '-':
                        result = op1 - op2;
                        break;
                    
                    case '*':
                        result = op1 * op2;
                        break;
                    
                    case '/':
                        if(op2 == 0)
                            isDefined = false;
                        result = op1 / op2;
                        break;
                }

                operands.push(result);
            }
            else if(isFuncOperator(*it)){
                unsigned short n_operands_required = get_operands_func_operator(*it);
                std::vector<double> op(n_operands_required);

                for(unsigned short i = n_operands_required - 1; i < n_operands_required; --i){
                    op[i] = operands.top();
                    operands.pop();
                }
                std::pair<bool, double> result = eval_func_operator(*it, op.data());

                if(!result.first)
                    isDefined = false;
                else
                    operands.push(result.second);
            }
        }
        
        if(!isDefined)
            return std::make_pair(false, 0.0);

        if(operands.size() != 1)
            throw std::runtime_error(EXCP_GENERAL_ERROR);
        
        return std::make_pair(true, operands.top());
    }

    bool add_operand(const std::string &op_name, double op_value)
    {
        if(std::isinf(op_value) || std::isnan(op_value))
            return false;
        
        for(std::string::const_iterator it = op_name.cbegin(); it != op_name.cend(); ++it)
            if(!islower(*it))
                return false;
        
        additional_operands[op_name] = op_value;
        return true;
    }

    bool remove_operand(const std::string &op_name)
    {
        if(!isAdditionalOperand(op_name))
            return false;
        
        additional_operands.erase(op_name);
        return true;
    }

    double get_operand(const std::string &op_name)
    {
        if(!isAdditionalOperand(op_name))
            return 0.0;
        return additional_operands[op_name];
    }

    const std::unordered_map<std::string, double> &get_all_operands()
    {
        return additional_operands;
    }

    void clear_all_operands()
    {
        additional_operands.clear();
    }
}
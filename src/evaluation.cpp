/**
 * @file evaluation.cpp
 * @brief Expression evaluation implementation for the Scheme interpreter
 * @author luke36
 * 
 * This file implements evaluation methods for all expression types in the Scheme
 * interpreter. Functions are organized according to ExprType enumeration order
 * from Def.hpp for consistency and maintainability.
 */

#include "value.hpp"
#include "expr.hpp"
#include "RE.hpp"
#include "syntax.hpp"
#include <cstring>
#include <vector>
#include <map>
#include <climits>

extern std::map<std::string, ExprType> primitives;
extern std::map<std::string, ExprType> reserved_words;

// Global environment from main.cpp
extern Assoc global_env;

Value Fixnum::eval(Assoc &e) { // evaluation of a fixnum
    return IntegerV(n);
}

Value RationalNum::eval(Assoc &e) { // evaluation of a rational number
    return RationalV(numerator, denominator);
}

Value StringExpr::eval(Assoc &e) { // evaluation of a string
    return StringV(s);
}

Value True::eval(Assoc &e) { // evaluation of #t
    return BooleanV(true);
}

Value False::eval(Assoc &e) { // evaluation of #f
    return BooleanV(false);
}

Value MakeVoid::eval(Assoc &e) { // (void)
    return VoidV();
}

Value Exit::eval(Assoc &e) { // (exit)
    return TerminateV();
}

Value Unary::eval(Assoc &e) { // evaluation of single-operator primitive
    return evalRator(rand->eval(e));
}

Value Binary::eval(Assoc &e) { // evaluation of two-operators primitive
    return evalRator(rand1->eval(e), rand2->eval(e));
}

Value Variadic::eval(Assoc &e) { // evaluation of multi-operator primitive
    std::vector<Value> values;
    for (const auto& rand : rands) {
        values.push_back(rand->eval(e));
    }
    return evalRator(values);
}

Value Var::eval(Assoc &e) { // evaluation of variable
    // Check if variable is defined in the environment
    Value matched_value = find(x, e);
    if (matched_value.get() == nullptr) {
        // Variable not found - throw RuntimeError
        throw RuntimeError("undefined variable " + x + " in the current scope");
    }
    return matched_value;
}

Value Plus::evalRator(const Value &rand1, const Value &rand2) { // +
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        int n1 = dynamic_cast<Integer*>(rand1.get())->n;
        int n2 = dynamic_cast<Integer*>(rand2.get())->n;
        return IntegerV(n1 + n2);
    } else if (rand1->v_type == V_RATIONAL && rand2->v_type == V_RATIONAL) {
        Rational* r1 = dynamic_cast<Rational*>(rand1.get());
        Rational* r2 = dynamic_cast<Rational*>(rand2.get());
        int num = r1->numerator * r2->denominator + r2->numerator * r1->denominator;
        int den = r1->denominator * r2->denominator;
        return RationalV(num, den);
    } else if (rand1->v_type == V_INT && rand2->v_type == V_RATIONAL) {
        int n1 = dynamic_cast<Integer*>(rand1.get())->n;
        Rational* r2 = dynamic_cast<Rational*>(rand2.get());
        int num = n1 * r2->denominator + r2->numerator;
        int den = r2->denominator;
        return RationalV(num, den);
    } else if (rand1->v_type == V_RATIONAL && rand2->v_type == V_INT) {
        Rational* r1 = dynamic_cast<Rational*>(rand1.get());
        int n2 = dynamic_cast<Integer*>(rand2.get())->n;
        int num = r1->numerator + n2 * r1->denominator;
        int den = r1->denominator;
        return RationalV(num, den);
    }
    throw(RuntimeError("Wrong typename"));
}

Value Minus::evalRator(const Value &rand1, const Value &rand2) { // -
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        int n1 = dynamic_cast<Integer*>(rand1.get())->n;
        int n2 = dynamic_cast<Integer*>(rand2.get())->n;
        return IntegerV(n1 - n2);
    } else if (rand1->v_type == V_RATIONAL && rand2->v_type == V_RATIONAL) {
        Rational* r1 = dynamic_cast<Rational*>(rand1.get());
        Rational* r2 = dynamic_cast<Rational*>(rand2.get());
        int num = r1->numerator * r2->denominator - r2->numerator * r1->denominator;
        int den = r1->denominator * r2->denominator;
        return RationalV(num, den);
    } else if (rand1->v_type == V_INT && rand2->v_type == V_RATIONAL) {
        int n1 = dynamic_cast<Integer*>(rand1.get())->n;
        Rational* r2 = dynamic_cast<Rational*>(rand2.get());
        int num = n1 * r2->denominator - r2->numerator;
        int den = r2->denominator;
        return RationalV(num, den);
    } else if (rand1->v_type == V_RATIONAL && rand2->v_type == V_INT) {
        Rational* r1 = dynamic_cast<Rational*>(rand1.get());
        int n2 = dynamic_cast<Integer*>(rand2.get())->n;
        int num = r1->numerator - n2 * r1->denominator;
        int den = r1->denominator;
        return RationalV(num, den);
    }
    throw(RuntimeError("Wrong typename"));
}

Value Mult::evalRator(const Value &rand1, const Value &rand2) { // *
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        int n1 = dynamic_cast<Integer*>(rand1.get())->n;
        int n2 = dynamic_cast<Integer*>(rand2.get())->n;
        return IntegerV(n1 * n2);
    } else if (rand1->v_type == V_RATIONAL && rand2->v_type == V_RATIONAL) {
        Rational* r1 = dynamic_cast<Rational*>(rand1.get());
        Rational* r2 = dynamic_cast<Rational*>(rand2.get());
        int num = r1->numerator * r2->numerator;
        int den = r1->denominator * r2->denominator;
        return RationalV(num, den);
    } else if (rand1->v_type == V_INT && rand2->v_type == V_RATIONAL) {
        int n1 = dynamic_cast<Integer*>(rand1.get())->n;
        Rational* r2 = dynamic_cast<Rational*>(rand2.get());
        int num = n1 * r2->numerator;
        int den = r2->denominator;
        return RationalV(num, den);
    } else if (rand1->v_type == V_RATIONAL && rand2->v_type == V_INT) {
        Rational* r1 = dynamic_cast<Rational*>(rand1.get());
        int n2 = dynamic_cast<Integer*>(rand2.get())->n;
        int num = r1->numerator * n2;
        int den = r1->denominator;
        return RationalV(num, den);
    }
    throw(RuntimeError("Wrong typename"));
}

Value Div::evalRator(const Value &rand1, const Value &rand2) { // /
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        int n1 = dynamic_cast<Integer*>(rand1.get())->n;
        int n2 = dynamic_cast<Integer*>(rand2.get())->n;
        if (n2 == 0) {
            throw(RuntimeError("Division by zero"));
        }
        return RationalV(n1, n2);
    } else if (rand1->v_type == V_RATIONAL && rand2->v_type == V_RATIONAL) {
        Rational* r1 = dynamic_cast<Rational*>(rand1.get());
        Rational* r2 = dynamic_cast<Rational*>(rand2.get());
        if (r2->numerator == 0) {
            throw(RuntimeError("Division by zero"));
        }
        int num = r1->numerator * r2->denominator;
        int den = r1->denominator * r2->numerator;
        return RationalV(num, den);
    } else if (rand1->v_type == V_INT && rand2->v_type == V_RATIONAL) {
        int n1 = dynamic_cast<Integer*>(rand1.get())->n;
        Rational* r2 = dynamic_cast<Rational*>(rand2.get());
        if (r2->numerator == 0) {
            throw(RuntimeError("Division by zero"));
        }
        int num = n1 * r2->denominator;
        int den = r2->numerator;
        return RationalV(num, den);
    } else if (rand1->v_type == V_RATIONAL && rand2->v_type == V_INT) {
        Rational* r1 = dynamic_cast<Rational*>(rand1.get());
        int n2 = dynamic_cast<Integer*>(rand2.get())->n;
        if (n2 == 0) {
            throw(RuntimeError("Division by zero"));
        }
        int num = r1->numerator;
        int den = r1->denominator * n2;
        return RationalV(num, den);
    }
    throw(RuntimeError("Wrong typename"));
}

Value Modulo::evalRator(const Value &rand1, const Value &rand2) { // modulo
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        int dividend = dynamic_cast<Integer*>(rand1.get())->n;
        int divisor = dynamic_cast<Integer*>(rand2.get())->n;
        if (divisor == 0) {
            throw(RuntimeError("Division by zero"));
        }
        return IntegerV(dividend % divisor);
    }
    throw(RuntimeError("modulo is only defined for integers"));
}

Value PlusVar::evalRator(const std::vector<Value> &args) { // + with multiple args
    if (args.empty()) {
        return IntegerV(0);  // (+) returns 0
    }

    Value result = args[0];
    for (size_t i = 1; i < args.size(); i++) {
        // Use binary Plus logic
        if (result->v_type == V_INT && args[i]->v_type == V_INT) {
            int n1 = dynamic_cast<Integer*>(result.get())->n;
            int n2 = dynamic_cast<Integer*>(args[i].get())->n;
            result = IntegerV(n1 + n2);
        } else if (result->v_type == V_RATIONAL && args[i]->v_type == V_RATIONAL) {
            Rational* r1 = dynamic_cast<Rational*>(result.get());
            Rational* r2 = dynamic_cast<Rational*>(args[i].get());
            int num = r1->numerator * r2->denominator + r2->numerator * r1->denominator;
            int den = r1->denominator * r2->denominator;
            result = RationalV(num, den);
        } else if (result->v_type == V_INT && args[i]->v_type == V_RATIONAL) {
            int n1 = dynamic_cast<Integer*>(result.get())->n;
            Rational* r2 = dynamic_cast<Rational*>(args[i].get());
            int num = n1 * r2->denominator + r2->numerator;
            int den = r2->denominator;
            result = RationalV(num, den);
        } else if (result->v_type == V_RATIONAL && args[i]->v_type == V_INT) {
            Rational* r1 = dynamic_cast<Rational*>(result.get());
            int n2 = dynamic_cast<Integer*>(args[i].get())->n;
            int num = r1->numerator + n2 * r1->denominator;
            int den = r1->denominator;
            result = RationalV(num, den);
        } else {
            throw RuntimeError("Wrong typename");
        }
    }
    return result;
}

Value MinusVar::evalRator(const std::vector<Value> &args) { // - with multiple args
    if (args.empty()) {
        throw RuntimeError("Wrong number of arguments for -");
    }
    if (args.size() == 1) {
        // Unary minus
        if (args[0]->v_type == V_INT) {
            int n = dynamic_cast<Integer*>(args[0].get())->n;
            return IntegerV(-n);
        } else if (args[0]->v_type == V_RATIONAL) {
            Rational* r = dynamic_cast<Rational*>(args[0].get());
            return RationalV(-r->numerator, r->denominator);
        }
        throw RuntimeError("Wrong typename");
    }

    // Multiple arguments: left-associative subtraction
    Value result = args[0];
    for (size_t i = 1; i < args.size(); i++) {
        // Use binary Minus logic
        if (result->v_type == V_INT && args[i]->v_type == V_INT) {
            int n1 = dynamic_cast<Integer*>(result.get())->n;
            int n2 = dynamic_cast<Integer*>(args[i].get())->n;
            result = IntegerV(n1 - n2);
        } else if (result->v_type == V_RATIONAL && args[i]->v_type == V_RATIONAL) {
            Rational* r1 = dynamic_cast<Rational*>(result.get());
            Rational* r2 = dynamic_cast<Rational*>(args[i].get());
            int num = r1->numerator * r2->denominator - r2->numerator * r1->denominator;
            int den = r1->denominator * r2->denominator;
            result = RationalV(num, den);
        } else if (result->v_type == V_INT && args[i]->v_type == V_RATIONAL) {
            int n1 = dynamic_cast<Integer*>(result.get())->n;
            Rational* r2 = dynamic_cast<Rational*>(args[i].get());
            int num = n1 * r2->denominator - r2->numerator;
            int den = r2->denominator;
            result = RationalV(num, den);
        } else if (result->v_type == V_RATIONAL && args[i]->v_type == V_INT) {
            Rational* r1 = dynamic_cast<Rational*>(result.get());
            int n2 = dynamic_cast<Integer*>(args[i].get())->n;
            int num = r1->numerator - n2 * r1->denominator;
            int den = r1->denominator;
            result = RationalV(num, den);
        } else {
            throw RuntimeError("Wrong typename");
        }
    }
    return result;
}

Value MultVar::evalRator(const std::vector<Value> &args) { // * with multiple args
    if (args.empty()) {
        return IntegerV(1);  // (*) returns 1
    }

    Value result = args[0];
    for (size_t i = 1; i < args.size(); i++) {
        // Use binary Mult logic
        if (result->v_type == V_INT && args[i]->v_type == V_INT) {
            int n1 = dynamic_cast<Integer*>(result.get())->n;
            int n2 = dynamic_cast<Integer*>(args[i].get())->n;
            result = IntegerV(n1 * n2);
        } else if (result->v_type == V_RATIONAL && args[i]->v_type == V_RATIONAL) {
            Rational* r1 = dynamic_cast<Rational*>(result.get());
            Rational* r2 = dynamic_cast<Rational*>(args[i].get());
            int num = r1->numerator * r2->numerator;
            int den = r1->denominator * r2->denominator;
            result = RationalV(num, den);
        } else if (result->v_type == V_INT && args[i]->v_type == V_RATIONAL) {
            int n1 = dynamic_cast<Integer*>(result.get())->n;
            Rational* r2 = dynamic_cast<Rational*>(args[i].get());
            int num = n1 * r2->numerator;
            int den = r2->denominator;
            result = RationalV(num, den);
        } else if (result->v_type == V_RATIONAL && args[i]->v_type == V_INT) {
            Rational* r1 = dynamic_cast<Rational*>(result.get());
            int n2 = dynamic_cast<Integer*>(args[i].get())->n;
            int num = r1->numerator * n2;
            int den = r1->denominator;
            result = RationalV(num, den);
        } else {
            throw RuntimeError("Wrong typename");
        }
    }
    return result;
}

Value DivVar::evalRator(const std::vector<Value> &args) { // / with multiple args
    if (args.empty()) {
        throw RuntimeError("Wrong number of arguments for /");
    }
    if (args.size() == 1) {
        // Unary division (reciprocal)
        if (args[0]->v_type == V_INT) {
            int n = dynamic_cast<Integer*>(args[0].get())->n;
            if (n == 0) {
                throw RuntimeError("Division by zero");
            }
            return RationalV(1, n);
        } else if (args[0]->v_type == V_RATIONAL) {
            Rational* r = dynamic_cast<Rational*>(args[0].get());
            if (r->numerator == 0) {
                throw RuntimeError("Division by zero");
            }
            return RationalV(r->denominator, r->numerator);
        }
        throw RuntimeError("Wrong typename");
    }

    // Multiple arguments: left-associative division
    Value result = args[0];
    for (size_t i = 1; i < args.size(); i++) {
        // Use binary Div logic
        if (result->v_type == V_INT && args[i]->v_type == V_INT) {
            int n1 = dynamic_cast<Integer*>(result.get())->n;
            int n2 = dynamic_cast<Integer*>(args[i].get())->n;
            if (n2 == 0) {
                throw RuntimeError("Division by zero");
            }
            result = RationalV(n1, n2);
        } else if (result->v_type == V_RATIONAL && args[i]->v_type == V_RATIONAL) {
            Rational* r1 = dynamic_cast<Rational*>(result.get());
            Rational* r2 = dynamic_cast<Rational*>(args[i].get());
            if (r2->numerator == 0) {
                throw RuntimeError("Division by zero");
            }
            int num = r1->numerator * r2->denominator;
            int den = r1->denominator * r2->numerator;
            result = RationalV(num, den);
        } else if (result->v_type == V_INT && args[i]->v_type == V_RATIONAL) {
            int n1 = dynamic_cast<Integer*>(result.get())->n;
            Rational* r2 = dynamic_cast<Rational*>(args[i].get());
            if (r2->numerator == 0) {
                throw RuntimeError("Division by zero");
            }
            int num = n1 * r2->denominator;
            int den = r2->numerator;
            result = RationalV(num, den);
        } else if (result->v_type == V_RATIONAL && args[i]->v_type == V_INT) {
            Rational* r1 = dynamic_cast<Rational*>(result.get());
            int n2 = dynamic_cast<Integer*>(args[i].get())->n;
            if (n2 == 0) {
                throw RuntimeError("Division by zero");
            }
            int num = r1->numerator;
            int den = r1->denominator * n2;
            result = RationalV(num, den);
        } else {
            throw RuntimeError("Wrong typename");
        }
    }
    return result;
}

Value Expt::evalRator(const Value &rand1, const Value &rand2) { // expt
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        int base = dynamic_cast<Integer*>(rand1.get())->n;
        int exponent = dynamic_cast<Integer*>(rand2.get())->n;
        
        if (exponent < 0) {
            throw(RuntimeError("Negative exponent not supported for integers"));
        }
        if (base == 0 && exponent == 0) {
            throw(RuntimeError("0^0 is undefined"));
        }
        
        long long result = 1;
        long long b = base;
        int exp = exponent;
        
        while (exp > 0) {
            if (exp % 2 == 1) {
                result *= b;
                if (result > INT_MAX || result < INT_MIN) {
                    throw(RuntimeError("Integer overflow in expt"));
                }
            }
            b *= b;
            if (b > INT_MAX || b < INT_MIN) {
                if (exp > 1) {
                    throw(RuntimeError("Integer overflow in expt"));
                }
            }
            exp /= 2;
        }
        
        return IntegerV((int)result);
    }
    throw(RuntimeError("Wrong typename"));
}

//A FUNCTION TO SIMPLIFY THE COMPARISON WITH INTEGER AND RATIONAL NUMBER
int compareNumericValues(const Value &v1, const Value &v2) {
    if (v1->v_type == V_INT && v2->v_type == V_INT) {
        int n1 = dynamic_cast<Integer*>(v1.get())->n;
        int n2 = dynamic_cast<Integer*>(v2.get())->n;
        return (n1 < n2) ? -1 : (n1 > n2) ? 1 : 0;
    }
    else if (v1->v_type == V_RATIONAL && v2->v_type == V_INT) {
        Rational* r1 = dynamic_cast<Rational*>(v1.get());
        int n2 = dynamic_cast<Integer*>(v2.get())->n;
        int left = r1->numerator;
        int right = n2 * r1->denominator;
        return (left < right) ? -1 : (left > right) ? 1 : 0;
    }
    else if (v1->v_type == V_INT && v2->v_type == V_RATIONAL) {
        int n1 = dynamic_cast<Integer*>(v1.get())->n;
        Rational* r2 = dynamic_cast<Rational*>(v2.get());
        int left = n1 * r2->denominator;
        int right = r2->numerator;
        return (left < right) ? -1 : (left > right) ? 1 : 0;
    }
    else if (v1->v_type == V_RATIONAL && v2->v_type == V_RATIONAL) {
        Rational* r1 = dynamic_cast<Rational*>(v1.get());
        Rational* r2 = dynamic_cast<Rational*>(v2.get());
        int left = r1->numerator * r2->denominator;
        int right = r2->numerator * r1->denominator;
        return (left < right) ? -1 : (left > right) ? 1 : 0;
    }
    throw RuntimeError("Wrong typename in numeric comparison");
}

Value Less::evalRator(const Value &rand1, const Value &rand2) { // <
    int result = compareNumericValues(rand1, rand2);
    return BooleanV(result < 0);
}

Value LessEq::evalRator(const Value &rand1, const Value &rand2) { // <=
    int result = compareNumericValues(rand1, rand2);
    return BooleanV(result <= 0);
}

Value Equal::evalRator(const Value &rand1, const Value &rand2) { // =
    int result = compareNumericValues(rand1, rand2);
    return BooleanV(result == 0);
}

Value GreaterEq::evalRator(const Value &rand1, const Value &rand2) { // >=
    int result = compareNumericValues(rand1, rand2);
    return BooleanV(result >= 0);
}

Value Greater::evalRator(const Value &rand1, const Value &rand2) { // >
    int result = compareNumericValues(rand1, rand2);
    return BooleanV(result > 0);
}

Value LessVar::evalRator(const std::vector<Value> &args) { // < with multiple args
    if (args.size() < 2) {
        throw RuntimeError("Wrong number of arguments for <");
    }

    for (size_t i = 0; i < args.size() - 1; i++) {
        int cmp = compareNumericValues(args[i], args[i+1]);
        if (cmp >= 0) {
            return BooleanV(false);
        }
    }
    return BooleanV(true);
}

Value LessEqVar::evalRator(const std::vector<Value> &args) { // <= with multiple args
    if (args.size() < 2) {
        throw RuntimeError("Wrong number of arguments for <=");
    }

    for (size_t i = 0; i < args.size() - 1; i++) {
        int cmp = compareNumericValues(args[i], args[i+1]);
        if (cmp > 0) {
            return BooleanV(false);
        }
    }
    return BooleanV(true);
}

Value EqualVar::evalRator(const std::vector<Value> &args) { // = with multiple args
    if (args.size() < 2) {
        throw RuntimeError("Wrong number of arguments for =");
    }

    for (size_t i = 0; i < args.size() - 1; i++) {
        int cmp = compareNumericValues(args[i], args[i+1]);
        if (cmp != 0) {
            return BooleanV(false);
        }
    }
    return BooleanV(true);
}

Value GreaterEqVar::evalRator(const std::vector<Value> &args) { // >= with multiple args
    if (args.size() < 2) {
        throw RuntimeError("Wrong number of arguments for >=");
    }

    for (size_t i = 0; i < args.size() - 1; i++) {
        int cmp = compareNumericValues(args[i], args[i+1]);
        if (cmp < 0) {
            return BooleanV(false);
        }
    }
    return BooleanV(true);
}

Value GreaterVar::evalRator(const std::vector<Value> &args) { // > with multiple args
    if (args.size() < 2) {
        throw RuntimeError("Wrong number of arguments for >");
    }

    for (size_t i = 0; i < args.size() - 1; i++) {
        int cmp = compareNumericValues(args[i], args[i+1]);
        if (cmp <= 0) {
            return BooleanV(false);
        }
    }
    return BooleanV(true);
}

Value Cons::evalRator(const Value &rand1, const Value &rand2) { // cons
    return PairV(rand1, rand2);
}

Value ListFunc::evalRator(const std::vector<Value> &args) { // list function
    //TODO: To complete the list logic
}

Value IsList::evalRator(const Value &rand) { // list?
    //TODO: To complete the list? logic
}

Value Car::evalRator(const Value &rand) { // car
    if (rand->v_type != V_PAIR) {
        throw RuntimeError("car expects a pair");
    }
    Pair* p = dynamic_cast<Pair*>(rand.get());
    return p->car;
}

Value Cdr::evalRator(const Value &rand) { // cdr
    if (rand->v_type != V_PAIR) {
        throw RuntimeError("cdr expects a pair");
    }
    Pair* p = dynamic_cast<Pair*>(rand.get());
    return p->cdr;
}

Value SetCar::evalRator(const Value &rand1, const Value &rand2) { // set-car!
    //TODO: To complete the set-car! logic
}

Value SetCdr::evalRator(const Value &rand1, const Value &rand2) { // set-cdr!
   //TODO: To complete the set-cdr! logic
}

Value IsEq::evalRator(const Value &rand1, const Value &rand2) { // eq?
    // Check if type is Integer
    if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        return BooleanV((dynamic_cast<Integer*>(rand1.get())->n) == (dynamic_cast<Integer*>(rand2.get())->n));
    }
    // Check if type is Boolean
    else if (rand1->v_type == V_BOOL && rand2->v_type == V_BOOL) {
        return BooleanV((dynamic_cast<Boolean*>(rand1.get())->b) == (dynamic_cast<Boolean*>(rand2.get())->b));
    }
    // Check if type is Symbol
    else if (rand1->v_type == V_SYM && rand2->v_type == V_SYM) {
        return BooleanV((dynamic_cast<Symbol*>(rand1.get())->s) == (dynamic_cast<Symbol*>(rand2.get())->s));
    }
    // Check if type is Null or Void
    else if ((rand1->v_type == V_NULL && rand2->v_type == V_NULL) ||
             (rand1->v_type == V_VOID && rand2->v_type == V_VOID)) {
        return BooleanV(true);
    } else {
        return BooleanV(rand1.get() == rand2.get());
    }
}

Value IsBoolean::evalRator(const Value &rand) { // boolean?
    return BooleanV(rand->v_type == V_BOOL);
}

Value IsFixnum::evalRator(const Value &rand) { // number?
    return BooleanV(rand->v_type == V_INT);
}

Value IsNull::evalRator(const Value &rand) { // null?
    return BooleanV(rand->v_type == V_NULL);
}

Value IsPair::evalRator(const Value &rand) { // pair?
    return BooleanV(rand->v_type == V_PAIR);
}

Value IsProcedure::evalRator(const Value &rand) { // procedure?
    return BooleanV(rand->v_type == V_PROC);
}

Value IsSymbol::evalRator(const Value &rand) { // symbol?
    return BooleanV(rand->v_type == V_SYM);
}

Value IsString::evalRator(const Value &rand) { // string?
    return BooleanV(rand->v_type == V_STRING);
}

Value Begin::eval(Assoc &e) {
    Value result = VoidV();
    for (const auto& expr : es) {
        result = expr->eval(e);
    }
    return result;
}

Value Quote::eval(Assoc& e) {
    // Quote returns the syntax tree as a value without evaluation
    // We need to convert the syntax directly to a value

    // Try to handle different syntax types
    if (auto num = dynamic_cast<Number*>(s.get())) {
        return IntegerV(num->n);
    } else if (auto rational = dynamic_cast<RationalSyntax*>(s.get())) {
        return RationalV(rational->numerator, rational->denominator);
    } else if (auto str = dynamic_cast<StringSyntax*>(s.get())) {
        return StringV(str->s);
    } else if (auto sym = dynamic_cast<SymbolSyntax*>(s.get())) {
        return SymbolV(sym->s);
    } else if (auto trueSym = dynamic_cast<TrueSyntax*>(s.get())) {
        return BooleanV(true);
    } else if (auto falseSym = dynamic_cast<FalseSyntax*>(s.get())) {
        return BooleanV(false);
    } else if (auto list = dynamic_cast<List*>(s.get())) {
        // For lists, we need to build a pair chain
        if (list->stxs.empty()) {
            return NullV();
        }

        // Convert each syntax element to a value recursively
        Value result = NullV();
        for (int i = list->stxs.size() - 1; i >= 0; i--) {
            Value elem = Quote(list->stxs[i]).eval(e);
            result = PairV(elem, result);
        }
        return result;
    }

    // Default case - shouldn't reach here
    throw RuntimeError("Unknown syntax type in quote");
}

Value AndVar::eval(Assoc &e) { // and with short-circuit evaluation
    if (rands.empty()) {
        return BooleanV(true);  // Empty and returns #t
    }

    Value result = BooleanV(true);
    for (const auto& expr : rands) {
        result = expr->eval(e);
        // Only #f is false in Scheme
        if (result->v_type == V_BOOL && dynamic_cast<Boolean*>(result.get())->b == false) {
            return result;  // Short-circuit on first #f
        }
    }
    return result;  // Return last evaluated value
}

Value OrVar::eval(Assoc &e) { // or with short-circuit evaluation
    if (rands.empty()) {
        return BooleanV(false);  // Empty or returns #f
    }

    for (const auto& expr : rands) {
        Value result = expr->eval(e);
        // Only #f is false in Scheme
        if (!(result->v_type == V_BOOL && dynamic_cast<Boolean*>(result.get())->b == false)) {
            return result;  // Short-circuit on first non-#f
        }
    }
    return BooleanV(false);  // All were #f
}

Value Not::evalRator(const Value &rand) { // not
    // In Scheme, only #f is false, everything else is true
    if (rand->v_type == V_BOOL) {
        bool b = dynamic_cast<Boolean*>(rand.get())->b;
        return BooleanV(!b);
    }
    // Any non-boolean value is considered true, so not returns #f
    return BooleanV(false);
}

Value If::eval(Assoc &e) {
    Value cond_result = cond->eval(e);
    // Only #f is false in Scheme
    bool is_false = (cond_result->v_type == V_BOOL && dynamic_cast<Boolean*>(cond_result.get())->b == false);

    if (is_false) {
        return alter->eval(e);
    } else {
        return conseq->eval(e);
    }
}

Value Cond::eval(Assoc &env) {
    //TODO: To complete the cond logic
}

Value Lambda::eval(Assoc &env) {
    // Lambda creates a closure with the current environment
    return ProcedureV(x, e, env);
}

Value Apply::eval(Assoc &e) {
    // Evaluate the operator to get the procedure
    Value proc_val = rator->eval(e);
    if (proc_val->v_type != V_PROC) {
        throw RuntimeError("Attempt to apply a non-procedure");
    }

    // Get the procedure closure
    Procedure* clos_ptr = dynamic_cast<Procedure*>(proc_val.get());

    // Evaluate all arguments
    std::vector<Value> args;
    for (const auto& arg_expr : rand) {
        args.push_back(arg_expr->eval(e));
    }

    // Check argument count
    if (args.size() != clos_ptr->parameters.size()) {
        throw RuntimeError("Wrong number of arguments");
    }

    // Create new environment with parameters bound to arguments
    Assoc param_env = clos_ptr->env;
    for (size_t i = 0; i < clos_ptr->parameters.size(); i++) {
        param_env = extend(clos_ptr->parameters[i], args[i], param_env);
    }

    // Evaluate the procedure body in the new environment
    return clos_ptr->e->eval(param_env);
}

Value Define::eval(Assoc &env) {
    // Evaluate the expression and bind it to the variable in the global environment
    Value val = e->eval(env);
    // Check if variable already exists in global environment
    bool found = false;
    for (auto i = global_env; i.get() != nullptr; i = i->next) {
        if (var == i->x) {
            i->v = val;
            found = true;
            break;
        }
    }
    if (!found) {
        // Extend global environment with new binding
        global_env = extend(var, val, global_env);
    }
    return VoidV();
}

Value Let::eval(Assoc &env) {
    //TODO: To complete the let logic
}

Value Letrec::eval(Assoc &env) {
    //TODO: To complete the letrec logic
}

Value Set::eval(Assoc &env) {
    //TODO: To complete the set logic
}

Value Display::evalRator(const Value &rand) { // display function
    if (rand->v_type == V_STRING) {
        String* str_ptr = dynamic_cast<String*>(rand.get());
        std::cout << str_ptr->s;
    } else {
        rand->show(std::cout);
    }
    
    return VoidV();
}

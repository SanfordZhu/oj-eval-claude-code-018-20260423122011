/**
 * @file parser.cpp
 * @brief Parsing implementation for Scheme syntax tree to expression tree conversion
 * 
 * This file implements the parsing logic that converts syntax trees into
 * expression trees that can be evaluated.
 * primitive operations, and function applications.
 */

#include "RE.hpp"
#include "Def.hpp"
#include "syntax.hpp"
#include "value.hpp"
#include "expr.hpp"
#include <map>
#include <string>
#include <iostream>

#define mp make_pair
using std::string;
using std::vector;
using std::pair;

extern std::map<std::string, ExprType> primitives;
extern std::map<std::string, ExprType> reserved_words;

/**
 * @brief Default parse method (should be overridden by subclasses)
 */
Expr Syntax::parse(Assoc &env) {
    throw RuntimeError("Unimplemented parse method");
}

Expr Number::parse(Assoc &env) {
    return Expr(new Fixnum(n));
}

Expr RationalSyntax::parse(Assoc &env) {
    return Expr(new RationalNum(numerator, denominator));
}

Expr SymbolSyntax::parse(Assoc &env) {
    return Expr(new Var(s));
}

Expr StringSyntax::parse(Assoc &env) {
    return Expr(new StringExpr(s));
}

Expr TrueSyntax::parse(Assoc &env) {
    return Expr(new True());
}

Expr FalseSyntax::parse(Assoc &env) {
    return Expr(new False());
}

Expr List::parse(Assoc &env) {
    if (stxs.empty()) {
        return Expr(new Quote(Syntax(new List())));
    }

    //TODO: check if the first element is a symbol
    //If not, use Apply function to package to a closure;
    //If so, find whether it's a variable or a keyword;
    SymbolSyntax *id = dynamic_cast<SymbolSyntax*>(stxs[0].get());
    if (id == nullptr) {
        //TODO: TO COMPLETE THE LOGIC
    }else{
    string op = id->s;
    if (find(op, env).get() != nullptr) {
        //TODO: TO COMPLETE THE PARAMETER PARSER LOGIC
    }
    if (primitives.count(op) != 0) {
        vector<Expr> parameters;
        // Parse all parameters
        for (size_t i = 1; i < stxs.size(); i++) {
            parameters.push_back(stxs[i]->parse(env));
        }

        ExprType op_type = primitives[op];
        if (op_type == E_PLUS) {
            if (parameters.size() == 2) {
                return Expr(new Plus(parameters[0], parameters[1]));
            } else {
                throw RuntimeError("Wrong number of arguments for +");
            }
        } else if (op_type == E_MINUS) {
            if (parameters.size() == 2) {
                return Expr(new Minus(parameters[0], parameters[1]));
            } else {
                throw RuntimeError("Wrong number of arguments for -");
            }
        } else if (op_type == E_MUL) {
            if (parameters.size() == 2) {
                return Expr(new Mult(parameters[0], parameters[1]));
            } else {
                throw RuntimeError("Wrong number of arguments for *");
            }
        }  else if (op_type == E_DIV) {
            if (parameters.size() == 2) {
                return Expr(new Div(parameters[0], parameters[1]));
            } else {
                throw RuntimeError("Wrong number of arguments for /");
            }
        } else if (op_type == E_MODULO) {
            if (parameters.size() != 2) {
                throw RuntimeError("Wrong number of arguments for modulo");
            }
            return Expr(new Modulo(parameters[0], parameters[1]));
        } else if (op_type == E_LIST) {
            return Expr(new ListFunc(parameters));
        } else if (op_type == E_LT) {
            if (parameters.size() == 2) {
                return Expr(new Less(parameters[0], parameters[1]));
            } else {
                throw RuntimeError("Wrong number of arguments for <");
            }
        } else if (op_type == E_LE) {
            if (parameters.size() == 2) {
                return Expr(new LessEq(parameters[0], parameters[1]));
            } else {
                throw RuntimeError("Wrong number of arguments for <=");
            }
        } else if (op_type == E_EQ) {
            if (parameters.size() == 2) {
                return Expr(new Equal(parameters[0], parameters[1]));
            } else {
                throw RuntimeError("Wrong number of arguments for =");
            }
        } else if (op_type == E_GE) {
            if (parameters.size() == 2) {
                return Expr(new GreaterEq(parameters[0], parameters[1]));
            } else {
                throw RuntimeError("Wrong number of arguments for >=");
            }
        } else if (op_type == E_GT) {
            if (parameters.size() == 2) {
                return Expr(new Greater(parameters[0], parameters[1]));
            } else {
                throw RuntimeError("Wrong number of arguments for >");
            }
        } else if (op_type == E_CONS) {
            if (parameters.size() == 2) {
                return Expr(new Cons(parameters[0], parameters[1]));
            } else {
                throw RuntimeError("Wrong number of arguments for cons");
            }
        } else if (op_type == E_CAR) {
            if (parameters.size() != 1) {
                throw RuntimeError("Wrong number of arguments for car");
            }
            return Expr(new Car(parameters[0]));
        } else if (op_type == E_CDR) {
            if (parameters.size() != 1) {
                throw RuntimeError("Wrong number of arguments for cdr");
            }
            return Expr(new Cdr(parameters[0]));
        } else if (op_type == E_NOT) {
            if (parameters.size() != 1) {
                throw RuntimeError("Wrong number of arguments for not");
            }
            return Expr(new Not(parameters[0]));
        } else if (op_type == E_AND) {
            return Expr(new AndVar(parameters));
        } else if (op_type == E_OR) {
            return Expr(new OrVar(parameters));
        } else if (op_type == E_EQQ) {
            if (parameters.size() == 2) {
                return Expr(new IsEq(parameters[0], parameters[1]));
            } else {
                throw RuntimeError("Wrong number of arguments for eq?");
            }
        } else if (op_type == E_BOOLQ) {
            if (parameters.size() != 1) {
                throw RuntimeError("Wrong number of arguments for boolean?");
            }
            return Expr(new IsBoolean(parameters[0]));
        } else if (op_type == E_INTQ) {
            if (parameters.size() != 1) {
                throw RuntimeError("Wrong number of arguments for number?");
            }
            return Expr(new IsFixnum(parameters[0]));
        } else if (op_type == E_NULLQ) {
            if (parameters.size() != 1) {
                throw RuntimeError("Wrong number of arguments for null?");
            }
            return Expr(new IsNull(parameters[0]));
        } else if (op_type == E_PAIRQ) {
            if (parameters.size() != 1) {
                throw RuntimeError("Wrong number of arguments for pair?");
            }
            return Expr(new IsPair(parameters[0]));
        } else if (op_type == E_PROCQ) {
            if (parameters.size() != 1) {
                throw RuntimeError("Wrong number of arguments for procedure?");
            }
            return Expr(new IsProcedure(parameters[0]));
        } else if (op_type == E_SYMBOLQ) {
            if (parameters.size() != 1) {
                throw RuntimeError("Wrong number of arguments for symbol?");
            }
            return Expr(new IsSymbol(parameters[0]));
        } else if (op_type == E_STRINGQ) {
            if (parameters.size() != 1) {
                throw RuntimeError("Wrong number of arguments for string?");
            }
            return Expr(new IsString(parameters[0]));
        } else if (op_type == E_DISPLAY) {
            if (parameters.size() != 1) {
                throw RuntimeError("Wrong number of arguments for display");
            }
            return Expr(new Display(parameters[0]));
        } else if (op_type == E_VOID) {
            if (parameters.size() != 0) {
                throw RuntimeError("Wrong number of arguments for void");
            }
            return Expr(new MakeVoid());
        } else if (op_type == E_EXIT) {
            if (parameters.size() != 0) {
                throw RuntimeError("Wrong number of arguments for exit");
            }
            return Expr(new Exit());
        } else {
            // Unknown primitive
            throw RuntimeError("Unknown primitive: " + op);
        }
    }

    if (reserved_words.count(op) != 0) {
    	switch (reserved_words[op]) {
			case E_BEGIN: {
			    vector<Expr> exprs;
			    for (size_t i = 1; i < stxs.size(); i++) {
			        exprs.push_back(stxs[i]->parse(env));
			    }
			    return Expr(new Begin(exprs));
			}
			case E_QUOTE: {
			    if (stxs.size() != 2) {
			        throw RuntimeError("Wrong number of arguments for quote");
			    }
			    // For quote, we need to preserve the syntax tree as-is
			    Syntax quoted_syntax = stxs[1];
			    return Expr(new Quote(quoted_syntax));
			}
			case E_IF: {
			    if (stxs.size() != 4) {
			        throw RuntimeError("Wrong number of arguments for if");
			    }
			    Expr cond = stxs[1]->parse(env);
			    Expr conseq = stxs[2]->parse(env);
			    Expr alter = stxs[3]->parse(env);
			    return Expr(new If(cond, conseq, alter));
			}
			case E_COND: {
			    vector<vector<Expr>> clauses;
			    for (size_t i = 1; i < stxs.size(); i++) {
			        if (auto clause_list = dynamic_cast<List*>(stxs[i].get())) {
			            vector<Expr> clause;
			            for (auto& syntax : clause_list->stxs) {
			                clause.push_back(syntax->parse(env));
			            }
			            clauses.push_back(clause);
			        } else {
			            throw RuntimeError("Invalid cond clause");
			        }
			    }
			    return Expr(new Cond(clauses));
			}
			case E_LAMBDA: {
			    if (stxs.size() != 3) {
			        throw RuntimeError("Wrong number of arguments for lambda");
			    }
			    // Parse parameter list
			    vector<string> params;
			    if (auto param_list = dynamic_cast<List*>(stxs[1].get())) {
			        for (auto& param : param_list->stxs) {
			            if (auto sym = dynamic_cast<SymbolSyntax*>(param.get())) {
			                params.push_back(sym->s);
			            } else {
			                throw RuntimeError("Lambda parameters must be symbols");
			            }
			        }
			    } else {
			        throw RuntimeError("Lambda parameters must be a list");
			    }
			    Expr body = stxs[2]->parse(env);
			    return Expr(new Lambda(params, body));
			}
			case E_DEFINE: {
			    if (stxs.size() != 3) {
			        throw RuntimeError("Wrong number of arguments for define");
			    }
			    // Check if it's function definition syntax: (define (name params...) body)
			    if (auto func_list = dynamic_cast<List*>(stxs[1].get())) {
			        // Function definition syntax
			        if (func_list->stxs.empty()) {
			            throw RuntimeError("Invalid function definition");
			        }
			        string func_name;
			        if (auto name_sym = dynamic_cast<SymbolSyntax*>(func_list->stxs[0].get())) {
			            func_name = name_sym->s;
			        } else {
			            throw RuntimeError("Function name must be a symbol");
			        }
			        // Parse parameters
			        vector<string> params;
			        for (size_t i = 1; i < func_list->stxs.size(); i++) {
			            if (auto param_sym = dynamic_cast<SymbolSyntax*>(func_list->stxs[i].get())) {
			                params.push_back(param_sym->s);
			            } else {
			                throw RuntimeError("Function parameters must be symbols");
			            }
			        }
			        // Parse body
			        Expr body = stxs[2]->parse(env);
			        // Create lambda expression
			        Expr lambda = Expr(new Lambda(params, body));
			        return Expr(new Define(func_name, lambda));
			    } else if (auto sym = dynamic_cast<SymbolSyntax*>(stxs[1].get())) {
			        // Regular variable definition
			        string var_name = sym->s;
			        Expr expr = stxs[2]->parse(env);
			        return Expr(new Define(var_name, expr));
			    } else {
			        throw RuntimeError("Define expects a symbol or function definition as first argument");
			    }
			}
			case E_LET: {
			    if (stxs.size() != 3) {
			        throw RuntimeError("Wrong number of arguments for let");
			    }
			    // Parse bindings
			    vector<pair<string, Expr>> bindings;
			    if (auto bind_list = dynamic_cast<List*>(stxs[1].get())) {
			        for (auto& binding : bind_list->stxs) {
			            if (auto bind_pair = dynamic_cast<List*>(binding.get())) {
			                if (bind_pair->stxs.size() != 2) {
			                    throw RuntimeError("Invalid let binding");
			                }
			                string var_name;
			                if (auto sym = dynamic_cast<SymbolSyntax*>(bind_pair->stxs[0].get())) {
			                    var_name = sym->s;
			                } else {
			                    throw RuntimeError("Let binding variable must be a symbol");
			                }
			                Expr expr = bind_pair->stxs[1]->parse(env);
			                bindings.push_back(make_pair(var_name, expr));
			            } else {
			                throw RuntimeError("Invalid let binding format");
			            }
			        }
			    } else {
			        throw RuntimeError("Let bindings must be a list");
			    }
			    Expr body = stxs[2]->parse(env);
			    return Expr(new Let(bindings, body));
			}
			case E_LETREC: {
			    if (stxs.size() != 3) {
			        throw RuntimeError("Wrong number of arguments for letrec");
			    }
			    // Parse bindings (same as let)
			    vector<pair<string, Expr>> bindings;
			    if (auto bind_list = dynamic_cast<List*>(stxs[1].get())) {
			        for (auto& binding : bind_list->stxs) {
			            if (auto bind_pair = dynamic_cast<List*>(binding.get())) {
			                if (bind_pair->stxs.size() != 2) {
			                    throw RuntimeError("Invalid letrec binding");
			                }
			                string var_name;
			                if (auto sym = dynamic_cast<SymbolSyntax*>(bind_pair->stxs[0].get())) {
			                    var_name = sym->s;
			                } else {
			                    throw RuntimeError("Letrec binding variable must be a symbol");
			                }
			                Expr expr = bind_pair->stxs[1]->parse(env);
			                bindings.push_back(make_pair(var_name, expr));
			            } else {
			                throw RuntimeError("Invalid letrec binding format");
			            }
			        }
			    } else {
			        throw RuntimeError("Letrec bindings must be a list");
			    }
			    Expr body = stxs[2]->parse(env);
			    return Expr(new Letrec(bindings, body));
			}
			case E_SET: {
			    if (stxs.size() != 3) {
			        throw RuntimeError("Wrong number of arguments for set!");
			    }
			    // Get variable name
			    string var_name;
			    if (auto sym = dynamic_cast<SymbolSyntax*>(stxs[1].get())) {
			        var_name = sym->s;
			    } else {
			        throw RuntimeError("set! expects a symbol as first argument");
			    }
			    Expr expr = stxs[2]->parse(env);
			    return Expr(new Set(var_name, expr));
			}
        	default:
            	throw RuntimeError("Unknown reserved word: " + op);
    	}
    }

    //default: use Apply to be an expression
    // Parse function and arguments
    Expr func_expr = stxs[0]->parse(env);
    vector<Expr> args;
    for (size_t i = 1; i < stxs.size(); i++) {
        args.push_back(stxs[i]->parse(env));
    }
    return Expr(new Apply(func_expr, args));
}
}

#include <bashclass/BBashHelper.h>
#include <bashclass/BGenerateCode.h>
#include <bashclass/BTypes.h>
#include <bashclass/BException.h>
#include <bashclass/BGlobal.h>
#include <bashclass/BVariableCall.h>
#include <bashclass/BFunctionCall.h>
#include <iostream>

// Constants
std::string FUNCTION_THIS = "_this_";
std::string FUNCTION_RETURN = "_return_";

void _indent(std::shared_ptr<BScope> parent, std::stringstream& ss) {
    if(!parent) {
        throw BException("Parent scope should not be empty");
    }

    while(!std::dynamic_pointer_cast<BGlobal>(parent) && !std::dynamic_pointer_cast<BClass>(parent)) {
        ss << "\t";
        parent = parent->getParentScope();
    }
}

std::string _chainCallToCode(std::shared_ptr<BChainCall> chainCallPtr) {
    BChainCall &chainCall = *chainCallPtr;
    std::string chainStr;
    std::shared_ptr<BScope> prevTypeScope;
    for (size_t i = 0; i < chainCall.size(); i++) {

        // Cast element
        auto variableCallCast = std::dynamic_pointer_cast<BVariableCall>(chainCall[i]);
        auto functionCallCast = std::dynamic_pointer_cast<BFunctionCall>(chainCall[i]);
        auto thisCallCast = std::dynamic_pointer_cast<BThisCall>(chainCall[i]);

        if (variableCallCast) {
            if (i == 0) {
                chainStr = variableCallCast->getVariable()->getLabel().str();
            } else {
                chainStr = prevTypeScope->getLabel().str() +
                           "[${" + chainStr + "},\"" + variableCallCast->getVariable()->getLabel().str() + "\"]";
            }
            prevTypeScope = variableCallCast->getVariable()->getTypeScope();
        } else if (functionCallCast) {
            if (i == 0) {
                chainStr = functionCallCast->getFunction()->getLabel().str();
            } else {
                chainStr = functionCallCast->getFunction()->getLabel().str() + "[" + chainStr + "]";
            }
        } else if(thisCallCast) {
            chainStr = FUNCTION_THIS;
            prevTypeScope = thisCallCast->getReference();
        } else {
            throw BException("Cannot generate code for an unrecognized element call");
        }
    }
    return chainStr;
}

void BBashHelper::header() {
    BGenerateCode::get().writePreCode();
}

void BBashHelper::footer() {
    BGenerateCode::get().writePostCode();
}

void BBashHelper::declareClass(std::shared_ptr<BClass> classScope) {
    std::stringstream ss;
    ss << "declare -A " << classScope->getLabel().str() << "=()" << std::endl;
    BGenerateCode::get().write(ss);
}

void BBashHelper::uniqueCounter(std::shared_ptr<BClass> classScope) {
    std::stringstream ss;
    ss << "_" << classScope->getName()->getValue() << "_=0" << std::endl;
    BGenerateCode::get().write(ss);
}

void BBashHelper::createGlobalVar(std::shared_ptr<BVariable> variable, std::string defaultValue) {
    std::stringstream ss;
    ss << variable->getLabel().str() << "=" << defaultValue << std::endl;
    BGenerateCode::get().write(ss);
}

void BBashHelper::createLocalVar(std::shared_ptr<BVariable> variable, std::string defaultValue) {
    std::stringstream ss;
    _indent(variable->getParentScope(), ss);
    ss << "local " << variable->getLabel().str() << "=" << defaultValue << std::endl;
    BGenerateCode::get().write(ss);
}

void BBashHelper::bash(std::shared_ptr<ecc::LexicalToken> token) {
    std::stringstream ss;
    if(token->getName() == BType::TYPE_NAME_BASH_INLINE) {
        ss << token->getValue().substr(2,token->getValue().length()-2);
    } else if(token->getName() == BType::TYPE_NAME_BASH_SUB || token->getName() == BType::TYPE_NAME_BASH_BLOCK) {
        ss << token->getValue().substr(2, token->getValue().length()-4);
    } else {
        throw BException("Bash code can only be generated by bash lexical tokens");
    }
    ss << std::endl;
    BGenerateCode::get().write(ss);
}

void BBashHelper::createFunction(std::shared_ptr<BFunction> function) {
    std::stringstream ss;
    ss << "function " << function->getLabel().str() << "() {" << std::endl;

    // Create parameters
    int paramPos = 1;
    for(auto param : function->findAllParameters()) {
        _indent(param->getParentScope(), ss);
        ss << "local " << param->getLabel().str() << "=\"$" << paramPos++ << "\"" << std::endl;
    }

    // Generate additional parameters
    _indent(function, ss);
    ss << "local " << FUNCTION_THIS << "=$" << paramPos++ << std::endl;
    _indent(function, ss);
    ss << "declare -n " << FUNCTION_RETURN << "=$" << paramPos++ << std::endl;

    BGenerateCode::get().write(ss);
}

void BBashHelper::closeFunction(std::shared_ptr<BFunction> function) {
    std::stringstream ss;
    _indent(function->getParentScope(), ss);
    ss << "}" << std::endl << std::endl;
    BGenerateCode::get().write(ss);
}

void BBashHelper::assignVariable(std::shared_ptr<BChainCall> chainCall) {
    std::stringstream ss;
    _indent(chainCall->getParentScope(), ss);

    // Convert chain to code
    ss << _chainCallToCode(chainCall) << "=" << std::endl;
    BGenerateCode::get().write(ss);
}
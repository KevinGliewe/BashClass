#include <bashclass/BFunction.h>
#include <iostream>
#include <bashclass/BException.h>
#include <bashclass/BReport.h>
#include <bashclass/BVariable.h>
#include <stack>

BFunction::BFunction() {
    m_isConstructor = false;
    m_type = std::make_shared<BElementType>();
}

std::stringstream BFunction::getLabel() {
    std::stringstream stream = m_parentScope->getLabel();
    stream << "_f_" << m_name->getValue();
    return stream;
}

bool BFunction::isClassMember() {
    if(!m_parentScope) {
        throw BException("Cannot check if a function is a class member if it does not have a parent scope");
    }
    return std::dynamic_pointer_cast<BClass>(m_parentScope) != nullptr;
}

bool BFunction::requiresReturn() {
    return !m_type->isVoid() && !isConstructor();
}

void BFunction::verifyReturns() {
    if(requiresReturn() && !hasReturn()) {
        BReport::getInstance().error()
                << "Missing return statement in function " << m_name->getValue() << std::endl;
        BReport::getInstance().printError();
    }

    // No need to check for the case of return not required
    // since it is already handled when it is set into a scope
}

void BFunction::verifyParameters() {
    auto params = findAllParameters();

    // If a parameter has an default expression,
    // then all the following ones must also have a default expression
    bool hasExpression = false;
    for(size_t i=0; i < params.size(); i++) {
        if(hasExpression && !params[i]->getExpression()) {
            BReport::getInstance().error()
                    << "Parameter " << i+1 << " in function " << m_name->getValue()
                    << " is missing a default expression" << std::endl;
            BReport::getInstance().printError();
        }
        hasExpression |= params[i]->getExpression() != nullptr;
    }
}

std::shared_ptr<BFunction> BFunction::findClosestFunction() {
    return std::static_pointer_cast<BFunction>(shared_from_this());
}

bool BFunction::isConstructor() {
    return m_isConstructor;
}

std::vector<std::shared_ptr<BVariable>> BFunction::findAllParameters(const char *name) {
    std::vector<std::shared_ptr<BVariable>> parameters;
    for(auto variable : m_variables) {
        if(variable.second->isParam() && (!name ||
                                          (variable.second->getName() && variable.second->getName()->getValue() == name))) {
            parameters.push_back(variable.second);
        }
    }
    return parameters;
}

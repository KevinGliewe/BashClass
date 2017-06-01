#include <bashclass/BWhile.h>
#include <bashclass/BTypes.h>
#include <iostream>

std::stringstream BWhile::getLabel() {
    std::stringstream stream = m_parentScope->getLabel();
    stream << "_while_";
    return stream;
}

void BWhile::setExpression(std::shared_ptr<IBCallable> expression) {

    // Store the condition/expression
    m_expression = expression;

    // Verify the type of the condition is boolean
    std::string expressionType = expression->getTypeValueAsString();
    if(expressionType == BType::UNDEFINED) {
        std::cerr << "While statement condition cannot be of undefined type" << std::endl;
    } else if(expressionType != BType::TYPE_VALUE_BOOLEAN) {
        std::cerr << "A while condition must evaluate to a boolean instead of " << expressionType << std::endl;
    }
}

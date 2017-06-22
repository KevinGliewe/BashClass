#ifndef BASHCLASS_BTHISCHAINACCESS_H
#define BASHCLASS_BTHISCHAINACCESS_H

#include <bashclass/IBChainable.h>
#include <bashclass/BClass.h>
#include <easycc/LexicalToken.h>
#include <memory>

class BThisChainAccess : public IBChainable {
private:

    // The class that this reference points to
    std::shared_ptr<BClass> m_reference;

    // Hold the lexical token for this reference
    std::shared_ptr<ecc::LexicalToken> m_lexicalToken;
public:

    /**
    * Get reference class
    * @return reference
    */
    std::shared_ptr<BClass> getReference() {return m_reference;}

    /**
     * Set reference class
     * @param reference
     */
    void setReference(std::shared_ptr<BClass> reference) {m_reference = reference;}

    /**
     * Get lexical token of this reference
     * @return lexicalToken
     */
    std::shared_ptr<ecc::LexicalToken> getLexicalToken() {return m_lexicalToken;}

    /**
     * Set lexical token for this reference
     * @param lexicalToken
     */
    void setLexicalToken(std::shared_ptr<ecc::LexicalToken> lexicalToken) { m_lexicalToken = lexicalToken;}

    /**
     * Check if reference is found
     * @return true if found
     */
    bool isFound();

    /**
     * Get this reference type
     * @return type
     */
    std::shared_ptr<IBType> getType();
};

#endif

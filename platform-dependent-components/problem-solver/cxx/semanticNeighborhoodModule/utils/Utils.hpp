#pragma once

#include "sc-memory/kpm/sc_agent.hpp"

namespace semanticNeighborhood{

class Utils {
  public:
    static void makeConceptEntityTemplate(ScAddr const & concept, ScAddr const & parameter, ScAddr const & lang, ScTemplate & templ);

    static void makeSubjectDomainMembershipTemplate(ScAddr const & concept, ScTemplate & templ);
    
    static std::string getMainIdtf(ScMemoryContext * ms_context, ScAddr const & concept, ScAddr const & lang);

    static std::string makeIndent(int indentCount);

    static std::string formatForTex(std::string const & input);

    static std::vector<std::string> conceptEntityTemplateVarNames;

    static std::string linkWithDescriptionVarName;
    
    static std::string subjDomainVarName;

    static std::string roleRelationVarName;

    static std::vector<std::string> subjectDomainMembershipTemplateVarNames;
};

struct ScnTexKeywords {
  static std::string const idtf;

  static std::string const header;

  static std::string const relto;

  static std::string const text;
  
  static std::string const iselementrole;

  static std::string const relfrom;

  static std::string const indent;

  static std::string const eqtoset;

  static std::string const item;
};

struct TexKeywords {
  static std::string const begin;

  static std::string const end;
};

}
#pragma once

#include "sc-memory/kpm/sc_agent.hpp"

namespace semanticNeighborhood{

class Utils {
  public:
    static void makeConceptEntityTemplate(ScAddr const & concept, ScAddr const & parameter, ScAddr const & lang, ScTemplate & templ);

    static std::vector<std::string> conceptEntityTemplateVarNames;

    static std::string linkWithDescriptionVarName;
    
};

}
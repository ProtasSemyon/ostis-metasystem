#include "Utils.hpp"

#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "keynodes/Keynodes.hpp"

using namespace semanticNeighborhood;

std::string Utils::linkWithDescriptionVarName = "_description";

std::vector<std::string> Utils::conceptEntityTemplateVarNames = {
    "_set",
    "_set2",
    linkWithDescriptionVarName,
    "_arc1",
    "_arc2",
    "_arc3",
    "_arc4",
    "_arc5",
    "_arc6",
    "_arc7",
};

std::string Utils::subjDomainVarName = "_subject_domain";
std::string Utils::roleRelationVarName = "_role_relation";

std::vector<std::string> Utils::subjectDomainMembershipTemplateVarNames = {
    subjDomainVarName,
    roleRelationVarName,
    "_arc1",
    "_arc2",
};

std::string const ScnTexKeywords::idtf = "scnidtf";

std::string const ScnTexKeywords::header = "scnheader";

std::string const ScnTexKeywords::relto = "scnrelto";

std::string const ScnTexKeywords::text = "scntext";

std::string const ScnTexKeywords::iselementrole = "scniselementrole";

std::string const ScnTexKeywords::relfrom = "scnrelfrom";

std::string const ScnTexKeywords::indent = "scnindent";

std::string const ScnTexKeywords::eqtoset = "scneqtoset";

std::string const ScnTexKeywords::item = "scnitem";

std::string const TexKeywords::begin = "begin";

std::string const TexKeywords::end = "end";

void Utils::makeConceptEntityTemplate(
    ScAddr const & concept, 
    ScAddr const & parameter, 
    ScAddr const & lang,
    ScTemplate & templ)
{
    templ.Clear();
    templ.Triple(
        parameter, 
        ScType::EdgeAccessVarPosPerm >> "_arc1", 
        ScType::NodeVar >> "_set"
    );
    templ.Quintuple(
        "_set",
        ScType::EdgeAccessVarPosPerm >> "_arc2",
        concept,
        ScType::EdgeAccessVarPosPerm >> "_arc3",
        scAgentsCommon::CoreKeynodes::rrel_key_sc_element
    );
    templ.Quintuple(
        ScType::NodeVar >> "_set2",
        ScType::EdgeDCommonVar >> "_arc4",
        "_set", 
        ScType::EdgeAccessVarPosPerm >> "_arc5",
        scAgentsCommon::CoreKeynodes::nrel_sc_text_translation
    );
    templ.Quintuple(
        "_set2",
        ScType::EdgeAccessVarPosPerm >> "_arc6",
        ScType::LinkVar >> linkWithDescriptionVarName,
        ScType::EdgeAccessVarPosPerm >> "_arc7",
        Keynodes::rrel_example
    );
    templ.Triple(
        lang, 
        ScType::EdgeAccessVarPosPerm,
        linkWithDescriptionVarName
    );
}

void Utils::makeSubjectDomainMembershipTemplate(ScAddr const & concept, ScTemplate & templ) 
{
    templ.Clear();
    templ.Triple(
        Keynodes::subject_domain, 
        ScType::EdgeAccessVarPosPerm,
        ScType::NodeVar >> subjDomainVarName
    );
    templ.Quintuple(
        subjDomainVarName, 
        ScType::EdgeAccessVarPosPerm >> "_arc1", 
        concept, 
        ScType::EdgeAccessVarPosPerm >> "_arc2", 
        ScType::NodeVarRole >> roleRelationVarName
    );
}

std::string Utils::getMainIdtf(ScMemoryContext * ms_context, ScAddr const & concept, ScAddr const & lang)
{
    ScIterator5Ptr mainIdtfIterator = 
        ms_context->Iterator5(
            concept, 
            ScType::EdgeDCommonConst, 
            ScType::LinkConst, 
            ScType::EdgeAccessConstPosPerm, 
            scAgentsCommon::CoreKeynodes::nrel_main_idtf
        );
    
    if (!mainIdtfIterator->IsValid())
        return ms_context->HelperGetSystemIdtf(concept);

    while (mainIdtfIterator->Next())
    {
        ScAddr mainIdtfAddr = mainIdtfIterator->Get(2);
        bool isValidLang = ms_context->HelperCheckEdge(
            lang, 
            mainIdtfAddr,
            ScType::EdgeAccessConstPosPerm);
        if (isValidLang) 
        {
            std::string stringContent;
            if (ms_context->GetLinkContent(mainIdtfAddr, stringContent))
            {
                return stringContent;
            }
        }
    }

    return ms_context->HelperGetSystemIdtf(concept);
}

std::string Utils::makeIndent(int indentCount)
{
    return std::string(indentCount, '\t');
}

std::string Utils::formatForTex(std::string const & input) 
{
    static std::map<char, std::string> specSymbols = {
        {'_', "\\_"},
        {'#', "\\#"},
        {'$', "\\$"},
        {'%', "\\%"},
        {'&', "\\&"},
        {'{', "\\{"},
        {'}', "\\}"},
        {'~', "$\\sim$"},
        {'^', "$\\hat{}$"},
        {'\\', "$\\backslash$"},
        {'<', "$<$"},
        {'>', "$>$"},
    };
    std::string newInput;
    for (auto const & symbol : input) 
    {
        if (specSymbols.find(symbol) != specSymbols.end())
        {
            newInput += specSymbols[symbol];
        }
        else 
            newInput += symbol;
    }
    return newInput;
}

#include "keynodes/Keynodes.hpp"
#include "SemanticNeighborhoodToScnTranslationAgent.hpp"

#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "utils/Utils.hpp"

using namespace utils;

using namespace semanticNeighborhood;

ScAddr SemanticNeighborhoodToScnTranslationAgent::outputLang;

SC_AGENT_IMPLEMENTATION(SemanticNeighborhoodToScnTranslationAgent)
{
    ScAddr const & questionNode = otherAddr;

    if (!checkAction(questionNode))
        return SC_RESULT_OK;

    SC_LOG_DEBUG("SemanticNeighborhoodToScnTranslationAgent started");

    //Initialization
    ScAddr conceptAddr = IteratorUtils::getAnyByOutRelation(
        &m_memoryCtx, 
        questionNode, 
        scAgentsCommon::CoreKeynodes::rrel_1);
    
    ScAddr parametersAddr = IteratorUtils::getAnyByOutRelation(
        &m_memoryCtx, 
        questionNode, 
        scAgentsCommon::CoreKeynodes::rrel_2);

    ScAddr langAddr = IteratorUtils::getAnyByOutRelation(
        &m_memoryCtx, 
        questionNode, 
        scAgentsCommon::CoreKeynodes::rrel_3);

    //Validation
    if (!m_memoryCtx.IsElement(conceptAddr))
        return exitInvalidParams("concept node not found.", questionNode);

    if (!m_memoryCtx.IsElement(parametersAddr))
        return exitInvalidParams("parameters node not found.", questionNode);

    if (!m_memoryCtx.IsElement(langAddr))
        return exitInvalidParams("language node not found.", questionNode);
    else
        outputLang = langAddr;

    // Output generation

    ScAddr const & answerLink = m_memoryCtx.CreateLink();

    std::string answerLinkContent;
    answerLinkContent += formatToTex(
        ScnTexKeywords::header, 
        Utils::getMainIdtf(&m_memoryCtx, conceptAddr, langAddr));

    ScAddr currentParamAddr = IteratorUtils::getAnyByOutRelation(
        &m_memoryCtx, 
        parametersAddr, 
        scAgentsCommon::CoreKeynodes::rrel_1);

    int indentLevel = 1;

    do {
        answerLinkContent += obtainSemanticNeighborhoodByParameter(conceptAddr, currentParamAddr, indentLevel);
        currentParamAddr = IteratorUtils::getNextFromSet(&m_memoryCtx, parametersAddr, currentParamAddr); 
    } while (currentParamAddr.IsValid());
    
    m_memoryCtx.SetLinkContent(answerLink, answerLinkContent);

    AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, {answerLink}, true);

    SC_LOG_DEBUG("SemanticNeighborhoodToScnTranslationAgent finished");

    return SC_RESULT_OK;
};

bool SemanticNeighborhoodToScnTranslationAgent::checkAction(ScAddr const & questionNode) 
{
    return m_memoryCtx.HelperCheckEdge(
        Keynodes::action_translate_semantic_neighborhood_to_scn,
        questionNode,
        ScType::EdgeAccessConstPosPerm
        );
}

sc_result SemanticNeighborhoodToScnTranslationAgent::exitInvalidParams(std::string const & message, ScAddr const & questionNode)
{
    SC_LOG_ERROR(formatToLog(message));
    AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, false);
    return SC_RESULT_ERROR_INVALID_PARAMS;
}

std::string SemanticNeighborhoodToScnTranslationAgent::formatToLog(std::string const & message) {
    return "SemanticNeighborhoodToScnTranslationAgent: " + message;
}

std::string SemanticNeighborhoodToScnTranslationAgent::formatToTex(
    std::string const & keyword, 
    std::string const & content,
    int indentLevel
)
{
    return Utils::makeIndent(indentLevel) 
        + "\\" + Utils::formatForTex(keyword) 
        + "{" + Utils::formatForTex(content) + "}" 
        + "\n";
}

std::string SemanticNeighborhoodToScnTranslationAgent::formatToTex(
    std::string const & keyword, 
    std::string const & parameter, 
    std::string const & content,
    int indentLevel
)
{
    return Utils::makeIndent(indentLevel) 
        + "\\" + Utils::formatForTex(keyword) 
        + "{" + Utils::formatForTex(parameter) + "}" 
        + "{" + Utils::formatForTex(content) + "}" 
        +"\n";
}


std::string SemanticNeighborhoodToScnTranslationAgent::obtainSemanticNeighborhoodByParameter(
    ScAddr const & concept, 
    ScAddr const & parameter,
    int indentLevel
)
{
    static const FunctionsByParameterMap functionsByParameterMap = {
        {scAgentsCommon::CoreKeynodes::nrel_system_identifier, processIdtfParameter},
        {scAgentsCommon::CoreKeynodes::nrel_idtf, processIdtfParameter},
        {Keynodes::definition, processEntityParameter},
        {Keynodes::explanation, processEntityParameter},
        {Keynodes::note, processEntityParameter},
        {Keynodes::nrel_subdividing, processSubdividingParameter},
        {Keynodes::subject_domain, processSubjDomainParameter},
        {scAgentsCommon::CoreKeynodes::nrel_inclusion, processOutNoroleRelationParameter},
        {Keynodes::nrel_strict_inclusion, processOutNoroleRelationParameter},
        {Keynodes::nrel_authors, processOutNoroleRelationParameter}
    };

    if (functionsByParameterMap.find(parameter) == functionsByParameterMap.end()) {
        SC_LOG_WARNING(formatToLog("processing of parameter '"
            + m_memoryCtx.HelperGetSystemIdtf(parameter))
            + "' not implemented.");
        return {};
    }

    return functionsByParameterMap.at(parameter)(&m_memoryCtx, concept, parameter, indentLevel);
}

std::string SemanticNeighborhoodToScnTranslationAgent::processIdtfParameter (
    ScMemoryContext * ms_context,
    ScAddr const & concept, 
    ScAddr const & parameter,
    int indentLevel
)
{
    ScIterator5Ptr idtfIterator = ms_context->Iterator5(
        concept, 
        ScType::EdgeDCommonConst, 
        ScType::Unknown, 
        ScType::EdgeAccessConstPosPerm, 
        parameter);

    if (!idtfIterator->IsValid())
        return {};

    std::string answer;

    while (idtfIterator->Next())
    {
        ScAddr const & idtfLink = idtfIterator->Get(2);
        bool isLangCorrect = ms_context->HelperCheckEdge(
            outputLang, 
            idtfLink, 
            ScType::EdgeAccessConstPosPerm
        );

        if (!isLangCorrect && parameter != scAgentsCommon::CoreKeynodes::nrel_system_identifier)
            continue;
        
        std::string idtfLinkContent;
        ms_context->GetLinkContent(idtfLink, idtfLinkContent);
        answer += formatToTex(ScnTexKeywords::idtf, idtfLinkContent, indentLevel);
    }

    return answer;
}

std::string SemanticNeighborhoodToScnTranslationAgent::processEntityParameter(
    ScMemoryContext * ms_context,
    ScAddr const & concept,
    ScAddr const & parameter,
    int indentLevel
)
{
    ScTemplate templ;
    Utils::makeConceptEntityTemplate(concept, parameter, outputLang, templ);

    ScTemplateSearchResult templResult;
    ms_context->HelperSearchTemplate(templ, templResult);

    if (templResult.IsEmpty())
        return {};
    
    std::string result;

    for (size_t currentTemplResult = 0;currentTemplResult < templResult.Size(); currentTemplResult++)
    {
        ScAddr const & linkWithDescription = templResult[currentTemplResult][Utils::linkWithDescriptionVarName];
        std::string linkContent;
        ms_context->GetLinkContent(linkWithDescription, linkContent);
        result += formatToTex(
            ScnTexKeywords::text, 
            Utils::getMainIdtf(ms_context, parameter, outputLang), 
            linkContent, 
            indentLevel);
    }

    return result;
}

std::string SemanticNeighborhoodToScnTranslationAgent::processSubdividingParameter(
    ScMemoryContext * ms_context,
    ScAddr const & concept,
    ScAddr const & parameter,
    int indentLevel
)
{
    ScIterator5Ptr subdivIterator = ms_context->Iterator5(
        ScType::NodeConstTuple, 
        ScType::EdgeDCommonConst, 
        concept, 
        ScType::EdgeAccessConstPosPerm, 
        parameter);

    if (!subdivIterator->IsValid())
        return {};

    std::string answer;

    while (subdivIterator->Next()) 
    {
        ScAddr const & subdivTuple = subdivIterator->Get(0);\

        answer += formatToTex(
            ScnTexKeywords::relfrom,
            formatRelation(Utils::getMainIdtf(ms_context, parameter, outputLang)),
            Utils::getMainIdtf(ms_context, subdivTuple, outputLang),
            indentLevel);
        answer += formatToTex(TexKeywords::begin, ScnTexKeywords::indent, indentLevel + 1);
        answer += formatToTex(TexKeywords::begin, ScnTexKeywords::eqtoset, indentLevel + 2);

        ScIterator3Ptr subdivElementsIterator = ms_context->Iterator3(
            subdivTuple, 
            ScType::EdgeAccessConstPosPerm, 
            ScType::Unknown);

        while (subdivElementsIterator->Next())
        {
            answer += formatToTex(
                ScnTexKeywords::item, 
                Utils::getMainIdtf(ms_context, subdivElementsIterator->Get(2), outputLang), 
                indentLevel + 3);
        }

        answer += formatToTex(TexKeywords::end, ScnTexKeywords::eqtoset, indentLevel + 2);
        answer += formatToTex(TexKeywords::end, ScnTexKeywords::indent, indentLevel + 1);

    }

    return answer;
}

std::string SemanticNeighborhoodToScnTranslationAgent::processSubjDomainParameter(
    ScMemoryContext * ms_context,
    ScAddr const & concept,
    ScAddr const & parameter,
    int indentLevel
)
{
    ScTemplate templ;
    Utils::makeSubjectDomainMembershipTemplate(concept, templ);

    ScTemplateSearchResult result;
    ms_context->HelperSearchTemplate(templ, result);

    if (result.IsEmpty())
        return {};
    
    std::string answer;

    for (size_t currentTemplResult = 0;currentTemplResult < result.Size(); currentTemplResult++)
    {
        answer += formatToTex(
            TexKeywords::begin, 
            ScnTexKeywords::iselementrole, 
            formatRelation(Utils::getMainIdtf(
                        ms_context, 
                        result[currentTemplResult][Utils::roleRelationVarName], 
                        outputLang)), 
            indentLevel);

        answer += Utils::makeIndent(indentLevel + 1) 
            + "{" 
            + Utils::getMainIdtf(ms_context, result[currentTemplResult][Utils::subjDomainVarName], outputLang)
            + "}\n";
        
        answer += formatToTex(TexKeywords::end, ScnTexKeywords::iselementrole, indentLevel);
    }
    return answer;
}

std::string SemanticNeighborhoodToScnTranslationAgent::processOutNoroleRelationParameter(
    ScMemoryContext * ms_context,
    ScAddr const & concept,
    ScAddr const & parameter,
    int indentLevel
)
{
    ScIterator5Ptr outRelationIterator = ms_context->Iterator5(
        concept, 
        ScType::EdgeDCommonConst, 
        ScType::Unknown, 
        ScType::EdgeAccessConstPosPerm, 
        parameter);

    if (!outRelationIterator->IsValid())
        return {};
    
    std::string answer;
    while (outRelationIterator->Next())
    {
        answer += formatToTex(
            ScnTexKeywords::relfrom, 
            formatRelation(Utils::getMainIdtf(ms_context, parameter, outputLang)),            
            Utils::getMainIdtf(ms_context, outRelationIterator->Get(2), outputLang),
            indentLevel);
    }

    return answer;
}

std::string SemanticNeighborhoodToScnTranslationAgent::formatRelation(std::string const & relIdtf)
{
    std::string answer = relIdtf;
    if (!answer.empty() 
        && (answer.back() == '*' || answer.back() == '\''))
    {
        answer.pop_back();
    }

    return answer;
}
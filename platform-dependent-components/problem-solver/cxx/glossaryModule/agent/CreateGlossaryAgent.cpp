#include "keynodes/GlossaryKeynodes.hpp"
#include "CreateGlossaryAgent.hpp"

#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "sc-agents-common/utils/CommonUtils.hpp"

using namespace utils;

using namespace glossaryModule;

ScAddr CreateGlossaryAgent::answerLang;

int const kWaitTime = 1000;

SC_AGENT_IMPLEMENTATION(CreateGlossaryAgent)
{
    ScAddr const & questionNode = otherAddr;

    if (!checkAction(questionNode))
        return SC_RESULT_OK;

    SC_LOG_DEBUG("CreateGlossaryAgent started");

    //Initialization
    ScAddr subjectDomainsSetAddr = 
        IteratorUtils::getAnyByOutRelation(
                &m_memoryCtx,
                questionNode, 
                scAgentsCommon::CoreKeynodes::rrel_1
            );
    
    ScAddr langAddr = 
        IteratorUtils::getAnyByOutRelation(
            &m_memoryCtx,
            questionNode, 
            scAgentsCommon::CoreKeynodes::rrel_2
        );

    ScAddr parametersSetAddr = 
        IteratorUtils::getAnyByOutRelation(
                &m_memoryCtx,
                questionNode, 
                scAgentsCommon::CoreKeynodes::rrel_3
            );
    
    //Validation
    if (!m_memoryCtx.IsElement(subjectDomainsSetAddr))
    {
        return exitInvalidParams("subject domains set node not found.", questionNode);
    }
    
    if (!m_memoryCtx.IsElement(langAddr))
    {
        return exitInvalidParams("language node not found..", questionNode);
    }
    else 
    {
        CreateGlossaryAgent::answerLang = langAddr;
    }

    if (!m_memoryCtx.IsElement(parametersSetAddr))
    {
        parametersSetAddr = GlossaryKeynodes::default_glossary_parameters;
        SC_LOG_WARNING(formatToLog(
            "parameters set node not found. By default, "
            + m_memoryCtx.HelperGetSystemIdtf(parametersSetAddr) + " is used."));
    }       
    
    //Creating glossary
    ScAddrVector answer;

    formGlossary(subjectDomainsSetAddr, parametersSetAddr, answer);

    AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, answer, true);
    SC_LOG_DEBUG("CreateGlossaryAgent finished");

    return SC_RESULT_OK;
}

void CreateGlossaryAgent::formGlossary(ScAddr const & setOfSubjDomains, ScAddr const & params, ScAddrVector & answerElements)
{
    clearPreviousResultNode(setOfSubjDomains, ScType::Node);
    clearPreviousResultNode(setOfSubjDomains, ScType::Link);

    ScAddr const & answerStruct = formScResultNode(setOfSubjDomains, answerElements);
    ScAddr const & answerLink = formScnTexResultNode(setOfSubjDomains, answerElements);

    std::string answerLinkContent = "\\begin{SCn}\n \\scnstructheader{";
    answerLinkContent += (answerLang == scAgentsCommon::CoreKeynodes::lang_ru 
        ? "Глоссарий для "
        : "Glossary for ");

    std::string setOfSubjDomainsIdtf = CommonUtils::getMainIdtf(&m_memoryCtx, setOfSubjDomains);
    if (setOfSubjDomainsIdtf.empty())
        setOfSubjDomainsIdtf = m_memoryCtx.HelperGetSystemIdtf(setOfSubjDomains);

    auto formatForTex = [](std::string const & input){
        std::string output;

        for (auto const & symbol : input)
        {
            if (symbol == '_')
            {
                output += "\\_";
            }
            else 
            {
                output += symbol;
            }
        }
        return output;
    };

    answerLinkContent += formatForTex(getMainIdtf(setOfSubjDomains)) + "}\n";
    answerLinkContent += "  \\begin{scnstruct}\n";

    ScAddrVector subjectDomains = getValidSetElementsByTypeAndValidSet(
        setOfSubjDomains, 
        ScType::NodeConstStruct, 
        GlossaryKeynodes::subject_domain, 
        "is not a subject domain"
    ); 

    std::multimap<std::string, std::string> conceptScnTex;

    for (auto const & currentSubjDomain : subjectDomains)
    {
        ScAddrVector concepts = getConceptsFromSubjDomain(currentSubjDomain);

        for (auto const & concept : concepts)
        {
            ScAddr const & actionForScResult = AgentUtils::applyActionAndGetResultIfExists(
                &m_memoryCtx, 
                GlossaryKeynodes::action_get_semantic_neighborhood, 
                {concept, params, answerLang});

            ScAddr const & actionForScnTex = AgentUtils::applyActionAndGetResultIfExists(
                &m_memoryCtx, 
                GlossaryKeynodes::action_translate_semantic_neighborhood_to_scn, 
                {concept, params, answerLang});
            
            if (actionForScResult.IsValid())
            {
                ScAddrVector resultElements = IteratorUtils::getAllWithType(&m_memoryCtx, actionForScResult, ScType::Unknown);
                for (auto & el : resultElements) 
                {
                    ScAddr arc = m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, answerStruct, el);
                    answerElements.insert(answerElements.end(), {el, arc});
                }

                ScAddr arc = m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, answerStruct, concept);
                answerElements.insert(answerElements.end(), {concept, arc});
            }
            else 
            {
                SC_LOG_WARNING(formatToLog("concept '" + m_memoryCtx.HelperGetSystemIdtf(concept) + "' does not have the required semantic neighborhood"));
            }

            if (actionForScnTex.IsValid())
            {
                ScAddr const & linkWithScn = IteratorUtils::getAnyFromSet(&m_memoryCtx, actionForScnTex);
                if (!linkWithScn.IsValid() || m_memoryCtx.GetElementType(linkWithScn) != ScType::LinkConst)
                    continue;

                std::string linkContent;
                m_memoryCtx.GetLinkContent(linkWithScn, linkContent);
                conceptScnTex.insert({getMainIdtf(concept), linkContent});
            }
            else 
            {
                SC_LOG_WARNING(formatToLog("concept '" + m_memoryCtx.HelperGetSystemIdtf(concept) + "' does not have the required semantic neighborhood"));
            }
        }
    }

    for (auto const &[conceptName, conceptScn] : conceptScnTex)
    {
        answerLinkContent += conceptScn;
    }

    answerLinkContent += "  \\end{scnstruct}\n";
    answerLinkContent += "\\end{SCn}\n";

    //It's just for beautiful output in sc-link. 
    //To get scn-tex text, remove the <pre> tag from 
    //the beginning and end of the sc-link content
    answerLinkContent = "<pre>" + answerLinkContent + "</pre>";

    m_memoryCtx.SetLinkContent(answerLink, answerLinkContent);

}

void CreateGlossaryAgent::clearPreviousResultNode(ScAddr const & setOfSubjDomains, ScType const & nodeType)
{
    ScIterator5Ptr previousResultsIterator = 
        m_memoryCtx.Iterator5(
            setOfSubjDomains,
            ScType::EdgeDCommonConst, 
            nodeType, 
            ScType::EdgeAccessConstPosPerm, 
            GlossaryKeynodes::nrel_glossary
        );

    while (previousResultsIterator->Next())
    {
        m_memoryCtx.EraseElement(previousResultsIterator->Get(2));
    }
}

ScAddrVector CreateGlossaryAgent::getConceptsFromSubjDomain(ScAddr const & subjDomain)
{
    ScAddrVector allConcepts;

    getConceptsFromSubjDomainByRelationType(
        subjDomain, 
        GlossaryKeynodes::rrel_maximum_studied_object_class, 
        allConcepts
    );

    getConceptsFromSubjDomainByRelationType(
        subjDomain, 
        GlossaryKeynodes::rrel_explored_relation, 
        allConcepts
    );

    getConceptsFromSubjDomainByRelationType(
        subjDomain, 
        GlossaryKeynodes::rrel_not_maximum_studied_object_class, 
        allConcepts
    );

    return allConcepts;
}

void CreateGlossaryAgent::getConceptsFromSubjDomainByRelationType(
    ScAddr const & subjDomain, 
    ScAddr const & relationType, 
    ScAddrVector & output
)
{
    ScIterator5Ptr subjDomainConceptsIterator = 
        m_memoryCtx.Iterator5(
            subjDomain,
            ScType::EdgeAccessConstPosPerm, 
            ScType::Unknown, 
            ScType::EdgeAccessConstPosPerm, 
            relationType
        );

    while (subjDomainConceptsIterator->Next())
    {
        output.emplace_back(subjDomainConceptsIterator->Get(2));
    }
}

bool CreateGlossaryAgent::checkAction(ScAddr const & questionNode) 
{
    return m_memoryCtx.HelperCheckEdge(
            GlossaryKeynodes::action_create_glossary,
            questionNode,
            ScType::EdgeAccessConstPosPerm
            );
}

ScAddrVector CreateGlossaryAgent::getValidSetElementsByTypeAndValidSet(
    ScAddr const & set, 
    ScType type, 
    ScAddr const & validSet, 
    std::string warnMessage
    ) 
{
    ScAddrVector validSetElements;
    ScAddrVector setElements =
        IteratorUtils::getAllWithType(
            &m_memoryCtx,
            set,
            type
        );

    for (ScAddr const & currentSetElement : setElements) 
    {
        bool isParameterValid = 
            m_memoryCtx.HelperCheckEdge(
                validSet,
                currentSetElement,
                ScType::EdgeAccessConstPosPerm
            );

        if (isParameterValid)
        {
            validSetElements.emplace_back(currentSetElement);
        }
        else 
        { 
            SC_LOG_WARNING(formatToLog(m_memoryCtx.HelperGetSystemIdtf(currentSetElement) + " " + warnMessage));
        }
    }

    return validSetElements;
}

std::string CreateGlossaryAgent::formatToLog(std::string const & message)
{
    return "CreateGlossaryAgent: " + message;
}

sc_result CreateGlossaryAgent::exitInvalidParams(std::string const & message, ScAddr const & questionNode) 
{
    SC_LOG_ERROR(formatToLog(message));
    AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, false);
    return SC_RESULT_ERROR_INVALID_PARAMS;
}

ScAddr CreateGlossaryAgent::formScResultNode(ScAddr const & setOfSubjDomains, ScAddrVector & answerElements)
{
    ScAddr const & resultsSetStruct = m_memoryCtx.CreateNode(ScType::NodeConstStruct);
    ScAddr const & createGlossaryResultRelationPair = 
        m_memoryCtx.CreateEdge(
            ScType::EdgeDCommonConst, 
            setOfSubjDomains, 
            resultsSetStruct
        );
    ScAddr const & relationAccessArc = 
        m_memoryCtx.CreateEdge(
            ScType::EdgeAccessConstPosPerm, 
            GlossaryKeynodes::nrel_glossary, 
            createGlossaryResultRelationPair
        );
    answerElements.insert(answerElements.end(), {
        resultsSetStruct, 
        createGlossaryResultRelationPair, 
        relationAccessArc,
        }
    );

    return resultsSetStruct;
}

ScAddr CreateGlossaryAgent::formScnTexResultNode(ScAddr const & setOfSubjDomains, ScAddrVector & answerElements)
{
    ScAddr const & resultLink = m_memoryCtx.CreateLink();

    ScAddr const & edgeBetweenSetAndLink = m_memoryCtx.CreateEdge(
        ScType::EdgeDCommonConst, 
        setOfSubjDomains, 
        resultLink);

    ScAddr const & accessArc = m_memoryCtx.CreateEdge(
        ScType::EdgeAccessConstPosPerm, 
        GlossaryKeynodes::nrel_glossary, 
        edgeBetweenSetAndLink);

    ScAddr formatEdge = m_memoryCtx.CreateEdge(
        ScType::EdgeDCommonConst, 
        resultLink,
        GlossaryKeynodes::format_html);
        
    m_memoryCtx.CreateEdge(
        ScType::EdgeAccessConstPosPerm, 
        GlossaryKeynodes::nrel_format, 
        formatEdge);
    
    answerElements.insert(answerElements.end(), {
        resultLink, 
        edgeBetweenSetAndLink, 
        accessArc});

    return resultLink;
}

std::string CreateGlossaryAgent::getMainIdtf(ScAddr const & concept)
{
    ScIterator5Ptr mainIdtfIterator = 
        m_memoryCtx.Iterator5(
            concept, 
            ScType::EdgeDCommonConst, 
            ScType::LinkConst, 
            ScType::EdgeAccessConstPosPerm, 
            scAgentsCommon::CoreKeynodes::nrel_main_idtf
        );
    
    if (!mainIdtfIterator->IsValid())
        return m_memoryCtx.HelperGetSystemIdtf(concept);

    while (mainIdtfIterator->Next())
    {
        ScAddr mainIdtfAddr = mainIdtfIterator->Get(2);
        bool isValidLang = m_memoryCtx.HelperCheckEdge(
            answerLang, 
            mainIdtfAddr,
            ScType::EdgeAccessConstPosPerm);
        if (isValidLang) 
        {
            std::string stringContent;
            if (m_memoryCtx.GetLinkContent(mainIdtfAddr, stringContent))
            {
                return stringContent;
            }
        }
    }

    return m_memoryCtx.HelperGetSystemIdtf(concept);
}






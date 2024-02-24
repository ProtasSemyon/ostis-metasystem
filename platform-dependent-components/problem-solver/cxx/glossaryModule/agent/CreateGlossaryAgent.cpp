#include "keynodes/GlossaryKeynodes.hpp"
#include "CreateGlossaryAgent.hpp"

#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"

using namespace utils;

using namespace glossaryModule;

ScAddr CreateGlossaryAgent::answerLang;

std::vector<std::string> CreateGlossaryAgent::idtfsVarForAdd = {
        "_defSet",
        "_defSet2",
        "_definition",
        "_arc1",
        "_arc2",
        "_arc3",
        "_arc4",
        "_arc5",
        "_arc6",
        "_arc7",
    };

std::string CreateGlossaryAgent::entityVarIdtf = "_definition";

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

    //Sets elements validation
    ScAddrVector subjectDomains = 
        getValidSetElementsByTypeAndValidSet(
            subjectDomainsSetAddr, 
            ScType::NodeConstStruct, 
            GlossaryKeynodes::subject_domain, 
            "is not a subject domain"
        );        
    
    ScAddrVector parameters = 
        getValidSetElementsByTypeAndValidSet(
            parametersSetAddr, 
            ScType::NodeConst, 
            GlossaryKeynodes::valid_glossary_parameters, 
            "is not a valid parameter"
        );

    //Creating glossary
    ScAddrVector answer;

    std::map<std::string, std::string> scnTexForConceptsMap;

    formGlossaryScFormat(subjectDomainsSetAddr, subjectDomains, parameters, answer);
    formGlossaryScnTexFormat(subjectDomainsSetAddr, subjectDomains, parameters, answer);

    AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, answer, true);
    SC_LOG_DEBUG("CreateGlossaryAgent finished");

    return SC_RESULT_OK;
}

void CreateGlossaryAgent::clearPreviousResultNode(ScAddr const & setOfSubjDomains, ScType const & resultNodeType)
{
    ScIterator5Ptr previousResultsIterator = 
        m_memoryCtx.Iterator5(
            setOfSubjDomains,
            ScType::EdgeDCommonConst, 
            resultNodeType, 
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

void CreateGlossaryAgent::makeDefinitionTemplate(ScAddr const & concept, ScAddr const & parameter, ScTemplate & templ)
{
    templ.Clear();
    templ.Triple(
        parameter, 
        ScType::EdgeAccessVarPosPerm >> "_arc1", 
        ScType::NodeVar >> "_defSet"
    );
    templ.Quintuple(
        "_defSet",
        ScType::EdgeAccessVarPosPerm >> "_arc2",
        concept,
        ScType::EdgeAccessVarPosPerm >> "_arc3",
        scAgentsCommon::CoreKeynodes::rrel_key_sc_element
    );
    templ.Quintuple(
        ScType::NodeVar >> "_defSet2",
        ScType::EdgeDCommonVar >> "_arc4",
        "_defSet", 
        ScType::EdgeAccessVarPosPerm >> "_arc5",
        scAgentsCommon::CoreKeynodes::nrel_sc_text_translation
    );
    templ.Quintuple(
        "_defSet2",
        ScType::EdgeAccessVarPosPerm >> "_arc6",
        ScType::LinkVar >> "_definition",
        ScType::EdgeAccessVarPosPerm >> "_arc7",
        GlossaryKeynodes::rrel_example
    );
    templ.Triple(
        CreateGlossaryAgent::answerLang, 
        ScType::EdgeAccessVarPosPerm,
        "_definition"
    );
}





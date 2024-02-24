#include "keynodes/GlossaryKeynodes.hpp"
#include "CreateGlossaryAgent.hpp"

using namespace utils;

using namespace glossaryModule;

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

void CreateGlossaryAgent::formGlossaryScFormat(
    ScAddr const & setOfSubjDomains,
    ScAddrVector const & subjectDomains, 
    ScAddrVector const & parameters, 
    ScAddrVector & answer
)
{
    clearPreviousResultNode(setOfSubjDomains, ScType::NodeConstStruct);
    
    ScAddr resultsSetStruct = formScResultNode(setOfSubjDomains, answer);

    for (ScAddr const & subjDomain : subjectDomains)
    {
        ScAddrVector subjDomainConcepts = getConceptsFromSubjDomain(subjDomain);
        for (ScAddr const & concept : subjDomainConcepts) {
            processConceptByParametersSc(concept, parameters, resultsSetStruct, answer);
        }
    }
}

void CreateGlossaryAgent::processConceptByParametersSc(
    ScAddr const & concept, 
    ScAddrVector const & parameters, 
    ScAddr const & resultStructure,
    ScAddrVector & answer
) 
{
    ScAddr accessArcFromResult =  m_memoryCtx.CreateEdge(
        ScType::EdgeAccessConstPosPerm, 
        resultStructure, 
        concept
    );
    answer.insert(answer.end(), {accessArcFromResult, concept});
    for (ScAddr const & parameter : parameters)
    {
        processConceptByParameterSc(concept, parameter, resultStructure, answer);
    }
} 

void CreateGlossaryAgent::processConceptByParameterSc(
      ScAddr const & concept, 
      ScAddr const & parameter, 
      ScAddr const & resultStructure,
      ScAddrVector & answer
)
{
    static ProcessConceptMapSc parameterFunctionsMap = {
        {GlossaryKeynodes::nrel_main_idtf, CreateGlossaryAgent::processConceptIdtfSc},
        {GlossaryKeynodes::nrel_idtf, CreateGlossaryAgent::processConceptIdtfSc},
        {GlossaryKeynodes::nrel_system_identifier, CreateGlossaryAgent::processConceptIdtfSc},
        {GlossaryKeynodes::definition, CreateGlossaryAgent::processConceptEntitySc},
        {GlossaryKeynodes::explanation, CreateGlossaryAgent::processConceptEntitySc},
        {GlossaryKeynodes::note, CreateGlossaryAgent::processConceptEntitySc},
    };
    
    if (parameterFunctionsMap.find(parameter) == parameterFunctionsMap.end())
    {
        SC_LOG_ERROR(formatToLog("parameter " 
            + m_memoryCtx.HelperGetSystemIdtf(parameter) 
            + " marked as valid, but not implemented"));
        return;
    }
    parameterFunctionsMap[parameter](&m_memoryCtx, concept, parameter, resultStructure, answer);
    
}

void addIfNotFound(
    ScMemoryContext * ms_context, 
    ScAddr const & addrForAdd, 
    ScAddr const & resultStructure, 
    ScAddrVector & output
)
{
    if (std::find(output.begin(), output.end(), addrForAdd) == output.end())
    {
        ScAddr accessArc = ms_context->CreateEdge(
            ScType::EdgeAccessConstPosPerm, 
            resultStructure, 
            addrForAdd
        );
        output.insert(output.end(), {accessArc, addrForAdd});
    }
}

void CreateGlossaryAgent::processConceptIdtfSc(
    ScMemoryContext * ms_context,
    ScAddr const & concept, 
    ScAddr const & parameter, 
    ScAddr const & resultStructure,
    ScAddrVector & answer
)
{
    ScIterator5Ptr idtfIterator = 
        ms_context->Iterator5(
            concept, 
            ScType::EdgeDCommonConst, 
            ScType::Unknown, 
            ScType::EdgeAccessConstPosPerm, 
            parameter
        );
    if (!idtfIterator->IsValid())
        return;

    addIfNotFound(ms_context, parameter, resultStructure, answer);

    while (idtfIterator->Next())
    {
        bool isLangCorrect = ms_context->HelperCheckEdge(
            answerLang, 
            idtfIterator->Get(2), 
            ScType::EdgeAccessConstPosPerm
        );

        if (!isLangCorrect && parameter != GlossaryKeynodes::nrel_system_identifier)
            continue;

        for (size_t addrIndexInIterator = 1; addrIndexInIterator < 4; addrIndexInIterator++)
        {
            ScAddr addrForAdd = idtfIterator->Get(addrIndexInIterator);
            ScAddr accessArc = ms_context->CreateEdge(
                ScType::EdgeAccessConstPosPerm, 
                resultStructure, 
                addrForAdd
            );
            answer.insert(answer.end(), {accessArc, addrForAdd});
        }
    }
}

void CreateGlossaryAgent::processConceptEntitySc(
    ScMemoryContext * ms_context,
    ScAddr const & concept, 
    ScAddr const & parameter, 
    ScAddr const & resultStructure,
    ScAddrVector & answer
)
{
    ScTemplate templ;
    makeDefinitionTemplate(concept, parameter, templ);

    ScTemplateSearchResult result;
    ms_context->HelperSearchTemplate(templ, result);

    auto addIfNotFoundLocal = [&](ScAddr const & addrForAdd){
        addIfNotFound(ms_context, addrForAdd, resultStructure, answer);
    };

    if (!result.IsEmpty())
    {
        addIfNotFoundLocal(parameter);
        addIfNotFoundLocal(scAgentsCommon::CoreKeynodes::rrel_key_sc_element);
        addIfNotFoundLocal(scAgentsCommon::CoreKeynodes::rrel_key_sc_element);
        addIfNotFoundLocal(scAgentsCommon::CoreKeynodes::nrel_sc_text_translation);
        addIfNotFoundLocal(GlossaryKeynodes::rrel_example);

        for (size_t currentTemplate = 0; currentTemplate < result.Size();currentTemplate++)
        {
            for (auto const & currentIdtf : idtfsVarForAdd)
            {
                ScAddr addrForAdd = result[currentTemplate][currentIdtf];
                ScAddr accessArc = ms_context->CreateEdge(
                    ScType::EdgeAccessConstPosPerm, 
                    resultStructure, 
                    addrForAdd
                );
                answer.insert(answer.end(), {accessArc, addrForAdd});
            }
        }
    }
}
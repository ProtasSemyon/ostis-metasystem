#include "keynodes/GlossaryKeynodes.hpp"
#include "CreateGlossaryAgent.hpp"

using namespace utils;

using namespace glossaryModule;

int const kIndentSize = 4;

std::string const scnTexOutFile = "/home/smn/ostis/testTex/test.tex";

std::string formatForTex(std::string const & input) 
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

std::string makeIndent(int indentLevel)
{
    std::string result;
    for (int i = 0; i < indentLevel * kIndentSize; i++)
        result += ' ';
    
    return result;
}

void CreateGlossaryAgent::formGlossaryScnTexFormat(
      ScAddr const & setOfSubjDomains, 
      ScAddrVector const & subjectDomains, 
      ScAddrVector const & parameters,
      ScAddrVector & answer
)
{   
    clearPreviousResultNode(setOfSubjDomains, ScType::LinkConst);

    ScAddr const & resultLink = formScnTexResultNode(setOfSubjDomains, answer);
    std::string resultLinkContent = "\\begin{SCn}\n";

    int indentLevel = 1;

    resultLinkContent += makeIndent(indentLevel) 
        + "\\scnstructheader{Глоссарий для " 
        + formatForTex(getMainIdtf(&m_memoryCtx, setOfSubjDomains)) 
        +"}\n";
    resultLinkContent += makeIndent(indentLevel) + "\\begin{scnstruct}\n";

    ScnTexConceptsMap conceptsMap;
    for (ScAddr const & subjDomain : subjectDomains)
    {
        ScAddrVector subjDomainConcepts = getConceptsFromSubjDomain(subjDomain);
        for (ScAddr const & concept : subjDomainConcepts) {
            processConceptByParametersScnTex(concept, parameters, conceptsMap, indentLevel + 1);
        }
    }

    for (auto const & [conceptName, conceptDescription] : conceptsMap)
    {
        resultLinkContent += conceptDescription;
    }

    resultLinkContent += makeIndent(indentLevel) + "\\end{scnstruct}\n";
    resultLinkContent += "\\end{SCn}\n";

    std::ofstream out(scnTexOutFile);
    out << resultLinkContent;
    out.close();

    m_memoryCtx.SetLinkContent(resultLink, "<pre>" + resultLinkContent + "</pre>");
}

void CreateGlossaryAgent::processConceptByParametersScnTex(
    ScAddr const & concept, 
    ScAddrVector const & parameters,
    ScnTexConceptsMap & conceptsMap,
    int indentLevel
)
{
    static ScAddrVector parametersOrder = {
        GlossaryKeynodes::nrel_system_identifier,
        GlossaryKeynodes::nrel_idtf,
        GlossaryKeynodes::definition,
        GlossaryKeynodes::explanation,
        GlossaryKeynodes::note,
    };

    std::string mainIdtf = formatForTex(getMainIdtf(&m_memoryCtx, concept));
    std::string conceptInScnTex = makeIndent(indentLevel) + "\\scnheader{" + mainIdtf + "}\n";

    for (auto const & parameter : parametersOrder)
    {
        if (std::find(parameters.begin(), parameters.end(), parameter) != parameters.end())
        {
            conceptInScnTex += processConceptByParameterScnTex(concept, parameter, indentLevel + 1);
        }
    }

    conceptsMap.insert({mainIdtf, conceptInScnTex});
}

std::string CreateGlossaryAgent::processConceptByParameterScnTex(
    ScAddr const & concept, 
    ScAddr const & parameter,
    int indentLevel
)
{
    static ProcessConceptMapScnTex parameterFunctionsMap = {
        {GlossaryKeynodes::nrel_system_identifier, processConceptIdtfScnTex},
        {GlossaryKeynodes::nrel_idtf, processConceptIdtfScnTex},
        {GlossaryKeynodes::definition, processConceptEntityScnTex},
        {GlossaryKeynodes::explanation, processConceptEntityScnTex},
        {GlossaryKeynodes::note, processConceptEntityScnTex},
    };

    if (parameterFunctionsMap.find(parameter) == parameterFunctionsMap.end())
    {
        SC_LOG_ERROR(formatToLog("parameter " 
            + m_memoryCtx.HelperGetSystemIdtf(parameter) 
            + " marked as valid, but not implemented"));
        return {};
    }

    return parameterFunctionsMap[parameter](&m_memoryCtx, concept, parameter, indentLevel);
}

std::string CreateGlossaryAgent::processConceptIdtfScnTex(
    ScMemoryContext * ms_context,
    ScAddr const & concept, 
    ScAddr const & parameter,
    int indentLevel
)
{
    ScIterator5Ptr idtfIterator = ms_context->Iterator5(
        concept, 
        ScType::EdgeDCommonConst, 
        ScType::LinkConst, 
        ScType::EdgeAccessConstPosPerm, 
        parameter);

    std::string processingResult;

    while (idtfIterator->Next())
    {
        ScAddr idtfAddr = idtfIterator->Get(2);
        bool isValidLang = ms_context->HelperCheckEdge(
            answerLang, 
            idtfAddr,
            ScType::EdgeAccessConstPosPerm);
        if (isValidLang || parameter == GlossaryKeynodes::nrel_system_identifier) 
        {
            std::string stringContent;
            if (ms_context->GetLinkContent(idtfAddr, stringContent))
            {
                processingResult += makeIndent(indentLevel) + "\\scnidtf{" + formatForTex(stringContent) + "}\n";
            }
        }
    }

    return processingResult;
}

std::string CreateGlossaryAgent::getMainIdtf(ScMemoryContext * ms_context, ScAddr const & concept)
{
    ScIterator5Ptr mainIdtfIterator = 
        ms_context->Iterator5(
            concept, 
            ScType::EdgeDCommonConst, 
            ScType::LinkConst, 
            ScType::EdgeAccessConstPosPerm, 
            GlossaryKeynodes::nrel_main_idtf
        );
    
    if (!mainIdtfIterator->IsValid())
        return ms_context->HelperGetSystemIdtf(concept);

    while (mainIdtfIterator->Next())
    {
        ScAddr mainIdtfAddr = mainIdtfIterator->Get(2);
        bool isValidLang = ms_context->HelperCheckEdge(
            answerLang, 
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

std::string CreateGlossaryAgent::processConceptEntityScnTex(
    ScMemoryContext * ms_context,
    ScAddr const & concept,
    ScAddr const & parameter,
    int indentLevel
)
{
    ScTemplate templ;
    makeDefinitionTemplate(concept, parameter, templ);

    ScTemplateSearchResult result;
    ms_context->HelperSearchTemplate(templ, result);

    if (!result.IsEmpty()) 
    {
        for (size_t currentTemplate = 0; currentTemplate < result.Size(); currentTemplate++)
        {
            std::string linkContent;
            ms_context->GetLinkContent(result[currentTemplate][entityVarIdtf], linkContent);
            return makeIndent(indentLevel) 
                + "\\scntext{" 
                + formatForTex(getMainIdtf(ms_context, parameter)) 
                + "}{"
                + formatForTex(linkContent)
                + "}\n";
        }
    }
    return "";
}

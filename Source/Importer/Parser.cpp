//---------------------------------------------------------------------------
#include "AGD Studio.pch.h"
//---------------------------------------------------------------------------
#include <cctype>
#include "Parser.h"
#include "Services/File.h"
#include "Project/Documents/DocumentManager.h"
//---------------------------------------------------------------------------
#include "Parser.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
using namespace Importer;
//---------------------------------------------------------------------------
Parser::Parser()
: m_CurrentVariable("")
{
}
//---------------------------------------------------------------------------
bool Parser::CreateMatchSets(const String& machine)
{
    // Create the pattern match sets
    auto id = std::make_unique<ImportDefinition>(machine);
    bool result = id->LoadFile(machine);
    if (result) {
        result &= AddMatchSection("window"        , id->Sections.Window        );
        result &= AddMatchSection("jumptable"     , id->Sections.JumpTable     );
        result &= AddMatchSection("controlset"    , id->Sections.ControlSet    );
        result &= AddMatchSection("objects"       , id->Sections.Objects       );
        result &= AddMatchSection("sprites"       , id->Sections.Sprites       );
        result &= AddMatchSection("blocks"        , id->Sections.Blocks        );
        result &= AddMatchSection("font"          , id->Sections.Font          );
        result &= AddMatchSection("palette"       , id->Sections.Palette       );
        result &= AddMatchSection("events"        , id->Sections.Events        );
        result &= AddMatchSection("messagelist"   , id->Sections.Messages      );
        result &= AddMatchSection("screens"       , id->Sections.Screens       );
        result &= AddMatchSection("spriteposition", id->Sections.SpritePosition);
        result &= AddMatchSection("map"           , id->Sections.Map           );
    } else {
        ErrorMessage("[Import Parser] Failed to load a file parser definition for machine. " + machine);
    }
    return result;
}
//---------------------------------------------------------------------------
bool Parser::AddMatchSection(const String& variable, ImportDefinition::Matcher& matcher)
{
    // find the section name from the match pattern
    auto lcp = matcher.Pattern.LowerCase();
    String sectionName = "";
    for (int i = 1; i <= lcp.Length(); i++) {
        if (!isalpha(lcp[i])) {
            sectionName = lcp.SubString(1, i - 1).LowerCase();
            break;
        }
    }
    if (sectionName != "") {
        // tokenize the matcher patterns for use during parsing
        matcher.Tokens = Tokenize(lcp, " ", true);
        matcher.Variable = variable.LowerCase();
        m_MatchSections[sectionName] = matcher;
    } else {
        ErrorMessage("[Import Parser] Section '" + variable + "' match pattern in parser definition file is not recognised.");
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
Tokens Parser::Tokenize(const String& line, const String& separator, bool incVars) const
{
    Tokens tokens;
    // section names must start on pos 1 and be an alpha character only
    bool couldBeSectionName = line.Length() > 1 && isalpha(line[1]);
    // since we use space(' ') as a saparator, then we have to replace any quoted ' 's with their ascii equilivant value
    auto parts = SplitString(line.Trim(), separator);
    for (auto part : parts) {
        Token token;
        if (token.ize(part, couldBeSectionName, incVars)) {
            tokens.push_back(token);
        }
        if (incVars && token.isa(Token::ttInvalid)) {
            ErrorMessage("[Import Parser] Importer definition match pattern format is invalid");
            ErrorMessage("[Import Parser] Line: " + line);
        }
        couldBeSectionName = false;
    }
    return tokens;
}
//---------------------------------------------------------------------------
bool Parser::Parse(const String& file, const String& machine)
{
    if (CreateMatchSets(machine)) {
        // Load the file
        InformationMessage("[Import Parser] Parsing AGD   : " + file);
        InformationMessage("[Import Parser] Import Machine: " + machine);
        auto lines = Services::File::ReadLines(file);
        // parse the lines
        auto lineCount = 1;
        for (auto line : lines) {
            try {
                auto lineTokens = ProcessLine(line);
                // no, tokenise the current line tokens against the current section tokens
                while (!lineTokens.empty()) {
                    // parse the current token
                    if (ParseToken(lineTokens.front())) {
                        // if parsed successfully, move to the next one
                        lineTokens.pop_front();
                    } else {
                        ErrorMessage("[Import Parser] Failed to parse line in file: " + file);
                        ErrorMessage("[Import Parser] Line: " + IntToStr(lineCount) + ": " + line);
                        return false;
                    }
                }
                lineCount++;
            } catch (Exception &exception) {
                ErrorMessage("[Import Parser] Section: " + m_CurrentMatcher.Variable + ", Pattern Match: " + m_CurrentMatcher.Pattern);
                ErrorMessage("[Import Parser] Exception: " + exception.Message);
                if (exception.InnerException) ErrorMessage("[Import Parser] Inner Exception: " + exception.InnerException->Message);
                ErrorMessage("[Import Parser] Failed to parse line in file: " + file);
                ErrorMessage("[Import Parser] Line: " + IntToStr(lineCount) + ": " + line);
                return false;
            }
        }
        InformationMessage("[Import Parser] Parsing completed successfully");
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool Parser::ParseToken(const Token& token)
{
    if (token.isa(Token::ttSection) && ProcessSection(token))
        return true;
    if (ProcessValue(token))
        return true;
    ParseError("Invalid token type found in import file.", token);
    return false;
}
//---------------------------------------------------------------------------
bool Parser::ProcessSection(const Token& token)
{
    auto name = token.Value.LowerCase();
    if (m_SectionTokens.size() > 0 && m_SectionTokens.front().isa(Token::ttArray)) {
        // we stop processing arrays here.
        PopSectionToken();
        // check if we can drop any ignore tokens
        while (m_SectionTokens.front().isa(Token::ttIgnore)) {
            m_SectionTokens.pop_front();
        }
        // keep processing if more tokens exists
        if (m_SectionTokens.size() > 0) {
            // false means we'll fall through to processing value tokens
            // the only occurence of this for now is the ENDMAP word match
            return false;
        }
    }
    // do we have a section token match for the token value (section name)?
    if (m_MatchSections.count(name) == 1) {
        // get the tokens to match for the new section
        m_SectionTokens = m_MatchSections[name].Tokens;
        // are they compatible?
        if (m_SectionTokens.front().isa(token.Type)) {
            auto varname = m_MatchSections[name].Variable;
            m_CurrentMatcher = m_MatchSections[name];
            // variable names with an 's' indicate an array (multiple sections named SPRITE or BLOCK etc)
            if (varname[varname.Length()] == 's') {
                // so we set an array counter for the variable
                if (m_VariableCounts.count(varname) == 0) {
                    m_VariableCounts[varname] = 0;
                }
                m_VariableCounts[varname] = m_VariableCounts[varname] + 1;
                // add the instance count to the variables name (ie. SPRITE1, SPRITE2 etc)
                varname += PadNum(UIntToStr(m_VariableCounts[varname]));
            }
            // change to the new variable
            m_CurrentVariable = varname.LowerCase();
            // pop the section token
            PopSectionToken();
            InformationMessage("[Import Parser] Starting Section: " + varname);
            return true;
        } else {
            ParseError("Invalid token type found in section match pattern.", token, m_SectionTokens.front());
            return false;
        }
    }
    ParseError("Invalid section name.", token);
    return false;
}
//---------------------------------------------------------------------------
bool Parser::ProcessValue(const Token& token)
{
    // get the current section token
    auto sectionToken = m_SectionTokens.front();
    // replace variable references with their respective values
    sectionToken = ReplaceVariableReferencesWithValues(sectionToken);
    // check it is a variable and excepts the same type as the line token
    auto secVar = SanitizeName(sectionToken.Value);
    if (sectionToken.isa(token.Type)) {
        if (sectionToken.isa(Token::ttVariable)) {
            // set(single)/add(array) value from line token to the section variable
            SetVariable(secVar, token.Value);
            // yes, then we keep the section token until all array elements are read
            if (m_ArrayCounts.count(m_CurrentVariable) == 0 || m_ArrayCounts[m_CurrentVariable].count(secVar) == 0) {
                // validate the number of items (by default all variables have 1 array element
                int count = 1;
                // find the size of the array
                auto bl = sectionToken.Value.Pos('[');
                auto br = sectionToken.Value.Pos(']');
                if (br > bl) {
                    auto sizes = SplitString(sectionToken.Value.SubString(bl + 1, br - bl - 1), ",");
                    for (auto s : sizes) {
                        count *= StrToInt(s);
                    }
                }
                m_ArrayCounts[m_CurrentVariable][secVar] = count;
            }
            // do we have all the array elements required?
            auto curcount = m_Variables[m_CurrentVariable][secVar].size();
            auto reqcount = m_ArrayCounts[m_CurrentVariable][secVar];
            if (m_ArrayCounts[m_CurrentVariable][secVar] > 0 && (curcount == reqcount || reqcount == 1)) {
                if (reqcount == 1) {
                    InformationMessage("[Import Parser] Variable set: " + secVar + " = " + token.Value);
                }
                // yes, move to the next section token
                PopSectionToken();
            }
        } else {
            // consume a word instead of a section at the start of a line
            if (token.isa(Token::ttWord) && token.Value.LowerCase() == secVar) {
                PopSectionToken();
                return true;
            }
            ParseError("Expected a Word token", token, sectionToken);
            return false;
        }
        return true;
    }
    if (sectionToken.isa(Token::ttLine)) {
        // any other tokens, while in line capture mode can still be processed
        return true;
    }
    ParseError("Tokens mismatch. ", token, sectionToken);
    return false;
}
//---------------------------------------------------------------------------
Tokens Parser::ProcessLine(const String& line)
{
    // tokenize the line
    auto nl = StringReplace(line, " ' ' ", " 32 ", TReplaceFlags()).Trim();
    Tokens tokens = Tokenize(nl, " ");
    // if the first token is not a section match and we are processing lines
    if (tokens.size() && m_MatchSections.count(tokens.front().Value) == 0 && m_SectionTokens.front().isa(Token::ttLine)) {
        // then add to the line array
        SetVariable(m_SectionTokens.front().Value, line);
        // a line comsumes all the remaining tokens
        tokens.clear();
    }
    // return our line tokens
    return tokens;
}
//---------------------------------------------------------------------------
Token Parser::ReplaceVariableReferencesWithValues(Token token)
{
    // variable value substitution
    auto pos = token.Value.Pos("var=");
    if (pos > 0) {
        // find the variable name from within the token string
        auto varName = token.Value.SubString(pos + 4, token.Value.Length() - pos - 4);
        for (auto i = 0; i < varName.Length(); i++) {
            if (varName.IsDelimiter(",]>", i)) {
                // strip out from the delimiter to the end of the string
                varName = varName.SubString(1, i - 1).LowerCase();
                break;
            }
        }
        if (m_Variables[m_CurrentVariable].size() > 0 && m_Variables[m_CurrentVariable][varName].size() > 0) {
            // ie. parameter "sprite.frames" in variable group "sprite.001"
            auto varValue = m_Variables[m_CurrentVariable][varName].front();
            token.Value = StringReplace(token.Value.LowerCase(), "var="+varName, varValue, TReplaceFlags());
        } else {
            // get the group name of the variable {group.parameter}
            // ie. window.width is group=window, parameter=width
            pos = varName.Pos(".");
            if (pos > 0) {
                auto group = varName.SubString(1, pos - 1);
                if (m_Variables[group].size() > 0) {
                    for (auto it = m_Variables[group].begin(); it != m_Variables[group].end(); it++) {
                        auto name = it->first;
                        if (m_Variables[group][name].size() > 0) {
                            auto varValue = m_Variables[group][name].front();
                            token.Value = StringReplace(token.Value.LowerCase(), "var="+name, varValue, TReplaceFlags());
                        }
                    }
                }
            }
        }
    }
    // substitute in the variable list counts {'var name'}
    if (token.Value.Pos("}") > token.Value.Pos("{")) {
        auto hp = token.Value.Pos("{");
        auto dp = token.Value.Pos("}");
        auto varName = SanitizeName(token.Value.SubString(hp + 1, dp - hp - 1));
        auto number = PadNum(UIntToStr(m_VariableCounts[varName]));
        token.Value = StringReplace(token.Value, "{" + varName + "}", number, TReplaceFlags());
    }
    // return the processed token
    return token;
}
//---------------------------------------------------------------------------
void Parser::ParseError(const String& message, Token token) const
{
    ErrorMessage("[Import Parser] " + message);
    ErrorMessage("[Import Parser] token: " + token.toStr());
}
//---------------------------------------------------------------------------
void Parser::ParseError(const String& message, Token lineToken, Token sectionToken) const
{
    ErrorMessage("[Import Parser] " + message);
    ErrorMessage("[Import Parser] Import token: " + lineToken.toStr());
    ErrorMessage("[Import Parser] Parser token: " + sectionToken.toStr());
}
//---------------------------------------------------------------------------
// Remove the current section pattern token we are matching to the incoming line token
void Parser::PopSectionToken()
{
    auto sectionToken = m_SectionTokens.front();
    if (sectionToken.isa(Token::ttArray)) {
        // get the variable name and display array information
        sectionToken = ReplaceVariableReferencesWithValues(sectionToken);
        auto secVar = SanitizeName(sectionToken.Value);
        auto curcount = m_Variables[m_CurrentVariable][secVar].size();
        if (curcount > 1) {
            String type = m_SectionTokens.front().isa(Token::ttLine) ? " lines" : " bytes";
            InformationMessage("[Import Parser] Array '" + secVar + "' is " + UIntToStr(curcount) + type);
        }
    }
    // finished processing section
    m_SectionTokens.pop_front();
    if (m_SectionTokens.size() == 0) {
        // finished the entire match pattern
        InformationMessage("[Import Parser] Finished Section: " + m_CurrentVariable);
    }
}
//---------------------------------------------------------------------------
String Parser::SanitizeName(const String& name) const
{
    // variable names are lower case and we remove the array dimensions
    return  (name.Pos("[") > 1) ? name.LowerCase().SubString(1, name.Pos("[") - 1) : name.LowerCase();
}
//---------------------------------------------------------------------------
void Parser::SetVariable(const String& var, const String& value)
{
    m_Variables[m_CurrentVariable][SanitizeName(var)].push_back(value);
}
//---------------------------------------------------------------------------
String Parser::PadNum(const String& string) const
{
    auto padded = "000" + string;
    return "." + padded.SubString(padded.Length() - 2, 3);
}
//---------------------------------------------------------------------------
bool Parser::hasVariable(const String& variable)
{
    return m_Variables.count(variable) != 0;
}
//---------------------------------------------------------------------------
int Parser::GetVarCount(const String& variable)
{
    return m_VariableCounts[variable];
}
//---------------------------------------------------------------------------
int Parser::GetArrayCount(const String& variable, const String& array)
{
    return m_ArrayCounts[variable][array];
}
//---------------------------------------------------------------------------


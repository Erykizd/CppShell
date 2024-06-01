#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>

using namespace std;

string deleteNotBracketsOrNotQuote(string str);
bool areCompleteBracketsOrQuotes(const string& str);
bool isBracketOrQuote(char c);
bool isCharInString(char c, const string& str);
bool areAllBracketsAndQuotesClosed(string str);
bool isKeyPressed(auto key);
bool isAnyLetterPressed();
string typeOfScope(string& scope);
void joinVectorElements(vector<string>& baseVector,const vector<string>& elementsToJoin);
void addScopeToValidVector(string &scope, vector<string> &objectsBeforeMainFunction,
     vector<string> &preprocessorInstructions, vector<string> &usings,
     vector<string> &mainFunctionLines);
void displayVector(vector<string>& v);
bool isFunctionWithType(string& scope, string& basicTypeStr);
void typeCode(string& textToType);
void clickKeyNTimes(auto key, int n);
void saveVectorToFile(vector<string>& vec, const string& fileName);
bool runCppFile(const string& fileName);
int getKeyForChar(char ch, int& additionalKey);

bool isScopeTemporary = false;

int main()
{
    bool displayCppFileContent = false;
    bool closedScope = false;
    string currentInputLine;
    string currentInputScope;
    string cleanedCurrentInputScope;
    int indexInOldScopes = 0;
    vector<string> oldScopes;
    vector<string> inputLines;
    vector<string> preprocessorInstructions;
    int preprocessorInstructionsOldSize;
    vector<string> usings;
    int usingsOldSize;
    vector<string> objectsBeforeMainFunction;
    int objectsBeforeMainFunctionOldSize;
    vector<string> mainFunctionLines;
    int mainFunctionLinesOldSize;
    vector<string> programLines;
    string fileName = "test.cpp";

    preprocessorInstructions.push_back("#include <iostream>");
    usings.push_back("using namespace std;");
    mainFunctionLines.push_back("int main()");
    mainFunctionLines.push_back("{");
    mainFunctionLines.push_back("\treturn 0;");
    mainFunctionLines.push_back("}");

    while(true)
    {
        programLines.clear();
        cout << "> ";
        //use arrows to get previous commands
        while(!isAnyLetterPressed() && !isKeyPressed(VK_RETURN) && oldScopes.size() > 0)
        {
            if(isKeyPressed(VK_UP))
            {
                clickKeyNTimes(VK_DOWN, 5);
                clickKeyNTimes(VK_BACK, oldScopes[indexInOldScopes].size());
                indexInOldScopes--;
                indexInOldScopes = indexInOldScopes % oldScopes.size();
                typeCode(oldScopes[indexInOldScopes]);
            }

            if(isKeyPressed(VK_DOWN))
            {
                clickKeyNTimes(VK_BACK, oldScopes[indexInOldScopes].size());
                indexInOldScopes++;
                indexInOldScopes = indexInOldScopes % oldScopes.size();
                typeCode(oldScopes[indexInOldScopes]);
            }
        }

        getline(cin, currentInputLine);
        currentInputScope += currentInputLine;
        cleanedCurrentInputScope = deleteNotBracketsOrNotQuote(currentInputScope);
        Sleep(200); //wait in ms for pressing shift if no new scope needed
        closedScope = areAllBracketsAndQuotesClosed(cleanedCurrentInputScope) && !isKeyPressed(VK_SHIFT);
        if(isKeyPressed(VK_SHIFT))
        {
            displayCppFileContent = !displayCppFileContent;
        }

        while(!closedScope)
        {
            cout << ">... ";
            getline(cin, currentInputLine);
            currentInputScope += ("\n\t" + currentInputLine);
            cleanedCurrentInputScope = deleteNotBracketsOrNotQuote(currentInputScope);
            Sleep(200); //wait in ms
            closedScope = areAllBracketsAndQuotesClosed(cleanedCurrentInputScope) && !isKeyPressed(VK_SHIFT);
        }

        objectsBeforeMainFunctionOldSize = objectsBeforeMainFunction.size();
        preprocessorInstructionsOldSize = preprocessorInstructions.size();
        usingsOldSize = usings.size();
        mainFunctionLinesOldSize = mainFunctionLines.size();

        oldScopes.push_back(currentInputScope);
        addScopeToValidVector(currentInputScope, objectsBeforeMainFunction, preprocessorInstructions, usings, mainFunctionLines);

        joinVectorElements(programLines, preprocessorInstructions);
        joinVectorElements(programLines, usings);
        joinVectorElements(programLines, objectsBeforeMainFunction);
        joinVectorElements(programLines, mainFunctionLines);

        if(displayCppFileContent)
        {
            cout << "cpp file content: " << endl;
            displayVector(programLines);
        }

        saveVectorToFile(programLines, fileName);
        if(runCppFile(fileName) && !isScopeTemporary) //if no compilation errors and not temporary scope
        {
            indexInOldScopes = oldScopes.size() - 1;
        }
        else
        {
            if(objectsBeforeMainFunctionOldSize < objectsBeforeMainFunction.size())
            {
                objectsBeforeMainFunction.pop_back();
            }
            else if(preprocessorInstructionsOldSize < preprocessorInstructions.size())
            {
                preprocessorInstructions.pop_back();
            }
            else if(usingsOldSize < usings.size())
            {
                usings.pop_back();
            }
            else if(mainFunctionLinesOldSize < mainFunctionLines.size())
            {
                swap(mainFunctionLines[mainFunctionLines.size() - 3], mainFunctionLines[mainFunctionLines.size() - 2]);
                swap(mainFunctionLines[mainFunctionLines.size() - 2], mainFunctionLines[mainFunctionLines.size() - 1]);
                mainFunctionLines.pop_back();
            }
        }


        currentInputScope = "";
        currentInputLine = "";
    }

    return 0;
}

string deleteNotBracketsOrNotQuote(string str)
{
    string ret;
    for (int i = 0; i < str.size(); ++i)
    {
        if (isBracketOrQuote(str.at(i)))
        {
            ret += str.at(i);
        }
    }

    return ret;
}

bool isBracketOrQuote(char c)
{
    string bracketsAndQuotes = "()[]{}'\"";
    return isCharInString(c, bracketsAndQuotes);
}

bool areCompleteBracketsOrQuotes(const string& str) // Użycie const string&
{
    return (str == "()" || str == "[]" || str == "{}" || str == "''" || str == "\"\"");
}

bool isCharInString(char c, const string& str)
{
    for (int i = 0; i < str.size(); ++i)
    {
        if (str.at(i) == c)
        {
            return true;
        }
    }
    return false;
}

bool areAllBracketsAndQuotesClosed(string str)
{
    bool isSomethingRemovedThisTime;
    do
    {
        isSomethingRemovedThisTime = false;
        if(str.size() > 1)
        {
            for (int i = 0; i < str.size() - 1; ++i)
            {
                if (areCompleteBracketsOrQuotes(str.substr(i, 2)))
                {
                    str.erase(i, 2);
                    isSomethingRemovedThisTime = true;
                    break;
                }
            }
        }
    }
    while (isSomethingRemovedThisTime); // Dodano brakujący średnik

    return str.empty();
}

bool isKeyPressed(auto key)
{
    return (GetAsyncKeyState(key) & 0x8000) != 0;
}

bool isAnyLetterPressed()
{
    for (char letter = 'A'; letter <= 'Z'; ++letter)
    {
        if (GetAsyncKeyState(letter) & 0x8000)
        {
            return true;
        }
    }

    return false;
}

string typeOfScope(string& scope)
{
    isScopeTemporary = false;

    vector<string> basicTypesStrs = {"int","double","float", "char", "void", "bool", "string", "vector", "array",
                                "short", "long", "long long", "unsigned short", "long double",
                                "unsigned int", "unsigned long", "unsigned long long", "unsigned char"};
    vector<string> elementsToAdd;

    for (int i = 0; i < basicTypesStrs.size(); ++i)
    {
        elementsToAdd.push_back(basicTypesStrs[i] + "*");
        elementsToAdd.push_back(basicTypesStrs[i] + " *");
        elementsToAdd.push_back(basicTypesStrs[i] + "**");
        elementsToAdd.push_back(basicTypesStrs[i] + " **");
        elementsToAdd.push_back(basicTypesStrs[i] + "&");
        elementsToAdd.push_back(basicTypesStrs[i] + " &");
    }

    joinVectorElements(basicTypesStrs, elementsToAdd);

    vector<string> advancedTypeStrs = {"#include", "using", "#define", "struct", "class", "public", "private"};

    for (const auto & advancedTypeStr : advancedTypeStrs)
    {
        if(advancedTypeStr.size() > scope.size())
        {
            continue;
        }

        if(scope.substr(0, advancedTypeStr.size()) == advancedTypeStr)
        {
            return advancedTypeStr;
        }
    }

    for (const auto & basicTypeStr : basicTypesStrs)
    {
        if(basicTypeStr.size() > scope.size())
        {
            continue;
        }

        if(scope.substr(0, basicTypeStr.size()) == basicTypeStr)
        {
            if(isFunctionWithType((string&) scope, (string&) basicTypeStr))
            {
                return basicTypeStr + " function";
            }
            return basicTypeStr;
        }
    }

    isScopeTemporary = true;

    return "none";
}

void joinVectorElements(vector<string>& baseVector,const vector<string>& elementsToJoin)
{
    for (const string & element : elementsToJoin)
    {
        baseVector.push_back(element);
    }
}

void displayVector(vector<string>& v)
{
    for (const auto & el : v)
    {
        cout << el << endl;
    }
}

bool isFunctionWithType(string& scope, string& basicTypeStr)
{
    if(basicTypeStr.size() > scope.size())
    {
        return false;
    }

    if(scope.substr(0, basicTypeStr.size()) != basicTypeStr)
    {
        return false;
    }

    int i;
    for (i = basicTypeStr.size(); i < scope.size(); ++i)
    {
        if(isCharInString(scope.at(i),")="))
        {
            return false;
        }

        if(isCharInString(scope.at(i),"("))
        {
            break;
        }
    }

    for (; i < scope.size(); ++i)
    {
        if(isCharInString(scope.at(i),")"))
        {
            break;
        }
    }

     return true;
}

void typeCode(string& textToType)
{
    int dt = 10;
    int additionalKey = 0;
    for (char letter : textToType)
    {
        additionalKey = 0;
        int virtualKey = getKeyForChar(letter, additionalKey);

        if (additionalKey)
        {
            keybd_event(additionalKey, 0, 0, 0);
            Sleep(dt);
        }

        keybd_event(virtualKey, 0, 0, 0);
        Sleep(dt);
        keybd_event(virtualKey, 0, KEYEVENTF_KEYUP, 0);
        Sleep(dt);

        if (additionalKey)
        {
            keybd_event(additionalKey, 0, KEYEVENTF_KEYUP, 0);
            Sleep(dt);
        }
    }
}

void clickKeyNTimes(auto key, int n)
{
    int dt = 10;
    for (int i = 0; i < n; ++i)
    {
        Sleep(dt);
        keybd_event(key, 0, 0, 0);
        Sleep(dt);
        keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
    }
}

void saveVectorToFile(vector<string>& vec, const string& fileName)
{
    ofstream outFile(fileName);

    if (!outFile)
    {
        cerr << "Can't open file: " << fileName << endl;
        return;
    }

    for (const std::string& str : vec)
    {
        outFile << str << endl;
    }

    outFile.close();
}

bool runCppFile(const string& fileName)
{
    string cmd = "g++ -std=c++20 -o test.exe " + fileName + " && test.exe";
    int exitCode = system(cmd.c_str());

    return exitCode == 0;
}

void addScopeToValidVector(string &scope, vector<string> &objectsBeforeMainFunction,
     vector<string> &preprocessorInstructions, vector<string> &usings,
     vector<string> &mainFunctionLines)
{
    string typeOfCurrentScope = typeOfScope(scope);
    if(typeOfCurrentScope == "class" || typeOfCurrentScope == "struct" || typeOfCurrentScope.ends_with("function"))
    {
        objectsBeforeMainFunction.push_back(scope);
    }
    else if(typeOfCurrentScope == "#include" || typeOfCurrentScope == "#define")
    {
        preprocessorInstructions.push_back(scope);
    }
    else if(typeOfCurrentScope == "using")
    {
        usings.push_back(scope);
    }
    else
    {
        if(isScopeTemporary && !(scope.substr(0,4) == "cout") && !(scope.at(scope.size() - 1) == ';'))
        {
            mainFunctionLines.push_back("\tcout << " + scope + " << endl;");
        }
        else
        {
            mainFunctionLines.push_back("\t" + scope);
        }
        swap(mainFunctionLines[mainFunctionLines.size() - 2], mainFunctionLines[mainFunctionLines.size() - 1]);
        swap(mainFunctionLines[mainFunctionLines.size() - 3], mainFunctionLines[mainFunctionLines.size() - 2]);
    }
}

int getKeyForChar(char ch, int& additionalKey)
{
    SHORT result = VkKeyScan(ch);

    if (result == -1)
    {
        cout << "Key for this sign can't be found: " << ch << std::endl;
        return 0;
    }

    int virtualKey = result & 0xff;
    int shiftState = (result >> 8) & 0xff;

    additionalKey = 0;
    if (shiftState & 1)
        additionalKey = VK_SHIFT;
    if (shiftState & 2)
        additionalKey = VK_CONTROL;
    if (shiftState & 4)
        additionalKey = VK_MENU; // alt button

    return virtualKey;
}

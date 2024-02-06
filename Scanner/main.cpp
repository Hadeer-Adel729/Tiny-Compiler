#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

struct Token {
    int lineNumber;
    string lexeme;
    string type;
};


class Scanner {
public:
    Scanner(const string& InputFile) : currentLineNumber(0) {
        inputFile.open(InputFile);
        if (!inputFile.is_open()) {
            cerr << "Error: Unable to open the input file." << endl;
            exit(EXIT_FAILURE);
        }
    }
    void tokenize() {
        string line;
        string token;
        bool inComment = false;  // Flag to track if currently inside a comment

        while (getline(inputFile, line)) {//This starts a loop that reads each line from the inputFile
            currentLineNumber++;

            size_t pos = 0; //to keep track of the current position within the line string.

            while (pos < line.length()) { //iterates over each character in the current line.
                // Skip whitespaces
                while (pos < line.length() && isspace(line[pos])) {
                    pos++;
                }
                token="";
                // Skip comments
                if (inComment) {
                    // Check for the end of the comment
                    size_t endPos = line.find('}', pos);
                    if (endPos != string::npos) {
                        inComment = false;
                        token = line[endPos];
                        processToken(token, currentLineNumber);
                        pos = endPos + 1;  // Move past the closing curly brace '}'
                    } else {
                        // The comment continues on the next line
                        break;
                    }
                } else if (pos < line.length() && line[pos] == '{') {
                    inComment = true;
                    token = line[pos];
                    processToken(token, currentLineNumber);
                    pos++; // Skip the opening curly brace '{'
                    continue; // Continue to the next iteration of the loop
                }
                // Extract the next token
                while (pos < line.length() && !isspace(line[pos]) && line[pos] != '{')
                {
                    if(IsLetterOrUnderscore(line[pos])){
                        while(IsLetterOrUnderscore(line[pos]))
                        {
                            token += line[pos];
                            pos++;
                        }

                    }else if(IsDigit(line[pos])){
                        while(IsDigit(line[pos]))
                        {
                            token += line[pos];
                            pos++;
                        }
                    }else if(IsSpecialSympol(line[pos])){

                        if(line[pos] == ':'){

                            token = line[pos];
                            pos++;
                        }
                        token += line[pos];
                        pos++;
                    }

                    if (!token.empty()) {
                        processToken(token, currentLineNumber);
                    }
                    token="";
                }

                // Move to the next character
                pos++;
            }
        }
        // Close the input file
        inputFile.close();
    }


void printTokens(string& filePath) {
    ofstream outputFile(filePath);

    // Check if the file is open
    if (outputFile.is_open()) {
        // Write the result to the file
        for (const auto& token : tokens) {
            outputFile << "[" << token.lineNumber << "] " << token.lexeme << " (" << token.type << ")\n";
            //cout << "[" << token.lineNumber << "] " << token.lexeme << " (" << token.type << ")\n";
        }
        cout << "Done!";
        // Close the file
        outputFile.close();
    } else {
        cerr << "Unable to open file: " << filePath << endl;
    }

}

private:
    ifstream inputFile;
    vector<Token> tokens;
    int currentLineNumber;

     bool IsDigit(char ch)
     {
         return (ch>='0' && ch<='9');
     }
     bool IsLetter(char ch)
     {
         return ((ch>='a' && ch<='z') || (ch>='A' && ch<='Z'));
     }
     bool IsLetterOrUnderscore(char ch)
     {
         return (IsLetter(ch) || ch=='_');
     }
     bool IsSpecialSympol(char ch)
    {
        return(ch == ':' || ch=='=' || ch=='<'|| ch=='>' || ch=='+' || ch=='-' ||
                ch=='*' || ch=='/' ||ch=='^' || ch==';' || ch=='(' || ch==')');
    }


    string getTokenType(const string& lexeme) {
    // Check if the lexeme is a keyword
        if (lexeme == "if")
            return "IF";
        else if (lexeme == "then")
            return "Then";
        else if (lexeme == "else")
            return "ELSE";
        else if (lexeme == "end")
            return "END";
        else if (lexeme == "repeat")
            return "REPEAT";
        else if (lexeme == "until")
            return "UNTIL";
        else if (lexeme == "read")
            return "READ";
        else if (lexeme == "write")
            return "WRITE";


        // Check if the lexeme is an operator
        if (lexeme == ":=" )
            return "ASSIGN";
        else if(lexeme== "<=")
            return "LESS_THAN_Or_EQUAL";
        else if(lexeme==">=")
            return "GREATER_THAN_OR_EQUEL";
        else if(lexeme == "=")
            return "EQUAL";
        else if(lexeme == "<")
            return "LESS_THAN";
        else if(lexeme==">")
            return "GREATER_THAN";
        else if(lexeme == "+")
            return "PLUS";
        else if(lexeme == "-")
            return "MINUS";
        else if(lexeme == "*")
            return "TIMES";
        else if(lexeme == "/")
            return "DIVIDE";
        else if(lexeme == "^")
            return "POWER";
        else if(lexeme == ";")
            return "SEMI_COLON";
        else if(lexeme == "(")
            return "LEFT_PAREN";
        else if(lexeme == ")")
            return "RIGHT_PAREN";
        else if(lexeme == "{")
            return "LEFT_BRACE";
        else if(lexeme == "}")
            return "RIGHT_BRACE";

        // Check if the lexeme is a number
        if (IsDigit(lexeme[0])) {
            return "Num";
        }

        // Check if the lexeme is an identifier
        if (IsLetterOrUnderscore(lexeme[0])) {
            return "ID";
        }
        // If none of the above, return an error
        return "Error";
    }

    void processToken(const string& lexeme, int lineNumber) {
        string type = getTokenType(lexeme);
        tokens.push_back({lineNumber, lexeme, type});
    }

};
int main(){
    string InputFile = "input1.txt";
    string outputFile = "output1.txt";
    Scanner Scanner(InputFile);

    Scanner.tokenize();
    Scanner.printTokens(outputFile);

    return 0;
}

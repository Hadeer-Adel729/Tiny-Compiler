#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <deque>

/********************Scanner**************************/

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
    vector<Token> tokenize() {
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

        return tokens;
    }


    void printTokens(string& filePathIn , vector<Token> tokenns) {
        ofstream outputFile(filePathIn);

        // Check if the file is open
        if (outputFile.is_open()) {
            // Write the result to the file
            for (const auto& token : tokenns) {
                outputFile << "[" << token.lineNumber << "] " << token.lexeme << " (" << token.type << ")\n";
            }
            cout << "Done!";
            // Close the file
            outputFile.close();
        } else {
            cerr << "Unable to open file: " << filePathIn << endl;
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
            return "NUMBER";
        }

        // Check if the lexeme is an identifier
        if (IsLetterOrUnderscore(lexeme[0])) {
            return "IDENTIFIER";
        }
        // If none of the above, return an error
        return "Error";
    }

    void processToken(const string& lexeme, int lineNumber) {
        string type = getTokenType(lexeme);
        tokens.push_back({lineNumber, lexeme, type});
    }

};

/*************************************Parser******************************************/

// Node structure for the parse tree
struct ParseNode {
    string label;
    string value;
 /*variable to check if the current ID for Read node or
   Assign node?to output it without spaces */
    bool check;
    deque<ParseNode> children;
};

class Analyzer {
public:
    // Constructor
    Analyzer(const string& filePathIn,const string& filePathOut) : scanner(filePathIn), Tokens(scanner.tokenize())
    {
        outputfil=filePathOut;
    }
    void parse() {
        try {
            ParseNode parseTree = ProgramParseTree();
            PrintTree(parseTree);

        } catch (const exception& e) {
            cerr << "Error: " << e.what() << endl;
        }
    }

private:
    Scanner scanner;
    vector<Token> Tokens;
    int currentTokenIndex = 0;
    string outputfil;
    ParseNode ProgramParseTree() {
        // program -> stmtseq

        ParseNode programNode;
        programNode.label = "PROGRAM";

        programNode.children.push_back(ParseStmtSeq());

        return programNode;
    }
    // stmtseq -> stmt { ; stmt }
    ParseNode ParseStmtSeq() {
        ParseNode stmtSeqNode;
        stmtSeqNode.label = "STMTSEQ";

        while (currentTokenIndex < Tokens.size()) {
            stmtSeqNode.children.push_back(ParseStmt());

            // Check for a semicolon and consume it if present
            if(currentTokenIndex < Tokens.size() && Tokens[currentTokenIndex].lexeme==";"){
                currentTokenIndex++;
            }//here to stop parsing new StmtSeq
            if(currentTokenIndex < Tokens.size() &&(Tokens[currentTokenIndex].lexeme=="until"
                                                    ||Tokens[currentTokenIndex].lexeme=="end")){
                break;
            }
        }

        return stmtSeqNode;
    }
    // stmt -> ifstmt | repeatstmt | assignstmt | readstmt | writestmt
    ParseNode ParseStmt() {
        ParseNode stmtNode;
        stmtNode.label = "STMT";

        if (currentTokenIndex < Tokens.size()) {
            if (Tokens[currentTokenIndex].lexeme == "if") {
                stmtNode.children.push_back(ParseIfStmt());
            } else if (Tokens[currentTokenIndex].lexeme == "repeat") {
                stmtNode.children.push_back(ParseRepeatStmt());
            } else if (Tokens[currentTokenIndex].lexeme == "read") {
                stmtNode.children.push_back(ParseReadStmt());
            } else if (Tokens[currentTokenIndex].lexeme == "write") {
                stmtNode.children.push_back(ParseWriteStmt());
            } else if (Tokens[currentTokenIndex].type == "IDENTIFIER") {
                stmtNode.children.push_back(ParseAssignStmt());
                //here we skip comments
            } else if (Tokens[currentTokenIndex].lexeme=="{"||Tokens[currentTokenIndex].lexeme=="}"){
                currentTokenIndex++;
            } else {
                cout << currentTokenIndex <<" " << Tokens[currentTokenIndex].type <<"\n";
                throw runtime_error("Unexpected token in statement.");
            }

        }

        return stmtNode;
    }
    // ifstmt -> if exp then stmtseq [ else stmtseq ] end
    ParseNode ParseIfStmt(){
        ParseNode ifStmtNode;
        ifStmtNode.label = "IF_NODE";
        // Consume "if"
        if (currentTokenIndex < Tokens.size() && Tokens[currentTokenIndex].lexeme == "if") {
            currentTokenIndex++;
        } else {
            cout << "Error: Expected 'if' keyword." << endl;
        }
        ParseNode ifNode;
        ifNode.label = "If";
        ifStmtNode.children.push_back(ifNode);

        // Parse expression
        ifStmtNode.children.push_back(ParseExpr());

        // Consume "then"
        if (currentTokenIndex < Tokens.size() && Tokens[currentTokenIndex].lexeme == "then") {
            currentTokenIndex++;
        } else {
            cout << "Error: Expected 'then' keyword." << endl;
        }

        // Parse statement sequence
        ifStmtNode.children.push_back(ParseStmtSeq());

        // Parse optional "else"
        if (currentTokenIndex < Tokens.size() && Tokens[currentTokenIndex].lexeme == "else") {
            currentTokenIndex++;

            ParseNode elsNode;
            elsNode.label = "Else";
            ifStmtNode.children.push_back(elsNode);
            ifStmtNode.children.push_back(ParseStmtSeq());
        }

        // Consume "end"
        if (currentTokenIndex < Tokens.size() && Tokens[currentTokenIndex].lexeme == "end") {
            currentTokenIndex++;
        } else {
            cout << "Error: Expected 'end' keyword." << endl;
        }

        return ifStmtNode;
    }
    // repeatstmt -> repeat stmtseq until expr
    ParseNode ParseRepeatStmt() {
        ParseNode repeatStmtNode;
        repeatStmtNode.label = "REPEAT_NODE";

        // Consume "repeat"
        if (currentTokenIndex < Tokens.size() && Tokens[currentTokenIndex].lexeme == "repeat") {
            currentTokenIndex++;
        } else {
            cout << "Error: Expected 'repeat' keyword in repeat statement." << endl;
        }
        ParseNode repeatNode;
        repeatNode.label = "Repeat";
        repeatStmtNode.children.push_back(repeatNode);
        // Parse statement sequence (stmtseq)
        repeatStmtNode.children.push_back(ParseStmtSeq());

        // Consume "until"
        if (currentTokenIndex < Tokens.size() && Tokens[currentTokenIndex].lexeme == "until") {
            currentTokenIndex++;
        } else {
            cout << "Error: Expected 'until' keyword in repeat statement." << endl;
        }
        //ParseNode UntilNode;
        //UntilNode.label = "Until";
        //repeatStmtNode.children.push_back(UntilNode);

        // Parse expression (expr)
        repeatStmtNode.children.push_back(ParseExpr());

        return repeatStmtNode;
    }
    // readstmt -> read identifier
    ParseNode ParseReadStmt() {
        ParseNode readStmtNode;
        readStmtNode.label = "READ_NODE";
        // Consume "read"
        if (currentTokenIndex<Tokens.size()&&Tokens[currentTokenIndex].lexeme=="read") {
            currentTokenIndex++;

        } else {
            cout << "Error: Expected 'read' keyword in read statement." << endl;
        }
        ParseNode ReadNode;
        ReadNode.label = "Read";
        readStmtNode.children.push_back(ReadNode);

        // Parse identifier
        if (currentTokenIndex<Tokens.size()&&Tokens[currentTokenIndex].type=="IDENTIFIER") {
            ParseNode idNode;
            idNode.label = "ID";
            idNode.check=1;
            idNode.value = Tokens[currentTokenIndex].lexeme;
            readStmtNode.children.push_back(idNode);
            currentTokenIndex++;
        } else {
            cout << "Error: Expected identifier after 'read' keyword." << endl;
        }

        return readStmtNode;
    }
    // writestmt -> write expr
    ParseNode ParseWriteStmt() {
        ParseNode writeStmtNode;
        writeStmtNode.label = "WRITE_NODE";

        // Consume "write"
        if (currentTokenIndex < Tokens.size() && Tokens[currentTokenIndex].lexeme == "write") {
            currentTokenIndex++;
        } else {
            cerr << "Error: Expected 'write' keyword in write statement." << endl;
        }
        ParseNode writeNode;
        writeNode.label = "Write";
        writeStmtNode.children.push_back(writeNode);
        // Parse expression (expr)
        writeStmtNode.children.push_back(ParseExpr());

        return writeStmtNode;
    }
    // assignstmt -> identifier := expr
    ParseNode ParseAssignStmt() {
        ParseNode assignStmtNode;
        assignStmtNode.label = "ASSIGN_NODE";

        // Parse identifier
        if (currentTokenIndex < Tokens.size() && Tokens[currentTokenIndex].type == "IDENTIFIER") {
            ParseNode idNode;
            idNode.label = "ID";
            idNode.check=1;
            idNode.value = Tokens[currentTokenIndex].lexeme;
            assignStmtNode.children.push_back(idNode);
            currentTokenIndex++;
        } else {
            cerr << "Error: Expected identifier in assignment statement." << endl;
        }

        // Consume ":="
        if (currentTokenIndex < Tokens.size() && Tokens[currentTokenIndex].lexeme == ":=") {
            ParseNode assignNode;
            assignNode.label = "Assign";
            assignNode.check=0;
            assignStmtNode.children.push_front(assignNode);
            currentTokenIndex++;
        } else {
            cout<<currentTokenIndex << "\n";
            cerr << "Error: Expected ':=' in assignment statement." << endl;
        }
        // Parse expression (expr)
        assignStmtNode.children.push_back(ParseExpr());

        return assignStmtNode;
    }
    // expr -> mathexpr [ (<|=) mathexpr ]
    ParseNode ParseExpr() {
        ParseNode exprNode;
        exprNode.label = "EXPR";

        // Parse logical expression
        exprNode.children.push_back(ParseMathExpr());

        // Parse optional comparison
        if (currentTokenIndex < Tokens.size() && (Tokens[currentTokenIndex].lexeme == "<" || Tokens[currentTokenIndex].lexeme == "=")) {
            ParseNode operNode;
            operNode.label = "Oper";
            operNode.value = Tokens[currentTokenIndex].type;
            exprNode.children.push_front(operNode);
            currentTokenIndex++;

            // Parse the second math expression
            exprNode.children.push_back(ParseMathExpr());
        }

        return exprNode;
    }
    // mathexpr -> term { (+|-) term }    left associative
    ParseNode ParseMathExpr() {
        ParseNode mathExprNode;
        mathExprNode.label = "MATHEXPR";

        // Parse the first term
        mathExprNode.children.push_back(ParseTerm());

        // Parse additional terms if any
        while (currentTokenIndex < Tokens.size() && (Tokens[currentTokenIndex].lexeme == "+" || Tokens[currentTokenIndex].lexeme == "-")) {
            ParseNode operNode;
            operNode.label = "Oper";
            operNode.value = Tokens[currentTokenIndex].type;
            mathExprNode.children.push_front(operNode);
            currentTokenIndex++;

            // Parse the next term
            mathExprNode.children.push_back(ParseTerm());
        }

        return mathExprNode;
    }
    // term -> factor { (*|/) factor }    left associative
    ParseNode ParseTerm() {
        ParseNode termNode;
        termNode.label = "TERM";

        // Parse the first factor
        termNode.children.push_back(ParseFactor());

        // Parse additional factors if any
        while (currentTokenIndex < Tokens.size() && (Tokens[currentTokenIndex].lexeme == "*" || Tokens[currentTokenIndex].lexeme == "/")) {
            ParseNode operNode;
            operNode.label = "Oper";
            operNode.value = Tokens[currentTokenIndex].type;
            termNode.children.push_front(operNode);
            currentTokenIndex++;

            // Parse the next factor
            termNode.children.push_back(ParseFactor());
        }

        return termNode;
    }
    // factor -> newexpr { ^ newexpr }    right associative
    ParseNode ParseFactor() {
        ParseNode factorNode;
        factorNode.label = "FACTOR";

        // Parse the base expression (newexpr)
        factorNode.children.push_back(ParseNewExpr());

        // Parse additional exponentiation if any exist
        while (currentTokenIndex < Tokens.size() && Tokens[currentTokenIndex].lexeme == "^") {
            ParseNode operNode;
            operNode.label = "Oper";
            operNode.value = Tokens[currentTokenIndex].type;
            factorNode.children.push_front(operNode);
            currentTokenIndex++;

            // Parse the next newexpr
            factorNode.children.push_back(ParseNewExpr());
        }


        return factorNode;
    }
    // newexpr -> ( mathexpr ) | number | identifier
    ParseNode ParseNewExpr() {
        ParseNode newExprNode;
        newExprNode.label = "NEWEXPR";

        if (currentTokenIndex < Tokens.size()) {
            if (Tokens[currentTokenIndex].type == "NUMBER") {
                // Case: number
                ParseNode NumNode;
                NumNode.label = "NUM";
                NumNode.value = Tokens[currentTokenIndex].lexeme;
                newExprNode.children.push_back(NumNode);
                currentTokenIndex++;

            } else if (Tokens[currentTokenIndex].type == "IDENTIFIER") {
                // Case: identifier
                ParseNode idNode;
                idNode.label = "ID";
                idNode.value = Tokens[currentTokenIndex].lexeme;
                newExprNode.children.push_back(idNode);
                currentTokenIndex++;
            }else if (Tokens[currentTokenIndex].lexeme== "(") {
                ParseNode OpenNode;
                OpenNode.label = "(";
                OpenNode.value = Tokens[currentTokenIndex].lexeme;
                newExprNode.children.push_back(OpenNode);

                // Case: ( mathexpr )
                currentTokenIndex++;  // Consume "("
                newExprNode.children.push_back(ParseMathExpr());

                // Consume ")"
                if (currentTokenIndex < Tokens.size() && Tokens[currentTokenIndex].lexeme == ")") {
                    ParseNode CloseNode;
                    CloseNode.label = ")";
                    CloseNode.value = Tokens[currentTokenIndex].lexeme;
                    newExprNode.children.push_back(CloseNode);
                    currentTokenIndex++;
                } else {
                    cout << "currentTokenIndex: " << currentTokenIndex << Tokens[currentTokenIndex].lexeme << " Error: Expected ')' in newexpr." << endl;
                }
            } else {
                cout << "Error: Unexpected token in newexpr." << endl;
            }
        } else {
            cout << "Error: Unexpected end of input in newexpr." << endl;
        }

        return newExprNode;
    }
    //here we check if node is a leaf node or not
    bool NodeKind(const ParseNode& node){
        return(node.label!="PROGRAM" && node.label!="STMTSEQ" && node.label!="STMT"
               &&node.label!="READ_NODE"&&node.label!="ASSIGN_NODE"&&node.label!="IF_NODE"
               &&node.label!="EXPR"&&node.label!="MATHEXPR"&&node.label!="TERM"
               &&node.label!="FACTOR"&&node.label!="NEWEXPR"
               &&node.label!="REPEAT_NODE"&&node.label!="WRITE_NODE"
               &&node.label!="("&&node.label!=")");
    }

    //Function to print the Parse tree
    void PrintTree(const ParseNode& node, int sh=0)
    {
        ofstream output(outputfil,ios::app);
        if(output.is_open()) {

            int  NSH=1,flag=0 ;
            if(NodeKind(node)){

                if(node.check!=1)for(int i=0;i<sh;i++) output<<" ";

                if(node.check==0)output << "[" << node.label << "]";
                if(node.label=="Oper") output << "[" << node.value << "]";
                else if(node.label=="NUM") output << "[" << node.value << "]";
                else if(node.label=="ID") output << "[" << node.value << "]";
                else if(node.label=="Read"||node.label=="Assign"){
                    flag=1;
                }

                if(flag==0)output << endl;
            }
            for (const auto& child : node.children) {
                PrintTree(child, sh+NSH);

            }
            output.close();
        }else{
            cout<<"Unable to open output file\n";
        }

    }

};
int main() {
    string InputFile = "input.txt";
    string  OutputFile = "output.txt";

    Analyzer tinyAnalyzer(InputFile,OutputFile);
    tinyAnalyzer.parse();
    return 0;
}

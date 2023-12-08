#include "Lexer.h"
#include "Parser.h"
#include "ErrorHandler.h"
#include "ir/Driver.h"
#include "backend/MipsDriver.h"
#include "SymTable.h"
using namespace std;

int main() {
    ifstream file("testfile.txt");
    ostringstream oss;
    ofstream output, outputMIPS;
    //output.open("error.txt");
    output.open("llvm_ir.txt");
    outputMIPS.open("mips.txt");
    if (file.is_open()) {
        oss << file.rdbuf();
        string testfile = oss.str();
        ErrorHandler errorHandler =  ErrorHandler(output);
        Lexer lexer =  Lexer(testfile, errorHandler);
        Parser parser = Parser(lexer, errorHandler, output);
        file.close();
        parser.preLexer();
        parser.CompUnit();
        errorHandler.errorHandle(parser.compUnit_);
        Driver driver = Driver(parser.compUnit_, output);
        driver.run();
        Module* irModule = driver.getModule();
        MipsDriver* mipsDriver = new MipsDriver(irModule, outputMIPS);
    } else {
        cout << "error_ in file open" << endl;
    }
    return 0;
}

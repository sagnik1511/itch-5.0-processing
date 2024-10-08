#include "include/parser.hpp"

int main(){
    std::string binary_file = "/workspaces/itch-5.0-processing/01302019.NASDAQ_ITCH50";
    Parser parser = Parser(binary_file);

    parser.parse();
    parser.processRunningVWAP();

    return 0;

}
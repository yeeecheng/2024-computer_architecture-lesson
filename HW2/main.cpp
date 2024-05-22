#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include "tomasulo.h"
using namespace std;

int main(int argc, char* argv[]){
    string file_name = argv[1];
    //string file_name = "./sample_data/sample3.txt";
    cout << "load " << file_name << "\n";
    ifstream infile;
    infile.open(file_name);
    string ss;

    Tomasulo tomasulo;

    while(getline(infile, ss)){
        tomasulo.SetInstructionUnit(ss);
    } 
    tomasulo.Start();
}
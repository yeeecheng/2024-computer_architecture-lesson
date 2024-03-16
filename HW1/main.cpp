#include <fstream>
#include <sstream>
#include <iostream>

#include "cache.h"
using namespace std;

int main(int argc, char* argv[]){
    string file_name = argv[1];
    
    int cache_size = stoi(argv[2]), block_size = stoi(argv[3]), set_degree = stoi(argv[4]);
    cache_controller cache = cache_controller(cache_size, block_size, set_degree);
    // file read
    ifstream infile;
    infile.open(file_name);
    string ss;
    while(infile >> ss){
        cache.dashboard_content += "bytes address: " + ss + " (hex)"; 
        cache.access_address(ss);
        cache.dashboard_content += "--------------------------------------------------------\n"; 
    }
    cache.dashboard_content += "Hit count: " + to_string(cache.hit_count) + " ,Miss count: " + to_string(cache.miss_count) + "\n";
    cache.dashboard_content += "Miss rate: " + to_string(float(cache.miss_count) / float(cache.miss_count + cache.hit_count)) + "\n";
    cout << "Hit count: " << cache.hit_count << " ,Miss count: " << cache.miss_count << "\n";
    cout << "Miss rate: " << float(cache.miss_count) / float(cache.miss_count + cache.hit_count) << "\n";

    // file write
    ofstream outfile;
    string save_path = "./out/";
    outfile.open(save_path + to_string(cache_size) + "_" + to_string(block_size) + "_" + to_string(set_degree) + ".txt");
    outfile << cache.dashboard_content;
    outfile.close();
    return 0;
}


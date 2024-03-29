#ifndef CAHCE_H
#define CACHE_H
#include <iostream>
#include <bitset>
#include <vector>
#include <sstream>
using namespace std;

class cache_controller;

class cache_block{
    friend class cache_controller;
    public:
        cache_block() : valid(false), time(0){}
    private:
        bool valid;
        int tag;
        int data;
        int time;
};
class cache_controller{
    public:
    
        int hit_count = 0, miss_count = 0;
        string dashboard_content = "";
        /*
        cache_size: KByte
        block_size: Word
        set_degree: number of block in one set
        */
        cache_controller(int cache_size, int block_size, int set_degree)
        : cache_size(cache_size), block_size(block_size), set_degree(set_degree), count(0){
            // num of total block
            num_block = cache_size * 256 / block_size;
            // num of set
            num_set = num_block / set_degree;
            // create cache 
            cache.resize(num_set, vector<cache_block>(set_degree));
            
            dashboard_content += "Cache Setting: \n";
            dashboard_content += "[cache size]: " + to_string(cache_size) + "K ,[block size]: " + to_string(block_size) + "word ,[n-way]: " + to_string(set_degree) + "\n";
            dashboard_content += "[number of total block]: " + to_string(num_block) + " ,[number of set]: " + to_string(num_set) + "\n";
            dashboard_content += "--------------------------------------------------------\n";
            cout << "Cache Setting: \n";
            cout << "[cache size]: " << cache_size << "K ,[block size]: " << block_size << "word ,[n-way]: " << set_degree << "\n";
            cout << "[number of total block]: " << num_block << " ,[number of set]: " << num_set << "\n";
            cout << "--------------------------------------------------------\n";
        }
        void access_address(string mem_address_hex){
            int mem_byteAddress = stoi(mem_address_hex, 0, 16);
            // number of memory block
            int mem_blockAddress = mem_byteAddress / (block_size * 4);
            int loc_set_addr = mem_blockAddress % num_set;
            // get tag (bytes address / (idx + offset))
            int tag = mem_blockAddress / num_set;
            // add access count
            count++;
            dashboard_content += "[time]: " + to_string(count) + "\n";
            dashboard_content += "bytes address: " + mem_address_hex + " (hex)"; 
            dashboard_content +=  " --> " + to_string(mem_byteAddress) + " (decimal)\n";
            dashboard_content += "[memory block number]: " + to_string(mem_blockAddress);
            dashboard_content += " ,[set address]: " + to_string(loc_set_addr);
            dashboard_content += " ,[tag]: " + to_string(tag) + "\n";
            // Miss
            if(!check_valid(loc_set_addr, tag)){
                /*
                There are two condition in Miss:
                1) All block's valid is true, but tag isn't match.
                2) There's some block's valid is false which can put data here.
                */
                bool chk = false;
                // count LRU times, record the recently access time.
                pair<int, int> min_time_idx = {INT32_MAX, -1};
                // need placed index 
                int place_idx = -1;
                for (int i = 0; i < set_degree; i++){
                    // find the block which is minimum time 
                    if(min_time_idx.first > cache[loc_set_addr][i].time){
                        min_time_idx = {cache[loc_set_addr][i].time, i};
                    }
                
                    // 2)
                    if(!cache[loc_set_addr][i].valid){
                        place_idx = i;
                        chk = true;
                        break;
                    }
                }
                if(!chk){
                    // 1)
                    dashboard_content +=  "LRU trigger, Replace tag " + to_string(cache[loc_set_addr][min_time_idx.second].tag) + ", which recently access time is " + to_string(min_time_idx.first) + "\n" ;
                    place_idx = min_time_idx.second;
                }
                cache[loc_set_addr][place_idx].valid = true;
                cache[loc_set_addr][place_idx].tag = tag;
                cache[loc_set_addr][place_idx].data = mem_blockAddress;
                cache[loc_set_addr][place_idx].time = count;
                miss_count++;
            }
            else{
                // Hit
                // update the times
                for (int i = 0; i < set_degree; i++){
                    if(cache[loc_set_addr][i].valid && cache[loc_set_addr][i].tag == tag){
                        cache[loc_set_addr][i].time = count;
                        continue;
                    }
                }
                hit_count++;
            }
        }



    private:
        int cache_size;
        int block_size;
        int set_degree;
        int num_block;
        int num_set;
        // record total access address
        int count;
        vector<vector<cache_block>> cache;
        

        bool check_valid(int loc_set_addr, int tag){
        
            for (int i = 0; i < set_degree; i++){
                if(cache[loc_set_addr][i].valid && cache[loc_set_addr][i].tag == tag){
                    dashboard_content += "Hit\n";

                    return true;
                }
            }
            dashboard_content += "Miss\n";

            return false;
        }

};

#endif
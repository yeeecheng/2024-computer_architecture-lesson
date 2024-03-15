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
        : cache_size(cache_size), block_size(block_size), set_degree(set_degree){
            // num of total block
            num_block = cache_size * 256 / block_size;
            // num of set
            num_set = num_block / set_degree;
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
            dashboard_content +=  " --> " + to_string(mem_byteAddress) + " (decimal)\n";
            // cout << " --> " << mem_byteAddress << " (decimal)\n";
            // number of memory block
            int mem_blockAddress = mem_byteAddress / (block_size * 4);
            dashboard_content += "[memory block number]: " + to_string(mem_blockAddress);
            // cout << "[memory block number]: " << mem_blockAddress ;
            int loc_set_addr = mem_blockAddress % num_set;
            dashboard_content += " ,[set address]: " + to_string(loc_set_addr);
            // cout << " ,[set address]: " << loc_set_addr;
            // get tag
    
            int tag = mem_blockAddress >> (get_idx_bit() + block_size);
            dashboard_content += " ,[tag]: " + to_string(tag) + "\n";
            // cout << " ,[tag]: " << tag << "\n";
            // Miss
            if(!check_valid(loc_set_addr, tag)){
                /*
                There are two condition in Miss:
                1) All block's valid is true, but tag isn't match.
                2) There's some block's valid is false which can put data here.
                */
                bool chk = false;
                pair<int, int> min_time_idx = {INT32_MAX, -1};
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
                    place_idx = min_time_idx.second;
                }
                cache[loc_set_addr][place_idx].valid = true;
                cache[loc_set_addr][place_idx].tag = tag;
                cache[loc_set_addr][place_idx].data = mem_blockAddress;
                cache[loc_set_addr][place_idx].time = 1;
                miss_count++;
            }
            else{
                // Hit
                hit_count++;
            }
        }



    private:
        int cache_size;
        int block_size;
        int set_degree;
        int num_block;
        int num_set;
        vector<vector<cache_block>> cache;
        

        bool check_valid(int loc_set_addr, int tag){
        
            for (int i = 0; i < set_degree; i++){
                if(cache[loc_set_addr][i].valid && cache[loc_set_addr][i].tag == tag){
                    dashboard_content += "Hit\n";
                    // cout << "Hit\n";
                    return true;
                }
            }
            dashboard_content += "Miss\n";
            // cout << "Miss\n";
            return false;
        }

        int get_idx_bit(){
            bitset<32> idx_bit(set_degree);
            for (int i = 31; i >= 0; i--){
                if(idx_bit[i] == 1){
                    return i ;
                }
            }
            return 0;
        }

};

#endif
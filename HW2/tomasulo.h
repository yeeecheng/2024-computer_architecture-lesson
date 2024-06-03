#ifndef TOMASULO_H
#define TOMASULO_H
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>
using namespace std;

#define NUM_ADDER 3
#define NUM_MUL 2
#define NUM_LOAD_BUFFER 2
#define NUM_STORE_BUFFER 2
#define CYCLE_L_D 2
#define CYCLE_S_D 1
#define CYCLE_ADD_D 2
#define CYCLE_SUB_D 2
#define CYCLE_MUL_D 10
#define CYCLE_DIV_D 40

class RegisterResultStatus{
    
    public:
        RegisterResultStatus(){
            qi.resize(16, 0);
        }

        int get_qi(int idx){
            if(idx > 32 || idx % 2 == 1){
                throw invalid_argument("idx error");
            }
            return qi[idx / 2];
        }

        bool set_qi(int idx, int func_unit){
            if(idx > 32 || idx % 2){
                throw invalid_argument("idx error");
            }
            qi[idx / 2] = func_unit;
            return true;
        }
        
        int func_unit_find_reg_f(int func_unit){
            for (int i = 0; i < 16; i++){
                if(qi[i] == func_unit){
                    return i * 2;
                } 
            }
            // no find
            return -1;
        }

    private:
        vector<int> qi;

};

class ReservationStation{
    friend class Tomasulo;

    public:
        ReservationStation(){
            busy_ = false;
            op_ = "";
            vj_ = 0;
            vk_ = 0;
            qj_ = 0;
            qk_ = 0;
            a_ = -1;
            count_cycle_ = -1;
            
            offset_ = -1;
            res_ = -1;
        }

    private:
        bool busy_;
        string op_;
        double vj_;
        double vk_;
        int qj_;
        int qk_;
        // load & store using
        int a_;
        int count_cycle_;
        
        int function_unit_;

        // for recording the cycle
        int offset_;

        // compute result in execute phase
        double res_;
};

class InstructionUnit{

    friend class Tomasulo;
    public:
        InstructionUnit(string instruction_unit, int offset){
            offset_ = offset;
            instruction_ = instruction_unit;
            string s;
            stringstream ss;
            ss << instruction_unit;
            int cnt = 0;
            while(getline(ss, s, ' ')){
                if(cnt == 0){
                    opcode_ = s;
                }
                else if(cnt == 1){
                    int l = s.find('F'), r = s.find(',');
                    rd_ = stoi(s.substr(l + 1, r - l -1));
                }
                else if(cnt == 2){
                    if(opcode_ == "S.D" || opcode_ == "L.D"){
                        int l = 0, r = s.find('(');
                        imm_ = stoi(s.substr(l, r - l));
                        rt_ = rd_;
                        l = s.find('('), r = s.find(')');
                        rs_ = stoi(s.substr(l + 2, r - l - 2));
                    }
                    else{
                        int l = s.find('F'), r = s.find(',');
                        rs_ = stoi(s.substr(l + 1, r - l - 1));
                    }
                }
                else{
                    if(!(opcode_ == "S.D" || opcode_ == "L.D")){
                        int l = s.find('F'), r = s.find(',');
                        rt_ = stoi(s.substr(l + 1, s.size() - l - 1));
                    }
                }
                cnt++;
            } 
        }
    private:
        string opcode_;
        int rd_;
        int rs_;
        int rt_;
        int imm_;

        string instruction_ = "";
        int issue_ = -1;
        int exec_comp_ = -1;
        int write_res_ = -1;
        
        // for recording the cycle
        int offset_;
};

class Tomasulo{

    public:
        Tomasulo(){
            // initialize
            Init_Reg();
            Init_Mem();
            Init_Reservation_Station();
        }

        void SetInstructionUnit(string instruction_unit){
            instruction_queue_.push_back(InstructionUnit(instruction_unit, instruction_queue_.size()));
        }
        
        void Start(){

            while(write_res_cnt < instruction_queue_.size()){
                cycle_++;
                cout << "Cycle " << cycle_ << "\n";

                Update_All_Reservation_Station();
                Show_Instruction_Status();
                Show_Reservation_Station();
                Show_Register_Result_Status();
                Show_FRegister();
                Show_IntRegister();
                Show_Memory();
            }
        }

        void Record_Exec_Result(){

        }

        void Show_Instruction_Status(){
            cout << left << std::setw(21) << "Instruction" << setw(6) << "Issue" << setw(6) << "Comp" << setw(6) << "Result" << "\n";
            for (int i = 0; i < instruction_queue_.size(); i++){
                cout << left << setw(21) << instruction_queue_[i].instruction_ << setw(6) 
                    << instruction_queue_[i].issue_ << setw(6) << instruction_queue_[i].exec_comp_ 
                    << setw(6) << instruction_queue_[i].write_res_ << "\n";
            }
            cout << setfill('-') << setw(40) << "" << "\n";
            cout << setfill(' ');
        }

        void Show_Reservation_Station(){
            cout << "Reservation Station\n";
            cout << setw(6) << "Time" << setw(10) << "Name" << setw(12) << "func_unit" << setw(6) << "Busy" << setw(8) 
                << "Op" << setw(4) << "Vj" << setw(4)<< "Vk" << setw(8) << "Qj" << setw(8) << "Qk";
            cout << "\n";
            map<int, string> idx_to_name = {
                {0, "Add"},
                {1, "Mul"},
                {2, "load"},
                {3, "Store"}
            };
            for (int i = 0; i < 4; i++){
                vector<ReservationStation> &reservation_ = *all_reservation_station_[i];
                for (int j = 0; j < reservation_.size(); j++){
                    ReservationStation &instruction_ = reservation_[j];
                    cout << setw(6) << instruction_.count_cycle_ << setw(10) << idx_to_name[i] + to_string(j + 1)
                        << setw(12) << instruction_.function_unit_ << setw(6) << instruction_.busy_ << setw(8) 
                        << instruction_.op_ << setw(4) << instruction_.vj_  << setw(4) << instruction_.vk_ 
                        << setw(8) << instruction_.qj_ << setw(8) << instruction_.qk_ << "\n";
                }
            }
            cout << setfill('-') << setw(40) << "" << "\n";
            cout << setfill(' ');
        }

        void Show_Load_Buffer(){

        }

        void Show_Store_Buffer(){

        }

        void Show_Register_Result_Status(){
            cout << "Register Result Status\n";
            for (int i = 0; i < 16; i++){
                cout << setw(6) << "F" + to_string(i * 2);
            }
            cout << "\n";
            for (int i = 0; i < 16; i++){
                cout << setw(6) << register_result_status_.get_qi(i * 2);
            }
            cout << "\n" << setfill('-') << setw(40) << "" << "\n";
            cout << setfill(' ');
        }

        void Show_IntRegister(){
            
            for (int i = 0; i < 16; i++){
                cout << setw(4) << "R" + to_string(i);
            }
            cout << "\n";
            for (int i = 0; i < 16; i++){
                cout << setw(4) << int_reg_[i];
            }
            cout << "\n";
            for (int i = 16; i < int_reg_.size(); i++){
                cout << setw(4) << "R" + to_string(i);
            }
            cout << "\n";
            for (int i = 16; i < int_reg_.size(); i++){
                cout << setw(4) << int_reg_[i];
            }
            cout << "\n" << setfill('-') << setw(40) << "" << "\n";
            cout << setfill(' ');
        }

        void Show_FRegister(){
            
            for (int i = 0; i < f_reg_.size(); i++){
                cout << setw(6) << "F" + to_string(i*2);
            }
            cout << "\n";
            for (int i = 0; i < f_reg_.size(); i++){
                cout << setw(6) << f_reg_[i];
            }
            cout << "\n" << setfill('-') << setw(40) << "" << "\n";
            cout << setfill(' ');
        }

        void Show_Memory(){
            for (int i = 0; i < mem_.size(); i++){
                cout << setw(4) << "M" + to_string(i);
            }
            cout << "\n";
            for (int i = 0; i < mem_.size(); i++){
                cout << setw(4) << mem_[i];
            }
            cout << "\n" << setfill('-') << setw(40) << "" << "\n";
            cout << setfill(' ');
        }


    private:
        vector<int> int_reg_;
        vector<double> f_reg_;
        vector<double> mem_;
        vector<ReservationStation> adder_;
        vector<ReservationStation> mul_;
        vector<ReservationStation> load_buffer_;
        vector<ReservationStation> store_buffer_;
        map<int, ReservationStation*> all_reservation_station_mapping_;
        vector<vector<ReservationStation>*> all_reservation_station_;
        

        RegisterResultStatus register_result_status_;
        vector<InstructionUnit> instruction_queue_;
        int instruction_queue_point_ = 0;
        int cycle_ = 0;
        // check whether all instructions are end.
        int write_res_cnt = 0;

        void Init_Reg(){
            int_reg_.resize(32, 0);
            int_reg_[1] = 16;
            f_reg_.resize(16, (double)1.0);
        }

        void Init_Mem(){
            mem_.resize(8, double(1));
        }

        void Init_Reservation_Station(){
            
            adder_.resize(NUM_ADDER);
            mul_.resize(NUM_MUL);
            load_buffer_.resize(NUM_LOAD_BUFFER);
            store_buffer_.resize(NUM_STORE_BUFFER);
            all_reservation_station_.push_back(&adder_);
            all_reservation_station_.push_back(&mul_);
            all_reservation_station_.push_back(&load_buffer_);
            all_reservation_station_.push_back(&store_buffer_);

            int function_unit_number = -1001;
            for (int i = 0; i < 4; i++){
                vector<ReservationStation> &reservation_ = *all_reservation_station_[i];
                for (int j = 0; j < reservation_.size(); j++){
                    reservation_[j].function_unit_ = function_unit_number;
                    all_reservation_station_mapping_[function_unit_number] = &reservation_[j];
                    function_unit_number -= 1;
                }
            }
        }

        void Update_All_Reservation_Station(){

            bool chk_write_res = false;
            vector<ReservationStation*> need_init_instruction_;
            /*
            if the qk of the store becomes 0 in the same cycle, 
            the store must wait until the next cycle to write the result. 
            Therefore, the store checks first to avoid writing results in the same cycle. 
            */
            for (int i = 3; i >= 0; i--){
                vector<ReservationStation> &reservation_ = *all_reservation_station_[i];
                for (int j = 0; j < reservation_.size(); j++){
                    ReservationStation &instruction_ = reservation_[j];
                    // count cycle is -1 representing not executing completely.
                    if(instruction_.busy_ && instruction_.count_cycle_ != -1){
                        // execution calculate
                        if(instruction_.count_cycle_ != 0){
                            Exec(instruction_);
                        }
                        // count cycle is 0
                        else {
                            // S.D has special condition
                            
                            if (instruction_.op_ == "S.D" && instruction_.qk_ != 0) {
                                continue;
                            }
                            Write_Res(instruction_);
                            chk_write_res = true;
                            need_init_instruction_.push_back(&instruction_);
                        }
                    }
                    
                }
            }
            Issue();
            if(chk_write_res){
                for (int i = 0; i < need_init_instruction_.size(); i++){
                    Init_ReservationStation(*need_init_instruction_[i]);
                }
            }
            Set_Count_Cycle();
        }

        void Write_Res(ReservationStation& instruction){
            
            write_res_cnt++;
            InstructionUnit & instruction_unit = instruction_queue_[instruction.offset_];
            // record cycle.
            instruction_unit.write_res_ = cycle_;

            int f_reg_idx = instruction_unit.rd_;
            // WAW hazard don't need to update value of register. 
            if(instruction.function_unit_ == register_result_status_.get_qi(f_reg_idx)){
                // update to empty function unit.
                register_result_status_.set_qi(f_reg_idx, 0);
                // save result to f_register.
                f_reg_[f_reg_idx / 2] = instruction.res_;
            }
            // check whether find the reservation station needing to write result. 

            if(instruction.op_ == "S.D" && instruction.qk_ == 0){
                mem_[instruction.a_ / 8] = instruction.vk_;
                return;
            }

            // check whether has a reservation with vk or vj which is equal to qi.
            for (int i = 0; i < 4; i++){
                vector<ReservationStation> &reservation_ = *all_reservation_station_[i];
                for (int j = 0; j < reservation_.size(); j++){
                    ReservationStation &instruction_ = reservation_[j];
                
                    // update value of vj, vk, qj, qk.
                    if(instruction_.qj_ == instruction.function_unit_){
                        instruction_.vj_ = instruction.res_;
                        instruction_.qj_ = 0;
                    }
                    if(instruction_.qk_ == instruction.function_unit_){
                        instruction_.vk_ = instruction.res_;
                        instruction_.qk_ = 0;
                    }
                }
            }
        }

        void Exec(ReservationStation& instruction){

            instruction.count_cycle_ -= 1;
            
            // step1 load
            if(instruction.op_ == "L.D"&& instruction.count_cycle_ == 1 && instruction.qj_ == 0){
                instruction.a_ = instruction.vj_ + instruction.a_;
            }

            // record cycle of of instruction execute completely
            if(instruction.count_cycle_ == 0){
                instruction_queue_[instruction.offset_].exec_comp_ = cycle_;
                // step1 store 
                if(instruction.op_ == "S.D" && instruction.qj_ == 0){
                    instruction.a_ = instruction.vj_ + instruction.a_;
                    return;
                }
                // get the result from FP, load
                double res;
                // FP compute res
                if(instruction.op_ == "ADD.D"){
                    res =(double) instruction.vj_ + (double) instruction.vk_;
                }
                else if(instruction.op_ == "SUB.D"){
                    res = (double) instruction.vj_ - (double) instruction.vk_;
                }
                else if(instruction.op_ == "MUL.D"){
                    res = (double) instruction.vj_ * (double) instruction.vk_;
                }
                else if(instruction.op_ == "DIV.D"){
                    res = (double) instruction.vj_ / (double) instruction.vk_;
                }
                // step2 load
                else if(instruction.op_ == "L.D"){
                    // because memory consists of 8 doubles which have 8 bytes.
                    res = (double) mem_[instruction.a_ / 8];
                }
                instruction.res_ = res;
            }
        }

        void Issue(){
            
            // check whether exist instruction
            if(instruction_queue_point_ >= instruction_queue_.size()){
                return;
            }
            // get instruction from instruction queue
            InstructionUnit& instruction_unit = instruction_queue_[instruction_queue_point_];
            
            int function_unit = Check_Structure_Hazard(instruction_unit.opcode_);            
            // structure hazard resulting stall.
            if(function_unit == -1) return;

            instruction_queue_point_++;
            instruction_unit.issue_ = cycle_;
        
            // get one useable reservation station.
            ReservationStation& instruction = *all_reservation_station_mapping_[function_unit];
            // instruction order
            instruction.offset_ = instruction_unit.offset_;
            // all instruction

            // set reservation name, which is ADD, SUB, DIV, or MUL.
            instruction.op_ = instruction_unit.opcode_;

            instruction.busy_ = true;
            // load only
            if(instruction_unit.opcode_ == "L.D"){
                instruction.a_ = instruction_unit.imm_;
                // set vj, qj value
                instruction.vj_ = int_reg_[instruction_unit.rs_];
                instruction.qj_ = 0;
                register_result_status_.set_qi(instruction_unit.rt_, function_unit);
            }
            // store only
            else if(instruction_unit.opcode_ == "S.D"){
                instruction.a_ = instruction_unit.imm_;
                // set vk, qk value
                int rt_reg_func_unit = register_result_status_.get_qi(instruction_unit.rt_);
                instruction.qk_ = rt_reg_func_unit;
                if(rt_reg_func_unit == 0){
                    instruction.vk_ = f_reg_[instruction_unit.rt_ / 2];
                    instruction.qk_ = 0;
                }
                // set vj, qj value
                instruction.vj_ = int_reg_[instruction_unit.rs_];
                instruction.qj_ = 0;
            }
            // FP operation
            else{
                int rs_reg_func_unit = register_result_status_.get_qi(instruction_unit.rs_);
                
                instruction.qj_ = rs_reg_func_unit;
                if(rs_reg_func_unit == 0){
                    instruction.vj_ = f_reg_[instruction_unit.rs_ / 2];
                    instruction.qj_ = 0;
                }

                // set vk, qk value
                int rt_reg_func_unit = register_result_status_.get_qi(instruction_unit.rt_);
                
                instruction.qk_ = rt_reg_func_unit;
                if(rt_reg_func_unit == 0){
                    instruction.vk_ = f_reg_[instruction_unit.rt_ / 2];
                    instruction.qk_ = 0;
                }
                register_result_status_.set_qi(instruction_unit.rd_, function_unit);
            }
        }

        int Check_Structure_Hazard(string& opcode){
            
            if(opcode == "ADD.D" || opcode == "SUB.D"){
                for(int i = 0; i < NUM_ADDER; i++){
                    if(adder_[i].busy_ != true){
                        return adder_[i].function_unit_;
                    }
                }
                // all adder be used 
                return -1;
            }
            else if(opcode == "MUL.D" || opcode == "DIV.D"){

                for(int i = 0; i < NUM_MUL; i++){
                    if(mul_[i].busy_ != true){
                        return mul_[i].function_unit_;
                    }
                }
                // all mul be used 
                return -1;
            }
            else if(opcode == "L.D"){
                for(int i = 0; i < NUM_LOAD_BUFFER; i++){
                    if(load_buffer_[i].busy_ != true){
                        return load_buffer_[i].function_unit_;
                    }
                }
                // all load buffer be used 
                return -1;
            }
            else if(opcode == "S.D"){
                for(int i = 0; i < NUM_STORE_BUFFER; i++){
                    if(store_buffer_[i].busy_ != true){
                        return store_buffer_[i].function_unit_;
                    }
                }
                // all store buffer be used 
                return -1;
            }

            return -1;
        }

        void Set_Count_Cycle(){

            map<string, int> op_to_cycle_number = {
                {"ADD.D", CYCLE_ADD_D},
                {"SUB.D", CYCLE_SUB_D},
                {"MUL.D", CYCLE_MUL_D},
                {"DIV.D", CYCLE_DIV_D},
                {"L.D", CYCLE_L_D},
                {"S.D", CYCLE_S_D}
            };
            
            for (int i = 0; i < 4; i++){
                vector<ReservationStation> &reservation_ = *all_reservation_station_[i];
                for (int j = 0; j < reservation_.size(); j++){
                    ReservationStation &instruction_ = reservation_[j];
                    // check whether is first time setting cycle time.

                    // load need to check qj.
                    if(instruction_.op_ == "L.D" ){
                        if(instruction_.busy_ && instruction_.qj_ == 0 && instruction_.count_cycle_ == -1){
                            instruction_.count_cycle_ = op_to_cycle_number[instruction_.op_];
                        }
                    }
                    // store need to check qk.
                    else if(instruction_.op_ == "S.D"){
                        if(instruction_.busy_ && instruction_.qj_ == 0 && instruction_.count_cycle_ == -1){
                            instruction_.count_cycle_ = op_to_cycle_number[instruction_.op_];
                        }
                    }
                    // FP
                    else{
                        if(instruction_.busy_ && instruction_.qj_ == 0 && instruction_.qk_ == 0 && instruction_.count_cycle_ == -1){
                            instruction_.count_cycle_ = op_to_cycle_number[instruction_.op_];
                        }
                    }
                }
            }
            
        }

        void Init_ReservationStation(ReservationStation& instruction){
            instruction.count_cycle_ = -1;
            instruction.busy_ = false;
            instruction.op_ = "";
            instruction.vj_ = 0;
            instruction.vk_ = 0;
            instruction.qj_ = 0;
            instruction.qk_ = 0;
            instruction.a_ = -1;
            instruction.offset_ = -1;
            instruction.res_ = -1;
        }
};

#endif 

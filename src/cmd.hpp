#ifndef CMD_HPP
#define	CMD_HPP
#include "api_calls.hpp"
#include "device.hpp"
#include <map>
#include <vector>
#include <string.h>
#include <iostream>
#include <string>

typedef int (*helper)(std::vector<string>);
typedef std::map<std::string,helper> call_map;

#define INVALID_CALL -1
#define ERR_CALL -2

#define HAS_FLAG(x) if ((connected_device[dev_index].motor[mot_index].status_status_bits & x) == x) 

#define GET_NUM(x) try {                                         \
                x = std::stoi(args.at(i+1), 0, 10);         \
            }                                                   \
            catch(const std::exception& e) {                    \
                printf("Given argument is not a valid number\n");\
                return INVALID_CALL;                            \
            }


#define CHANNEL_OPERATION(operation) if (args.size() <= i+1 ) {             \
                printf("Channel\\bay number not specified\n");              \
                return INVALID_CALL;                                        \
            }                                                               \
            GET_NUM(index)                                                  \
            if (opened_device.bays == -1){                                  \
                if (operation(0x50,index) == INVALID_CHANNEL ){             \
                    printf("Not existing channel number given\n");          \
                    return ERR_CALL;                                        \
                }                                                           \
            }                                                               \
            else {                                                          \
                index += 0x20;                                              \
                if (operation(index) == INVALID_DEST ){                     \
                    printf("Not existing channel number given\n");          \
                    return ERR_CALL;                                        \
                }                                                           \
            }                                                               \
            i++;

#define GET_MESSAGE(mess_type, call)   \
        mess_type* mess = (mess_type*) malloc(sizeof(mess_type));\
        if (opened_device.bays == -1){  \
            if (call(mess, 0x50, index) == INVALID_CHANNEL){  \
                printf("Not existing channel number given\n");  \
                free(mess);         \
                return ERR_CALL;    \
            }                       \
        }                           \
        else {                      \
            index += 0x20;          \
            if (call(mess, index) == INVALID_DEST){   \
                printf("Wrong address given\n");                            \
                free(mess);                                                 \
                return ERR_CALL;                                            \
            }                                                               \
        }                                                                   


#define NULL_ARGS if (args.size() == 1) {       \
        printf("No arguments\n");               \
        return INVALID_CALL;                    \
    }


#define SET_FLAG if (args.at(i).compare("-s") == 0){    \
            if (operation != -1) {  \
                printf("Operation has to be specified exactly once\n"); \
                return INVALID_CALL;    \
            }   \
            operation = 1;  \
            if (args.size() <= i+1){    \
                printf("Not enough paramaters\n");  \
                return INVALID_CALL;    \
            }   \
            GET_NUM(index)  \
        }

#define GET_FLAG if (args.at(i).compare("-g") == 0){    \
            if (operation != -1) {  \
                printf("Operation has to be specified exactly once");   \
                return INVALID_CALL;    \
            }   \
            operation = 0;  \
            if (args.size() <= i+1){    \
                printf("Not enough parameters\n");  \
                return INVALID_CALL;    \
            }   \
            GET_NUM(index)  \
        }

#define FLAG(lookup_str, int_val, bool_val, err_string) \
            if (args.at(i).compare(lookup_str) == 0){   \
            if (args.size() <= i+1){                    \
                printf("Not enough parameters\n");      \
                return INVALID_CALL;                    \
            }                                           \
            if (bool_val){                              \
                printf(err_string);                     \
                return INVALID_CALL;                    \
            }                                           \
            GET_NUM(int_val)                            \
            acc_spec = true;                            \
        }

bool validation = false;

int HelpC(std::vector<string> args){
    if (args.size() > 1) printf("No arguments needed\n");
    printf(" help        prints this help message, for command info use -h\n");
    printf(" open        switch between controlled devices \n");
    printf(" devinfo     prints connected device information \n");
    printf(" flash       flashes front panel LED \n");
    printf(" chan        channel state \n");
    printf(" poscount    device's position counter \n");
    printf(" enccount    device's encoder counter \n");
    printf(" velp        acceleration and maximum velocity \n");
    printf(" jogp        jog move parameters \n");
    printf(" powerp      power used while motor moves or rests \n");
    printf(" bdist       backlash distance value \n");
    printf(" relmovep    relative move parameters \n");
    printf(" absmovep    absolute move parameters \n");
    printf(" limitsw     limit switch parameters \n");
    printf(" homingvel   homing velocity \n");
    printf(" home        move to home position \n");
    printf(" relmove     start relative move \n");
    printf(" absmove     start absolute move \n");
    printf(" jogmove     start jog move \n");
    printf(" velmove     start move with set velocity \n");
    printf(" stop        stop movement \n");
    printf(" ledp        front LED parameters \n");
    printf(" buttp       device's buttons parameters \n");
    printf(" status      get status \n");
    printf(" statusdc    get status for dc servo controller \n");
    printf(" eom         trigger parameters \n");
    return 0;
}

int OpenDeviceC(std::vector<string> args){
    NULL_ARGS
    if (args.size() > 3 ) {
        printf("Unexpected number of arguments, see -h for help\n");
        return INVALID_CALL;
    } 
    if (args.at(1) == "-h"){ 
        printf("Choose which connected device to control\n");
        printf("-n NUMBER       device number in list created by program, see devinfo\n");
        printf("-s SN           serial number of device\n");
        return 0;
    }
    if ( args.at(1).compare("-n") == 0 ) {
        if (args.size() == 2 ) {
            printf("Number not given\n");
            return INVALID_CALL;
        }
        unsigned int num = -1;
        try {
            num = std::stoi(args.at(2));
        }
        catch(const std::exception& e) { 
            printf("Given argument is not a valid number\n");
            return INVALID_CALL;
        }
        if (!validation) if (OpenDevice(num-1) != 0 ){ //for user devices are numbered from 1
            printf("Incorrect device number\n");
            return ERR_CALL;
        }       
        return 0;
    }
    if ( args.at(1).compare("-s") == 0) {
        if (args.size() == 2 ) {
            printf("Serial not specified\n");
            return INVALID_CALL;
        }
        for ( unsigned int i = 0; i< devices_connected; i++ ){
            std::string to_comp (connected_device[i].SN);
            if ( to_comp.compare(args.at(2)) == 0 ) OpenDevice(i); 
        }
        if (!validation){ 
            printf("Device with specified serial number not present\n");
            return ERR_CALL;
        }
        return 0;
    }
    printf("Unrecognized parameter %s, see -h for help\n", args.at(1).c_str());
    return 0;
}

void DeviceInfoHelper(int dev_index, int mot_index){
    if (connected_device[dev_index].motor[mot_index].enc_count != 0) printf("  Encoder count : %d\n",connected_device[dev_index].motor[mot_index].enc_count);
        if (connected_device[dev_index].motor[mot_index].homing) printf("  Moving to home position\n");
        if (connected_device[dev_index].motor[mot_index].moving) printf("  Moving\n");
        if (connected_device[dev_index].motor[mot_index].stopping) printf("  Stopping\n");
        if (connected_device[dev_index].motor[mot_index].status_updates){
            if (connected_device[dev_index].motor[mot_index].enc_count != 0) printf("  Status message - encoder count : %d\n",connected_device[dev_index].motor[mot_index].status_enc_count);
            else printf("  Status message - position : %d\n",connected_device[dev_index].motor[mot_index].status_position);
            printf("  Status message - velocity : %d\n",connected_device[dev_index].motor[mot_index].status_velocity);
            printf("  Status bits info: \n");
            HAS_FLAG(0x00000001) printf("   Forward hardware limit switch active\n");
            HAS_FLAG(0x00000002) printf("   Reverse hardware limit switch active\n");
            HAS_FLAG(0x00000010) printf("   Moving forward\n");
            HAS_FLAG(0x00000020) printf("   Moving reverse\n");
            HAS_FLAG(0x00000040) printf("   Jogging forward\n");
            HAS_FLAG(0x00000080) printf("   Jogging reverse\n");
            HAS_FLAG(0x00000200) printf("   Homing\n");
            HAS_FLAG(0x00000400) printf("   Homed\n");
            HAS_FLAG(0x00001000) printf("   Tracking\n");
            HAS_FLAG(0x00002000) printf("   Settled\n");
            HAS_FLAG(0x00004000) printf("   Motion error\n");
            HAS_FLAG(0x01000000) printf("   Motor limit reached\n");
            HAS_FLAG(0x80000000) printf("   Channel enabled\n");
        }
}

int DeviceInfoC(std::vector<string> args){
    if (args.size() > 1 && args.at(1) == "-h"){ 
        printf("Prints information to all compatible devices connected\n");
        return 0;
    }
    if (args.size() > 1) printf("No arguments except -h\n");
    int prev_opened = opened_device_index;
    for (unsigned int i = 0; i< devices_connected; i++){
        OpenDevice(i);
        printf(" Device number %d\n", i+1);
        printf(" Serial: %s\n",connected_device[i].SN);
        if (connected_device[i].bays != -1 ){ 
            printf(" Bays: %d\n", connected_device[i].bays);
            for (int j = 0; j < connected_device[i].bays; j++ ){ 
                printf(" Motor in bay %d: ",j+1);                           //bays numbered from 1 for user
                if (connected_device[i].bay_used[j]) printf("Unused\n");
                else{
                    printf("Used\n");
                    printf("  Destination address : %d\n",connected_device[i].motor[j].dest);
                    DeviceInfoHelper(i,j);
                }
            }
        }
        if (connected_device[i].channels != -1 ){ 
            printf(" Channels: %d\n", connected_device[i].channels);
            printf(" Channel 1\n");
            DeviceInfoHelper(i,0);
            if (connected_device[i].channels == 2 ){
                printf(" Channel 2\n");
                DeviceInfoHelper(i,1);
            }
        }
        HwInfo *info = (HwInfo*) malloc(sizeof(HwInfo));
        if (device_calls::GetHwInfo(info) != 0) printf("Error occured while receiving info from device\n");
        printf("  Model number: %s\n",info->ModelNumber().c_str());
        printf("  Hardware version: %d\n", info->HwVersion());
        printf("  Notes: %s", info->Notes().c_str());
        free(info);
    }
    
    OpenDevice(prev_opened);
    return 0;
}

int IdentC(std::vector<string> args){
    if (args.size() == 1 ) {
        if (!validation) device_calls::Identify();
        return 0;
    }
    uint8_t dest;

    if (args.at(1) == "-h"){
        printf("Flashes front LED on device, optional destination parameter\n");
        printf("-d VALUE    optional parameter - destination address in decimal\n");
        printf("-D VALUE    optional parameter - destination address in hexadecimal\n");
        return 0;
    }
    if (args.at(1).compare("-d") == 0 ){
        if (args.size() == 2 ) {
            printf("Address not given\n");
            return INVALID_CALL;
        }
        try {
            dest = std::stoi(args.at(2), 0, 10);
        }
        catch(const std::exception& e) { 
            printf("Given argument is not a valid number\n");
            return INVALID_CALL;
        }
    }
    else if (args.at(1).compare("-D") == 0 ){
        if (args.size() == 2 ) {
            printf("Address not given\n");
            return INVALID_CALL;
        }
        try {
            dest = std::stoi(args.at(2), 0, 16);
        }
        catch(const std::exception& e) { 
            printf("Given argument is not a valid number\n");
            return INVALID_CALL;
        }
    }
    else printf("Unrecognized parameter %s, see -h for help\n", args.at(1).c_str());
    if (!validation) if (device_calls::Identify(dest) == INVALID_DEST){ 
        printf("Invalid destination given\n");
        return ERR_CALL;
    }
    return 0;
}

int ChannelAbleC(std::vector<string> args){
    NULL_ARGS
    for (unsigned int i = 1; i< args.size(); i++){
        if (args.at(i).compare("-e") == 0 || args.at(i).compare("-d") == 0 || args.at(i).compare("-i") == 0 ) {i++; continue; }
        if (args.at(i).compare("-h") != 0 ){
                printf("Unknown parameter %s\n",args.at(i).c_str());
                return INVALID_CALL;
            }
    }
    for (unsigned int i = 1; i< args.size(); i++){
        uint8_t index;
        if (args.at(i).compare("-h") == 0){
            printf("Operating channels or bays, numbers range from 1 \n");
            printf("-e NUMBER    enable channel or bay with given number\n");
            printf("-d NUMBER    disable channel or bay with given number\n");
            printf("-i NUMBER    information about ability state for channel or bay at given position\n");
        }
        if (args.at(i).compare("-e") == 0){ CHANNEL_OPERATION(device_calls::EnableChannel) }
        if (args.at(i).compare("-d") == 0){ CHANNEL_OPERATION(device_calls::DisableChannel) }
        if (args.at(i).compare("-i") == 0){
            if (args.size() <= i+1 ) {             
                printf("Channel\\bay number not specified\n");              
                return INVALID_CALL;                                        
            }
            GET_NUM(index)
            GetChannelState *state = (GetChannelState*) malloc(sizeof(GetChannelState));
            if (opened_device.bays == -1){     
                if (device_calls::ChannelState(state,0x50,index) == INVALID_CHANNEL){
                    printf("Not existing channel number given\n");
                    free(state);
                    return ERR_CALL;                                               
                }
                if (state->GetSecondParam() == 0x01 ) printf("Enabled\n");
                else printf("Disabled\n");
            }                                                               
            else {                                                          
                index += 0x20;                                                
                if (device_calls::ChannelState(state,index) == INVALID_DEST){
                    printf("Wrong address given\n");
                    free(state);
                    return ERR_CALL;  
                }
                if(state->GetSecondParam() == 0x01 ) printf("Enabled\n");
                else printf("Disabled\n");
            }
            free(state);
            i++; 
        }
    }
    return 0;
}

int PosCounterC(std::vector<string> args){
    NULL_ARGS
    for (unsigned int i = 1; i< args.size(); i++){
        if (args.at(i).compare("-g") == 0) {i++; continue; }
        if (args.at(i).compare("-s") == 0) {i += 2; continue; }
        if (args.at(i).compare("-h") != 0 ){
                printf("Unknown parameter %s\n",args.at(i).c_str());
                return INVALID_CALL;
            }
    }
    for (unsigned int i = 1; i< args.size(); i++){
        if (args.at(i).compare("-h") == 0){
            printf("WARNING: Setting position counter isn't standard operation and may result in unexpected behavior\n");
            printf("Set or get actual position counter in device\n");
            printf("-s NUMBER VALUE      set position counter for given motor channel to specified VALUE\n");
            printf("-g NUMBER            get position counter for given motor channel\n");
        }
        if (args.at(i).compare("-s") == 0){
            if (args.size() <= i+2){ 
                printf("Not enough parameters\n");
                return INVALID_CALL;
            }
            uint8_t index;
            int32_t value;
            try {
                index = std::stoi(args.at(i+1), 0, 10);
                value = std::stoi(args.at(i+2), 0, 10);
            }
            catch(const std::exception& e) { 
                printf("Given argument is not a valid number\n");
                return INVALID_CALL;
            } 
            if (opened_device.bays == -1){
                if (device_calls::SetPositionCounter(value, 0x50, index) == INVALID_CHANNEL){
                    printf("Not existing channel number given\n");
                    return ERR_CALL;
                }
            }
            else {
                index += 0x20;
                if (device_calls::SetPositionCounter(value, index) == INVALID_DEST){
                    printf("Wrong address given\n");
                    return ERR_CALL;
                }
            }
            i+=2;
        }
        
        if (args.at(i).compare("-g") == 0){
            if (args.size() <= i+i){ 
                printf("Not enough paramaters\n");
                return INVALID_CALL;
            }
            uint8_t index;
            GET_NUM(index)
            GetPosCounter* counter = (GetPosCounter*) malloc(sizeof(GetPosCounter));
            if (opened_device.bays == -1){
                if (device_calls::GetPositionCounter(counter, 0x50, index) == INVALID_CHANNEL){
                    printf("Not existing channel number given\n");
                    free(counter);
                    return ERR_CALL;
                }
            }
            else {
                index += 0x20;
                if (device_calls::GetPositionCounter(counter, index) == INVALID_DEST){
                    printf("Wrong address given\n");
                    free(counter);
                    return ERR_CALL;
                }
            }
            printf("Position counter: %d\n", counter->GetPosition());
            i++;
        }
    }
    return 0;
}

int EncCountC(std::vector<string> args){
    NULL_ARGS
    for (unsigned int i = 1; i< args.size(); i++){
        if (args.at(i).compare("-g") == 0) {i++; continue; }
        if (args.at(i).compare("-s") == 0) {i += 2; continue; }
        if (args.at(i).compare("-h") != 0 ){
                printf("Unknown parameter %s\n",args.at(i).c_str());
                return INVALID_CALL;
            }
    }
    for (unsigned int i = 1; i< args.size(); i++){
        if (args.at(i).compare("-h") == 0){
            printf("WARNING: Setting encoder counter isn't standard operation and may result in unexpected behavior\n");
            printf("Set or get actual encoder counter in device\n");
            printf("-s NUMBER VALUE      set encoder counter for given motor channel to specified VALUE\n");
            printf("-g NUMBER            get encoder counter for given motor channel\n");
        }
        if (args.at(i).compare("-s") == 0){
            if (args.size() <= i+2){ 
                printf("Not enough paramaters\n");
                return INVALID_CALL;
            }
            uint8_t index;
            int32_t value;
            try {
                index = std::stoi(args.at(i+1), 0, 10);
                value = std::stoi(args.at(i+2), 0, 10);
            }
            catch(const std::exception& e) { 
                printf("Given argument is not a valid number\n");
                return INVALID_CALL;
            } 
            if (opened_device.bays == -1){
                if (device_calls::SetEncoderCounter(value, 0x50, index) == INVALID_CHANNEL){
                    printf("Not existing channel number given\n");
                    return ERR_CALL;
                }
            }
            else {
                index += 0x20;
                if (device_calls::SetEncoderCounter(value, index) == INVALID_DEST){
                    printf("Wrong address given\n");
                    return ERR_CALL;
                }
            }
            i+=2;
        }
        
        if (args.at(i).compare("-g") == 0){
            if (args.size() <= i+i){ 
                printf("Not enough parameters\n");
                return INVALID_CALL;
            }
            uint8_t index;
            GET_NUM(index)
            GetEncCount* counter = (GetEncCount*) malloc(sizeof(GetEncCount));
            if (opened_device.bays == -1){
                if (device_calls::GetEncoderCounter(counter, 0x50, index) == INVALID_CHANNEL){
                    printf("Not existing channel number given\n");
                    free(counter);
                    return ERR_CALL;
                }
            }
            else {
                index += 0x20;
                if (device_calls::GetEncoderCounter(counter, index) == INVALID_DEST){
                    printf("Wrong address given\n");
                    free(counter);
                    return ERR_CALL;
                }
            }
            printf("Encoder counter: %d\n", counter->GetEncCounter());
            i++;
        }
    }
    return 0;
}

int VelParamC(std::vector<string> args){
    NULL_ARGS
    for (unsigned int i = 1; i< args.size(); i++){
        if (args.at(i).compare("-g") == 0 || args.at(i).compare("-s") == 0 || args.at(i).compare("-m") == 0 || args.at(i).compare("-a") == 0) {i++; continue; }
        if (args.at(i).compare("-h") != 0 ){
                printf("Unknown parameter %s\n",args.at(i).c_str());
                return INVALID_CALL;
            }
    }
    int operation = -1; // -1 unspecified, 0 get, 1 set
    uint8_t index;
    int32_t acc;
    int32_t maxvel;
    bool acc_spec = false;
    bool maxvel_spec = false;
    for (unsigned int i = 1; i< args.size(); i++){
        if (args.at(i).compare("-h") == 0){
            printf("Set or get velocity parameters for specified motor channel. Parameters are acceleration and maximum velocity.\n");
            printf("Velocity is specified in encoder counts per second or microsteps per second, depending on controller. Acceleration is specified in counts/sec/sec or microsteps/sec/sec. \n");
            printf("-s NUMBER       set operation for given motor channel, setting both parameters is mandatory\n");
            printf("-g NUMBER       get parameters for given motor channel\n");
            printf("-m VALUE        set maximum velocity\n");
            printf("-a VALUE        acceleration\n");
        }
        
        SET_FLAG   
        GET_FLAG
        FLAG("-m", maxvel, maxvel_spec, "Maximum velocity already specified\n")
        FLAG("-a", acc, acc_spec, "Acceleration already specified\n")
        
    }
    
    if (operation == -1) {
        printf("Operation not specified\n");
        return INVALID_CALL;
    }
    if (operation == 0){
        GET_MESSAGE(GetVelocityParams, device_calls::GetVelocityP)
        printf("Acceleration: %d\n",mess->GetAcceleration());
        printf("Maximum velocity: %d\n",mess->GetMaxVel());
    }
    if (operation == 1){
        int ret;
        if(!acc_spec || !maxvel_spec) {
            printf("Not all mandatory parameters specified\n");
            return INVALID_CALL;
        }
        if (opened_device.bays == -1){
            ret = device_calls::SetVelocityP(acc, maxvel, 0x50, index);
            switch (ret){
                case INVALID_CHANNEL: {printf("Not existing channel given\n"); return ERR_CALL;}
                case INVALID_PARAM_1: {printf("Velocity given exceeds maximal velocity\n"); return ERR_CALL;}
                case INVALID_PARAM_2: {printf("Acceleration given exceed maximal acceleration\n"); return ERR_CALL;}
            };
        }
        else {
            index += 0x20;
            ret = device_calls::SetVelocityP(acc, maxvel, index);
            switch (ret){
                case INVALID_CHANNEL: {printf("Not existing channel given\n"); return ERR_CALL;}
                case INVALID_PARAM_1: {printf("Velocity given exceeds maximal velocity\n"); return ERR_CALL;}
                case INVALID_PARAM_2: {printf("Acceleration given exceed maximal acceleration\n"); return ERR_CALL;}
            };
        }
    }
    return 0;
}

int JogParamC(std::vector<string> args){
    NULL_ARGS
    for (unsigned int i = 1; i< args.size(); i++){
        if (args.at(i).compare("-g") == 0 || args.at(i).compare("-s") == 0 || args.at(i).compare("-m") == 0 || args.at(i).compare("-a") == 0) {i++; continue; }
        if (args.at(i).compare("-h") != 0 ){
                printf("Unknown parameter %s\n",args.at(i).c_str());
                return INVALID_CALL;
            }
    }
    int operation = -1; // -1 unspecified, 0 get, 1 set
    uint8_t index;
    int32_t maxvel, acc, step_size;
    uint16_t mode, stop_mode;
    bool acc_spec = false, maxvel_spec = false, step_size_spec = false, mode_spec = false, stop_mode_spec = false;
    for (unsigned int i = 1; i< args.size(); i++){
        if (args.at(i).compare("-h") == 0){
            printf("Set or get jog parameters\n");
            printf("-s NUMBER       set operation for given motor channel, setting all parameters is mandatory\n");
            printf("-g NUMBER       get parameters for given motor channel\n");
            printf("-m VALUE        mode: 1 for continuous jogging, 2 for single step\n");
            printf("-v VALUE        maximal velocity\n");
            printf("-a VALUE        acceleration\n");
            printf("-z VALUE        step size\n");
            printf("-s VALUE        stop mode: 1 for immediate stop, 2 for profiled\n");
        }
        
        SET_FLAG
        GET_FLAG
        FLAG("-m", mode, mode_spec, "Mode already specified\n")
        FLAG("-v", maxvel, maxvel_spec, "Maximal velocity already specified\n")
        FLAG("-a", acc, acc_spec, "Acceleration already specified\n")
        FLAG("-z", step_size, step_size_spec, "Step size already specified\n")
        FLAG("-s", stop_mode, stop_mode_spec, "Stop mode already specified\n")
    }
    
    if (operation == -1) {
        printf("Operation not specified\n");
        return INVALID_CALL;
    }
    if (operation == 0){
        GET_MESSAGE(GetJogParams, device_calls::GetJogP)
        printf("Acceleration: %d\n",mess->GetAcceleration());
        printf("Maximum velocity: %d\n",mess->GetMaxVel());
        printf("Mode: %d\n",mess->GetJogMode());
        printf("Stop mode: %d\n",mess->GetStopMode());
        printf("Step size: %d\n",mess->GetStepSize());
    }
    if (operation == 1){
        int ret;
        if(!acc_spec || !maxvel_spec || !step_size_spec || !mode_spec || !stop_mode_spec) {
            printf("Not all mandatory parameters specified\n");
            return INVALID_CALL;
        }
        if (opened_device.bays == -1){
            ret = device_calls::SetJogP(mode, step_size, maxvel, acc, stop_mode, 0x50, index); 
            switch (ret){
                case INVALID_CHANNEL: {printf("Not existing channel given\n"); return ERR_CALL;}
                case INVALID_PARAM_1: {printf("Invalid mode given\n"); return ERR_CALL;}
                case INVALID_PARAM_3: {printf("Velocity given exceeds maximal velocity\n"); return ERR_CALL;}
                case INVALID_PARAM_4: {printf("Acceleration given exceed maximal acceleration\n"); return ERR_CALL;}
                case INVALID_PARAM_5: {printf("Invalid stop mode given\n"); return ERR_CALL;}
            };
        }
        else {
            index += 0x20;
            ret = device_calls::SetJogP(mode, step_size, maxvel, acc, stop_mode, index); 
            switch (ret){
                case INVALID_DEST:  {printf("Wrong address given\n"); return ERR_CALL;}
                case INVALID_PARAM_1: {printf("Invalid mode given\n"); return ERR_CALL;}
                case INVALID_PARAM_3: {printf("Velocity given exceeds maximal velocity\n"); return ERR_CALL;}
                case INVALID_PARAM_4: {printf("Acceleration given exceed maximal acceleration\n"); return ERR_CALL;}
                case INVALID_PARAM_5: {printf("Invalid stop mode given\n"); return ERR_CALL;}
            };
        }
    }
    return 0;
}

int PowerParamC(std::vector<string> args){
    NULL_ARGS
    for (unsigned int i = 1; i< args.size(); i++){
        if (args.at(i).compare("-g") == 0 || args.at(i).compare("-s") == 0 || args.at(i).compare("-r") == 0  || args.at(i).compare("-m") == 0 ) {i++; continue; }
        if (args.at(i).compare("-h") != 0 ){
                printf("Unknown parameter %s\n",args.at(i).c_str());
                return INVALID_CALL;
            }
    }
    int operation = -1; // -1 unspecified, 0 get, 1 set
    uint8_t index;
    uint16_t mfact, rfact;
    bool mfact_spec = false, rfact_spec = false;
    for (unsigned int i = 1; i< args.size(); i++){
        if (args.at(i).compare("-h") == 0){
            printf("Set or get power used when motor is moving and resting\n");
            printf("-s NUMBER       set power parameters\n");
            printf("-g NUMBER       get power parameters\n");
            printf("-m VALUE        move factor in %\n");
            printf("-r VALUE        rest factor in %\n");
        }
        
        SET_FLAG
        GET_FLAG
        FLAG("-m", mfact, mfact_spec, "Move power factor already specified\n")
        FLAG("-r", rfact, rfact_spec, "Rest power factor already specified\n")
    }
    if (operation == -1) {
        printf("Operation not specified\n");
        return INVALID_CALL;
    }
    if (operation == 0){
        GET_MESSAGE(GetPowerParams, device_calls::GetPowerUsed)
        printf("Rest factor: %d\n",mess->GetRestFactor());
        printf("Move factor: %d\n",mess->GetMoveFactor());
    }
    if (operation == 1){
        int ret;
        if(!mfact_spec || !rfact ) {
            printf("Not all mandatory parameters specified\n");
            return INVALID_CALL;
        }
        if (opened_device.bays == -1){
            ret = device_calls::SetPowerUsed(rfact, mfact, 0x50, index); 
            switch (ret){
                case INVALID_CHANNEL: {printf("Not existing channel given\n"); return ERR_CALL;}
                case INVALID_PARAM_1: {printf("Invalid rest factor given\n"); return ERR_CALL;}
                case INVALID_PARAM_2: {printf("Invalid move factor given\n"); return ERR_CALL;}
            };
        }
        else {
            index += 0x20;
            ret = device_calls::SetPowerUsed(rfact, mfact, index); 
            switch (ret){
                case INVALID_DEST:  {printf("Wrong address given\n"); return ERR_CALL;}
                case INVALID_PARAM_1: {printf("Invalid rest factor given\n"); return ERR_CALL;}
                case INVALID_PARAM_2: {printf("Invalid move factor given\n"); return ERR_CALL;}
            };
        }
    }
    return 0;
}

int BacklashDistC(std::vector<string> args){
    // set, get
    //not implemented
    return 0;
}

int RelMoveParamC(std::vector<string> args){
    // set, get
    //not implemented
    return 0;
}

int AbsMoveParamC(std::vector<string> args){
    // set, get
    //not implemented
    return 0;
}

int HomingVelC(std::vector<string> args){
    // set, get
    //not implemented
    return 0;
}

int LimitSwitchC(std::vector<string> args){
    // set, get
    //not implemented
    return 0;
}

int HomeC(std::vector<string> args){
    //not implemented
    return 0;
}

int StartRelMoveC(std::vector<string> args){
    //not implemented
    return 0;
}

int StartAbsMoveC(std::vector<string> args){
    //not implemented
    return 0;
}

int StartJogMoveC(std::vector<string> args){
    //not implemented
    return 0;
}

int StartVelMoveC(std::vector<string> args){
    //not implemented
    return 0;
}

int StopC(std::vector<string> args){
    //not implemented
    return 0;
}

int AccParamC(std::vector<string> args){
    // get, set
    //not implemented
    return 0;
}

int LedParamC(std::vector<string> args){
    // get, set
    //not implemented
    return 0;
}

int ButtonsParamC(std::vector<string> args){
    // get, set
    //not implemented
    return 0;
}

int StatusC(std::vector<string> args){
    //GetStatBits, GetStatus
    //not implemented
    return 0;
}

int DCStatusC(std::vector<string> args){
    //not implemented
    return 0;
}

int TriggerParamC(std::vector<string> args){
    // get, set
    //not implemented
    return 0;
}

call_map calls = {
    std::make_pair("help", &HelpC),
    std::make_pair("flash", &IdentC),
    std::make_pair("chan", &ChannelAbleC),
    std::make_pair("open", &OpenDeviceC),
    std::make_pair("devinfo", &DeviceInfoC),
    std::make_pair("poscount", &PosCounterC),
    std::make_pair("enccount", &EncCountC),
    std::make_pair("velp", &VelParamC),
    std::make_pair("jogp", &JogParamC),
    std::make_pair("powerp", &PowerParamC),
    std::make_pair("bdist", &BacklashDistC),
    std::make_pair("relmovep", &RelMoveParamC),
    std::make_pair("absmovep", &AbsMoveParamC),
    std::make_pair("homingvel", &HomingVelC),
    std::make_pair("limitsw", &LimitSwitchC),
    std::make_pair("home", &HomeC),
    std::make_pair("relmove", &StartRelMoveC),
    std::make_pair("absmove", &StartAbsMoveC),
    std::make_pair("jogmove", &StartJogMoveC),
    std::make_pair("velmove", &StartVelMoveC),
    std::make_pair("stop", &StopC),
    std::make_pair("accp", &AccParamC),
    std::make_pair("ledp", &LedParamC),
    std::make_pair("buttp", &ButtonsParamC),
    std::make_pair("status", &StatusC),
    std::make_pair("statusdc", &DCStatusC),
    std::make_pair("triggp", &TriggerParamC)
};


int run_cmd(){
      
    while(true){
        std::string line = "";
        std::getline(std::cin, line);
        if (line == "" ) continue;
        
        std::vector<std::string> args;
        const char delimiter = ' ';
        char* token = strtok(strdup(line.c_str()), &delimiter);
        while(token != NULL){
            args.push_back(std::string(token));
            token = strtok(NULL, &delimiter);
        }
        
        if ( args.at(0).compare("exit") == 0 ) break;
        if ( calls.count(args.at(0))== 0 ) {
            printf("Unrecognized command %s\n", args.at(0).c_str() );
            continue;
        }
        
        calls.at(args.at(0))(args);
        device_calls::SendServerAlive(0x50);
        }
      
    return 0;
}

#endif	/* CMD_HPP */


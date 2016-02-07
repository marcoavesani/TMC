#include <endian.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "restrictions.hpp"

#define ERROR_FOUND -2
#define BAD_PARAMS -4
#define ERROR_RETURNED -3

typedef struct {
    int32_t SN;
    uint8_t device_type;
    uint8_t HWtype;
    uint8_t dest = 0x50;
    uint8_t source = 0x01;
    uint8_t chanID = 0x01;
    uint16_t chan = 0x0001;
} defaults;

defaults gdefaults;

uint8_t DefaultDest(){
    return gdefaults.dest;
}

uint8_t DefaultSource(){
    return gdefaults.source;
}

uint8_t DefaultChanel(){
    return gdefaults.chanID;
}

int SendMessage(Message message, FT_HANDLE &handle){
    FT_STATUS wrStatus;
    unsigned int wrote;
    wrStatus = FT_Write(handle, message.data(), message.msize(), &wrote );
    if (wrStatus == FT_OK && wrote == message.msize()){
        return 0;
    }
    else {
        printf("Sending message failed, error code : %d \n", wrStatus );
        printf("wrote : %d should write: %d \n", wrote, message.msize());
    }
    return -1;
}

int CheckMessages(){
    //not implemented
    //check incoming queue for error messages, return fail on error, 
    // other messages handled
    return 0;
}


int Identify( FT_HANDLE &handle, uint8_t dest = DefaultDest(), uint8_t source = DefaultSource() ){
    if (CheckMessages() != 0) return ERROR_FOUND;
    IdentifyMs mes(dest, source);
    SendMessage(mes, handle); 
    if (CheckMessages() != 0) return ERROR_RETURNED;
    return 0;
}

int EnableChannel(FT_HANDLE &handle, uint8_t chanel = DefaultChanel(), uint8_t dest = DefaultDest(), uint8_t source = DefaultSource()){
    parameters to_check;
    to_check.dest = &dest;
    to_check.source = &source;
    if (CheckRestrictions(to_check) != 0 ) return BAD_PARAMS;
    
    if (CheckMessages() != 0) return ERROR_FOUND;
    SetChannelState mes(chanel, 1, dest, source);
    SendMessage(mes, handle);
    if (CheckMessages() != 0) return ERROR_RETURNED;
    return 0;
}

int DisableChannel(FT_HANDLE &handle,uint8_t chanel = DefaultChanel(), uint8_t dest = DefaultDest(), uint8_t source = DefaultSource()){
    //if (CheckRestrictions() != 0 ) return BAD_PARAMS;
    if (CheckMessages() != 0) return ERROR_FOUND;
    SetChannelState mes(chanel, 2, dest, source);
    SendMessage(mes, handle);
    if (CheckMessages() != 0) return ERROR_RETURNED;
    return 0;
}

int ChannelState(FT_HANDLE &handle, ChannelStateInfo *info, uint8_t chanel = DefaultChanel(), uint8_t dest = DefaultDest(), uint8_t source = DefaultSource()){
    //if (CheckRestrictions() != 0 ) return BAD_PARAMS;
    if (CheckMessages() != 0) return ERROR_FOUND;
    ReqChannelState mes(chanel, dest, source);
    SendMessage(mes, handle);
    if (CheckMessages() != 0) return ERROR_RETURNED;
    //load class to pointer
    return 0;
}
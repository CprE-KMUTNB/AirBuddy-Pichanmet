#pragma once
#include <stdio.h>
#include <string.h>
#include <IRac.h>

class AC
{
private:
    IRac ac;
    void initAc();
public:
    AC(uint16_t irSendPin);
    void setProtocol(decode_type_t protocol);
    bool sendAc(bool power, int protocol, int mode, int degrees, int fanspeed, int swingv, int swingh, bool econo, bool turbo);
};

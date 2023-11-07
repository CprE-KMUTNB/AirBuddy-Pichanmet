#pragma once
#include "AC.hpp"

AC::AC(uint16_t irSendPin) : ac(irSendPin)
{
    initAc();
}

void AC::initAc()
{
    ac.next.power = false;                          // Turn off the A/C initially.
    ac.next.model = 1;                              // Some A/Cs have different models. Try just the first.
    ac.next.mode = stdAc::opmode_t::kCool;          // Run in cool mode initially.
    ac.next.celsius = true;                         // Use Celsius for temp units. False = Fahrenheit
    ac.next.degrees = 25;                           // 25 degrees.
    ac.next.fanspeed = stdAc::fanspeed_t::kMedium;  // Start the fan at medium.
    ac.next.swingv = stdAc::swingv_t::kUpperMiddle; // Don't swing the fan up or down.
    ac.next.swingh = stdAc::swingh_t::kOff;         // Don't swing the fan left or right.
    ac.next.light = false;                          // Turn off any LED/Lights/Display that we can.
    ac.next.beep = false;                           // Turn off any beep from the A/C if we can.
    ac.next.econo = false;                          // Turn off any economy modes if we can.
    ac.next.filter = false;                         // Turn off any Ion/Mold/Health filters if we can.
    ac.next.turbo = false;                          // Don't use any turbo/powerful/etc modes.
    ac.next.quiet = false;                          // Don't use any quiet/silent/etc modes.
    ac.next.sleep = -1;                             // Don't set any sleep time or modes.
    ac.next.clean = false;                          // Turn off any Cleaning options if we can.
    ac.next.clock = -1;                             // Don't set any current time if we can avoid it.}
}

bool AC::sendAc(bool power, int protocol, int mode, int degrees, int fanspeed, int swingv, int swingh, bool econo, bool turbo)
{
  ac.next.power = power;
  ac.next.protocol = (decode_type_t)protocol;                     
  ac.next.mode = (stdAc::opmode_t)mode;         
  ac.next.degrees = degrees;                     
  ac.next.fanspeed = (stdAc::fanspeed_t)fanspeed;
  ac.next.swingv = (stdAc::swingv_t)swingv;      
  ac.next.swingh = (stdAc::swingh_t)swingh;      
  ac.next.econo = econo;                         
  ac.next.turbo = turbo;                         
  return ac.sendAc();
}
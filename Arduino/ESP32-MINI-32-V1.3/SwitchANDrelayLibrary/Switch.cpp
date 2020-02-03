/*

                        _______________________      _false
                       |                       |   || |
 input                 |                       || ||| |
                  _____|                       ||_||| ||____________

 poll                   ^                        ^   ^          ^
 switchedTime           ^                        ^
                         <---------100ms--------><--->
 debounceDelay           <----50ms---->          <----50ms---->
 switched               1                        1   0          0
 newlevel               1                        0   1          0
                         ________________________
 level             _____|                        |___________________

.......................................................................
                            _______________________             _______
                           |                       |           |
 input                     |                       |           |
                   ________|                       |___________|

 longPressDelay             <----------->

 doubleClickDelay           <-------------------------------------->
                                          _________
 longPressLatch    ______________________|         |_________________
                                          _
 _longPress        ______________________| |__________________________
                                                                _
_doubleClick       ____________________________________________| |____

*/

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif
#include "Switch.h"

// level(0)
Switch::Switch(byte _pin, byte PinMode, bool polarity, int debounceDelay, int longPressDelay, int doubleClickDelay):
pin(_pin), polarity(polarity), debounceDelay(debounceDelay), longPressDelay(longPressDelay), doubleClickDelay(doubleClickDelay)
{ pinMode(pin, PinMode);
  _switchedTime = millis();
  level = digitalRead(pin);
}

bool Switch::poll()
{ _longPress = _doubleClick = false;
  bool newlevel = digitalRead(pin);

  if(!longPressLatch)
  { _longPress = on() && ((long)(millis() - pushedTime) > longPressDelay); // true just one time between polls
    longPressLatch = _longPress; // will be reset at next switch
  }

  if((newlevel != level) & (millis() - _switchedTime >= debounceDelay))
  { _switchedTime = millis();
    level = newlevel;
    _switched = 1;
    longPressLatch = false;

    if(pushed())
    { _doubleClick = (long)(millis() - pushedTime) < doubleClickDelay;
      pushedTime = millis();
    }
    return _switched;
  }
  return _switched = 0;
}

bool Switch::switched()
{ return _switched;
}

bool Switch::on()
{ return !(level^polarity);
}

bool Switch::pushed()
{ return _switched && !(level^polarity);
}

bool Switch::released()
{ return _switched && (level^polarity);
}

bool Switch::longPress()
{ return _longPress;
}

bool Switch::doubleClick()
{ return _doubleClick;
}

#include "peripheral.h"




const char* get_subsystem(subsystem_type system) {
    switch (system) {
        case LIGHT_SENSOR: return "light_sensor";

        case FAN: return "fan";

        case DOOR: return "door";

        case BACKLIGHT: return "backlight";
    }
    return "";
}














typedef char *ASCII_char;

char* get_param(ASCII_char param_num)
{
    if (param_num == NULL) return; // Safety check
    switch (atoi(param_num)) { // Convert ASCII_char (char*) to integer
        case 1:  return "Heating";
        case 2:  return "Standby";
        case 4:  return "Run-up Time Control";
        case 9:  return "Error Acknowledgment";
        case 10: return "Pumping Station: (Error Acknowledgment)";
        case 12: return "Enable Venting";
        case 17: return "Rotation Speed Switch Point";
        case 19: return "Configuration Output DO2";
        case 23: return "Motor Pump";
        case 24: return "Configuration Output DO1";
        case 25: return "Operation Mode Backing Pump";
        case 26: return "Rotation Speed Setting Mode";
        case 27: return "Gas Mode";
        case 30: return "Venting Mode";
        case 35: return "Configuration Accessory Connection A1";
        case 36: return "Configuration Accessory Connection B1";
        case 37: return "Configuration Accessory Connection A2";
        case 38: return "Configuration Accessory Connection B2";
        case 50: return "Sealing Gas";
        case 55: return "Configuration Output AO1";
        case 60: return "Control Via Interface";
        case 61: return "Interface Selection Locked";
        case 62: return "Configuration Input DI1";
        case 63: return "Configuration Input DI2";
        case 300: return "Remote Priority";
        case 302: return "Rotation Speed Switch Point Attained";
        case 303: return "Error Code";
        case 304: return "Excess Temperature Electronic Drive Unit";
        case 305: return "Excess Temperature Pump";
        case 306: return "Set Rotation Speed Attained";
        case 307: return "Pump Accelerates";
        case 308: return "Set Rotation Speed (Hz)";
        case 309: return "Actual Rotation Speed (Hz)";
        case 310: return "Drive Current";
        case 311: return "Operating Hours Pump";
        case 315: return "Nominal Rotation Speed (Hz)";
        case 316: return "Drive Power";
        case 319: return "Pump Cycles";
        case 326: return "Temperature Electronic";
        case 330: return "Temperature Pump Bottom Part";
        case 336: return "Acceleration / Deceleration";
        case 342: return "Temperature Bearing";
        case 346: return "Temperature Motor";
        case 700: return "Set Value Run-up Time";
        case 701: return "Rotation Speed Switch Point 1";
        case 707: return "Set Value in Rotation Speed Setting Mode";
        default: return "Unknown command";
    }
}
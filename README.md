# CGEM-IT_interlock

Software implementation of the CGEM-IT interlock. 
In this file the pinout and the main variables are reported, all these variables can be tuned according to the situation.
  
  --- SERIAL MONITOR COMMANDS ---

  RESET            = restart the system
  activateSUM      = activate the SUM, all the controls are skipped;
  disableSUM       = deactivate the SUM, the controls are performed again;
  activateTESTMODE = activate the TESTMODE, all the measurements are perfomed but not the state machine;
  disableTESTMODE  = deactivate the TESTMODE.
  
  --- LED PINOUT  ---

  A0 = LV state
  A1 = HV state (meaningless for the moment)
  A2 = environment state
  A3 = gas state
  A4 = cooling state

  --- PINOUT ---

  CONTROLLINO_R0 = OUTPUT, mainframe LV;
  CONTROLLINO_R1 = OUTPUT, mainframe HV;
  CONTROLLINO_R2 = OUTPUT, board HV;
 
           = INPUT, pin LV determination state;

  A7,A9              = INPUT, Ar/ISO measure;
  A14                = INPUT, cooling/chiller state;
  20,21              = SDA,SCL, BUS I2C;

  --- I2C ADDRESSES ---

  CHIP CAP = {0X22,0X24,0X28}
  BME      = {0X77}

  --- VARIABLES ---

  ramp_down              = unisgned long (ms), time length of the HV ramp down;

  UDP_state              = bool, to activate the UDP communication to all variables;
  UDP_time_interval      = unisgned long (ms), time interval in between UDP messages
  UDP_time_post_interval = unisgned long (ms), time interval post alarm during which the alarms are sent with max frequency

  CONTROLLINO IP = {192,168,2,177}
  REMOTE IP      = {192,168,2,176}

  SUM                  = bool, activate the super user mode;                         
  SUM_gas_skip         = bool, skip the gas control, gas state impose to 1;
  SUM_cooling_skip     = bool, skip the cooling control, cooling state impose to 1;
  SUM_environment_skip = bool, skip the environment control, environment state impose to 1;
  SUM_LV_skip          = bool, skip the LV control, LV state impose to 1;
  SUM_HV_skip          = bool, skip the HV control, HV state impose to 1;

  --- ETHERNET VARIABLES ---

  PC settings:

    IP assignment:   manual
    IPv4 address:    192.168.2.176
    IPv4 mask:       255.255.255.0
    IPv4 gateway:    192.168.2.177
    IPv4 DNS server: 192.168.2.177






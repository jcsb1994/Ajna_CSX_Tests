-- =============================================================================
-- The following directives assign pins to the locations specific for the
-- PSoC 4100 S Plus device.
-- =============================================================================

-- === CapSense ===
attribute port_location of \CapSense:CintA(0)\ : label is "PORT(4,2)";
attribute port_location of \CapSense:CintB(0)\ : label is "PORT(4,3)";
attribute port_location of \CapSense:Rx(0)\ : label is "PORT(4,6)";
attribute port_location of \CapSense:Tx(0)\ : label is "PORT(0,2)";

-- === I2C ===
attribute port_location of \EZI2C:scl(0)\ : label is "PORT(3,0)";
attribute port_location of \EZI2C:sda(0)\ : label is "PORT(3,1)";
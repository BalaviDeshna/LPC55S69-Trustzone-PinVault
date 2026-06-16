# LPC55S69_Trustzone_PinVault
Project implementing ARM TrustZone-M security on the NXP LPCXpresso55S69 development board.  
<br>
  
## Project Overview
A secure PIN vault demonstrating TrustZone secure/non-secure world separation. 
The PIN is stored in the secure world and is never accessible to the non-secure code. 
Includes attempt lockout and LED feedback.  
<br>
  
## Hardware
- NXP LPCXpresso55S69 Development Board (LPC55S69, Cortex-M)
<br>
  
## Tools
- MCUXpresso IDE
- NXP MCUXpresso SDK for LPC55S69  
<br>
  
## How to build
1. download NXP MCUXpresso SDK for LPC55S69 from mcuxpresso.nxp.com
2. import the _s and the  _ns projects from a trustzone example
3. replace main_s, main_ns and veneer_table files in the project source folder with the files from this repository
4. build the _s project first, followed by the _ns project
5. flash the _ns project first (this automatically flashes both the secure and the non-secure binaries together)
<br><br>

> [!NOTE]
> The secure project must always be built before the non-secure one.
> The secure world configures the TrustZone memory boundaries and SAU at startup. The non-secure application depends on this being in place before it runs.
> the NS debug configuration automatically includes the secure binary in the flash sequence.

<br>

## License
BSD 3-Clause License, see LICENCSE file  
<br><br>

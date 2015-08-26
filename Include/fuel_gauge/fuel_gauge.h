/* Copyright (c) 2014 Before Technology. All Rights Reserved.
 */

/** @file
 *
 * @details This module implements a fuel gauge module by communicating with the MAX17048(Maxim) IC 
 *			using the I2C transport protocol. The interfaces of I2C are based on the I2C APIs provided 
 * 			by Nordic Semiconductor and the details describing in the data sheet of MAX17048 IC.
 * 			
 * 			This module describes specific methods to work as a simulative fuel gauge for knowing the 
 * 			capacity of a lithium-ion battery which is the main power supply of the Air Purifier, the 
 * 			products of Before Technology.
 * 			
 * 			There are maybe some problems of this module and you can contact the engineers of Before
 * 			Technology.
 *
 * @note	If there is no special statement the fuel gauge means the MAX17048 IC which is as a I2C slave.
 *			Also know that the MSB means the upper byte of one register and the address of MSB is lower
 * 			than the address of the LSB. For example, the address of MSB part of MODE register is 0x06 and 
 *			the address of LSB part is 0x07. And the data of MSB part should be firstly transmitted to the 
 *			fuel gauge.
 *
 *			Especially note that the alert service routine should clear the corresponding bit of STATUS register.
 *
 */
#ifndef FUEL_GAUGLE_H__
#define FUEL_GAUGLE_H__

#include <stdint.h>

// The address of the fuel gauge
#define FUEL_GAUGLE_SLAVE_ADDRESS	(uint8_t)0x6C
#define FUEL_GAUGLE_WRITE_ADDRESS	(uint8_t)0x6C
#define FUEL_GAUGLE_READ_ADDRESS	(uint8_t)0x6D

// The address of registers of fuel gauge(get the details from MAX17048 data sheet)
#define FUEL_GAUGLE_REGISTER_VCELL_ADDRESS		(uint8_t)0x02 // Read
#define FUEL_GAUGLE_REGISTER_SOC_ADDRESS		(uint8_t)0x04 // Read
#define FUEL_GAUGLE_REGISTER_MODE_ADDRESS		(uint8_t)0x06 // Write
#define FUEL_GAUGLE_REGISTER_VERSION_ADDRESS	(uint8_t)0x08 // Read
#define FUEL_GAUGLE_REGISTER_HIBRT_ADDRESS		(uint8_t)0x0A // Read/Write
#define FUEL_GAUGLE_REGISTER_CONFIG_ADDRESS		(uint8_t)0x0C // Read/Write
#define FUEL_GAUGLE_REGISTER_VALRT_ADDRESS		(uint8_t)0x14 // Read/Write
#define FUEL_GAUGLE_REGISTER_CRATE_ADDRESS		(uint8_t)0x16 // Read
#define FUEL_GAUGLE_REGISTER_VRESET_ADDRESS		(uint8_t)0x18 // Read/Write
#define FUEL_GAUGLE_REGISTER_STATUS_ADDRESS		(uint8_t)0x1A // Read/Write
#define FUEL_GAUGLE_REGISTER_TABLE_ADDRESS		(uint8_t)0x40 // Write
#define FUEL_GAUGLE_REGISTER_CMD_ADDRESS		(uint8_t)0xFE // Read/Write

// The units of some registers provided above
#define FUEL_GAUGLE_VCELL_UNIT	(float)78.125	// the value of VCELL register should multiply be the unit(uV)
#define FUEL_GAUGLE_CRATE_UNIT	(float)0.208	// the value of CRATE register should multiply be the unit(%)

// The bit-positions of some registers provided above
// MODE register
#define FUEL_GAUGLE_REGISTER_MODE_MSB_QS_BIT_POS		(6U)	// Quick-Start
#define FUEL_GAUGLE_REGISTER_MODE_MSB_ES_BIT_POS		(5U)	// Enable Sleep Mode
#define FUEL_GAUGLE_REGISTER_MODE_MSB_HS_BIT_POS		(4U)	// Hibernate State
// CONFIG register
#define FUEL_GAUGLE_REGISTER_CONFIG_MSB_RCOMP_BIT_POS	(0U)	// The value for optimization. The default value is 0x97
#define FUEL_GAUGLE_REGISTER_CONFIG_LSB_SLEEP_BIT_POS	(7U)	// Force the IC in or out of sleep mode
#define FUEL_GAUGLE_REGISTER_CONFIG_LSB_ALSC_BIT_POS	(6U)	// Enable or disable SOC change alert 
#define FUEL_GAUGLE_REGISTER_CONFIG_LSB_ALRT_BIT_POS	(5U)	// Set if an alert occurs and clear it in alert service routine
#define FUEL_GAUGLE_REGISTER_CONFIG_LSB_ATHD_BIT_POS	(4U)	// Empty alert threshold. The default value is 0x1C(32% - 0x1C% = 4%)
// STATUS register
#define FUEL_GAUGLE_REGISTER_STATUS_MSB_ENVR_BIT_POS	(6U)	// Enable the alert of voltage-reset event(see VRESET/ID register)
#define FUEL_GAUGLE_REGISTER_STATUS_MSB_SC_BIT_POS		(5U)	// 1% SOC change
#define FUEL_GAUGLE_REGISTER_STATUS_MSB_HD_BIT_POS		(4U)	// SOC low
#define FUEL_GAUGLE_REGISTER_STATUS_MSB_VR_BIT_POS		(3U)	// voltage reset
#define FUEL_GAUGLE_REGISTER_STATUS_MSB_VL_BIT_POS		(2U)	// voltage low
#define FUEL_GAUGLE_REGISTER_STATUS_MSB_VH_BIT_POS		(1U)	// voltage high
#define FUEL_GAUGLE_REGISTER_STATUS_MSB_RI_BIT_POS		(0U)	// reset indicator. Set when the device powers.

// Some default value of register
#define FUEL_GAUGLE_REGISTER_CONFIG_RCOMP_VALUE			(uint8_t)0x97

// Macros functions
#define CHECK_BIT(byte, bit)	(((byte)>>bit)&(1U))
#define CLEAR_BIT(byte, pos)	((byte)&(~(1U<<(pos))))

// As described in MAX17048 data sheet, the empty alert threshold is the value of (32-ATHD)%.
#define GET_ATHD_VALUE(cap)		(32-(cap))

// Other macros
#ifndef ENABLE
#define ENABLE 1
#endif

#ifndef DISABLE
#define DISABLE 0
#endif

#ifndef SET_BIT
#define SET_BIT 1
#endif

#ifndef UNSET_BIT
#define UNSET_BIT 0
#endif

// Statements for APIs
 
/**@brief Initialize the fuel gauge
 * 
 * @note Attention that before initialize the fuel gauge, the I2C peripheral must be initialized.
 *
 * @retval true if success to initialize the fuel gauge.
 *         false if fail to initialize the fuel gauge.
 *
 */
bool InitFuelGauge(void);

 

/**@brief Get the capacity of the battery by reading the SOC register of the fuel gauge.
 * 
 * @note The precision of capacity is integer. Modify this function to involve the LSB part of SOC register 
 *       if want to get the fractional part of capacity.
 *
 * @retval The capacity of the battery.
 */
uint8_t ReadBatteryCapacity(void);



#endif

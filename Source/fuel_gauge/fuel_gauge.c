/* 
 * Copyright (c) 2014 Before Technology. All Rights Reserved.
 */

#include "fuel_gauge.h"
#include <twi_master.h>
 
/**@brief Write data to register of the fuel gauge
 * 
 * @note The uMSBData will be transmitted firstly and followed by the uLSBData. As the description in 
 *       the MAX17048 data sheet, the uMSBData will be written to address specified by the uMemAddr and
 *       the uLSBData will be written to uMemAddr+1.
 *
 * @param uMemAddr Indicate the address of register of fuel gauge.
 * @param uMSBData The MSB part of data.
 * @param uLSBData The LSB part of data.
 *
 * @return
 * @retval true if success to write data to register of the fuel gauge.
 * @retval false if fail to write data to register of the fuel gauge.
 *
 */
bool WriteFuelGauge(uint8_t uMemAddr, uint8_t uMSBData, uint8_t uLSBData)
{
	uint8_t uAddress = FUEL_GAUGLE_WRITE_ADDRESS;
	uint8_t uLength = 3;
	uint8_t gData[uLength];
	gData[0] = uMemAddr;
	gData[1] = uMSBData;
	gData[2] = uLSBData;
	return twi_master_transfer(uAddress, gData, uLength, true);
}

 
/**@brief Read data from register of the fuel gauge
 * 
 * @note As the description in the MAX17048 data sheet, the first byte read is the MSB value of register 
 *       and the second byte read is the LSB value of register. The MSB data will be stored in the memory 
 *       pointed by puMSBData and the LSB data will be stored in the memory pointed by puLSBData.
 *
 * @param uMemAddr Indicate the address of register of fuel gauge.
 * @param puMSBData Pointer to the MSB part of data.
 * @param puLSBData Pointer to the LSB part of data.
 *
 * @return
 * @retval true if success to read data to register of the fuel gauge.
 * @retval false if fail to read data to register of the fuel gauge.
 *
 */
bool ReadFuelGauge(uint8_t uMemAddr, uint8_t *puMSBData, uint8_t *puLSBData)
{
	uint8_t uWriteAddress = FUEL_GAUGLE_WRITE_ADDRESS;
	uint8_t uReadAddress = FUEL_GAUGLE_READ_ADDRESS;
	uint8_t uLength = 2;
	uint8_t gData[uLength];
	if (false == twi_master_transfer(uWriteAddress, uMemAddr, 1, false))
		return false;
	if (false == twi_master_transfer(uReadAddress, gData, uLength, false))
		return false;
	if (NULL != puMSBData)
		puMSBData = gData[0];
	if (NULL != puLSBData)
		puLSBData = gData[1];
	return true;
}

 
/**@brief Initialize the fuel gauge
 * 
 * @note Attention that before initialize the fuel gauge, the I2C peripheral must be initialized.
 *
 * @retval true if success to initialize the fuel gauge.
 *         false if fail to initialize the fuel gauge.
 *
 */
bool InitFuelGauge(void)
{
	uint8_t uConfigMSB = 0;
	uint8_t uConfigLSB = 0;
	uConfigMSB = FUEL_GAUGLE_REGISTER_CONFIG_RCOMP_VALUE << FUEL_GAUGLE_REGISTER_CONFIG_MSB_RCOMP_BIT_POS;
	uConfigLSB = (DISABLE << FUEL_GAUGLE_REGISTER_CONFIG_LSB_SLEEP_BIT_POS) | 
				 (ENABLE << FUEL_GAUGLE_REGISTER_CONFIG_LSB_ALSC_BIT_POS) | 
				 (UNSET_BIT << FUEL_GAUGLE_REGISTER_CONFIG_LSB_ALRT_BIT_POS) | 
				 (GET_ATHD_VALUE(10) << FUEL_GAUGLE_REGISTER_CONFIG_LSB_ATHD_BIT_POS);
	return WriteFuelGauge(FUEL_GAUGLE_REGISTER_CONFIG_ADDRESS, uConfigMSB, uConfigLSB);
}

 

/**@brief Get the capacity of the battery by reading the SOC register of the fuel gauge.
 * 
 * @note The precision of capacity is integer. Modify this function to involve the LSB part of SOC register 
 *       if want to get the fractional part of capacity.
 *
 * @retval The capacity of the battery.
 */
uint8_t ReadBatteryCapacity(void)
{
	uint8_t uCapacityMSB = 0;
	uint8_t uCapacityLSB = 0;
	ReadFuelGauge(FUEL_GAUGLE_REGISTER_SOC_ADDRESS, &uCapacityMSB, &uCapacityLSB);
	return uCapacityMSB;
}
 
 
/**@brief ASR (alert service routine) of fuel gauge.
 * 
 * @note The content of this ASR should be placed in the ISR of GPIOTE(corresponding to the pin ALERT).
 * @note Clear CONFIG.ALRT and the corresponding bit of STATUS register if it had been set.
 * @note The alert of voltage reset doesn't open in this application.
 *
 */
void FuelGaugeASR(void)
{
	uint8_t uConfigMSB = 0;
	uint8_t uConfigLSB = 0;
	ReadFuelGauge(FUEL_GAUGLE_REGISTER_CONFIG_ADDRESS, &uConfigMSB, &uConfigLSB);
	// Check whether CONFIG.ALRT had been set and handle it if so.
	if (CHECK_BIT(uConfigLSB, FUEL_GAUGLE_REGISTER_CONFIG_LSB_ALRT_BIT_POS)) {
		CLEAR_BIT(uConfigLSB, FUEL_GAUGLE_REGISTER_CONFIG_LSB_ALRT_BIT_POS); // clear the alert;
		WriteFuelGauge(FUEL_GAUGLE_REGISTER_CONFIG_ADDRESS, uConfigMSB, uConfigLSB);
		// Then check the value of STATUS register to verify the source of this alert.
		// note that there are maybe more than one source to generate the alert so check it carefully.
		uint8_t uStatusMSB = 0;
		ReadFuelGauge(FUEL_GAUGLE_REGISTER_STATUS_ADDRESS, &uStatusMSB, NULL);
		if (CHECK_BIT(uStatusMSB, FUEL_GAUGLE_REGISTER_STATUS_MSB_RI_BIT_POS)) { // Power up
			InitFuelGauge();
			CLEAR_BIT(uStatusMSB, FUEL_GAUGLE_REGISTER_STATUS_MSB_RI_BIT_POS);
		}
		if (CHECK_BIT(uStatusMSB, FUEL_GAUGLE_REGISTER_STATUS_MSB_VH_BIT_POS)) {
			//DoSomthing();
			CLEAR_BIT(uStatusMSB, FUEL_GAUGLE_REGISTER_STATUS_MSB_VH_BIT_POS);
		}
		if (CHECK_BIT(uStatusMSB, FUEL_GAUGLE_REGISTER_STATUS_MSB_VL_BIT_POS)) {
			//DoSomthing();
			CLEAR_BIT(uStatusMSB, FUEL_GAUGLE_REGISTER_STATUS_MSB_VL_BIT_POS);
		}
		if (CHECK_BIT(uStatusMSB, FUEL_GAUGLE_REGISTER_STATUS_MSB_VR_BIT_POS)) {
			//DoSomthing();
			CLEAR_BIT(uStatusMSB, FUEL_GAUGLE_REGISTER_STATUS_MSB_VR_BIT_POS);
		}
		if (CHECK_BIT(uStatusMSB, FUEL_GAUGLE_REGISTER_STATUS_MSB_HD_BIT_POS)) {
			//DoSomthing();
			CLEAR_BIT(uStatusMSB, FUEL_GAUGLE_REGISTER_STATUS_MSB_HD_BIT_POS);
		}
		if (CHECK_BIT(uStatusMSB, FUEL_GAUGLE_REGISTER_STATUS_MSB_SC_BIT_POS)) {
			// uBatteryCapacity = ReadBatteryCapacity();
			CLEAR_BIT(uStatusMSB, FUEL_GAUGLE_REGISTER_STATUS_MSB_SC_BIT_POS);
		}
	}
}
 
 
 
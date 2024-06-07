#include "main.h"

uint8_t gearInformation;

void GearInformationSerice(uint8_t *data)
{
	gearInformation = data[2] & 0x0F;
}

uint8_t GearInformationGet(void)
{
	return gearInformation;
}


#include "main.h"


PowerModeStatusStruct powerModeStatus = {0};

void PowerModeService(uint8_t *data)
{
	powerModeStatus.Bytes.byte0 = data[0] & 0x1F;
	powerModeStatus.Bytes.byte1 = data[1] & 0x03;
}

uint8_t PowerModeGetStatus(void)
{
	return powerModeStatus.Status.keyPosition;
}

bool PowerModeGetKeyInSense(void)
{
	return powerModeStatus.Status.keyInSense;
}


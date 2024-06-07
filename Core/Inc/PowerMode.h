
#ifndef POWER_MODE_H
#define POWER_MODE_H


#define POWER_MODE_OFF 0x00
#define POWER_MODE_ACCESSORY 0x03
#define POWER_MODE_IGNITION 0x04
#define POWER_MODE_CRANK 0x15
#define POWER_MODE_MASK 0x1F

// MSCAN
typedef union PowerModeStatusStruct
{
	struct
	{
		uint8_t byte1;
		uint8_t byte0;
	}Bytes;
	struct
	{
		unsigned keyOutSense:1;
		unsigned keyInSense:1;
		unsigned :6;
		unsigned keyPosition:5;
		unsigned :3;
	}Status;
}PowerModeStatusStruct;


void PowerModeService(uint8_t *data);
uint8_t PowerModeGetStatus(void);
bool GetKeySense(void);

#endif // POWER_MODE_H

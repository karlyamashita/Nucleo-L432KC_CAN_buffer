#ifndef GEAR_INFORMATION_H
#define GEAR_INFORMATION_H

#define GEAR_PARK 0x0D
#define GEAR_REVERSE 0x0B
#define GEAR_NUETRAL 0x00
#define GEAR_DRIVE 0x01

void GearInformationSerice(uint8_t *data);
uint8_t GearInformationGet(void);

#endif // GEAR_INFORMATION_H

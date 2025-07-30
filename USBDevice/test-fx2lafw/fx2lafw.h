#ifndef FX2LAFW_H
#define FX2LAFW_H

// fx2lafw Protocol Commands
#define CMD_GET_FW_VERSION      0xB0
#define CMD_START               0xB1
#define CMD_GET_REVID           0xB2
#define CMD_SET_COUPLING        0xB4
#define CMD_SET_SAMPLERATE      0xB5
#define CMD_SET_NUMSAMPLES      0xB6

// fx2lafw Protocol Values
#define FX2LAFW_COUPLING_AC     0x00
#define FX2LAFW_COUPLING_DC     0x01

// Supported sample rates (Hz)
static const uint32_t fx2lafw_samplerates[] = {
    20000, 25000, 50000, 100000, 200000, 250000, 500000, 
    1000000, 2000000, 3000000, 4000000, 6000000, 8000000, 
    12000000, 16000000, 20000000, 24000000
};

#define FX2LAFW_NUM_SAMPLERATES (sizeof(fx2lafw_samplerates) / sizeof(fx2lafw_samplerates[0]))

#endif // FX2LAFW_H

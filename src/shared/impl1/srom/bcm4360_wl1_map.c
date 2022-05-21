#include <typedefs.h>

/* NOTE: PLEASE CHOOSE THE PROPER REFERENCE SROM MAP AS THE BASE */

#if 0

uint16 wl_srom_map_1108[234] = {
/* NOTE: This piece of srom is medium power "MC5" design with 5GHz front-end module */
/* BCM934360MC5 P353 */
/* PA values MUST be calibrated and refilled for the actual EEPROM/SROM-less design*/
/* MAC address are zero'ed */
/* CRC here is not recalculated, do not program this to EEPROM/SROM, this is for EEPROM/SROM-less use only */
/* srom[000]: */ 0x3801, 0x3000, 0x0633, 0x14e4, 0x0218, 0x1b7e, 0x0a00, 0x2bc4,
/* srom[008]: */ 0x2a64, 0x2964, 0x2c64, 0x3ce7, 0x460a, 0x113c, 0x2132, 0x3164,
/* srom[016]: */ 0x1852, 0x0005, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/* srom[024]: */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/* srom[032]: */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/* srom[040]: */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/* srom[048]: */ 0x43a2, 0x8000, 0x0002, 0x0000, 0x3ff3, 0x1800, 0x0000, 0xffff,
/* srom[056]: */ 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
/* srom[064]: */ 0x0634, 0x1353, 0x0000, 0x1000, 0x0002, 0x0000, 0x0000, 0x0000,
/* srom[072]: */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0xffff, 0xffff,
/* srom[080]: */ 0x0707, 0x4747, 0x4785, 0x8585, 0x0077, 0x30a1, 0x00a1, 0xffff,
/* srom[088]: */ 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
/* srom[096]: */ 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x3222, 0x3222, 0x3222,
/* srom[104]: */ 0x0100, 0x0100, 0x0100, 0x0004, 0xff42, 0xff24, 0x188e, 0xfce6,
/* srom[112]: */ 0xffff, 0xb300, 0x4c4c, 0x4c4c, 0xff5d, 0x1760, 0xfd23, 0xff54,
/* srom[120]: */ 0x1731, 0xfd1e, 0xff4c, 0x16ec, 0xfd22, 0xff62, 0x173d, 0xfd39,
/* srom[128]: */ 0xff42, 0xff3e, 0x15f9, 0xfd36, 0xffff, 0xb300, 0x4c4c, 0x4c4c,
/* srom[136]: */ 0xff5a, 0x1737, 0xfd26, 0xff52, 0x16f5, 0xfd28, 0xff4f, 0x171d,
/* srom[144]: */ 0xfd20, 0xff50, 0x16d1, 0xfd2f, 0xff42, 0xff25, 0x18a6, 0xfce2,
/* srom[152]: */ 0xffff, 0xb300, 0x4c4c, 0x4c4c, 0xff5a, 0x1729, 0xfd25, 0xff62,
/* srom[160]: */ 0x175c, 0xfd30, 0xff48, 0x1720, 0xfd15, 0xff54, 0x1741, 0xfd21,
/* srom[168]: */ 0x0000, 0x0000, 0x4000, 0x9944, 0x4000, 0x9944, 0x4400, 0x0000,
/* srom[176]: */ 0x6420, 0xeca8, 0x6420, 0xeca8, 0x6420, 0xeca8, 0x0000, 0x0000,
/* srom[184]: */ 0x6420, 0xcca8, 0x6420, 0xcca8, 0x6420, 0xcca8, 0x0000, 0x0000,
/* srom[192]: */ 0x6420, 0xcca8, 0x6420, 0xcca8, 0x6420, 0xcca8, 0x0000, 0x0000,
/* srom[200]: */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/* srom[208]: */ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/* srom[216]: */ 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0x0f12, 0xffff, 0xffff,
/* srom[224]: */ 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
/* srom[232]: */ 0xffff, 0xb70b
};

#else

uint16 wl_srom_map_1108[234] = {
/* NOTE: THIS PIECE OF SROM IS HIGH POWER "MCH5" DESIGN */
/* BCM94360MCH5 P202 */
/* PA values MUST be calibrated and refilled for the actual EEPROM/SROM-less design*/
/* MAC address are zero'ed */
/* CRC here is not recalculated, do not program this to EEPROM/SROM, this is for EEPROM/SROM-less use only */
/* srom[000]: */  0x3801, 0x0200, 0x0621, 0x14e4, 0x0218, 0x1b7e, 0x0a00, 0x2bc4,
/* srom[008]: */  0x2a64, 0x2964, 0x2c64, 0x3ce7, 0x460a, 0x113c, 0x2132, 0x3164,
/* srom[016]: */  0x1852, 0x0005, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/* srom[024]: */  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/* srom[032]: */  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/* srom[040]: */  0x0000, 0x0000, 0x1008, 0x0305, 0x0000, 0x0000, 0x0000, 0x0000,
/* srom[048]: */  0x43a2, 0x8000, 0x0002, 0x0000, 0x3ff3, 0x1800, 0x0000, 0xffff,
/* srom[056]: */  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
/* srom[064]: */  0x0634, 0x1202, 0x0000, 0x1000, 0x0000, 0x0030, 0x0000, 0x0000,
/* srom[072]: */  0x0000, 0x0000, 0x0000, 0x5131, 0x001b, 0xffff, 0xffff, 0xffff,
/* srom[080]: */  0x0700, 0x4747, 0x4785, 0x8585, 0x0077, 0x1841, 0x0041, 0xffff,
/* srom[088]: */  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
/* srom[096]: */  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000,
/* srom[104]: */  0x0000, 0x0000, 0x0000, 0x0004, 0xff4c, 0xfe72, 0x14c0, 0xfac7,
/* srom[112]: */  0xffff, 0x0b00, 0x6460, 0x6464, 0xff39, 0x1a55, 0xfcc7, 0xff38,
/* srom[120]: */  0x1a7f, 0xfcc3, 0xff33, 0x1a66, 0xfcc4, 0xff36, 0x1a7b, 0xfcc2,
/* srom[128]: */  0xff4c, 0xfe80, 0x1472, 0xfabc, 0xffff, 0x0b00, 0x6460, 0x6464,
/* srom[136]: */  0xff3a, 0x1a0b, 0xfcd3, 0xff38, 0x1a37, 0xfccd, 0xff37, 0x1aa1,
/* srom[144]: */  0xfcc0, 0xff37, 0x1a6f, 0xfcc4, 0xff4c, 0xfe82, 0x14bf, 0xfad9,
/* srom[152]: */  0xffff, 0x0b00, 0x6460, 0x6464, 0xff3a, 0x1a28, 0xfccd, 0xff38,
/* srom[160]: */  0x1a2a, 0xfcce, 0xff35, 0x1a93, 0xfcc1, 0xff38, 0x1aab, 0xfcbe,
/* srom[168]: */  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/* srom[176]: */  0x4000, 0xff44, 0x4000, 0xff44, 0x4000, 0xff44, 0x0000, 0x0000,
/* srom[184]: */  0x4000, 0xee66, 0x4000, 0xee66, 0x4000, 0xee66, 0x0000, 0x0000,
/* srom[192]: */  0x4000, 0xee66, 0x4000, 0xee66, 0x4000, 0xee66, 0x0000, 0x0000,
/* srom[200]: */  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/* srom[208]: */  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
/* srom[216]: */  0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0x0f12, 0xffff, 0xffff,
/* srom[224]: */  0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff,
/* srom[232]: */  0xffff, 0xff0b
};

#endif




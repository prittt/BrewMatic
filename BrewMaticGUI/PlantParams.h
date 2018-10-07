//Settings
#ifndef PLANTPARAMS_H_
#define PLANTPARAMS_H_

float hysteresis = 0.5; // Must be between 0.1 and 0.9 (diplay reasons)
float inertia    = 0.9; // The fire will be turned OFF at target + delta - inertia. Inertia must be less than hysteresis*2 and between 0.1 and 0.9 (diplay reasons)
float maxDelta   = 2.0 ; // Maximum difference between lower and upper temperature im the mash tune. Must be between 0.1 and 0.9 (diplay reasons)

#define MASH_TUN_WEIGHT 20 // Kg
#define STEEL_CP 0.502 // KJ/(Kg*K)
#define WATER_CP 4.186 // KJ/(Kg*K)
#define MALTS_CP 3.500 // KJ/(Kg*K)

#endif //PLANTPARAMS_H_

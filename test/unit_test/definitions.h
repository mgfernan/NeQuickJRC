#ifndef TEST_UNIT_TEST_DEFINITIONS_H
#define TEST_UNIT_TEST_DEFINITIONS_H

#include <stdlib.h>

#include "NeQuickG_JRC_iono_profile.h"

/*
    This file contains the definitions that were under ifdef FTR_UNIT_TEST directive

    They have moved away from production code
*/
#define NEQUICK_UNIT_TEST_EXCEPTION -10

/** Unit test for CF2 and Cm3
 * @param[in, out] pContext F2 Fourier coefficients context
 * @param[in] pTime time context
 * @param[in] Azr Effective sun spot number
 */
extern int32_t F2_layer_fourier_coefficients_interpolate(
    F2_layer_fourier_coeff_context_t* const pContext,
    const NeQuickG_time_t * const pTime,
    const double_t Azr);



#endif  // TEST_UNIT_TEST_DEFINITIONS_H
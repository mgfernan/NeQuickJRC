/**
 * NeQuickG API Unit test
 *
 * @author Angela Aragon-Angel (maria-angeles.aragon@ec.europa.eu)
 * @ingroup NeQuickG_JRC_UT
 * @copyright Joint Research Centre (JRC), 2019<br>
 *  This software has been released as free and open source software
 *  under the terms of the European Union Public Licence (EUPL), version 1.<br>
 *  Questions? Submit your query at https://www.gsc-europa.eu/contact-us/helpdesk
 * @file
 */
#include "NeQuickG_JRC_API_test.h"

#include <math.h>

#include "NeQuickG_JRC.h"
#include "NeQuickG_JRC_context.h"
#include "NeQuickG_JRC_macros.h"
#include "NeQuickG_JRC_ray_slant.h"

#include "macros.h"

#ifndef FTR_MODIP_CCIR_AS_CONSTANTS
static bool test_NULL_modip_file(
  const char* const pCCIR_directory) {

  char *pModip_file = NULL;
  NeQuickG_handle nequick;
  (void)NeQuickG.init(pModip_file, pCCIR_directory, &nequick);
  return (nequick == NEQUICKG_INVALID_HANDLE);
}

static bool test_NULL_CCIR_dir(
  const char* const pModip_file) {

  char *pCCIR_directory = NULL;
  NeQuickG_handle nequick;
  (void)NeQuickG.init(pModip_file, pCCIR_directory, &nequick);
  return (nequick == NEQUICKG_INVALID_HANDLE);
}
#endif // !FTR_MODIP_CCIR_AS_CONSTANTS

#ifdef FTR_MODIP_CCIR_AS_CONSTANTS
#define test_bad_month(pModip_file, pCCIR_directory) \
  test_bad_month()
#endif

static bool test_bad_month(
  const char* const pModip_file,
  const char* const pCCIR_directory) {

  NeQuickG_handle nequick;
  int32_t ret_init = NeQuickG.init(pModip_file, pCCIR_directory, &nequick);
  if (ret_init != NEQUICK_OK) {
    return false;
  }

  bool ret = true;

  if (NeQuickG.set_time(nequick, 0, 0.0) == NEQUICK_OK) {
    ret = false;
  }

  if (NeQuickG.set_time(nequick, 13, 0.0) == NEQUICK_OK) {
    ret = false;
  }
  NeQuickG.close(nequick);

  return ret;
}

#ifdef FTR_MODIP_CCIR_AS_CONSTANTS
#define test_bad_UTC(pModip_file, pCCIR_directory) \
  test_bad_UTC()
#endif

static bool test_bad_UTC(
  const char* const pModip_file,
  const char* const pCCIR_directory) {

  NeQuickG_handle nequick;
  int32_t ret_init = NeQuickG.init(pModip_file, pCCIR_directory, &nequick);
  if (ret_init != NEQUICK_OK) {
    return false;
  }

  bool ret = true;

  if (NeQuickG.set_time(nequick, 1, -1.0) == NEQUICK_OK) {
    ret = false;
  }

  if (NeQuickG.set_time(nequick, 13, 25.0) == NEQUICK_OK) {
    ret = false;
  }
  NeQuickG.close(nequick);

  return ret;
}

#ifdef FTR_MODIP_CCIR_AS_CONSTANTS
#define test_bad_latitude(pModip_file, pCCIR_directory) \
  test_bad_latitude()
#endif

static bool test_bad_latitude(
  const char* const pModip_file,
  const char* const pCCIR_directory) {

  NeQuickG_handle nequick;
  int32_t ret_init = NeQuickG.init(pModip_file, pCCIR_directory, &nequick);
  if (ret_init != NEQUICK_OK) {
    return false;
  }

  bool ret = true;

  if (NeQuickG.set_receiver_position(nequick, 0.0, -91.0, 0.0) == NEQUICK_OK) {
    ret = false;
  }

  if (NeQuickG.set_receiver_position(nequick, 0.0, 91.0, 0.0) == NEQUICK_OK) {
    ret = false;
  }

  NeQuickG.close(nequick);

  return ret;
}

#ifdef FTR_MODIP_CCIR_AS_CONSTANTS
#define test_bad_coefficients(pModip_file, pCCIR_directory) \
  test_bad_coefficients()
#endif

static bool test_bad_coefficients(
  const char* const pModip_file,
  const char* const pCCIR_directory) {

  NeQuickG_handle nequick;
  int32_t ret_init = NeQuickG.init(pModip_file, pCCIR_directory, &nequick);
  if (ret_init != NEQUICK_OK) {
    return false;
  }

  bool ret = true;

  if (NeQuickG.set_solar_activity_coefficients(nequick, NULL, (size_t)3)
      == NEQUICK_OK) {
    ret = false;
  }

  double_t az[2] = {0.0, 0.0};
  if (NeQuickG.set_solar_activity_coefficients(nequick, az, (size_t)2)
      == NEQUICK_OK) {
    ret = false;
  }

  NeQuickG.close(nequick);

  return ret;
}

#ifdef FTR_MODIP_CCIR_AS_CONSTANTS
#define test_receiver_satellite_same_position(pModip_file, pCCIR_directory) \
  test_receiver_satellite_same_position()
#endif

// check that the TEC returned is close to 0
static bool test_receiver_satellite_same_position(
  const char* const pModip_file,
  const char* const pCCIR_directory) {

  NeQuickG_handle nequick;
  int32_t ret_init = NeQuickG.init(pModip_file, pCCIR_directory, &nequick);
  if (ret_init != NEQUICK_OK) {
    return false;
  }

  bool ret = true;

  double_t az[NEQUICKG_AZ_COEFFICIENTS_COUNT] = {0.0, 0.0, 0.0};
  if (NeQuickG.set_solar_activity_coefficients(
    nequick, az, (size_t)NEQUICKG_AZ_COEFFICIENTS_COUNT) != NEQUICK_OK) {
    ret = false;
  }

  uint8_t month = 1;
  double_t UTC = 0.0;
  if (NeQuickG.set_time(nequick, month, UTC) != NEQUICK_OK) {
    ret = false;
  }

  {
    double_t latitude = 0.0;
    double_t longitude = 0.0;
    double_t height = 0.0;
    if (NeQuickG.set_receiver_position(
        nequick, longitude, latitude, height) != NEQUICK_OK) {
      ret = false;
    }
  }

  {
    double_t latitude = 0.0;
    double_t longitude = 0.0;
    double_t height = 0.0;
    if (NeQuickG.set_satellite_position(
        nequick, longitude, latitude, height) != NEQUICK_OK) {
      ret = false;
    }
  }

  double_t TEC;
  if (NeQuickG.get_total_electron_content(nequick, &TEC) != NEQUICK_OK) {
    ret = false;
  }

  if (!THRESHOLD_COMPARE_TO_ZERO(TEC, 1.0e-10)) {
    ret = false;
  }

  NeQuickG.close(nequick);
  return ret;
}

#ifdef FTR_MODIP_CCIR_AS_CONSTANTS
#define test_bad_ray(pModip_file, pCCIR_directory) \
  test_bad_ray()
#endif

static bool test_bad_ray(
  const char* const pModip_file,
  const char* const pCCIR_directory) {

  NeQuickG_handle nequick;
  int32_t ret_init = NeQuickG.init(pModip_file, pCCIR_directory, &nequick);
  if (ret_init != NEQUICK_OK) {
    return false;
  }

  bool ret = true;

  double_t az[NEQUICKG_AZ_COEFFICIENTS_COUNT] = {0.0, 0.0, 0.0};
  if (NeQuickG.set_solar_activity_coefficients(
    nequick, az, (size_t)NEQUICKG_AZ_COEFFICIENTS_COUNT) != NEQUICK_OK) {
    ret = false;
  }

  uint8_t month = 1;
  double_t UTC = 0.0;
  if (NeQuickG.set_time(nequick, month, UTC) != NEQUICK_OK) {
    ret = false;
  }

  {
    double_t latitude = 0.0;
    double_t longitude = 0.0;
    double_t height = -3000000.0;
    if (NeQuickG.set_receiver_position(
        nequick, longitude, latitude, height) != NEQUICK_OK) {
      ret = false;
    }
  }

  {
    double_t latitude = 90.0;
    double_t longitude = 0.0;
    double_t height = 2000000.0;
    if (NeQuickG.set_satellite_position(
        nequick, longitude, latitude, height) != NEQUICK_OK) {
      ret = false;
    }
  }

  double_t TEC;
  if (NeQuickG.get_total_electron_content(nequick, &TEC) == NEQUICK_OK) {
    ret = false;
  }

  NeQuickG.close(nequick);
  return ret;
}

static bool setup_nequick(
  const char* const pModip_file,
  const char* const pCCIR_directory,
  NeQuickG_handle* const pNequick) {

  (void)pModip_file;
  (void)pCCIR_directory;

  if (NeQuickG.init(pModip_file, pCCIR_directory, pNequick) != NEQUICK_OK) {
    return false;
  }

  const double_t az[NEQUICKG_AZ_COEFFICIENTS_COUNT] = {
    236.831641,
    -0.39362878,
    0.00402826613,
  };

  if (NeQuickG.set_solar_activity_coefficients(
    *pNequick,
    az,
    (size_t)NEQUICKG_AZ_COEFFICIENTS_COUNT) != NEQUICK_OK) {
    NeQuickG.close(*pNequick);
    *pNequick = NEQUICKG_INVALID_HANDLE;
    return false;
  }

  if (NeQuickG.set_time(*pNequick, 3, 12.0) != NEQUICK_OK) {
    NeQuickG.close(*pNequick);
    *pNequick = NEQUICKG_INVALID_HANDLE;
    return false;
  }

  return true;
}

static bool set_link_endpoints(
  const NeQuickG_handle nequick,
  const double_t receiver_lon,
  const double_t receiver_lat,
  const double_t receiver_height_m,
  const double_t satellite_lon,
  const double_t satellite_lat,
  const double_t satellite_height_m) {

  if (NeQuickG.set_receiver_position(
      nequick,
      receiver_lon,
      receiver_lat,
      receiver_height_m) != NEQUICK_OK) {
    return false;
  }

  if (NeQuickG.set_satellite_position(
      nequick,
      satellite_lon,
      satellite_lat,
      satellite_height_m) != NEQUICK_OK) {
    return false;
  }

  return true;
}

static bool test_limb_path_matches_trapezoid_reference(
  const char* const pModip_file,
  const char* const pCCIR_directory) {

  NeQuickG_handle nequick;
  if (!setup_nequick(pModip_file, pCCIR_directory, &nequick)) {
    return false;
  }

  bool ret = true;
  if (!set_link_endpoints(
      nequick,
      -20.0,
      0.0,
      700000.0,
      20.0,
      0.0,
      700000.0)) {
    NeQuickG.close(nequick);
    return false;
  }

  double_t tec_model = 0.0;
  if (NeQuickG.get_total_electron_content(nequick, &tec_model) != NEQUICK_OK) {
    NeQuickG.close(nequick);
    return false;
  }

  NeQuickG_context_t* const pContext = (NeQuickG_context_t*)nequick;
  const double_t s_lo = min(
    pContext->ray.slant.receiver_s_km,
    pContext->ray.slant.satellite_s_km);
  const double_t s_hi = max(
    pContext->ray.slant.receiver_s_km,
    pContext->ray.slant.satellite_s_km);

  if (!(s_lo < 0.0 && s_hi > 0.0)) {
    ret = false;
  }

  const size_t segments = 4000;
  const double_t ds = (s_hi - s_lo) / (double_t)segments;

  double_t reference_integral = 0.0;
  for (size_t i = 0; i <= segments; i++) {
    const double_t s = s_lo + ((double_t)i * ds);
    double_t density;
    if (ray_slant_get_electron_density(pContext, s, &density) != NEQUICK_OK) {
      ret = false;
      break;
    }

    const double_t weight = (i == 0 || i == segments) ? 0.5 : 1.0;
    reference_integral += (weight * density);
  }

  const double_t tec_reference = (reference_integral * ds) / 1.0e13;
  if (!THRESHOLD_COMPARE(tec_model, tec_reference, 3.0e-2)) {
    ret = false;
  }

  NeQuickG.close(nequick);
  return ret;
}

static bool test_receiver_above_transmitter_supported(
  const char* const pModip_file,
  const char* const pCCIR_directory) {

  NeQuickG_handle nequick;
  if (!setup_nequick(pModip_file, pCCIR_directory, &nequick)) {
    return false;
  }

  bool ret = true;
  if (!set_link_endpoints(
      nequick,
      0.0,
      0.0,
      10000000.0,
      20.0,
      0.0,
      5000000.0)) {
    NeQuickG.close(nequick);
    return false;
  }

  double_t tec = 0.0;
  if (NeQuickG.get_total_electron_content(nequick, &tec) != NEQUICK_OK) {
    ret = false;
  }

  if (!(tec > 0.0)) {
    ret = false;
  }

  NeQuickG.close(nequick);
  return ret;
}

static bool test_near_grazing_perigee_supported(
  const char* const pModip_file,
  const char* const pCCIR_directory) {

  NeQuickG_handle nequick;
  if (!setup_nequick(pModip_file, pCCIR_directory, &nequick)) {
    return false;
  }

  bool ret = true;
  if (!set_link_endpoints(
      nequick,
      -24.75,
      0.0,
      700000.0,
      24.75,
      0.0,
      700000.0)) {
    NeQuickG.close(nequick);
    return false;
  }

  double_t tec = 0.0;
  if (NeQuickG.get_total_electron_content(nequick, &tec) != NEQUICK_OK) {
    ret = false;
  }

  if (!(tec > 0.0)) {
    ret = false;
  }

  NeQuickG.close(nequick);
  return ret;
}

static bool test_subsurface_perigee_rejected(
  const char* const pModip_file,
  const char* const pCCIR_directory) {

  NeQuickG_handle nequick;
  if (!setup_nequick(pModip_file, pCCIR_directory, &nequick)) {
    return false;
  }

  bool ret = true;
  if (!set_link_endpoints(
      nequick,
      -30.0,
      0.0,
      700000.0,
      30.0,
      0.0,
      700000.0)) {
    NeQuickG.close(nequick);
    return false;
  }

  double_t tec = 0.0;
  if (NeQuickG.get_total_electron_content(nequick, &tec) == NEQUICK_OK) {
    ret = false;
  }

  NeQuickG.close(nequick);
  return ret;
}


#if defined(FTR_MODIP_CCIR_AS_CONSTANTS) && (defined(__GNUC__) || defined(__GNUG__))
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wmissing-prototypes"
#endif
#ifdef FTR_MODIP_CCIR_AS_CONSTANTS
#undef NeQuickG_API_test
#endif
bool NeQuickG_API_test(
  const char* const pModip_file,
  const char* const pCCIR_folder) {
  bool ret = true;

#ifndef FTR_MODIP_CCIR_AS_CONSTANTS
  if (!test_NULL_modip_file(pCCIR_folder)) {
    ret = false;
  }

  if (!test_NULL_CCIR_dir(pModip_file)) {
    ret = false;
  }
#endif // !FTR_MODIP_CCIR_AS_CONSTANTS

  if (!test_bad_month(pModip_file, pCCIR_folder)) {
    ret = false;
  }

  if (!test_bad_UTC(pModip_file, pCCIR_folder)) {
    ret = false;
  }

  if (!test_bad_latitude(pModip_file, pCCIR_folder)) {
    ret = false;
  }

  if (!test_bad_coefficients(pModip_file, pCCIR_folder)) {
    ret = false;
  }

  if (!test_receiver_satellite_same_position(pModip_file, pCCIR_folder)) {
    ret = false;
  }

  if (!test_bad_ray(pModip_file, pCCIR_folder)) {
    ret = false;
  }

  if (!test_limb_path_matches_trapezoid_reference(pModip_file, pCCIR_folder)) {
    LOG_ERROR("Limb path reference test failed.");
    ret = false;
  }

  if (!test_receiver_above_transmitter_supported(pModip_file, pCCIR_folder)) {
    LOG_ERROR("Receiver-above-transmitter test failed.");
    ret = false;
  }

  if (!test_near_grazing_perigee_supported(pModip_file, pCCIR_folder)) {
    LOG_ERROR("Near-grazing perigee test failed.");
    ret = false;
  }

  if (!test_subsurface_perigee_rejected(pModip_file, pCCIR_folder)) {
    LOG_ERROR("Subsurface perigee rejection test failed.");
    ret = false;
  }

  return ret;
}
#if defined(FTR_MODIP_CCIR_AS_CONSTANTS) && (defined(__GNUC__) || defined(__GNUG__))
  #pragma GCC diagnostic pop
#endif

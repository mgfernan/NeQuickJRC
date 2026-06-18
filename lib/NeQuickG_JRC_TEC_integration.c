/** NeQuickG Total Electron Content (TEC) integration routine
 *
 * @author Angela Aragon-Angel (maria-angeles.aragon@ec.europa.eu)
 * @ingroup NeQuickG_JRC
 * @copyright Joint Research Centre (JRC), 2019<br>
 *  This software has been released as free and open source software
 *  under the terms of the European Union Public Licence (EUPL), version 1.<br>
 *  Questions? Submit your query at https://www.gsc-europa.eu/contact-us/helpdesk
 * @file
 */
#include "NeQuickG_JRC_TEC_integration.h"

#include <assert.h>
#include <stdlib.h>

#include "NeQuickG_JRC.h"
#include "NeQuickG_JRC_Gauss_Kronrod_integration.h"
#include "NeQuickG_JRC_macros.h"
#include "NeQuickG_JRC_math_utils.h"
#include "NeQuickG_JRC_ray_vertical.h"

#define NEQUICK_G_JRC_INTEGRATION_FIRST_POINT_KM (1000.0)
#define NEQUICK_G_JRC_INTEGRATION_SECOND_POINT_KM (2000.0)

/** Default integration tolerance for Kronrod G7-K15 integration method below 1000 km
 * See F.2.3.3.
 */
#define NEQUICK_G_JRC_INTEGRATION_KRONROD_TOLERANCE_BELOW_FIRST_POINT (0.001)
/** Default integration tolerance for Kronrod G7-K15 integration method above 1000 km
 * See F.2.3.3.
 */
#define NEQUICK_G_JRC_INTEGRATION_KRONROD_TOLERANCE_ABOVE_FIRST_POINT (0.01)

/** Kronrod G7-K15 integration maximum recursion level */
#define NEQUICK_G_JRC_RECURSION_LIMIT_MAX (50)
#define NEQUICK_G_JRC_NODE_MAX_COUNT (8)
#define NEQUICK_G_JRC_NODE_EPSILON (1.0e-10)

static int32_t compare_double_t(const void* const pLhs, const void* const pRhs) {
  const double_t lhs = *(const double_t*)pLhs;
  const double_t rhs = *(const double_t*)pRhs;

  if (lhs < rhs) {
    return -1;
  }
  if (lhs > rhs) {
    return 1;
  }
  return 0;
}

static bool is_strictly_inside(
  const double_t value,
  const double_t lower,
  const double_t upper) {

  return (
    (value > lower + NEQUICK_G_JRC_NODE_EPSILON) &&
    (value < upper - NEQUICK_G_JRC_NODE_EPSILON));
}

static void add_node(
  double_t* const pNodes,
  size_t* const pNodes_count,
  const double_t node) {

  assert(*pNodes_count < NEQUICK_G_JRC_NODE_MAX_COUNT);

  pNodes[*pNodes_count] = node;
  (*pNodes_count)++;
}

static void add_node_if_strictly_inside(
  double_t* const pNodes,
  size_t* const pNodes_count,
  const double_t node,
  const double_t lower,
  const double_t upper) {

  if (!is_strictly_inside(node, lower, upper)) {
    return;
  }

  for (size_t i = 0; i < *pNodes_count; i++) {
    if (fabs(pNodes[i] - node) <= NEQUICK_G_JRC_NODE_EPSILON) {
      return;
    }
  }

  add_node(pNodes, pNodes_count, node);
}

static void build_vertical_nodes(
  const double_t lower,
  const double_t upper,
  double_t* const pNodes,
  size_t* const pNodes_count) {

  add_node(pNodes, pNodes_count, lower);
  add_node(pNodes, pNodes_count, upper);

  add_node_if_strictly_inside(
    pNodes,
    pNodes_count,
    NEQUICK_G_JRC_INTEGRATION_FIRST_POINT_KM,
    lower,
    upper);

  add_node_if_strictly_inside(
    pNodes,
    pNodes_count,
    NEQUICK_G_JRC_INTEGRATION_SECOND_POINT_KM,
    lower,
    upper);

  qsort(pNodes, *pNodes_count, sizeof(double_t), compare_double_t);
}

static void build_slant_nodes(
  const ray_context_t* const pRay,
  const double_t lower,
  const double_t upper,
  double_t* const pNodes,
  size_t* const pNodes_count) {

  add_node(pNodes, pNodes_count, lower);
  add_node(pNodes, pNodes_count, upper);

  add_node_if_strictly_inside(
    pNodes,
    pNodes_count,
    0.0,
    lower,
    upper);

  const double_t shell_heights[] = {
    NEQUICK_G_JRC_INTEGRATION_FIRST_POINT_KM,
    NEQUICK_G_JRC_INTEGRATION_SECOND_POINT_KM,
  };

  for (size_t i = 0; i < 2; i++) {
    const double_t shell_radius = get_radius_from_height(shell_heights[i]);
    const double_t radius_delta =
      NeQuickG_square(shell_radius) -
      NeQuickG_square(pRay->slant.perigee_radius_km);

    if (radius_delta < 0.0) {
      continue;
    }

    const double_t crossing = sqrt(radius_delta);

    add_node_if_strictly_inside(
      pNodes,
      pNodes_count,
      -crossing,
      lower,
      upper);

    add_node_if_strictly_inside(
      pNodes,
      pNodes_count,
      crossing,
      lower,
      upper);
  }

  qsort(pNodes, *pNodes_count, sizeof(double_t), compare_double_t);
}

static double_t get_subinterval_max_height_km(
  const NeQuickG_context_t* const pContext,
  const double_t point_1,
  const double_t point_2) {

  if (pContext->ray.is_vertical) {
    return max(point_1, point_2);
  }

  const double_t max_abs_s = max(fabs(point_1), fabs(point_2));
  const double_t max_radius_km = sqrt(
    NeQuickG_square(pContext->ray.slant.perigee_radius_km) +
    NeQuickG_square(max_abs_s));

  return get_height_from_radius(max_radius_km);
}

static int32_t Gauss_Kronrod_integrate_impl(
  gauss_kronrod_context_t* const pContext,
  NeQuickG_context_t* const pNequick_Context,
  const double_t point_1,
  const double_t point_2,
  double_t* const pTEC) {

  pContext->recursion_level = 0;
  pContext->recursion_max = NEQUICK_G_JRC_RECURSION_LIMIT_MAX;

  return Gauss_Kronrod_integrate(
    pContext,
    pNequick_Context,
    point_1,
    point_2,
    pTEC);
}

static int32_t integrate_interval_list(
  NeQuickG_context_t* const pContext,
  const double_t* const pNodes,
  const size_t nodes_count,
  double_t* const pTEC) {

  int32_t ret;
  *pTEC = 0.0;

  gauss_kronrod_context_t gauss_kronrod_context;

  for (size_t i = 0; i + 1 < nodes_count; i++) {
    const double_t point_1 = pNodes[i];
    const double_t point_2 = pNodes[i + 1];

    const double_t max_height_km =
      get_subinterval_max_height_km(pContext, point_1, point_2);

    gauss_kronrod_context.tolerance =
      (max_height_km <= NEQUICK_G_JRC_INTEGRATION_FIRST_POINT_KM) ?
      NEQUICK_G_JRC_INTEGRATION_KRONROD_TOLERANCE_BELOW_FIRST_POINT :
      NEQUICK_G_JRC_INTEGRATION_KRONROD_TOLERANCE_ABOVE_FIRST_POINT;

    double_t partial_tec;
    ret = Gauss_Kronrod_integrate_impl(
      &gauss_kronrod_context,
      pContext,
      point_1,
      point_2,
      &partial_tec);
    if (ret != NEQUICK_OK) {
      return ret;
    }

    *pTEC += partial_tec;
  }

  return NEQUICK_OK;
}

int32_t NeQuickG_integrate(
  NeQuickG_context_t* const pContext,
  double_t* const pTEC) {

  int32_t ret;

  if (pContext->ray.is_vertical) {
    ret = ray_vertical_get_profile(pContext);
    if (ret != NEQUICK_OK) {
      return ret;
    }

    const double_t start_height_km =
      max(0.0, pContext->ray.receiver_position.height);
    const double_t end_height_km = pContext->ray.satellite_position.height;
    const double_t lower = min(start_height_km, end_height_km);
    const double_t upper = max(start_height_km, end_height_km);

    double_t nodes[NEQUICK_G_JRC_NODE_MAX_COUNT];
    size_t nodes_count = 0;
    build_vertical_nodes(lower, upper, nodes, &nodes_count);

    return integrate_interval_list(pContext, nodes, nodes_count, pTEC);
  }

  const double_t start_s_km = pContext->ray.slant.receiver_s_km;
  const double_t end_s_km = pContext->ray.slant.satellite_s_km;
  const double_t lower = min(start_s_km, end_s_km);
  const double_t upper = max(start_s_km, end_s_km);

  double_t nodes[NEQUICK_G_JRC_NODE_MAX_COUNT];
  size_t nodes_count = 0;
  build_slant_nodes(&pContext->ray, lower, upper, nodes, &nodes_count);

  return integrate_interval_list(pContext, nodes, nodes_count, pTEC);
}

#undef NEQUICK_G_JRC_NODE_EPSILON
#undef NEQUICK_G_JRC_NODE_MAX_COUNT
#undef NEQUICK_G_JRC_RECURSION_LIMIT_MAX
#undef NEQUICK_G_JRC_INTEGRATION_KRONROD_TOLERANCE_BELOW_FIRST_POINT
#undef NEQUICK_G_JRC_INTEGRATION_KRONROD_TOLERANCE_ABOVE_FIRST_POINT
#undef NEQUICK_G_JRC_INTEGRATION_FIRST_POINT_KM
#undef NEQUICK_G_JRC_INTEGRATION_SECOND_POINT_KM

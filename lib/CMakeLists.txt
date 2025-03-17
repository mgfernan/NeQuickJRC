


set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/CCIR/NeQuickG_JRC_CCIR.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CCIR/NeQuickG_JRC_ccir11.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CCIR/NeQuickG_JRC_ccir12.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CCIR/NeQuickG_JRC_ccir13.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CCIR/NeQuickG_JRC_ccir14.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CCIR/NeQuickG_JRC_ccir15.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CCIR/NeQuickG_JRC_ccir16.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CCIR/NeQuickG_JRC_ccir17.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CCIR/NeQuickG_JRC_ccir18.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CCIR/NeQuickG_JRC_ccir19.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CCIR/NeQuickG_JRC_ccir20.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CCIR/NeQuickG_JRC_ccir21.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CCIR/NeQuickG_JRC_ccir22.c

    ${CMAKE_CURRENT_SOURCE_DIR}/ITU_R_P_371_8.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_coordinates.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_electron_density.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_Gauss_Kronrod_integration.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_geometry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_input_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_interpolate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_iono_E_layer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_iono_F1_layer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_iono_F2_layer_fourier_coefficients.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_iono_F2_layer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_iono_profile.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_math_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_MODIP_grid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_MODIP.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_ray_slant.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_ray_vertical.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_ray.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_solar_activity.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_solar.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_TEC_integration.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_time.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC.c
)
# For usage as a library
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include/ )

add_library(${NEQUICK_LIB_NAME} ${SOURCES})
target_include_directories(${NEQUICK_LIB_NAME} PUBLIC include)
install(TARGETS ${NEQUICK_LIB_NAME} DESTINATION lib)
install(FILES include/NeQuickG_JRC.h DESTINATION include)

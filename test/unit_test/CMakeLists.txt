set(TARGET unit_test)


set(TEST_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_API_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_Az_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_iono_E_layer_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_iono_F1_layer_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_iono_F2_layer_fourier_coefficients_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_iono_F2_layer_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_iono_layer_amplitudes_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_iono_layer_thickness_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_MODIP_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_ray_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_solar_test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/test.c
)

get_filename_component(BASENAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(EXECUTABLE test_${BASENAME}${SUFFIX}.e )
set(TEST_NAME ${BASENAME} )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/include ${CMAKE_CURRENT_SOURCE_DIR}/../../lib)
add_executable (${EXECUTABLE} ${TEST_SOURCE_FILES})

target_link_libraries(${EXECUTABLE} ${NEQUICK_LIB_NAME} m)


add_test(NAME ${TEST_NAME} COMMAND ${EXECUTABLE})


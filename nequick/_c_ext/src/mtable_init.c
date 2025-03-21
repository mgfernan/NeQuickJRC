#include <Python.h>
#include <datetime.h>
#include <structmember.h>

#include "NeQuickG_JRC.h"

// Define the NeQuick object structure
typedef struct {
    PyObject_HEAD
    NeQuickG_handle nequick_handle;
} NeQuickObject;

// __init__ method: Initialize the NeQuick object with 3 coefficients
static int NeQuick_init(NeQuickObject *self, PyObject *args, PyObject *kwds) {

    double a[3] = {0.0, 0.0, 0.0};
    int ret = -1;

    if (!PyArg_ParseTuple(args, "ddd", &a[0], &a[1], &a[2])) {
        goto exit;
    }

    if (NEQUICK_OK != NeQuickG.init(NULL, NULL, &self->nequick_handle)) {
        goto exit;
    }

    if (NEQUICK_OK != NeQuickG.set_solar_activity_coefficients(
        self->nequick_handle, a, 3)) {
        goto exit;
    }

    ret = 0;  // Return 0 on success
exit:
    return ret;
}

// Method to update the coefficients
static PyObject *NeQuick_update_coefficients(NeQuickObject *self, PyObject *args) {

    double a[3] = {0.0, 0.0, 0.0};

    if (!PyArg_ParseTuple(args, "ddd", &a[0], &a[1], &a[2])) {
        return NULL;  // Return NULL on failure
    }

    if (NEQUICK_OK != NeQuickG.set_solar_activity_coefficients(
        self->nequick_handle, a, 3)) {
        return NULL;  // Return NULL on failure
    }

    Py_RETURN_NONE;  // Return None on success
}

// Method to compute STEC based on epoch, station coordinates, and satellite coordinates
static PyObject *NeQuick_compute_stec(NeQuickObject *self, PyObject *args) {

    PyObject *epoch_obj;
    double pos[3], sat_pos[3];
    double stec = NAN;

    if (!PyArg_ParseTuple(args, "Odddddd", &epoch_obj, &pos[0], &pos[1], &pos[2], &sat_pos[0], &sat_pos[1], &sat_pos[2])) {
        return NULL;  // Return NULL on failure
    }

    // Ensure epoch is a datetime object
    if (!PyDateTime_Check(epoch_obj)) {
        PyErr_SetString(PyExc_TypeError, "epoch must be a datetime object");
        return NULL;
    }

    // Extract month and decimal hour from the datetime object
    int month = PyDateTime_GET_MONTH(epoch_obj);
    int hour = PyDateTime_DATE_GET_HOUR(epoch_obj);
    int minute = PyDateTime_DATE_GET_MINUTE(epoch_obj);
    int second = PyDateTime_DATE_GET_SECOND(epoch_obj);
    double decimal_hour = hour + (minute / 60.0) + (second / 3600.0);

    if (NeQuickG.set_time(self->nequick_handle, month, decimal_hour) != NEQUICK_OK) {
        goto exit;
    }
    if (NeQuickG.set_receiver_position(self->nequick_handle, pos[0], pos[1], pos[2]) != NEQUICK_OK) {
        goto exit;
    }
    if (NeQuickG.set_satellite_position(self->nequick_handle, sat_pos[0], sat_pos[1], sat_pos[2]) != NEQUICK_OK) {
        goto exit;
    }
    if (NeQuickG.get_total_electron_content(self->nequick_handle, &stec) != NEQUICK_OK) {
        goto exit;
    }

exit:
    return Py_BuildValue("d", stec);  // Return the computed STEC
}

// Method to compute VTEC based on epoch and coordinates (lat, lon)
static PyObject *NeQuick_compute_vtec(NeQuickObject *self, PyObject *args) {

    static const double STATION_ALT = 0.0;
    static const double SATELLITE_ALT = 25000000;

    PyObject *epoch_obj;
    double station_lat, station_lon;

    // Parse arguments: epoch (datetime object), and coordinates (lon, lat)
    if (!PyArg_ParseTuple(args, "Odd", &epoch_obj, &station_lon, &station_lat)) {
        return NULL;  // Return NULL on failure
    }

    // Use the same logic as compute_stec
    PyObject *args_stec = Py_BuildValue("Odddddd", epoch_obj,
        station_lon, station_lat, STATION_ALT,
        station_lon, station_lat, SATELLITE_ALT);
    if (args_stec == NULL) {
        return NULL;  // Propagate the error if Py_BuildValue fails
    }

    PyObject *stec_result = NeQuick_compute_stec(self, args_stec);
    if (stec_result == NULL) {
        return NULL;  // Propagate the error if compute_stec fails
    }

    // Return the result from compute_stec as the VTEC value
    return stec_result;
}

// Define the methods of the NeQuick class
static PyMethodDef NeQuick_methods[] = {
    {"update_coefficients", (PyCFunction)NeQuick_update_coefficients, METH_VARARGS,
        "Update the coefficients (a0, a1, a2).\n\n"
        ":param a0: first NeQuick coefficient.\n"
        ":param a1: first NeQuick coefficient.\n"
        ":param a2: first NeQuick coefficient.\n\n"
        "Returns:\n"
        "    None"},
    {"compute_stec", (PyCFunction)NeQuick_compute_stec, METH_VARARGS,
        "Compute STEC based on epoch, station coordinates, and satellite coordinates.\n\n"
        "Arguments:\n"
        ":param epoch (datetime.datetime): The epoch for the computation.\n"
        ":param station_lon (float): Latitude of the station in degrees.\n"
        ":param station_lat (float): Longitude of the station in degrees.\n"
        ":param station_alt (float): Altitude of the station in meters.\n"
        ":param sat_lon (float): Latitude of the satellite in degrees.\n"
        ":param sat_lat (float): Longitude of the satellite in degrees.\n"
        ":param sat_alt (float): Altitude of the satellite in meters.\n\n"
        "Returns:\n"
        "    float: The computed Slant Total Electron Content (STEC)."
    },
    {"compute_vtec", (PyCFunction)NeQuick_compute_vtec, METH_VARARGS,
        "Compute VTEC based on epoch and coordinates (lon, lat).\n\n"
        "Arguments:\n"
        ":param epoch (datetime.datetime): The epoch for the computation.\n"
        ":param lon (float): Longitude in degrees.\n"
        ":param lat (float): Latitude in degrees.\n\n"
        "Returns:\n"
        "    float: The computed Vertical Total Electron Content (VTEC)."
    },
    {NULL}  // Sentinel
};

// Define the NeQuick type
static PyTypeObject NeQuickType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "nequick.NeQuick",
    .tp_basicsize = sizeof(NeQuickObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = "NeQuick model",
    .tp_methods = NeQuick_methods,
    .tp_init = (initproc)NeQuick_init,
    .tp_new = PyType_GenericNew,
};

// Module initialization function
static PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    .m_name = "_c_ext",
    .m_doc = "C extension for the NeQuick model",
    .m_size = -1,
};

PyMODINIT_FUNC PyInit__c_ext(void) {
    PyObject *m;

    PyDateTime_IMPORT;

    if (PyType_Ready(&NeQuickType) < 0) {
        return NULL;
    }

    m = PyModule_Create(&moduledef);
    if (m == NULL) {
        return NULL;
    }

    Py_INCREF(&NeQuickType);
    if (PyModule_AddObject(m, "NeQuick", (PyObject *)&NeQuickType) < 0) {
        Py_DECREF(&NeQuickType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}

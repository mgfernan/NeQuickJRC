#include <stddef.h>
#include <Python.h>
#include <datetime.h>
#include <structmember.h>

#include "NeQuickG_JRC.h"

// Define the NeQuick object structure
typedef struct {
    PyObject_HEAD
    NeQuickG_handle nequick_handle;
    PyObject *weakreflist;
} NeQuickObject;


static int NeQuick_configure_ray(
    NeQuickObject *self,
    PyObject *epoch_obj,
    const double *pos,
    const double *sat_pos) {

    int month;
    int hour;
    int minute;
    int second;
    double decimal_hour;

    if (!PyDateTime_Check(epoch_obj)) {
        PyErr_SetString(PyExc_TypeError, "epoch must be a datetime object");
        return -1;
    }

    month = PyDateTime_GET_MONTH(epoch_obj);
    hour = PyDateTime_DATE_GET_HOUR(epoch_obj);
    minute = PyDateTime_DATE_GET_MINUTE(epoch_obj);
    second = PyDateTime_DATE_GET_SECOND(epoch_obj);
    decimal_hour = hour + (minute / 60.0) + (second / 3600.0);

    if (NeQuickG.set_time(self->nequick_handle, month, decimal_hour) != NEQUICK_OK) {
        PyErr_SetString(PyExc_RuntimeError, "failed to set NeQuick time");
        return -1;
    }
    if (NeQuickG.set_receiver_position(self->nequick_handle, pos[0], pos[1], pos[2]) != NEQUICK_OK) {
        PyErr_SetString(PyExc_RuntimeError, "failed to set receiver position");
        return -1;
    }
    if (NeQuickG.set_satellite_position(self->nequick_handle, sat_pos[0], sat_pos[1], sat_pos[2]) != NEQUICK_OK) {
        PyErr_SetString(PyExc_RuntimeError, "failed to set satellite position");
        return -1;
    }

    return 0;
}


static int NeQuick_append_ne_profile_sample(
    PyObject *profile,
    double distance_km,
    double height_meters,
    double electron_density) {

    PyObject *sample = Py_BuildValue("(ddd)", distance_km, height_meters, electron_density);
    int ret;

    if (sample == NULL) {
        return -1;
    }

    ret = PyList_Append(profile, sample);
    Py_DECREF(sample);

    return ret;
}


// __init__ method: Initialize the NeQuick object with 3 coefficients
static int NeQuick_init(NeQuickObject *self, PyObject *args, PyObject *kwds) {

    double a[3] = {0.0, 0.0, 0.0};
    int ret = -1;

    if (!PyArg_ParseTuple(args, "ddd", &a[0], &a[1], &a[2])) {
        goto exit;
    }

    if (NEQUICK_OK != NeQuickG.init(NULL, NULL, &self->nequick_handle)) {
        PyErr_SetString(PyExc_RuntimeError, "failed to initialize NeQuick context");
        goto exit;
    }

    if (NEQUICK_OK != NeQuickG.set_solar_activity_coefficients(
        self->nequick_handle, a, 3)) {
        PyErr_SetString(PyExc_RuntimeError, "failed to set NeQuick solar activity coefficients");
        goto exit;
    }

    self->weakreflist = NULL;

    ret = 0;  // Return 0 on success
exit:
    return ret;
}


static void NeQuick_dealloc(NeQuickObject *self) {
    if (self->weakreflist != NULL) {
        PyObject_ClearWeakRefs((PyObject *)self);
    }

    if (self->nequick_handle != NEQUICKG_INVALID_HANDLE) {
        NeQuickG.close(self->nequick_handle);
        self->nequick_handle = NEQUICKG_INVALID_HANDLE;
    }

    Py_TYPE(self)->tp_free((PyObject *)self);
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

    if (NeQuick_configure_ray(self, epoch_obj, pos, sat_pos) != 0) {
        return NULL;
    }
    if (NeQuickG.get_total_electron_content(self->nequick_handle, &stec) != NEQUICK_OK) {
        PyErr_SetString(PyExc_RuntimeError, "failed to compute STEC for the provided ray geometry");
        return NULL;
    }

    return Py_BuildValue("d", stec);  // Return the computed STEC
}


static PyObject *NeQuick_compute_ne_profile(NeQuickObject *self, PyObject *args) {

    static const double DEFAULT_STEP_KM = 10.0;
    static const double DISTANCE_TOLERANCE_KM = 1.0e-9;

    PyObject *epoch_obj;
    PyObject *profile;
    double pos[3], sat_pos[3];
    double step_km = DEFAULT_STEP_KM;
    double path_length_km = 0.0;
    double distance_km = 0.0;
    double last_sampled_distance_km = NAN;

    if (!PyArg_ParseTuple(
            args,
            "Odddddd|d",
            &epoch_obj,
            &pos[0],
            &pos[1],
            &pos[2],
            &sat_pos[0],
            &sat_pos[1],
            &sat_pos[2],
            &step_km)) {
        return NULL;
    }

    if (step_km <= 0.0) {
        PyErr_SetString(PyExc_ValueError, "step_km must be greater than zero");
        return NULL;
    }

    if (NeQuick_configure_ray(self, epoch_obj, pos, sat_pos) != 0) {
        return NULL;
    }

    if (NeQuickG.get_ray_path_length(self->nequick_handle, &path_length_km) != NEQUICK_OK) {
        PyErr_SetString(PyExc_RuntimeError, "failed to compute ray length for the provided geometry");
        return NULL;
    }

    profile = PyList_New(0);
    if (profile == NULL) {
        return NULL;
    }

    while (distance_km < path_length_km) {
        double height_meters = NAN;
        double electron_density = NAN;

        if (NeQuickG.get_ray_point_electron_density(
                self->nequick_handle,
                distance_km,
                &height_meters,
                &electron_density) != NEQUICK_OK) {
            Py_DECREF(profile);
            PyErr_SetString(PyExc_RuntimeError, "failed to compute electron density profile sample");
            return NULL;
        }

        if (NeQuick_append_ne_profile_sample(profile, distance_km, height_meters, electron_density) != 0) {
            Py_DECREF(profile);
            return NULL;
        }

        last_sampled_distance_km = distance_km;
        distance_km += step_km;
    }

    if (isnan(last_sampled_distance_km) || fabs(last_sampled_distance_km - path_length_km) > DISTANCE_TOLERANCE_KM) {
        double height_meters = NAN;
        double electron_density = NAN;

        if (NeQuickG.get_ray_point_electron_density(
                self->nequick_handle,
                path_length_km,
                &height_meters,
                &electron_density) != NEQUICK_OK) {
            Py_DECREF(profile);
            PyErr_SetString(PyExc_RuntimeError, "failed to compute electron density profile sample");
            return NULL;
        }

        if (NeQuick_append_ne_profile_sample(profile, path_length_km, height_meters, electron_density) != 0) {
            Py_DECREF(profile);
            return NULL;
        }
    }

    return profile;
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
    Py_DECREF(args_stec);
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
    {"compute_ne_profile", (PyCFunction)NeQuick_compute_ne_profile, METH_VARARGS,
        "Compute the electron density profile along the receiver-satellite ray.\n"
        "Vertical and slant geometries are both supported, including occultation-like links.\n\n"
        "Arguments:\n"
        ":param epoch (datetime.datetime): The epoch for the computation.\n"
        ":param station_lon (float): Longitude of the station in degrees.\n"
        ":param station_lat (float): Latitude of the station in degrees.\n"
        ":param station_alt (float): Altitude of the station in meters.\n"
        ":param sat_lon (float): Longitude of the satellite in degrees.\n"
        ":param sat_lat (float): Latitude of the satellite in degrees.\n"
        ":param sat_alt (float): Altitude of the satellite in meters.\n"
        ":param step_km (float, optional): Distance between consecutive samples in km. Defaults to 10 km.\n\n"
        "Returns:\n"
        "    list[tuple[float, float, float]]: Samples as (distance_from_start_km, ellipsoidal_height_m, electron_density_e_per_m3)."
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
    .tp_dealloc = (destructor)NeQuick_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = "NeQuick model",
    .tp_methods = NeQuick_methods,
    .tp_init = (initproc)NeQuick_init,
    .tp_new = PyType_GenericNew,
    .tp_weaklistoffset = offsetof(NeQuickObject, weakreflist),
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

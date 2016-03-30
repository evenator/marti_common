// *****************************************************************************
//
// Copyright (c) 2016, Southwest Research Institute® (SwRI®)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Southwest Research Institute® (SwRI®) nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL SwRI BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// *****************************************************************************

#include <Python.h>
#include <swri_transform_util/earth_constants.h>
#include <swri_transform_util/frames.h>
#include <swri_transform_util/local_xy_util.h>
#include <swri_transform_util/transform_util.h>
#include <tf/transform_datatypes.h>

static PyObject *pModulegeometrymsgs = NULL;

static PyObject *transform_converter(const tf::Transform* transform)
{
  PyObject *pclass, *pargs, *ptransform = NULL;
  pclass = PyObject_GetAttrString(pModulegeometrymsgs, "Transform");
  if (pclass == NULL)
  {
    printf("Can't get geometry_msgs.msg.Transform\n");
    return NULL;
  }

  pargs = Py_BuildValue("()");
  if (pargs == NULL)
  {
    printf("Can't build argument list\n");
    return NULL;
  }

  ptransform = PyEval_CallObject(pclass, pargs);
  Py_DECREF(pclass);
  Py_DECREF(pargs);

  if (ptransform == NULL)
  {
    printf("Can't create class\n");
    return NULL;
  }

  PyObject *ptranslation = PyObject_GetAttrString(ptransform, "translation");
  PyObject_SetAttrString(ptranslation, "x", PyFloat_FromDouble(transform->getOrigin().getX()));
  PyObject_SetAttrString(ptranslation, "y", PyFloat_FromDouble(transform->getOrigin().getY()));
  PyObject_SetAttrString(ptranslation, "z", PyFloat_FromDouble(transform->getOrigin().getZ()));
  Py_DECREF(ptranslation);

  PyObject *protation = PyObject_GetAttrString(ptransform, "rotation");
  PyObject_SetAttrString(protation, "x", PyFloat_FromDouble(transform->getRotation().getX()));
  PyObject_SetAttrString(protation, "y", PyFloat_FromDouble(transform->getRotation().getY()));
  PyObject_SetAttrString(protation, "z", PyFloat_FromDouble(transform->getRotation().getZ()));
  PyObject_SetAttrString(protation, "w", PyFloat_FromDouble(transform->getRotation().getW()));
  Py_DECREF(protation);

  return ptransform;
}

static PyObject* LocalXyFromWgs84(PyObject *self, PyObject *args, PyObject *kw)
{
  static const char *keywords[] = { "latitude", "longitude", "reference_latitude", "reference_longitude", NULL };
  double latitude, longitude, reference_latitude, reference_longitude, x, y;
  if (!PyArg_ParseTupleAndKeywords(args, kw, "dddd", (char**)keywords,
      &latitude,
      &longitude,
      &reference_latitude,
      &reference_longitude))
  {
    return NULL;
  }
  swri_transform_util::LocalXyFromWgs84(
      latitude, longitude,
      reference_latitude, reference_longitude,
      x, y);
  return Py_BuildValue("(dd)", x, y);
}

static PyObject* GreatCircleDistance(PyObject *self, PyObject *args, PyObject *kw)
{
  static const char *keywords[] = { "src_latitude", "src_longitude", "dst_latitude", "dst_longitude", NULL };
  double src_latitude, src_longitude, dst_latitude, dst_longitude;
  if (!PyArg_ParseTupleAndKeywords(args, kw, "dddd", (char**)keywords,
      &src_latitude,
      &src_longitude,
      &dst_latitude,
      &dst_longitude))
  {
    return NULL;
  }
  return PyFloat_FromDouble(swri_transform_util::GreatCircleDistance(
      src_latitude,
      src_longitude,
      dst_latitude,
      dst_longitude));
}

static PyObject* Wgs84FromLocalXy(PyObject *self, PyObject *args, PyObject *kw)
{
  static const char *keywords[] = { "x", "y", "reference_latitude", "reference_longitude", NULL };
  double latitude, longitude, reference_latitude, reference_longitude, x, y;
  if (!PyArg_ParseTupleAndKeywords(args, kw, "dddd", (char**)keywords,
      &x,
      &y,
      &reference_latitude,
      &reference_longitude))
  {
    return NULL;
  }
  swri_transform_util::Wgs84FromLocalXy(
      x, y,
      reference_latitude, reference_longitude,
      latitude, longitude);
  return Py_BuildValue("(dd)", latitude, longitude);
}

static PyObject* GetRelativeTransform(PyObject *self, PyObject *args, PyObject *kw)
{
  static const char* keywords[] = {
      "latitude",
      "longitude",
      "yaw",
      "reference_latitude",
      "reference_longitude",
      "reference_yaw"
  };
  double latitude, longitude, yaw;
  double reference_latitude, reference_longitude, reference_yaw;
  if (!PyArg_ParseTupleAndKeywords(args, kw, "dddddd", (char**)keywords,
      &latitude,
      &longitude,
      &yaw,
      &reference_latitude,
      &reference_longitude,
      &reference_yaw))
  {
    return NULL;
  }
  tf::Transform transform = swri_transform_util::GetRelativeTransform(
      latitude, longitude, yaw,
      reference_latitude, reference_longitude, reference_yaw);
  return Py_BuildValue("O&", transform_converter, &transform);
}

static PyMethodDef module_methods[] = {
    // Methods from local_xy_util.h
    { "LocalXyFromWgs84", (PyCFunction)LocalXyFromWgs84, METH_VARARGS, NULL },
    { "Wgs84FromLocalXy", (PyCFunction)Wgs84FromLocalXy, METH_VARARGS, NULL },
    // Methods from transform_util.h
    { "GetRelativeTransform", (PyCFunction)GetRelativeTransform, METH_VARARGS, NULL },
     { "GreatCircleDistance", (PyCFunction)GreatCircleDistance, METH_VARARGS, NULL },
    /*
     { "GetBearing", (PyCFunction)GetBearing, METH_VARARGS, NULL },
     { "GetHeading", (PyCFunction)GetMidpointLatLon, METH_VARARGS, NULL },
     { "ToHeading", (PyCFunction)GetMidpointLatLon, METH_VARARGS, NULL },
     { "ToYaw", (PyCFunction)GetMidpointLatLon, METH_VARARGS, NULL },
     { "SnapToRightAngle", (PyCFunction)SnapToRightAngle, METH_VARARGS, NULL },
     { "GetPrimaryAxis", (PyCFunction)GetPrimaryAxis, METH_VARARGS, NULL },
     { "IsRotation", (PyCFunction)IsRotation, METH_VARARGS, NULL },
     { "GetUpperLeft", (PyCFunction)GetUpperLeft, METH_VARARGS, NULL },
     { "GetLowerRight", (PyCFunction)GetLowerRight, METH_VARARGS, NULL },
     { "Get3x3Cov", (PyCFunction)Get3x3Cov, METH_VARARGS, NULL },
     { "Set3x3Cov", (PyCFunction)Set3x3Cov, METH_VARARGS, NULL },
     { "SetUpperLeft", (PyCFunction)SetUpperLeft, METH_VARARGS, NULL },
     { "SetLowerRight", (PyCFunction)SetLowerRight, METH_VARARGS, NULL },
     { "LongitudeDegreesFromMeters", (PyCFunction)LongitudeDegreesFromMeters, METH_VARARGS, NULL },
     { "LatitudeDegreesFromMeters", (PyCFunction)LatitudeDegreesFromMeters, METH_VARARGS, NULL },
     */
    { NULL, NULL, 0, NULL },
};

extern "C" void init_swri_transform_util()
{
  PyObject *m = Py_InitModule("_swri_transform_util", module_methods);
  pModulegeometrymsgs = PyImport_ImportModule("geometry_msgs.msg");

  // Constants from earth_constants.h
  PyModule_AddObject(m,
      "earth_equator_radius",
      PyFloat_FromDouble(swri_transform_util::_earth_equator_radius));
  PyModule_AddObject(m,
      "earth_mean_radius",
      PyFloat_FromDouble(swri_transform_util::_earth_mean_radius));
  PyModule_AddObject(m,
      "earth_eccentricity",
      PyFloat_FromDouble(swri_transform_util::_earth_eccentricity));
  PyModule_AddObject(m,
      "earth_flattening",
      PyFloat_FromDouble(swri_transform_util::_earth_flattening));
  PyModule_AddObject(m,
      "earth_rotation_rate",
      PyFloat_FromDouble(swri_transform_util::_earth_rotation_rate));

  // Constants from frames.h
  PyModule_AddStringConstant(m,
      "wgs_84_frame",
      swri_transform_util::_wgs84_frame.c_str());
  PyModule_AddStringConstant(m,
      "utm_frame",
      swri_transform_util::_utm_frame.c_str());
  PyModule_AddStringConstant(m,
      "local_xy_frame",
      swri_transform_util::_local_xy_frame.c_str());
  PyModule_AddStringConstant(m,
      "tf_frame",
      swri_transform_util::_wgs84_frame.c_str());
  PyModule_AddStringConstant(m,
      "wgs_84_frame",
      swri_transform_util::_tf_frame.c_str());
}

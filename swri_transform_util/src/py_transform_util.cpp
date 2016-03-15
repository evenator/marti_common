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
// ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// *****************************************************************************

#include <Python.h>
#include <swri_transform_util/local_xy_util.h>

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

static PyMethodDef module_methods[] = {
    { "LocalXyFromWgs84", (PyCFunction)LocalXyFromWgs84, METH_VARARGS, NULL },
    { "Wgs84FromLocalXy", (PyCFunction)Wgs84FromLocalXy, METH_VARARGS, NULL },
  {NULL, NULL, 0, NULL},
};

extern "C" void init_swri_transform_util()
{
  PyObject *m = Py_InitModule("_swri_transform_util", module_methods);
}

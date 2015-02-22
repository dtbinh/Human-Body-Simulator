/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file gameengine/Converter/BL_ArmatureMuscle.cpp
 *  \ingroup bgeconv
 */


#include "DNA_armature_types.h"
#include "BL_ArmatureMuscle.h"
#include "BL_ArmatureObject.h"
//#include "BL_ArmatureConstraint.h"
#include "BLI_math.h"
#include "BLI_string.h"
#include <stddef.h>

#ifdef WITH_PYTHON

PyTypeObject BL_ArmatureMuscle::Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"BL_ArmatureMuscle",
	sizeof(PyObjectPlus_Proxy),
	0,
	py_base_dealloc,
	0,
	0,
	0,
	0,
	py_base_repr,
	0,0,0,0,0,0,0,0,0,
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	0,0,0,0,0,0,0,
	Methods,
	0,
	0,
	&CValue::Type,
	0,0,0,0,0,0,
	py_base_new
};

PyObject *BL_ArmatureMuscle::py_repr(void)
{
    return PyUnicode_FromString(m_musclechannel->name);
}

PyObject *BL_ArmatureMuscle::GetProxy()
{
    return GetProxyPlus_Ext(this, &Type, m_musclechannel);
}

PyObject *BL_ArmatureMuscle::NewProxy(bool py_owns)
{
    return NewProxyPlus_Ext(this, &Type, m_musclechannel, py_owns);
}

#endif // WITH_PYTHON

BL_ArmatureMuscle::BL_ArmatureMuscle(
    BL_ArmatureObject *armature,
    bPoseChannel *musclechannel)
    : PyObjectPlus(), m_musclechannel(musclechannel), m_armature(armature)
{
}

BL_ArmatureMuscle::~BL_ArmatureMuscle()
{
}

#ifdef WITH_PYTHON

// PYTHON

PyMethodDef BL_ArmatureMuscle::Methods[] = {
    {NULL,NULL} // Sentinel
};

// order of definition of attributes, must match Attributes[] array
#define BCA_MUSCLE      0
#define BCA_PARENT      1

PyAttributeDef BL_ArmatureMuscle::Attributes[] = {
    // Keep these attributes in order of BCA_ defines!!! used by py_attr_getattr and py_attr_setattr
    KX_PYATTRIBUTE_RO_FUNCTION("muscle", BL_ArmatureMuscle, py_attr_getattr),
    KX_PYATTRIBUTE_RO_FUNCTION("parent", BL_ArmatureMuscle, py_attr_getattr),
    
    { NULL }    // Sentinel
};

/* Attributes taken directly from bPoseChannel */
PyAttributeDef BL_ArmatureMuscle::AttributesPtr[] = {
    KX_PYATTRIBUTE_CHAR_RO("name", bPoseChannel, name),
    KX_PYATTRIBUTE_FLOAT_VECTOR_RW("location", -FLT_MAX, FLT_MAX, bPoseChannel, loc, 3),
    KX_PYATTRIBUTE_FLOAT_VECTOR_RW("scale", -FLT_MAX, FLT_MAX, bPoseChannel, size, 3),
    KX_PYATTRIBUTE_FLOAT_VECTOR_RW("rotation_quaternion", -1.0f, 1.0f, bPoseChannel, quat, 4),
    KX_PYATTRIBUTE_FLOAT_VECTOR_RW("rotation_euler", -10.f, 10.f, bPoseChannel, eul, 3),
    KX_PYATTRIBUTE_SHORT_RW("rotation_mode", ROT_MODE_MIN, ROT_MODE_MAX, false, bPoseChannel, rotmode),
    KX_PYATTRIBUTE_FLOAT_MATRIX_RO("channel_matrix", bPoseChannel, chan_mat, 4),
    KX_PYATTRIBUTE_FLOAT_MATRIX_RO("pose_matrix", bPoseChannel, pose_mat, 4),
    KX_PYATTRIBUTE_FLOAT_VECTOR_RO("pose_head", bPoseChannel, pose_head, 3),
    KX_PYATTRIBUTE_FLOAT_VECTOR_RO("pose_tail", bPoseChannel, pose_tail, 3),
    { NULL }    //Sentinel
};

PyObject *BL_ArmatureMuscle::py_attr_getattr(void *self_v, const struct KX_PYATTRIBUTE_DEF *attrdef)
{
    BL_ArmatureMuscle* self = static_cast<BL_ArmatureMuscle*>(self_v);
    bPoseChannel* channel = self->m_musclechannel;
    int attr_order = attrdef-Attributes;
    
    if (!channel) {
        PyErr_SetString(PyExc_AttributeError, "muscle is NULL");
        return NULL;
    }
    
    switch (attr_order) {
    case BCA_MUSCLE:
        return NewProxyPlus_Ext(NULL, &BL_ArmatureBone::Type, channel->bone, false);
    case BCA_PARENT:
        {
            BL_ArmatureMuscle* parent = self->m_armature->GetMuscle(channel->parent);
            if (parent)
                return parent->GetProxy();
            else
                Py_RETURN_NONE;
        }
    }
    PyErr_SetString(PyExc_AttributeError, "muscle unknown attribute");
    return NULL;
}

int BL_ArmatureMuscle::py_attr_setattr(void *self_v, const struct KX_PYATTRIBUTE_DEF *attrdef, PyObject *value)
{
    BL_ArmatureMuscle* self = static_cast<BL_ArmatureMuscle*>(self_v);
    bPoseChannel* channel = self->m_musclechannel;
    int attr_order = attrdef-Attributes;
    
    if (!channel) {
        PyErr_SetString(PyExc_AttributeError, "muscle is NULL");
        return PY_SET_ATTR_FAIL;
    }
    
    switch(attr_order) {
    default:
        break;
    }
    
    PyErr_SetString(PyExc_AttributeError, "muscle unknown attribute");
    return PY_SET_ATTR_FAIL;
}

#endif // WITH_PYTHON

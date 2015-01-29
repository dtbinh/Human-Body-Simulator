
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
 * Contributor(s): Rohan Smith
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file gameengine/Converter/BL_MuscleChannel.cpp
 *  \ingroup bgeconv
 */

#include "BLI_math.h"

#include "DNA_armature_types.h"
#include "BL_MuscleChannel.h"
#include "BL_ArmatureObject.h"
#include <stddef.h>

#ifdef WITH_PYTHON

PyTypeObject BL_MuscleChannel::Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "BL_MuscleChannel",
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

PyObject *BL_MuscleChannel::py_repr(void)
{
    return PyUnicode_FromString(m_musclechannel->name);
}

PyObject *BL_MuscleChannel::GetProxy()
{
    return GetProxyPlus_Ext(this, &Type, m_musclechannel);
}

PyObject *BL_MuscleChannel::NewProxy(bool py_owns)
{
    return NewProxyPlus_Ext(this, &Type, m_musclechannel, py_owns);
}

#endif // WITH_PYTHON

BL_MuscleChannel::BL_MuscleChannel(
    BL_ArmatureObject *armature,
    bMuscleChannel *musclechannel)
    : PyObjectPlus(), m_musclechannel(musclechannel), m_armature(armature),
    lref(1.), lopt(0.2), lrest(0.4), w(0.4 * lopt), lce(lopt), lse(lrest-lopt),
    lm(lce+lse), N(1.5), K(5), Fmax(22e3), a1(2.55), a2(0.), a6(2.6), b2(tanh(a1*(1+a2))-tanh(a1*a2)),
    b1((1-tanh(a1*(1+a2)))/b2), q((1-tanh(a1*(1+a2)))/(1-tanh(a1*a2))), q0(q), m(11.25), c(1.373e-6)
{
    sub_v3_v3v3(p_origDiff, musclechannel->pose_tail, musclechannel->pose_head);
}

BL_MuscleChannel::~BL_MuscleChannel()
{
}

//double BL_MuscleChannel::Fse(double l, double zeta)
//{
//    delta = (l + lambda_bar * (1 - zeta) - (1 - zeta_bar) * l_bar) / (zeta_bar * l_bar);
//    return F_bar * sin(theta_bar) * ((exp(sigma * delta) - 1) / (exp(sigma) - 1));
//}
//
//double BL_MuscleChannel::Q0(double zeta)
//{
//    return (5.33e4 * pow(1 - pow(zeta_hat / zeta, s), 0.5)) / pow(1 - pow(zeta_hat, s), 0.5);
//}
//
//double BL_MuscleChannel::Q(double zeta)
//{
//    return (6.62e4 * pow(1 - pow(zeta_hat / zeta, s), 0.5)) / pow(1 - pow(zeta_hat, s), 0.5);
//}
//double BL_MuscleChannel::q(double zeta, double gamma)
//{
//    return (q0 + pow(Q(zeta) * gamma, 2)) / (1 + pow(Q(zeta) * gamma, 2));
//}
//
//double BL_MuscleChannel::k(double zeta)
//{
//    return exp(-a6 * (zeta - 1));
//}
//
//double BL_MuscleChannel::sintheta(double zeta)
//{
//    return 1.; // More complicated expression, but only doing fusiform muscles atm, so 1
//}
//
//// Not equivalent to copysign in cmath or signf in BLI_math
//// sgn returns 1 for val > 0, -1 for val < 0, and 0 for val == 0
//double BL_MuscleChannel::sgn(double val)
//{
//    return (0. < val) - (val < .0);
//}

// Force-velocity relationship, looks kind of like a hyperbolic tangent
// Input is velocity of contractile element
double BL_MuscleChannel::fv(double nyu)
{
    if(nyu < 0)
        return (nyu_bar - nyu) / (nyu_bar + K * nyu);

    return N + (N - 1) * (nyu_bar + nyu) / (7.56 * K * nyu - nyu_bar);
}

// Force-length relationship, looks like a bell curve
// Input is length of contractile element
double BL_MuscleChannel::fl(double lce)
{
    // Magic numbers, whoo
    return exp(pow(log(0.05) * abs((lce - lopt) / (lopt * w)), 3));
}

// Calculates force in elastic element, works for PE and SE
double BL_MuscleChannel::fee(double lee)
{
    return pow((lee - lrest) / lrest, 2);
}

double BL_MuscleChannel::h(double zeta)
{
    if(zeta < 0.56) // Should be zeta_hat afaik
        return zeta = 0.56;

    return zeta = sqrt((1-0.56/zeta));
}

void BL_MuscleChannel::updateControlSystem(double curtime)
{
    // Update timestep and frametime
    // Can't help but think that over very long timeframes,
    // This'll start becoming inaccurate
    m_timestep = curtime - m_lastframe;
    m_lastframe = curtime;

    // Update control system of muscle channel
    // and update positional data based on control system
    // Control system is described in:
    // A Myocybenetic Control Model of Skeletal Muscle (Hatze, 1977)
    // A General Myocybernetic Control Model of Skeletal Muscle (Hatze, 1988)
    // Flexible muscle-based locomotion for bipedal creatures (Geijtenbeek et al, 2013)
    f_v = fv(nyu);
    f_l = fl(lce);
    Fce = q * Fmax * f_l * f_v;
    Fse = fee(lm - lce);
    Fpe = fee(lce);

    lce_dot = (1./f_v) * ((Fse - Fpe) / Fce);
    lce = lce + lce_dot * m_timestep;

    zeta_old = zeta;
    zeta = lce / lopt;

    nyu = (zeta - zeta_old) * lopt / -nyu_bar;

    gamma_dot = m * (c * v - gamma);
    gamma = gamma + gamma_dot * m_timestep;
    q = 1. - (1. - q0) * (2.14 * exp(-1.1167e7 * h(zeta_old) * gamma_old) - 1.14 * exp(-2.096e7 * h(zeta_old) * gamma_old));

    nyu = 0.;

//    epsilon = ((exp(c_bar * n) - 1) * q(zeta, psi) + (exp(c_bar * n + c_bar * r) - exp(c_bar * n) * q(zeta, phi)) + (exp(c_bar) - exp(c_bar * n + c_bar * r)) * q0) / (exp(c_bar) - 1);
//
//    n_dot = n_dot_hat * z;
//
//    r_dot = -n_dot_hat * z * ((r - w_minus - delta_bar)/(r + delta_bar)) - (1 + w_minus) * (m * r) / (log(1 + 1e-3 * m + (phi) / (k2 * c)));
//
//    psi_dot = m * (c * v - psi) + w_plus * z * c_bar * n_dot_hat * ((1 - exp(Q(zeta) * (psi - phi)) / (Q(zeta) * (1 - exp(-c_bar * n - delta_bar))))) - (1 + w_minus) * phi * m;
//
//    phi_dot = -phi * m - w_minus * (m * ((c * v)/(psi + delta_bar)) - 1) * phi - z * c_bar * n_dot_hat * ((1 - exp(Q0(zeta) * (phi - psi))) / (Q0(zeta) * (exp(c_bar * r + delta_bar) - 1)));
//
//    zeta_dot = (-lambda0_dot / lambda_bar) * (a2 * (1./a1) * atanh(((Fse(l, zeta) / (F_bar * sintheta(zeta))) + b1 * exp(-a6 * (zeta - 1)) * b2) / (epsilon * k(zeta)) - 1));
}

#ifdef WITH_PYTHON

// PYTHON

PyMethodDef BL_MuscleChannel::Methods[] = {
    {NULL,NULL} // Sentinel
};

// Order of attributes, must match Attributes[] array
#define BCA_MUSCLE      0
#define BCA_PARENT      1

PyAttributeDef BL_MuscleChannel::Attributes[] = {
    // Keep these attributes in order of BCA_ defines, used by py_attr_getattr and py_attr_setattr
    KX_PYATTRIBUTE_RO_FUNCTION("muscle", BL_MuscleChannel, py_attr_getattr),
    KX_PYATTRIBUTE_RO_FUNCTION("parent", BL_MuscleChannel, py_attr_getattr),

    KX_PYATTRIBUTE_FLOAT_VECTOR_RO("original_difference", BL_MuscleChannel, p_origDiff, 3),
    KX_PYATTRIBUTE_FLOAT_VECTOR_RO("current_difference", BL_MuscleChannel, p_diff, 3),

    { NULL } // Sentinel
};

/* Attributes directly taken from bMuscleChannel */
PyAttributeDef BL_MuscleChannel::AttributesPtr[] = {
    KX_PYATTRIBUTE_CHAR_RO("name", bMuscleChannel, name),
    KX_PYATTRIBUTE_FLOAT_VECTOR_RW("location",-FLT_MAX,FLT_MAX,bMuscleChannel,loc,3),
    KX_PYATTRIBUTE_FLOAT_VECTOR_RW("scale",-FLT_MAX,FLT_MAX,bMuscleChannel,size,3),
    KX_PYATTRIBUTE_FLOAT_VECTOR_RW("rotation_quaternion",-1.0f,1.0f,bMuscleChannel,quat,4),
    KX_PYATTRIBUTE_FLOAT_VECTOR_RW("rotation_euler",-10.f,10.f,bMuscleChannel,eul,3),
    KX_PYATTRIBUTE_FLOAT_MATRIX_RO("channel_matrix",bMuscleChannel,chan_mat,4),
    KX_PYATTRIBUTE_FLOAT_MATRIX_RO("pose_matrix",bMuscleChannel,pose_mat,4),
    KX_PYATTRIBUTE_FLOAT_VECTOR_RO("pose_head",bMuscleChannel,pose_head,3),
    KX_PYATTRIBUTE_FLOAT_VECTOR_RO("pose_tail",bMuscleChannel,pose_tail,3),

    { NULL } // Sentinel
};

PyObject *BL_MuscleChannel::py_attr_getattr(void *self_v, const struct KX_PYATTRIBUTE_DEF *attrdef)
{
    BL_MuscleChannel* self = static_cast<BL_MuscleChannel*>(self_v);
    bMuscleChannel* channel = self->m_musclechannel;
    int attr_order = attrdef-Attributes;

    if (!channel) {
        PyErr_SetString(PyExc_AttributeError, "channel is NULL");
        return NULL;
    }

    switch (attr_order) {
    case BCA_MUSCLE:
        // muscles are standalone proxy
        return NewProxyPlus_Ext(NULL,&BL_ArmatureMuscle::Type,channel->muscle,false);
    case BCA_PARENT:
        {
            BL_MuscleChannel* parent = self->m_armature->GetMuscle(channel->parent);
            if (parent)
                return parent->GetProxy();
            else
                Py_RETURN_NONE;
        }
    }
    PyErr_SetString(PyExc_AttributeError, "channel unknown attribute");
    return NULL;
}

int BL_MuscleChannel::py_attr_setattr(void *self_v, const struct KX_PYATTRIBUTE_DEF *attrdef, PyObject *value)
{
    BL_MuscleChannel *self = static_cast<BL_MuscleChannel*>(self_v);
    bMuscleChannel* channel = self->m_musclechannel;
    int attr_order = attrdef-Attributes;

    if (!channel) {
        PyErr_SetString(PyExc_AttributeError, "channel is NULL");
        return PY_SET_ATTR_FAIL;
    }

    switch (attr_order) {
    default:
        break;
    }

    PyErr_SetString(PyExc_AttributeError, "channel unknown attribute");
    return PY_SET_ATTR_FAIL;
}

// *************************
// BL_ArmatureMuscle
//
// Access to Muscle structure
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
    py_muscle_repr,
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

// not used
PyObject *BL_ArmatureMuscle::GetProxy()
{
    return NULL;
}
PyObject *BL_ArmatureMuscle::NewProxy(bool py_owns)
{
    return NULL;
}

PyObject *BL_ArmatureMuscle::py_muscle_repr(PyObject *self)
{
    Muscle* muscle = static_cast<Muscle*>BGE_PROXY_PTR(self);
    return PyUnicode_FromString(muscle->name);
}

PyMethodDef BL_ArmatureMuscle::Methods[] = {
    {NULL,NULL} // Sentinel
};

// no attributes since never instantiated
PyAttributeDef BL_ArmatureMuscle::Attributes[] = {
    { NULL }    // Sentinel
};

// attributes that work on proxy ptr
PyAttributeDef BL_ArmatureMuscle::AttributesPtr[] = {
    KX_PYATTRIBUTE_CHAR_RO("name",Muscle,name),
    KX_PYATTRIBUTE_FLAG_RO("connected",Muscle,flag, MUSCLE_CONNECTED),
    KX_PYATTRIBUTE_FLOAT_RO("roll",Muscle,roll),
    KX_PYATTRIBUTE_FLOAT_VECTOR_RO("head",Muscle,head,3),
    KX_PYATTRIBUTE_FLOAT_VECTOR_RO("tail",Muscle,tail,3),
    KX_PYATTRIBUTE_FLOAT_RO("length",Muscle,length),
    KX_PYATTRIBUTE_FLOAT_VECTOR_RO("arm_head",Muscle,arm_head,3),
    KX_PYATTRIBUTE_FLOAT_VECTOR_RO("arm_tail",Muscle,arm_tail,3),
    KX_PYATTRIBUTE_FLOAT_MATRIX_RO("arm_mat",Muscle,arm_mat,4),
    KX_PYATTRIBUTE_FLOAT_MATRIX_RO("muscle_mat",Muscle,muscle_mat,3),
    KX_PYATTRIBUTE_RO_FUNCTION("parent",BL_ArmatureMuscle,py_muscle_get_parent),
    KX_PYATTRIBUTE_RO_FUNCTION("children",BL_ArmatureMuscle,py_muscle_get_children),
    { NULL } // Sentinel
};

PyObject *BL_ArmatureMuscle::py_muscle_get_parent(void *self, const struct KX_PYATTRIBUTE_DEF *attrdef)
{
    Muscle* muscle = reinterpret_cast<Muscle*>(self);
    if (muscle->parent) {
        //
        return NewProxyPlus_Ext(NULL,&Type,muscle->parent,false);
    }
    Py_RETURN_NONE;
}

PyObject *BL_ArmatureMuscle::py_muscle_get_children(void *self, const struct KX_PYATTRIBUTE_DEF *attrdef)
{
    Muscle* muscle = reinterpret_cast<Muscle*>(self);
    Muscle* child;
    int count = 0;
    for (child = (Muscle *)muscle->childbase.first; child; child = child->next)
        count++;

    PyObject *childrenlist = PyList_New(count);

    for (count = 0, child = (Muscle *)muscle->childbase.first; child; child = child->next)
        PyList_SET_ITEM(childrenlist,count,NewProxyPlus_Ext(NULL,&Type,child,false));

    return childrenlist;
}

#endif // WITH_PYTHON

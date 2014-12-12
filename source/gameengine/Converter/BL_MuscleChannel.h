
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

/** \file BL_ArmatureMuscle.h
 *  \ingroup bgeconv
 */

#ifndef __BL_MUSCLECHANNEL_H__
#define __BL_MUSCLECHANNEL_H__

#include <cmath>

#include "DNA_action_types.h"
#include "CTR_HashedPtr.h"
#include "CTR_Map.h"
#include "PyObjectPlus.h"

class SCA_IObject;
class KX_GameObject;
class BL_ArmatureObject;
struct bPoseChannel;
struct Object;
struct bPose;

class BL_MuscleChannel : public PyObjectPlus
{
    // use Py_HeaderPtr because I have nfi what I'm doing and am copying off BL_ArmatureChannel
    Py_HeaderPtr;

private:
    friend class BL_ArmatureObject;

    struct bMuscleChannel* m_musclechannel; // The original muscle data such as muscles it's joined to etc
    BL_ArmatureObject* m_armature; // Armature the muscle is attached to

    double m_lastframe;
    double m_timestep;

    float p_origDiff[3];
    float p_diff[3];

    // Constants required for control system
//    double m; // 11.25 fast, 3.67 slow
//    double c; // 1.373e-4
//    double c_bar;
//    double zeta_0; // Extension of elastic element at rest
//    double zeta_bar; // Maximum extension of elastic element at F == F_bar, 0.05-0.15
//    double zeta_hat;
//    double lambda_bar; // Extension of contractile element at F == F_bar
//    double lambda0_dot;
//
//    // Muscle specific properties
//    double sigma; // 1.3 to 3.5
//    double l_bar; // Length of muscle when F == F_bar
//    double delta_bar; // 10e-8, to avoid divide by zero
//    double a1; // 1.6 to 2.7 fast, 2.9 to 3.8 slow
//    double a2; // -0.05 to 0.1 fast, -0.01 to 0.1 slow
//    double a6; // 2.6 for mammalian muscle
//    double b1; // (1 - tanh(a1 * (1 + a2))) / b2
//    double b2; // tanh(a1 * (1 + a2)) - tanh(a1 * a2)
//    double s; // 1, blame biomech people
//    double epsilon; // Excitation function
//    double q0; // Initial value for q, active state of muscle fibre at rest
//
//    double k2; // Arbitrary constant, 10e-4
//
//    // Control variables, need a way to set these
//    double z; // Normalised recruitment rate, -z_hat to 1
//    double v; // Relative stimulation rate, 0 to 1
//
//    double F_bar; // Maximum force produced by contractile element
//    double theta_bar; // Angle between perpendicular of muscle longitudinal axis and muscle fibre
//    double delta; // Used in calculation of Fse
//
//    double l; // Instantaneous length of entire muscle
//
//    // w values, either 0 or 1
//    double w_plus;
//    double w_minus;
//    double w;
//
//    // state variables
//    double n; // Normalised number of stimulated motor units
//    double n_dot; // Rate of motor unit recruitment
//    double n_dot_hat; // Maximum rate of motor unit recruitment
//    double r; // Normalised number of semi-active motor units
//    double r_dot;
//    double psi; // Calcium concentration in active motor units
//    double psi_dot;
//    double phi; // Calcium concentration in semi-active motor units
//    double phi_dot;
//    double zeta; // Normalised length of muscle
//    double zeta_dot;

    double lref;
    double lopt;
    double lrest;
    double w;
    double lce;
    double lce_dot;
    double lse;
    double lm;
    double N;
    double K;
    double Fmax;
    double a1;
    double a2;
    double a6;
    double b2;
    double b1;
    double q;
    double q0;
    double m;
    double c;
    double v;

    double f_v;
    double f_l;
    double Fce;
    double Fse;
    double Fpe;

    double nyu;
    double nyu_bar;
    double zeta;
    double zeta_old;
    double gamma_dot;
    double gamma;
    double gamma_old;

    // various other variables


    double fv(double nyu);
    double fl(double lce);
    double fee(double lee);
    double h(double zeta);
//    double Fse(double l, double zeta); // Calculate the force in the series elastic element
//    double sintheta(double zeta); // Returns 1 atm
//    double Q0(double zeta);
//    double Q(double zeta);
//
//    double q(double zeta, double gamma); // Calculates active state of muscle fibre
//    double k(double zeta);
//    double sgn(double val); // signum function, copysign and signf don't properly implement it

public:
    BL_MuscleChannel(class BL_ArmatureObject *armature,
                     struct bMuscleChannel *musclechannel);
    virtual ~BL_MuscleChannel();

    void updateControlSystem(double curtime);

#ifdef WITH_PYTHON
    // Python access
    virtual PyObject *py_repr(void);

    static PyObject *py_attr_getattr(void *self, const struct KX_PYATTRIBUTE_DEF *attrdef);
    static int py_attr_setattr(void *self, const struct KX_PYATTRIBUTE_DEF *attrdef, PyObject *value);
#endif /* WITH_PYTHON */
};

class BL_ArmatureMuscle : public PyObjectPlus
{
    Py_HeaderPtr;
private:
    BL_ArmatureMuscle() {}
    virtual ~BL_ArmatureMuscle() {}

public:

#ifdef WITH_PYTHON
    static PyObject *py_muscle_repr(PyObject *self);
    static PyObject *py_muscle_get_parent(void *self, const struct KX_PYATTRIBUTE_DEF *attrdef);
    static PyObject *py_muscle_get_children(void *self, const struct KX_PYATTRIBUTE_DEF *attrdef);
#endif
};

#endif // __BL_MUSCLECHANNEL_H__

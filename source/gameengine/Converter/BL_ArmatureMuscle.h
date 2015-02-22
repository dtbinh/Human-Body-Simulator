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

/** \file BL_ArmatureMuscle.h
 *  \ingroup bgeconv
 */

#ifndef __BL_ARMATUREMUSCLE_H__
#define __BL_ARMATUREMUSCLE_H__

#include "DNA_action_types.h"
#include "CTR_HashedPtr.h"
#include "CTR_Map.h"
#include "PyObjectPlus.h"

class SCA_IObject;
class KX_GameObject;
class BL_ArmatureObject;
struct bConstraint;
struct bPoseChannel;
struct Object;
struct bPose;

class BL_ArmatureMuscle : public PyObjectPlus
{
    // use Py_HeaderPtr since we use generic pointer in proxy
    Py_HeaderPtr;
    
private:
    friend class BL_ArmatureObject;
    struct bPoseChannel* m_musclechannel;
    BL_ArmatureObject* m_armature;
    
public:
    BL_ArmatureMuscle(class BL_ArmatureObject *armature,
                       struct bPoseChannel *posechannel);
    virtual ~BL_ArmatureMuscle();
    
#ifdef WITH_PYTHON
    // Python access
    virtual PyObject *py_repr(void);
    
    static PyObject *py_attr_getattr(void *self, const struct KX_PYATTRIBUTE_DEF *attrdef);
    static int py_attr_setattr(void *self, const struct KX_PYATTRIBUTE_DEF *attrdef, PyObject *value);
#endif /* WITH_PYTHON */
};

#endif /* __BL_ARMATUREMUSCLE_H__ */

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
 * Contributor(s): Blender Foundation, 2002-2009 full recode.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file blender/editors/armature/muscle_utils.c
 *  \ingroup edarmature
 */

#include "DNA_armature_types.h"
#include "DNA_object_types.h"

#include "MEM_guardedalloc.h"

#include "BLI_blenlib.h"
#include "BLI_math.h"

#include "BKE_armature.h"
#include "BKE_context.h"
#include "BKE_deform.h"
#include "BKE_depsgraph.h"
#include "BKE_global.h"
#include "BKE_idprop.h"
#include "BKE_main.h"

#include "ED_armature.h"
#include "ED_util.h"

#include "armature_intern.h"

/* emuscle stuff here */
void muscle_free(bArmature *arm, EditMuscle *muscle)
{
    if (arm->act_edmuscle == muscle)
        arm->act_edmuscle = NULL;

    if (muscle->prop) {
        IDP_FreeProperty(muscle->prop);
        MEM_freeN(muscle->prop);
    }

    BLI_freelinkN(arm->edmu, muscle);
}

void ED_armature_emuscle_to_mat3(EditMuscle *emuscle, float mat[3][3])
{
    float delta[3];

    sub_v3_v3v3(delta, emuscle->tail, emuscle->head);
    vec_roll_to_mat3(delta, emuscle->roll, mat);
}

void ED_armature_emuscle_to_mat4(EditMuscle *emuscle, float mat[4][4])
{
    float m3[3][3];

    ED_armature_emuscle_to_mat3(emuscle, m3);

    copy_m4_m3(mat, m3);
    copy_v3_v3(mat[3], emuscle->head);
}

EditMuscle *make_muscleList(ListBase *edmu, ListBase *muscles, EditMuscle *parent, Muscle *actMuscle)
{
    EditMuscle  *eMuscle = NULL;
    EditMuscle  *eMuscleAct = NULL;
    EditMuscle  *eMuscleTest = NULL;
    Muscle      *curMuscle;

    for (curMuscle = muscles->first; curMuscle; curMuscle = curMuscle->next) {
        eMuscle = MEM_callocN(sizeof(EditMuscle), "make_editmuscle");

        eMuscle->parent = parent;
        BLI_strncpy(eMuscle->name, curMuscle->name, sizeof(eMuscle->name));
        eMuscle->flag = eMuscle->flag;

        if (eMuscle->flag & MUSCLE_SELECTED) {
            eMuscle->flag |= MUSCLE_TIPSEL;
            if (eMuscle->parent && (eMuscle->flag & MUSCLE_CONNECTED)) {
                eMuscle->parent->flag |= MUSCLE_TIPSEL;
                eMuscle->flag &= ~MUSCLE_ROOTSEL;
            }
            else {
                eMuscle->flag |= MUSCLE_ROOTSEL;
            }
        }
        else {
            if (eMuscle->parent && (eMuscle->flag & MUSCLE_CONNECTED)) {
                eMuscle->flag &= ~MUSCLE_ROOTSEL;
            }
        }

        copy_v3_v3(eMuscle->head, curMuscle->arm_head);
        copy_v3_v3(eMuscle->tail, curMuscle->arm_tail);

        eMuscle->length = curMuscle->length;

        eMuscle->rad_head = curMuscle->rad_head;
        eMuscle->rad_tail = curMuscle->rad_tail;

        eMuscle->layer = curMuscle->layer;

        BLI_addtail(edmu, eMuscle);

        if (curMuscle->childbase.first) {
            eMuscleTest = make_muscleList(edmu, &curMuscle->childbase, eMuscle, actMuscle);
            if (eMuscleTest)
                eMuscleAct = eMuscleTest;
        }

        if (curMuscle == actMuscle)
            eMuscleAct = eMuscle;
    }

    return eMuscleAct;
}

int ED_armature_emuscle_selectflag_get(const EditMuscle *emuscle)
{
    if (emuscle->parent && (emuscle->flag & MUSCLE_SELECTED)) {
        return ((emuscle->flag & (MUSCLE_SELECTED | MUSCLE_TIPSEL)) |
                ((emuscle->parent->flag & MUSCLE_TIPSEL) ? MUSCLE_ROOTSEL : 0));
    }
    else {
        return (emuscle->flag & (MUSCLE_SELECTED | MUSCLE_ROOTSEL | MUSCLE_TIPSEL));
    }
}

void ED_armature_emuscle_selectflag_set(EditMuscle *emuscle, int flag)
{
    flag = flag & (MUSCLE_SELECTED | MUSCLE_ROOTSEL | MUSCLE_TIPSEL);

    if (emuscle->parent && (emuscle->flag & MUSCLE_CONNECTED)) {
        emuscle->flag &= ~(MUSCLE_SELECTED | MUSCLE_ROOTSEL | MUSCLE_TIPSEL);
        emuscle->parent->flag &= ~BONE_TIPSEL;

        emuscle->flag |= flag;
        emuscle->parent->flag |= (flag & MUSCLE_ROOTSEL) ? MUSCLE_TIPSEL : 0;
    }
    else {
        emuscle->flag &= ~(MUSCLE_SELECTED | MUSCLE_ROOTSEL | MUSCLE_TIPSEL);
        emuscle->flag |= flag;
    }
}

void ED_armature_emuscle_selectflag_enable(EditMuscle *emuscle, int flag)
{
    BLI_assert((flag & (MUSCLE_SELECTED | MUSCLE_ROOTSEL | MUSCLE_TIPSEL)) != 0);
    ED_armature_emuscle_selectflag_set(emuscle, emuscle->flag | flag);
}

void ED_armature_emuscle_selectflag_disable(EditMuscle *emuscle, int flag)
{
    BLI_assert((flag & (MUSCLE_SELECTED | MUSCLE_ROOTSEL | MUSCLE_TIPSEL)) != 0);
    ED_armature_emuscle_selectflag_set(emuscle, emuscle->flag & ~flag);
}

void ED_armature_emuscle_select_set(EditMuscle *emuscle, bool select)
{
    int flag;
    if (select) {
        BLI_assert((emuscle->flag & MUSCLE_UNSELECTABLE) == 0);
        flag = (MUSCLE_SELECTED | MUSCLE_TIPSEL | MUSCLE_ROOTSEL);
    }
    else {
        flag = 0;
    }
    ED_armature_emuscle_selectflag_set(emuscle, flag);
}

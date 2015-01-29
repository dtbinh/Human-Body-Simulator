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
 *
 * API's and Operators for selecting armature bones in EditMode
 */

/** \file blender/editors/armature/muscle_select.c
 *  \ingroup edarmature
 */

#include "DNA_armature_types.h"
#include "DNA_object_types.h"
#include "DNA_scene_types.h"

#include "BLI_blenlib.h"
#include "BLI_math.h"

#include "BKE_context.h"
#include "BKE_deform.h"
#include "BKE_report.h"

#include "BIF_gl.h"

#include "RNA_access.h"
#include "RNA_define.h"

#include "WM_api.h"
#include "WM_types.h"

#include "ED_armature.h"
#include "ED_screen.h"
#include "ED_view3d.h"

#include "armature_intern.h"

#define EMUSCLE_PREV_FLAG_GET(emuscle) ((void)0, (GET_INT_FROM_POINTER((emuscle)->temp)))
#define EMUSCLE_PREV_FLAG_SET(emuscle, val) ((emuscle)->temp = SET_INT_IN_POINTER(val))

Muscle *get_indexed_muscle(Object *ob, int index)
{
    bMuscleChannel *pmuscle;
    if (ob->pose == NULL) return NULL;
    index >>= 16;

    pmuscle = BLI_findlink(&ob->pose->musclebase, index);
    return pmuscle ? pmuscle->muscle : NULL;
}

void *get_muscle_from_selectbuffer(Scene *scene, Base *base, unsigned int *buffer, short hits, short findunsel)
{
    Object *obedit = scene->obedit;
    Muscle *muscle;
    EditMuscle *emuscle;
    void *firstunSel = NULL, *firstSel = NULL, *data;
    unsigned int hitresult;
    short i;
    bool takeNext = false;

    for (i = 0; i < hits; i++) {
        hitresult = buffer[3 + (i * 4)];

        if (!(hitresult & MUSCLESEL_NOSEL)) {
            if (hitresult & MUSCLESEL_ANY) {
                bool sel;

                hitresult &= ~(MUSCLESEL_ANY);

                if (obedit == NULL || base->object != obedit) {
                    if (base->selcol == (hitresult & 0xFFFF)) {
                        muscle = get_indexed_muscle(base->object, hitresult);

                        if (findunsel)
                            sel = (muscle->flag & MUSCLE_SELECTED);
                        else
                            sel = !(muscle->flag & MUSCLE_SELECTED);

                        data = muscle;
                    }
                    else {
                        data = NULL;
                        sel = 0;
                    }
                }
                else {
                    bArmature *arm = obedit->data;

                    emuscle = BLI_findlink(arm->edmu, hitresult);
                    if (findunsel)
                        sel = (emuscle->flag & MUSCLE_SELECTED);
                    else
                        sel = !(emuscle->flag & MUSCLE_SELECTED);

                    data = emuscle;
                }

                if (data) {
                    if (sel) {
                        if (!firstSel) firstSel = data;
                        takeNext = 1;
                    }
                    else {
                        if (!firstunSel)
                            firstunSel = data;
                        if (takeNext)
                            return data;
                    }
                }
            }
        }
    }

    if (firstunSel)
        return firstunSel;
    else
        return firstSel;
}

void *get_nearest_muscle(bContext *C, short findunsel, int x, int y)
{
    ViewContext vc;
    rcti rect;
    unsigned int buffer[MAXPICKBUF];
    short hits;

    view3d_set_viewcontext(C, &vc);

    rect.xmin = rect.xmax = x;
    rect.ymin = rect.ymax = y;

    glInitNames();
    hits = view3d_opengl_select(&vc, buffer, MAXPICKBUF, &rect, true);

    if (hits > 0)
        return get_muscle_from_selectbuffer(vc.scene, vc.scene->basact, buffer, hits, findunsel);

    return NULL;
}

static void muscle_select_more(bArmature *arm, EditMuscle *emuscle)
{
    if ((EMUSCLE_PREV_FLAG_GET(emuscle) & (MUSCLE_ROOTSEL | MUSCLE_TIPSEL)) != 0) {
        if (EMUSCLE_SELECTABLE(arm, emuscle)) {
            ED_armature_emuscle_select_set(emuscle, true);
        }
    }
}

static void muscle_select_less(bArmature *UNUSED(arm), EditMuscle *emuscle)
{
    if ((EMUSCLE_PREV_FLAG_GET(emuscle) & (MUSCLE_ROOTSEL | MUSCLE_TIPSEL)) != (MUSCLE_ROOTSEL | MUSCLE_TIPSEL)) {
        ED_armature_emuscle_select_set(emuscle, false);
    }
}

static void muscle_select_more_less(Object *ob, bool more)
{
    bArmature *arm = (bArmature *)ob->data;
    EditMuscle *emuscle;

    for (emuscle = arm->edmu->first; emuscle; emuscle = emuscle->next) {
        // TODO: fix this up - EBONE_PREV_FLAG_SET(ebone, ED_armature_ebone_selectflag_get(ebone));
    }

    for (emuscle = arm->edmu->first; emuscle; emuscle = emuscle->next) {
        if (EMUSCLE_VISIBLE(arm, emuscle)) {
            if (more) {
                muscle_select_more(arm, emuscle);
            }
            else {
                muscle_select_less(arm, emuscle);
            }
        }
    }

    for (emuscle = arm->edmu->first; emuscle; emuscle = emuscle->next) {
        if (EMUSCLE_VISIBLE(arm, emuscle)) {
            if (more == false) {
                if (emuscle->flag & MUSCLE_SELECTED) {
                    ED_armature_emuscle_select_set(emuscle, true);
                }
            }
        }
        emuscle->temp = NULL;
    }

    ED_muscle_sync_selection(arm->edmu);
}

static int muscle_de_select_more_exec(bContext *C, wmOperator *UNUSED(op))
{
    Object *obedit = CTX_data_edit_object(C);
    muscle_select_more_less(obedit, true);
    WM_event_add_notifier(C, NC_OBJECT | ND_BONE_SELECT, obedit);

    return OPERATOR_FINISHED;
}

void MUSCLE_OT_select_more(wmOperatorType *ot)
{
    /* identifiers */
    ot->name = "Select More";
    ot->idname = "MUSCLE_OT_select_more";
    ot->description = "Select those bones connected to the initial selection";

    /* api callbacks */
    ot->exec = muscle_de_select_more_exec;
    ot->poll = ED_operator_editarmature;

    /* flags */
    ot->flag = OPTYPE_REGISTER | OPTYPE_UNDO;
}

static int muscle_de_select_less_exec(bContext *C, wmOperator *UNUSED(op))
{
    Object *obedit = CTX_data_edit_object(C);
    muscle_select_more_less(obedit, false);
    WM_event_add_notifier(C, NC_OBJECT | ND_BONE_SELECT, obedit);

    return OPERATOR_FINISHED;
}

void MUSCLE_OT_select_less(wmOperatorType *ot)
{
    /* identifiers */
    ot->name = "Select Less";
    ot->idname = "MUSCLE_OT_select_less";
    ot->description = "Deselect those bones at the boundary of each selection region";

    /* api callbacks */
    ot->exec = muscle_de_select_less_exec;
    ot->poll = ED_operator_editarmature;

    /* flags */
    ot->flag = OPTYPE_REGISTER | OPTYPE_UNDO;
}

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
 * Contributor(s): Rohan Smith
 *
 * ***** END GPL LICENSE BLOCK *****
 *
 * Operators and API's for creating bones
 */

 /** \file blender/editors/armature/muscle_add.c
  *  \ingroup edarmature
  */

#include <stdio.h>

#include "DNA_armature_types.h"
#include "DNA_object_types.h"

#include "MEM_guardedalloc.h"

#include "BLI_blenlib.h"
#include "BLI_math.h"

#include "BKE_context.h"

#include "WM_api.h"
#include "WM_types.h"

#include "ED_armature.h"
#include "ED_screen.h"
#include "ED_view3d.h"

#include "RNA_access.h"
#include "RNA_define.h"

#include "armature_intern.h"


/* *************** Adding stuff in editmode *************** */

EditMuscle *ED_armature_edit_muscle_add(bArmature *arm, const char *name)
{
    EditMuscle *muscle = MEM_callocN(sizeof(EditMuscle), "eMuscle");
//    EditMuscle *temp;

    BLI_strncpy(muscle->name, name, sizeof(muscle->name));
    /* Should have a unique name generator here */

    BLI_addtail(arm->edmu, muscle);
//    temp = arm->edmu->first;

    muscle->flag |= MUSCLE_TIPSEL;
    /* All the default settings go here */
    muscle->rad_head = 0.10f;
    muscle->rad_tail = 0.05f;
    muscle->layer = arm->layer;

    return muscle;
}

static int armature_muscle_primitive_add_exec(bContext *C, wmOperator *op)
{
    // Basically a copy of armature_bone_primitive_add_exec
    RegionView3D *rv3d = CTX_wm_region_view3d(C);
    Object *obedit = CTX_data_edit_object(C);
    bArmature *arm = obedit->data;
    EditMuscle *muscle;
//    EditMuscle *temp;
    float obmat[3][3], curs[3], viewmat[3][3], totmat[3][3], imat[3][3];

    char name[MAXMUSCLENAME];

    RNA_string_get(op->ptr, "name", name);

    copy_v3_v3(curs, ED_view3d_cursor3d_get(CTX_data_scene(C), CTX_wm_view3d(C)));

    /* Get inverse point for head and orientation for tail */
    invert_m4_m4(obedit->imat, obedit->obmat);
    mul_m4_v3(obedit->imat, curs);

    if (rv3d && (U.flag & USER_ADD_VIEWALIGNED))
        copy_m3_m4(obmat, rv3d->viewmat);
    else unit_m3(obmat);

    copy_m3_m4(viewmat, obedit->obmat);
    mul_m3_m3m3(totmat, obmat, viewmat);
    invert_m3_m3(imat, totmat);

    ED_armature_deselect_all(obedit, 0);

    /*  Create a muscle  */
    muscle = ED_armature_edit_muscle_add(obedit->data, name);
//    temp = arm->edmu->first;

    copy_v3_v3(muscle->head, curs);

    if (rv3d && (U.flag & USER_ADD_VIEWALIGNED))
		add_v3_v3v3(muscle->tail, muscle->head, imat[1]);   // bone with unit length 1
	else
		add_v3_v3v3(muscle->tail, muscle->head, imat[2]);   // bone with unit length 1, pointing up Z

	/* note, notifier might evolve */
	WM_event_add_notifier(C, NC_OBJECT | ND_BONE_SELECT, obedit);

    return OPERATOR_FINISHED;
}

void MUSCLE_OT_muscle_primitive_add(wmOperatorType *ot)
{
    /* identifiers */
    ot->name = "Add Muscle";
    ot->idname = "MUSCLE_OT_muscle_primitive_add";
    ot->description = "Add a new muscle located at the 3D-Cursor";

    /* api callbacks */
    ot->exec = armature_muscle_primitive_add_exec;
    ot->poll = ED_operator_editarmature;

    /*flags */
    ot->flag = OPTYPE_REGISTER | OPTYPE_UNDO;

    RNA_def_string(ot->srna, "name", "Muscle", MAXMUSCLENAME, "Name", "Name of the newly created muscle");
}


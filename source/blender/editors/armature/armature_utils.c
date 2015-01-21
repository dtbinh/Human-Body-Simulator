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

/** \file blender/editors/armature/armature_utils.c
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

/* *************************************************************** */
/* Validation */

/* Sync selection to parent for connected children */
void ED_armature_sync_selection(ListBase *edbo)
{
	EditArmatureElement *ebo;

	for (ebo = edbo->first; ebo; ebo = ebo->next) {
		/* if bone is not selectable, we shouldn't alter this setting... */
		if ((ebo->flag & ELEMENT_UNSELECTABLE) == 0) {
			if ((ebo->flag & ELEMENT_CONNECTED) && (ebo->parent)) {
				if (ebo->parent->flag & ELEMENT_TIPSEL)
					ebo->flag |= ELEMENT_ROOTSEL;
				else
					ebo->flag &= ~ELEMENT_ROOTSEL;
			}

			if ((ebo->flag & ELEMENT_TIPSEL) && (ebo->flag & ELEMENT_ROOTSEL))
				ebo->flag |= ELEMENT_SELECTED;
			else
				ebo->flag &= ~ELEMENT_SELECTED;
		}
	}
}

void ED_armature_validate_active(struct bArmature *arm)
{
	EditArmatureElement *ebone = arm->act_edelement;

	if (ebone) {
		if (ebone->flag & ELEMENT_HIDDEN_A)
			arm->act_edelement = NULL;
	}
}

/* *************************************************************** */
/* Bone Operations */

/* XXX bone_looper is only to be used when we want to access settings
 * (i.e. editability/visibility/selected) that context doesn't offer */
int bone_looper(Object *ob, Bone *bone, void *data,
                int (*bone_func)(Object *, Bone *, void *))
{
	/* We want to apply the function bone_func to every bone
	 * in an armature -- feed bone_looper the first bone and
	 * a pointer to the bone_func and watch it go!. The int count
	 * can be useful for counting bones with a certain property
	 * (e.g. skinnable)
	 */
	int count = 0;

	if (bone) {
		/* only do bone_func if the bone is non null */
		count += bone_func(ob, bone, data);

		/* try to execute bone_func for the first child */
		count += bone_looper(ob, bone->childbase.first, data, bone_func);

		/* try to execute bone_func for the next bone at this
		 * depth of the recursion.
		 */
		count += bone_looper(ob, bone->next, data, bone_func);
	}

	return count;
}

/* *************************************************************** */
/* Bone Removal */

void element_free(bArmature *arm, EditArmatureElement *bone)
{
	if (arm->act_edelement == bone)
		arm->act_edelement = NULL;

	if (bone->prop) {
		IDP_FreeProperty(bone->prop);
		MEM_freeN(bone->prop);
	}

	BLI_freelinkN(arm->edbo, bone);
}

void ED_armature_edit_element_remove(bArmature *arm, EditArmatureElement *exBone)
{
	EditArmatureElement *curBone;

	/* Find any bones that refer to this bone */
	for (curBone = arm->edbo->first; curBone; curBone = curBone->next) {
		if (curBone->parent == exBone) {
			curBone->parent = exBone->parent;
			curBone->flag &= ~ELEMENT_CONNECTED;
		}
	}

	element_free(arm, exBone);
}

bool ED_armature_ebone_is_child_recursive(EditArmatureElement *ebone_parent, EditArmatureElement *ebone_child)
{
	for (ebone_child = ebone_child->parent; ebone_child; ebone_child = ebone_child->parent) {
		if (ebone_child == ebone_parent)
			return true;
	}
	return false;
}

/**
 * Finds the first parent shared by \a ebone_child
 *
 * \param ebone_child  Children bones to search
 * \param ebone_child_tot  Size of the ebone_child array
 * \return The shared parent or NULL.
 */
EditArmatureElement *ED_armature_bone_find_shared_parent(EditArmatureElement *ebone_child[], const unsigned int ebone_child_tot)
{
	unsigned int i;
	EditArmatureElement *ebone_iter;

#define EELEMENT_TEMP_UINT(ebone) (*((unsigned int *)(&((ebone)->temp))))

	/* clear all */
	for (i = 0; i < ebone_child_tot; i++) {
		for (ebone_iter = ebone_child[i]; ebone_iter; ebone_iter = ebone_iter->parent) {
			EELEMENT_TEMP_UINT(ebone_iter) = 0;
		}
	}

	/* accumulate */
	for (i = 0; i < ebone_child_tot; i++) {
		ebone_iter = ebone_child[i];
		for (ebone_iter = ebone_child[i]->parent; ebone_iter; ebone_iter = ebone_iter->parent) {
			EELEMENT_TEMP_UINT(ebone_iter) += 1;
		}
	}

	/* only need search the first chain */
	for (ebone_iter = ebone_child[0]->parent; ebone_iter; ebone_iter = ebone_iter->parent) {
		if (EELEMENT_TEMP_UINT(ebone_iter) == ebone_child_tot) {
			return ebone_iter;
		}
	}

#undef EELEMENT_TEMP_UINT

	return NULL;
}

void ED_armature_eelement_to_mat3(EditArmatureElement *eelement, float mat[3][3])
{
    float delta[3];

    sub_v3_v3v3(delta, eelement->tail, eelement->head);
    vec_roll_to_mat3(delta, eelement->roll, mat);
}

void ED_armature_eelement_to_mat4(EditArmatureElement *eelement, float mat[4][4])
{
    float m3[3][3];

    ED_armature_eelement_to_mat3(eelement, m3);

    copy_m4_m3(mat, m3);
    copy_v3_v3(mat[3], eelement->head);
}

void ED_armature_eelement_from_mat3(EditArmatureElement *eelement, float mat[3][3])
{
    float vec[3], roll;
	const float len = len_v3v3(eelement->head, eelement->tail);

	mat3_to_vec_roll(mat, vec, &roll);

	madd_v3_v3v3fl(eelement->tail, eelement->head, vec, len);
	eelement->roll = roll;
}

void ED_armature_eelement_from_mat4(EditArmatureElement *eelement, float mat[4][4])
{
	float mat3[3][3];

	copy_m3_m4(mat3, mat);
	/* We want normalized matrix here, to be consistent with ebone_to_mat. */
	BLI_ASSERT_UNIT_M3(mat3);

	sub_v3_v3(eelement->tail, eelement->head);
	copy_v3_v3(eelement->head, mat[3]);
	add_v3_v3(eelement->tail, mat[3]);
	ED_armature_eelement_from_mat3(eelement, mat3);
}

/**
 * Return a pointer to the bone of the given name
 */
EditArmatureElement *ED_armature_armatureelement_find_name(const ListBase *edbo, const char *name)
{
	return BLI_findstring(edbo, name, offsetof(EditArmatureElement, name));
}


/* *************************************************************** */
/* Mirroring */

/**
 * \see #BKE_pose_channel_get_mirrored (pose-mode, matching function)
 */
EditArmatureElement *ED_armature_bone_get_mirrored(const ListBase *edbo, EditArmatureElement *eel)
{
	char name_flip[MAXBONENAME];

	if (eel == NULL)
		return NULL;

	BKE_deform_flip_side_name(name_flip, eel->name, false);

	if (!STREQ(name_flip, eel->name)) {
		return ED_armature_armatureelement_find_name(edbo, name_flip);
	}

	return NULL;
}

/* ------------------------------------- */

/* helper function for tools to work on mirrored parts.
 * it leaves mirrored bones selected then too, which is a good indication of what happened */
void armature_select_mirrored(bArmature *arm)
{
	/* Select mirrored bones */
	if (arm->flag & ARM_MIRROR_EDIT) {
		EditArmatureElement *curBone, *ebone_mirr;

		for (curBone = arm->edbo->first; curBone; curBone = curBone->next) {
			if (arm->layer & curBone->layer) {
				if (curBone->flag & ELEMENT_SELECTED) {
					ebone_mirr = ED_armature_bone_get_mirrored(arm->edbo, curBone);
					if (ebone_mirr)
						ebone_mirr->flag |= ELEMENT_SELECTED;
				}
			}
		}
	}

}

void armature_tag_select_mirrored(bArmature *arm)
{
	EditArmatureElement *curBone;

	/* always untag */
	for (curBone = arm->edbo->first; curBone; curBone = curBone->next) {
		curBone->flag &= ~ELEMENT_DONE;
	}

	/* Select mirrored bones */
	if (arm->flag & ARM_MIRROR_EDIT) {
		for (curBone = arm->edbo->first; curBone; curBone = curBone->next) {
			if (arm->layer & curBone->layer) {
				if (curBone->flag & (ELEMENT_SELECTED | ELEMENT_ROOTSEL | ELEMENT_TIPSEL)) {
					EditArmatureElement *ebone_mirr = ED_armature_bone_get_mirrored(arm->edbo, curBone);
					if (ebone_mirr && (ebone_mirr->flag & ELEMENT_SELECTED) == 0) {
						ebone_mirr->flag |= ELEMENT_DONE;
					}
				}
			}
		}

		for (curBone = arm->edbo->first; curBone; curBone = curBone->next) {
			if (curBone->flag & ELEMENT_DONE) {
				EditArmatureElement *ebone_mirr = ED_armature_bone_get_mirrored(arm->edbo, curBone);
				curBone->flag |= ebone_mirr->flag & (ELEMENT_SELECTED | ELEMENT_ROOTSEL | ELEMENT_TIPSEL);
			}
		}
	}
}

/* only works when tagged */
void armature_tag_unselect(bArmature *arm)
{
	EditArmatureElement *curBone;

	for (curBone = arm->edbo->first; curBone; curBone = curBone->next) {
		if (curBone->flag & ELEMENT_DONE) {
			curBone->flag &= ~(ELEMENT_SELECTED | ELEMENT_ROOTSEL | ELEMENT_TIPSEL | ELEMENT_DONE);
		}
	}
}

/* ------------------------------------- */

/* if editbone (partial) selected, copy data */
/* context; editmode armature, with mirror editing enabled */
void transform_armature_mirror_update(Object *obedit)
{
	bArmature *arm = obedit->data;
	EditArmatureElement *ebo, *eboflip;

	for (ebo = arm->edbo->first; ebo; ebo = ebo->next) {
		/* no layer check, correct mirror is more important */
		if (ebo->flag & (ELEMENT_TIPSEL | ELEMENT_ROOTSEL)) {
			eboflip = ED_armature_bone_get_mirrored(arm->edbo, ebo);

			if (eboflip) {
				/* we assume X-axis flipping for now */
				if (ebo->flag & ELEMENT_TIPSEL) {
					EditArmatureElement *children;

					eboflip->tail[0] = -ebo->tail[0];
					eboflip->tail[1] = ebo->tail[1];
					eboflip->tail[2] = ebo->tail[2];
					eboflip->rad_tail = ebo->rad_tail;
					eboflip->roll = -ebo->roll;

					/* Also move connected children, in case children's name aren't mirrored properly */
					for (children = arm->edbo->first; children; children = children->next) {
						if (children->parent == eboflip && children->flag & ELEMENT_CONNECTED) {
							copy_v3_v3(children->head, eboflip->tail);
							children->rad_head = ebo->rad_tail;
						}
					}
				}
				if (ebo->flag & ELEMENT_ROOTSEL) {
					eboflip->head[0] = -ebo->head[0];
					eboflip->head[1] = ebo->head[1];
					eboflip->head[2] = ebo->head[2];
					eboflip->rad_head = ebo->rad_head;
					eboflip->roll = -ebo->roll;

					/* Also move connected parent, in case parent's name isn't mirrored properly */
					if (eboflip->parent && eboflip->flag & ELEMENT_CONNECTED) {
						EditArmatureElement *parent = eboflip->parent;
						copy_v3_v3(parent->tail, eboflip->head);
						parent->rad_tail = ebo->rad_head;
					}
				}
				if (ebo->flag & ELEMENT_SELECTED) {
					((EditBoneElement*)eboflip->data)->dist = ((EditBoneElement*)ebo->data)->dist;
					eboflip->roll = -ebo->roll;
					eboflip->xwidth = ebo->xwidth;
					eboflip->zwidth = ebo->zwidth;
				}
			}
		}
	}
}

/* *************************************************************** */
/* Armature EditMode Conversions */

/* converts Bones to EditBone list, used for tools as well */
EditArmatureElement *make_elementList(ListBase *edList, ListBase *elements, EditArmatureElement *parent, ArmatureElement *actElement)
{
    EditArmatureElement *eElement;
    EditArmatureElement *eElementAct = NULL;
    EditArmatureElement *eElementTest = NULL;
    ArmatureElement     *curElement;

	for (curElement = elements->first; curElement; curElement = curElement->next) {
        eElement = MEM_callocN(sizeof(EditArmatureElement), "make_editbone");

		/*	Copy relevant data from bone to eBone */
		eElement->parent = parent;
		BLI_strncpy(eElement->name, curElement->name, sizeof(eElement->name));
		eElement->flag = curElement->flag;

		/* fix selection flags */
		if (eElement->flag & ELEMENT_SELECTED) {
			/* if the bone is selected the copy its root selection to the parents tip */
			eElement->flag |= ELEMENT_TIPSEL;
			if (eElement->parent && (eElement->flag & ELEMENT_CONNECTED)) {
				eElement->parent->flag |= ELEMENT_TIPSEL;
				eElement->flag &= ~ELEMENT_ROOTSEL; /* this is ignored when there is a connected parent, so unset it */
			}
			else {
				eElement->flag |= ELEMENT_ROOTSEL;
			}
		}
		else {
			/* if the bone is not selected, but connected to its parent
			 * always use the parents tip selection state */
			if (eElement->parent && (eElement->flag & ELEMENT_CONNECTED)) {
				eElement->flag &= ~ELEMENT_ROOTSEL;
			}
		}

		copy_v3_v3(eElement->head, curElement->arm_head);
		copy_v3_v3(eElement->tail, curElement->arm_tail);
		eElement->type = curElement->type;
		eElement->roll = curElement->arm_roll;

		/* rest of stuff copy */
		eElement->length = curElement->length;
		eElement->xwidth = curElement->xwidth;
		eElement->zwidth = curElement->zwidth;
		eElement->rad_head = curElement->rad_head;
		eElement->rad_tail = curElement->rad_tail;
		eElement->segments = curElement->segments;
		eElement->layer = curElement->layer;

		switch(eElement->type)
		{
                case(AE_BONE):
                    eElement->data = MEM_callocN(sizeof(EditBoneElement), "make_editbonedata");
                    ((EditBoneElement*)eElement->data)->dist = ((BoneData*)curElement->data)->dist;
                    ((EditBoneElement*)eElement->data)->weight = ((BoneData*)curElement->data)->weight;
                    ((EditBoneElement*)eElement->data)->ease1 = ((BoneData*)curElement->data)->ease1;
                    ((EditBoneElement*)eElement->data)->ease2 = ((BoneData*)curElement->data)->ease2;
                    break;
                case(AE_MUSCLE):
                    break;
		}

		if (curElement->prop)
			eElement->prop = IDP_CopyProperty(curElement->prop);

		BLI_addtail(edList, eElement);

		/*	Add children if necessary */
		if (curElement->childbase.first) {
			eElementTest = make_elementList(edList, &curElement->childbase, eElement, actElement);
			if (eElementTest)
				eElementAct = eElementTest;
		}

		if (curElement == actElement)
			eElementAct = eElement;
	}

	return eElementAct;
}

/* nasty stuff for converting roll in editbones into bones */
/* also sets restposition in armature (arm_mat) */
static void fix_bonelist_roll(ListBase *bonelist, ListBase *editbonelist)
{
	ArmatureElement *curBone;
	EditArmatureElement *ebone;
	float premat[3][3];
	float postmat[3][3];
	float difmat[3][3];
	float imat[3][3];

	for (curBone = bonelist->first; curBone; curBone = curBone->next) {
		/* sets local matrix and arm_mat (restpos) */
		BKE_armature_where_is_bone(curBone, curBone->parent);

		/* Find the associated editbone */
		for (ebone = editbonelist->first; ebone; ebone = ebone->next)
			if ((ArmatureElement *)ebone->temp == curBone)
				break;

		if (ebone) {
			/* Get the ebone premat */
			ED_armature_eelement_to_mat3(ebone, premat);

			/* Get the bone postmat */
			copy_m3_m4(postmat, curBone->arm_mat);

			invert_m3_m3(imat, premat);
			mul_m3_m3m3(difmat, imat, postmat);
#if 0
			printf("Bone %s\n", curBone->name);
			print_m4("premat", premat);
			print_m4("postmat", postmat);
			print_m4("difmat", difmat);
			printf("Roll = %f\n",  RAD2DEGF(-atan2(difmat[2][0], difmat[2][2])));
#endif
			curBone->roll = -atan2f(difmat[2][0], difmat[2][2]);

			/* and set restposition again */
			BKE_armature_where_is_bone(curBone, curBone->parent);
		}
		fix_bonelist_roll(&curBone->childbase, editbonelist);
	}
}

/* put EditMode back in Object */
void ED_armature_from_edit(bArmature *arm)
{
	EditArmatureElement *eElem, *neElem;
	ArmatureElement *newElem;
	Object *obt;

	/* armature bones */
    BKE_armature_elementlist_free(&arm->bonebase);
    BKE_armature_elementlist_free(&arm->musclebase);
    arm->act_element = NULL;

    BKE_armature_elementlist_free(&arm->elementbase);
    arm->act_element = NULL;

	/* remove zero sized bones, this gives unstable restposes */
	for (eElem = arm->edbo->first; eElem; eElem = neElem) {
		float len = len_v3v3(eElem->head, eElem->tail);
		neElem = eElem->next;
		if (len <= 0.000001f) {  /* FLT_EPSILON is too large? */
			EditArmatureElement *fBone;

			/*	Find any bones that refer to this bone	*/
			for (fBone = arm->edbo->first; fBone; fBone = fBone->next) {
				if (fBone->parent == eElem)
					fBone->parent = eElem->parent;
			}
			if (G.debug & G_DEBUG)
				printf("Warning: removed zero sized bone: %s\n", eElem->name);
			element_free(arm, eElem);
		}
	}

	/*	Copy the bones from the editData into the armature */
	for (eElem = arm->edbo->first; eElem; eElem = eElem->next) {
		newElem = MEM_callocN(sizeof(ArmatureElement), "bone");
		newElem->data = MEM_callocN(sizeof(BoneData), "bonedata");
		eElem->temp = newElem;   /* Associate the real Bones with the EditBones */

		BLI_strncpy(newElem->name, eElem->name, sizeof(newElem->name));
		copy_v3_v3(newElem->arm_head, eElem->head);
		copy_v3_v3(newElem->arm_tail, eElem->tail);
		newElem->arm_roll = eElem->roll;

		newElem->flag = eElem->flag;

		if (eElem == arm->act_edelement) {
			/* don't change active selection, this messes up separate which uses
			 * editmode toggle and can separate active bone which is de-selected originally */
			/* newElem->flag |= ELEMENT_SELECTED; */ /* important, editbones can be active with only 1 point selected */
			arm->act_element = newElem;
		}
		newElem->roll = 0.0f;

		((BoneData*)newElem->data)->weight = ((EditBoneElement*)eElem->data)->weight;
		((BoneData*)newElem->data)->dist = ((EditBoneElement*)eElem->data)->dist;

		newElem->xwidth = eElem->xwidth;
		newElem->zwidth = eElem->zwidth;
		((BoneData*)newElem->data)->ease1 = ((EditBoneElement*)eElem->data)->ease1;
		((BoneData*)newElem->data)->ease2 = ((EditBoneElement*)eElem->data)->ease2;
		newElem->rad_head = eElem->rad_head;
		newElem->rad_tail = eElem->rad_tail;
		newElem->segments = eElem->segments;
		newElem->layer = eElem->layer;

		if (eElem->prop)
			newElem->prop = IDP_CopyProperty(eElem->prop);
	}

	/* Fix parenting in a separate pass to ensure ebone->bone connections
	 * are valid at this point */
	for (eElem = arm->edbo->first; eElem; eElem = eElem->next) {
		newElem = (ArmatureElement *)eElem->temp;
		if (eElem->parent) {
			newElem->parent = (ArmatureElement *)eElem->parent->temp;
			BLI_addtail(&newElem->parent->childbase, newElem);

			{
				float M_parentRest[3][3];
				float iM_parentRest[3][3];

				/* Get the parent's  matrix (rotation only) */
				ED_armature_eelement_to_mat3(eElem->parent, M_parentRest);

				/* Invert the parent matrix */
				invert_m3_m3(iM_parentRest, M_parentRest);

				/* Get the new head and tail */
				sub_v3_v3v3(newElem->head, eElem->head, eElem->parent->tail);
				sub_v3_v3v3(newElem->tail, eElem->tail, eElem->parent->tail);

				mul_m3_v3(iM_parentRest, newElem->head);
				mul_m3_v3(iM_parentRest, newElem->tail);
			}
		}
		/*	...otherwise add this bone to the armature's bonebase */
		else {
			copy_v3_v3(newElem->head, eElem->head);
			copy_v3_v3(newElem->tail, eElem->tail);
			BLI_addtail(&arm->bonebase, newElem);
		}
	}

	/* Make a pass through the new armature to fix rolling */
	/* also builds restposition again (like BKE_armature_where_is) */
	fix_bonelist_roll(&arm->bonebase, arm->edbo);

	/* so all users of this armature should get rebuilt */
	for (obt = G.main->object.first; obt; obt = obt->id.next) {
		if (obt->data == arm)
			BKE_pose_rebuild(obt, arm);
	}

	DAG_id_tag_update(&arm->id, 0);
}

void ED_armature_edit_free(struct bArmature *arm)
{
	EditArmatureElement *eElement;

	/*	Clear the editbones list */
	if (arm->edbo) {
		if (arm->edbo->first) {
			for (eElement = arm->edbo->first; eElement; eElement = eElement->next) {
				if (eElement->prop) {
					IDP_FreeProperty(eElement->prop);
					MEM_freeN(eElement->prop);
				}
				MEM_freeN(eElement->data);
			}

			BLI_freelistN(arm->edbo);
		}
		MEM_freeN(arm->edbo);
		arm->edbo = NULL;
		arm->act_edelement = NULL;
	}

//	if (arm->edmu) {
//        if (arm->edmu->first) {
//            for (eElement = arm->edmu->first; eElement; eElement = eElement->next) {
//                if (eElement->prop) {
//                    IDP_FreeProperty(eElement->prop);
//                    MEM_freeN(eElement->prop);
//                }
//                MEM_freeN(eElement->data);
//            }
//
//            BLI_freelistN(arm->edmu);
//        }
//        MEM_freeN(arm->edmu);
//        arm->edmu = NULL;
//        arm->act_edmuscle = NULL;
//	}
}

/* Put armature in EditMode */
void ED_armature_to_edit(bArmature *arm)
{
	ED_armature_edit_free(arm);
	arm->edbo = MEM_callocN(sizeof(ListBase), "edbo armature");
	arm->act_edelement = make_elementList(arm->edbo, &arm->bonebase, NULL, arm->act_edelement);

	arm->edmu = MEM_callocN(sizeof(ListBase), "edmu armature");
	arm->act_edelement = make_elementList(arm->edmu, &arm->musclebase, NULL, arm->act_edelement);

//	BIF_freeTemplates(); /* force template update when entering editmode */
}

/* *************************************************************** */
/* Undo for Armature EditMode*/

/* free's bones and their properties */

static void ED_armature_eelement_listbase_free(ListBase *lb)
{
	EditArmatureElement *ebone, *ebone_next;

	for (ebone = lb->first; ebone; ebone = ebone_next) {
		ebone_next = ebone->next;

		if (ebone->prop) {
			IDP_FreeProperty(ebone->prop);
			MEM_freeN(ebone->prop);
		}

		MEM_freeN(ebone);
	}

	BLI_listbase_clear(lb);
}

static void ED_armature_ebone_listbase_copy(ListBase *lb_dst, ListBase *lb_src)
{
	EditArmatureElement *ebone_src;
	EditArmatureElement *ebone_dst;

	BLI_assert(BLI_listbase_is_empty(lb_dst));

	for (ebone_src = lb_src->first; ebone_src; ebone_src = ebone_src->next) {
		ebone_dst = MEM_dupallocN(ebone_src);
		if (ebone_dst->prop) {
			ebone_dst->prop = IDP_CopyProperty(ebone_dst->prop);
		}
		ebone_src->temp = ebone_dst;
		BLI_addtail(lb_dst, ebone_dst);
	}

	/* set pointers */
	for (ebone_dst = lb_dst->first; ebone_dst; ebone_dst = ebone_dst->next) {
		if (ebone_dst->parent) {
			ebone_dst->parent = ebone_dst->parent->temp;
		}
	}
}

static void ED_armature_ebone_listbase_temp_clear(ListBase *lb)
{
	EditArmatureElement *ebone;
	/* be sure they don't hang ever */
	for (ebone = lb->first; ebone; ebone = ebone->next) {
		ebone->temp = NULL;
	}
}

typedef struct UndoArmature {
	EditArmatureElement *act_edelement;
	ListBase lb;
} UndoArmature;

static void undoBones_to_editBones(void *uarmv, void *armv, void *UNUSED(data))
{
	UndoArmature *uarm = uarmv;
	bArmature *arm = armv;
	EditArmatureElement *ebone;

	ED_armature_eelement_listbase_free(arm->edbo);
	ED_armature_ebone_listbase_copy(arm->edbo, &uarm->lb);

	/* active bone */
	if (uarm->act_edelement) {
		ebone = uarm->act_edelement;
		arm->act_edelement = ebone->temp;
	}
	else {
		arm->act_edelement = NULL;
	}

	ED_armature_ebone_listbase_temp_clear(arm->edbo);
}

static void *editBones_to_undoBones(void *armv, void *UNUSED(obdata))
{
	bArmature *arm = armv;
	UndoArmature *uarm;
	EditArmatureElement *ebone;

	uarm = MEM_callocN(sizeof(UndoArmature), "listbase undo");

	ED_armature_ebone_listbase_copy(&uarm->lb, arm->edbo);

	/* active bone */
	if (arm->act_edelement) {
		ebone = arm->act_edelement;
		uarm->act_edelement = ebone->temp;
	}

	ED_armature_ebone_listbase_temp_clear(&uarm->lb);

	return uarm;
}

static void free_undoBones(void *uarmv)
{
	UndoArmature *uarm = uarmv;

	ED_armature_eelement_listbase_free(&uarm->lb);

	MEM_freeN(uarm);
}

static void *get_armature_edit(bContext *C)
{
	Object *obedit = CTX_data_edit_object(C);
	if (obedit && obedit->type == OB_ARMATURE) {
		return obedit->data;
	}
	return NULL;
}

/* and this is all the undo system needs to know */
void undo_push_armature(bContext *C, const char *name)
{
	// XXX solve getdata()
	undo_editmode_push(C, name, get_armature_edit, free_undoBones, undoBones_to_editBones, editBones_to_undoBones, NULL);
}

/* *************************************************************** */
/* Low level selection functions which hide connected-parent
 * flag behavior which gets tricky to handle in selection operators.
 * (no flushing in ED_armature_ebone_select.*, that should be explicit) */

int ED_armature_ebone_selectflag_get(const EditArmatureElement *ebone)
{
	if (ebone->parent && (ebone->flag & ELEMENT_CONNECTED)) {
		return ((ebone->flag & (ELEMENT_SELECTED | ELEMENT_TIPSEL)) |
		        ((ebone->parent->flag & ELEMENT_TIPSEL) ? ELEMENT_ROOTSEL : 0));
	}
	else {
		return (ebone->flag & (ELEMENT_SELECTED | ELEMENT_ROOTSEL | ELEMENT_TIPSEL));
	}
}

void ED_armature_ebone_selectflag_set(EditArmatureElement *ebone, int flag)
{
	flag = flag & (ELEMENT_SELECTED | ELEMENT_ROOTSEL | ELEMENT_TIPSEL);

	if (ebone->parent && (ebone->flag & ELEMENT_CONNECTED)) {
		ebone->flag &= ~(ELEMENT_SELECTED | ELEMENT_ROOTSEL | ELEMENT_TIPSEL);
		ebone->parent->flag &= ~ELEMENT_TIPSEL;

		ebone->flag |= flag;
		ebone->parent->flag |= (flag & ELEMENT_ROOTSEL) ? ELEMENT_TIPSEL : 0;
	}
	else {
		ebone->flag &= ~(ELEMENT_SELECTED | ELEMENT_ROOTSEL | ELEMENT_TIPSEL);
		ebone->flag |= flag;
	}
}

void ED_armature_ebone_selectflag_enable(EditArmatureElement *ebone, int flag)
{
	BLI_assert((flag & (ELEMENT_SELECTED | ELEMENT_ROOTSEL | ELEMENT_TIPSEL)) != 0);
	ED_armature_ebone_selectflag_set(ebone, ebone->flag | flag);
}

void ED_armature_ebone_selectflag_disable(EditArmatureElement *ebone, int flag)
{
	BLI_assert((flag & (ELEMENT_SELECTED | ELEMENT_ROOTSEL | ELEMENT_TIPSEL)) != 0);
	ED_armature_ebone_selectflag_set(ebone, ebone->flag & ~flag);
}

/* could be used in more places */
void ED_armature_ebone_select_set(EditArmatureElement *ebone, bool select)
{
	int flag;
	if (select) {
		BLI_assert((ebone->flag & ELEMENT_UNSELECTABLE) == 0);
		flag = (ELEMENT_SELECTED | ELEMENT_TIPSEL | ELEMENT_ROOTSEL);
	}
	else {
		flag = 0;
	}
	ED_armature_ebone_selectflag_set(ebone, flag);
}

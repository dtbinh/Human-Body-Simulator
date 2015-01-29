/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
<<<<<<< HEAD
 * of the License, or (at your option) any later version. 
=======
 * of the License, or (at your option) any later version.
>>>>>>> Initial commit
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
 * The Original Code is Copyright (C) 2005 by the Blender Foundation.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file blender/editors/space_view3d/drawarmature.c
 *  \ingroup spview3d
 */


#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "DNA_anim_types.h"
#include "DNA_armature_types.h"
#include "DNA_constraint_types.h"
#include "DNA_scene_types.h"
#include "DNA_screen_types.h"
#include "DNA_view3d_types.h"
#include "DNA_object_types.h"

#include "BLI_blenlib.h"
#include "BLI_math.h"
#include "BLI_dlrbTree.h"
#include "BLI_utildefines.h"

#include "BKE_animsys.h"
#include "BKE_action.h"
#include "BKE_armature.h"
#include "BKE_global.h"
#include "BKE_modifier.h"
#include "BKE_nla.h"


#include "BIF_gl.h"
#include "BIF_glutil.h"

#include "ED_armature.h"
#include "ED_keyframes_draw.h"


#include "UI_resources.h"

#include "view3d_intern.h"

#include "GPU_select.h"

/* *************** Armature Drawing - Coloring API ***************************** */

/* global here is reset before drawing each bone */
static ThemeWireColor *bcolor = NULL;

/* values of colCode for set_pchan_glcolor */
enum {
	PCHAN_COLOR_NORMAL  = 0,        /* normal drawing */
	PCHAN_COLOR_SOLID,              /* specific case where "solid" color is needed */
	PCHAN_COLOR_CONSTS,             /* "constraint" colors (which may/may-not be suppressed) */

	PCHAN_COLOR_SPHEREBONE_BASE,    /* for the 'stick' of sphere (envelope) bones */
	PCHAN_COLOR_SPHEREBONE_END,     /* for the ends of sphere (envelope) bones */
	PCHAN_COLOR_LINEBONE            /* for the middle of line-bones */
};

/* This function sets the color-set for coloring a certain bone */
static void set_pchan_colorset(Object *ob, bPoseChannel *pchan)
{
	bPose *pose = (ob) ? ob->pose : NULL;
	bArmature *arm = (ob) ? ob->data : NULL;
	bActionGroup *grp = NULL;
	short color_index = 0;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* sanity check */
	if (ELEM(NULL, ob, arm, pose, pchan)) {
		bcolor = NULL;
		return;
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* only try to set custom color if enabled for armature */
	if (arm->flag & ARM_COL_CUSTOM) {
		/* currently, a bone can only use a custom color set if it's group (if it has one),
		 * has been set to use one
		 */
		if (pchan->agrp_index) {
			grp = (bActionGroup *)BLI_findlink(&pose->agroups, (pchan->agrp_index - 1));
			if (grp)
				color_index = grp->customCol;
		}
	}
<<<<<<< HEAD
	
	/* bcolor is a pointer to the color set to use. If NULL, then the default
	 * color set (based on the theme colors for 3d-view) is used. 
=======

	/* bcolor is a pointer to the color set to use. If NULL, then the default
	 * color set (based on the theme colors for 3d-view) is used.
>>>>>>> Initial commit
	 */
	if (color_index > 0) {
		bTheme *btheme = UI_GetTheme();
		bcolor = &btheme->tarm[(color_index - 1)];
	}
	else if (color_index == -1) {
		/* use the group's own custom color set */
		bcolor = (grp) ? &grp->cs : NULL;
	}
<<<<<<< HEAD
	else 
=======
	else
>>>>>>> Initial commit
		bcolor = NULL;
}

/* This function is for brightening/darkening a given color (like UI_ThemeColorShade()) */
static void cp_shade_color3ub(unsigned char cp[3], const int offset)
{
	int r, g, b;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	r = offset + (int) cp[0];
	CLAMP(r, 0, 255);
	g = offset + (int) cp[1];
	CLAMP(g, 0, 255);
	b = offset + (int) cp[2];
	CLAMP(b, 0, 255);
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	cp[0] = r;
	cp[1] = g;
	cp[2] = b;
}

/* This function sets the gl-color for coloring a certain bone (based on bcolor) */
static bool set_pchan_glColor(short colCode, int boneflag, short constflag)
{
	switch (colCode) {
		case PCHAN_COLOR_NORMAL:
		{
			if (bcolor) {
				unsigned char cp[3];
<<<<<<< HEAD
			
				if (boneflag & BONE_DRAW_ACTIVE) {
					copy_v3_v3_char((char *)cp, bcolor->active);
					if (!(boneflag & BONE_SELECTED)) {
						cp_shade_color3ub(cp, -80);
					}
				}
				else if (boneflag & BONE_SELECTED) {
=======

				if (boneflag & ELEMENT_DRAW_ACTIVE) {
					copy_v3_v3_char((char *)cp, bcolor->active);
					if (!(boneflag & ELEMENT_SELECTED)) {
						cp_shade_color3ub(cp, -80);
					}
				}
				else if (boneflag & ELEMENT_SELECTED) {
>>>>>>> Initial commit
					copy_v3_v3_char((char *)cp, bcolor->select);
				}
				else {
					/* a bit darker than solid */
					copy_v3_v3_char((char *)cp, bcolor->solid);
					cp_shade_color3ub(cp, -50);
				}
<<<<<<< HEAD
			
				glColor3ubv(cp);
			}
			else {
				if ((boneflag & BONE_DRAW_ACTIVE) && (boneflag & BONE_SELECTED)) {
					UI_ThemeColor(TH_BONE_POSE_ACTIVE);
				}
				else if (boneflag & BONE_DRAW_ACTIVE) {
					UI_ThemeColorBlend(TH_WIRE, TH_BONE_POSE, 0.15f); /* unselected active */
				}
				else if (boneflag & BONE_SELECTED) {
=======

				glColor3ubv(cp);
			}
			else {
				if ((boneflag & ELEMENT_DRAW_ACTIVE) && (boneflag & ELEMENT_SELECTED)) {
					UI_ThemeColor(TH_BONE_POSE_ACTIVE);
				}
				else if (boneflag & ELEMENT_DRAW_ACTIVE) {
					UI_ThemeColorBlend(TH_WIRE, TH_BONE_POSE, 0.15f); /* unselected active */
				}
				else if (boneflag & ELEMENT_SELECTED) {
>>>>>>> Initial commit
					UI_ThemeColor(TH_BONE_POSE);
				}
				else {
					UI_ThemeColor(TH_WIRE);
				}
			}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
			return true;
		}
		case PCHAN_COLOR_SOLID:
		{
			if (bcolor) {
				glColor3ubv((unsigned char *)bcolor->solid);
			}
			else
				UI_ThemeColor(TH_BONE_SOLID);
<<<<<<< HEAD
			
=======

>>>>>>> Initial commit
			return true;
		}
		case PCHAN_COLOR_CONSTS:
		{
			if ((bcolor == NULL) || (bcolor->flag & TH_WIRECOLOR_CONSTCOLS)) {
				if (constflag & PCHAN_HAS_TARGET) glColor4ub(255, 150, 0, 80);
				else if (constflag & PCHAN_HAS_IK) glColor4ub(255, 255, 0, 80);
				else if (constflag & PCHAN_HAS_SPLINEIK) glColor4ub(200, 255, 0, 80);
				else if (constflag & PCHAN_HAS_CONST) glColor4ub(0, 255, 120, 80);
<<<<<<< HEAD
			
=======

>>>>>>> Initial commit
				return true;
			}
			return false;
		}
		case PCHAN_COLOR_SPHEREBONE_BASE:
		{
			if (bcolor) {
				unsigned char cp[3];

<<<<<<< HEAD
				if (boneflag & BONE_DRAW_ACTIVE) {
					copy_v3_v3_char((char *)cp, bcolor->active);
				}
				else if (boneflag & BONE_SELECTED) {
=======
				if (boneflag & ELEMENT_DRAW_ACTIVE) {
					copy_v3_v3_char((char *)cp, bcolor->active);
				}
				else if (boneflag & ELEMENT_SELECTED) {
>>>>>>> Initial commit
					copy_v3_v3_char((char *)cp, bcolor->select);
				}
				else {
					copy_v3_v3_char((char *)cp, bcolor->solid);
				}

				glColor3ubv(cp);
			}
			else {
<<<<<<< HEAD
				if (boneflag & BONE_DRAW_ACTIVE) UI_ThemeColorShade(TH_BONE_POSE, 40);
				else if (boneflag & BONE_SELECTED) UI_ThemeColor(TH_BONE_POSE);
				else UI_ThemeColor(TH_BONE_SOLID);
			}
			
=======
				if (boneflag & ELEMENT_DRAW_ACTIVE) UI_ThemeColorShade(TH_BONE_POSE, 40);
				else if (boneflag & ELEMENT_SELECTED) UI_ThemeColor(TH_BONE_POSE);
				else UI_ThemeColor(TH_BONE_SOLID);
			}

>>>>>>> Initial commit
			return true;
		}
		case PCHAN_COLOR_SPHEREBONE_END:
		{
			if (bcolor) {
				unsigned char cp[3];

<<<<<<< HEAD
				if (boneflag & BONE_DRAW_ACTIVE) {
					copy_v3_v3_char((char *)cp, bcolor->active);
					cp_shade_color3ub(cp, 10);
				}
				else if (boneflag & BONE_SELECTED) {
=======
				if (boneflag & ELEMENT_DRAW_ACTIVE) {
					copy_v3_v3_char((char *)cp, bcolor->active);
					cp_shade_color3ub(cp, 10);
				}
				else if (boneflag & ELEMENT_SELECTED) {
>>>>>>> Initial commit
					copy_v3_v3_char((char *)cp, bcolor->select);
					cp_shade_color3ub(cp, -30);
				}
				else {
					copy_v3_v3_char((char *)cp, bcolor->solid);
					cp_shade_color3ub(cp, -30);
				}
<<<<<<< HEAD
			
				glColor3ubv(cp);
			}
			else {
				if (boneflag & BONE_DRAW_ACTIVE) UI_ThemeColorShade(TH_BONE_POSE, 10);
				else if (boneflag & BONE_SELECTED) UI_ThemeColorShade(TH_BONE_POSE, -30);
=======

				glColor3ubv(cp);
			}
			else {
				if (boneflag & ELEMENT_DRAW_ACTIVE) UI_ThemeColorShade(TH_BONE_POSE, 10);
				else if (boneflag & ELEMENT_SELECTED) UI_ThemeColorShade(TH_BONE_POSE, -30);
>>>>>>> Initial commit
				else UI_ThemeColorShade(TH_BONE_SOLID, -30);
			}
			break;
		}
		case PCHAN_COLOR_LINEBONE:
		{
			/* inner part in background color or constraint */
			if ((constflag) && ((bcolor == NULL) || (bcolor->flag & TH_WIRECOLOR_CONSTCOLS))) {
				if (constflag & PCHAN_HAS_TARGET) glColor3ub(255, 150, 0);
				else if (constflag & PCHAN_HAS_IK) glColor3ub(255, 255, 0);
				else if (constflag & PCHAN_HAS_SPLINEIK) glColor3ub(200, 255, 0);
				else if (constflag & PCHAN_HAS_CONST) glColor3ub(0, 255, 120);
				else if (constflag) UI_ThemeColor(TH_BONE_POSE);  /* PCHAN_HAS_ACTION */
			}
			else {
				if (bcolor) {
					const char *cp = bcolor->solid;
					glColor4ub(cp[0], cp[1], cp[2], 204);
				}
				else
					UI_ThemeColorShade(TH_BACK, -30);
			}
<<<<<<< HEAD
		
			return true;
		}
	}
	
=======

			return true;
		}
	}

>>>>>>> Initial commit
	return false;
}

static void set_ebone_glColor(const unsigned int boneflag)
{
<<<<<<< HEAD
	if ((boneflag & BONE_DRAW_ACTIVE) && (boneflag & BONE_SELECTED)) {
		UI_ThemeColor(TH_EDGE_SELECT);
	}
	else if (boneflag & BONE_DRAW_ACTIVE) {
		UI_ThemeColorBlend(TH_WIRE_EDIT, TH_EDGE_SELECT, 0.15f); /* unselected active */
	}
	else if (boneflag & BONE_SELECTED) {
=======
	if ((boneflag & ELEMENT_DRAW_ACTIVE) && (boneflag & ELEMENT_SELECTED)) {
		UI_ThemeColor(TH_EDGE_SELECT);
	}
	else if (boneflag & ELEMENT_DRAW_ACTIVE) {
		UI_ThemeColorBlend(TH_WIRE_EDIT, TH_EDGE_SELECT, 0.15f); /* unselected active */
	}
	else if (boneflag & ELEMENT_SELECTED) {
>>>>>>> Initial commit
		UI_ThemeColorShade(TH_EDGE_SELECT, -20);
	}
	else {
		UI_ThemeColor(TH_WIRE_EDIT);
	}
}

/* *************** Armature drawing, helper calls for parts ******************* */

/* half the cube, in Y */
static const float cube[8][3] = {
	{-1.0,  0.0, -1.0},
	{-1.0,  0.0,  1.0},
	{-1.0,  1.0,  1.0},
	{-1.0,  1.0, -1.0},
	{ 1.0,  0.0, -1.0},
	{ 1.0,  0.0,  1.0},
	{ 1.0,  1.0,  1.0},
	{ 1.0,  1.0, -1.0},
};

static void drawsolidcube_size(float xsize, float ysize, float zsize)
{
	static GLuint displist = 0;
	float n[3] = {0.0f};
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	glScalef(xsize, ysize, zsize);

	if (displist == 0) {
		displist = glGenLists(1);
		glNewList(displist, GL_COMPILE);

		glBegin(GL_QUADS);
		n[0] = -1.0;
<<<<<<< HEAD
		glNormal3fv(n); 
		glVertex3fv(cube[0]); glVertex3fv(cube[1]); glVertex3fv(cube[2]); glVertex3fv(cube[3]);
		n[0] = 0;
		n[1] = -1.0;
		glNormal3fv(n); 
		glVertex3fv(cube[0]); glVertex3fv(cube[4]); glVertex3fv(cube[5]); glVertex3fv(cube[1]);
		n[1] = 0;
		n[0] = 1.0;
		glNormal3fv(n); 
		glVertex3fv(cube[4]); glVertex3fv(cube[7]); glVertex3fv(cube[6]); glVertex3fv(cube[5]);
		n[0] = 0;
		n[1] = 1.0;
		glNormal3fv(n); 
		glVertex3fv(cube[7]); glVertex3fv(cube[3]); glVertex3fv(cube[2]); glVertex3fv(cube[6]);
		n[1] = 0;
		n[2] = 1.0;
		glNormal3fv(n); 
		glVertex3fv(cube[1]); glVertex3fv(cube[5]); glVertex3fv(cube[6]); glVertex3fv(cube[2]);
		n[2] = -1.0;
		glNormal3fv(n); 
=======
		glNormal3fv(n);
		glVertex3fv(cube[0]); glVertex3fv(cube[1]); glVertex3fv(cube[2]); glVertex3fv(cube[3]);
		n[0] = 0;
		n[1] = -1.0;
		glNormal3fv(n);
		glVertex3fv(cube[0]); glVertex3fv(cube[4]); glVertex3fv(cube[5]); glVertex3fv(cube[1]);
		n[1] = 0;
		n[0] = 1.0;
		glNormal3fv(n);
		glVertex3fv(cube[4]); glVertex3fv(cube[7]); glVertex3fv(cube[6]); glVertex3fv(cube[5]);
		n[0] = 0;
		n[1] = 1.0;
		glNormal3fv(n);
		glVertex3fv(cube[7]); glVertex3fv(cube[3]); glVertex3fv(cube[2]); glVertex3fv(cube[6]);
		n[1] = 0;
		n[2] = 1.0;
		glNormal3fv(n);
		glVertex3fv(cube[1]); glVertex3fv(cube[5]); glVertex3fv(cube[6]); glVertex3fv(cube[2]);
		n[2] = -1.0;
		glNormal3fv(n);
>>>>>>> Initial commit
		glVertex3fv(cube[7]); glVertex3fv(cube[4]); glVertex3fv(cube[0]); glVertex3fv(cube[3]);
		glEnd();

		glEndList();
	}

	glCallList(displist);
}

static void drawcube_size(float xsize, float ysize, float zsize)
{
	static GLuint displist = 0;
<<<<<<< HEAD
	
	if (displist == 0) {
		displist = glGenLists(1);
		glNewList(displist, GL_COMPILE);
		
=======

	if (displist == 0) {
		displist = glGenLists(1);
		glNewList(displist, GL_COMPILE);

>>>>>>> Initial commit
		glBegin(GL_LINE_STRIP);
		glVertex3fv(cube[0]); glVertex3fv(cube[1]); glVertex3fv(cube[2]); glVertex3fv(cube[3]);
		glVertex3fv(cube[0]); glVertex3fv(cube[4]); glVertex3fv(cube[5]); glVertex3fv(cube[6]);
		glVertex3fv(cube[7]); glVertex3fv(cube[4]);
		glEnd();
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		glBegin(GL_LINES);
		glVertex3fv(cube[1]); glVertex3fv(cube[5]);
		glVertex3fv(cube[2]); glVertex3fv(cube[6]);
		glVertex3fv(cube[3]); glVertex3fv(cube[7]);
		glEnd();
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		glEndList();
	}

	glScalef(xsize, ysize, zsize);
	glCallList(displist);
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
}


static void draw_bonevert(void)
{
	static GLuint displist = 0;
<<<<<<< HEAD
	
	if (displist == 0) {
		GLUquadricObj   *qobj;
		
		displist = glGenLists(1);
		glNewList(displist, GL_COMPILE);
			
		glPushMatrix();
		
		qobj    = gluNewQuadric();
		gluQuadricDrawStyle(qobj, GLU_SILHOUETTE); 
		gluDisk(qobj, 0.0,  0.05, 16, 1);
		
		glRotatef(90, 0, 1, 0);
		gluDisk(qobj, 0.0,  0.05, 16, 1);
		
		glRotatef(90, 1, 0, 0);
		gluDisk(qobj, 0.0,  0.05, 16, 1);
		
		gluDeleteQuadric(qobj);  
		
=======

	if (displist == 0) {
		GLUquadricObj   *qobj;

		displist = glGenLists(1);
		glNewList(displist, GL_COMPILE);

		glPushMatrix();

		qobj    = gluNewQuadric();
		gluQuadricDrawStyle(qobj, GLU_SILHOUETTE);
		gluDisk(qobj, 0.0,  0.05, 16, 1);

		glRotatef(90, 0, 1, 0);
		gluDisk(qobj, 0.0,  0.05, 16, 1);

		glRotatef(90, 1, 0, 0);
		gluDisk(qobj, 0.0,  0.05, 16, 1);

		gluDeleteQuadric(qobj);

>>>>>>> Initial commit
		glPopMatrix();
		glEndList();
	}

	glCallList(displist);
}

static void draw_bonevert_solid(void)
{
	static GLuint displist = 0;
<<<<<<< HEAD
	
	if (displist == 0) {
		GLUquadricObj *qobj;
		
		displist = glGenLists(1);
		glNewList(displist, GL_COMPILE);
		
		qobj = gluNewQuadric();
		gluQuadricDrawStyle(qobj, GLU_FILL); 
		glShadeModel(GL_SMOOTH);
		gluSphere(qobj, 0.05, 8, 5);
		glShadeModel(GL_FLAT);
		gluDeleteQuadric(qobj);  
		
=======

	if (displist == 0) {
		GLUquadricObj *qobj;

		displist = glGenLists(1);
		glNewList(displist, GL_COMPILE);

		qobj = gluNewQuadric();
		gluQuadricDrawStyle(qobj, GLU_FILL);
		glShadeModel(GL_SMOOTH);
		gluSphere(qobj, 0.05, 8, 5);
		glShadeModel(GL_FLAT);
		gluDeleteQuadric(qobj);

>>>>>>> Initial commit
		glEndList();
	}

	glCallList(displist);
}

static const float bone_octahedral_verts[6][3] = {
	{ 0.0f, 0.0f,  0.0f},
	{ 0.1f, 0.1f,  0.1f},
	{ 0.1f, 0.1f, -0.1f},
	{-0.1f, 0.1f, -0.1f},
	{-0.1f, 0.1f,  0.1f},
	{ 0.0f, 1.0f,  0.0f}
};

static const unsigned int bone_octahedral_wire_sides[8] = {0, 1, 5, 3, 0, 4, 5, 2};
static const unsigned int bone_octahedral_wire_square[8] = {1, 2, 3, 4, 1};

static const unsigned int bone_octahedral_solid_tris[8][3] = {
	{2, 1, 0}, /* bottom */
	{3, 2, 0},
	{4, 3, 0},
	{1, 4, 0},

	{5, 1, 2}, /* top */
	{5, 2, 3},
	{5, 3, 4},
	{5, 4, 1}
};

/* aligned with bone_octahedral_solid_tris */
static const float bone_octahedral_solid_normals[8][3] = {
	{ M_SQRT1_2,   -M_SQRT1_2,    0.00000000f},
	{-0.00000000f, -M_SQRT1_2,   -M_SQRT1_2},
	{-M_SQRT1_2,   -M_SQRT1_2,    0.00000000f},
	{ 0.00000000f, -M_SQRT1_2,    M_SQRT1_2},
	{ 0.99388373f,  0.11043154f, -0.00000000f},
	{ 0.00000000f,  0.11043154f, -0.99388373f},
	{-0.99388373f,  0.11043154f,  0.00000000f},
	{ 0.00000000f,  0.11043154f,  0.99388373f}
};

static void draw_bone_octahedral(void)
{
	static GLuint displist = 0;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	if (displist == 0) {
		displist = glGenLists(1);
		glNewList(displist, GL_COMPILE);

		/*	Section 1, sides */
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, bone_octahedral_verts);
		glDrawElements(GL_LINE_LOOP,
		               sizeof(bone_octahedral_wire_sides) / sizeof(*bone_octahedral_wire_sides),
		               GL_UNSIGNED_INT,
		               bone_octahedral_wire_sides);

		/*	Section 1, square */
		glDrawElements(GL_LINE_LOOP,
		               sizeof(bone_octahedral_wire_square) / sizeof(*bone_octahedral_wire_square),
		               GL_UNSIGNED_INT,
		               bone_octahedral_wire_square);
		glDisableClientState(GL_VERTEX_ARRAY);
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		glEndList();
	}

	glCallList(displist);
<<<<<<< HEAD
}	
=======
}
>>>>>>> Initial commit

static void draw_bone_solid_octahedral(void)
{
	static GLuint displist = 0;

	if (displist == 0) {
		int i;

		displist = glGenLists(1);
		glNewList(displist, GL_COMPILE);

#if 1
		glBegin(GL_TRIANGLES);
		for (i = 0; i < 8; i++) {
			glNormal3fv(bone_octahedral_solid_normals[i]);
			glVertex3fv(bone_octahedral_verts[bone_octahedral_solid_tris[i][0]]);
			glVertex3fv(bone_octahedral_verts[bone_octahedral_solid_tris[i][1]]);
			glVertex3fv(bone_octahedral_verts[bone_octahedral_solid_tris[i][2]]);
		}

		glEnd();

#else   /* not working because each vert needs a different normal */
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glNormalPointer(GL_FLOAT, 0, bone_octahedral_solid_normals);
		glVertexPointer(3, GL_FLOAT, 0, bone_octahedral_verts);
		glDrawElements(GL_TRIANGLES, sizeof(bone_octahedral_solid_tris) / sizeof(unsigned int),
		               GL_UNSIGNED_INT, bone_octahedral_solid_tris);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
#endif

		glEndList();
	}

	glCallList(displist);
<<<<<<< HEAD
}	
=======
}
>>>>>>> Initial commit

/* *************** Armature drawing, bones ******************* */


static void draw_bone_points(const short dt, int armflag, unsigned int boneflag, int id)
{
	/*	Draw root point if we are not connected */
<<<<<<< HEAD
	if ((boneflag & BONE_CONNECTED) == 0) {
		if (id != -1)
			GPU_select_load_id(id | BONESEL_ROOT);
		
		if (dt <= OB_WIRE) {
			if (armflag & ARM_EDITMODE) {
				if (boneflag & BONE_ROOTSEL) UI_ThemeColor(TH_VERTEX_SELECT);
=======
	if ((boneflag & ELEMENT_CONNECTED) == 0) {
		if (id != -1)
			GPU_select_load_id(id | ELEMENTSEL_ROOT);

		if (dt <= OB_WIRE) {
			if (armflag & ARM_EDITMODE) {
				if (boneflag & ELEMENT_ROOTSEL) UI_ThemeColor(TH_VERTEX_SELECT);
>>>>>>> Initial commit
				else UI_ThemeColor(TH_VERTEX);
			}
		}
		else {
<<<<<<< HEAD
			if (armflag & ARM_POSEMODE) 
=======
			if (armflag & ARM_POSEMODE)
>>>>>>> Initial commit
				set_pchan_glColor(PCHAN_COLOR_SOLID, boneflag, 0);
			else
				UI_ThemeColor(TH_BONE_SOLID);
		}
<<<<<<< HEAD
		
		if (dt > OB_WIRE) 
			draw_bonevert_solid();
		else 
			draw_bonevert();
	}
	
	/*	Draw tip point */
	if (id != -1)
		GPU_select_load_id(id | BONESEL_TIP);
	
	if (dt <= OB_WIRE) {
		if (armflag & ARM_EDITMODE) {
			if (boneflag & BONE_TIPSEL) UI_ThemeColor(TH_VERTEX_SELECT);
=======

		if (dt > OB_WIRE)
			draw_bonevert_solid();
		else
			draw_bonevert();
	}

	/*	Draw tip point */
	if (id != -1)
		GPU_select_load_id(id | ELEMENTSEL_TIP);

	if (dt <= OB_WIRE) {
		if (armflag & ARM_EDITMODE) {
			if (boneflag & ELEMENT_TIPSEL) UI_ThemeColor(TH_VERTEX_SELECT);
>>>>>>> Initial commit
			else UI_ThemeColor(TH_VERTEX);
		}
	}
	else {
<<<<<<< HEAD
		if (armflag & ARM_POSEMODE) 
=======
		if (armflag & ARM_POSEMODE)
>>>>>>> Initial commit
			set_pchan_glColor(PCHAN_COLOR_SOLID, boneflag, 0);
		else
			UI_ThemeColor(TH_BONE_SOLID);
	}
<<<<<<< HEAD
	
	glTranslatef(0.0f, 1.0f, 0.0f);
	if (dt > OB_WIRE) 
		draw_bonevert_solid();
	else 
		draw_bonevert();
	glTranslatef(0.0f, -1.0f, 0.0f);
	
=======

	glTranslatef(0.0f, 1.0f, 0.0f);
	if (dt > OB_WIRE)
		draw_bonevert_solid();
	else
		draw_bonevert();
	glTranslatef(0.0f, -1.0f, 0.0f);

>>>>>>> Initial commit
}

/* 16 values of sin function (still same result!) */
static const float si[16] = {
	0.00000000f,
	0.20129852f, 0.39435585f,
	0.57126821f, 0.72479278f,
	0.84864425f, 0.93775213f,
	0.98846832f, 0.99871650f,
	0.96807711f, 0.89780453f,
	0.79077573f, 0.65137248f,
	0.48530196f, 0.29936312f,
	0.10116832f
};
/* 16 values of cos function (still same result!) */
static const float co[16] = {
	1.00000000f,
	0.97952994f, 0.91895781f,
	0.82076344f, 0.68896691f,
	0.52896401f, 0.34730525f,
	0.15142777f, -0.05064916f,
	-0.25065253f, -0.44039415f,
	-0.61210598f, -0.75875812f,
	-0.87434661f, -0.95413925f,
	-0.99486932f
};



/* smat, imat = mat & imat to draw screenaligned */
<<<<<<< HEAD
static void draw_sphere_bone_dist(float smat[4][4], float imat[4][4], bPoseChannel *pchan, EditBone *ebone)
{
	float head, tail, dist /*, length*/;
	float *headvec, *tailvec, dirvec[3];
	
	/* figure out the sizes of spheres */
	if (ebone) {
		/* this routine doesn't call get_matrix_editbone() that calculates it */
		ebone->length = len_v3v3(ebone->head, ebone->tail);

		/*length = ebone->length;*/ /*UNUSED*/
		tail = ebone->rad_tail;
		dist = ebone->dist;
		if (ebone->parent && (ebone->flag & BONE_CONNECTED))
			head = ebone->parent->rad_tail;
		else
			head = ebone->rad_head;
		headvec = ebone->head;
		tailvec = ebone->tail;
=======
static void draw_sphere_element_dist(float smat[4][4], float imat[4][4], bPoseChannel *pchan, EditArmatureElement *eelement)
{
	float head, tail, dist /*, length*/;
	float *headvec, *tailvec, dirvec[3];

	/* figure out the sizes of spheres */
	if (eelement) {
		/* this routine doesn't call get_matrix_editbone() that calculates it */
		eelement->length = len_v3v3(eelement->head, eelement->tail);

		/*length = eelement->length;*/ /*UNUSED*/
		tail = eelement->rad_tail;
		dist = ((BoneData*)eelement->data)->dist;
		if (eelement->parent && (eelement->flag & ELEMENT_CONNECTED))
			head = eelement->parent->rad_tail;
		else
			head = eelement->rad_head;
		headvec = eelement->head;
		tailvec = eelement->tail;
>>>>>>> Initial commit
	}
	else {
		/*length = pchan->bone->length;*/ /*UNUSED*/
		tail = pchan->bone->rad_tail;
<<<<<<< HEAD
		dist = pchan->bone->dist;
		if (pchan->parent && (pchan->bone->flag & BONE_CONNECTED))
=======
		dist = ((BoneData*)pchan->bone->data)->dist;
		if (pchan->parent && (pchan->bone->flag & ELEMENT_CONNECTED))
>>>>>>> Initial commit
			head = pchan->parent->bone->rad_tail;
		else
			head = pchan->bone->rad_head;
		headvec = pchan->pose_head;
		tailvec = pchan->pose_tail;
	}
<<<<<<< HEAD
	
	/* ***** draw it ***** */
	
=======

	/* ***** draw it ***** */

>>>>>>> Initial commit
	/* move vector to viewspace */
	sub_v3_v3v3(dirvec, tailvec, headvec);
	mul_mat3_m4_v3(smat, dirvec);
	/* clear zcomp */
	dirvec[2] = 0.0f;

	if (head != tail) {
		/* correction when viewing along the bones axis
		 * it pops in and out but better then artifacts, [#23841] */
		float view_dist = len_v2(dirvec);

		if (head - view_dist > tail) {
			tailvec = headvec;
			tail = head;
			zero_v3(dirvec);
			dirvec[0] = 0.00001;  /* XXX. weak but ok */
		}
		else if (tail - view_dist > head) {
			headvec = tailvec;
			head = tail;
			zero_v3(dirvec);
			dirvec[0] = 0.00001;  /* XXX. weak but ok */
		}
	}

	/* move vector back */
	mul_mat3_m4_v3(imat, dirvec);
<<<<<<< HEAD
	
	if (0.0f != normalize_v3(dirvec)) {
		float norvec[3], vec1[3], vec2[3], vec[3];
		int a;
		
		//mul_v3_fl(dirvec, head);
		cross_v3_v3v3(norvec, dirvec, imat[2]);
		
		glBegin(GL_QUAD_STRIP);
		
=======

	if (0.0f != normalize_v3(dirvec)) {
		float norvec[3], vec1[3], vec2[3], vec[3];
		int a;

		//mul_v3_fl(dirvec, head);
		cross_v3_v3v3(norvec, dirvec, imat[2]);

		glBegin(GL_QUAD_STRIP);

>>>>>>> Initial commit
		for (a = 0; a < 16; a++) {
			vec[0] = -si[a] * dirvec[0] + co[a] * norvec[0];
			vec[1] = -si[a] * dirvec[1] + co[a] * norvec[1];
			vec[2] = -si[a] * dirvec[2] + co[a] * norvec[2];

			madd_v3_v3v3fl(vec1, headvec, vec, head);
			madd_v3_v3v3fl(vec2, headvec, vec, head + dist);
<<<<<<< HEAD
			
=======

>>>>>>> Initial commit
			glColor4ub(255, 255, 255, 50);
			glVertex3fv(vec1);
			//glColor4ub(255, 255, 255, 0);
			glVertex3fv(vec2);
		}
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		for (a = 15; a >= 0; a--) {
			vec[0] = si[a] * dirvec[0] + co[a] * norvec[0];
			vec[1] = si[a] * dirvec[1] + co[a] * norvec[1];
			vec[2] = si[a] * dirvec[2] + co[a] * norvec[2];

			madd_v3_v3v3fl(vec1, tailvec, vec, tail);
			madd_v3_v3v3fl(vec2, tailvec, vec, tail + dist);
<<<<<<< HEAD
			
=======

>>>>>>> Initial commit
			//glColor4ub(255, 255, 255, 50);
			glVertex3fv(vec1);
			//glColor4ub(255, 255, 255, 0);
			glVertex3fv(vec2);
		}
		/* make it cyclic... */
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		vec[0] = -si[0] * dirvec[0] + co[0] * norvec[0];
		vec[1] = -si[0] * dirvec[1] + co[0] * norvec[1];
		vec[2] = -si[0] * dirvec[2] + co[0] * norvec[2];

		madd_v3_v3v3fl(vec1, headvec, vec, head);
		madd_v3_v3v3fl(vec2, headvec, vec, head + dist);

		//glColor4ub(255, 255, 255, 50);
		glVertex3fv(vec1);
		//glColor4ub(255, 255, 255, 0);
		glVertex3fv(vec2);
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		glEnd();
	}
}

<<<<<<< HEAD

/* smat, imat = mat & imat to draw screenaligned */
static void draw_sphere_bone_wire(float smat[4][4], float imat[4][4],
                                  int armflag, int boneflag, short constflag, unsigned int id,
                                  bPoseChannel *pchan, EditBone *ebone)
{
	float head, tail /*, length*/;
	float *headvec, *tailvec, dirvec[3];
	
	/* figure out the sizes of spheres */
	if (ebone) {
		/* this routine doesn't call get_matrix_editbone() that calculates it */
		ebone->length = len_v3v3(ebone->head, ebone->tail);
		
		/*length = ebone->length;*/ /*UNUSED*/
		tail = ebone->rad_tail;
		if (ebone->parent && (boneflag & BONE_CONNECTED))
			head = ebone->parent->rad_tail;
		else
			head = ebone->rad_head;
		headvec = ebone->head;
		tailvec = ebone->tail;
=======
/* smat, imat = mat & imat to draw screenaligned */
static void draw_sphere_element_wire(float smat[4][4], float imat[4][4],
                                  int armflag, int boneflag, short constflag, unsigned int id,
                                  bPoseChannel *pchan, EditArmatureElement *eelement)
{
	float head, tail /*, length*/;
	float *headvec, *tailvec, dirvec[3];

	/* figure out the sizes of spheres */
	if (eelement) {
		/* this routine doesn't call get_matrix_editbone() that calculates it */
		eelement->length = len_v3v3(eelement->head, eelement->tail);

		/*length = ebone->length;*/ /*UNUSED*/
		tail = eelement->rad_tail;
		if (eelement->parent && (boneflag & ELEMENT_CONNECTED))
			head = eelement->parent->rad_tail;
		else
			head = eelement->rad_head;
		headvec = eelement->head;
		tailvec = eelement->tail;
>>>>>>> Initial commit
	}
	else {
		/*length = pchan->bone->length;*/ /*UNUSED*/
		tail = pchan->bone->rad_tail;
<<<<<<< HEAD
		if ((pchan->parent) && (boneflag & BONE_CONNECTED))
=======
		if ((pchan->parent) && (boneflag & ELEMENT_CONNECTED))
>>>>>>> Initial commit
			head = pchan->parent->bone->rad_tail;
		else
			head = pchan->bone->rad_head;
		headvec = pchan->pose_head;
		tailvec = pchan->pose_tail;
	}
<<<<<<< HEAD
	
	/* sphere root color */
	if (armflag & ARM_EDITMODE) {
		if (boneflag & BONE_ROOTSEL) UI_ThemeColor(TH_VERTEX_SELECT);
=======

	/* sphere root color */
	if (armflag & ARM_EDITMODE) {
		if (boneflag & ELEMENT_ROOTSEL) UI_ThemeColor(TH_VERTEX_SELECT);
>>>>>>> Initial commit
		else UI_ThemeColor(TH_VERTEX);
	}
	else if (armflag & ARM_POSEMODE)
		set_pchan_glColor(PCHAN_COLOR_NORMAL, boneflag, constflag);
<<<<<<< HEAD
	
	/*	Draw root point if we are not connected */
	if ((boneflag & BONE_CONNECTED) == 0) {
		if (id != -1)
			GPU_select_load_id(id | BONESEL_ROOT);
		
		drawcircball(GL_LINE_LOOP, headvec, head, imat);
	}
	
	/*	Draw tip point */
	if (armflag & ARM_EDITMODE) {
		if (boneflag & BONE_TIPSEL) UI_ThemeColor(TH_VERTEX_SELECT);
		else UI_ThemeColor(TH_VERTEX);
	}
	
	if (id != -1)
		GPU_select_load_id(id | BONESEL_TIP);
	
	drawcircball(GL_LINE_LOOP, tailvec, tail, imat);
	
	/* base */
	if (armflag & ARM_EDITMODE) {
		if (boneflag & BONE_SELECTED) UI_ThemeColor(TH_SELECT);
		else UI_ThemeColor(TH_WIRE_EDIT);
	}
	
	sub_v3_v3v3(dirvec, tailvec, headvec);
	
=======

	/*	Draw root point if we are not connected */
	if ((boneflag & ELEMENT_CONNECTED) == 0) {
		if (id != -1)
			GPU_select_load_id(id | ELEMENTSEL_ROOT);

		drawcircball(GL_LINE_LOOP, headvec, head, imat);
	}

	/*	Draw tip point */
	if (armflag & ARM_EDITMODE) {
		if (boneflag & ELEMENT_TIPSEL) UI_ThemeColor(TH_VERTEX_SELECT);
		else UI_ThemeColor(TH_VERTEX);
	}

	if (id != -1)
		GPU_select_load_id(id | ELEMENTSEL_TIP);

	drawcircball(GL_LINE_LOOP, tailvec, tail, imat);

	/* base */
	if (armflag & ARM_EDITMODE) {
		if (boneflag & ELEMENT_SELECTED) UI_ThemeColor(TH_SELECT);
		else UI_ThemeColor(TH_WIRE_EDIT);
	}

	sub_v3_v3v3(dirvec, tailvec, headvec);

>>>>>>> Initial commit
	/* move vector to viewspace */
	mul_mat3_m4_v3(smat, dirvec);
	/* clear zcomp */
	dirvec[2] = 0.0f;
	/* move vector back */
	mul_mat3_m4_v3(imat, dirvec);
<<<<<<< HEAD
	
	if (0.0f != normalize_v3(dirvec)) {
		float norvech[3], norvect[3], vec[3];
		
		copy_v3_v3(vec, dirvec);
		
		mul_v3_fl(dirvec, head);
		cross_v3_v3v3(norvech, dirvec, imat[2]);
		
		mul_v3_fl(vec, tail);
		cross_v3_v3v3(norvect, vec, imat[2]);
		
		if (id != -1)
			GPU_select_load_id(id | BONESEL_BONE);
		
=======

	if (0.0f != normalize_v3(dirvec)) {
		float norvech[3], norvect[3], vec[3];

		copy_v3_v3(vec, dirvec);

		mul_v3_fl(dirvec, head);
		cross_v3_v3v3(norvech, dirvec, imat[2]);

		mul_v3_fl(vec, tail);
		cross_v3_v3v3(norvect, vec, imat[2]);

		if (id != -1)
			GPU_select_load_id(id | ELEMENTSEL_ELEMENT);

>>>>>>> Initial commit
		glBegin(GL_LINES);

		add_v3_v3v3(vec, headvec, norvech);
		glVertex3fv(vec);

		add_v3_v3v3(vec, tailvec, norvect);
		glVertex3fv(vec);

		sub_v3_v3v3(vec, headvec, norvech);
		glVertex3fv(vec);

		sub_v3_v3v3(vec, tailvec, norvect);
		glVertex3fv(vec);
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		glEnd();
	}
}

/* does wire only for outline selecting */
<<<<<<< HEAD
static void draw_sphere_bone(const short dt, int armflag, int boneflag, short constflag, unsigned int id,
                             bPoseChannel *pchan, EditBone *ebone)
=======
static void draw_sphere_element(const short dt, int armflag, int boneflag, short constflag, unsigned int id,
                             bPoseChannel *pchan, EditArmatureElement *eelement)
>>>>>>> Initial commit
{
	GLUquadricObj *qobj;
	float head, tail, length;
	float fac1, fac2;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	glPushMatrix();
	qobj = gluNewQuadric();

	/* figure out the sizes of spheres */
<<<<<<< HEAD
	if (ebone) {
		length = ebone->length;
		tail = ebone->rad_tail;
		if (ebone->parent && (boneflag & BONE_CONNECTED))
			head = ebone->parent->rad_tail;
		else
			head = ebone->rad_head;
=======
	if (eelement) {
		length = eelement->length;
		tail = eelement->rad_tail;
		if (eelement->parent && (boneflag & ELEMENT_CONNECTED))
			head = eelement->parent->rad_tail;
		else
			head = eelement->rad_head;
>>>>>>> Initial commit
	}
	else {
		length = pchan->bone->length;
		tail = pchan->bone->rad_tail;
<<<<<<< HEAD
		if (pchan->parent && (boneflag & BONE_CONNECTED))
=======
		if (pchan->parent && (boneflag & ELEMENT_CONNECTED))
>>>>>>> Initial commit
			head = pchan->parent->bone->rad_tail;
		else
			head = pchan->bone->rad_head;
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* move to z-axis space */
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

	if (dt == OB_SOLID) {
		/* set up solid drawing */
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_LIGHTING);
<<<<<<< HEAD
		
		gluQuadricDrawStyle(qobj, GLU_FILL); 
		glShadeModel(GL_SMOOTH);
	}
	else {
		gluQuadricDrawStyle(qobj, GLU_SILHOUETTE); 
	}
	
	/* sphere root color */
	if (armflag & ARM_EDITMODE) {
		if (boneflag & BONE_ROOTSEL) UI_ThemeColor(TH_VERTEX_SELECT);
=======

		gluQuadricDrawStyle(qobj, GLU_FILL);
		glShadeModel(GL_SMOOTH);
	}
	else {
		gluQuadricDrawStyle(qobj, GLU_SILHOUETTE);
	}

	/* sphere root color */
	if (armflag & ARM_EDITMODE) {
		if (boneflag & ELEMENT_ROOTSEL) UI_ThemeColor(TH_VERTEX_SELECT);
>>>>>>> Initial commit
		else UI_ThemeColorShade(TH_BONE_SOLID, -30);
	}
	else if (armflag & ARM_POSEMODE)
		set_pchan_glColor(PCHAN_COLOR_SPHEREBONE_END, boneflag, constflag);
	else if (dt == OB_SOLID)
		UI_ThemeColorShade(TH_BONE_SOLID, -30);
<<<<<<< HEAD
	
	/*	Draw root point if we are not connected */
	if ((boneflag & BONE_CONNECTED) == 0) {
		if (id != -1)
			GPU_select_load_id(id | BONESEL_ROOT);
		gluSphere(qobj, head, 16, 10);
	}
	
	/*	Draw tip point */
	if (armflag & ARM_EDITMODE) {
		if (boneflag & BONE_TIPSEL) UI_ThemeColor(TH_VERTEX_SELECT);
=======

	/*	Draw root point if we are not connected */
	if ((boneflag & ELEMENT_CONNECTED) == 0) {
		if (id != -1)
			GPU_select_load_id(id | ELEMENTSEL_ROOT);
		gluSphere(qobj, head, 16, 10);
	}

	/*	Draw tip point */
	if (armflag & ARM_EDITMODE) {
		if (boneflag & ELEMENT_TIPSEL) UI_ThemeColor(TH_VERTEX_SELECT);
>>>>>>> Initial commit
		else UI_ThemeColorShade(TH_BONE_SOLID, -30);
	}

	if (id != -1)
<<<<<<< HEAD
		GPU_select_load_id(id | BONESEL_TIP);
	
	glTranslatef(0.0f, 0.0f, length);
	gluSphere(qobj, tail, 16, 10);
	glTranslatef(0.0f, 0.0f, -length);
	
	/* base */
	if (armflag & ARM_EDITMODE) {
		if (boneflag & BONE_SELECTED) UI_ThemeColor(TH_SELECT);
=======
		GPU_select_load_id(id | ELEMENTSEL_TIP);

	glTranslatef(0.0f, 0.0f, length);
	gluSphere(qobj, tail, 16, 10);
	glTranslatef(0.0f, 0.0f, -length);

	/* base */
	if (armflag & ARM_EDITMODE) {
		if (boneflag & ELEMENT_SELECTED) UI_ThemeColor(TH_SELECT);
>>>>>>> Initial commit
		else UI_ThemeColor(TH_BONE_SOLID);
	}
	else if (armflag & ARM_POSEMODE)
		set_pchan_glColor(PCHAN_COLOR_SPHEREBONE_BASE, boneflag, constflag);
	else if (dt == OB_SOLID)
		UI_ThemeColor(TH_BONE_SOLID);
<<<<<<< HEAD
	
	fac1 = (length - head) / length;
	fac2 = (length - tail) / length;
	
	if (length > (head + tail)) {
		if (id != -1)
			GPU_select_load_id(id | BONESEL_BONE);
		
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(-1.0f, -1.0f);
		
		glTranslatef(0.0f, 0.0f, head);
		gluCylinder(qobj, fac1 * head + (1.0f - fac1) * tail, fac2 * tail + (1.0f - fac2) * head, length - head - tail, 16, 1);
		glTranslatef(0.0f, 0.0f, -head);
		
		glDisable(GL_POLYGON_OFFSET_FILL);
		
=======

	fac1 = (length - head) / length;
	fac2 = (length - tail) / length;

	if (length > (head + tail)) {
		if (id != -1)
			GPU_select_load_id(id | ELEMENTSEL_ELEMENT);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(-1.0f, -1.0f);

		glTranslatef(0.0f, 0.0f, head);
		gluCylinder(qobj, fac1 * head + (1.0f - fac1) * tail, fac2 * tail + (1.0f - fac2) * head, length - head - tail, 16, 1);
		glTranslatef(0.0f, 0.0f, -head);

		glDisable(GL_POLYGON_OFFSET_FILL);

>>>>>>> Initial commit
		/* draw sphere on extrema */
		glTranslatef(0.0f, 0.0f, length - tail);
		gluSphere(qobj, fac2 * tail + (1.0f - fac2) * head, 16, 10);
		glTranslatef(0.0f, 0.0f, -length + tail);
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		glTranslatef(0.0f, 0.0f, head);
		gluSphere(qobj, fac1 * head + (1.0f - fac1) * tail, 16, 10);
	}
	else {
		/* 1 sphere in center */
		glTranslatef(0.0f, 0.0f, (head + length - tail) / 2.0f);
		gluSphere(qobj, fac1 * head + (1.0f - fac1) * tail, 16, 10);
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* restore */
	if (dt == OB_SOLID) {
		glShadeModel(GL_FLAT);
		glDisable(GL_LIGHTING);
		glDisable(GL_COLOR_MATERIAL);
	}
<<<<<<< HEAD
	
	glPopMatrix();
	gluDeleteQuadric(qobj);  
=======

	glPopMatrix();
	gluDeleteQuadric(qobj);
>>>>>>> Initial commit
}

static GLubyte bm_dot6[] = {0x0, 0x18, 0x3C, 0x7E, 0x7E, 0x3C, 0x18, 0x0};
static GLubyte bm_dot8[] = {0x3C, 0x7E, 0xFF, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C};

static GLubyte bm_dot5[] = {0x0, 0x0, 0x10, 0x38, 0x7c, 0x38, 0x10, 0x0};
static GLubyte bm_dot7[] = {0x0, 0x38, 0x7C, 0xFE, 0xFE, 0xFE, 0x7C, 0x38};

<<<<<<< HEAD

static void draw_line_bone(int armflag, int boneflag, short constflag, unsigned int id,
                           bPoseChannel *pchan, EditBone *ebone)
{
	float length;
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	if (pchan) 
		length = pchan->bone->length;
	else 
		length = ebone->length;
	
	glPushMatrix();
	glScalef(length, length, length);
	
=======
static void draw_line_element(int armflag, int boneflag, short constflag, unsigned int id,
                           bPoseChannel *pchan, EditArmatureElement *eelement)
{
	float length;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if (pchan)
		length = pchan->bone->length;
	else
		length = eelement->length;

	glPushMatrix();
	glScalef(length, length, length);

>>>>>>> Initial commit
	/* this chunk not in object mode */
	if (armflag & (ARM_EDITMODE | ARM_POSEMODE)) {
		glLineWidth(4.0f);
		if (armflag & ARM_POSEMODE)
			set_pchan_glColor(PCHAN_COLOR_NORMAL, boneflag, constflag);
		else if (armflag & ARM_EDITMODE) {
			UI_ThemeColor(TH_WIRE_EDIT);
		}
<<<<<<< HEAD
		
		/*	Draw root point if we are not connected */
		if ((boneflag & BONE_CONNECTED) == 0) {
			if (G.f & G_PICKSEL) {  /* no bitmap in selection mode, crashes 3d cards... */
				GPU_select_load_id(id | BONESEL_ROOT);
=======

		/*	Draw root point if we are not connected */
		if ((boneflag & ELEMENT_CONNECTED) == 0) {
			if (G.f & G_PICKSEL) {  /* no bitmap in selection mode, crashes 3d cards... */
				GPU_select_load_id(id | ELEMENTSEL_ROOT);
>>>>>>> Initial commit
				glBegin(GL_POINTS);
				glVertex3f(0.0f, 0.0f, 0.0f);
				glEnd();
			}
			else {
				glRasterPos3f(0.0f, 0.0f, 0.0f);
				glBitmap(8, 8, 4, 4, 0, 0, bm_dot8);
			}
		}
<<<<<<< HEAD
		
		if (id != -1)
			GPU_select_load_id((GLuint) id | BONESEL_BONE);
		
=======

		if (id != -1)
			GPU_select_load_id((GLuint) id | ELEMENTSEL_ELEMENT);

>>>>>>> Initial commit
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
		glEnd();
<<<<<<< HEAD
		
		/* tip */
		if (G.f & G_PICKSEL) {
			/* no bitmap in selection mode, crashes 3d cards... */
			GPU_select_load_id(id | BONESEL_TIP);
=======

		/* tip */
		if (G.f & G_PICKSEL) {
			/* no bitmap in selection mode, crashes 3d cards... */
			GPU_select_load_id(id | ELEMENTSEL_TIP);
>>>>>>> Initial commit
			glBegin(GL_POINTS);
			glVertex3f(0.0f, 1.0f, 0.0f);
			glEnd();
		}
		else {
			glRasterPos3f(0.0f, 1.0f, 0.0f);
			glBitmap(8, 8, 4, 4, 0, 0, bm_dot7);
		}
<<<<<<< HEAD
		
		/* further we send no names */
		if (id != -1)
			GPU_select_load_id(id & 0xFFFF);  /* object tag, for bordersel optim */
		
		if (armflag & ARM_POSEMODE)
			set_pchan_glColor(PCHAN_COLOR_LINEBONE, boneflag, constflag);
	}
	
	glLineWidth(2.0);
	
	/*Draw root point if we are not connected */
	if ((boneflag & BONE_CONNECTED) == 0) {
		if ((G.f & G_PICKSEL) == 0) {
			/* no bitmap in selection mode, crashes 3d cards... */
			if (armflag & ARM_EDITMODE) {
				if (boneflag & BONE_ROOTSEL) UI_ThemeColor(TH_VERTEX_SELECT);
=======

		/* further we send no names */
		if (id != -1)
			GPU_select_load_id(id & 0xFFFF);  /* object tag, for bordersel optim */

		if (armflag & ARM_POSEMODE)
			set_pchan_glColor(PCHAN_COLOR_LINEBONE, boneflag, constflag);
	}

	glLineWidth(2.0);

	/*Draw root point if we are not connected */
	if ((boneflag & ELEMENT_CONNECTED) == 0) {
		if ((G.f & G_PICKSEL) == 0) {
			/* no bitmap in selection mode, crashes 3d cards... */
			if (armflag & ARM_EDITMODE) {
				if (boneflag & ELEMENT_ROOTSEL) UI_ThemeColor(TH_VERTEX_SELECT);
>>>>>>> Initial commit
				else UI_ThemeColor(TH_VERTEX);
			}
			glRasterPos3f(0.0f, 0.0f, 0.0f);
			glBitmap(8, 8, 4, 4, 0, 0, bm_dot6);
		}
	}
<<<<<<< HEAD
	
	if (armflag & ARM_EDITMODE) {
		if (boneflag & BONE_SELECTED) UI_ThemeColor(TH_EDGE_SELECT);
=======

	if (armflag & ARM_EDITMODE) {
		if (boneflag & ELEMENT_SELECTED) UI_ThemeColor(TH_EDGE_SELECT);
>>>>>>> Initial commit
		else UI_ThemeColorShade(TH_BACK, -30);
	}
	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glEnd();
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* tip */
	if ((G.f & G_PICKSEL) == 0) {
		/* no bitmap in selection mode, crashes 3d cards... */
		if (armflag & ARM_EDITMODE) {
<<<<<<< HEAD
			if (boneflag & BONE_TIPSEL) UI_ThemeColor(TH_VERTEX_SELECT);
=======
			if (boneflag & ELEMENT_TIPSEL) UI_ThemeColor(TH_VERTEX_SELECT);
>>>>>>> Initial commit
			else UI_ThemeColor(TH_VERTEX);
		}
		glRasterPos3f(0.0f, 1.0f, 0.0f);
		glBitmap(8, 8, 4, 4, 0, 0, bm_dot5);
	}
<<<<<<< HEAD
	
	glLineWidth(1.0);
	
	glPopMatrix();
}

static void draw_b_bone_boxes(const short dt, bPoseChannel *pchan, float xwidth, float length, float zwidth)
{
	int segments = 0;
	
	if (pchan) 
		segments = pchan->bone->segments;
	
=======

	glLineWidth(1.0);

	glPopMatrix();
}

static void draw_b_element_boxes(const short dt, bPoseChannel *pchan, float xwidth, float length, float zwidth)
{
	int segments = 0;

	if (pchan)
		segments = pchan->bone->segments;

>>>>>>> Initial commit
	if ((segments > 1) && (pchan)) {
		float dlen = length / (float)segments;
		Mat4 bbone[MAX_BBONE_SUBDIV];
		int a;

		b_bone_spline_setup(pchan, 0, bbone);

		for (a = 0; a < segments; a++) {
			glPushMatrix();
			glMultMatrixf(bbone[a].mat);
			if (dt == OB_SOLID) drawsolidcube_size(xwidth, dlen, zwidth);
			else drawcube_size(xwidth, dlen, zwidth);
			glPopMatrix();
		}
	}
	else {
		glPushMatrix();
		if (dt == OB_SOLID) drawsolidcube_size(xwidth, length, zwidth);
		else drawcube_size(xwidth, length, zwidth);
		glPopMatrix();
	}
}

<<<<<<< HEAD
static void draw_b_bone(const short dt, int armflag, int boneflag, short constflag, unsigned int id,
                        bPoseChannel *pchan, EditBone *ebone)
{
	float xwidth, length, zwidth;
	
=======
static void draw_b_element(const short dt, int armflag, int boneflag, short constflag, unsigned int id,
                        bPoseChannel *pchan, EditArmatureElement *eelement)
{
	float xwidth, length, zwidth;

>>>>>>> Initial commit
	if (pchan) {
		xwidth = pchan->bone->xwidth;
		length = pchan->bone->length;
		zwidth = pchan->bone->zwidth;
	}
	else {
<<<<<<< HEAD
		xwidth = ebone->xwidth;
		length = ebone->length;
		zwidth = ebone->zwidth;
	}
	
=======
		xwidth = eelement->xwidth;
		length = eelement->length;
		zwidth = eelement->zwidth;
	}

>>>>>>> Initial commit
	/* draw points only if... */
	if (armflag & ARM_EDITMODE) {
		/* move to unitspace */
		glPushMatrix();
		glScalef(length, length, length);
		draw_bone_points(dt, armflag, boneflag, id);
		glPopMatrix();
		length *= 0.95f;  /* make vertices visible */
	}

	/* colors for modes */
	if (armflag & ARM_POSEMODE) {
		if (dt <= OB_WIRE)
			set_pchan_glColor(PCHAN_COLOR_NORMAL, boneflag, constflag);
<<<<<<< HEAD
		else 
=======
		else
>>>>>>> Initial commit
			set_pchan_glColor(PCHAN_COLOR_SOLID, boneflag, constflag);
	}
	else if (armflag & ARM_EDITMODE) {
		if (dt == OB_WIRE) {
			set_ebone_glColor(boneflag);
		}
<<<<<<< HEAD
		else 
			UI_ThemeColor(TH_BONE_SOLID);
	}
	
	if (id != -1) {
		GPU_select_load_id((GLuint) id | BONESEL_BONE);
	}
	
=======
		else
			UI_ThemeColor(TH_BONE_SOLID);
	}

	if (id != -1) {
		GPU_select_load_id((GLuint) id | ELEMENTSEL_ELEMENT);
	}

>>>>>>> Initial commit
	/* set up solid drawing */
	if (dt > OB_WIRE) {
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_LIGHTING);
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		if (armflag & ARM_POSEMODE)
			set_pchan_glColor(PCHAN_COLOR_SOLID, boneflag, constflag);
		else
			UI_ThemeColor(TH_BONE_SOLID);
<<<<<<< HEAD
		
		draw_b_bone_boxes(OB_SOLID, pchan, xwidth, length, zwidth);
		
=======

		draw_b_element_boxes(OB_SOLID, pchan, xwidth, length, zwidth);

>>>>>>> Initial commit
		/* disable solid drawing */
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_LIGHTING);
	}
	else {
		/* wire */
		if (armflag & ARM_POSEMODE) {
			if (constflag) {
				/* set constraint colors */
				if (set_pchan_glColor(PCHAN_COLOR_CONSTS, boneflag, constflag)) {
					glEnable(GL_BLEND);
<<<<<<< HEAD
					
					draw_b_bone_boxes(OB_SOLID, pchan, xwidth, length, zwidth);
					
					glDisable(GL_BLEND);
				}
				
=======

					draw_b_element_boxes(OB_SOLID, pchan, xwidth, length, zwidth);

					glDisable(GL_BLEND);
				}

>>>>>>> Initial commit
				/* restore colors */
				set_pchan_glColor(PCHAN_COLOR_NORMAL, boneflag, constflag);
			}
		}
<<<<<<< HEAD
		
		draw_b_bone_boxes(OB_WIRE, pchan, xwidth, length, zwidth);
=======

		draw_b_element_boxes(OB_WIRE, pchan, xwidth, length, zwidth);
>>>>>>> Initial commit
	}
}

static void draw_wire_bone_segments(bPoseChannel *pchan, Mat4 *bbones, float length, int segments)
{
	if ((segments > 1) && (pchan)) {
		float dlen = length / (float)segments;
		Mat4 *bbone = bbones;
		int a;
<<<<<<< HEAD
		
		for (a = 0; a < segments; a++, bbone++) {
			glPushMatrix();
			glMultMatrixf(bbone->mat);
			
=======

		for (a = 0; a < segments; a++, bbone++) {
			glPushMatrix();
			glMultMatrixf(bbone->mat);

>>>>>>> Initial commit
			glBegin(GL_LINES);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, dlen, 0.0f);
			glEnd();  /* GL_LINES */
<<<<<<< HEAD
			
=======

>>>>>>> Initial commit
			glPopMatrix();
		}
	}
	else {
		glPushMatrix();
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		glBegin(GL_LINES);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, length, 0.0f);
		glEnd();
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		glPopMatrix();
	}
}

<<<<<<< HEAD
static void draw_wire_bone(const short dt, int armflag, int boneflag, short constflag, unsigned int id,
                           bPoseChannel *pchan, EditBone *ebone)
=======
static void draw_wire_element(const short dt, int armflag, int boneflag, short constflag, unsigned int id,
                           bPoseChannel *pchan, EditArmatureElement *eelement)
>>>>>>> Initial commit
{
	Mat4 bbones_array[MAX_BBONE_SUBDIV];
	Mat4 *bbones = NULL;
	int segments = 0;
	float length;
<<<<<<< HEAD
	
	if (pchan) {
		segments = pchan->bone->segments;
		length = pchan->bone->length;
		
=======

	if (pchan) {
		segments = pchan->bone->segments;
		length = pchan->bone->length;

>>>>>>> Initial commit
		if (segments > 1) {
			b_bone_spline_setup(pchan, 0, bbones_array);
			bbones = bbones_array;
		}
	}
<<<<<<< HEAD
	else 
		length = ebone->length;
	
=======
	else
		length = eelement->length;

>>>>>>> Initial commit
	/* draw points only if... */
	if (armflag & ARM_EDITMODE) {
		/* move to unitspace */
		glPushMatrix();
		glScalef(length, length, length);
		draw_bone_points(dt, armflag, boneflag, id);
		glPopMatrix();
		length *= 0.95f;  /* make vertices visible */
	}
<<<<<<< HEAD
	
	/* this chunk not in object mode */
	if (armflag & (ARM_EDITMODE | ARM_POSEMODE)) {
		if (id != -1)
			GPU_select_load_id((GLuint) id | BONESEL_BONE);
		
		draw_wire_bone_segments(pchan, bbones, length, segments);
		
=======

	/* this chunk not in object mode */
	if (armflag & (ARM_EDITMODE | ARM_POSEMODE)) {
		if (id != -1)
			GPU_select_load_id((GLuint) id | ELEMENTSEL_ELEMENT);

		draw_wire_bone_segments(pchan, bbones, length, segments);

>>>>>>> Initial commit
		/* further we send no names */
		if (id != -1)
			GPU_select_load_id(id & 0xFFFF);    /* object tag, for bordersel optim */
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* colors for modes */
	if (armflag & ARM_POSEMODE) {
		set_pchan_glColor(PCHAN_COLOR_NORMAL, boneflag, constflag);
	}
	else if (armflag & ARM_EDITMODE) {
		set_ebone_glColor(boneflag);
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* draw normal */
	draw_wire_bone_segments(pchan, bbones, length, segments);
}

<<<<<<< HEAD
static void draw_bone(const short dt, int armflag, int boneflag, short constflag, unsigned int id, float length)
{
	
	/* Draw a 3d octahedral bone, we use normalized space based on length,
	 * for display-lists */
	
=======
static void draw_element(const short dt, int armflag, int boneflag, short constflag, unsigned int id, float length)
{

	/* Draw a 3d octahedral bone, we use normalized space based on length,
	 * for display-lists */

>>>>>>> Initial commit
	glScalef(length, length, length);

	/* set up solid drawing */
	if (dt > OB_WIRE) {
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_LIGHTING);
		UI_ThemeColor(TH_BONE_SOLID);
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* colors for posemode */
	if (armflag & ARM_POSEMODE) {
		if (dt <= OB_WIRE)
			set_pchan_glColor(PCHAN_COLOR_NORMAL, boneflag, constflag);
<<<<<<< HEAD
		else 
			set_pchan_glColor(PCHAN_COLOR_SOLID, boneflag, constflag);
	}
	
	
	draw_bone_points(dt, armflag, boneflag, id);
	
	/* now draw the bone itself */
	if (id != -1) {
		GPU_select_load_id((GLuint) id | BONESEL_BONE);
	}
	
=======
		else
			set_pchan_glColor(PCHAN_COLOR_SOLID, boneflag, constflag);
	}


	draw_bone_points(dt, armflag, boneflag, id);

	/* now draw the bone itself */
	if (id != -1) {
		GPU_select_load_id((GLuint) id | ELEMENTSEL_ELEMENT);
	}

>>>>>>> Initial commit
	/* wire? */
	if (dt <= OB_WIRE) {
		/* colors */
		if (armflag & ARM_EDITMODE) {
			set_ebone_glColor(boneflag);
		}
		else if (armflag & ARM_POSEMODE) {
			if (constflag) {
				/* draw constraint colors */
				if (set_pchan_glColor(PCHAN_COLOR_CONSTS, boneflag, constflag)) {
					glEnable(GL_BLEND);
<<<<<<< HEAD
					
					draw_bone_solid_octahedral();
					
					glDisable(GL_BLEND);
				}
				
=======

					draw_bone_solid_octahedral();

					glDisable(GL_BLEND);
				}

>>>>>>> Initial commit
				/* restore colors */
				set_pchan_glColor(PCHAN_COLOR_NORMAL, boneflag, constflag);
			}
		}
		draw_bone_octahedral();
	}
	else {
		/* solid */
		if (armflag & ARM_POSEMODE)
			set_pchan_glColor(PCHAN_COLOR_SOLID, boneflag, constflag);
		else
			UI_ThemeColor(TH_BONE_SOLID);
		draw_bone_solid_octahedral();
	}

	/* disable solid drawing */
	if (dt > OB_WIRE) {
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_LIGHTING);
	}
}

static void draw_custom_bone(Scene *scene, View3D *v3d, RegionView3D *rv3d, Object *ob,
                             const short dt, int armflag, int boneflag, unsigned int id, float length)
{
	if (ob == NULL) return;
<<<<<<< HEAD
	
	glScalef(length, length, length);
	
=======

	glScalef(length, length, length);

>>>>>>> Initial commit
	/* colors for posemode */
	if (armflag & ARM_POSEMODE) {
		set_pchan_glColor(PCHAN_COLOR_NORMAL, boneflag, 0);
	}
<<<<<<< HEAD
	
	if (id != -1) {
		GPU_select_load_id((GLuint) id | BONESEL_BONE);
	}
	
=======

	if (id != -1) {
		GPU_select_load_id((GLuint) id | ELEMENTSEL_ELEMENT);
	}

>>>>>>> Initial commit
	draw_object_instance(scene, v3d, rv3d, ob, dt, armflag & ARM_POSEMODE);
}


static void pchan_draw_IK_root_lines(bPoseChannel *pchan, short only_temp)
{
	bConstraint *con;
	bPoseChannel *parchan;
<<<<<<< HEAD
	
	for (con = pchan->constraints.first; con; con = con->next) {
		if (con->enforce == 0.0f)
			continue;
		
=======

	for (con = pchan->constraints.first; con; con = con->next) {
		if (con->enforce == 0.0f)
			continue;

>>>>>>> Initial commit
		switch (con->type) {
			case CONSTRAINT_TYPE_KINEMATIC:
			{
				bKinematicConstraint *data = (bKinematicConstraint *)con->data;
				int segcount = 0;
<<<<<<< HEAD
				
				/* if only_temp, only draw if it is a temporary ik-chain */
				if ((only_temp) && !(data->flag & CONSTRAINT_IK_TEMP))
					continue;
				
				setlinestyle(3);
				glBegin(GL_LINES);
				
=======

				/* if only_temp, only draw if it is a temporary ik-chain */
				if ((only_temp) && !(data->flag & CONSTRAINT_IK_TEMP))
					continue;

				setlinestyle(3);
				glBegin(GL_LINES);

>>>>>>> Initial commit
				/* exclude tip from chain? */
				if ((data->flag & CONSTRAINT_IK_TIP) == 0)
					parchan = pchan->parent;
				else
					parchan = pchan;
<<<<<<< HEAD
				
				glVertex3fv(parchan->pose_tail);
				
=======

				glVertex3fv(parchan->pose_tail);

>>>>>>> Initial commit
				/* Find the chain's root */
				while (parchan->parent) {
					segcount++;
					if (segcount == data->rootbone || segcount > 255) {
						break;  /* 255 is weak */
					}
					parchan = parchan->parent;
				}
				if (parchan)
					glVertex3fv(parchan->pose_head);
<<<<<<< HEAD
				
=======

>>>>>>> Initial commit
				glEnd();
				setlinestyle(0);
				break;
			}
<<<<<<< HEAD
			case CONSTRAINT_TYPE_SPLINEIK: 
			{
				bSplineIKConstraint *data = (bSplineIKConstraint *)con->data;
				int segcount = 0;
				
				setlinestyle(3);
				glBegin(GL_LINES);
				
				parchan = pchan;
				glVertex3fv(parchan->pose_tail);
				
=======
			case CONSTRAINT_TYPE_SPLINEIK:
			{
				bSplineIKConstraint *data = (bSplineIKConstraint *)con->data;
				int segcount = 0;

				setlinestyle(3);
				glBegin(GL_LINES);

				parchan = pchan;
				glVertex3fv(parchan->pose_tail);

>>>>>>> Initial commit
				/* Find the chain's root */
				while (parchan->parent) {
					segcount++;
					/* FIXME: revise the breaking conditions */
					if (segcount == data->chainlen || segcount > 255) break;  /* 255 is weak */
					parchan = parchan->parent;
				}
				if (parchan)  /* XXX revise the breaking conditions to only stop at the tail? */
					glVertex3fv(parchan->pose_head);

				glEnd();
				setlinestyle(0);
				break;
			}
		}
	}
}

static void bgl_sphere_project(float ax, float az)
{
	float dir[3], sine, q3;

	sine = 1.0f - ax * ax - az * az;
	q3 = (sine < 0.0f) ? 0.0f : (2.0f * sqrtf(sine));

	dir[0] = -az * q3;
	dir[1] = 1.0f - 2.0f * sine;
	dir[2] = ax * q3;

	glVertex3fv(dir);
}

static void draw_dof_ellipse(float ax, float az)
{
	const float staticSine[16] = {
		0.0f, 0.104528463268f, 0.207911690818f, 0.309016994375f,
		0.406736643076f, 0.5f, 0.587785252292f, 0.669130606359f,
		0.743144825477f, 0.809016994375f, 0.866025403784f,
		0.913545457643f, 0.951056516295f, 0.978147600734f,
		0.994521895368f, 1.0f
	};

	int i, j, n = 16;
	float x, z, px, pz;

	glEnable(GL_BLEND);
	glDepthMask(0);

	glColor4ub(70, 70, 70, 50);

	glBegin(GL_QUADS);
	pz = 0.0f;
	for (i = 1; i < n; i++) {
		z = staticSine[i];
<<<<<<< HEAD
		
		px = 0.0f;
		for (j = 1; j <= (n - i); j++) {
			x = staticSine[j];
			
=======

		px = 0.0f;
		for (j = 1; j <= (n - i); j++) {
			x = staticSine[j];

>>>>>>> Initial commit
			if (j == n - i) {
				glEnd();
				glBegin(GL_TRIANGLES);
				bgl_sphere_project(ax * px, az * z);
				bgl_sphere_project(ax * px, az * pz);
				bgl_sphere_project(ax * x, az * pz);
				glEnd();
				glBegin(GL_QUADS);
			}
			else {
				bgl_sphere_project(ax * x, az * z);
				bgl_sphere_project(ax * x, az * pz);
				bgl_sphere_project(ax * px, az * pz);
				bgl_sphere_project(ax * px, az * z);
			}
<<<<<<< HEAD
			
=======

>>>>>>> Initial commit
			px = x;
		}
		pz = z;
	}
	glEnd();

	glDisable(GL_BLEND);
	glDepthMask(1);

	glColor3ub(0, 0, 0);

	glBegin(GL_LINE_STRIP);
	for (i = 0; i < n; i++)
		bgl_sphere_project(staticSine[n - i - 1] * ax, staticSine[i] * az);
	glEnd();
}

static void draw_pose_dofs(Object *ob)
{
	bArmature *arm = ob->data;
	bPoseChannel *pchan;
<<<<<<< HEAD
	Bone *bone;
	
	for (pchan = ob->pose->chanbase.first; pchan; pchan = pchan->next) {
		bone = pchan->bone;
		
		if ((bone != NULL) && !(bone->flag & (BONE_HIDDEN_P | BONE_HIDDEN_PG))) {
			if (bone->flag & BONE_SELECTED) {
				if (bone->layer & arm->layer) {
=======
	ArmatureElement *element;

	for (pchan = ob->pose->chanbase.first; pchan; pchan = pchan->next) {
		element = pchan->bone;

		if ((element != NULL) && !(element->flag & (ELEMENT_HIDDEN_P | ELEMENT_HIDDEN_PG))) {
			if (element->flag & ELEMENT_SELECTED) {
				if (element->layer & arm->layer) {
>>>>>>> Initial commit
					if (pchan->ikflag & (BONE_IK_XLIMIT | BONE_IK_ZLIMIT)) {
						if (BKE_pose_channel_in_IK_chain(ob, pchan)) {
							float corner[4][3], posetrans[3], mat[4][4];
							float phi = 0.0f, theta = 0.0f, scale;
							int a, i;
<<<<<<< HEAD
							
							/* in parent-bone pose, but own restspace */
							glPushMatrix();
							
							copy_v3_v3(posetrans, pchan->pose_mat[3]);
							glTranslatef(posetrans[0], posetrans[1], posetrans[2]);
							
=======

							/* in parent-bone pose, but own restspace */
							glPushMatrix();

							copy_v3_v3(posetrans, pchan->pose_mat[3]);
							glTranslatef(posetrans[0], posetrans[1], posetrans[2]);

>>>>>>> Initial commit
							if (pchan->parent) {
								copy_m4_m4(mat, pchan->parent->pose_mat);
								mat[3][0] = mat[3][1] = mat[3][2] = 0.0f;
								glMultMatrixf(mat);
							}
<<<<<<< HEAD
							
							copy_m4_m3(mat, pchan->bone->bone_mat);
							glMultMatrixf(mat);
							
							scale = bone->length * pchan->size[1];
							glScalef(scale, scale, scale);
							
							if (pchan->ikflag & BONE_IK_XLIMIT) {
								if (pchan->ikflag & BONE_IK_ZLIMIT) {
									float amin[3], amax[3];
									
=======

							copy_m4_m3(mat, pchan->bone->AE_mat);
							glMultMatrixf(mat);

							scale = element->length * pchan->size[1];
							glScalef(scale, scale, scale);

							if (pchan->ikflag & BONE_IK_XLIMIT) {
								if (pchan->ikflag & BONE_IK_ZLIMIT) {
									float amin[3], amax[3];

>>>>>>> Initial commit
									for (i = 0; i < 3; i++) {
										/* *0.5f here comes from M_PI/360.0f when rotations were still in degrees */
										amin[i] = sinf(pchan->limitmin[i] * 0.5f);
										amax[i] = sinf(pchan->limitmax[i] * 0.5f);
									}
<<<<<<< HEAD
									
=======

>>>>>>> Initial commit
									glScalef(1.0f, -1.0f, 1.0f);
									if ((amin[0] != 0.0f) && (amin[2] != 0.0f))
										draw_dof_ellipse(amin[0], amin[2]);
									if ((amin[0] != 0.0f) && (amax[2] != 0.0f))
										draw_dof_ellipse(amin[0], amax[2]);
									if ((amax[0] != 0.0f) && (amin[2] != 0.0f))
										draw_dof_ellipse(amax[0], amin[2]);
									if ((amax[0] != 0.0f) && (amax[2] != 0.0f))
										draw_dof_ellipse(amax[0], amax[2]);
									glScalef(1.0f, -1.0f, 1.0f);
								}
							}
<<<<<<< HEAD
							
=======

>>>>>>> Initial commit
							/* arcs */
							if (pchan->ikflag & BONE_IK_ZLIMIT) {
								/* OpenGL requires rotations in degrees; so we're taking the average angle here */
								theta = RAD2DEGF(0.5f * (pchan->limitmin[2] + pchan->limitmax[2]));
								glRotatef(theta, 0.0f, 0.0f, 1.0f);
<<<<<<< HEAD
								
=======

>>>>>>> Initial commit
								glColor3ub(50, 50, 255);  /* blue, Z axis limit */
								glBegin(GL_LINE_STRIP);
								for (a = -16; a <= 16; a++) {
									/* *0.5f here comes from M_PI/360.0f when rotations were still in degrees */
									float fac = ((float)a) / 16.0f * 0.5f;
<<<<<<< HEAD
									
									phi = fac * (pchan->limitmax[2] - pchan->limitmin[2]);
									
=======

									phi = fac * (pchan->limitmax[2] - pchan->limitmin[2]);

>>>>>>> Initial commit
									i = (a == -16) ? 0 : 1;
									corner[i][0] = sinf(phi);
									corner[i][1] = cosf(phi);
									corner[i][2] = 0.0f;
									glVertex3fv(corner[i]);
								}
								glEnd();
<<<<<<< HEAD
								
								glRotatef(-theta, 0.0f, 0.0f, 1.0f);
							}
							
=======

								glRotatef(-theta, 0.0f, 0.0f, 1.0f);
							}

>>>>>>> Initial commit
							if (pchan->ikflag & BONE_IK_XLIMIT) {
								/* OpenGL requires rotations in degrees; so we're taking the average angle here */
								theta = RAD2DEGF(0.5f * (pchan->limitmin[0] + pchan->limitmax[0]));
								glRotatef(theta, 1.0f, 0.0f, 0.0f);
<<<<<<< HEAD
								
=======

>>>>>>> Initial commit
								glColor3ub(255, 50, 50);  /* Red, X axis limit */
								glBegin(GL_LINE_STRIP);
								for (a = -16; a <= 16; a++) {
									/* *0.5f here comes from M_PI/360.0f when rotations were still in degrees */
									float fac = ((float)a) / 16.0f * 0.5f;
									phi = (float)(0.5 * M_PI) + fac * (pchan->limitmax[0] - pchan->limitmin[0]);
<<<<<<< HEAD
									
=======

>>>>>>> Initial commit
									i = (a == -16) ? 2 : 3;
									corner[i][0] = 0.0f;
									corner[i][1] = sinf(phi);
									corner[i][2] = cosf(phi);
									glVertex3fv(corner[i]);
								}
								glEnd();
<<<<<<< HEAD
								
								glRotatef(-theta, 1.0f, 0.0f, 0.0f);
							}
							
							/* out of cone, out of bone */
							glPopMatrix(); 
=======

								glRotatef(-theta, 1.0f, 0.0f, 0.0f);
							}

							/* out of cone, out of bone */
							glPopMatrix();
>>>>>>> Initial commit
						}
					}
				}
			}
		}
	}
}

static void bone_matrix_translate_y(float mat[4][4], float y)
{
	float trans[3];

	copy_v3_v3(trans, mat[1]);
	mul_v3_fl(trans, y);
	add_v3_v3(mat[3], trans);
}

/* assumes object is Armature with pose */
static void draw_pose_bones(Scene *scene, View3D *v3d, ARegion *ar, Base *base,
                            const short dt, const unsigned char ob_wire_col[4],
                            const bool do_const_color, const bool is_outline)
{
	RegionView3D *rv3d = ar->regiondata;
	Object *ob = base->object;
	bArmature *arm = ob->data;
	bPoseChannel *pchan;
<<<<<<< HEAD
	Bone *bone;
=======
	ArmatureElement *element;
>>>>>>> Initial commit
	GLfloat tmp;
	float smat[4][4], imat[4][4], bmat[4][4];
	int index = -1;
	short do_dashed = 3;
	bool draw_wire = false;
	int flag;
	bool is_cull_enabled;
<<<<<<< HEAD
	
	/* being set below */
	arm->layer_used = 0;
	
=======

	/* being set below */
	arm->layer_used = 0;

>>>>>>> Initial commit
	/* hacky... prevent outline select from drawing dashed helplines */
	glGetFloatv(GL_LINE_WIDTH, &tmp);
	if (tmp > 1.1f) do_dashed &= ~1;
	if (v3d->flag & V3D_HIDE_HELPLINES) do_dashed &= ~2;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* precalc inverse matrix for drawing screen aligned */
	if (arm->drawtype == ARM_ENVELOPE) {
		/* precalc inverse matrix for drawing screen aligned */
		copy_m4_m4(smat, rv3d->viewmatob);
		mul_mat3_m4_fl(smat, 1.0f / len_v3(ob->obmat[0]));
		invert_m4_m4(imat, smat);
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		/* and draw blended distances */
		if (arm->flag & ARM_POSEMODE) {
			glEnable(GL_BLEND);
			//glShadeModel(GL_SMOOTH);
<<<<<<< HEAD
			
			if (v3d->zbuf) glDisable(GL_DEPTH_TEST);
			
			for (pchan = ob->pose->chanbase.first; pchan; pchan = pchan->next) {
				bone = pchan->bone;
				if (bone) {
					/* 1) bone must be visible, 2) for OpenGL select-drawing cannot have unselectable [#27194] 
					 * NOTE: this is the only case with (NO_DEFORM == 0) flag, as this is for envelope influence drawing
					 */
					if (((bone->flag & (BONE_HIDDEN_P | BONE_NO_DEFORM | BONE_HIDDEN_PG)) == 0) &&
					    ((G.f & G_PICKSEL) == 0 || (bone->flag & BONE_UNSELECTABLE) == 0))
					{
						if (bone->flag & (BONE_SELECTED)) {
							if (bone->layer & arm->layer)
								draw_sphere_bone_dist(smat, imat, pchan, NULL);
=======

			if (v3d->zbuf) glDisable(GL_DEPTH_TEST);

			for (pchan = ob->pose->chanbase.first; pchan; pchan = pchan->next) {
				element = pchan->bone;
				if (element) {
					/* 1) bone must be visible, 2) for OpenGL select-drawing cannot have unselectable [#27194]
					 * NOTE: this is the only case with (NO_DEFORM == 0) flag, as this is for envelope influence drawing
					 */
					if (((element->flag & (ELEMENT_HIDDEN_P | ELEMENT_NO_DEFORM | ELEMENT_HIDDEN_PG)) == 0) &&
					    ((G.f & G_PICKSEL) == 0 || (element->flag & ELEMENT_UNSELECTABLE) == 0))
					{
						if (element->flag & (ELEMENT_SELECTED)) {
							if (element->layer & arm->layer)
								draw_sphere_element_dist(smat, imat, pchan, NULL);
>>>>>>> Initial commit
						}
					}
				}
			}
<<<<<<< HEAD
			
=======

>>>>>>> Initial commit
			if (v3d->zbuf) glEnable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);
			//glShadeModel(GL_FLAT);
		}
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* little speedup, also make sure transparent only draws once */
	glCullFace(GL_BACK);
	if (v3d->flag2 & V3D_BACKFACE_CULLING) {
		glEnable(GL_CULL_FACE);
		is_cull_enabled = true;
	}
	else {
		is_cull_enabled = false;
	}

	/* if solid we draw that first, with selection codes, but without names, axes etc */
	if (dt > OB_WIRE) {
<<<<<<< HEAD
		if (arm->flag & ARM_POSEMODE) 
			index = base->selcol;
		
		for (pchan = ob->pose->chanbase.first; pchan; pchan = pchan->next) {
			bone = pchan->bone;
			arm->layer_used |= bone->layer;
			
			/* 1) bone must be visible, 2) for OpenGL select-drawing cannot have unselectable [#27194] */
			if (((bone->flag & (BONE_HIDDEN_P | BONE_HIDDEN_PG)) == 0) &&
			    ((G.f & G_PICKSEL) == 0 || (bone->flag & BONE_UNSELECTABLE) == 0))
			{
				if (bone->layer & arm->layer) {
					const bool use_custom = (pchan->custom) && !(arm->flag & ARM_NO_CUSTOM);
					glPushMatrix();
					
=======
		if (arm->flag & ARM_POSEMODE)
			index = base->selcol;

		for (pchan = ob->pose->chanbase.first; pchan; pchan = pchan->next) {
			element = pchan->bone;
			arm->layer_used |= element->layer;

			/* 1) bone must be visible, 2) for OpenGL select-drawing cannot have unselectable [#27194] */
			if (((element->flag & (ELEMENT_HIDDEN_P | ELEMENT_HIDDEN_PG)) == 0) &&
			    ((G.f & G_PICKSEL) == 0 || (element->flag & ELEMENT_UNSELECTABLE) == 0))
			{
				if (element->layer & arm->layer) {
					const bool use_custom = (pchan->custom) && !(arm->flag & ARM_NO_CUSTOM);
					glPushMatrix();

>>>>>>> Initial commit
					if (use_custom && pchan->custom_tx) {
						glMultMatrixf(pchan->custom_tx->pose_mat);
					}
					else {
						glMultMatrixf(pchan->pose_mat);
					}
<<<<<<< HEAD
					
					/* catch exception for bone with hidden parent */
					flag = bone->flag;
					if ((bone->parent) && (bone->parent->flag & (BONE_HIDDEN_P | BONE_HIDDEN_PG))) {
						flag &= ~BONE_CONNECTED;
					}
					
					/* set temporary flag for drawing bone as active, but only if selected */
					if (bone == arm->act_bone)
						flag |= BONE_DRAW_ACTIVE;
					
=======

					/* catch exception for bone with hidden parent */
					flag = element->flag;
					if ((element->parent) && (element->parent->flag & (ELEMENT_HIDDEN_P | ELEMENT_HIDDEN_PG))) {
						flag &= ~ELEMENT_CONNECTED;
					}

					/* set temporary flag for drawing bone as active, but only if selected */
					if (element == arm->act_element)
						flag |= ELEMENT_DRAW_ACTIVE;

>>>>>>> Initial commit
					if (do_const_color) {
						/* keep color */
					}
					else {
						/* set color-set to use */
						set_pchan_colorset(ob, pchan);
					}
<<<<<<< HEAD
					
					if (use_custom) {
						/* if drawwire, don't try to draw in solid */
						if (pchan->bone->flag & BONE_DRAWWIRE) {
=======

					if (use_custom) {
						/* if drawwire, don't try to draw in solid */
						if (pchan->bone->flag & ELEMENT_DRAWWIRE) {
>>>>>>> Initial commit
							draw_wire = true;
						}
						else {
							if (is_cull_enabled && (v3d->flag2 & V3D_BACKFACE_CULLING) == 0) {
								is_cull_enabled = false;
								glDisable(GL_CULL_FACE);
							}

							draw_custom_bone(scene, v3d, rv3d, pchan->custom,
<<<<<<< HEAD
							                 OB_SOLID, arm->flag, flag, index, bone->length);
=======
							                 OB_SOLID, arm->flag, flag, index, element->length);
>>>>>>> Initial commit
						}
					}
					else {
						if (is_cull_enabled == false) {
							is_cull_enabled = true;
							glEnable(GL_CULL_FACE);
						}

						if (arm->drawtype == ARM_LINE) {
							/* nothing in solid */
						}
						else if (arm->drawtype == ARM_WIRE) {
							/* nothing in solid */
						}
						else if (arm->drawtype == ARM_ENVELOPE) {
<<<<<<< HEAD
							draw_sphere_bone(OB_SOLID, arm->flag, flag, 0, index, pchan, NULL);
						}
						else if (arm->drawtype == ARM_B_BONE) {
							draw_b_bone(OB_SOLID, arm->flag, flag, 0, index, pchan, NULL);
						}
						else {
							draw_bone(OB_SOLID, arm->flag, flag, 0, index, bone->length);
=======
							draw_sphere_element(OB_SOLID, arm->flag, flag, 0, index, pchan, NULL);
						}
						else if (arm->drawtype == ARM_B_BONE) {
							draw_b_element(OB_SOLID, arm->flag, flag, 0, index, pchan, NULL);
						}
						else {
							draw_element(OB_SOLID, arm->flag, flag, 0, index, element->length);
>>>>>>> Initial commit
						}
					}

					glPopMatrix();
				}
			}
<<<<<<< HEAD
			
			if (index != -1)
				index += 0x10000;  /* pose bones count in higher 2 bytes only */
		}
		
		/* very very confusing... but in object mode, solid draw, we cannot do GPU_select_load_id yet,
		 * stick bones and/or wire custom-shapes are drawn in next loop 
=======

			if (index != -1)
				index += 0x10000;  /* pose bones count in higher 2 bytes only */
		}

		/* very very confusing... but in object mode, solid draw, we cannot do GPU_select_load_id yet,
		 * stick bones and/or wire custom-shapes are drawn in next loop
>>>>>>> Initial commit
		 */
		if (ELEM(arm->drawtype, ARM_LINE, ARM_WIRE) == 0 && (draw_wire == false) && index != -1) {
			/* object tag, for bordersel optim */
			GPU_select_load_id(index & 0xFFFF);
			index = -1;
		}
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* draw custom bone shapes as wireframes */
	if (!(arm->flag & ARM_NO_CUSTOM) &&
	    (draw_wire || (dt <= OB_WIRE)) )
	{
		if (arm->flag & ARM_POSEMODE)
			index = base->selcol;
<<<<<<< HEAD
			
		/* only draw custom bone shapes that need to be drawn as wires */
		for (pchan = ob->pose->chanbase.first; pchan; pchan = pchan->next) {
			bone = pchan->bone;
			
			/* 1) bone must be visible, 2) for OpenGL select-drawing cannot have unselectable [#27194] */
			if (((bone->flag & (BONE_HIDDEN_P | BONE_HIDDEN_PG)) == 0) &&
			    ((G.f & G_PICKSEL) == 0 || (bone->flag & BONE_UNSELECTABLE) == 0) )
			{
				if (bone->layer & arm->layer) {
					if (pchan->custom) {
						if ((dt < OB_SOLID) || (bone->flag & BONE_DRAWWIRE)) {
							glPushMatrix();
							
=======

		/* only draw custom bone shapes that need to be drawn as wires */
		for (pchan = ob->pose->chanbase.first; pchan; pchan = pchan->next) {
			element = pchan->bone;

			/* 1) bone must be visible, 2) for OpenGL select-drawing cannot have unselectable [#27194] */
			if (((element->flag & (ELEMENT_HIDDEN_P | ELEMENT_HIDDEN_PG)) == 0) &&
			    ((G.f & G_PICKSEL) == 0 || (element->flag & ELEMENT_UNSELECTABLE) == 0) )
			{
				if (element->layer & arm->layer) {
					if (pchan->custom) {
						if ((dt < OB_SOLID) || (element->flag & ELEMENT_DRAWWIRE)) {
							glPushMatrix();

>>>>>>> Initial commit
							if (pchan->custom_tx) {
								glMultMatrixf(pchan->custom_tx->pose_mat);
							}
							else {
								glMultMatrixf(pchan->pose_mat);
							}
<<<<<<< HEAD
							
=======

>>>>>>> Initial commit
							/* prepare colors */
							if (do_const_color) {
								/* 13 October 2009, Disabled this to make ghosting show the right colors (Aligorith) */
							}
							else if (arm->flag & ARM_POSEMODE)
								set_pchan_colorset(ob, pchan);
							else {
								glColor3ubv(ob_wire_col);
							}
<<<<<<< HEAD
								
							/* catch exception for bone with hidden parent */
							flag = bone->flag;
							if ((bone->parent) && (bone->parent->flag & (BONE_HIDDEN_P | BONE_HIDDEN_PG)))
								flag &= ~BONE_CONNECTED;
								
							/* set temporary flag for drawing bone as active, but only if selected */
							if (bone == arm->act_bone)
								flag |= BONE_DRAW_ACTIVE;
							
							draw_custom_bone(scene, v3d, rv3d, pchan->custom,
							                 OB_WIRE, arm->flag, flag, index, bone->length);
							
=======

							/* catch exception for bone with hidden parent */
							flag = element->flag;
							if ((element->parent) && (element->parent->flag & (ELEMENT_HIDDEN_P | ELEMENT_HIDDEN_PG)))
								flag &= ~ELEMENT_CONNECTED;

							/* set temporary flag for drawing bone as active, but only if selected */
							if (element == arm->act_element)
								flag |= ELEMENT_DRAW_ACTIVE;

							draw_custom_bone(scene, v3d, rv3d, pchan->custom,
							                 OB_WIRE, arm->flag, flag, index, element->length);

>>>>>>> Initial commit
							glPopMatrix();
						}
					}
				}
			}
<<<<<<< HEAD
			
			if (index != -1) 
=======

			if (index != -1)
>>>>>>> Initial commit
				index += 0x10000;  /* pose bones count in higher 2 bytes only */
		}
		/* stick or wire bones have not been drawn yet so don't clear object selection in this case */
		if (ELEM(arm->drawtype, ARM_LINE, ARM_WIRE) == 0 && draw_wire && index != -1) {
			/* object tag, for bordersel optim */
			GPU_select_load_id(index & 0xFFFF);
			index = -1;
		}
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* wire draw over solid only in posemode */
	if ((dt <= OB_WIRE) || (arm->flag & ARM_POSEMODE) || ELEM(arm->drawtype, ARM_LINE, ARM_WIRE)) {
		/* draw line check first. we do selection indices */
		if (ELEM(arm->drawtype, ARM_LINE, ARM_WIRE)) {
<<<<<<< HEAD
			if (arm->flag & ARM_POSEMODE) 
=======
			if (arm->flag & ARM_POSEMODE)
>>>>>>> Initial commit
				index = base->selcol;
		}
		/* if solid && posemode, we draw again with polygonoffset */
		else if ((dt > OB_WIRE) && (arm->flag & ARM_POSEMODE)) {
			ED_view3d_polygon_offset(rv3d, 1.0);
		}
		else {
			/* and we use selection indices if not done yet */
<<<<<<< HEAD
			if (arm->flag & ARM_POSEMODE) 
=======
			if (arm->flag & ARM_POSEMODE)
>>>>>>> Initial commit
				index = base->selcol;
		}

		if (is_cull_enabled == false) {
			is_cull_enabled = true;
			glEnable(GL_CULL_FACE);
		}

		for (pchan = ob->pose->chanbase.first; pchan; pchan = pchan->next) {
<<<<<<< HEAD
			bone = pchan->bone;
			arm->layer_used |= bone->layer;
			
			/* 1) bone must be visible, 2) for OpenGL select-drawing cannot have unselectable [#27194] */
			if (((bone->flag & (BONE_HIDDEN_P | BONE_HIDDEN_PG)) == 0) &&
			    ((G.f & G_PICKSEL) == 0 || (bone->flag & BONE_UNSELECTABLE) == 0))
			{
				if (bone->layer & arm->layer) {
					const short constflag = pchan->constflag;
					if ((do_dashed & 1) && (pchan->parent)) {
						/* Draw a line from our root to the parent's tip 
						 * - only if V3D_HIDE_HELPLINES is enabled...
						 */
						if ((do_dashed & 2) && ((bone->flag & BONE_CONNECTED) == 0)) {
=======
			element = pchan->bone;
			arm->layer_used |= element->layer;

			/* 1) bone must be visible, 2) for OpenGL select-drawing cannot have unselectable [#27194] */
			if (((element->flag & (ELEMENT_HIDDEN_P | ELEMENT_HIDDEN_PG)) == 0) &&
			    ((G.f & G_PICKSEL) == 0 || (element->flag & ELEMENT_UNSELECTABLE) == 0))
			{
				if (element->layer & arm->layer) {
					const short constflag = pchan->constflag;
					if ((do_dashed & 1) && (pchan->parent)) {
						/* Draw a line from our root to the parent's tip
						 * - only if V3D_HIDE_HELPLINES is enabled...
						 */
						if ((do_dashed & 2) && ((element->flag & ELEMENT_CONNECTED) == 0)) {
>>>>>>> Initial commit
							if (arm->flag & ARM_POSEMODE) {
								GPU_select_load_id(index & 0xFFFF);  /* object tag, for bordersel optim */
								UI_ThemeColor(TH_WIRE);
							}
							setlinestyle(3);
							glBegin(GL_LINES);
							glVertex3fv(pchan->pose_head);
							glVertex3fv(pchan->parent->pose_tail);
							glEnd();
							setlinestyle(0);
						}
<<<<<<< HEAD
						
						/* Draw a line to IK root bone 
=======

						/* Draw a line to IK root bone
>>>>>>> Initial commit
						 *  - only if temporary chain (i.e. "autoik")
						 */
						if (arm->flag & ARM_POSEMODE) {
							if (constflag & PCHAN_HAS_IK) {
<<<<<<< HEAD
								if (bone->flag & BONE_SELECTED) {
=======
								if (element->flag & ELEMENT_SELECTED) {
>>>>>>> Initial commit
									if (constflag & PCHAN_HAS_TARGET) glColor3ub(200, 120, 0);
									else glColor3ub(200, 200, 50);  /* add theme! */

									GPU_select_load_id(index & 0xFFFF);
									pchan_draw_IK_root_lines(pchan, !(do_dashed & 2));
								}
							}
							else if (constflag & PCHAN_HAS_SPLINEIK) {
<<<<<<< HEAD
								if (bone->flag & BONE_SELECTED) {
									glColor3ub(150, 200, 50);  /* add theme! */
									
=======
								if (element->flag & ELEMENT_SELECTED) {
									glColor3ub(150, 200, 50);  /* add theme! */

>>>>>>> Initial commit
									GPU_select_load_id(index & 0xFFFF);
									pchan_draw_IK_root_lines(pchan, !(do_dashed & 2));
								}
							}
						}
					}
<<<<<<< HEAD
					
					glPushMatrix();
					if (arm->drawtype != ARM_ENVELOPE)
						glMultMatrixf(pchan->pose_mat);
					
					/* catch exception for bone with hidden parent */
					flag = bone->flag;
					if ((bone->parent) && (bone->parent->flag & (BONE_HIDDEN_P | BONE_HIDDEN_PG)))
						flag &= ~BONE_CONNECTED;
					
					/* set temporary flag for drawing bone as active, but only if selected */
					if (bone == arm->act_bone)
						flag |= BONE_DRAW_ACTIVE;
					
=======

					glPushMatrix();
					if (arm->drawtype != ARM_ENVELOPE)
						glMultMatrixf(pchan->pose_mat);

					/* catch exception for bone with hidden parent */
					flag = element->flag;
					if ((element->parent) && (element->parent->flag & (ELEMENT_HIDDEN_P | ELEMENT_HIDDEN_PG)))
						flag &= ~ELEMENT_CONNECTED;

					/* set temporary flag for drawing bone as active, but only if selected */
					if (element == arm->act_element)
						flag |= ELEMENT_DRAW_ACTIVE;

>>>>>>> Initial commit
					/* extra draw service for pose mode */

					/* set color-set to use */
					if (do_const_color) {
						/* keep color */
					}
					else {
						set_pchan_colorset(ob, pchan);
					}
<<<<<<< HEAD
					
=======

>>>>>>> Initial commit
					if ((pchan->custom) && !(arm->flag & ARM_NO_CUSTOM)) {
						/* custom bone shapes should not be drawn here! */
					}
					else if (arm->drawtype == ARM_ENVELOPE) {
						if (dt < OB_SOLID)
<<<<<<< HEAD
							draw_sphere_bone_wire(smat, imat, arm->flag, flag, constflag, index, pchan, NULL);
					}
					else if (arm->drawtype == ARM_LINE)
						draw_line_bone(arm->flag, flag, constflag, index, pchan, NULL);
					else if (arm->drawtype == ARM_WIRE)
						draw_wire_bone(dt, arm->flag, flag, constflag, index, pchan, NULL);
					else if (arm->drawtype == ARM_B_BONE)
						draw_b_bone(OB_WIRE, arm->flag, flag, constflag, index, pchan, NULL);
					else
						draw_bone(OB_WIRE, arm->flag, flag, constflag, index, bone->length);
					
					glPopMatrix();
				}
			}
			
			/* pose bones count in higher 2 bytes only */
			if (index != -1) 
=======
							draw_sphere_element_wire(smat, imat, arm->flag, flag, constflag, index, pchan, NULL);
					}
					else if (arm->drawtype == ARM_LINE)
						draw_line_element(arm->flag, flag, constflag, index, pchan, NULL);
					else if (arm->drawtype == ARM_WIRE)
						draw_wire_element(dt, arm->flag, flag, constflag, index, pchan, NULL);
					else if (arm->drawtype == ARM_B_BONE)
						draw_b_element(OB_WIRE, arm->flag, flag, constflag, index, pchan, NULL);
					else
						draw_element(OB_WIRE, arm->flag, flag, constflag, index, element->length);

					glPopMatrix();
				}
			}

			/* pose bones count in higher 2 bytes only */
			if (index != -1)
>>>>>>> Initial commit
				index += 0x10000;
		}
		/* restore things */
		if (!ELEM(arm->drawtype, ARM_WIRE, ARM_LINE) && (dt > OB_WIRE) && (arm->flag & ARM_POSEMODE))
			ED_view3d_polygon_offset(rv3d, 0.0);
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* restore */
	if (is_cull_enabled) {
		glDisable(GL_CULL_FACE);
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* draw DoFs */
	if (arm->flag & ARM_POSEMODE) {
		if (((base->flag & OB_FROMDUPLI) == 0) && ((v3d->flag & V3D_HIDE_HELPLINES) == 0)) {
			draw_pose_dofs(ob);
		}
	}

	/* finally names and axes */
	if ((arm->flag & (ARM_DRAWNAMES | ARM_DRAWAXES)) &&
	    (is_outline == 0) &&
	    ((base->flag & OB_FROMDUPLI) == 0))
	{
		/* patch for several 3d cards (IBM mostly) that crash on GL_SELECT with text drawing */
		if ((G.f & G_PICKSEL) == 0) {
			float vec[3];

			unsigned char col[4];
			if (do_const_color) {
				/* so we can draw bone names in current const color */
				float tcol[4];
				glGetFloatv(GL_CURRENT_COLOR, tcol);
				rgb_float_to_uchar(col, tcol);
				col[3] = 255;
			}
			else {
				col[0] = ob_wire_col[0];
				col[1] = ob_wire_col[1];
				col[2] = ob_wire_col[2];
				col[3] = 255;
			}
<<<<<<< HEAD
			
			if (v3d->zbuf) glDisable(GL_DEPTH_TEST);
			
			for (pchan = ob->pose->chanbase.first; pchan; pchan = pchan->next) {
				if ((pchan->bone->flag & (BONE_HIDDEN_P | BONE_HIDDEN_PG)) == 0) {
					if (pchan->bone->layer & arm->layer) {
						if (arm->flag & (ARM_EDITMODE | ARM_POSEMODE)) {
							bone = pchan->bone;
							UI_GetThemeColor3ubv((bone->flag & BONE_SELECTED) ? TH_TEXT_HI : TH_TEXT, col);
=======

			if (v3d->zbuf) glDisable(GL_DEPTH_TEST);

			for (pchan = ob->pose->chanbase.first; pchan; pchan = pchan->next) {
				if ((pchan->bone->flag & (ELEMENT_HIDDEN_P | ELEMENT_HIDDEN_PG)) == 0) {
					if (pchan->bone->layer & arm->layer) {
						if (arm->flag & (ARM_EDITMODE | ARM_POSEMODE)) {
							element = pchan->bone;
							UI_GetThemeColor3ubv((element->flag & ELEMENT_SELECTED) ? TH_TEXT_HI : TH_TEXT, col);
>>>>>>> Initial commit
						}
						else if (dt > OB_WIRE) {
							UI_GetThemeColor3ubv(TH_TEXT, col);
						}
<<<<<<< HEAD
						
=======

>>>>>>> Initial commit
						/*  Draw names of bone  */
						if (arm->flag & ARM_DRAWNAMES) {
							mid_v3_v3v3(vec, pchan->pose_head, pchan->pose_tail);
							view3d_cached_text_draw_add(vec, pchan->name, strlen(pchan->name), 10, 0, col);
						}
<<<<<<< HEAD
						
=======

>>>>>>> Initial commit
						/*	Draw additional axes on the bone tail  */
						if ((arm->flag & ARM_DRAWAXES) && (arm->flag & ARM_POSEMODE)) {
							glPushMatrix();
							copy_m4_m4(bmat, pchan->pose_mat);
							bone_matrix_translate_y(bmat, pchan->bone->length);
							glMultMatrixf(bmat);
<<<<<<< HEAD
							
							glColor3ubv(col);
							drawaxes(pchan->bone->length * 0.25f, OB_ARROWS);
							
=======

							glColor3ubv(col);
							drawaxes(pchan->bone->length * 0.25f, OB_ARROWS);

>>>>>>> Initial commit
							glPopMatrix();
						}
					}
				}
			}
<<<<<<< HEAD
			
=======

>>>>>>> Initial commit
			if (v3d->zbuf) glEnable(GL_DEPTH_TEST);
		}
	}

	if (index != -1) {
		GPU_select_load_id(-1);
	}
}

/* in editmode, we don't store the bone matrix... */
<<<<<<< HEAD
static void get_matrix_editbone(EditBone *ebone, float bmat[4][4])
{
	ebone->length = len_v3v3(ebone->tail, ebone->head);
	ED_armature_ebone_to_mat4(ebone, bmat);
}

static void draw_ebones(View3D *v3d, ARegion *ar, Object *ob, const short dt)
{
	RegionView3D *rv3d = ar->regiondata;
	EditBone *eBone;
=======
static void get_matrix_editarmatureelement(EditArmatureElement *eelement, float bmat[4][4])
{
    eelement->length = len_v3v3(eelement->tail, eelement->head);
    ED_armature_eelement_to_mat4(eelement, bmat);
}

static void draw_earmature_elements(View3D *v3d, ARegion *ar, Object *ob, const short dt)
{
	RegionView3D *rv3d = ar->regiondata;
	EditArmatureElement *eelement;
>>>>>>> Initial commit
	bArmature *arm = ob->data;
	float smat[4][4], imat[4][4], bmat[4][4];
	unsigned int index;
	int flag;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* being set in code below */
	arm->layer_used = 0;

	ED_view3d_check_mats_rv3d(rv3d);

	/* envelope (deform distance) */
	if (arm->drawtype == ARM_ENVELOPE) {
		/* precalc inverse matrix for drawing screen aligned */
		copy_m4_m4(smat, rv3d->viewmatob);
		mul_mat3_m4_fl(smat, 1.0f / len_v3(ob->obmat[0]));
		invert_m4_m4(imat, smat);
<<<<<<< HEAD
		
		/* and draw blended distances */
		glEnable(GL_BLEND);
		//glShadeModel(GL_SMOOTH);
		
		if (v3d->zbuf) glDisable(GL_DEPTH_TEST);

		for (eBone = arm->edbo->first; eBone; eBone = eBone->next) {
			if (eBone->layer & arm->layer) {
				if ((eBone->flag & (BONE_HIDDEN_A | BONE_NO_DEFORM)) == 0) {
					if (eBone->flag & (BONE_SELECTED | BONE_TIPSEL | BONE_ROOTSEL))
						draw_sphere_bone_dist(smat, imat, NULL, eBone);
				}
			}
		}
		
=======

		/* and draw blended distances */
		glEnable(GL_BLEND);
		//glShadeModel(GL_SMOOTH);

		if (v3d->zbuf) glDisable(GL_DEPTH_TEST);

		for (eelement = arm->edbo->first; eelement; eelement = eelement->next) {
			if (eelement->layer & arm->layer) {
				if ((eelement->flag & (ELEMENT_HIDDEN_A | ELEMENT_NO_DEFORM)) == 0) {
					if (eelement->flag & (ELEMENT_SELECTED | ELEMENT_TIPSEL | ELEMENT_ROOTSEL))
						draw_sphere_element_dist(smat, imat, NULL, eelement);
				}
			}
		}

>>>>>>> Initial commit
		if (v3d->zbuf) glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		//glShadeModel(GL_FLAT);
	}
<<<<<<< HEAD
	
	/* if solid we draw it first */
	if ((dt > OB_WIRE) && (arm->drawtype != ARM_LINE)) {
		for (eBone = arm->edbo->first, index = 0; eBone; eBone = eBone->next, index++) {
			if (eBone->layer & arm->layer) {
				if ((eBone->flag & BONE_HIDDEN_A) == 0) {
					glPushMatrix();
					get_matrix_editbone(eBone, bmat);
					glMultMatrixf(bmat);
					
					/* catch exception for bone with hidden parent */
					flag = eBone->flag;
					if ((eBone->parent) && !EBONE_VISIBLE(arm, eBone->parent)) {
						flag &= ~BONE_CONNECTED;
					}
						
					/* set temporary flag for drawing bone as active, but only if selected */
					if (eBone == arm->act_edbone)
						flag |= BONE_DRAW_ACTIVE;
					
					if (arm->drawtype == ARM_ENVELOPE)
						draw_sphere_bone(OB_SOLID, arm->flag, flag, 0, index, NULL, eBone);
					else if (arm->drawtype == ARM_B_BONE)
						draw_b_bone(OB_SOLID, arm->flag, flag, 0, index, NULL, eBone);
					else if (arm->drawtype == ARM_WIRE)
						draw_wire_bone(OB_SOLID, arm->flag, flag, 0, index, NULL, eBone);
					else {
						draw_bone(OB_SOLID, arm->flag, flag, 0, index, eBone->length);
					}
					
=======

	/* if solid we draw it first */
	if ((dt > OB_WIRE) && (arm->drawtype != ARM_LINE)) {
		for (eelement = arm->edbo->first, index = 0; eelement; eelement = eelement->next, index++) {
			if (eelement->layer & arm->layer) {
				if ((eelement->flag & ELEMENT_HIDDEN_A) == 0) {
					glPushMatrix();
					get_matrix_editarmatureelement(eelement, bmat);
					glMultMatrixf(bmat);

					/* catch exception for bone with hidden parent */
					flag = eelement->flag;
					if ((eelement->parent) && !EELEMENT_VISIBLE(arm, eelement->parent)) {
						flag &= ~ELEMENT_CONNECTED;
					}

					/* set temporary flag for drawing bone as active, but only if selected */
					if (eelement == arm->act_edelement)
						flag |= ELEMENT_DRAW_ACTIVE;

					if (arm->drawtype == ARM_ENVELOPE)
						draw_sphere_element(OB_SOLID, arm->flag, flag, 0, index, NULL, eelement);
					else if (arm->drawtype == ARM_B_BONE)
						draw_b_element(OB_SOLID, arm->flag, flag, 0, index, NULL, eelement);
					else if (arm->drawtype == ARM_WIRE)
						draw_wire_element(OB_SOLID, arm->flag, flag, 0, index, NULL, eelement);
					else {
						draw_element(OB_SOLID, arm->flag, flag, 0, index, eelement->length);
					}

>>>>>>> Initial commit
					glPopMatrix();
				}
			}
		}
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* if wire over solid, set offset */
	index = -1;
	GPU_select_load_id(-1);
	if (ELEM(arm->drawtype, ARM_LINE, ARM_WIRE)) {
		if (G.f & G_PICKSEL)
			index = 0;
	}
<<<<<<< HEAD
	else if (dt > OB_WIRE) 
		ED_view3d_polygon_offset(rv3d, 1.0);
	else if (arm->flag & ARM_EDITMODE) 
		index = 0;  /* do selection codes */
	
	for (eBone = arm->edbo->first; eBone; eBone = eBone->next) {
		arm->layer_used |= eBone->layer;
		if (eBone->layer & arm->layer) {
			if ((eBone->flag & BONE_HIDDEN_A) == 0) {
				
				/* catch exception for bone with hidden parent */
				flag = eBone->flag;
				if ((eBone->parent) && !EBONE_VISIBLE(arm, eBone->parent)) {
					flag &= ~BONE_CONNECTED;
				}
					
				/* set temporary flag for drawing bone as active, but only if selected */
				if (eBone == arm->act_edbone)
					flag |= BONE_DRAW_ACTIVE;
				
				if (arm->drawtype == ARM_ENVELOPE) {
					if (dt < OB_SOLID)
						draw_sphere_bone_wire(smat, imat, arm->flag, flag, 0, index, NULL, eBone);
				}
				else {
					glPushMatrix();
					get_matrix_editbone(eBone, bmat);
					glMultMatrixf(bmat);
					
					if (arm->drawtype == ARM_LINE) 
						draw_line_bone(arm->flag, flag, 0, index, NULL, eBone);
					else if (arm->drawtype == ARM_WIRE)
						draw_wire_bone(OB_WIRE, arm->flag, flag, 0, index, NULL, eBone);
					else if (arm->drawtype == ARM_B_BONE)
						draw_b_bone(OB_WIRE, arm->flag, flag, 0, index, NULL, eBone);
					else
						draw_bone(OB_WIRE, arm->flag, flag, 0, index, eBone->length);
					
					glPopMatrix();
				}
				
				/* offset to parent */
				if (eBone->parent) {
					UI_ThemeColor(TH_WIRE_EDIT);
					GPU_select_load_id(-1);  /* -1 here is OK! */
					setlinestyle(3);
					
					glBegin(GL_LINES);
					glVertex3fv(eBone->parent->tail);
					glVertex3fv(eBone->head);
					glEnd();
					
=======
	else if (dt > OB_WIRE)
		ED_view3d_polygon_offset(rv3d, 1.0);
	else if (arm->flag & ARM_EDITMODE)
		index = 0;  /* do selection codes */

	for (eelement = arm->edbo->first; eelement; eelement = eelement->next) {
		arm->layer_used |= eelement->layer;
		if (eelement->layer & arm->layer) {
			if ((eelement->flag & ELEMENT_HIDDEN_A) == 0) {

				/* catch exception for bone with hidden parent */
				flag = eelement->flag;
				if ((eelement->parent) && !EELEMENT_VISIBLE(arm, eelement->parent)) {
					flag &= ~ELEMENT_CONNECTED;
				}

				/* set temporary flag for drawing bone as active, but only if selected */
				if (eelement == arm->act_edelement)
					flag |= ELEMENT_DRAW_ACTIVE;

				if (arm->drawtype == ARM_ENVELOPE) {
					if (dt < OB_SOLID)
						draw_sphere_element_wire(smat, imat, arm->flag, flag, 0, index, NULL, eelement);
				}
				else {
					glPushMatrix();
					get_matrix_editarmatureelement(eelement, bmat);
					glMultMatrixf(bmat);

					if (arm->drawtype == ARM_LINE)
						draw_line_element(arm->flag, flag, 0, index, NULL, eelement);
					else if (arm->drawtype == ARM_WIRE)
						draw_wire_element(OB_WIRE, arm->flag, flag, 0, index, NULL, eelement);
					else if (arm->drawtype == ARM_B_BONE)
						draw_b_element(OB_WIRE, arm->flag, flag, 0, index, NULL, eelement);
					else
						draw_element(OB_WIRE, arm->flag, flag, 0, index, eelement->length);

					glPopMatrix();
				}

				/* offset to parent */
				if (eelement->parent) {
					UI_ThemeColor(TH_WIRE_EDIT);
					GPU_select_load_id(-1);  /* -1 here is OK! */
					setlinestyle(3);

					glBegin(GL_LINES);
					glVertex3fv(eelement->parent->tail);
					glVertex3fv(eelement->head);
					glEnd();

>>>>>>> Initial commit
					setlinestyle(0);
				}
			}
		}
		if (index != -1) index++;
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* restore */
	if (index != -1) {
		GPU_select_load_id(-1);
	}

	if (ELEM(arm->drawtype, ARM_LINE, ARM_WIRE)) {
		/* pass */
	}
	else if (dt > OB_WIRE) {
		ED_view3d_polygon_offset(rv3d, 0.0);
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* finally names and axes */
	if (arm->flag & (ARM_DRAWNAMES | ARM_DRAWAXES)) {
		/* patch for several 3d cards (IBM mostly) that crash on GL_SELECT with text drawing */
		if ((G.f & G_PICKSEL) == 0) {
			float vec[3];
			unsigned char col[4];
			col[3] = 255;
<<<<<<< HEAD
			
			if (v3d->zbuf) glDisable(GL_DEPTH_TEST);
			
			for (eBone = arm->edbo->first; eBone; eBone = eBone->next) {
				if (eBone->layer & arm->layer) {
					if ((eBone->flag & BONE_HIDDEN_A) == 0) {

						UI_GetThemeColor3ubv((eBone->flag & BONE_SELECTED) ? TH_TEXT_HI : TH_TEXT, col);

						/*	Draw name */
						if (arm->flag & ARM_DRAWNAMES) {
							mid_v3_v3v3(vec, eBone->head, eBone->tail);
							glRasterPos3fv(vec);
							view3d_cached_text_draw_add(vec, eBone->name, strlen(eBone->name), 10, 0, col);
=======

			if (v3d->zbuf) glDisable(GL_DEPTH_TEST);

			for (eelement = arm->edbo->first; eelement; eelement = eelement->next) {
				if (eelement->layer & arm->layer) {
					if ((eelement->flag & ELEMENT_HIDDEN_A) == 0) {

						UI_GetThemeColor3ubv((eelement->flag & ELEMENT_SELECTED) ? TH_TEXT_HI : TH_TEXT, col);

						/*	Draw name */
						if (arm->flag & ARM_DRAWNAMES) {
							mid_v3_v3v3(vec, eelement->head, eelement->tail);
							glRasterPos3fv(vec);
							view3d_cached_text_draw_add(vec, eelement->name, strlen(eelement->name), 10, 0, col);
>>>>>>> Initial commit
						}
						/*	Draw additional axes */
						if (arm->flag & ARM_DRAWAXES) {
							glPushMatrix();
<<<<<<< HEAD
							get_matrix_editbone(eBone, bmat);
							bone_matrix_translate_y(bmat, eBone->length);
							glMultMatrixf(bmat);

							glColor3ubv(col);
							drawaxes(eBone->length * 0.25f, OB_ARROWS);
							
							glPopMatrix();
						}
						
					}
				}
			}
			
=======
							get_matrix_editarmatureelement(eelement, bmat);
							bone_matrix_translate_y(bmat, eelement->length);
							glMultMatrixf(bmat);

							glColor3ubv(col);
							drawaxes(eelement->length * 0.25f, OB_ARROWS);

							glPopMatrix();
						}

					}
				}
			}

>>>>>>> Initial commit
			if (v3d->zbuf) glEnable(GL_DEPTH_TEST);
		}
	}
}

/* ****************************** Armature Visualization ******************************** */

/* ---------- Paths --------- */

/* draw bone paths
<<<<<<< HEAD
 *	- in view space 
=======
 *	- in view space
>>>>>>> Initial commit
 */
static void draw_pose_paths(Scene *scene, View3D *v3d, ARegion *ar, Object *ob)
{
	bAnimVizSettings *avs = &ob->pose->avs;
	bArmature *arm = ob->data;
	bPoseChannel *pchan;
<<<<<<< HEAD
	
	/* setup drawing environment for paths */
	draw_motion_paths_init(v3d, ar);
	
=======

	/* setup drawing environment for paths */
	draw_motion_paths_init(v3d, ar);

>>>>>>> Initial commit
	/* draw paths where they exist and they releated bone is visible */
	for (pchan = ob->pose->chanbase.first; pchan; pchan = pchan->next) {
		if ((pchan->bone->layer & arm->layer) && (pchan->mpath))
			draw_motion_path_instance(scene, ob, pchan, avs, pchan->mpath);
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* cleanup after drawing */
	draw_motion_paths_cleanup(v3d);
}


/* ---------- Ghosts --------- */

<<<<<<< HEAD
/* helper function for ghost drawing - sets/removes flags for temporarily 
=======
/* helper function for ghost drawing - sets/removes flags for temporarily
>>>>>>> Initial commit
 * hiding unselected bones while drawing ghosts
 */
static void ghost_poses_tag_unselected(Object *ob, short unset)
{
	bArmature *arm = ob->data;
	bPose *pose = ob->pose;
	bPoseChannel *pchan;
<<<<<<< HEAD
	
	/* don't do anything if no hiding any bones */
	if ((arm->flag & ARM_GHOST_ONLYSEL) == 0)
		return;
		
=======

	/* don't do anything if no hiding any bones */
	if ((arm->flag & ARM_GHOST_ONLYSEL) == 0)
		return;

>>>>>>> Initial commit
	/* loop over all pchans, adding/removing tags as appropriate */
	for (pchan = pose->chanbase.first; pchan; pchan = pchan->next) {
		if ((pchan->bone) && (arm->layer & pchan->bone->layer)) {
			if (unset) {
				/* remove tags from all pchans if cleaning up */
<<<<<<< HEAD
				pchan->bone->flag &= ~BONE_HIDDEN_PG;
			}
			else {
				/* set tags on unselected pchans only */
				if ((pchan->bone->flag & BONE_SELECTED) == 0)
					pchan->bone->flag |= BONE_HIDDEN_PG;
=======
				pchan->bone->flag &= ~ELEMENT_HIDDEN_PG;
			}
			else {
				/* set tags on unselected pchans only */
				if ((pchan->bone->flag & ELEMENT_SELECTED) == 0)
					pchan->bone->flag |= ELEMENT_HIDDEN_PG;
>>>>>>> Initial commit
			}
		}
	}
}

<<<<<<< HEAD
/* draw ghosts that occur within a frame range 
=======
/* draw ghosts that occur within a frame range
>>>>>>> Initial commit
 *  note: object should be in posemode
 */
static void draw_ghost_poses_range(Scene *scene, View3D *v3d, ARegion *ar, Base *base)
{
	Object *ob = base->object;
	AnimData *adt = BKE_animdata_from_id(&ob->id);
	bArmature *arm = ob->data;
	bPose *posen, *poseo;
	float start, end, stepsize, range, colfac;
	int cfrao, flago, ipoflago;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	start = (float)arm->ghostsf;
	end = (float)arm->ghostef;
	if (end <= start)
		return;
<<<<<<< HEAD
	
	stepsize = (float)(arm->ghostsize);
	range = (float)(end - start);
	
=======

	stepsize = (float)(arm->ghostsize);
	range = (float)(end - start);

>>>>>>> Initial commit
	/* store values */
	ob->mode &= ~OB_MODE_POSE;
	cfrao = CFRA;
	flago = arm->flag;
	arm->flag &= ~(ARM_DRAWNAMES | ARM_DRAWAXES);
	ipoflago = ob->ipoflag;
	ob->ipoflag |= OB_DISABLE_PATH;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* copy the pose */
	poseo = ob->pose;
	BKE_pose_copy_data(&posen, ob->pose, 1);
	ob->pose = posen;
	BKE_pose_rebuild(ob, ob->data);    /* child pointers for IK */
	ghost_poses_tag_unselected(ob, 0);      /* hide unselected bones if need be */
<<<<<<< HEAD
	
	glEnable(GL_BLEND);
	if (v3d->zbuf) glDisable(GL_DEPTH_TEST);
	
=======

	glEnable(GL_BLEND);
	if (v3d->zbuf) glDisable(GL_DEPTH_TEST);

>>>>>>> Initial commit
	/* draw from first frame of range to last */
	for (CFRA = (int)start; CFRA <= end; CFRA += (int)stepsize) {
		colfac = (end - (float)CFRA) / range;
		UI_ThemeColorShadeAlpha(TH_WIRE, 0, -128 - (int)(120.0f * sqrtf(colfac)));
<<<<<<< HEAD
		
=======

>>>>>>> Initial commit
		BKE_animsys_evaluate_animdata(scene, &ob->id, adt, (float)CFRA, ADT_RECALC_ALL);
		BKE_pose_where_is(scene, ob);
		draw_pose_bones(scene, v3d, ar, base, OB_WIRE, NULL, true, false);
	}
	glDisable(GL_BLEND);
	if (v3d->zbuf) glEnable(GL_DEPTH_TEST);
<<<<<<< HEAD
	
	/* before disposing of temp pose, use it to restore object to a sane state */
	BKE_animsys_evaluate_animdata(scene, &ob->id, adt, (float)cfrao, ADT_RECALC_ALL);
	
	/* clean up temporary pose */
	ghost_poses_tag_unselected(ob, 1);      /* unhide unselected bones if need be */
	BKE_pose_free(posen);
	
=======

	/* before disposing of temp pose, use it to restore object to a sane state */
	BKE_animsys_evaluate_animdata(scene, &ob->id, adt, (float)cfrao, ADT_RECALC_ALL);

	/* clean up temporary pose */
	ghost_poses_tag_unselected(ob, 1);      /* unhide unselected bones if need be */
	BKE_pose_free(posen);

>>>>>>> Initial commit
	/* restore */
	CFRA = cfrao;
	ob->pose = poseo;
	arm->flag = flago;
	ob->mode |= OB_MODE_POSE;
	ob->ipoflag = ipoflago;
}

<<<<<<< HEAD
/* draw ghosts on keyframes in action within range 
 *	- object should be in posemode 
=======
/* draw ghosts on keyframes in action within range
 *	- object should be in posemode
>>>>>>> Initial commit
 */
static void draw_ghost_poses_keys(Scene *scene, View3D *v3d, ARegion *ar, Base *base)
{
	Object *ob = base->object;
	AnimData *adt = BKE_animdata_from_id(&ob->id);
	bAction *act = (adt) ? adt->action : NULL;
	bArmature *arm = ob->data;
	bPose *posen, *poseo;
	DLRBT_Tree keys;
	ActKeyColumn *ak, *akn;
	float start, end, range, colfac, i;
	int cfrao, flago;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	start = (float)arm->ghostsf;
	end = (float)arm->ghostef;
	if (end <= start)
		return;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* get keyframes - then clip to only within range */
	BLI_dlrbTree_init(&keys);
	action_to_keylist(adt, act, &keys, NULL);
	BLI_dlrbTree_linkedlist_sync(&keys);
<<<<<<< HEAD
	
	range = 0;
	for (ak = keys.first; ak; ak = akn) {
		akn = ak->next;
		
=======

	range = 0;
	for (ak = keys.first; ak; ak = akn) {
		akn = ak->next;

>>>>>>> Initial commit
		if ((ak->cfra < start) || (ak->cfra > end))
			BLI_freelinkN((ListBase *)&keys, ak);
		else
			range++;
	}
	if (range == 0) return;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* store values */
	ob->mode &= ~OB_MODE_POSE;
	cfrao = CFRA;
	flago = arm->flag;
	arm->flag &= ~(ARM_DRAWNAMES | ARM_DRAWAXES);
	ob->ipoflag |= OB_DISABLE_PATH;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* copy the pose */
	poseo = ob->pose;
	BKE_pose_copy_data(&posen, ob->pose, 1);
	ob->pose = posen;
	BKE_pose_rebuild(ob, ob->data);  /* child pointers for IK */
	ghost_poses_tag_unselected(ob, 0);    /* hide unselected bones if need be */
<<<<<<< HEAD
	
	glEnable(GL_BLEND);
	if (v3d->zbuf) glDisable(GL_DEPTH_TEST);
	
=======

	glEnable(GL_BLEND);
	if (v3d->zbuf) glDisable(GL_DEPTH_TEST);

>>>>>>> Initial commit
	/* draw from first frame of range to last */
	for (ak = keys.first, i = 0; ak; ak = ak->next, i++) {
		colfac = i / range;
		UI_ThemeColorShadeAlpha(TH_WIRE, 0, -128 - (int)(120.0f * sqrtf(colfac)));
<<<<<<< HEAD
		
		CFRA = (int)ak->cfra;
		
=======

		CFRA = (int)ak->cfra;

>>>>>>> Initial commit
		BKE_animsys_evaluate_animdata(scene, &ob->id, adt, (float)CFRA, ADT_RECALC_ALL);
		BKE_pose_where_is(scene, ob);
		draw_pose_bones(scene, v3d, ar, base, OB_WIRE, NULL, true, false);
	}
	glDisable(GL_BLEND);
	if (v3d->zbuf) glEnable(GL_DEPTH_TEST);
<<<<<<< HEAD
	
	/* before disposing of temp pose, use it to restore object to a sane state */
	BKE_animsys_evaluate_animdata(scene, &ob->id, adt, (float)cfrao, ADT_RECALC_ALL);
	
=======

	/* before disposing of temp pose, use it to restore object to a sane state */
	BKE_animsys_evaluate_animdata(scene, &ob->id, adt, (float)cfrao, ADT_RECALC_ALL);

>>>>>>> Initial commit
	/* clean up temporary pose */
	ghost_poses_tag_unselected(ob, 1);  /* unhide unselected bones if need be */
	BLI_dlrbTree_free(&keys);
	BKE_pose_free(posen);
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* restore */
	CFRA = cfrao;
	ob->pose = poseo;
	arm->flag = flago;
	ob->mode |= OB_MODE_POSE;
}

/* draw ghosts around current frame
 *  - object is supposed to be armature in posemode
 */
static void draw_ghost_poses(Scene *scene, View3D *v3d, ARegion *ar, Base *base)
{
	Object *ob = base->object;
	AnimData *adt = BKE_animdata_from_id(&ob->id);
	bArmature *arm = ob->data;
	bPose *posen, *poseo;
	float cur, start, end, stepsize, range, colfac, actframe, ctime;
	int cfrao, flago;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* pre conditions, get an action with sufficient frames */
	if (ELEM(NULL, adt, adt->action))
		return;

	calc_action_range(adt->action, &start, &end, 0);
	if (start == end)
		return;

	stepsize = (float)(arm->ghostsize);
	range = (float)(arm->ghostep) * stepsize + 0.5f;   /* plus half to make the for loop end correct */
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* store values */
	ob->mode &= ~OB_MODE_POSE;
	cfrao = CFRA;
	actframe = BKE_nla_tweakedit_remap(adt, (float)CFRA, 0);
	flago = arm->flag;
	arm->flag &= ~(ARM_DRAWNAMES | ARM_DRAWAXES);
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* copy the pose */
	poseo = ob->pose;
	BKE_pose_copy_data(&posen, ob->pose, 1);
	ob->pose = posen;
	BKE_pose_rebuild(ob, ob->data);    /* child pointers for IK */
	ghost_poses_tag_unselected(ob, 0);      /* hide unselected bones if need be */
<<<<<<< HEAD
	
	glEnable(GL_BLEND);
	if (v3d->zbuf) glDisable(GL_DEPTH_TEST);
	
=======

	glEnable(GL_BLEND);
	if (v3d->zbuf) glDisable(GL_DEPTH_TEST);

>>>>>>> Initial commit
	/* draw from darkest blend to lowest */
	for (cur = stepsize; cur < range; cur += stepsize) {
		ctime = cur - (float)fmod(cfrao, stepsize);  /* ensures consistent stepping */
		colfac = ctime / range;
		UI_ThemeColorShadeAlpha(TH_WIRE, 0, -128 - (int)(120.0f * sqrtf(colfac)));
<<<<<<< HEAD
		
		/* only within action range */
		if (actframe + ctime >= start && actframe + ctime <= end) {
			CFRA = (int)BKE_nla_tweakedit_remap(adt, actframe + ctime, NLATIME_CONVERT_MAP);
			
=======

		/* only within action range */
		if (actframe + ctime >= start && actframe + ctime <= end) {
			CFRA = (int)BKE_nla_tweakedit_remap(adt, actframe + ctime, NLATIME_CONVERT_MAP);

>>>>>>> Initial commit
			if (CFRA != cfrao) {
				BKE_animsys_evaluate_animdata(scene, &ob->id, adt, (float)CFRA, ADT_RECALC_ALL);
				BKE_pose_where_is(scene, ob);
				draw_pose_bones(scene, v3d, ar, base, OB_WIRE, NULL, true, false);
			}
		}
<<<<<<< HEAD
		
		ctime = cur + (float)fmod((float)cfrao, stepsize) - stepsize + 1.0f;   /* ensures consistent stepping */
		colfac = ctime / range;
		UI_ThemeColorShadeAlpha(TH_WIRE, 0, -128 - (int)(120.0f * sqrtf(colfac)));
		
		/* only within action range */
		if ((actframe - ctime >= start) && (actframe - ctime <= end)) {
			CFRA = (int)BKE_nla_tweakedit_remap(adt, actframe - ctime, NLATIME_CONVERT_MAP);
			
=======

		ctime = cur + (float)fmod((float)cfrao, stepsize) - stepsize + 1.0f;   /* ensures consistent stepping */
		colfac = ctime / range;
		UI_ThemeColorShadeAlpha(TH_WIRE, 0, -128 - (int)(120.0f * sqrtf(colfac)));

		/* only within action range */
		if ((actframe - ctime >= start) && (actframe - ctime <= end)) {
			CFRA = (int)BKE_nla_tweakedit_remap(adt, actframe - ctime, NLATIME_CONVERT_MAP);

>>>>>>> Initial commit
			if (CFRA != cfrao) {
				BKE_animsys_evaluate_animdata(scene, &ob->id, adt, (float)CFRA, ADT_RECALC_ALL);
				BKE_pose_where_is(scene, ob);
				draw_pose_bones(scene, v3d, ar, base, OB_WIRE, NULL, true, false);
			}
		}
	}
	glDisable(GL_BLEND);
	if (v3d->zbuf) glEnable(GL_DEPTH_TEST);
<<<<<<< HEAD
	
	/* before disposing of temp pose, use it to restore object to a sane state */
	BKE_animsys_evaluate_animdata(scene, &ob->id, adt, (float)cfrao, ADT_RECALC_ALL);
	
	/* clean up temporary pose */
	ghost_poses_tag_unselected(ob, 1);      /* unhide unselected bones if need be */
	BKE_pose_free(posen);
	
=======

	/* before disposing of temp pose, use it to restore object to a sane state */
	BKE_animsys_evaluate_animdata(scene, &ob->id, adt, (float)cfrao, ADT_RECALC_ALL);

	/* clean up temporary pose */
	ghost_poses_tag_unselected(ob, 1);      /* unhide unselected bones if need be */
	BKE_pose_free(posen);

>>>>>>> Initial commit
	/* restore */
	CFRA = cfrao;
	ob->pose = poseo;
	arm->flag = flago;
	ob->mode |= OB_MODE_POSE;
}

/* ********************************** Armature Drawing - Main ************************* */

/* called from drawobject.c, return 1 if nothing was drawn
 * (ob_wire_col == NULL) when drawing ghost */
bool draw_armature(Scene *scene, View3D *v3d, ARegion *ar, Base *base,
                   const short dt, const short dflag, const unsigned char ob_wire_col[4],
                   const bool is_outline)
{
	Object *ob = base->object;
	bArmature *arm = ob->data;
	bool retval = false;

	if (v3d->flag2 & V3D_RENDER_OVERRIDE)
		return true;
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	if (dt > OB_WIRE && !ELEM(arm->drawtype, ARM_LINE, ARM_WIRE)) {
		/* we use color for solid lighting */
		const float white[4] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
		glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
		glFrontFace((ob->transflag & OB_NEG_SCALE) ? GL_CW : GL_CCW);  /* only for lighting... */
	}
<<<<<<< HEAD
	
=======

>>>>>>> Initial commit
	/* arm->flag is being used to detect mode... */
	/* editmode? */
	if (arm->edbo) {
		arm->flag |= ARM_EDITMODE;
<<<<<<< HEAD
		draw_ebones(v3d, ar, ob, dt);
=======
		//draw_ebones(v3d, ar, ob, dt);
		draw_earmature_elements(v3d, ar, ob, dt);
>>>>>>> Initial commit
		arm->flag &= ~ARM_EDITMODE;
	}
	else {
		/*	Draw Pose */
		if (ob->pose && ob->pose->chanbase.first) {
			/* drawing posemode selection indices or colors only in these cases */
			if (!(base->flag & OB_FROMDUPLI)) {
				if (G.f & G_PICKSEL) {
#if 0
					/* nifty but actually confusing to allow bone selection out of posemode */
					if (OBACT && (OBACT->mode & OB_MODE_WEIGHT_PAINT)) {
						if (ob == modifiers_isDeformedByArmature(OBACT))
							arm->flag |= ARM_POSEMODE;
					}
					else
#endif
					if (ob->mode & OB_MODE_POSE) {
						arm->flag |= ARM_POSEMODE;
					}
				}
				else if (ob->mode & OB_MODE_POSE) {
					if (arm->ghosttype == ARM_GHOST_RANGE) {
						draw_ghost_poses_range(scene, v3d, ar, base);
					}
					else if (arm->ghosttype == ARM_GHOST_KEYS) {
						draw_ghost_poses_keys(scene, v3d, ar, base);
					}
					else if (arm->ghosttype == ARM_GHOST_CUR) {
						if (arm->ghostep)
							draw_ghost_poses(scene, v3d, ar, base);
					}
					if ((dflag & DRAW_SCENESET) == 0) {
						if (ob == OBACT)
							arm->flag |= ARM_POSEMODE;
						else if (OBACT && (OBACT->mode & OB_MODE_WEIGHT_PAINT)) {
							if (ob == modifiers_isDeformedByArmature(OBACT))
								arm->flag |= ARM_POSEMODE;
						}
						draw_pose_paths(scene, v3d, ar, ob);
					}
				}
			}
			draw_pose_bones(scene, v3d, ar, base, dt, ob_wire_col, (dflag & DRAW_CONSTCOLOR), is_outline);
<<<<<<< HEAD
			arm->flag &= ~ARM_POSEMODE; 
			
=======
			arm->flag &= ~ARM_POSEMODE;

>>>>>>> Initial commit
			if (ob->mode & OB_MODE_POSE)
				UI_ThemeColor(TH_WIRE);  /* restore, for extra draw stuff */
		}
		else {
			retval = true;
		}
	}
	/* restore */
	glFrontFace(GL_CCW);

	return retval;
}

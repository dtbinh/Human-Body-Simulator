# ***** BEGIN GPL LICENSE BLOCK *****
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# Contributor(s): Jacques Beaurain.
#
# ***** END GPL LICENSE BLOCK *****

# --env-system-scripts allows to run without the install target. 

# Use '--write-blend=/tmp/test.blend' to view output

# Some tests are interesting but take too long to run
# and don't give deterministic results
set(USE_EXPERIMENTAL_TESTS FALSE)

set(TEST_SRC_DIR ${CMAKE_SOURCE_DIR}/../lib/tests)
set(TEST_OUT_DIR ${CMAKE_BINARY_DIR}/tests)

# ugh, any better way to do this on testing only?
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${TEST_OUT_DIR})

#~	if(NOT IS_DIRECTORY ${TEST_SRC_DIR})
#~		message(FATAL_ERROR "CMake test directory not found!")
#~	endif()

# all calls to blender use this
if(APPLE)
	if(${CMAKE_GENERATOR} MATCHES "Xcode")
		set(TEST_BLENDER_EXE ${EXECUTABLE_OUTPUT_PATH}/Debug/blender.app/Contents/MacOS/blender)
	else()
		set(TEST_BLENDER_EXE ${EXECUTABLE_OUTPUT_PATH}/blender.app/Contents/MacOS/blender)
	endif()
else()
	set(TEST_BLENDER_EXE ${EXECUTABLE_OUTPUT_PATH}/blender)
endif()

# for testing with valgrind prefix: valgrind --track-origins=yes --error-limit=no 
set(TEST_BLENDER_EXE_BARE ${TEST_BLENDER_EXE})
set(TEST_BLENDER_EXE ${TEST_BLENDER_EXE} --background -noaudio --factory-startup --env-system-scripts ${CMAKE_SOURCE_DIR}/release/scripts)


# ------------------------------------------------------------------------------
# GENERAL PYTHON CORRECTNESS TESTS
add_test(script_load_keymap ${TEST_BLENDER_EXE}
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_keymap_completeness.py
)

add_test(script_load_addons ${TEST_BLENDER_EXE}
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_load_addons.py
)

add_test(script_load_modules ${TEST_BLENDER_EXE}
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_load_py_modules.py
)

# test running operators doesn't segfault under various conditions
if(USE_EXPERIMENTAL_TESTS)
	add_test(script_run_operators ${TEST_BLENDER_EXE}
		--python ${CMAKE_CURRENT_LIST_DIR}/bl_run_operators.py
	)
endif()

# test running mathutils testing script
add_test(script_pyapi_mathutils ${TEST_BLENDER_EXE}
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_pyapi_mathutils.py
)

# ------------------------------------------------------------------------------
# MODELING TESTS
add_test(bevel ${TEST_BLENDER_EXE}
	${TEST_SRC_DIR}/modeling/bevel_regression.blend
	--python-text run_tests
)

# ------------------------------------------------------------------------------
# IO TESTS

# OBJ Import tests
add_test(import_obj_cube ${TEST_BLENDER_EXE}
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.import_scene.obj\(filepath='${TEST_SRC_DIR}/io_tests/obj/cube.obj'\)
	--md5=39cce4bacac2d1b18fc470380279bc15 --md5_method=SCENE
	--write-blend=${TEST_OUT_DIR}/import_obj_cube.blend
)

add_test(import_obj_nurbs_cyclic ${TEST_BLENDER_EXE}
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.import_scene.obj\(filepath='${TEST_SRC_DIR}/io_tests/obj/nurbs_cyclic.obj'\)
	--md5=ad3c307e5883224a0492378cd32691ab --md5_method=SCENE
	--write-blend=${TEST_OUT_DIR}/import_obj_nurbs_cyclic.blend
)

add_test(import_obj_makehuman ${TEST_BLENDER_EXE}
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.import_scene.obj\(filepath='${TEST_SRC_DIR}/io_tests/obj/makehuman.obj'\)
	--md5=c9f78b185e58358daa4ecaecfa75464e --md5_method=SCENE
	--write-blend=${TEST_OUT_DIR}/import_obj_makehuman.blend
)

# OBJ Export tests
add_test(export_obj_cube ${TEST_BLENDER_EXE}
	${TEST_SRC_DIR}/io_tests/blend_geometry/all_quads.blend
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.export_scene.obj\(filepath='${TEST_OUT_DIR}/export_obj_cube.obj',use_selection=False\)
	--md5_source=${TEST_OUT_DIR}/export_obj_cube.obj
	--md5_source=${TEST_OUT_DIR}/export_obj_cube.mtl
	--md5=70bdc394c2726203ad26c085176e3484 --md5_method=FILE
)

add_test(export_obj_nurbs ${TEST_BLENDER_EXE}
	${TEST_SRC_DIR}/io_tests/blend_geometry/nurbs.blend
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.export_scene.obj\(filepath='${TEST_OUT_DIR}/export_obj_nurbs.obj',use_selection=False,use_nurbs=True\)
	--md5_source=${TEST_OUT_DIR}/export_obj_nurbs.obj
	--md5_source=${TEST_OUT_DIR}/export_obj_nurbs.mtl
	--md5=a733ae4fa4a591ea9b0912da3af042de --md5_method=FILE
)

add_test(export_obj_all_objects ${TEST_BLENDER_EXE}
	${TEST_SRC_DIR}/io_tests/blend_scene/all_objects.blend
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.export_scene.obj\(filepath='${TEST_OUT_DIR}/export_obj_all_objects.obj',use_selection=False,use_nurbs=True\)
	--md5_source=${TEST_OUT_DIR}/export_obj_all_objects.obj
	--md5_source=${TEST_OUT_DIR}/export_obj_all_objects.mtl
	--md5=04b3ed97cede07a19548fc518ce9f8ca --md5_method=FILE
)



# PLY Import tests
add_test(import_ply_cube ${TEST_BLENDER_EXE}
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.import_mesh.ply\(filepath='${TEST_SRC_DIR}/io_tests/ply/cube_ascii.ply'\)
	--md5=527134343c27fc0ea73115b85fbfd3ac --md5_method=SCENE
	--write-blend=${TEST_OUT_DIR}/import_ply_cube.blend
)

add_test(import_ply_bunny ${TEST_BLENDER_EXE}
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.import_mesh.ply\(filepath='${TEST_SRC_DIR}/io_tests/ply/bunny2.ply'\)
	--md5=6ea5b8533400a17accf928b8fd024eaa --md5_method=SCENE
	--write-blend=${TEST_OUT_DIR}/import_ply_bunny.blend
)

add_test(import_ply_small_holes ${TEST_BLENDER_EXE}
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.import_mesh.ply\(filepath='${TEST_SRC_DIR}/io_tests/ply/many_small_holes.ply'\)
	--md5=c3093e26ecae5b6d59fbbcf2a0d0b39f --md5_method=SCENE
	--write-blend=${TEST_OUT_DIR}/import_ply_small_holes.blend
)

# PLY Export
add_test(export_ply_cube_all_data ${TEST_BLENDER_EXE}
	${TEST_SRC_DIR}/io_tests/blend_geometry/cube_all_data.blend
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.export_mesh.ply\(filepath='${TEST_OUT_DIR}/export_ply_cube_all_data.ply'\)
	--md5_source=${TEST_OUT_DIR}/export_ply_cube_all_data.ply
	--md5=6adc3748ceae8298496f99d0e7e76c15 --md5_method=FILE
)

add_test(export_ply_suzanne_all_data ${TEST_BLENDER_EXE}
	${TEST_SRC_DIR}/io_tests/blend_geometry/suzanne_all_data.blend
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.export_mesh.ply\(filepath='${TEST_OUT_DIR}/export_ply_suzanne_all_data.ply'\)
	--md5_source=${TEST_OUT_DIR}/export_ply_suzanne_all_data.ply
	--md5=68ba23f02efd6511bfd093f45f703221 --md5_method=FILE
)

add_test(export_ply_vertices ${TEST_BLENDER_EXE}  # lame, add a better one
	${TEST_SRC_DIR}/io_tests/blend_geometry/vertices.blend
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.export_mesh.ply\(filepath='${TEST_OUT_DIR}/export_ply_vertices.ply'\)
	--md5_source=${TEST_OUT_DIR}/export_ply_vertices.ply
	--md5=37faba0aa2014451b27f951afa92f870 --md5_method=FILE
)


# STL Import tests
add_test(import_stl_cube ${TEST_BLENDER_EXE}
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.import_mesh.stl\(filepath='${TEST_SRC_DIR}/io_tests/stl/cube.stl'\)
	--md5=8ceb5bb7e1cb5f4342fa1669988c66b4 --md5_method=SCENE
	--write-blend=${TEST_OUT_DIR}/import_stl_cube.blend
)

add_test(import_stl_conrod ${TEST_BLENDER_EXE}
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.import_mesh.stl\(filepath='${TEST_SRC_DIR}/io_tests/stl/conrod.stl'\)
	--md5=690a4b8eb9002dcd8631c5a575ea7348 --md5_method=SCENE
	--write-blend=${TEST_OUT_DIR}/import_stl_conrod.blend
)

add_test(import_stl_knot_max_simplified ${TEST_BLENDER_EXE}
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.import_mesh.stl\(filepath='${TEST_SRC_DIR}/io_tests/stl/knot_max_simplified.stl'\)
	--md5=baf82803f45a84ec4ddbad9cef57dd3e --md5_method=SCENE
	--write-blend=${TEST_OUT_DIR}/import_stl_knot_max_simplified.blend
)

# STL Export
add_test(export_stl_cube_all_data ${TEST_BLENDER_EXE}
	${TEST_SRC_DIR}/io_tests/blend_geometry/cube_all_data.blend
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.export_mesh.stl\(filepath='${TEST_OUT_DIR}/export_stl_cube_all_data.stl'\)
	--md5_source=${TEST_OUT_DIR}/export_stl_cube_all_data.stl
	--md5=64cb97c0cabb015e1c3f76369835075a --md5_method=FILE
)

add_test(export_stl_suzanne_all_data ${TEST_BLENDER_EXE}
	${TEST_SRC_DIR}/io_tests/blend_geometry/suzanne_all_data.blend
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.export_mesh.stl\(filepath='${TEST_OUT_DIR}/export_stl_suzanne_all_data.stl'\)
	--md5_source=${TEST_OUT_DIR}/export_stl_suzanne_all_data.stl
	--md5=e9b23c97c139ad64961c635105bb9192 --md5_method=FILE
)

add_test(export_stl_vertices ${TEST_BLENDER_EXE}  # lame, add a better one
	${TEST_SRC_DIR}/io_tests/blend_geometry/vertices.blend
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.export_mesh.stl\(filepath='${TEST_OUT_DIR}/export_stl_vertices.stl'\)
	--md5_source=${TEST_OUT_DIR}/export_stl_vertices.stl
	--md5=3fd3c877e573beeebc782532cc005820 --md5_method=FILE
)


# X3D Import
add_test(import_x3d_cube ${TEST_BLENDER_EXE}
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.import_scene.x3d\(filepath='${TEST_SRC_DIR}/io_tests/x3d/color_cube.x3d'\)
	--md5=3fae9be004199c145941cd3f9f80ad7b --md5_method=SCENE
	--write-blend=${TEST_OUT_DIR}/import_x3d_cube.blend
)

add_test(import_x3d_teapot ${TEST_BLENDER_EXE}
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.import_scene.x3d\(filepath='${TEST_SRC_DIR}/io_tests/x3d/teapot.x3d'\)
	--md5=8ee196c71947dce4199d55698501691e --md5_method=SCENE
	--write-blend=${TEST_OUT_DIR}/import_x3d_teapot.blend
)

add_test(import_x3d_suzanne_material ${TEST_BLENDER_EXE}
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.import_scene.x3d\(filepath='${TEST_SRC_DIR}/io_tests/x3d/suzanne_material.x3d'\)
	--md5=3edea1353257d8b5a5f071942f417be6 --md5_method=SCENE
	--write-blend=${TEST_OUT_DIR}/import_x3d_suzanne_material.blend
)

# X3D Export
add_test(export_x3d_cube ${TEST_BLENDER_EXE}
	${TEST_SRC_DIR}/io_tests/blend_geometry/all_quads.blend
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.export_scene.x3d\(filepath='${TEST_OUT_DIR}/export_x3d_cube.x3d',use_selection=False\)
	--md5_source=${TEST_OUT_DIR}/export_x3d_cube.x3d
	--md5=05312d278fe41da33560fdfb9bdb268f --md5_method=FILE
)

add_test(export_x3d_nurbs ${TEST_BLENDER_EXE}
	${TEST_SRC_DIR}/io_tests/blend_geometry/nurbs.blend
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.export_scene.x3d\(filepath='${TEST_OUT_DIR}/export_x3d_nurbs.x3d',use_selection=False\)
	--md5_source=${TEST_OUT_DIR}/export_x3d_nurbs.x3d
	--md5=4286d4a2aa507ef78b22ddcbdcc88481 --md5_method=FILE
)

add_test(export_x3d_all_objects ${TEST_BLENDER_EXE}
	${TEST_SRC_DIR}/io_tests/blend_scene/all_objects.blend
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.export_scene.x3d\(filepath='${TEST_OUT_DIR}/export_x3d_all_objects.x3d',use_selection=False\)
	--md5_source=${TEST_OUT_DIR}/export_x3d_all_objects.x3d
	--md5=f5f9fa4c5619a0eeab66685aafd2f7f0 --md5_method=FILE
)



# 3DS Import
add_test(import_3ds_cube ${TEST_BLENDER_EXE}
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.import_scene.autodesk_3ds\(filepath='${TEST_SRC_DIR}/io_tests/3ds/cube.3ds'\)
	--md5=cb5a45c35a343c3f5beca2a918472951 --md5_method=SCENE
	--write-blend=${TEST_OUT_DIR}/import_3ds_cube.blend
)

add_test(import_3ds_hierarchy_lara ${TEST_BLENDER_EXE}
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.import_scene.autodesk_3ds\(filepath='${TEST_SRC_DIR}/io_tests/3ds/hierarchy_lara.3ds'\)
	--md5=766c873d9fdb5f190e43796cfbae63b6 --md5_method=SCENE
	--write-blend=${TEST_OUT_DIR}/import_3ds_hierarchy_lara.blend
)

add_test(import_3ds_hierarchy_greek_trireme ${TEST_BLENDER_EXE}
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.import_scene.autodesk_3ds\(filepath='${TEST_SRC_DIR}/io_tests/3ds/hierarchy_greek_trireme.3ds'\)
	--md5=b62ee30101e8999cb91ef4f8a8760056 --md5_method=SCENE
	--write-blend=${TEST_OUT_DIR}/import_3ds_hierarchy_greek_trireme.blend
)

# 3DS Export
add_test(export_3ds_cube ${TEST_BLENDER_EXE}
	${TEST_SRC_DIR}/io_tests/blend_geometry/all_quads.blend
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.export_scene.autodesk_3ds\(filepath='${TEST_OUT_DIR}/export_3ds_cube.3ds',use_selection=False\)
	--md5_source=${TEST_OUT_DIR}/export_3ds_cube.3ds
	--md5=a31f5071b6c6dc7445b9099cdc7f63b3 --md5_method=FILE
)

add_test(export_3ds_nurbs ${TEST_BLENDER_EXE}
	${TEST_SRC_DIR}/io_tests/blend_geometry/nurbs.blend
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.export_scene.autodesk_3ds\(filepath='${TEST_OUT_DIR}/export_3ds_nurbs.3ds',use_selection=False\)
	--md5_source=${TEST_OUT_DIR}/export_3ds_nurbs.3ds
	--md5=5bdd21be3c80d814fbc83cb25edb08c2 --md5_method=FILE
)

add_test(export_3ds_all_objects ${TEST_BLENDER_EXE}
	${TEST_SRC_DIR}/io_tests/blend_scene/all_objects.blend
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.export_scene.autodesk_3ds\(filepath='${TEST_OUT_DIR}/export_3ds_all_objects.3ds',use_selection=False\)
	--md5_source=${TEST_OUT_DIR}/export_3ds_all_objects.3ds
	--md5=68447761ab0ca38e1e22e7c177ed48a8 --md5_method=FILE
)



# FBX Export
# 'use_metadata=False' for reliable md5's
add_test(export_fbx_cube ${TEST_BLENDER_EXE}
	${TEST_SRC_DIR}/io_tests/blend_geometry/all_quads.blend
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.export_scene.fbx\(filepath='${TEST_OUT_DIR}/export_fbx_cube.fbx',use_selection=False,use_metadata=False\)
	--md5_source=${TEST_OUT_DIR}/export_fbx_cube.fbx
	--md5=59a35577462f95f9a0b4e6035226ce9b --md5_method=FILE
)

add_test(export_fbx_nurbs ${TEST_BLENDER_EXE}
	${TEST_SRC_DIR}/io_tests/blend_geometry/nurbs.blend
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.export_scene.fbx\(filepath='${TEST_OUT_DIR}/export_fbx_nurbs.fbx',use_selection=False,use_metadata=False\)
	--md5_source=${TEST_OUT_DIR}/export_fbx_nurbs.fbx
	--md5=d31875f18f613fa0c3b16e978f87f6f8 --md5_method=FILE
)

add_test(export_fbx_all_objects ${TEST_BLENDER_EXE}
	${TEST_SRC_DIR}/io_tests/blend_scene/all_objects.blend
	--python ${CMAKE_CURRENT_LIST_DIR}/bl_test.py --
	--run={'FINISHED'}&bpy.ops.export_scene.fbx\(filepath='${TEST_OUT_DIR}/export_fbx_all_objects.fbx',use_selection=False,use_metadata=False\)
	--md5_source=${TEST_OUT_DIR}/export_fbx_all_objects.fbx
	--md5=b35eb2a9d0e73762ecae2278c25a38ac --md5_method=FILE
)

if(WITH_CYCLES)
	if(OPENIMAGEIO_IDIFF)
		add_test(cycles_shaders_test
			${CMAKE_CURRENT_LIST_DIR}/cycles_render_tests.py
			-blender "${TEST_BLENDER_EXE_BARE}"
			-testdir "${TEST_SRC_DIR}/cycles/ctests/shader"
			-idiff "${OPENIMAGEIO_IDIFF}"
		)
	endif()
endif()

#include "material/material_test_plane.h"
#include "resource/global_resource.h"
#include "shader/shader_pipeline.h"

MaterialTestPlane::MaterialTestPlane(int p_light_index)
        : light_index(p_light_index) {
    shader = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_material_test_plane"));
    flux_shader = nullptr;
    depth_shader = nullptr;
}

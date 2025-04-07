#include "material/material_test_box.h"
#include "resource/global_resource.h"
#include "shader/shader_pipeline.h"

MaterialTestBox::MaterialTestBox() {
    shader = REQUIRE_NON_NULL(GlobalResource::get_singleton()->get_resource<ShaderPipeline>("shader_material_test_box"));
    flux_shader = nullptr;
    depth_shader = nullptr;
}


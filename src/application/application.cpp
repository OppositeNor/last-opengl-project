#include "application/application.h"
#include "scene_node/scene_node.h"
#include "drawable.hpp"
#include "spot_light.h"
#include "point_light.h"

void Application::tick(double p_delta_time) {
    tick_tock = !tick_tock;
    for (auto* scene_node : scene_nodes) {
        scene_node->update_all_global_transform(tick_tock);
    }
    for (auto* point_light : point_light_objects) {
        point_light->update_point_light();
        point_light->get_light()->pre_draw();
        for (auto* drawable : drawables) {
            if (drawable->is_shadow_enabled()) {
                drawable->draw_transform_only(point_light->get_light()->get_depth_shader());
            }
        }
        point_light->get_light()->post_draw();
    }
    for (auto* spot_light : spot_light_objects) {
        spot_light->update_spot_light();
        spot_light->get_light()->pre_draw(spot_light->get_light()->get_shadow_map().get());
        for (auto* drawable : drawables) {
            if (drawable->is_shadow_enabled()) {
                drawable->draw_transform_only(spot_light->get_light()->get_shadow_map()->get_shader());
            }
        }
        switch (spot_light->get_light()->get_gi_type()) {
            case SpotLight::GIType::NONE:
                break;
            case SpotLight::GIType::LPV:
            case SpotLight::GIType::RSM: {
                Framebuffer* fb = REQUIRE_NON_NULL(spot_light->get_light()->get_coordinate_map().get());
                spot_light->get_light()->pre_draw(fb);
                for (auto* drawable : drawables) {
                    drawable->draw_transform_only(fb->get_shader());
                }
                fb = REQUIRE_NON_NULL(spot_light->get_light()->get_normal_map().get());
                spot_light->get_light()->pre_draw(fb);
                for (auto* drawable : drawables) {
                    drawable->draw_transform_only(fb->get_shader());
                }
                fb = REQUIRE_NON_NULL(spot_light->get_light()->get_flux_map().get());
                fb->bind();
                for (auto* drawable : drawables) {
                    drawable->draw_flux(spot_light->get_light());
                }
                fb->unbind();
                if (spot_light->get_light()->get_gi_type() == SpotLight::GIType::LPV) {
                    spot_light->get_light()->get_sg_lpv()->dispatch_compute();
                }
            }   break;
            default:
                break;
        }
        spot_light->get_light();
    }
    process(p_delta_time);
}


/*
Plugin Name
Copyright (C) <Year> <Developer> <Email Address>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include <obs-module.h>
#include <graphics/graphics.h>

#include "plugin-macros.generated.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

#define MODULE_NAME "simple-blur"

struct simple_blur_data {
	float strength;
	gs_effect_t *effect;
	gs_eparam_t *strength_param;
};

const char* simple_blur_get_name(void* unused)
{
	return "Simple Blur";
}

static void *simple_blur_create(obs_data_t *settings, obs_source_t *source)
{
	struct simple_blur_data *data = (struct simple_blur_data *)bzalloc(
		sizeof(struct simple_blur_data));
	data->strength = (float)obs_data_get_double(settings, "strength");

	obs_enter_graphics();

	// エフェクトの取得
	data->effect = gs_effect_create("simple-blur.effect", NULL, NULL);

	// パラメータの取得
	data->strength_param =
		gs_effect_get_param_by_name(data->effect, "strength");

	obs_leave_graphics();

	return data;
}

static void simple_blur_destroy(void *data)
{
	struct simple_blur_data *effect_data = (struct simple_blur_data *)data;

	if (effect_data->effect) {
		obs_enter_graphics();
		gs_effect_destroy(effect_data->effect);
		obs_leave_graphics();
	}

	bfree(effect_data);
}

void simple_blur_video_render(void *data, gs_effect_t *effect,
			      struct texture *input, struct texture *output,
			      const struct vec2 *uv,
			      const struct vec4 *draw_rect)
{
	struct simple_blur_data *effect_data = (struct simple_blur_data *)data;

	gs_effect_set_float(effect_data->strength_param, effect_data->strength);

	gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"),
			      input);
	gs_draw_sprite(effect, 0, output, uv, draw_rect);
}


void simple_blur_defaults(obs_data_t *settings)
{
	obs_data_set_default_double(settings, "strength", 1.0);
}

obs_properties_t *simple_blur_properties(void *unused)
{
	obs_properties_t *props = obs_properties_create();
	obs_properties_add_float(props, "strength", "Strength", 0.0, 100.0,
				 1.0);
	return props;
}

bool obs_module_load(void)
{
	struct obs_source_info source_info = {
		.id = "simple-blur",
		.type = OBS_SOURCE_TYPE_FILTER,
		.output_flags = OBS_SOURCE_VIDEO,
		.get_name = simple_blur_get_name,
		.create = simple_blur_create,
		.destroy = simple_blur_destroy,
		.video_render = simple_blur_video_render,
		.get_properties = simple_blur_properties,
		.get_defaults = simple_blur_defaults,
	};

	obs_register_source(&source_info);

	return true;
}
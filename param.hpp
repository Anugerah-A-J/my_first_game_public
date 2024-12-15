#include <allegro5/allegro_font.h>
#include <allegro5/color.h>
#pragma once

namespace param {
const int window_width = 800;
const int window_height = 600;
const float unit_length = 10;
const float line_width = 1;
const float reach_radius = window_height / 4;
// const float reach_radius    = window_height;
const int life = 3;
// const int life              = 1;
const int translation_step = 10;

// static float delta() { return 0.1f;

const ALLEGRO_COLOR black = al_map_rgba_f(0.1f, 0.1f, 0.1f, 1);
const ALLEGRO_COLOR red = al_map_rgba_f(0.9f, 0.1f, 0.1f, 1);
const ALLEGRO_COLOR yellow = al_map_rgba_f(0.9f, 0.9f, 0.1f, 1);
const ALLEGRO_COLOR green = al_map_rgba_f(0.1f, 0.9f, 0.1f, 1);
const ALLEGRO_COLOR cyan = al_map_rgba_f(0.1f, 0.9f, 0.9f, 1);
const ALLEGRO_COLOR blue = al_map_rgba_f(0.1f, 0.1f, 0.9f, 1);
const ALLEGRO_COLOR magenta = al_map_rgba_f(0.9f, 0.1f, 0.9f, 1);
const ALLEGRO_COLOR white = al_map_rgba_f(0.9f, 0.9f, 0.9f, 1);
const ALLEGRO_COLOR vanish = al_map_rgba_f(0.1f, 0.1f, 0.1f, 0);
const ALLEGRO_COLOR gray = al_map_rgba_f(0.5f, 0.5f, 0.5f, 0);

const float color_transformation_ratio = 0.5f;

const float sqrt_2 = 1.41421356237309504880f;
const float sqrt_3 = 1.73205080756887729352f;
const float pi = 3.14159265358979323846f;
const float sin_15 = (sqrt_3 - 1) * sqrt_2 / 4;
const float sin_45 = sqrt_2 / 2;
const float sin_75 = (sqrt_3 + 1) * sqrt_2 / 4;

class Theme
{
public:
    Theme(const ALLEGRO_COLOR &passive_text_color,
          const ALLEGRO_COLOR &active_text_color,
          const ALLEGRO_COLOR &background_color,
          const ALLEGRO_COLOR &line_color)
        : passive_text_color{passive_text_color}
        , active_text_color{active_text_color}
        , background_color{background_color}
        , line_color{line_color}
    {}

    const ALLEGRO_COLOR passive_text_color;
    const ALLEGRO_COLOR active_text_color;
    const ALLEGRO_COLOR background_color;
    const ALLEGRO_COLOR line_color;
};

const Theme default_theme = Theme(param::gray,
                                  param::white,
                                  al_map_rgba_f(0.2f, 0.2f, 0.2f, 1),
                                  param::gray);
}; // namespace param

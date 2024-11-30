#include "geometry.h"
#include <allegro5/color.h>
#include <allegro5/allegro_primitives.h>
#include "param.h"
#include <vector>
#pragma once

class King
{
public:
    King(
        const Circle& king_shape,
        const Rectangle& throne_shape,
        const ALLEGRO_COLOR& color,
        float line_width,
        const Vector& last_life_position)
    :
        king_shape{king_shape},
        throne_shape{throne_shape},
        color{color},
        line_width{line_width},
        life{param::life},
        life_shapes{king_shape, king_shape, king_shape}
    {
        for (auto& life_shape: life_shapes)
        {
            life_shape.Scale(0.5);
            life_shape.Center(last_life_position);
        }

        for (auto it = life_shapes.begin() + 1; it != life_shapes.end(); ++it)
            (*it).Translate(0, -throne_shape.Size().Y() / 2 * (it - life_shapes.begin()));
    };

    void Draw() const
    {
        king_shape.Draw(color);
        throne_shape.Draw(color, line_width);
    };

    void Draw_life() const
    {
        for (auto it = life_shapes.begin(); it != life_shapes.begin() + life; ++it)
            (*it).Draw(color);
    }

    bool Contain(const Vector& point) const
    {
        return king_shape.Contain(point);
    }

    Vector Center() const
    {
        return king_shape.Center();
    }

    const ALLEGRO_COLOR& Color() const { return color; }

    const Circle& King_shape() const { return king_shape; }
    const Rectangle& Throne_shape() const { return throne_shape; }
    int Life() const { return life; }
    void Life_will_be_decreased() { decrease_life = true; }
    void Update_life()
    {
        if (!decrease_life)
            return;
            
        life--;
        decrease_life = false;
    }
    void Reset_life() { life = param::life; }
private:
    Circle king_shape;
    Rectangle throne_shape;
    ALLEGRO_COLOR color;
    float line_width;
    int life;
    std::vector<Circle> life_shapes;
    bool decrease_life;
};

class King_magenta
:
    public King
{
public:
    King_magenta()
    :
        King{
            Circle(
                param::window_width - param::unit_length * 3.5,
                param::window_height / 2,
                param::unit_length / 2
            ),
            Rectangle(
                param::window_width - param::unit_length * 5,
                param::window_height / 2 - param::unit_length * 1.5,
                param::unit_length * 3,
                param::unit_length * 3
            ),
            param::magenta,
            param::line_width,
            Vector(
                param::window_width - param::unit_length,
                param::window_height / 2 + param::unit_length * 1.5
            )
        }
    {};
};

class King_cyan
:
    public King
{
public:
    King_cyan()
    :
        King{
            Circle(
                param::unit_length * 3.5,
                param::window_height / 2,
                param::unit_length / 2
            ),
            Rectangle(
                param::unit_length * 2,
                param::window_height / 2 - param::unit_length * 1.5,
                param::unit_length * 3,
                param::unit_length * 3
            ),
            param::cyan,
            param::line_width,
            Vector(
                param::unit_length,
                param::window_height / 2 + param::unit_length * 1.5
            )
        }
    {};
};

class Pawn
{
public:
    Pawn(float cx, float cy, const ALLEGRO_COLOR& color)
    :
        shape{cx, cy, param::unit_length / 2},
        color{color}
    {}

    Pawn(const Vector& center, const ALLEGRO_COLOR& color)
    :
        shape{center, param::unit_length / 2},
        color{color}
    {}

    void Draw() const
    {
        shape.Draw(color);
    }

    bool Contain(const Vector& point) const
    {
        return shape.Contain(point);
    }

    static void Update_translation(const Vector& start, const Vector& end)
    {
        translation = (end - start) / param::unit_length;
    }

    static void Reset_translation_step_count()
    {
        translation_step_count = 0;
    }

    static void Stop()
    {
        translation_step_count = param::translation_step;
    }

    void Move()
    {
        if (translation_step_count == param::translation_step)
            return;

        translation_step_count ++;

        shape.Translate(translation);
    }

    static bool Finish_moving() { return translation_step_count == param::translation_step; }

    void Retreat(float compared_to_latest_translation)
    {
        shape.Translate(-compared_to_latest_translation * translation);
    }

    void Transform_color_to_vanish()
    {
        Transform_color(color, param::vanish, param::color_transformation_ratio);
    };

    bool Color_equal_vanish()
    {
        if(Equal(color, param::vanish, 0.05f))
            return true;

        return false;
    };

    Line Last_translation() const
    {
        return Line(
            shape.Center() - translation,
            shape.Center()
        );
    }

    Vector Center() const { return shape.Center(); }

    static void Vanish_immediately(bool value) { vanish_immediately = value; }
    static bool Vanish_immediately() { return vanish_immediately; }

    const Circle& Shape() const { return shape; }
private:
    inline static unsigned int translation_step_count = 0;
    inline static Vector translation = Vector(0, 0);
    inline static bool vanish_immediately = false;
    Circle shape;
    ALLEGRO_COLOR color;
};
#include <allegro5/color.h>
#include <allegro5/allegro_primitives.h>
#include "geometry.h"
#include "param.h"
#pragma once

class Aim
{
public:
    Aim()
    :
        reach_circle{0, 0, param::reach_radius},
        pawn_destination{0, 0},
        direction_sign{
            Vector(0, 0),
            Vector(0, 0),
            Vector(0, 0)
        },
        color{param::magenta},
        line_width{param::line_width},
        reach_circle_is_visible{false},
        direction_sign_is_visible{false}
    {}

    void Draw() const
    {
        if (reach_circle_is_visible)
            reach_circle.Draw(color, line_width);

        if (direction_sign_is_visible)
        {
            direction_sign.Draw(color, line_width);
            Line(reach_circle.Center(), pawn_destination).Draw(color, line_width);
        }
    }

    void Center(const Vector& point) { reach_circle.Center(point); }
    const Vector& Center() { return reach_circle.Center(); }

    const Vector& Pawn_destination() const { return pawn_destination; }

    void Update_direction(const Vector& mouse_coordinate)
    {
        Vector unit = (mouse_coordinate - reach_circle.Center()).Unit();

        pawn_destination = reach_circle.Center() - unit * reach_circle.Radius();
        
        direction_sign.Vertex_1(reach_circle.Center() + unit * param::unit_length);

        Vector temp = direction_sign.Vertex_1() + unit * param::unit_length;

        direction_sign.Vertex_2(temp + Matrix(0, 1, -1, 0) * unit * param::unit_length / param::sqrt_3);

        direction_sign.Vertex_3(temp + Matrix(0, -1, 1, 0) * unit * param::unit_length / param::sqrt_3);
    }

    void Show_reach_circle() { reach_circle_is_visible = true; }
    void Show_direction_sign() { direction_sign_is_visible = true; }
    void Hide()
    {
        reach_circle_is_visible = false;
        direction_sign_is_visible = false;
    }
    void Color(const ALLEGRO_COLOR& color) { this->color = color; }
private:
    Circle reach_circle;
    Vector pawn_destination;
    Triangle direction_sign;
    ALLEGRO_COLOR color;
    float line_width;
    bool reach_circle_is_visible;
    bool direction_sign_is_visible;
};

class Clipper
{
public:
    Clipper()
    :
        left{
            0,
            0,
            2 * param::unit_length,
            param::window_height
        },
        top{
            0,
            0,
            param::window_width,
            param::unit_length
        },
        right{
            param::window_width - 2 * param::unit_length,
            0,
            2 * param::unit_length,
            param::window_height
        },
        bottom{
            0,
            param::window_height - param::unit_length,
            param::window_width,
            param::unit_length
        },
        color{param::black}
    {};

    void Draw() const
    {
        left.Draw(color);
        top.Draw(color);
        right.Draw(color);
        bottom.Draw(color);
    };
private:
    Rectangle left;
    Rectangle top;
    Rectangle right;
    Rectangle bottom;
    ALLEGRO_COLOR color;
};

class Fence
{
public:
    Fence()
    :
        shape{
            2 * param::unit_length,
            param::unit_length,
            param::window_width - 4 * param::unit_length,
            param::window_height - 2 * param::unit_length
        },
        color{param::red},
        line_width{param::line_width}
    {};

    void Draw() const
    {
        shape.Draw(color, line_width);
    };

    const Rectangle& Shape() const { return shape; }
private:
    Rectangle shape;
    ALLEGRO_COLOR color;
    float line_width;
};
#include "object.h"
#include "character.h"
#include <set>
#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>
#pragma once

namespace collision
{
    void Response(std::set<Pawn*>& dying_pawns, Pawn& moving_pawn, const Fence& fence);
    void Response(std::set<Pawn*>& dying_pawns, const Pawn& moving_pawn, std::vector<Pawn>& pawns);
    void Response(Pawn& moving_pawn, King& king, const std::function<void(float f)>& what_to_do);

    float Circle_vs_circle(const Circle& moving_circle, const Circle& nonmoving_circle, const Line& velocity);
    float Circle_vs_rectangle(const Circle& moving_circle, const Rectangle& nonmoving_rectangle, const Line& velocity);
    float Circle_inside_rectangle(const Circle& moving_circle, const Rectangle& nonmoving_rectangle, const Line& velocity);
    
    float Intersect(const Line& line1, const Line& line2);
    float Intersect(const Line& line, const Circle& circle);
};

void collision::Response(std::set<Pawn*>& dying_pawns ,Pawn& moving_pawn, const Fence& fence)
{
    float t = Circle_inside_rectangle(moving_pawn.Shape(), fence.Shape(), moving_pawn.Last_translation());

    if (t == 2 || Pawn::Vanish_immediately())
        return;
    
    moving_pawn.Retreat(1 - t);
    moving_pawn.Stop();
    dying_pawns.emplace(&moving_pawn);
};

void collision::Response(std::set<Pawn*>& dying_pawns ,const Pawn& moving_pawn, std::vector<Pawn>& pawns)
{
    for (auto& pawn: pawns)
    {
        if (Circle_vs_circle(moving_pawn.Shape(), pawn.Shape(), moving_pawn.Last_translation()) == 2)
            continue;

        dying_pawns.emplace(&pawn);
    }
};

void collision::Response(Pawn& moving_pawn, King& king, const std::function<void(float f)>& what_to_do_when_collide)
{
    float t = Circle_vs_rectangle(moving_pawn.Shape(), king.Throne_shape(), moving_pawn.Last_translation());
    // std::cout << "t throne: " << t << "\n";
    // if (t == 2)
    //     return;

    what_to_do_when_collide(t);
};

float collision::Circle_vs_circle(const Circle& moving_circle, const Circle& nonmoving_circle, const Line& velocity)
{
    Circle circle = nonmoving_circle;
    circle.Add_radius_by(moving_circle.Radius());

    return Intersect(velocity, circle);
};

float collision::Circle_vs_rectangle(const Circle& moving_circle, const Rectangle& nonmoving_rectangle, const Line& velocity)
{
    Line top = nonmoving_rectangle.Top();
    Line right = nonmoving_rectangle.Right();
    Line bottom = nonmoving_rectangle.Bottom();
    Line left = nonmoving_rectangle.Left();

    Circle top_left = moving_circle;
    Circle top_right = moving_circle;
    Circle bottom_right = moving_circle;
    Circle bottom_left = moving_circle;

    top_left.Center(top.Start());
    top_right.Center(top.End());
    bottom_right.Center(bottom.Start());
    bottom_left.Center(bottom.End());

    top.Translate(Vector(0, -moving_circle.Radius()));
    right.Translate(Vector(moving_circle.Radius(), 0));
    bottom.Translate(Vector(0, moving_circle.Radius()));
    left.Translate(Vector(-moving_circle.Radius(), 0));

    std::vector<float> ts;

    ts.emplace_back(Intersect(velocity, top));
    ts.emplace_back(Intersect(velocity, right));
    ts.emplace_back(Intersect(velocity, bottom));
    ts.emplace_back(Intersect(velocity, left));

    ts.emplace_back(Intersect(velocity, top_left));
    ts.emplace_back(Intersect(velocity, top_right));
    ts.emplace_back(Intersect(velocity, bottom_right));
    ts.emplace_back(Intersect(velocity, bottom_left));

    return *std::min_element(ts.begin(), ts.end());
};

float collision::Circle_inside_rectangle(const Circle& moving_circle, const Rectangle& nonmoving_rectangle, const Line& velocity)
{
    Rectangle rectangle = nonmoving_rectangle;
    rectangle.Translate(Vector(moving_circle.Radius(), moving_circle.Radius()));
    rectangle.Add_size_by(-2 * Vector(moving_circle.Radius(), moving_circle.Radius()));

    std::vector<float> ts;

    ts.emplace_back(Intersect(velocity, rectangle.Top()));
    ts.emplace_back(Intersect(velocity, rectangle.Right()));
    ts.emplace_back(Intersect(velocity, rectangle.Bottom()));
    ts.emplace_back(Intersect(velocity, rectangle.Left()));

    return *std::min_element(ts.begin(), ts.end());
};

float collision::Intersect(const Line& line1, const Line& line2)
{
    Vector A = line1.End() - line1.Start();
    Vector B = line2.Start() - line2.End();
    Vector C = line1.Start() - line2.Start();

    float t_numerator = B.Y() * C.X() - B.X() * C.Y();
    float u_numerator = C.Y() * A.X() - C.X() * A.Y();
    float denominator = A.Y() * B.X() - A.X() * B.Y();

    // // t < 0 and u < 0
    if (denominator > 0 && (t_numerator < 0 || u_numerator < 0))
        return 2;
    if (denominator < 0 && (t_numerator > 0 || u_numerator > 0))
        return 2;

    // // t > 1 and u > 1
    if (denominator > 0 && (t_numerator > denominator || u_numerator > denominator))
        return 2;
    if (denominator < 0 && (t_numerator < denominator || u_numerator < denominator))
        return 2;

    // if (equal(denominator, 0, 0.05f))
    if (denominator == 0)
        return 2;

    float t = t_numerator / denominator;
    float u = u_numerator / denominator;

    return t;
};

float collision::Intersect(const Line& line, const Circle& circle)
{
    Vector X = line.Start() - circle.Center();
    Vector Y = line.End() - line.Start();

    float a = Vector::Dot(Y, Y);
    float b = 2 * Vector::Dot(X, Y);
    float c = Vector::Dot(X, X) - circle.Radius() * circle.Radius();

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0)
    {
        return 2;
    }
    else
    {
        discriminant = sqrtf(discriminant);

        // Compute min and max solutions of t
        float t_min = (-b - discriminant) / (2 * a);
        float t_max = (-b + discriminant) / (2 * a);
        
        // Check whether either t is within bounds of segment
        if (t_min >= 0 && t_min <= 1)
        {
            return t_min;
        }
        else if (t_max >= 0 && t_max <= 1)
        {
            return t_max;
        }
        else
        {
            return 2;
        }
    }
};
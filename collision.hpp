// #include "object.hpp"
// #include "character.hpp"
#include "geometry.hpp"
#include <algorithm>
#include <iostream>
#include <set>
#include <vector>
#pragma once

namespace collision {
float Circle_vs_circle(const Circle &moving_circle,
                       const Circle &nonmoving_circle,
                       const Line &velocity);
float Circle_vs_line(const Circle &moving_circle, const Line &nonmoving_line, const Line &velocity);
float Circle_vs_rectangle(const Circle &moving_circle,
                          const Rectangle &nonmoving_rectangle,
                          const Line &velocity);
float Circle_inside_rectangle(const Circle &moving_circle,
                              const Rectangle &nonmoving_rectangle,
                              const Line &velocity);

float Intersect(const Line &line1, const Line &line2);
float Intersect(const Line &line, const Circle &circle);
}; // namespace collision

float collision::Circle_vs_circle(const Circle &moving_circle,
                                  const Circle &nonmoving_circle,
                                  const Line &velocity)
{
    Vector normal = velocity.Start() - nonmoving_circle.Center();

    if (normal.Magsq() <= 4 * moving_circle.Radius() * moving_circle.Radius()
        && Vector::Dot(normal, velocity.Direction()) >= 0)
        return 2;

    Circle circle = nonmoving_circle;
    circle.Add_radius_by(moving_circle.Radius());

    return Intersect(velocity, circle);
};

float collision::Circle_vs_line(const Circle &moving_circle,
                                const Line &nonmoving_line,
                                const Line &velocity)
{
    Line line_1 = nonmoving_line;
    Line line_2 = nonmoving_line;
    Circle start = moving_circle;
    Circle end = moving_circle;

    Vector translate = nonmoving_line.Direction().Unit().Swap() * moving_circle.Radius();

    line_1.Translate(translate);
    line_2.Translate(-translate);
    start.Center(nonmoving_line.Start());
    end.Center(nonmoving_line.End());

    std::vector<float> ts;
    ts.reserve(4);

    ts.push_back(Intersect(velocity, line_1));
    ts.push_back(Intersect(velocity, line_2));
    ts.push_back(Intersect(velocity, start));
    ts.push_back(Intersect(velocity, end));

    return *std::min_element(ts.begin(), ts.end());
}

float collision::Circle_vs_rectangle(const Circle &moving_circle,
                                     const Rectangle &nonmoving_rectangle,
                                     const Line &velocity)
{
    Vector rectangle_to_circle_past = velocity.Start()
                                      - nonmoving_rectangle.Closest_point_to(velocity.Start());

    if (rectangle_to_circle_past.Magsq() <= moving_circle.Radius() * moving_circle.Radius()) {
        if (Vector::Dot(rectangle_to_circle_past, velocity.Direction()) >= 0)
            return 2; // angle <= abs(90)
        // if () return 0; // angle > abs(90)
    }

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
    ts.reserve(8);

    ts.push_back(Intersect(velocity, top));
    ts.push_back(Intersect(velocity, right));
    ts.push_back(Intersect(velocity, bottom));
    ts.push_back(Intersect(velocity, left));

    ts.push_back(Intersect(velocity, top_left));
    ts.push_back(Intersect(velocity, top_right));
    ts.push_back(Intersect(velocity, bottom_right));
    ts.push_back(Intersect(velocity, bottom_left));

    return *std::min_element(ts.begin(), ts.end());
};

float collision::Circle_inside_rectangle(const Circle &moving_circle,
                                         const Rectangle &nonmoving_rectangle,
                                         const Line &velocity)
{
    Rectangle rectangle = nonmoving_rectangle;
    rectangle.Translate(Vector(moving_circle.Radius(), moving_circle.Radius()));
    rectangle.Add_size_by(-2 * Vector(moving_circle.Radius(), moving_circle.Radius()));

    std::vector<float> ts;

    ts.push_back(Intersect(velocity, rectangle.Top()));
    ts.push_back(Intersect(velocity, rectangle.Right()));
    ts.push_back(Intersect(velocity, rectangle.Bottom()));
    ts.push_back(Intersect(velocity, rectangle.Left()));

    return *std::min_element(ts.begin(), ts.end());
}

float collision::Intersect(const Line &line1, const Line &line2)
// return 0 to 1 if intersect
// return 2 if not intersect
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

float collision::Intersect(const Line &line, const Circle &circle)
// return 0 to 1 if intersect
// return 2 if not intersect
{
    Vector X = line.Start() - circle.Center();
    Vector Y = line.End() - line.Start();

    float a = Vector::Dot(Y, Y);
    float b = 2 * Vector::Dot(X, Y);
    float c = Vector::Dot(X, X) - circle.Radius() * circle.Radius();

    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return 2;
    } else {
        discriminant = sqrtf(discriminant);

        // Compute min and max solutions of t
        float t_min = (-b - discriminant) / (2 * a);
        float t_max = (-b + discriminant) / (2 * a);

        // Check whether either t is within bounds of segment
        if (t_min >= 0 && t_min <= 1) {
            return t_min;
        } else if (t_max >= 0 && t_max <= 1) {
            return t_max;
        } else {
            return 2;
        }
    }
}

// float collision::Square_min_distance(const Vector &point, const Rectangle &rectangle)
// {
//     // Compute differences for each axis

//     float dx = std::max(rectangle.Origin().X() - point.X(), 0);
//     dx = std::max(dx, point.X() - rectangle.Origin().X() - rectangle.Size().X());

//     float dy = std::max(rectangle.Origin().Y() - point.Y(), 0);
//     dy = std::max(dy, point.Y() - rectangle.Origin().Y() - rectangle.Size().Y());

//     // Distance squared formula
//     return dx * dx + dy * dy;
// }

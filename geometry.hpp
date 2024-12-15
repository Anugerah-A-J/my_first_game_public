#include <algorithm>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#pragma once

bool Equal(float f1, float f2, float margin)
{
    return fabsf(f1 - f2) < margin;
}

bool Equal(const ALLEGRO_COLOR &color_1, const ALLEGRO_COLOR &color_2, float margin)
{
    return Equal(color_1.r, color_2.r, margin) && Equal(color_1.g, color_2.g, margin)
           && Equal(color_1.b, color_2.b, margin) && Equal(color_1.a, color_2.a, margin);
}

void Transform_color(ALLEGRO_COLOR &changed_color,
                     const ALLEGRO_COLOR &target_color,
                     float color_transformation_ratio)
{
    changed_color.r += (target_color.r - changed_color.r) * color_transformation_ratio;
    changed_color.g += (target_color.g - changed_color.g) * color_transformation_ratio;
    changed_color.b += (target_color.b - changed_color.b) * color_transformation_ratio;
    changed_color.a += (target_color.a - changed_color.a) * color_transformation_ratio;
}

float Average(float f1, float f2)
{
    return (f1 + f2) / 2;
}

class Vector
{
public:
    Vector(float x, float y)
        : x{x}
        , y{y}
    {}

    float X() const { return x; }
    float Y() const { return y; }

    void X(float val) { x = val; }
    void Y(float val) { y = val; }

    Vector operator-() const { return Vector(-x, -y); }

    Vector operator+(const Vector &v) const { return Vector(x + v.x, y + v.y); }

    Vector operator-(const Vector &v) const { return Vector(x - v.x, y - v.y); }

    Vector operator*(float f) const { return Vector(x * f, y * f); }

    Vector operator/(float f) const { return Vector(x / f, y / f); }

    void operator*=(float f) { x *= f, y *= f; }

    void operator/=(float f) { x /= f, y /= f; }

    void operator+=(const Vector &v) { x += v.x, y += v.y; }

    void operator-=(const Vector &v) { x -= v.x, y -= v.y; }

    bool operator==(const Vector &v) const { return v.x == x && v.y == y; }

    Vector Swap() const { return Vector(y, x); }

    Vector Unit() const { return *this / sqrtf(x * x + y * y); }

    Vector Abs() const { return Vector(fabsf(x), fabsf(y)); }

    static float Dot(const Vector &v1, const Vector &v2);

    float Magsq() const { return x * x + y * y; }

private:
    float x;
    float y;
};

Vector operator*(float f, const Vector &v)
{
    return v * f;
};

float Vector::Dot(const Vector &v1, const Vector &v2)
{
    return v1.x * v2.x + v1.y * v2.y;
};

class Matrix
{
public:
    Matrix(float f1, float f2, float f3, float f4)
        : row_1{f1, f2}
        , row_2{f3, f4}
    {}
    const Vector &Row_1() const { return row_1; }
    const Vector &Row_2() const { return row_2; }

private:
    Vector row_1;
    Vector row_2;
};

Vector operator*(const Matrix &m, const Vector &v)
{
    return Vector(Vector::Dot(m.Row_1(), v), Vector::Dot(m.Row_2(), v));
};

class Line
{
public:
    Line(float x1, float y1, float x2, float y2)
        : start{x1, y1}
        , end{x2, y2}
    {}

    Line(float x, float y, const Vector &end)
        : start{x, y}
        , end{end}
    {}

    Line(const Vector &start, float x, float y)
        : start{start}
        , end{x, y}
    {}

    Line(const Vector &start, const Vector &end)
        : start{start}
        , end{end}
    {}

    void Translate(const Vector &displacement) { start += displacement, end += displacement; }

    const Vector &Start() const { return start; }

    const Vector &End() const { return end; }

    float Length() const { return std::sqrtf((start - end).Magsq()); }

    void Draw(const ALLEGRO_COLOR &color, float line_width) const
    {
        al_draw_line(start.X(), start.Y(), end.X(), end.Y(), color, line_width);
    }

    Line Mirror_x(const Vector &point) const
    {
        Vector translate_start = Vector(0, (point - start).Y()) * 2;
        Vector translate_end = Vector(0, (point - end).Y()) * 2;

        return Line(start + translate_start, end + translate_end);
    }

    Line Mirror_y(const Vector &point) const
    {
        Vector translate_start = Vector((point - start).X(), 0) * 2;
        Vector translate_end = Vector((point - end).X(), 0) * 2;

        return Line(start + translate_start, end + translate_end);
    }

    Vector Direction() const { return end - start; }

    Vector Center() const
    {
        return Vector(
            Average(start.X(), end.X()),
            Average(start.Y(), end.Y())
        );
    }

private:
    Vector start;
    Vector end;
};

class Rectangle
{
public:
    Rectangle(float x, float y, float w, float h)
        : origin{x, y}
        , size{fabsf(w), fabsf(h)}
    {}

    Rectangle(const Vector &origin, const Vector &size)
        : origin{origin}
        , size{size.Abs()}
    {}

    Rectangle(const Vector &origin, float height)
        : origin{origin}
        , size{0, height}
    {}

    void Draw(const ALLEGRO_COLOR &color) const
    {
        al_draw_filled_rectangle(origin.X(),
                                 origin.Y(),
                                 origin.X() + size.X(),
                                 origin.Y() + size.Y(),
                                 color);
    }

    void Draw(const ALLEGRO_COLOR &line_color, float line_width) const
    {
        al_draw_rectangle(origin.X(),
                          origin.Y(),
                          origin.X() + size.X(),
                          origin.Y() + size.Y(),
                          line_color,
                          line_width);
    }

    void Translate(const Vector &displacement) { origin += displacement; };

    Line Top() const { return Line(origin, origin.X() + size.X(), origin.Y()); }

    Line Right() const { return Line(origin + size, origin.X() + size.X(), origin.Y()); }

    Line Bottom() const { return Line(origin + size, origin.X(), origin.Y() + size.Y()); }

    Line Left() const { return Line(origin, origin.X(), origin.Y() + size.Y()); }

    bool Contain(const Vector &point) const
    {
        Vector distance_to_origin = point - origin;

        return distance_to_origin.X() >= 0 && distance_to_origin.Y() >= 0
               && distance_to_origin.X() <= size.X() && distance_to_origin.Y() <= size.Y();
    }

    const Vector &Size() const { return size; }
    void Add_size_by(const Vector &value) { size += value; }

    float Width() const { return size.X(); }
    void Width(float val) { size.X(val); }

    float Height() const { return size.Y(); }
    void Height(float val) { size.Y(val); }

    const Vector &Origin() const { return origin; }
    void Origin(const Vector &origin) { this->origin = origin; }

    Vector Center() const { return origin + size / 2; }

    Rectangle Mirror_x(const Vector &point) const
    {
        Vector center = Center();
        Vector translate = Vector(0, (point - center).Y());

        return Rectangle(center + translate * 2 - size / 2, size);
    }

    Rectangle Mirror_y(const Vector &point) const
    {
        Vector center = Center();
        Vector translate = Vector((point - center).X(), 0);

        return Rectangle(center + translate * 2 - size / 2, size);
    }

    Vector Closest_point_to(const Vector &point) const
    // return the point on the rectangle edge if arg is outside rectangle
    // return arg itself if arg is inside rectangle
    {
        // float dx = std::max((origin - point).X(), 0.f);
        // dx = std::max(dx, (point - origin - size).X());

        // float dy = std::max((origin - point).Y(), 0.f);
        // dy = std::max(dy, (point - origin - size).Y());

        // return dx * dx + dy * dy;

        float x = point.X();
        x = std::max(x, origin.X());
        x = std::min(x, (origin + size).X());

        float y = point.Y();
        y = std::max(y, origin.Y());
        y = std::min(y, (origin + size).Y());

        return Vector(x, y);
    }

private:
    Vector origin;
    Vector size;
};

class Circle
{
public:
    Circle(float cx, float cy, float r)
        : center{cx, cy}
        , radius{r} {};

    Circle(const Vector &center, float r)
        : center{center}
        , radius{r} {};

    void Draw(const ALLEGRO_COLOR &color) const
    {
        al_draw_filled_circle(center.X(), center.Y(), radius, color);
    }

    void Draw(const ALLEGRO_COLOR &line_color, float line_width) const
    {
        al_draw_circle(center.X(), center.Y(), radius, line_color, line_width);
    }

    void Translate(const Vector &displacement) { center += displacement; }
    void Translate(float x, float y) { center += Vector(x, y); }
    void Scale(float multiplier) { radius *= multiplier; }
    void Add_radius_by(float value) { radius += value; }

    bool Contain(const Vector &point) const { return (point - center).Magsq() <= radius * radius; }

    const Vector &Center() const { return center; }
    void Center(const Vector &position) { center = position; }

    float Radius() const { return radius; }

    Circle Mirror_x(const Vector &point) const
    {
        Vector translate = Vector(0, (point - center).Y());

        return Circle(center + translate * 2, radius);
    }

    Circle Mirror_y(const Vector &point) const
    {
        Vector translate = Vector((point - center).X(), 0);

        return Circle(center + translate * 2, radius);
    }

private:
    Vector center;
    float radius;
};

class Triangle
{
public:
    Triangle(const Vector &vertex_1, const Vector &vertex_2, const Vector &vertex_3)
        : vertex_1{vertex_1}
        , vertex_2{vertex_2}
        , vertex_3{vertex_3} {};

    void Draw(const ALLEGRO_COLOR &color) const
    {
        al_draw_filled_triangle(vertex_1.X(),
                                vertex_1.Y(),
                                vertex_2.X(),
                                vertex_2.Y(),
                                vertex_3.X(),
                                vertex_3.Y(),
                                color);
    }

    void Draw(const ALLEGRO_COLOR &line_color, float line_width) const
    {
        al_draw_triangle(vertex_1.X(),
                         vertex_1.Y(),
                         vertex_2.X(),
                         vertex_2.Y(),
                         vertex_3.X(),
                         vertex_3.Y(),
                         line_color,
                         line_width);
    }

    // void translate(const Vector& displacement)
    // {
    //     vertex_1_ += displacement;
    //     vertex_2_ += displacement;
    //     vertex_3_ += displacement;
    // }

    void Vertex_1(const Vector &point) { vertex_1 = point; }
    void Vertex_2(const Vector &point) { vertex_2 = point; }
    void Vertex_3(const Vector &point) { vertex_3 = point; }

    const Vector &Vertex_1() const { return vertex_1; }
    const Vector &Vertex_2() const { return vertex_2; }
    const Vector &Vertex_3() const { return vertex_3; }

private:
    Vector vertex_1;
    Vector vertex_2;
    Vector vertex_3;
};

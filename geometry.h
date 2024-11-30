#include <math.h>
#include <allegro5/allegro_primitives.h>
#pragma once

bool Equal(float f1, float f2, float margin)
{
    return fabsf(f1 - f2) < margin;
}

bool Equal(const ALLEGRO_COLOR& color_1, const ALLEGRO_COLOR& color_2, float margin)
{
    return
        Equal(color_1.r, color_2.r, margin) &&
        Equal(color_1.g, color_2.g, margin) &&
        Equal(color_1.b, color_2.b, margin) &&
        Equal(color_1.a, color_2.a, margin);
}

void Transform_color(
    ALLEGRO_COLOR& changed_color,
    const ALLEGRO_COLOR& target_color,
    float color_transformation_ratio)
{
    changed_color.r += (target_color.r - changed_color.r) * color_transformation_ratio;
    changed_color.g += (target_color.g - changed_color.g) * color_transformation_ratio;
    changed_color.b += (target_color.b - changed_color.b) * color_transformation_ratio;
    changed_color.a += (target_color.a - changed_color.a) * color_transformation_ratio;
}

class Vector
{
public:
    Vector(float x, float y): x{x}, y{y} {};
    
    float X() const { return x; };
    float Y() const { return y; };

    void X(float val) { x = val; };
    void Y(float val) { y = val; };

    Vector operator-() const { return Vector(-x, -y); };

    Vector operator+(const Vector& v) const { return Vector(x + v.x, y + v.y); };

    Vector operator-(const Vector& v) const { return Vector(x - v.x, y - v.y); };

    Vector operator*(float f) const { return Vector(x * f, y * f); };

    Vector operator/(float f) const { return Vector(x / f, y / f); };

    void operator*=(float f) { x *= f, y *= f; };

    void operator/=(float f) { x /= f, y /= f; };

    void operator+=(const Vector& v) { x += v.x, y += v.y; };

    void operator-=(const Vector& v) { x -= v.x, y -= v.y; };

    Vector Unit() const { return *this / sqrtf(x * x + y * y); };

    Vector Abs() const { return Vector(fabsf(x), fabsf(y)); };

    static float Dot(const Vector& v1, const Vector& v2);

    float Magsq() const { return x * x + y * y; };
private:
    float x;
    float y;
};

Vector operator*(float f, const Vector& v)
{
    return v * f;
};

float Vector::Dot(const Vector& v1, const Vector& v2)
{
    return v1.x * v2.x + v1.y * v2.y;
};

class Matrix
{
public:
    Matrix(float f1, float f2, float f3, float f4): row_1{f1, f2}, row_2{f3, f4} {};
    const Vector& Row_1() const { return row_1; };
    const Vector& Row_2() const { return row_2; };
private:
    Vector row_1;
    Vector row_2;
};

Vector operator*(const Matrix& m, const Vector& v)
{
    return Vector(
        Vector::Dot(m.Row_1(), v),
        Vector::Dot(m.Row_2(), v)
    );
};

class Line
{
public:
    Line(float x1, float y1, float x2, float y2): start{x1, y1}, end{x2, y2} {};
    Line(float x, float y, const Vector& end): start{x, y}, end{end} {};
    Line(const Vector& start, float x, float y): start{start}, end{x, y} {};
    Line(const Vector& start, const Vector& end): start{start}, end{end} {};
    void Translate(const Vector& displacement) { start += displacement, end += displacement; };
    const Vector& Start() const { return start; }
    const Vector& End() const { return end; }
    void Draw(const ALLEGRO_COLOR& color, float line_width) const
    {
        al_draw_line(
            start.X(),
            start.Y(),
            end.X(),
            end.Y(),
            color,
            line_width
        );
    }
private:
    Vector start;
    Vector end;
};

class Rectangle
{
public:
    Rectangle(float x, float y, float w, float h):
        origin{x, y},
        size{fabsf(w), fabsf(h)}
    {};

    Rectangle(const Vector& origin, const Vector& size):
        origin{origin},
        size{size.Abs()}
    {};

    Rectangle(const Vector& origin, float height):
        origin{origin},
        size{0, height}
    {};

    void Draw(const ALLEGRO_COLOR& color) const
    {
        al_draw_filled_rectangle(
            origin.X(),
            origin.Y(),
            origin.X() + size.X(),
            origin.Y() + size.Y(),
            color
        );
    }

    void Draw(const ALLEGRO_COLOR& line_color, float line_width) const
    {
        al_draw_rectangle(
            origin.X(),
            origin.Y(),
            origin.X() + size.X(),
            origin.Y() + size.Y(),
            line_color,
            line_width
        );
    }

    void Translate(const Vector& displacement) { origin += displacement; };

    Line Top() const { return Line(
        origin,
        origin.X() + size.X(),
        origin.Y()
    ); };

    Line Right() const { return Line(
        origin + size,
        origin.X() + size.X(),
        origin.Y()
    ); };

    Line Bottom() const { return Line(
        origin + size,
        origin.X(),
        origin.Y() + size.Y()
    ); }

    Line Left() const { return Line(
        origin,
        origin.X(),
        origin.Y() + size.Y()
    ); }

    bool Contain(const Vector& point) const
    {
        Vector distance_to_origin = point - origin;

        return 
            distance_to_origin.X() >= 0 &&
            distance_to_origin.Y() >= 0 &&
            distance_to_origin.X() <= size.X() &&
            distance_to_origin.Y() <= size.Y()
        ;
    }

    const Vector& Size() const { return size; }
    void Add_size_by(const Vector& value) { size += value; }
    
    float Width() const { return size.X(); }
    void Width(float val) { size.X(val); }

    float Height() const { return size.Y(); }
    void Height(float val) { size.Y(val); }
    
    const Vector& Origin() const { return origin; }
    void Origin(const Vector& origin) { this->origin = origin; }
private:
    Vector origin;
    Vector size;
};

class Circle
{
public:
    Circle(float cx, float cy, float r):
        center{cx, cy},
        radius{r}
    {};

    Circle(const Vector& center, float r):
        center{center},
        radius{r}
    {};

    void Draw(const ALLEGRO_COLOR& color) const
    {
        al_draw_filled_circle(
            center.X(),
            center.Y(),
            radius,
            color
        );
    }

    void Draw(const ALLEGRO_COLOR& line_color, float line_width) const
    {
        al_draw_circle(
            center.X(),
            center.Y(),
            radius,
            line_color,
            line_width
        );
    }

    void Translate(const Vector& displacement) { center += displacement; }
    void Translate(float x, float y) { center += Vector(x, y); }
    void Scale(float multiplier) { radius *= multiplier; }
    void Add_radius_by(float value) { radius += value; }

    bool Contain(const Vector& point) const
    {
        return (point - center).Magsq() <= radius * radius;
    }

    const Vector& Center() const { return center; }
    void Center(const Vector& position) { center = position; }

    float Radius() const { return radius; }
private:
    Vector center;
    float radius;
};

class Triangle
{
public:
    Triangle(const Vector& vertex_1, const Vector& vertex_2, const Vector& vertex_3):
        vertex_1{vertex_1},
        vertex_2{vertex_2},
        vertex_3{vertex_3}
    {};

    void Draw(const ALLEGRO_COLOR& color) const
    {
        al_draw_filled_triangle(
            vertex_1.X(),
            vertex_1.Y(),
            vertex_2.X(),
            vertex_2.Y(),
            vertex_3.X(),
            vertex_3.Y(),
            color
        );
    }

    void Draw(const ALLEGRO_COLOR& line_color, float line_width) const
    {
        al_draw_triangle(
            vertex_1.X(),
            vertex_1.Y(),
            vertex_2.X(),
            vertex_2.Y(),
            vertex_3.X(),
            vertex_3.Y(),
            line_color,
            line_width
        );
    }

    // void translate(const Vector& displacement)
    // {
    //     vertex_1_ += displacement;
    //     vertex_2_ += displacement;
    //     vertex_3_ += displacement;
    // }

    void Vertex_1(const Vector& point) { vertex_1 = point; }
    void Vertex_2(const Vector& point) { vertex_2 = point; }
    void Vertex_3(const Vector& point) { vertex_3 = point; }

    const Vector& Vertex_1() const { return vertex_1; }
    const Vector& Vertex_2() const { return vertex_2; }
    const Vector& Vertex_3() const { return vertex_3; }
private:
    Vector vertex_1;
    Vector vertex_2;
    Vector vertex_3;
};
#include "param.h"
#include "geometry.h"
#include <algorithm>
#include <vector>
#include <array>
#pragma once

class Box
{
public:
    Box(const Vector& center)
    :
        shape{
            center - Vector(param::unit_length, param::unit_length) / 2,
            Vector(param::unit_length, param::unit_length)
        }
    {}

    void Draw() const
    {
        shape.Draw(param::yellow);
    }

    const Rectangle& Shape() const
    {
        return shape;
    }
private:
    Rectangle shape;
};

class Tree
{
public:
    Tree(const Vector& center)
    :
        shape{6, Circle(center, param::unit_length)}
    {
        std::for_each(shape.begin(), shape.end(), [i = 0](Circle& c) mutable
        {
            c.Translate(
                2 * param::unit_length * std::cosf(param::pi / 6 * i),
                2 * param::unit_length * std::sinf(param::pi / 6 * i)
            );

            i++;
        });
    }

    void Draw() const
    {
        std::for_each(shape.begin(), shape.end(), [](const Circle& c)
        {
            c.Draw(param::green);
        });
    }
private:
    std::vector<Circle> shape;
};

class X
{
public:
    X(const Vector& center)
    :
        shape{
            Line(center, center + Vector(1.5 * param::unit_length, 1.5 * param::unit_length)),
            Line(center, center - Vector(1.5 * param::unit_length, 1.5 * param::unit_length)),
            Line(center, center + Vector(-1.5 * param::unit_length, 1.5 * param::unit_length)),
            Line(center, center + Vector(1.5 * param::unit_length, -1.5 * param::unit_length))
        }
    {}

    void Draw() const
    {
        std::for_each(shape.begin(), shape.end(), [](const Line& l)
        {
            l.Draw(param::red, param::line_width);
        });
    }
private:
    std::array<Line, 4> shape;
};

class Glass
{
public:
    Glass(const Vector& start, const Vector& end)
    :
        shape{start, end}
    {}

    void Draw() const
    {
        shape.Draw(param::blue, param::line_width);
    }
private:
    Line shape;
};

class Map_1
{
public:
    Map_1()
    {}

    void Draw() const
    {
        for (const Box& box : boxes)
            box.Draw();

        for (const Tree& tree : trees)
            tree.Draw();
        
        for (const X& x : xs)
            x.Draw();
        
        for (const Glass& glass : glasses)
            glass.Draw();
    }
private:
    std::vector<Box> boxes;
    std::vector<Tree> trees;
    std::vector<X> xs;
    std::vector<Glass> glasses;
};
#include "character.hpp"
#include "collision.hpp"
#include "geometry.hpp"
#include "object.hpp"
#include "param.hpp"
#include <algorithm>
#include <array>
#include <vector>
#pragma once

class Wall
{
public:
    Wall(const Vector &origin, const Vector &size)
        : shape{origin, size}
    {}

    void Draw() const { shape.Draw(param::yellow); }

    const Rectangle &Shape() const { return shape; }

    void Translate(const Vector &displacement) { shape.Translate(displacement); }

    float Width() const { return shape.Width(); }

    float Height() const { return shape.Height(); }

    Wall Mirror_x(const Vector &point) const
    {
        Wall temp = *this;
        temp.shape = temp.shape.Mirror_x(point);
        return temp;
        // return Wall(shape.Mirror_x(point));
    }

    Wall Mirror_y(const Vector &point) const
    {
        Wall temp = *this;
        temp.shape = temp.shape.Mirror_y(point);
        return temp;
        // return Wall(shape.Mirror_y(point));
    }

private:
    Rectangle shape;
    // Wall(const Rectangle& shape): shape{shape} {}
};

class Tree
{
public:
    Tree(const Vector &center, float overall_diameter)
        : diameter{overall_diameter}
        , shape{6, Circle(center, overall_diameter / 6)}
        , filler{center, shape.front().Radius() * 1.7321f}
    {
        std::array<float, 6> xs{1, 0.5f, -0.5f, -1, -0.5f, 0.5f};
        std::array<float, 6> ys{0, 0.5f, 0.5f, 0, -0.5f, -0.5f};

        std::for_each(shape.begin(), shape.end(), [&, i = 0](Circle &c) mutable {
            c.Translate(2 * c.Radius() * xs.at(i), 2 * c.Radius() * ys.at(i) * param::sqrt_3);

            i++;
        });
    }

    void Draw() const
    {
        std::for_each(shape.begin(), shape.end(), [](const Circle &c) { c.Draw(param::green); });

        filler.Draw(param::green);
    }

    void Translate(const Vector &displacement)
    {
        std::for_each(shape.begin(), shape.end(), [&](Circle &c) { c.Translate(displacement); });

        filler.Translate(displacement);
    }

    void Translate(float x, float y)
    {
        std::for_each(shape.begin(), shape.end(), [&](Circle &c) { c.Translate(x, y); });

        filler.Translate(x, y);
    }

    float Diameter() const { return diameter; }

    Tree Mirror_x(const Vector &point) const
    {
        Tree temp = *this;

        temp.filler = temp.filler.Mirror_x(point);

        std::transform(temp.shape.begin(),
                       temp.shape.end(),
                       temp.shape.begin(),
                       [&](const Circle &c) { return c.Mirror_x(point); });

        return temp;
    }

    float Min_t(const Pawn &moving_pawn) const
    {
        std::vector<float> t;
        t.reserve(shape.size());

        for (const Circle &circle : shape)
            t.push_back(collision::Circle_vs_circle(moving_pawn.Shape(),
                                                    circle,
                                                    moving_pawn.Last_translation()));

        return *std::min_element(t.begin(), t.end());
    }

private:
    float diameter;
    std::vector<Circle> shape;
    Circle filler;
};

class X
{
public:
    X(const Vector &center, float size)
        : size{size}
        , shape{Line(center, center + Vector(size, size) / 2),
                Line(center, center - Vector(size, size) / 2),
                Line(center, center + Vector(-size, size) / 2),
                Line(center, center + Vector(size, -size) / 2)}
    {}

    void Draw() const
    {
        std::for_each(shape.begin(), shape.end(), [](const Line &l) {
            l.Draw(param::red, param::line_width * 2);
        });
    }

    void Translate(const Vector &displacement)
    {
        std::for_each(shape.begin(), shape.end(), [&](Line &l) { l.Translate(displacement); });
    }

    float Size() const { return size; }

    float Min_t(const Pawn &moving_pawn) const
    {
        std::vector<float> t;
        t.reserve(shape.size());

        for (const Line &line : shape)
            t.push_back(collision::Circle_vs_line(moving_pawn.Shape(),
                                                  line,
                                                  moving_pawn.Last_translation()));

        return *std::min_element(t.begin(), t.end());
    }

private:
    float size;
    std::array<Line, 4> shape;
};

class Window
{
public:
    Window(const Vector &start, const Vector &end)
        : shape{start, end}
    {}

    void Draw() const { shape.Draw(param::blue, param::line_width * 2); }

    void Translate(const Vector &displacement) { shape.Translate(displacement); }

    float Length() const { return shape.Length(); }

    const Line &Shape() const { return shape; }

    Vector Center() const { return shape.Center(); }

private:
    Line shape;
};

class Map
{
public:
    void Draw() const
    {
        for (const Window &window : windows)
            window.Draw();

        for (const Wall &wall : walls)
            wall.Draw();

        for (const Tree &tree : trees)
            tree.Draw();

        for (const X &x : xs)
            x.Draw();
    }

    void Wall_stop(Pawn &moving_pawn) const
    {
        std::for_each(walls.begin(), walls.end(), [&](const Wall &wall) {
            float t = collision::Circle_vs_rectangle(moving_pawn.Shape(),
                                                     wall.Shape(),
                                                     moving_pawn.Last_translation());

            if (t == 2)
                return;

            moving_pawn.Retreat(1 - t);
            moving_pawn.Stop();
        });
    }

    void Tree_stop(Pawn &moving_pawn) const
    {
        std::for_each(trees.begin(), trees.end(), [&](const Tree &tree) {
            float t = tree.Min_t(moving_pawn);

            if (t == 2)
                return;

            moving_pawn.Retreat(1 - t);
            moving_pawn.Stop();
        });
    }

    void X_kill(Pawn &moving_pawn, std::set<Pawn *> &dying_pawns) const
    {
        std::for_each(xs.begin(), xs.end(), [&](const X &x) {
            float t = x.Min_t(moving_pawn);

            if (t == 2 || Pawn::Vanish_immediately())
                return;

            moving_pawn.Retreat(1 - t);
            moving_pawn.Stop();
            dying_pawns.insert(&moving_pawn);
        });
    }

    void Window_only_shoot(Pawn &moving_pawn) const
    {
        std::for_each(windows.begin(), windows.end(), [&](const Window &window) {
            float t = collision::Circle_vs_line(moving_pawn.Shape(),
                                                window.Shape(),
                                                moving_pawn.Last_translation());

            if (t != 2)
                Pawn::Vanish_immediately(true);
        });
    }

protected:
    Map(const Fence &fence,
        float the_number_of_walls,
        float the_number_of_windows,
        float the_number_of_xs,
        float the_number_of_trees)
        : fence{fence}
    {
        walls.reserve(the_number_of_walls);
        windows.reserve(the_number_of_windows);
        xs.reserve(the_number_of_xs);
        trees.reserve(the_number_of_trees);
    }

    const Fence &fence;
    std::vector<Wall> walls;
    std::vector<Window> windows;
    std::vector<X> xs;
    std::vector<Tree> trees;
};

class Map_1 : public Map
{
public:
    Map_1(const Fence &fence)
        : Map{fence, 6, 4, 2, 2}
    {
        for (int i = 0; i < walls.capacity(); i++)
            walls.emplace_back(fence.Origin(), Vector(param::unit_length, param::unit_length) * 6);

        for (int i = 0; i < windows.capacity(); i++)
            windows.emplace_back(fence.Origin(), fence.Origin() + Vector(0, walls.front().Height()));

        for (int i = 0; i < xs.capacity(); i++)
            xs.emplace_back(Vector(0, 0), param::unit_length * 6);

        for (int i = 0; i < trees.capacity(); i++)
            trees.emplace_back(fence.Center(), param::unit_length * 6);

        // Don't change this functions call order:
        arrange_walls();
        arrange_windows();
        arrange_xs();
        arrange_trees();
    }

private:
    void arrange_walls()
    {
        // std::vector<Wall> corners{3, walls.front()};
        // corners.at(1).Translate(Vector(corners.front().Width(), 0));
        // corners.at(2).Translate(Vector(0, corners.front().Height()));

        std::for_each(walls.begin(), walls.end(), [&, i = 0](Wall &wall) mutable {
            if (i % 2 == 0)
                wall.Translate(Vector(fence.Width() / 4 - wall.Width(), 0));
            else
                wall.Translate(Vector(fence.Width() * 3 / 4, 0));

            wall.Translate(Vector(0, fence.Height() / 2 - wall.Height() / 2));
            i++;
        });

        std::for_each(walls.begin(), walls.begin() + 2, [](Wall &wall) {
            wall.Translate(Vector(0, -wall.Height() * 2));
        });

        std::for_each(walls.end() - 2, walls.end(), [](Wall &wall) {
            wall.Translate(Vector(0, wall.Height() * 2));
        });

        // walls.insert(walls.end(), corners.begin(), corners.end());

        // std::transform(corners.begin(), corners.end(), corners.begin(), [&](const Wall &corner) {
        //     return corner.Mirror_x(fence.Center());
        // });
        // walls.insert(walls.end(), corners.begin(), corners.end());

        // std::transform(corners.begin(), corners.end(), corners.begin(), [&](const Wall &wall) {
        //     return wall.Mirror_y(fence.Center());
        // });
        // walls.insert(walls.end(), corners.begin(), corners.end());

        // std::transform(corners.begin(), corners.end(), corners.begin(), [&](const Wall &wall) {
        //     return wall.Mirror_x(fence.Center());
        // });
        // walls.insert(walls.end(), corners.begin(), corners.end());
    }

    void arrange_windows()
    {
        std::for_each(windows.begin(), windows.end(), [&, i = 0](Window &window) mutable {
            if (i % 2 == 0)
                window.Translate(Vector(fence.Width() / 4 - walls.front().Width() / 2, 0));
            else
                window.Translate(Vector(fence.Width() * 3 / 4 + walls.front().Width() / 2, 0));

            window.Translate(Vector(0, fence.Height() / 2 - window.Length() / 2));

            i++;
        });

        std::for_each(windows.begin(), windows.begin() + 2, [](Window &window) {
            window.Translate(Vector(0, -window.Length()));
        });

        std::for_each(windows.end() - 2, windows.end(), [](Window &window) {
            window.Translate(Vector(0, window.Length()));
        });
    }

    void arrange_xs()
    {
        // std::for_each(xs.begin(), xs.end(), [&](X &x) {
        //     x.Translate(Vector(fence.Width() / 2, fence.Height() / 2));
        // });

        // std::array<int, 4> widths{1, -1, -1, 1};
        // std::array<int, 4> heights{-1, -1, 1, 1};

        // for (int i = 0; i < 4; i++) {
        //     xs.at(i + 1).Translate(
        //         Vector((fence.Width() / 8 - xs.at(i).Size() / 2) * widths.at(i),
        //                (fence.Width() / 8 - xs.at(i).Size() / 2) * heights.at(i)));
        // }

        xs.front().Translate(Vector(param::window_width / 2, windows.front().Center().Y()));
        xs.back().Translate(Vector(param::window_width / 2, windows.back().Center().Y()));
    }

    void arrange_trees()
    {
        // std::for_each(trees.begin(), trees.end(), [&](Tree &tree) {
        //     tree.Translate(Vector(0, fence.Height() / 2 - tree.Diameter() / 2));
        // });

        // std::array<float, 5> xs{1, 0.5f, 0, -0.5f, -1};
        // std::array<float, 5> ys{0, -0.5f, -1, -0.5f, 0};

        // std::for_each(trees.begin() + 1, trees.end(), [&, i = 0](Tree &t) mutable {
        //     t.Translate(t.Diameter() * xs.at(i), t.Diameter() * ys.at(i) * param::sqrt_3);

        //     i++;
        // });

        trees.front().Translate(Vector(0, -fence.Height() * 0.5f + trees.front().Diameter() * 0.5f));
        trees.back().Translate(Vector(0, -fence.Height() * 0.5f + trees.back().Diameter() * 1.5f));

        std::vector<Tree> temp = trees;

        std::transform(temp.begin(), temp.end(), temp.begin(), [&](const Tree &t) {
            return t.Mirror_x(fence.Center());
        });

        trees.insert(trees.end(), temp.begin(), temp.end());
    }
};

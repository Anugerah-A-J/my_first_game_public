#include "geometry.hpp"
#include "param.hpp"
#include <algorithm>
#include <string>
#include <vector>
#pragma once

class One_line_text
{
public:
    // static void Font(const ALLEGRO_FONT* font) { font = font; }
    // float Font_height() { return al_get_font_line_height(font); }
    // static float font_width() { return al_get_font_line_height(font_); }

    One_line_text(const Vector &origin,
                  const std::string &text,
                  const ALLEGRO_FONT *const font,
                  const ALLEGRO_COLOR &text_color)
        : text{text}
        , // no validation for \n
        font{font}
        , text_color{text_color}
        , shape{origin,
                Vector(
                    // 8 * text.length(),
                    al_get_text_width(font, &text.front()),
                    al_get_font_line_height(font))}
    {}

    void Draw() const
    {
        al_draw_text(font,
                     text_color,
                     shape.Origin().X(),
                     shape.Origin().Y(),
                     ALLEGRO_ALIGN_LEFT,
                     &text.front());
    }

    bool Contain(const Vector &point) const { return shape.Contain(point); }

    float Width() const { return shape.Width(); }

    const std::string &Text() const { return text; }

    void Make_active() { text_color = param::default_theme.active_text_color; }
    void Make_passive() { text_color = param::default_theme.passive_text_color; }
    const Vector &Origin() const { return shape.Origin(); }
    void Translate(const Vector &displacement) { shape.Translate(displacement); }

private:
    std::string text;
    const ALLEGRO_FONT *const font;
    ALLEGRO_COLOR text_color;
    Rectangle shape;
};

class Dialog_box
{
public:
    void Update_selected_choice(int allegro_keyboard_event_keycode)
    {
        if (allegro_keyboard_event_keycode != ALLEGRO_KEY_DOWN
            && allegro_keyboard_event_keycode != ALLEGRO_KEY_UP)
            return;

        choices.at(selected_choice_index).Make_passive();

        if (allegro_keyboard_event_keycode == ALLEGRO_KEY_DOWN
            && selected_choice_index != choices.size() - 1)
            selected_choice_index++;

        else if (allegro_keyboard_event_keycode == ALLEGRO_KEY_DOWN
                 && selected_choice_index == choices.size() - 1)
            selected_choice_index = 0;

        if (allegro_keyboard_event_keycode == ALLEGRO_KEY_UP && selected_choice_index != 0)
            selected_choice_index--;

        else if (allegro_keyboard_event_keycode == ALLEGRO_KEY_UP && selected_choice_index == 0)
            selected_choice_index = choices.size() - 1;

        choices.at(selected_choice_index).Make_active();
    }

    void Update_selected_choice(const Vector &mouse_coordinate)
    {
        std::vector<One_line_text>::iterator selected_choice_iterator
            = find_if(choices.begin(), choices.end(), [&](const One_line_text &choice) {
                  return choice.Contain(mouse_coordinate);
              });

        if (selected_choice_iterator != choices.end()
            && selected_choice_iterator - choices.begin() != selected_choice_index) {
            choices.at(selected_choice_index).Make_passive();
            selected_choice_index = selected_choice_iterator - choices.begin();
            choices.at(selected_choice_index).Make_active();
        }
    }

    void Draw() const
    {
        shape.Draw(color);
        shape.Draw(line_color, line_width);

        for (const One_line_text &message : messages)
            message.Draw();

        for (const One_line_text &choice : choices)
            choice.Draw();
    }

    int Selected_choice_index() const { return selected_choice_index; }

    void Add_message(const std::string &text,
                     const ALLEGRO_COLOR &text_color = param::default_theme.passive_text_color,
                     const ALLEGRO_COLOR &background_color = param::default_theme.background_color)
    {
        if (Messages_width() + (text.length() + 1) * monospaced_font_width > shape.Width())
            shape.Width(Messages_width() + (text.length() + 1) * monospaced_font_width);

        shape.Origin(center - shape.Size() * 0.5f);

        Update_messages_origin();

        messages.emplace_back(Message_origin(), text, monospaced_font, text_color);
    }

    void Erase_message() { messages.clear(); }

protected:
    Dialog_box(
        const Vector& center,
        const ALLEGRO_FONT* const monospaced_font,
        const ALLEGRO_COLOR& color = param::default_theme.background_color,
        const ALLEGRO_COLOR& line_color = param::default_theme.line_color
    ):
        center{center},
        monospaced_font{monospaced_font},
        monospaced_font_height{al_get_font_line_height(monospaced_font)},
        monospaced_font_width{al_get_text_width(monospaced_font, "a")},
        shape{
            center - Vector(0, monospaced_font_height),
            2 * monospaced_font_height,
        },
        color{color},
        line_color{line_color},
        line_width{param::line_width},
        selected_choice_index{0}
    {}

    void Add_choice(const std::string &text,
                    const ALLEGRO_COLOR &text_color = param::default_theme.passive_text_color,
                    const ALLEGRO_COLOR &background_color = param::default_theme.background_color)
    {
        if ((text.length() + 1) * monospaced_font_width > shape.Width())
            shape.Width((text.length() + 1) * monospaced_font_width);

        shape.Height(shape.Height() + 1.5 * monospaced_font_height);

        shape.Origin(center - shape.Size() * 0.5);

        Update_choices_origin();

        choices.emplace_back(Choice_origin(), text, monospaced_font, text_color);

        if (choices.size() == 1)
            choices.front().Make_active();
    }

private:
    float Messages_width() const
    {
        float sum = 0;

        for (const One_line_text &message : messages)
            sum += message.Width();

        return sum;
    }

    Vector Message_origin() const
    {
        return shape.Origin()
               + Vector(Messages_width() + monospaced_font_width * 0.5f,
                        monospaced_font_height * 0.5f);
    }

    void Update_messages_origin()
    {
        if (messages.empty())
            return;

        Vector displacement = shape.Origin() - messages.front().Origin();

        std::for_each(messages.begin(), messages.end(), [&](One_line_text &message) {
            message.Translate(displacement);
        });
    }

    void Update_choices_origin()
    {
        if (choices.empty())
            return;

        Vector displacement = shape.Origin()
                              + Vector(monospaced_font_width * 0.5f, monospaced_font_height * 2)
                              - choices.front().Origin();

        std::for_each(choices.begin(), choices.end(), [&](One_line_text &choice) {
            choice.Translate(displacement);
        });
    }

    Vector Choice_origin() const
    {
        return shape.Origin()
               + Vector(monospaced_font_width * 0.5f,
                        (2 + choices.size() * 1.5) * monospaced_font_height);
    }

    const Vector center;
    const ALLEGRO_FONT *const monospaced_font;
    int monospaced_font_height;
    int monospaced_font_width;
    Rectangle shape;
    ALLEGRO_COLOR color;
    ALLEGRO_COLOR line_color;
    float line_width;
    std::vector<One_line_text> messages;
    std::vector<One_line_text> choices;
    int selected_choice_index;
};

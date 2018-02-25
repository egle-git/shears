#ifndef ANSI_SEQ_H
#define ANSI_SEQ_H

/// ANSI terminal control sequences
namespace ansi
{

/// Represents a color
struct color
{
    const int code;
};

/// Represents a color type (background and foreground)
struct color_type
{
    const int code;
};

const color_type foreground = color_type{30};
const color_type background = color_type{40};

const color black = color{0};
const color red = color{1};
const color green = color{2};
const color yellow = color{3};
const color blue = color{4};
const color magenta = color{5};
const color cyan = color{6};
const color white = color{7};

const color bright_red = color{61};
const color bright_green = color{62};
const color bright_yellow = color{63};
const color bright_blue = color{64};
const color bright_magenta = color{65};
const color bright_cyan = color{66};
const color bright_white = color{67};

/// Returns the sequence one has to print in order to use the given color.
inline std::string setcolor(color color, color_type type = foreground)
{
    return "\33[" + std::to_string(type.code + color.code) + "m";
}

/// Returns the sequence one has to print to get the terminal back to normal.
inline std::string reset() { return "\33[0m"; }

/// Returns the sequence one has to print to clear the current line.
inline std::string clear_line() { return "\033[K"; }
}

#endif // ANSI_SEQ_H

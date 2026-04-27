/**
 * @file        utils/string.hpp
 * @brief       String manipulation library
 * @author      Justin Scott
 * @date        2026-01-27
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <string>
#include <vector>



namespace utils::str
{
using namespace std::string_view_literals;

// Regex to check is a string isn't empty. A string that is only spaces is considered empty according to this
inline constexpr const char* const NON_EMPTY_REGEX = R"(^(?!\s*$).+)";

///////////////////////////////////////////////////////////////////////////////
/// Tokenize a std::string_view. Essentialy qTokenize()
///
/// @param[in]  haystack            String to tokenize
/// @param[in]  needle                 String to split on
///
/// @return     Vector of tokenized strings
///
///////////////////////////////////////////////////////////////////////////////
inline std::vector<std::string> tokenize (std::string_view haystack, const std::string& needle)
    {
    std::vector<std::string> res;

    while (!haystack.empty ())
        {
        size_t end = haystack.find (needle);

        if (std::string_view::npos == end)
            {
            res.emplace_back (haystack);
            break;
            }
        else
            {
            res.emplace_back (haystack.substr (0, end));
            }

        haystack = haystack.substr (end + needle.size ());
        }

    return res;
    }

///////////////////////////////////////////////////////////////////////////////
/// Split a string along a seperator. Resulting strings do not contain the seperator
///
/// @param[in]  str         String to seperate
/// @param[in]  sep         Seperator  string
///
/// @return     pair containing split string.
///             std::pair::first is the portion before the seperator
///             std::pair::second is the portion after the seperator
///
/// @remarks    If str does not contain sep, std::pair::first will be str, and std::pair::second will be empty
///
///////////////////////////////////////////////////////////////////////////////
inline std::pair<std::string_view, std::string_view>
split (std::string_view str, std::string_view sep)
    {
    size_t end = str.find (sep);

    return std::make_pair (str.substr (0, end),
                           std::string_view::npos == end ?
                           ""sv : str.substr (end + sep.size ()));
    }

///////////////////////////////////////////////////////////////////////////////
/// Extract a value bounded by a start and end token
///
/// @param[in]  str             String to extract from
/// @param[in]  start         Start token
/// @param[in]  end             End token
///
/// @return     Extracted string
///
/// @remarks    Takes a string formatted as:
///             "...<start><return-value><end>..."
///             e.g. "lorem=<value>=ipsum"
///             where start="=<", end=">=", return-value="value"
///             If either start or end is missing, this returns an empty string
///             If mutliple instance of start or end are present in str, the first instances are used.
///
///////////////////////////////////////////////////////////////////////////////
inline std::string_view
extract (std::string_view str, std::string_view start, std::string_view end)
    {
    std::string_view    res;
    size_t              startIdx;
    size_t              endIdx;

    if (std::string_view::npos == (startIdx = str.find (start)))
        {
        // Does not contain starting character
        }
    else if (std::string_view::npos ==
                    (endIdx = str.find (end, startIdx + start.size ())))
        {
        // Does not copntain ending character
        }
    else
        {
        res = str.substr (startIdx + start.size (),
                          endIdx - startIdx - start.size ());
        }

    return res;
    }


///////////////////////////////////////////////////////////////////////////////
/// Format a boolean as on/off
///
/// @param[in]  bl      Boolean value
///
/// @return     On if true
///             Off if false
///
///////////////////////////////////////////////////////////////////////////////
inline std::string formatOnOff (bool bl) { return bl ? "On" : "Off"; }

} // namespace utils::str


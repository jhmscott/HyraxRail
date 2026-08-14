/**
 * @file        utils/string.hpp
 * @brief       String manipulation library
 * @author      Justin Scott
 * @date        2026-01-27
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <QObject>
#include <QString>

#include <string>
#include <vector>

namespace utils::str
{

// Regex to check is a string isn't empty.
// A string that is only spaces is considered empty according to this
inline constexpr const char* const NON_EMPTY_REGEX = R"(^(?!\s*$).+)";

///////////////////////////////////////////////////////////////////////////////
/// Tokenize a std::string_view. Essentially qTokenize()
///
/// @param[in]  haystack    String to tokenize
/// @param[in]  needle      String to split on
///
/// @return     Vector of tokenized strings
///
///////////////////////////////////////////////////////////////////////////////
std::vector<std::string> tokenize (std::string_view haystack, const std::string& needle);

///////////////////////////////////////////////////////////////////////////////
/// Split a string along a separator. Resulting strings do not contain the separator
///
/// @param[in]  str         String to separate
/// @param[in]  sep         Separator  string
///
/// @return     pair containing split string.<BR>
///             std::pair::first is the portion before the separator<BR>
///             std::pair::second is the portion after the separator
///
/// @remarks    If str does not contain sep, std::pair::first will be str,
///             and std::pair::second will be empty
///
///////////////////////////////////////////////////////////////////////////////
std::pair<std::string_view, std::string_view>
split (std::string_view str, std::string_view sep);

///////////////////////////////////////////////////////////////////////////////
/// Extract a value bounded by a start and end token
///
/// @param[in]  str         String to extract from
/// @param[in]  start       Start token
/// @param[in]  end         End token
///
/// @return     Extracted string
///
/// @remarks    Takes a string formatted as:
///             "...<start><return-value><end>..."
///             e.g. "lorem=<value>=ipsum"
///             where start="=<", end=">=", return-value="value"
///             If either start or end is missing, this returns an empty string
///             If multiple instance of start or end are present in str,
///             the first instances are used.
///
///////////////////////////////////////////////////////////////////////////////
std::string_view extract (std::string_view str, std::string_view start, std::string_view end);

///////////////////////////////////////////////////////////////////////////////
/// Format a boolean as on/off
///
/// @param[in]  bl      Boolean value
///
/// @return     On if true
///             Off if false
///
///////////////////////////////////////////////////////////////////////////////
inline QString formatOnOff (bool bl) { return bl ? QObject::tr ("On") : QObject::tr ("Off"); }

///////////////////////////////////////////////////////////////////////////////
/// Escape text for use with a QButton
///
/// @param[in]  str     String to escape
///
/// @return     Escaped string
///
///////////////////////////////////////////////////////////////////////////////
inline QString escape (QString str) { return str.replace ("&", "&&"); }

///////////////////////////////////////////////////////////////////////////////
/// Remove diacritics from a string
///
/// @param[in]  text    Text with diacritics
///
/// @return     Text without diacritics
///
/// @remarks    Avoid this whenever you can. This can change the meaning of
///             localized strings. Only use when there is no other way to render
///             the diacritics, like with the 7 and 14 seg displays
///
///////////////////////////////////////////////////////////////////////////////
QString removeDiacritics (const QString& text);

} // namespace utils::str


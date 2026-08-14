/**
 * @file        utils/string.cpp
 * @brief       String manipulation library
 * @author      Justin Scott
 * @date        2026-08-11
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <utils/string.hpp>

using namespace std::string_view_literals;

namespace utils::str
{
std::vector<std::string> tokenize (std::string_view haystack, const std::string& needle)
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

std::pair<std::string_view, std::string_view>
split (std::string_view str, std::string_view sep)
    {
    size_t end = str.find (sep);

    return std::make_pair (str.substr (0, end),
                           std::string_view::npos == end ?
                           ""sv : str.substr (end + sep.size ()));
    }

std::string_view extract (std::string_view str, std::string_view start, std::string_view end)
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

QString removeDiacritics (const QString& text)
    {
    // Decompose characters into base letters + standalone diacritics
    QString formD = text.normalized (QString::NormalizationForm_D);
    QString filtered;

    filtered.reserve (formD.length ());

    // Filter out the diacritc characters
    std::copy_if (formD.begin (),
                  formD.end (),
                  std::back_inserter (filtered),
                  [] (QChar ch) -> bool
                  {
                  return QChar::Mark_NonSpacing != ch.category ();
                  });

    return filtered;
    }

} // namespace utils::str
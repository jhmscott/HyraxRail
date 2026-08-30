/**
 * @file        utils/stringtest.hpp
 * @brief       Test suite for the string utility library
 * @author      Justin Scott
 * @date        2026-08-11
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <utils/string.hpp>

#include <QRegularExpression>
#include <QtTest>

using namespace std::string_literals;
using namespace std::string_view_literals;
using namespace utils::str;

static const QString UNACCENTED_STRING =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the string utility library
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class StringTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for tokenizeTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void tokenizeTest_data ()
        {
        QTest::addColumn<std::string> ("string");
        QTest::addColumn<std::string> ("sep");

        // Basic cases
        QTest::newRow ("Space") << "Space separated text"s   << " "s;
        QTest::newRow ("Comma") << "Comma,separated,text"s   << ","s;
        QTest::newRow ("Pipe")  << "Pipe|separated|text"s    << "|"s;
        QTest::newRow ("LF")    << "Line\nfeed\ntext"s       << "\n"s;
        QTest::newRow ("Equal") << "Equal==separated==text"s << "=="s;

        // Edge cases
        QTest::newRow ("End")       << "Separator on end "s   << " "s;
        QTest::newRow ("Start")     << " Separator at start"s << " "s;
        QTest::newRow ("Repeated")  << "Repeated,,separator"s << ","s;
        QTest::newRow ("All")       << "Oops all separator"s  << "Oops all separator"s;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test tokenize() by comparing it's results to qTokenize()
    ///
    /// @see    utils::str::tokenize()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void tokenizeTest ()
        {
        QFETCH (std::string, string);
        QFETCH (std::string, sep);

        QString qString = string.c_str ();
        QString qSep    = sep.c_str ();

        auto tokenized  = tokenize (string, sep);
        auto qTokenized = qTokenize (qString, qSep);

        auto it = tokenized.begin ();
        auto jt = qTokenized.begin ();

        for (; it != tokenized.end () && jt != qTokenized.end (); ++it, ++jt)
            {
            QCOMPARE (*it, *jt);
            }

        // Make sure we've hit the end of both at the same time
        // i.e. tokenized results are the same size
        //
        QCOMPARE (it, tokenized.end ());
        QCOMPARE (jt, qTokenized.end ());
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for extractTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void extractTest_data ()
        {
        QTest::addColumn<std::string> ("string");
        QTest::addColumn<std::string> ("start");
        QTest::addColumn<std::string> ("end");
        QTest::addColumn<std::string> ("expected");

        QTest::newRow ("Basic")     << "=<To Extract>="s << "=<"s << ">="s << "To Extract"s;
        QTest::newRow ("No Start")  << "To Extract>="s   << "=<"s << ">="s << ""s;
        QTest::newRow ("No End")    << "=<To Extract"s   << "=<"s << ">="s << ""s;
        QTest::newRow ("Wrapped")   << "abc=<To Extract>=def"s
                                    << "=<"s << ">="s << "To Extract"s;

        QTest::newRow ("No Start")  << "--(Extract this)----(Not this)--"s
                                    << "--("s << ")--"s << "Extract this"s;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests the string extract() function
    ///
    /// @see    utils::str::extract()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void extractTest ()
        {
        QFETCH (std::string, string);
        QFETCH (std::string, start);
        QFETCH (std::string, end);
        QFETCH (std::string, expected);

        QCOMPARE (extract (string, start, end), expected);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for emptyRegexTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void emptyRegexTest_data ()
        {
        QTest::addColumn<QString> ("empty");

        QTest::newRow ("Empty") << "";
        QTest::newRow ("Space") << " ";
        QTest::newRow ("Multi") << "               ";
        QTest::newRow ("Tab")   << "\t";
        QTest::newRow ("Mixed") << "\t  \t";
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test NON_EMPTY_REGEX with empty strings
    ///
    /// @see    utils::str::NON_EMPTY_REGEX
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void emptyRegexTest ()
        {
        static const QRegularExpression regex{ NON_EMPTY_REGEX };

        QFETCH (QString, empty);

        QVERIFY (not regex.match (empty).hasMatch ());
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for nonEmptyRegexTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void nonEmptyRegexTest_data ()
        {
        QTest::addColumn<QString> ("nonempty");

        QTest::newRow ("Leading")   << " Leading Space";
        QTest::newRow ("Trailing")  << "Trailing Space ";
        QTest::newRow ("Tabs")      << "\tTabs\t";
        QTest::newRow ("One char")  << "    u         ";
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test NON_EMPTY_REGEX with non-empty strings
    ///
    /// @see    utils::str::NON_EMPTY_REGEX
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void nonEmptyRegexTest ()
        {
        static const QRegularExpression regex{ NON_EMPTY_REGEX };

        QFETCH (QString, nonempty);

        QVERIFY (regex.match (nonempty).hasMatch ());
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for splitTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void splitTest_data ()
        {
        QTest::addColumn<std::string_view> ("left");
        QTest::addColumn<std::string_view> ("right");
        QTest::addColumn<std::string_view> ("sep");

        static const std::string_view SEPARATORS[] =
            {
            ","sv,
            " "sv,
            ":"sv,
            "+"sv
            "___"sv,
            "->"sv,
            "=="sv,
            "string"sv
            };

        static const std::string_view STRINGS[] =
            {
            "Lorum Ipsum"sv,
            "Test Text"sv,
            "The number π = 3.14159"sv
            };

        int ii = 0;

        for (std::string_view sep : SEPARATORS)
            {
            for (std::string_view left : STRINGS)
                {
                for (std::string_view right : STRINGS)
                    {
                    QTest::addRow ("Row %d", ii) << left << right << sep;

                    ++ii;
                    }
                }
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test for the string split() function
    ///
    /// @see    utils::str::split()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void splitTest ()
        {
        QFETCH (std::string_view, left);
        QFETCH (std::string_view, right);
        QFETCH (std::string_view, sep);

        std::string toSplit = std::string{ left } + std::string{ sep } + std::string{ right };

        auto [actualLeft, actualRight] = split (toSplit, sep);

        QCOMPARE (actualLeft,  left);
        QCOMPARE (actualRight, right);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the corner cases of the string split() function
    ///
    /// @see    utils::str::split()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void splitCornerCases ()
        {

        // Test with no separator
        {
        static const std::string NO_SEPARATOR = "This is missing the separator";

        auto [left, right] = split (NO_SEPARATOR, ":");

        QCOMPARE (left, NO_SEPARATOR);
        QCOMPARE (right, "");
        }

        // Test with only separator
        {
        static const std::string ONLY_SEPARATOR = "This whole string is the separator";


        auto [left, right] = split (ONLY_SEPARATOR, ONLY_SEPARATOR);

        QCOMPARE (left, "");
        QCOMPARE (right, "");
        }

        // Test with multiple instances of the separator
        {
        static const std::string MULTI_SEPARATOR = "======";

        auto [left, right] = split (MULTI_SEPARATOR, "===");

        QCOMPARE (left, "");
        QCOMPARE (right, "===");
        }

        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the formatOnOff() function
    ///
    /// @see    utils::str::formatOnOff()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void onOffTest ()
        {
        QCOMPARE (formatOnOff (true),  "On");
        QCOMPARE (formatOnOff (false), "Off");
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for escapeTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void escapeTest_data ()
        {
        QTest::addColumn<QString> ("unescaped");
        QTest::addColumn<QString> ("escaped");

        QTest::newRow ("Start") << "& Ampersand at start"   << "&& Ampersand at start";
        QTest::newRow ("End")   << "Ampersand at end&"      << "Ampersand at start&&";
        QTest::newRow ("Middle")<< "Ampersand &in middle"   << "Ampersand &&in middle";
        QTest::newRow ("Three") << "&All&Three&"            << "&&All&&Three&&";
        QTest::newRow ("Double")<< "Double&&Ampersand"      << "Double&&&&Ampersand";
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the escape() function
    ///
    /// @see     utils::str::escape()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void escapeTest ()
        {
        QFETCH (QString, unescaped);
        QFETCH (QString, escaped);

        QCOMPARE (escape (unescaped), escaped);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for removeDiacriticsTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void removeDiacriticsTest_data ()
        {
        QTest::addColumn<QString> ("accented");

        for (wchar_t accent = L'\x0300'; accent <= L'\x036F'; ++accent)
            {
            QString accentedString;

            accentedString.reserve (UNACCENTED_STRING.size () * 2);

            for (const QChar unaccented : UNACCENTED_STRING)
                {
                accentedString.append (unaccented);
                accentedString.append (accent);
                }

            accentedString = accentedString.normalized (QString::NormalizationForm_C);

            QTest::addRow ("Row %x", static_cast<int> (accent)) << accentedString;
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test removal of diacritics from a string using removeDiacritics()
    ///
    /// @see    utils::str::removeDiacritics()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void removeDiacriticsTest ()
        {
        QFETCH (QString, accented);

        QCOMPARE (UNACCENTED_STRING, removeDiacritics (accented));
        }


    ///////////////////////////////////////////////////////////////////////////////
    /// Test the quote function
    ///
    /// @see    utils::str::quote()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void quoteTest ()
        {
        QCOMPARE (utils::str::quote ("Quote This"),
                  "\"Quote This\"");
        }
    };


QTEST_GUILESS_MAIN (StringTest);

#include "stringtest.moc"

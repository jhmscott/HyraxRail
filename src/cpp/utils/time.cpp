/**
 * @file        utils/time.cpp
 * @brief       Time utilities
 * @author      Justin Scott
 * @date        2026-06-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <utils/algorithm.hpp>
#include <utils/time.hpp>

namespace utils::time
{
QString formatDaysOfTheWeek (const days& dayset)
    {
    using namespace internal;

    days weekdays;
    days weekends;
    days oddDays;
    days evenDays;


    algorithm::makeBitset (weekdays,
                           MONDAY,
                           TUESDAY,
                           WEDNESDAY,
                           THURSDAY,
                           FRIDAY);

    algorithm::makeBitset (weekends,
                           SATURDAY,
                           SUNDAY);

    algorithm::makeBitset (oddDays,
                           MONDAY,
                           WEDNESDAY,
                           FRIDAY,
                           SUNDAY);

    algorithm::makeBitset (evenDays,
                           TUESDAY,
                           THURSDAY,
                           SATURDAY);

    QString text;

    if (weekdays == dayset)
        {
        text = TimeStringConstants::tr ("Weekdays");
        }
    else if (weekends == dayset)
        {
        text = TimeStringConstants::tr ("Weekends");
        }
    else if (oddDays == dayset || evenDays == dayset)
        {
        text = TimeStringConstants::tr ("Every other day");
        }
    else
        {
        auto set = algorithm::bitsetToSet (dayset);

        if (0 == set.size ())
            {
            text = TimeStringConstants::tr ("Never");
            }
        else if (1 == set.size ())
            {
            text = dayOfWeekText (static_cast<dayOfTheWeek> (*set.begin ()));
            }
        else if (7 == set.size ())
            {
            text = TimeStringConstants::tr ("Everyday");
            }
        else
            {
            for (size_t ii : set)
                {
                auto day = static_cast<dayOfTheWeek> (ii);

                text += dayOfWeekText (day) + ", ";
                }

            text.chop (2);
            }
        }

    return text;
    }


std::pair<int, unit> convertToBestUnit (const std::chrono::nanoseconds& duration,
                                        const units&                    units)
    {
    using count_t = std::chrono::nanoseconds::rep;

    const count_t   count = duration.count ();
    unit            current;
    unit            last = NUM_UNITS;

    for (size_t ii : algorithm::bitsetToSet (units))
        {
        current = static_cast<unit> (ii);

        count_t conversion  = unitConversion (current).count ();
        count_t tmpCount    = count / conversion;

        if (0 == tmpCount)
            {
            if (NUM_UNITS != last)
                {
                current = last;
                }
            break;
            }
        else if (tmpCount < 1000)
            {
            if (count % conversion < (conversion / 1000))
                {
                break;
                }
            else if (tmpCount >= 10)
                {
                break;
                }
            }

        last = current;
        }

    return std::make_pair (count / unitConversion (current).count (),
                           current);
    }

} // namespace utils::time

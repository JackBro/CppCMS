//
//  Copyright (c) 2009-2010 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_LOCALE_DATE_TIME_FACET_HPP_INCLUDED
#define BOOST_LOCALE_DATE_TIME_FACET_HPP_INCLUDED

#include <boost/locale/config.hpp>
#include <boost/cstdint.hpp>
#include <locale>

namespace boost {
    namespace locale {
        ///
        /// \brief Namespace that contains a enum that defines various periods like years, days
        ///
        namespace period {
            ///
            ///
            /// This enum provides the list of various time periods that can be used for manipulation over date and time
            /// Operators like +, - * defined for these period allowing to perform easy calculations over them
            ///
            typedef enum {
                invalid,                    ///< Special invalid value, should not be used directs
                era,                        ///< Era i.e. AC, BC in Gregorian and Julian calendar, range [0,1]
                year,                       ///< Year, it is calendar specific
                extended_year,              ///< Extended year for Gregorian/Julian calendars, where 1 BC == 0, 2 BC == -1.
                month,                      ///< The month of year, calendar specific, in Gregorian [0..11]
                day,                        ///< The day of month, calendar specific, in Gregorian [1..31]
                day_of_year,                ///< The number of day in year, starting from 1
                day_of_week,                ///< Day of week, starting from Sunday, [1..7]
                day_of_week_in_month,       ///< Original number of the day of the week in month.
                day_of_week_local,          ///< Local day of week, for example in France Monday is 1, in US Sunday is 1, [1..7]
                hour,                       ///< 24 clock hour [0..23]
                hour_12,                    ///< 12 clock hour [0..11]
                am_pm,                      ///< am or pm marker, [0..1]
                minute,                     ///< minute [0..59]
                second,                     ///< second [0..59]
                week_of_year,               ///< The week number in the year
                week_of_month,              ///< The week number withing current month
                first_day_of_week,          ///< For example Sunday in US, Monday in France
            } period_type;

        } // namespace period

        ///
        /// Structure that define POSIX time, seconds and milliseconds
        /// since Jan 1, 1970, 00:00 not including leap seconds.
        ///
        struct posix_time {
            int64_t seconds; ///< Seconds since epoch
            uint32_t nanoseconds;  ///< Nanoseconds resolution
        };

        ///
        /// This class defines generic calendar class, it is used by date_time and calendar
        /// objects internally. It is less useful for end users, but it is build for localization
        /// backend implementation
        ///

        class abstract_calendar {
        public:

            ///
            /// Type that defines how to fetch the value
            ///
            typedef enum {
                absolute_minimum,   ///< Absolute possible minimum for the value, for example for day is 1
                actual_minimum,     ///< Actual minimal value for this period.
                greatest_minimum,   ///< Maximal minimum value that can be for this period
                current,            ///< Current value of this period
                least_maximum,      ///< The last maximal value for this period, For example for Gregorian calendar
                                    ///< day it is 28
                actual_maximum,     ///< Actual maximum, for it can be 28, 29, 30, 31 for day according to current month
                absolute_maximum,   ///< Maximal value, for Gregorian day it would be 31.
            } value_type;

            ///
            /// A way to update the value
            ///
            typedef enum {
                move,   ///< Change the value up or down effecting others for example 1990-12-31 + 1 day = 1991-01-01
                roll,   ///< Change the value up or down not effecting others for example 1990-12-31 + 1 day = 1990-12-01
            } update_type;

            ///
            /// Information about calendar
            ///
            typedef enum {
                is_gregorian,  ///< Check if the calendar is Gregorian
            } calendar_option_type;

            ///
            /// Make a polymorphic copy of the calendar
            ///
            virtual abstract_calendar *clone() const = 0;

            ///
            /// Set specific \a value for period \a p, note not all values are settable.
            ///
            virtual void set_value(period::period_type p,int value) = 0;

            ///
            /// Get specific value for period \a p according to a value_type \a v
            ///
            virtual int get_value(period::period_type p,value_type v) const = 0;

            ///
            /// Set current time point
            ///
            virtual void set_time(posix_time const &p)  = 0;
            ///
            /// Get current time point
            ///
            virtual posix_time get_time() const  = 0;

            ///
            /// Set option for calendar, for future use
            ///
            virtual void set_option(calendar_option_type opt,int v) = 0;
            ///
            /// Get option for calendar, currently only check if it is Gregorian calendar
            ///
            virtual int get_option(calendar_option_type opt) const = 0;

            ///
            /// Adjust period's \a p value by \a difference items using a update_type \a u.
            /// Note: not all values are adjustable
            ///
            virtual void adjust_value(period::period_type p,update_type u,int difference) = 0;

            ///
            /// Calculate the difference between this calendar  and \a other in \a p units
            ///
            virtual int difference(abstract_calendar const *other,period::period_type p) const = 0;

            ///
            /// Set time zone, empty - use system
            ///
            virtual void set_timezone(std::string const &tz) = 0;
            ///
            /// Get current time zone, empty - system one
            ///
            virtual std::string get_timezone() const = 0;

            ///
            /// Check of two calendars have same rules
            ///
            virtual bool same(abstract_calendar const *other) const = 0;

            virtual ~abstract_calendar()
            {
            }

        };

        ///
        /// \brief the facet that generates calendar for specific locale
        ///
        class BOOST_LOCALE_DECL calendar_facet : public std::locale::facet {
        public:
            ///
            /// Basic constructor
            ///
            calendar_facet(size_t refs = 0) : std::locale::facet(refs) 
            {
            }
            ///
            /// Create a new calendar that points to current point of time.
            ///
            virtual abstract_calendar *create_calendar() const = 0;

            ///
            /// Locale id (for work with std::locale
            ///
            static std::locale::id id;
        };

    } // locale
} // boost

#endif
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4


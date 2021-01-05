#include "calendar.h"

bool Date::operator==(const Date &date)
{
    return this->year == date.year && this->month == date.month;
}

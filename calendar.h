#ifndef CALENDAR_H
#define CALENDAR_H

enum Month
{
    JAN = 1, FEB, MAR, APR, MAY, JUN, JUL, AUG, SEP, OCT, NOV, DEC
};

// This doesn't doesn't record a day, since the simulation currently only
// passes time month-by-month.
class Date
{
  public:
    int year;
    Month month;
    
    bool operator==(const Date& date);
};

#endif

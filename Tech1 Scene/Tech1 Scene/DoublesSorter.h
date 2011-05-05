/*
Copyright (C) 2011 Tech1 Game Engine.
*/

// Сортирует Double От меньшего к большему
// Если добавляемое значение уже существует то оно не добавляется

#ifndef _DOUBLES_SORTER_
#define _DOUBLES_SORTER_

struct DoublesOneUnit
{
DoublesOneUnit * Next;
double value;
};

class DoublesSorter
{
public:
DoublesOneUnit * Memory;
int counter;
inline int GetCounter() { return counter; }
void AddValue(double Value);
void PrintAllUnits();
double PrintUnit(int UnitNo);
DoublesSorter();
~DoublesSorter();

};

#endif
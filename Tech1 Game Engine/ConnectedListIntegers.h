/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#ifndef _CONNECTED_LIST_INTEGERS_
#define _CONNECTED_LIST_INTEGERS_

struct ListValues
{
int IntegerValue;
ListValues * NextValue;
};

class ConnectedListIntegers
{
public:
ListValues * List;
int NumElements;
ConnectedListIntegers();
~ConnectedListIntegers();
void AddValue(int ValueToAdd);
void PrintAllElements();
int FindValue(int ValueToFind);
};

#endif
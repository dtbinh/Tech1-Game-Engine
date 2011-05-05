#ifndef _CONNECTED_LIST_STRINGS_
#define _CONNECTED_LIST_STRINGS_

#ifndef _NULL
#define NULL 0
#endif
struct OneUnitInConnectedListStrings
{
char Unit[255];
OneUnitInConnectedListStrings * Next;
};

class ConnectedListStrings
{
public:
ConnectedListStrings();
~ConnectedListStrings();
void AddUnit(char * String);
bool CheckIsItemInList(char * String);
void PrintAllUnits();
OneUnitInConnectedListStrings * ListOfStrings;
int NumUnits;
};


#endif
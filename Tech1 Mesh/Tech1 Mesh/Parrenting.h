/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#ifndef _PARENTING_
#define _PARENTING_
struct Childs
{
char ParrentName[255];
Childs * NextChild;
};
struct OneParrentInfoEx
{
unsigned int NumChilds;
char MeshName[255];
Childs * ChildData;
Childs * LastChildAdded;
OneParrentInfoEx * NextParrentInfo;
};



class Parenting
{public:
	unsigned int Num;
	OneParrentInfoEx * ParrentInfo;
	OneParrentInfoEx * LastElementAdded;
	Parenting();
	~Parenting();
	void AddElement(char * Name);
	void AddChild(char * ElementName, char * ChildName);
	void PrintAllElements();
	unsigned int GetNumChilds(char * ElementName);
	unsigned int GetNumElements();
	char * GetElementNum(unsigned int ElementNum);
	char * GetChildNum(char * ElementName, unsigned int ChildNum);
	void Parenting::DeleteAllElements();
};
#endif;
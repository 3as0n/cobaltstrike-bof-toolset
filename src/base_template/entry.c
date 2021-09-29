#include "bofdefs.h"
#include "base.c"

#ifdef BOF
VOID go(IN PCHAR Buffer, IN ULONG Length) 
{
	//input param
	if(!bofstart())
	{
		return;
	}
	//CALLYOURFUNCHERE
	printoutput(TRUE);
};

#else

int main(int argc, char ** argv)
{
	//code for standalone exe for scanbuild / leak checks
	return 0;
}

#endif

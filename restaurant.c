/*  Implementation of Computerized Restaurant Management System - in linked lists.

	By Dorin Beery
	
	First year C project  */

#include "header.h"

int main()
{
	FILE* fpIns=NULL, * fpOut=NULL, * fpDishes=NULL;
	Menu menu;
	PTable tbls = NULL;
	int command = 0, numTbls = 0;

	/*Open files*/
	fpOut = fopen("output.txt", "w");
	if (fpOut == NULL)
		ErrMsg(stdout, "ERROR: opening output file.");

	fpIns = fopen("instructions.txt", "r");
	if (fpIns == NULL)
		ErrMsg(fpOut, "\nERROR: opening Instructions file.");

	fpDishes = fopen("dishes.txt", "r");
	if (fpDishes == NULL)
		ErrMsg(fpOut, "\nERR: opening dishes file. ");



	fscanf(fpIns, "%d", &numTbls); //Scan number of tables
	if (numTbls <= 0)
		ErrMsg(fpOut, "Number of tables must be positive. exits. ");


	fscanf(fpIns, "%d", &command); //Scan first command
	if (command != 1)
		ErrMsg(fpOut, "First command must be 1. exists.");
	else
	{
			InitialMenu(&menu); //nullify struct Menu, and array of struct Table
			ReadManot(&menu, fpDishes, fpOut);        //make Menu.

			tbls = (PTable)malloc(sizeof(Table) * numTbls); //Create array of struct Table
			if (tbls == NULL)
				ErrMsg(fpOut, "ERR: Memory allocation failed. exits. ");
			else
			{
				InitialTbls(tbls, numTbls);
				FollowInst(&menu, tbls, fpIns, fpOut, numTbls);//continue according to Instructions.txt
			}
	}
	


	/*Free memory allocations*/
	FreeOrders(tbls, numTbls, fpOut);
	FreeMenu(&menu);

	fclose(fpIns);
	fclose(fpDishes);
	fclose(fpOut);

	return 0;
}

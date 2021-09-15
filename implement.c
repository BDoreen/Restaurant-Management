/*This file contains implementations of functions that are used in the Program*/


#include "header.h"


/*function nullifys all fields of struct Menu*/
void InitialMenu(PMenu menu)
{
	menu->head = NULL;
	menu->tail = NULL;
	menu->size = 0;
}


/*Function scans file dishes.txt and creates
  linked list of structs Dish, to struct Menu.*/
void ReadManot(Menu* m, FILE* fDishes, FILE* out)
{
	Dish* toAdd = NULL;
	char dish[51];
	int isEOF, p, q;

	isEOF = fscanf(fDishes, "%s%d%d", dish, &q, &p); //name, quantity, price.

	/*Loop through dishes.txt*/
	while (isEOF != -1)
	{
		//check price and quantity are positive.
		if (p <= 0 || q <= 0)
		{
			fprintf(out, "Price (%d) and Quantity (%d) should be positive, therefor instruction is skipped.\n", p, q);
			continue;
		}

		//Check if dish is already in Menu or not.
		toAdd = CDishInMenu(*m, dish);
		if (toAdd != NULL) //Dish is already in Menu
			toAdd->quantity += q; //Add quantity to existing Dish.
		else                     //Dish is not already in Menu
		{
			toAdd = CreateDish(m, out, dish, q, p); //Create Dish

			//Link Dish to Menu
			if (m->size == 0) //is first dish? head points to it.
				m->head = toAdd;
			else
				m->tail->next = toAdd;
			m->tail = toAdd; //tail points to last dish
			m->size++;
		}
		isEOF = fscanf(fDishes, "%s%d%d", dish, &q, &p);	//receive the next course
	}
	fprintf(out, "The kitchen was created.\n");
}


/*Function returns pointer to dish in Menu.
  Returns NULL if dish is not in Menu.*/
PDish CDishInMenu(Menu m, char* name)
{
	Menu tMenu = m;
	PDish dish;

	while (tMenu.head != NULL)
	{
		if (strcmp(tMenu.head->productName, name) == 0)
		{
			dish = tMenu.head;
			return dish; //return pointer to the dish.
		}
		tMenu.head = tMenu.head->next;
	}
	return NULL; //there is no such course.
}


/*function allocates memory for node of a dish, and inserts values to fields.
returns pointer to allocated address. */
Dish* CreateDish(Menu* m, FILE* out, char* name, int q, int p)
{
	Dish* toAdd = NULL;

	toAdd = (Dish*)malloc(sizeof(Dish));
	if (toAdd == NULL)
	{
		FreeMenu(m); //Free accumulated malloced Dishes  
		ErrMsg(out, "\nERROR: Memory allocation failed. Prog exits. ");
	}
	else
	{
		toAdd->productName = (char*)malloc(sizeof(char) * (strlen(name) + 1));
		if (toAdd->productName == NULL)
		{
			free(toAdd);
			FreeMenu(m);
			ErrMsg(out, "\nERROR: Memory allocation failed. Prog exits.");
		}
		else
		{
			strcpy(toAdd->productName, name);	//copy name
			toAdd->price = p;					//copy price
			toAdd->quantity = q;				//copy quantity		
			toAdd->numOfOrders = 0;				//nullify competition
			toAdd->next = NULL;					//nullify field next.
			return toAdd;						//returns address to allocated memory.
		}
	}
	return NULL; 
}


/*Function frees malloc of Menu (Dishes)*/
void FreeMenu(Menu* m)
{
	Dish* tempDish;

	tempDish = m->head;
	while (m->head != NULL) //Loop through Menu
	{
		m->head = m->head->next; //Save pointer to next node
		if (m->head == NULL) //if its the end of the list, than make tail to also points to NULL
			m->tail = NULL;
		free(tempDish->productName); //free malloc of productName
		free(tempDish);				//free node
		tempDish = m->head;		//temp is next node.
		m->size--;
	}
}


/*function nullifys all fields of struct Table*/
void InitialTbls(PTable tbls, int numTbls)
{
	int i = numTbls - 1;

	while (i >= 0)
	{
		tbls[i].bill = 0;
		tbls[i].head = NULL;
		i--;
	}
}


/*Function scans a command from FILE Instruction.txt
  and executes its' matching case. */
void FollowInst(PMenu m, PTable tbls, FILE* fpIn, FILE* out,  int numTbls)
{
	int command, quantity, iTable, openTbls = 0;
	char name[51], checkLine = '0';

	while (fscanf(fpIn, "%d", &command) != -1) //Loop through Instruction.txt
	{
		switch (command)
		{
		case 2: //Add quantity to an existing Dish.
			fscanf(fpIn, "%s %d", name, &quantity);
			AddItems(m, tbls, name, quantity, out);
			break;
		case 3: //Order a Dish.
			fscanf(fpIn, "%d%s%d", &iTable, name, &quantity);
			OrderItem( m, tbls, out, numTbls, iTable, quantity, name, &openTbls);
			break;
		case 4: //Cancel Dish/es
			fscanf(fpIn, "%d%s%d, &iTable", &iTable, name, &quantity);
			RemoveItem(m, tbls, iTable, name, quantity, out, numTbls);
			break;
		case 5: //Close Table.
			fscanf(fpIn, "%d", &iTable);
			RemoveTable(m, tbls, iTable,  out,  numTbls, &openTbls);
			break;

		default:
			fprintf(out, "\nCommand %d doesn't exist. Moving to the next line.", command);
			while (checkLine != 10) //continue until end of line.
				fscanf(fpIn, "%c", &checkLine);
		}
	}
}


/*Function adds quantity to a Dish in Menu, 
  only if the Dish exists in the Menu. */
void AddItems(Menu* m, PTable tbls, char* ProductName, int q, FILE* out)
{
	PDish toAdd;

	toAdd = CDishInMenu(*m, ProductName); //Check for Dish in Menu

	if (toAdd == NULL) //Dish isn't in Menu
	{
		//Dish Hamburger was ordered by table number 6, doesn't exist in restaurant.
		fprintf(out, "Dish %s doesn't exist in restaurant, therefore, cannot add quantity from it.\n", ProductName);
		return;
	}

	if (q < 0)
	{
		fprintf(out, "Quantity (%d) must be positive. Item %s wasn't added.\n", q, ProductName);
		return;
	}

	toAdd->quantity += q; //add quantity to Dish.
	fprintf(out, "%d plates of %s were added to the kitchen. \n", q, ProductName);
}


/*Function adds an order to table i*/
void OrderItem(Menu* m, PTable tbl, FILE* out, int iTbls, int i, int q, char* name, int* openTbls)
{
	POrders toAdd, is_Order;
	PDish dish;

	/*Check that dish to be ordered exists in restaurants' Menu*/
	dish = CDishInMenu(*m, name); 
	if (dish == NULL)//not exists
	{
		fprintf(out, "Dish %s was ordered by table number %d but doesn't exist in restaurant.\n", name, i);
		return;
	}
	else /*check quantity and index of the ordering table*/
		if (CTermsOfOrder(m, name, dish, q, i, out, iTbls) == 0)
			return;

	is_Order = CDishInTable((&tbl[i - 1]), name); //check if Dish was already ordered by the table
	if (is_Order != NULL)//was ordered
		is_Order->quantity += q;
	else//wasn't ordered
	{
		/*Create order and link to list of tables' orders. */
		toAdd = CreateOrder(m, tbl, out, name, i);

		if (tbl[i - 1].head == NULL) //table didn't order any dish
			tbl[i - 1].head = toAdd;
		else
		{
			tbl[i - 1].head->prev = toAdd;
			toAdd->next = tbl[i - 1].head;
			tbl[i - 1].head = toAdd;
		}
		strcpy(tbl[i - 1].head->OrderName, name); //copy name.
		toAdd->quantity = q;					//update the quantity
		toAdd->price = dish->price;				//update the price
		(*openTbls)++; //add tbl to openTbls

	}
	tbl[i - 1].bill += (dish->price * q);	//update bill of table[i]
	dish->quantity -= q; //update number of courses left from dish in kitchen
	dish->numOfOrders += q; //update number of courses were ordered (for competition).
	fprintf(out, "%d plates of %s were added to table number %d. \n", q, name, i);
}


/* Function returns pointer to Order of Table which already ordered dish
   named "name". If Table didn't order "name", returns NULL */
POrders CDishInTable(PTable tbl, char* name)
{
	PTable tempTbl = tbl;

	while (tempTbl->head != NULL)
	{
		if (strcmp(tempTbl->head->OrderName, name) == 0) //If dish was already ordered by the table
			return tempTbl->head;						 //return pointer to order
		tempTbl->head = tempTbl->head->next;
	}
	return NULL;
}


/*Function checks if there is enough quantity of the Dish in the Menu(kitchen)*/
int CTermsOfOrder(PMenu m, char* name, PDish toCheck, int qToCheck, int requestedTbl, FILE* out, int numOfTbls)
{
	if (qToCheck <= 0)
	{
		fprintf(out, "Quantity (%d) must be a positive number, order %s cannot be added to table %d.\n" ,qToCheck, name, requestedTbl);
		return 0;
	}

	if (qToCheck > toCheck->quantity)//checks if quantity of course is enough for the order.
	{
		fprintf(out, "There is not enough quantity to provide from %s, sorry! Dish was not added to tables' (%d) bill. \n", name, requestedTbl);
		return 0; //end if the quantity is not enough.
	}

	if (requestedTbl > numOfTbls)//checks if requestedTbl is in range of tables inside dining room.
	{
		fprintf(out, "There are no %d tables in the restaurant. Order can not continue for supposedly table %d. \n", requestedTbl, requestedTbl);
		return 0;
	}
	return 1;
}


/*function allocates memory for struct Orders.
  returns address of the created memory.*/
Orders* CreateOrder(Menu* m, PTable tbls, FILE* out, char* name, int i)
{
	Orders* PtoAdd;

	PtoAdd = (POrders)malloc(sizeof(Orders));
	if (PtoAdd == NULL)
	{
		FreeMenu(m);
		free(tbls);
		ErrMsg(out, "\nERROR: Memory allocation failed.");
	}
	else
	{
		InitialOrder(PtoAdd); //nullify all fields of struct Orders
		PtoAdd->OrderName = (char*)malloc(sizeof(char) * (strlen(name) + 1));
		if (PtoAdd->OrderName == NULL)
		{
			FreeMenu(m);
			free(tbls);
			ErrMsg(out, "\nERROR: Memory allocation failed.");
		}
	}
	return PtoAdd;
}


/*Initialized malloced struct Orders fields*/
void InitialOrder(POrders order)
{
	order->prev = NULL;
	order->next = NULL;
	order->OrderName = NULL;
	order->price = 0;
	order->quantity = 0;
}


/* Function checks requirements to delete an Order from list of Orders.
   If it was the last order for Table i, Table[i-1]->head is nullified. */
void RemoveItem(Menu* m, PTable tbl, int i, char* ProductName, int qToDel, FILE* out, int numTbls)
{
	POrders delOrder, temp = NULL;
	PDish toCheck = NULL;

	/*Check if table i exists in dining room*/
	if (i > numTbls)
	{
		fprintf(out, "Table %d doesn't exist, cannot remove item from it.\n", i);
		return;
	}

	/*Check if Dish is in Menu.*/
	toCheck = CDishInMenu(*m, ProductName);
	if (toCheck == NULL)
	{
		fprintf(out, "The dish %s doesn't exist in the kitchen, so it cannot be removed from table %d.\n", ProductName, i);
		return;
	}

	/*Find Dish in list of orders of Table[i-1]*/
	delOrder = CDishInTable(&tbl[i-1], ProductName);
	if (delOrder == NULL)
	{
		fprintf(out, "%s was not ordered by table %d, so cannot remove it from bill.\n", ProductName, i + 1);
		return;
	}

	if (qToDel > delOrder->quantity)
	{
		fprintf(out, "Quantity (%d) to be removed higher than Tables initial request (%d).\n", qToDel, delOrder->quantity);
		return;
	}

	tbl[i-1].bill -= (delOrder->price * qToDel); //remove the order from the bill
	delOrder->quantity -= qToDel;				//remove the quantity from the table.

	/*Need to free memory only if delOrder->quantity == 0*/
	if (qToDel == 0)
	{
		temp = FreeOrder(delOrder);
		if(temp != NULL) //not end of list
			fprintf(out, "%d %s was returned to the kitchen from table number %d. \n", qToDel, ProductName, i);
		else //end of Orders' list for Table[i]
		{
			tbl[i - 1].head = NULL;
			fprintf(out, "No more orders for table %d. \n", i);
		}
	}
}


/* Function checks requirements for removing table.
   if all requirements are met, frees memory. also, prints most sold dishes if it recognizes last table being freed
   ALG: Release linked list from head to tail.*/
void RemoveTable(PMenu menu, PTable table, int i,  FILE* out,  int numOfTables, int* openTbls)
{
	POrders toDel;

	if (i > numOfTables) //check if there are i tables in restaurant
	{
		fprintf(out, "Table %d doesn't exist. \n", i);
		return;
	}

	if (table[i-1].head == NULL) //check if there are any orders for the table
	{
		fprintf(out, "Table %d hasn't ordered any course. Instruction to remove table cannot execute. \n", i + 1);
		return;
	}
	//else
	fprintf(out, "The bill for table number %d is:\n", i);
	while (table[i-1].head != NULL) //while there any orders
	{
		toDel = table[i-1].head;
		fprintf(out, "%d %s %d nis, \n", toDel->quantity, toDel->OrderName, toDel->price*toDel->quantity);
		toDel->quantity = 0;
		table[i-1].head = FreeOrder(toDel);
	}

	fprintf(out, "Sum: %d nis, please!\n", table[i - 1].bill);


	//assume bill was paid
	table[i - 1].bill = 0;

	(*openTbls)--; //remove table from openTbls

	if ((*openTbls) == 0) //if it was the last table
	{
		fprintf(out, "The most popular dish or dishes are:\n");
		mostSells(*menu, out);//function prints the most sold dishes for the day.
	}
}


/* Functions scans through array of struct Tables, to find open Orders of unpaid bills. 
   Is used for closing restaurant.*/
void FreeOrders(PTable table, int numTbls, FILE* out)
{
	POrders tempOrder = NULL;
	PTable tempTable = NULL;
	int bill = 0;

	numTbls--; //counts from 0.
	while (numTbls >= 0)
	{
		tempTable = &table[numTbls];
		if (tempTable->head != NULL) //If table wasn't closed.
		{
			tempOrder = table[numTbls].head; //first order
			while (tempOrder != NULL)		 //Loop through all orders
			{
				bill += table[numTbls].bill; //Sum the bill
				table[numTbls].head = tempOrder->next; //save pointer to next order.

				if (tempOrder->OrderName != NULL)
				{
					free(tempOrder->OrderName); //free malloc of Order.
					tempOrder = table[numTbls].head;
				}
			}
			fprintf(out, "Table %d has left without paying his bill (%d NIS). \n", numTbls + 1, bill);
			free(tempTable->head); //Close table
			tempTable = NULL;
		}
		numTbls--;
	}
	free(table);
}


/*Functions frees memory of malloced struct Order
  In case of freeing the list from head to tail, return pointer to head of list, and NULL if end of list. */
POrders FreeOrder(POrders toDel)
{
	POrders temp = NULL;

	free(toDel->OrderName);
	toDel->OrderName = NULL;
	toDel->price = 0;

	/*if middle order in list of orders*/
	if (toDel->next != NULL && toDel->prev != NULL)
	{
		toDel->prev->next = toDel->next;
		toDel->next->prev = toDel->prev;
		temp = toDel->prev;
	}
	else
	{
		/*If first order in list of orders*/
		if (toDel->prev == NULL && toDel->next != NULL)
		{
			toDel->next->prev = NULL;
			temp = toDel->next;
		}

		/*If last order in list of orders*/
		if (toDel->next == NULL && toDel->prev != NULL)
		{
			toDel->prev->next = NULL;
			temp = toDel->prev;
		}
	}

	free(toDel);
	toDel = NULL;

	return temp;
}


/*Function calculates the dish (or dishes) that were sold the most.*/
void mostSells(Menu m, FILE* out)
{
	int max = 0;
	PDish pmax = m.head;
	while (pmax != NULL)//loop to find maximum
	{
		if (pmax->numOfOrders > max)
		{
			max = pmax->numOfOrders;
		}
		pmax = pmax->next;
	}

	pmax = m.head; //initial pmax to point back to start of menu
	while (pmax != NULL) //loop prints the dish (or dishes) that had the highest sells.
	{
		if (pmax->numOfOrders == max)
			fprintf(out, "%s: %d plates.\n", pmax->productName, pmax->numOfOrders);
		pmax = pmax->next;
	}
}


void ErrMsg(FILE* out, char* msg)
{
	fprintf(out, "%s\n", msg);
	exit(-1);
}
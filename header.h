#ifndef header
#define header

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

typedef struct Dish
{
	char* productName;
	int quantity;
	int price;
	int numOfOrders;
	struct Dish* next;
}Dish, * PDish;

typedef struct Menu
{
	struct Dish* head;
	struct Dish* tail;
	int size;
}Menu, * PMenu;

typedef struct Orders
{
	char* OrderName;
	int quantity;
	int price;
	struct Orders* next;
	struct Orders* prev;
}Orders, * POrders;

typedef struct Table
{
	struct Orders* head;
	int bill;
}Table, * PTable;

void InitialMenu(PMenu menu);
void InitialOrder(POrders order);
void InitialTbls(PTable tbls, int numTbls);

Dish* CreateDish(Menu* m, FILE* out, char* name, int q, int p);
Orders* CreateOrder(Menu *m, PTable tbls, FILE* out, char* name, int i);

void AddItems(Menu* m, PTable tbls, char* ProductName, int q, FILE* out);
void OrderItem(Menu* m, PTable tbl, FILE* out, int iTbls, int i, int q, char* name, int* openTbls);

void ReadManot(Menu* m, FILE* fManot,  FILE* out );

void RemoveItem(Menu* m, PTable tbl, int i, char* ProductName, int q,  FILE* out, int iTbls);
void RemoveTable(PMenu menu, PTable table, int i,  FILE* out,  int numOfTables, int* openTbls);

void FreeOrders(PTable t, int i, FILE* out);
void FreeMenu(Menu* m);
POrders FreeOrder(POrders toDel);

PDish CDishInMenu(Menu m, char* name);
POrders CDishInTable(PTable tbl, char* name);
int CTermsOfOrder(PMenu m, char* name, PDish toCheck, int q, int i, FILE* out, int iTbls);

void mostSells(Menu m, FILE* out);
void FollowInst(PMenu m, PTable tbls, FILE* in, FILE* out,  int numTbls);
void ErrMsg(FILE* out, char* msg);

#endif // !header

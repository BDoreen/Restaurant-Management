# Restaurant-Management
Implementation of Computerized Restaurant Management System - in linked lists.  

# Details about Instructions.txt
Input is originated from Instructions.txt.  
The first line in the file specifies the maximum number of tables in the restaurant.
After that, each line begins with a number between 1-5 that indicates different instructions.  
1 - CreateProducts: Create linked list of available dishes in the Menu and their quantity (to do so, scans Manot.txt).  
2 - AddItem: Increase quantity of a dish in the Menu.  
3 - OrderItem: Order a dish to a specific table.  
4 - RemoveItem: Cancel a dish that was ordered by a specific table.  
5 - RemoveTable: Print the bill of a specific table to output.txt.  

# Details about Manot.txt
Second input is from Manot.txt. It consists of details about dishes available in the restaurant's kitchen.   

# Details about output.txt
All relevant output is directed to output.txt.  

# Data structures
Menu is a list of available Dishes in the restaurant.  
Table is a list of Orders that were ordered by the diners.  
Each memory allocation is being handled throughout the program.  
FreeMenu and FreeOrders are called at the end of main.c, to free the memory of Dishes and Orders that were not paid.  

*Number of dishes available in the kitchen is unlimited.  
*Number of occupied dining tables is not constant.  

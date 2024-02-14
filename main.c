#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROUTE_LIMIT 10			// Maximum number of trips per route
#define BUS13_LIMIT 13			// Maximum capacity of a 13-passenger vehicle
#define BUS16_LIMIT 16			// Maximum capacity of a 16-passenger vehicle
#define FLEET_LIMIT 20			// Maximum number of vehicles in the system
#define DATABASE_LIMIT 320		// Maximum possible number of passengers in the system (16 passengers * 20 buses)
#define MENU_EXIT_OPTION 4		// User key to quit the program in the main menu

typedef char string[100];

/* DATA STRUCTURES */
typedef struct Ticket
{
	int origNum;				// Ticket identification.		Example: 2 (Ticket #2)
	int inputDate;				// Date of entry.				Example: 03212020 (March 21, 2020)
	int inputTime;				// Time of entry.				Example: 1500H
	string passName;			// Name of passenger.			Example: Jean-Baptiste de La Salle
	int idNum;					// 7 to 8 digit ID number.		Example: 12345678
	int priority;				// Priority level from 1-6.		Example: 6 (Employees and Students with official business)
	int entryPoint;				// Point of entry. 				Example: 1 (Manila)
	int exitPoint;				// Point of exit. 				Example: 110 (Mamplasan Exit)
	int busNum;					// Bus assigned to passenger.	Example: AE101
} Ticket;

typedef struct Bus
{
	Ticket load[BUS16_LIMIT];	// Total capacity set at a maximum of 16 passengers
	int limitType;				// Determines the load limit, either 13 passengers or 16 passengers
	int busNum;					// Unique bus number.			Example: AE101
	int busTime;				// Bus departure time.			Example: 1530H
} Bus;

/* SPECIFIC INPUT VERIFICATION FUNCTIONS */
/* Determines if the year is a leap year */
int checkIfLeap(int inputYear)
{
	if ((inputYear % 4 == 0 && inputYear % 100 != 0) || inputYear % 400 == 0)
		return 1;
	else
		return 0;
}
/* Determines number of days in a month */
int getDaysInMonth(int inputDate)
{
	int inputMonth = inputDate / 1000000;
	int inputYear = inputDate % 10000;
	switch (inputMonth)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			return 31;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			return 30;
			break;
		case 2:
			if (checkIfLeap(inputYear))
				return 29;
			else
				return 28;
		default:
			return 0;
			break;
	}
}
/* Validate if the given inputDate has a valid day of a month */
int checkIfDay(int inputDate, int numDaysInMonth)
{
	if (inputDate > 0)
	{
		inputDate /= 10000;
		inputDate %= 100;
		if (inputDate <= numDaysInMonth)
			return 1;
	}

	return 0;
}
/* Validate if the given inputDate has a valid month of a year */
int checkIfMonth(int inputDate)
{
	inputDate /= 1000000;
	if (inputDate >= 1 && inputDate <= 12)
		return 1;
	else
		return 0;
}
/* Validate if the given inputTime is in 24H time format */
int checkIf24H(int inputTime)
{
	if (!(inputTime / 100 < 24 && inputTime % 100 < 60) || inputTime < 0)
		return 0;
	else
		return 1;
}
/* Verifies if the given ID number is at least 7 to 8 digits long */
int verifyIDNumber(int inputNum)
{
	int ctrID = 0;
	while (inputNum > 0)
	{
		inputNum /= 10;
		ctrID++;
	}

	if (ctrID >= 7 && ctrID <= 8) // checks that ID numbers are 7-8 digits only
		return 1;
	else
		return 0;
}
/* Verifies if the given drop-off code is valid. If valid, it returns the index + 1 of the drop-off code. */
int verifyDropOff(int inputKey, int inputTime, int entryPoint)
{
	int specialCase;
	if (inputTime > 1529 && inputTime < 1815)	// only allows drop-off code 104 for AE108 and AE109
		specialCase = 1;
	else
		specialCase = 0;

	switch (entryPoint)
	{
		case 1:						// Manila -> Laguna routes only
			switch (inputKey)
			{
				case 101:	
					return 1;
					break;
				case 102:
					return 2;
					break;
				case 103:
				case 112:
					return 3;
					break;
				case 111:
					return 5;
					break;
				case 104:
					if (specialCase)
						return 4;
					else
						return 0;
					break;
				default:
					return 0; // returns only if invalid
					break;
			}
		case 2:					// Laguna -> Manila routes only
			switch (inputKey)
			{
				case 201:
					return 6;
					break;
				case 221:
					return 7;
					break;
				case 211:
				case 222:
					return 8;
					break;
				case 212:
				case 223:
					return 9;
					break;
				case 213:
				case 224:
					return 10;
					break;

				default:
					return 0; // returns only if invalid
					break;
			}
		default:
			return 0;
			break;
	}
}

/* SYSTEM DISPLAY FUNCTIONS */
/* Accepts a number in HHMM format and prints it in 24-hour time format. */
void printIn24H(int inputTime)
{
	if (inputTime < 960 && inputTime > 99) // 0100-0959
		printf("0");
	else if (inputTime < 60 && inputTime > 9) // 0010-0059
		printf("00");
	else if (inputTime < 10) // 0000-0009
		printf("000");

	printf("%dH", inputTime);
}
/* Accepts a number in MMDDYYYY format and prints it in MM/DD/YYYY format. */
void printDate(int inputDate)
{
	int month = inputDate / 1000000;
	int day = (inputDate / 10000) % 100;
	int year = inputDate % 10000;

	if (month < 10) 
		printf("0");
	printf("%d/", month);
	if (day < 10)
		printf("0");
	printf("%d/%d", day, year);
}
/* Displays the 13-seater bus configuration */
void displayConfig13(struct Bus *fleet, int ctrBus)
{
	int x, y, ctrSeat = 0;

	printf("\n*---*---*---*\n");
	for (x = 0; x < 4; x++)
	{
		printf("| ");
		for (y = 0; y < 3; y++)
		{
			if (fleet[ctrBus].load[ctrSeat].exitPoint != 0)
				printf("O");
			else
				printf("X");
			ctrSeat++;
			if (y < 2)
				printf(" | ");
		}
		printf(" |");
		printf("\n*---*---*---*\n");
	}

	if (fleet[ctrBus].load[ctrSeat].exitPoint != 0)
		printf("| O | ");
	else
		printf("| X | ");

	printf("AE%d |\n", fleet[ctrBus].busNum);
	printf("|   | ");
	printIn24H(fleet[ctrBus].busTime);
	printf(" |");
	printf("\n*---*---*---*\n");
}
/* Displays the 16-seater bus configuration */
void displayConfig16(struct Bus *fleet, int ctrBus)
{
	int x, y, ctrSeat = 0;
	// displays the first three rows of the bus
	printf("\n*---*---*---*---*\n");
	for (x = 0; x < 3; x++)
	{
		printf("| ");
		for (y = 0; y < 4; y++)
		{
			if (fleet[ctrBus].load[ctrSeat].exitPoint != 0)
				printf("O");
			else
				printf("X");
			ctrSeat++;
			if (y < 3)
				printf(" | ");
		}
		printf(" |");
		printf("\n*---*---*---*---*\n");
	}
	// displays the fourth row of the bus
	printf("|   ");
	for (x = 0; x < 3; x++)
	{
		if (fleet[ctrBus].load[ctrSeat].exitPoint != 0)
			printf("| O ");
		else
			printf("| X ");
	}
	// displays the fifth row of the bus
	printf("|\n");
	printf("|   *---*---*---*\n");
	if (fleet[ctrBus].load[ctrSeat].exitPoint != 0)
		printf("| O ");
	else
		printf("| X ");

	printf("|   AE%d   |\n", fleet[ctrBus].busNum);
	printf("|   |   ");
	printIn24H(fleet[ctrBus].busTime);
	printf("   |");
	printf("\n*---*---*---*---*\n");
}
/* Display passenger info of a specific passenger of a specific bus unit */
void displayPassInfo(string codes[], int searchKey, struct Bus *fleet, int ctrBus, int currentDate)
{
	int localLimit = fleet[ctrBus].limitType, ctrLoad;
	for (ctrLoad = 0; ctrLoad < localLimit; ctrLoad++)
	{
		if (ctrLoad == searchKey - 1)
		{
			system("cls");
			printf("\nDe La Salle University\nArrows Express Line Embarkation System\n\nCurrent Date: ");
			printDate(currentDate);
			printf("\n\nBus AE%d - Seat %d - Ticket #%d\n", fleet[ctrBus].busNum, ctrLoad + 2, fleet[ctrBus].load[ctrLoad].origNum);

			if (fleet[ctrBus].load[ctrLoad].busNum != 0 && fleet[ctrBus].load[ctrLoad].exitPoint != 0)
			{
				printDate(currentDate);
				printf(" ");
				printIn24H(fleet[ctrBus].load[ctrLoad].inputTime);

				printf("\n\n%s\n", fleet[ctrBus].load[ctrLoad].passName);
				printf("ID %d\n", fleet[ctrBus].load[ctrLoad].idNum);
				printf("Priority Level %d\n", fleet[ctrBus].load[ctrLoad].priority);

				printf("\nEmbarkation Point: ");
				switch (fleet[ctrBus].load[ctrLoad].entryPoint)
				{
					case 1:
						printf("[1] Manila\n");
						break;
					case 2:
						printf("[2] Laguna\n");
						break;
					default:
						break;
				}

				printf("Drop-off Point: ");
				printf("%s\n", codes[verifyDropOff(fleet[ctrBus].load[ctrLoad].exitPoint, fleet[ctrBus].load[ctrLoad].inputTime, fleet[ctrBus].load[ctrLoad].entryPoint) - 1]);
			}
			else
				printf("\nThere is no passenger information available for this seat.");
		}
	}
}
/* Returns the drop-off name that corresponds to the given drop-off code */
char *returnDropOff(int inputKey)
{
	static string returnStr;		// makes the string variable constant so it can be sent as a return value
	string codes[10] = {"SLEX Mamplasan Exit","San Jose Village Phase 5","DLSU-STC MRR Building","Phoenix Gas Station","Laguna Blvd. Guard House","Petron Gas Station","De La Salle-CSB Manila","DLSU-Manila Gate 4","DLSU-Manila Gate 2","DLSU-Manila Gate 1"};
			
	if (inputKey < 0 || inputKey > 9)
		strcpy(returnStr, "Invalid Drop-Off Point");
	else
		strcpy(returnStr, codes[inputKey]);

	return returnStr;
}
/* Returns the route of the trip given the bus numbering scheme */
char *returnOrigin(int inputKey)
{
	if (inputKey >= 101 && inputKey < 110)
		return "Manila to Laguna";
	else if (inputKey >= 150 && inputKey < 161)
		return "Laguna to Manila";
	else
		return 0;
}
/* Counts the number of passengers assigned to each drop-off location */
void countDropOff()
{
	int validCodes[ROUTE_LIMIT] = {101, 102, 103, 104, 111, 201, 221, 222, 223, 224};
	int ctrCodes;

	printf("%s\n", returnOrigin(101));
	for (ctrCodes = 0; ctrCodes < 5; ctrCodes++)
	{
		printf("%s\n", returnDropOff(validCodes[ctrCodes]));
	}

	printf("\n");
	printf("%s\n", returnOrigin(201));
	for (ctrCodes = 5; ctrCodes < 10; ctrCodes++)
	{
		printf("%s\n", returnDropOff(validCodes[ctrCodes]));
	}
}

/* MAIN FUNCTIONS */
/* Returns the current load of a bus. See documentation below for different return modes. */
int checkBusLoad(struct Bus *fleet, int ctrBus, int returnMode)
{
	int ctrUsed = 0, ctrLoad = 0;
	int localLimit = fleet[ctrBus].limitType;
	int ctrVacant = 0;				// lowest index that contains a vacant seat

	while (ctrLoad < localLimit)
	{
		if (fleet[ctrBus].load[ctrLoad].busNum != 0 && fleet[ctrBus].load[ctrLoad].exitPoint != 0) // both values are 0 if the specific seat is unoccupied
			ctrUsed++;				// number of occupied seats on a bus
		else
			ctrVacant = ctrUsed;

		ctrLoad++;					// number of seats on a bus
	}
	//printf("AE%d: (%d/%d)\n", fleet[ctrBus].busNum, ctrUsed, fleet[ctrBus].limitType);
	switch (returnMode)
	{
		case 1:						// MODE 1: returns only the number of passenger onboard
			return ctrUsed;			
			break;
		case 2:						// MODE 2: returns only whether a bus is full or not in this mode
			switch (ctrUsed)
			{
				case BUS13_LIMIT:
					if (fleet[ctrBus].limitType == BUS13_LIMIT)
						return -1; 	// returns -1 if a 13-passenger bus is full
					else
						return 0;	// returns 0 if a 16-passenger bus has 13 passengers, which is not full yet
					break;
				case BUS16_LIMIT:
					return -2; 		// returns -2 if a 16-passenger bus is full
					break;
				default:
					return 0;		// returns 0 if either kind of bus has vacant seats
					break;
			}
			break;
		case 3:						// MODE 3: returns the index of a vacant seat on the bus
			return ctrVacant;
			break;
	}

	return -1;
}
/* Gets the lowest leveled passenger onboard and returns the ticket index of the passenger to be reprocessed into the system*/
int priorityManager(struct Bus *fleet, struct Ticket *p, int ctrTicket, int *ctrFindBus)
{
	struct Bus currBus = fleet[*ctrFindBus];			// bus struct container for the current bus
	struct Ticket temp;									// passenger struct container for outgoing passenger
	int localLimit = fleet[*ctrFindBus].limitType;		// current bus capacity
	int lowestIndex = 0;		// stores the index of the lowest priority leveled passenger
	int ctrLoad;				// counter for getting the lowest priority leveled passenger

	for (ctrLoad = 0; ctrLoad < localLimit; ctrLoad++)		// get lowest priority leveled passenger
	{
		if (currBus.load[ctrLoad].priority > currBus.load[lowestIndex].priority)
			lowestIndex = ctrLoad;
	}

	if (currBus.load[lowestIndex].priority > p[ctrTicket].priority) // compares the incoming passenger with the lowest priority passenger
	{
		temp = fleet[*ctrFindBus].load[lowestIndex];
		printf("\n[SYSTEM] Passenger #%d with priority level %d has been accomodated into Bus AE%d.\n", p[ctrTicket].origNum, p[ctrTicket].priority, fleet[*ctrFindBus].busNum);
		
		fleet[*ctrFindBus].load[lowestIndex] = p[ctrTicket];
		printf("\n[SYSTEM] Passenger #%d with priority level %d has been moved out of Bus AE%d.\n", temp.origNum, temp.priority, temp.busNum);
		return temp.origNum;	// returns the index of the outgoing passenger
	}
	else
		return -1;
}
/* Looks for an available bus schedule given the input time */
int findMatchingTime(struct Bus *fleet, struct Ticket *p, int ctrTicket)
{
	int searchTime, timeFound = 0, ctrValidIndex = 0, ctrFindBus = 0, ctrOut;
	struct Bus nextSched, lastSched;

	lastSched = fleet[FLEET_LIMIT - 1];

	while (timeFound == 0)
	{
		searchTime = p[ctrTicket].inputTime;	// updates every loop to get schedules later than the time
		switch (p[ctrTicket].entryPoint) // restricts the fleet options to only specific routes if needed
		{
			case 1:						// restricts to only Manila -> Laguna
				if (ctrFindBus == 0 && ctrValidIndex == 0)
					ctrFindBus = 0;

				if (fleet[ctrFindBus].busNum > 100 && fleet[ctrFindBus].busNum < 110)
					ctrValidIndex = 1;
				break;
			case 2:						// restricts to only Laguna -> Manila
				if (ctrFindBus == 0 && ctrValidIndex == 0)
					ctrFindBus = 9;

				if (fleet[ctrFindBus].busNum > 149 && fleet[ctrFindBus].busNum < 161)
					ctrValidIndex = 1;
				break;
		}

		nextSched = fleet[ctrFindBus];

		//printf("\n#%d: %d < AE%d (%d/%d): %d && %d: %d\n", ctrTicket, searchTime, nextSched.busNum, checkBusLoad(fleet, ctrFindBus, 1), nextSched.limitType, nextSched.busTime, p[ctrTicket].entryPoint, ctrValidIndex);

		if (searchTime < nextSched.busTime && ctrValidIndex) 	// checks for a bus schedule compatible with the passenger's input time
		{
			if (checkBusLoad(fleet, ctrFindBus, 2) == -1 && fleet[ctrFindBus].limitType == BUS13_LIMIT)
			{
				fleet[ctrFindBus].limitType = BUS16_LIMIT;
				printf("\n[SYSTEM] AE%d has been converted into a 16-passenger configuration.\n", fleet[ctrFindBus].busNum);
			}

			switch (checkBusLoad(fleet, ctrFindBus, 2))
			{
				case 0:
					system("cls");
					printf("\n[SYSTEM] Passenger #%d is elligible to board AE%d at ", ctrTicket + 1, nextSched.busNum);
					printIn24H(nextSched.busTime);
					printf(".\n");
					timeFound = 1;
					return ctrFindBus; // returns the bus index in the bus array if a time is found
					break;
				case -2:
					ctrOut = priorityManager(fleet, p, ctrTicket, &ctrFindBus); // sets the removed passenger back into the loop
					if (ctrOut >= 0)														   // checks if the index is valid, otherwise it means there is no more to remove from the bus
					{
						ctrTicket = ctrOut;
						ctrFindBus++;
					}
					break;
			}
		}

		else if (searchTime >= lastSched.busTime)
		{
			system("cls");
			printf("\n[SYSTEM] No more elligible trips for the day!\n");
			timeFound = 1;
			return -1; // returns negative if there are no more available trips
		}
		else
			ctrFindBus++;
	}

	return 0; // returns 0 if it has not yet found a matching schedule
}
/* Saves passenger structs to a text file */
void saveToTripFile(struct Bus *fleet, struct Ticket *p, int ctrBus, int ctrTicket, int ctrSeat, string fileName)
{
	FILE *destPtr;
	string matchStr = "Trip";
	int ctrValid = 0;
	int ctrCheck;								 // counter to check first four letters of filename

	for (ctrCheck = 0; ctrCheck < 4; ctrCheck++) // checks if the filename has "Trip" in it
	{
		if (fileName[ctrCheck] == matchStr[ctrCheck])
			ctrValid = 1;
		else
			ctrValid = 0;
	}

	if (ctrValid)
	{
		destPtr = fopen(fileName, "a+");
		fprintf(destPtr, "\n");

		switch (p[ctrTicket].entryPoint) // write embarkation point to file
		{
			case 1:
				fprintf(destPtr, "Manila\n");
				break;
			case 2:
				fprintf(destPtr, "Laguna\n");
				break;
			default:
				printf("[ERROR] A writing error was detected while writing to file \"%s\".\n", fileName);
				ctrValid = 0;
				break;
		}

		if (ctrValid)
		{
			fprintf(destPtr, "%s\n", p[ctrTicket].passName);
			fprintf(destPtr, "%d\n%d\n%d\n%d\n%d\n%d\n%d\n", p[ctrTicket].idNum, p[ctrTicket].priority, p[ctrTicket].inputTime, fleet[ctrBus].busNum, fleet[ctrBus].limitType, ctrSeat, p[ctrTicket].exitPoint); // write passenger name to file
		}

		fclose(destPtr);
	}
}
/* Assigns passenger struct to the bus struct's load */
void assignToSeat(struct Bus *fleet, struct Ticket *p, int ctrBus, int ctrTicket, string fileName)
{
	int ctrSeat = checkBusLoad(fleet, ctrBus, 3);				// gets an index for a vacant seat onboard the bus
	if (ctrSeat > -1)
	{
		p[ctrTicket].busNum = fleet[ctrBus].busNum;		 // assigns passenger's bus number with bus number
		fleet[ctrBus].load[ctrSeat] = p[ctrTicket];		 // assigns passenger to the bus load at that index
		fleet[ctrBus].load[ctrSeat].origNum = ctrTicket; // saves the passenger number to their info card
		saveToTripFile(fleet, p, ctrBus, ctrTicket, ctrSeat, fileName);
	}
}
/* Prints out all drop-off points in full names */
void displayAllRoutes(string codes[], int entryPoint, int inputTime)
{
	int ctrRoute, localLimit;
	switch (entryPoint)
	{
		case 1:
			ctrRoute = 0;
			localLimit = 5;
			break;
		case 2:
			ctrRoute = 5;
			localLimit = ROUTE_LIMIT;
			break;
	}

	printf("\n");
	for (; ctrRoute < localLimit; ctrRoute++)
	{
		if (inputTime > 1529 && inputTime < 1815) 	// checks if the input time is in the allowed time for AE108 and AE109
		{											
			switch (entryPoint)						// does not print if it is not AE108 or AE109
			{
				case 1:
					printf("%s\n", codes[ctrRoute]);
					break;
				default:
					break;
			}	
		}
		else
		{
			if (ctrRoute != 3)						// does not print if it is not AE108 or AE109
				printf("%s\n", codes[ctrRoute]);
		}
	}

}

/* Displays all buses in the bus fleet. */
void displayAllBuses(struct Bus *fleet)
{
	int ctrFleet;

	printf("\nBus No.\t\tDeparture\tCurrent Load\n");
	for (ctrFleet = 0; ctrFleet < FLEET_LIMIT; ctrFleet++)
	{
		printf("AE[%d]\t\t", fleet[ctrFleet].busNum);
		printIn24H(fleet[ctrFleet].busTime);
		printf("\t\t%d/%d\n", checkBusLoad(fleet, ctrFleet, 1), fleet[ctrFleet].limitType);

		if (ctrFleet == 8) // creates a newline divider between Manila and Laguna bound buses
			printf("\n");
	}
}

/* Verifies all integer inputs by the user. */
void verifyIntInput(int inputItem, int *inputDir, int inputItem2, int inputItem3, string inputMsg)
{
	int inputValid = 0, inputTemp;
	string errorMsg = "";
	while (inputValid == 0)
	{
		printf("%s", inputMsg);
		if (scanf("%d", &inputTemp) == 0)
			fflush(stdin);

		switch (inputItem)
		{
			case 1: // verify date
				if ((inputItem2 < 0 && checkIfDay(inputTemp, getDaysInMonth(inputTemp)) && checkIfMonth(inputTemp)) || (inputItem2 > 0 && inputTemp == inputItem2))
					inputValid = 1;
				else
				{
					if (inputItem2 > 0 && inputTemp != inputItem2)
						strcpy(errorMsg, "Please enter the current date. Advance bookings are not allowed.");
					else
						strcpy(errorMsg, "Please enter a valid date.");
				}
				break;
			case 2: // verify time
				if (checkIf24H(inputTemp))
					inputValid = 1;
				else
					strcpy(errorMsg, "Please enter a valid time.");
				break;
			case 4: // verify ID number
				if (verifyIDNumber(inputTemp))
					inputValid = 1;
				else
					strcpy(errorMsg, "Please enter a valid ID number.");
				break;
			case 5: // verify priority level
				if (inputTemp >= 0 && inputTemp <= 6)
					inputValid = 1;
				else
					strcpy(errorMsg, "Please enter a valid priority level from 1 to 6.");
				break;
			case 6: // verify entry code
				if (inputTemp >= 1 && inputTemp <= 2)
					inputValid = 1;
				else
					strcpy(errorMsg, "Please enter a valid route code from 1 to 2 only.");
				break;
			case 7: // verify exit code
				if (verifyDropOff(inputTemp, inputItem2, inputItem3))
					inputValid = 1;
				else
					strcpy(errorMsg, "Please enter a valid drop-off point code.");
				break;
			
			case 10: // verify menu option choice
				if (inputTemp >= 1 && inputTemp <= 5)
					inputValid = 1;
				else
					strcpy(errorMsg, "Please enter a valid input.");
				break;
			case 11: // verify bus selection
				if ( (inputTemp >= 101 && inputTemp <= 109) || (inputTemp >= 150 && inputTemp <= 160) || inputTemp == 0)
					inputValid = 1;
				else
					strcpy(errorMsg, "Please enter a valid bus number.");
				break;

			case 12: // verify bus seat selection
				if (inputTemp - 1 >= -1 && inputTemp - 1 < inputItem2)
					inputValid = 1;
				else
					strcpy(errorMsg, "Please enter a valid seat number.");
				break;

			default:
				break;
		}
		
		if (inputValid != 1)
			printf("\n[ERROR] Invalid input. %s\n\n", errorMsg);
		else
			*inputDir = inputTemp;
	}
}

/* Asks the user for passenger details */
void inputNewTicket(string codes[], struct Ticket *p, int ctrTicket, int currentDate)
{
	printf("\nDe La Salle University\nArrows Express Line Embarkation System\n\nCurrent Date: ");
	printDate(currentDate);
	printf("\n\n");

	p[ctrTicket].origNum = ctrTicket;			// saves the original struct ticket index
	p[ctrTicket].inputDate = currentDate;		// retrieves the date from the system date

	verifyIntInput(2, &p[ctrTicket].inputTime, -1, -1, "Current 24-Hour Time (HHMM): ");

	printf("Name of Passenger: ");
	fgetc(stdin);
	fgets(p[ctrTicket].passName, sizeof(string), stdin);
	p[ctrTicket].passName[strlen(p[ctrTicket].passName) - 1] = '\0'; // remove newline

	verifyIntInput(4, &p[ctrTicket].idNum, -1, -1, "ID Number: ");
	printf("\n[1] Faculty and ASF with Inter-campus assignments\n[2] Students with Inter-campus enrolled subjects or enrolled in thesis using Inter-campus facilities\n[3] Researchers\n[4] School Administrators (Academic Coordinators level and up for Faculty and ASF, and Director level and up for APSP)\n[5] University Fellows\n[6] Employees and Students with official business\n\n");
	verifyIntInput(5, &p[ctrTicket].priority, -1, -1, "Priority Level (1-6): ");
	verifyIntInput(6, &p[ctrTicket].entryPoint, -1, -1, "\n[1] Manila -> Laguna\n[2] Laguna -> Manila\nRoute of Trip: ");

	displayAllRoutes(codes, p[ctrTicket].entryPoint, p[ctrTicket].inputTime);
	verifyIntInput(7, &p[ctrTicket].exitPoint, p[ctrTicket].inputTime, p[ctrTicket].entryPoint, "Drop-off Point code: ");
}

/* Inserts initial values in all bus units */
void initializeBus(struct Bus *fleet)
{
	int schedule[20][2] = {            // List of bus schedules and corresponding departure time
        // Manila to Laguna (101-109)
        {101, 600}, {102, 730}, {103, 930}, {104, 1100}, {105, 1300}, {106, 1430}, {107, 1530}, {108, 1700}, {109, 1815},
        // Laguna to Manila (150-160)
        {150, 530}, {151, 545}, {152, 700}, {153, 730}, {154, 900}, {155, 1100}, {156, 1300}, {157, 1430}, {158, 1530}, {159, 1700}, {160, 1815}
    };
	
	int ctrFleet, ctrUnit;
	for (ctrFleet = 0; ctrFleet < FLEET_LIMIT; ctrFleet++)
	{
		fleet[ctrFleet].busNum = schedule[ctrFleet][0];
		fleet[ctrFleet].busTime = schedule[ctrFleet][1];
		fleet[ctrFleet].limitType = BUS13_LIMIT;				// initializes all buses with 13-passenger config
		for (ctrUnit = 0; ctrUnit < BUS16_LIMIT; ctrUnit++)		// all loads arrays can fit up to 16 passengers, but the system will limit the number of passengers to 13 passengers unless the limitType is changed
		{
			fleet[ctrFleet].load[ctrUnit].busNum = 0;
			fleet[ctrFleet].load[ctrUnit].exitPoint = 0;
		}
	}
}
/* Displays all drop-off points on screen and number of passengers for each drop-off */
void viewAllDropOffs(struct Ticket *p, string codes[], int currentDate, int ctrTicket)
{
	int ctrPass = ctrTicket, ctrList, ctrLoc, ctrData, verifyCode;
	string exitKey;

	printf("\nDe La Salle University\nArrows Express Line Embarkation System\n\nCurrent Date: ");
	printDate(currentDate);

	printf("\n\nCount\tDrop-off Point\n");

	for (ctrList = 0; ctrList < ROUTE_LIMIT; ctrList++)
	{
		ctrLoc = 0;
		if (ctrPass > 0)	// refrains from calculating counts if no passengers are detected in the system
		{
			for (ctrData = 0; ctrData < ctrPass; ctrData++)
			{
				switch (p[ctrData].busNum)
				{
					case 108:
					case 109:
						verifyCode = verifyDropOff(p[ctrData].exitPoint, p[ctrData].inputTime, p[ctrData].entryPoint) - 1; // verifyDropOff will return the index + 1 of the drop-off code, so we must subtract by 1 to match the index numbering of the codes[] array of strings. This one also collects the passenger input time so that it would work with the verification for drop-off point code 104.
						break;

					default:
						verifyCode = verifyDropOff(p[ctrData].exitPoint, -1, p[ctrData].entryPoint) - 1; // verifyDropOff will return the index + 1 of the drop-off code, so we must subtract by 1 to match the index numbering of the codes[] array of strings. This one disregards the input time.
						break;
				}

				if (verifyCode == ctrList)
					ctrLoc++;
			}
		}

		printf("%d\t%s\n", ctrLoc, codes[ctrList]);

		if (ctrList == 4)
			printf("\n");
	}

	printf("\nEnter any character to return to the main menu.\nInput: ");
	fgetc(stdin);
	fgets(exitKey, sizeof(string), stdin);
}
/* Displays all passengers onboard a bus */
void displayAllPassengers(string codes[], struct Bus *fleet, int ctrBus, int currentDate)
{
	int ctrSelect = 1, ctrList, localLimit = fleet[ctrBus].limitType;

	while (ctrSelect != 0)
	{
		printf("\nSeat\tName of Passenger\n");
		for (ctrList = 0; ctrList < localLimit; ctrList++)
		{
			if (fleet[ctrBus].load[ctrList].busNum != 0 && fleet[ctrBus].load[ctrList].exitPoint != 0)
				printf("[%d]\t%s\n", ctrList + 1, fleet[ctrBus].load[ctrList].passName);
			else
				printf("[%d]\t%s\n", ctrList + 1, "Vacant");
		}

		verifyIntInput(12, &ctrSelect, localLimit, -1, "\nEnter the seat number of the passenger for more information, or input [0] to return to the previous menu.\n\nInput: ");

		if (ctrSelect - 1 >= 0 && ctrSelect - 1 < localLimit)
		{
			system("cls");
			displayPassInfo(codes, ctrSelect, fleet, ctrBus, currentDate);
		}
	}
	system("cls");
}
/* Displays all buses in the bus fleet with passenger counts */
void viewBusFleet(string codes[], struct Bus *fleet, int currentDate)
{
	int ctrSelect = 1, ctrFleet = 0, localLimit = 0;

	while (ctrSelect != 0)
	{
		printf("\nDe La Salle University\nArrows Express Line Embarkation System\n\nCurrent Date: ");
		printDate(currentDate);
		printf("\n");

		displayAllBuses(fleet);
		printf("\nEnter a bus number [1xx] to view bus information or enter [0] to return to the main menu.\n");
		verifyIntInput(11, &ctrSelect, currentDate, -1, "Input: ");
		system("cls");
		
		ctrFleet = 0;
		localLimit = 0;

		while (ctrFleet < FLEET_LIMIT && localLimit == 0)
		{
			if (fleet[ctrFleet].busNum == ctrSelect)
				localLimit = fleet[ctrFleet].limitType;
			else
				ctrFleet++;
		}

		if (ctrSelect > 100)
		{
			printf("\nDe La Salle University\nArrows Express Line Embarkation System\n\nCurrent Date: ");
			printDate(currentDate);
			printf("\n");

			switch (localLimit)
			{
				case BUS13_LIMIT:
					printf("\nX indicates empty seats while O indicates filled seats.\n");
					displayConfig13(fleet, ctrFleet);
					break;
				case BUS16_LIMIT:
					printf("\nX indicates empty seats while O indicates filled seats.\n");
					displayConfig16(fleet, ctrFleet);
					break;
				default:
					printf("\n");
					break;
			}

			displayAllPassengers(codes, fleet, ctrFleet, currentDate);
		}
	}
}
/* Generates a file name given a date */
void generateTripFileName (string *fileName, int currentDate)
{
	int month = currentDate / 1000000;
	int day = (currentDate / 10000) % 100;
	int year = currentDate % 10000;
	string tempStr;

	strcpy(*fileName, "Trip-");

	if (day < 10)
		strcat(*fileName, "0");

	snprintf(tempStr, sizeof(day), "%d", day);
	strcat(*fileName, tempStr);
	strcat(*fileName, "-");

	if (month < 10)
		strcat(*fileName, "0");
	snprintf(tempStr, sizeof(month), "%d", month);
	strcat(*fileName, tempStr);
	strcat(*fileName, "-");

	snprintf(tempStr, sizeof(year) + 1, "%d", year);
	strcat(*fileName, tempStr);
	strcat(*fileName, ".txt");
}
/* Fills the system database with bus and passenger data from a date-specific text file */
void loadTripFile (struct Bus *fleet, struct Ticket *p, int currentDate, int *ctrTicket)
{
	string fileName, dumpStr;
	int scanResult = 1;
	generateTripFileName(&fileName, currentDate);
	FILE *srcPtr = fopen(fileName, "r");

	if (srcPtr == NULL)
	{
		fopen(fileName, "w");
		printf("\n[SYSTEM] New trip file created.\n");
		fclose(srcPtr);
	}
	else
	{
		//printf("\n[SYSTEM] File found. Reading...\n");
		do 
		{
			scanResult = fscanf(srcPtr, "%s", dumpStr);

			if (scanResult < 0)
				system("cls");		// hides the last entry's loading message before displaying main menu
			else
			{
				printf("%s\n", dumpStr);
				switch (dumpStr[0])
				{
					case 'M':
					case 'm':
						p[*ctrTicket].entryPoint = 1;
						break;
					case 'l':
					case 'L':
						p[*ctrTicket].entryPoint = 2;
						break;
					default:
						break;
				}
				
				fgetc(srcPtr);
				fgets(p[*ctrTicket].passName, sizeof(string), srcPtr);			// get name with spaces
				p[*ctrTicket].passName[strlen(p[*ctrTicket].passName) - 1] = '\0'; // remove newline
				printf("%s\n", p[*ctrTicket].passName); // store name

				fscanf(srcPtr, "%d\n", &p[*ctrTicket].idNum); // store ID number
				printf("%d\n", p[*ctrTicket].idNum);
				fscanf(srcPtr, "%d\n", &p[*ctrTicket].priority); // store priority number
				printf("%d\n", p[*ctrTicket].priority);
				fscanf(srcPtr, "%d\n", &p[*ctrTicket].inputTime); // store time of input
				printf("%d\n", p[*ctrTicket].inputTime);
				fscanf(srcPtr, "%s\n", dumpStr); 					// skip bus number
				fscanf(srcPtr, "%s\n", dumpStr); 					// skip bus configuration
				fscanf(srcPtr, "%s\n", dumpStr);					// skip bus seat index
				fscanf(srcPtr, "%d\n", &p[*ctrTicket].exitPoint);	// store drop off code
				printf("%d\n", p[*ctrTicket].exitPoint);

				p[*ctrTicket].inputDate = currentDate;										// store date of input
				//printf("scanResult: %d\tctrTicket: %d\n", scanResult, *ctrTicket);
				assignToSeat(fleet, p, findMatchingTime(fleet, p, *ctrTicket), *ctrTicket, "0"); // assign seat but dont save to file

				(*ctrTicket)++;
			}

			//displayAllBuses(fleet);
		}
		while (scanResult > 0 && *ctrTicket < DATABASE_LIMIT);

		fclose(srcPtr);

		if (scanResult == -2)
			printf("\n[ERROR] A reading error was encountered when attempting to read \"%s\".\n", fileName);
		else
			printf("\n[SYSTEM] Loading complete.\n");
	}
	
}

/* MAIN MENU FUNCTIONS */
/* Displays menu options and current date */
void displayMenuOptions(int currentDate, int ctrTicket)
{
	printf("\nDe La Salle University\nArrows Express Line Embarkation System\n\nCurrent Date: ");
	printDate(currentDate);
	printf("\nCurrent Passenger Count: %d\n", ctrTicket);
	printf("\n[1] Encode Passenger\n[2] View Bus and Passenger Info\n[3] View Route and Drop-Off Point Info\n[4] Exit\n\n");
}
/* Displays main menu and handles user input for menu options */
int displayMenu(int currentDate, int *ctrMenu, int *ctrTicket, int *ctrInit, struct Bus *fleet, struct Ticket *p)
{
	string fileName;				// pointer for the destination file name
	string codes[ROUTE_LIMIT] = {
		// MNL-LAG via SLEX Mamplasan Exit (101-104)
		"[101] SLEX Mamplasan Exit", "[102] San Jose Village Phase 5", "[103/112] DLSU-STC Milagros Del Rosario (MRR) Building - East Canopy", "[104] Phoenix Gas Station, Sta. Rosa-Tagaytay Rd.", // 104 is only for AE108 & AE109
		// MNL-LAG via SLEX Eton Exit (111-112)
		"[111] Laguna Blvd. Guard House",
		// LAG-MNL via all routes (201)
		"[201] Petron Gas Station, Gil Puyat Ave.",
		// LAG-MNL (211-213/221-224)
		"[221] De La Salle - College of St. Benilde Manila", "[211/222] DLSU-Manila Gate 4 - Gokongwei Gate", "[212/223] DLSU-Manila Gate 2 - North Gate", "[213/224] DLSU-Manila Gate 1 - South Gate"
	};

	if (*ctrInit)
	{
		loadTripFile(fleet, p, currentDate, ctrTicket);
		*ctrInit = 0;
	}

	generateTripFileName(&fileName, currentDate);
	displayMenuOptions(currentDate, *ctrTicket);
	verifyIntInput(10, ctrMenu, currentDate, -1, "Input: ");
	switch (*ctrMenu)
	{
		case 1:
			system("cls");
			inputNewTicket(codes, p, *ctrTicket, currentDate);
			assignToSeat(fleet, p, findMatchingTime(fleet, p, *ctrTicket), *ctrTicket, fileName);
			(*ctrTicket)++;
			break;
		case 2:
			system("cls");
			viewBusFleet(codes, fleet, currentDate);
			system("cls");
			break;
		case 3:
			system("cls");
			viewAllDropOffs(p, codes, currentDate, *ctrTicket);
			system("cls");
			break;
		case MENU_EXIT_OPTION:
			system("cls");
			printf("\nCCPROG2-S14B Machine Project\nTerm 2, AY 2019-2020\nDeveloped by John Matthew Gan\n");
			break;
		default:
			break;
	}

	return *ctrMenu;
}

/* START FUNCTION */
int main()
{
	int ctrMenu = 0, ctrTicket = 0, ctrInit = 1, currentDate;

	struct Bus fleet[FLEET_LIMIT];
	struct Ticket p[DATABASE_LIMIT];

	initializeBus(fleet);
	system("cls");
	printf("\nDe La Salle University\nArrows Express Line Embarkation System\n\n");
	verifyIntInput(1, &currentDate, -1, -1, "Current Date (MMDDYYYY): ");
	system("cls");

	while (displayMenu(currentDate, & ctrMenu, &ctrTicket, &ctrInit, fleet, p) != MENU_EXIT_OPTION);

	return 0;
}

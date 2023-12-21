/*
Joshua Edwards
CS 2060 003 AirUCCS Project Iteration 3
Made on Windows 10
This program creates a system for property owners to add their properties to the system
and then allows a user to view information about each rental property and then chose a 
property to rent and input the number of nights they want to rent that property and receive 
an charge based on the number of nights and the discount rate. The owner of the property can
also input a special value to receive back a summary of the nights rented and the total
money made which gets written to a file for each property.
*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <math.h>

//Maximum length of a string
#define STRING_LENGTH 80
//Two dimensional array storage amounts for rows and columns of surve data
#define VACATION_RENTERS 5
#define RENTER_SURVEY_CATEGORIES 4
//Rental property rating ranges
#define MINRATING 1
#define MAXRATING 5
//Rental property login and sentinal values
#define CORRECT_ID "id"
#define CORRECT_PASSCODE "ab"
#define LOGIN_MAX_ATTEMPTS 4
#define SENTINAL_NEG1 -1
//rental property constant ranges
#define MIN_RENTAL_NIGHTS 1
#define MAX_RENTAL_NIGHTS 14
#define MIN_RATE 1
#define MAX_RATE 1000
#define DISCOUNT_MULTIPLIER 2
// folder path
#define BASE_FOLDERPATH "C:/Users/black/Desktop/UCCS-Code-repo/fundraiser/"


// Defining property structure
// can be declared using the "Property" tag
typedef struct property {
	unsigned int minimumNights;
	unsigned int maximumNights;
	unsigned int interval1Nights;
	unsigned int interval2Nights;
	double minimumRate;
	double maximumRate;
	double rate;
	double discount;
	int discountMultiplier;

	unsigned int numOfRenters;
	unsigned int totalNights;
	double totalCharges;

	int rentalSurvey[VACATION_RENTERS][RENTER_SURVEY_CATEGORIES];
	double categoryAverages[RENTER_SURVEY_CATEGORIES];

	char name[STRING_LENGTH];
	char location[STRING_LENGTH];

	//self referencial structure
	struct Property* nextPtr;
} Property;

// Prints the rental property information 
void displayRentalPropertyInfo(const Property* propStrucPtr);
// Calculates the charge based on the number of nights rented
double calculateCharges(unsigned int nights, unsigned int interval1Nights, unsigned int interval2Nights, double rate, double discount);
// Prints the name, location, number of nights, and the charge
void printNightsCharges(const Property* propStrucPtr);

// login function
bool isLoggedIn(const char* correctID, const char* correctPassword, const unsigned int allowedAttempts, const unsigned int STR_LEN);
// rental mode logic function
void rentalMode(Property* headPropPtr, const int minRating, const unsigned int maxRating, size_t rows, size_t columns, const unsigned int discountMultiplier, const int sentinal, const char* correctID, const char* correctPassword, const int unsigned allowedAttempts, size_t STR_LEN, const char* categories[RENTER_SURVEY_CATEGORIES]);

// functions for ratings
void printCategories(const char* categories[RENTER_SURVEY_CATEGORIES], size_t totalCategories);
void getRatings(Property* propStrucPtr, size_t renters_rows, size_t category_columns, unsigned int min_rating, unsigned int max_rating, const char* categories[RENTER_SURVEY_CATEGORIES]);
void printSurveyResults(const Property* propStrucPtr, size_t renters_rows, size_t category_columns, const unsigned int min_rating, const unsigned int max_rating, const char* categories[RENTER_SURVEY_CATEGORIES]);
void calculateCategoryAverages(Property* propStrucPtr, size_t renters_rows, size_t category_columns);
void displaySurveyAverages(const Property* propStrucPtr, size_t total_categories);

// getting input functions
int getValidInt(const unsigned int min, const unsigned int max);
int getValidIntSentinal(const unsigned int min, const unsigned int max, const unsigned int sentinal);
void removeNewLine(char* stringPtr, int size);
bool scanInt(const char* const stringInput, int* const integerPtr);
char validateYesNo();

// functions for property setup (structure)
void initializeDefaultPropertyVals(Property* prop, const unsigned int minNights, const unsigned int maxNights, const double minRate, const double maxRate, int discountMultiplier);
void setUpProperty(Property* propertyPtr, int STR_SIZE, const unsigned int minNights, const unsigned int maxNights, const double minRate, const double maxRate);

// linked list functions
Property* choosePropToRent(Property** headPropPtr, int str_len);
int strcmpCaseIgnore(const char* string1, const char* string2, int string_length);
void insertPropLoop(Property** headPropPtr, int str_len);
void insertProp(Property** headPropPtr, Property* currentProp, int str_len);
void displayProperties(Property* headPropPtr);
void freeRemainingProperties(Property** headPropPtr);

// file operation functions
void writePropsToFile(Property* headPropPtr, char* folderPath, const char* categories[RENTER_SURVEY_CATEGORIES]);
void printCategoriesAndRatingsToFile(Property* propStrucPtr, FILE* filePtr, const char* categories[RENTER_SURVEY_CATEGORIES], size_t totalCategories);
void printNightsChargesToFile(Property* propStrucPtr, FILE* filePtr);
void replaceCharsInString(char* stringFilePathPtr, char charToScanFor, char replaceWithChar, size_t str_len);

// main function
int main(void) {
	// rating survey initializations
	const char* surveyCategories[RENTER_SURVEY_CATEGORIES] = { "Check-in Process", "Cleanliness", "Amenities" , "cat4"};

	// initialize master headPtr
	Property* headPtr = NULL;

	// log the user in, return true if user logs in correctly, false otherwise
	bool loggedIn = isLoggedIn(CORRECT_ID, CORRECT_PASSCODE, LOGIN_MAX_ATTEMPTS, STRING_LENGTH);

	// if the user logged in, enter loop
	if (loggedIn) {
		puts("\nLogin successful\n");
		// call insert property loop here... (linked list)
		insertPropLoop(&headPtr, STRING_LENGTH);
		rentalMode(&headPtr, MINRATING, MAXRATING, VACATION_RENTERS, RENTER_SURVEY_CATEGORIES, DISCOUNT_MULTIPLIER, SENTINAL_NEG1, CORRECT_ID, CORRECT_PASSCODE, LOGIN_MAX_ATTEMPTS, STRING_LENGTH, surveyCategories);
		
		// after rentalMode ends, check the total nights and total charges in the struct
		// iterate through list
		if (headPtr != NULL)
		{
			// declares new prop pointer for iteration
			Property* currentPropPtr = headPtr;
			// loops until current prop pointer is NULL (reached end of list)
			while (currentPropPtr != NULL)
			{
				// display results and go to next node
				if (currentPropPtr->totalNights == 0 || currentPropPtr->totalCharges == 0) // this is for when there are no rentals for a property
				{
					puts("\n==========================================================");
					printf("\nThere were no rentals for the property named \"%s\"...", currentPropPtr->name);
					puts("\n==========================================================\n");
				}
				else // there were charges, calculate and display averages
				{
					puts("\n==========================================================");
					printNightsCharges(currentPropPtr);
					printCategories(surveyCategories, RENTER_SURVEY_CATEGORIES);
					calculateCategoryAverages(currentPropPtr, VACATION_RENTERS, RENTER_SURVEY_CATEGORIES);
					displaySurveyAverages(currentPropPtr, RENTER_SURVEY_CATEGORIES);
					puts("\n==========================================================\n");
				}
				// advance to next property
				currentPropPtr = currentPropPtr->nextPtr;
			}
		}
		// there are no elements in list to begin with
		else
		{
			puts("List is empty");
		}
		// write property results to files
		writePropsToFile(headPtr, BASE_FOLDERPATH, surveyCategories);
		// deallocate remaining memory for all existing property structs
		freeRemainingProperties(&headPtr);
	}
	puts("\nExiting AirUCCS. :)");
} // end of main


// mathy functons
// ================================================================================================

/*	
	Function description : Calculates the charge based on the number of nights rented
	parameters: unsigned int nights, unsigned int interval 1, unsigned int interval 2, double rate, double discount
	return: double charge that was calculated based on nights and discounts
*/
 double calculateCharges(unsigned int nights, unsigned int interval1Nights, unsigned int
	interval2Nights, double rate, double discount) {
	double charge = 0;

	/* 
	This algorithm assume that the user is not getting any discount by calculating the charge using the base rent. Then the program will
	test the night input to see if the user is getting any discount. If the user does qualify for a discount, the program will SUBTRACT 
	the respective discount depending on how many nights the user entered. 
	*/

	// base case nights is less than interval 1
	charge = nights * rate; 
	// second case nights are between interval 1 and 2 (standard discount)
	if (nights > interval1Nights) { 
		if (nights > interval2Nights) 
		{
			// if nights is greater than interval 2, in other words if the user is maximizing their basic discount
			charge = charge - ((interval2Nights - interval1Nights) * discount);
		}
		else {
			// if the user is between interval 1 and interval 2
			charge = charge - ((nights - interval1Nights) * discount);
		}
	}
	// third case nights are over interval 2 (double discount)
	if (nights > interval2Nights) {
		charge = charge - ((nights - interval2Nights) * (discount * 2));
	}
	printf("\nNights\t\tCharge\n%d\t\t$%.2f", nights, charge);
	return charge;
}

/*
	description: this function reads through the 2D rating array and calculates the average rating for each category and writes it to the categoryAverages array
	parameters: property structure, sizes of array dimensions
	return: nothing
*/
void calculateCategoryAverages(Property* propStrucPtr, size_t renters_rows, size_t category_columns) {
	// read integers from each row and calculate average
	// loop through 2D rentalSurvey array and get sum total of each category
	for (size_t category = 0; category < category_columns; category++)
	{
		int sum = 0;
		for (size_t renter = 0; renter < renters_rows; renter++)
		{
			sum += propStrucPtr->rentalSurvey[renter][category]; // add each rating to total sum
		}
		// divide sum by amount of renters to get average
		double average = ((double)sum / (double)(propStrucPtr->numOfRenters)); // parse everything to double so there is no loss of data during the calculation
		// store average in categoryAverages array
		propStrucPtr->categoryAverages[category] = average;
	}
}

// ================================================================================================


// login function
// ================================================================================================

/*
	description: validates user login against correct ID and password
	parameters: constant character array (string) correctID, constant string correctPassword, constant unsigned int maximum allowed attempts, constant int string length
	return: bool depending on whether the user entered the correct credentials, they entered correctly = true, they did not enter correctly = false
*/
bool isLoggedIn(const char* correctID, const char* correctPassword, const unsigned int allowedAttempts, const unsigned int STR_LEN) {
	char id[] = { "Default ID" };
	char password[] = { "Default password" };
	puts("AirUCCS Rental Property Owner Login");
	bool isMatch = false;
	int attempts = 0;

	// loop exits when the user entered the correct ID and password, or they reached the max attempts allowed
	while (!(isMatch == true) && (attempts < allowedAttempts)) {
		puts("Enter your AirUCCS ID: ");
		fgets(id, STR_LEN, stdin);

		puts("Enter your AirUCCS password: ");
		fgets(password, STR_LEN, stdin);

		// removes newline char from input
		removeNewLine(id, strlen(id));
		removeNewLine(password, strlen(password));

		// used strcmp() to compare what the user entered to the correct values
		if (strcmp(id, correctID) != 0) {
			if (strcmp(password, correctPassword) != 0) {
				puts("The ID is incorrect");
				puts("The password is incorrect");
			}
			else {
				puts("The ID is incorrect");
			}
		}
		else if (strcmp(password, correctPassword) != 0) {
			puts("The password is incorrect");
		}
		// if they entered both the correct ID and correct password
		else if ((strcmp(id, correctID) == 0) && (strcmp(password, correctPassword) == 0)) {
			isMatch = true;
		}
		attempts++;
	}
	return isMatch;
}

// ================================================================================================


// rental mode logic function
// ================================================================================================
/*
	description: this is the main loop of the program and is responsible for handling all the non-owner user interactions. Renting, rating, repeat.
	parameters: property structure by reference, constant unsigned int minRating, constant unsigned int maxRating, array dimensions, constant unsigned int discountMultiplier, constant int sentinal value, constant string correctID and correctPassword, constant int unsigned allowed attempts, string length, constant string array of survey categories
	return: nothing, modifies values in structure by reference pointers
*/
void rentalMode(Property* headPropPtr, const int minRating, const unsigned int maxRating, size_t rows, size_t columns, const unsigned int discountMultiplier, const int sentinal, const char* correctID, const char* correctPassword, const int unsigned allowedAttempts, size_t STR_LEN, const char* categories[RENTER_SURVEY_CATEGORIES]) {
	bool loggedOut = false;
	// main program loop
	do {
		// allow user to choose property to rent (returns pointer to property selected)
		Property* currentPropertyPtr = choosePropToRent(&headPropPtr, STR_LEN);

		displayRentalPropertyInfo(currentPropertyPtr);
		printSurveyResults(currentPropertyPtr, rows, columns, minRating, maxRating, categories);
		// checks if limit of renters has been reached
		if (currentPropertyPtr->numOfRenters < rows) {
			puts("\n\nEnter the number of nights to rent: ");
			int nightInput = getValidIntSentinal(currentPropertyPtr->minimumNights, currentPropertyPtr->maximumNights, sentinal);

			// check for sentinal
			if (nightInput == sentinal) {
				// prompt for login again
				if (isLoggedIn(correctID, correctPassword, allowedAttempts, STR_LEN)) {
					puts("\nLogged out successfully, displaying results.\n");
					loggedOut = true;
				}
				else {
					puts("\nUser not logged in, returning to rental mode.\n");
					loggedOut = false;
				}
			}
			else {
				// calculate charge
				double charge = calculateCharges(nightInput, currentPropertyPtr->interval1Nights, currentPropertyPtr->interval2Nights,
					currentPropertyPtr->rate, currentPropertyPtr->discount);

				// incremement totalNights and totalCharges
				currentPropertyPtr->totalCharges += charge;
				currentPropertyPtr->totalNights += nightInput;

				getRatings(currentPropertyPtr, rows, columns, minRating, maxRating, categories);
			}
		}
		else {
			loggedOut = true;
		}
	} while (!loggedOut);
}

// ================================================================================================
 

// rating functions
// ================================================================================================
/*
	description: displays the character arrays of categories
	parameters: constant string of categories, number of categories
	return: nothing
*/
void printCategories(const char* categories[RENTER_SURVEY_CATEGORIES], size_t totalCategories)
{
	//loop to display each category horizontally
	printf("%s", "\n\nRating Categories:\t");
	for (size_t surveyCategory = 0; surveyCategory < totalCategories; ++surveyCategory)
	{
		printf("\t%zu.%s\t", surveyCategory + 1, categories[surveyCategory]);
	}
	puts(""); // start new line of output
}

/*
	description: this function will get ratings from the renters and fill the 2D array with those ratings
	parameters: property structure by reference, array dimensions, unsigned int min and max ratings, constant string of categories
	return: nothing
*/
void getRatings(Property* propStrucPtr, size_t renters_rows, size_t category_columns, unsigned int min_rating, unsigned int max_rating, const char* categories[RENTER_SURVEY_CATEGORIES]) {
	unsigned int nextIndexForRating = propStrucPtr->numOfRenters; 
	printCategories(categories, category_columns);
	
	// checks for out of bounds row
	if (nextIndexForRating < renters_rows) {
		// loops through each column
		for (size_t category = 0; category < category_columns; category++)
		{
			// ask user for each rating	
			printf("\nEnter your rating for \nCategory %d: ", (int)(category + 1)); // category + 1 because the category index starts at 0 and I want renters to start at 1
			// verify valid integer
			int rating = getValidInt(min_rating, max_rating);
			// write data back to array
			propStrucPtr->rentalSurvey[nextIndexForRating][category] = rating;
		}
		// increase row "counter" 
		(propStrucPtr->numOfRenters)++;
	}
	else {
		puts("Array full!");
	}
}

/*
	description: this function displays the results of the rating data to the console (reads through 2D array)
	parameters: constant property struct, array dimensions, constant unsigned ints for min and max ratings, constant string of categories
	return: nothing
*/
void printSurveyResults(const Property* propStrucPtr, size_t renters_rows, size_t category_columns, const unsigned int min_rating, const unsigned int max_rating, const char* categories[RENTER_SURVEY_CATEGORIES]) {
	size_t category = 0;
	printf("%s", "\nSurvey results");

	// loop through array and display the each row and column if ratings exist
	if (propStrucPtr->numOfRenters != 0) {	
		printCategories(categories, category_columns);
		for (size_t renter = 0; renter < renters_rows; renter++) {
			if ((propStrucPtr->rentalSurvey[renter - 1][category] >= min_rating) && (propStrucPtr->rentalSurvey[renter][category] <= max_rating)) { /* checks if the
				current row has ratings in it. If not, it does not display the row. renter - 1 because renter is incremented in the previous line and I need to reverse that to check the array element. */
				printf("\nSurvey %d: ", (int)(renter + 1)); // renter + 1 because index starts at 0 and we want to start at 1
				for (category = 0; category < category_columns; category++) {
					printf("\t%d", propStrucPtr->rentalSurvey[renter][category]); // print each rating
				}
			}
		}
	}
	else {
		puts("\nNo ratings currently");
	}
}

/*
	description: Prints the name and location of property, summary of nights and charges 
	parameters: constant property struct
	return: nothing, console output
*/
void printNightsCharges(const Property* propStrucPtr) {
	printf("\nResults for \"%s\" property...", propStrucPtr->name);
	printf("\nName: %s", propStrucPtr->name);
	printf("\nLocation: %s", propStrucPtr->location);
	puts("\nRental Property Owner Total Summary");
	printf("\nRenters\tNights\tCharge\n%d\t%d\t$%.0f", propStrucPtr->numOfRenters, propStrucPtr->totalNights, propStrucPtr->totalCharges);
}

/*
	description: displays the survey averages to console
	parameters: constant property structure, number of categories
	return: nothing, console output
*/
void displaySurveyAverages(const Property* propStrucPtr, size_t total_categories) {
	// this function prints the average of each category
	// iterate through categoryAverages array and display results
	printf("%s", "Rating averages:");
	for (size_t category = 0; category < total_categories; category++) {
		printf("\t\t\t%.2f", propStrucPtr->categoryAverages[category]);
	}
}

// ================================================================================================


// property functions
// ================================================================================================
/*
	description: initializes property structure variables to default values so they don't contain junk
	parameters: property struct, const unsigned int min and max nights, const double min and max rates
	return: nothing
*/
void initializeDefaultPropertyVals(Property* prop, const unsigned int minNights, const unsigned int maxNights, const double minRate, const double maxRate, int discountMultiplier) {
	prop->minimumNights = minNights;
	prop->maximumNights = maxNights;
	prop->minimumRate = minRate;
	prop->maximumRate = maxRate;
	prop->interval1Nights = 0;
	prop->interval2Nights = 0;
	prop->rate = 0;
	prop->discount = 0;
	prop->discountMultiplier = discountMultiplier;
	prop->numOfRenters = 0;
	prop->totalCharges = 0;
	prop->totalNights = 0;
	prop->numOfRenters = 0;
	prop->nextPtr = NULL; // <----- important!!!!! there are many checks in this program that checks for a NULL pointer value and if this is not initialized to NULL, problems arise
	strncpy(prop->name, " default name ", strlen(" default name "));
	strncpy(prop->location, " default location ", strlen(" default location "));

	// initialize arrays with 0s
	// 1D array
	for (size_t i = 0; i < RENTER_SURVEY_CATEGORIES; i++) {
		prop->categoryAverages[i] = 0;
	}
	// 2D array
	for (size_t row = 0; row < VACATION_RENTERS; row++) {
		for (size_t column = 0; column < RENTER_SURVEY_CATEGORIES; column++) {
			prop->rentalSurvey[row][column] = 0;
		}
	}
}

/*
	description: prompts owner for property rental information, calls input validation functions and writes input to property structure values
	parameters: property structure, string size, const unsigned int min and max nights, const double min and max rates
	return: nothing
*/
void setUpProperty(Property* propertyPtr, int STR_SIZE, const unsigned int minNights, const unsigned int maxNights, const double minRate, const double maxRate) {
	puts("Set up rental property information");

	puts("Enter the number of nights until first discount: ");
	propertyPtr->interval1Nights = getValidInt(minNights, maxNights-1);
	puts("Enter the number of nights until second discount: ");
	propertyPtr->interval2Nights = getValidInt(propertyPtr->interval1Nights + 1, maxNights);

	puts("Enter the nightly rental rate: ");
	propertyPtr->rate = getValidInt(minRate, maxRate);

	puts("Enter the discount: ");
	propertyPtr->discount = getValidInt(minRate, propertyPtr->rate);

	puts("Enter the property name: ");
	fgets(propertyPtr->name, STR_SIZE, stdin); // dont need & because passing array which already holds address
	removeNewLine(propertyPtr->name, strlen(propertyPtr->name));
	puts("Enter the property location: ");
	fgets(propertyPtr->location, STR_SIZE, stdin); // dont need & because passing array which already holds address
	removeNewLine(propertyPtr->location, strlen(propertyPtr->location));
}

/*
	description: Prints the rental property information 
	parameters: constant property structure, constant unsigned int min and max nights, const int discount multiplier 
	return: nothing, console output
*/
void displayRentalPropertyInfo(const Property* propStrucPtr) {
	// %.0f is rounding to 0 decimal places, cuts down double to appear like an integer
	printf("\n\nName: %s", propStrucPtr->name);
	printf("\nLocation: %s", propStrucPtr->location);
	printf("\nRental Property can be rented for %d to %d nights.", propStrucPtr->minimumNights, propStrucPtr->maximumNights);
	printf("\n$%.0f rate a night for the first %d nights", propStrucPtr->rate, propStrucPtr->interval1Nights);
	printf("\n$%.0f discount rate a night for nights %d to %d", propStrucPtr->discount, propStrucPtr->interval1Nights + 1, propStrucPtr->interval2Nights);
	printf("\n$%.0f discount rate a night for each remaining night over %d.", (propStrucPtr->discount * propStrucPtr->discountMultiplier), propStrucPtr->interval2Nights);
}

// ================================================================================================


// input validation functions
// ================================================================================================
/*
	description: getValidInt starts the process of getting a valid integer, calls scanInt during function excecution
	parameters: constant unsigned min and max values
	return: a valid integer that is within the range of the min and max values
*/
int getValidInt(const unsigned int min, const unsigned int max) {
	char inputStr[STRING_LENGTH];
	int integer = 0;
	bool result = false;

	do {
		fgets(inputStr, STRING_LENGTH, stdin);
		removeNewLine(inputStr, strlen(inputStr));
		result = scanInt(inputStr, &integer); // check if input is integer 

		// range check integer
		if (result) {
			if ((integer < min) || (integer > max)) {
				printf("Not between %d and %d. Try again: ", min, max);
				result = false;
			}
			else {
				result = true;
			}
		}
		else {
			printf("Not valid input. Please enter an integer between %d and %d: ", min, max);
		}
	} while (!result);
	return integer;
}

/*
	description: getValidIntSentinal starts the process of getting a valid integer with a sentinal check, calls scanInt during function excecution
	parameters: constant unsigned min and max, and sentinal values
	return: a valid integer that is within the range of the min and max values and checks for the sentinal value
*/
int getValidIntSentinal(const unsigned int min, const unsigned int max, const unsigned int sentinal) {
	char inputStr[STRING_LENGTH];
	int integer = 0;
	bool result = false;

	do {
		fgets(inputStr, STRING_LENGTH, stdin);
		removeNewLine(inputStr, strlen(inputStr));
		result = scanInt(inputStr, &integer); // check if input is integer

		// range check integer with sentinal check
		if (result) {
			if (((integer < min) || (integer > max)) && (integer != sentinal)) {
				printf("Not between %d and %d. Try again: ", min, max);
				result = false;
			}
			else {
				result = true;
			}
		}
		else {
			printf("Not valid input. Please enter an integer between %d and %d: ", min, max);
		}
	} while (!result);
	return integer;
}

/*
	description: function takes in a character array (string) and removes the newline character from the end of if it exists, replacing it with null char \0
	parameters: pointer to string input, size of string
	return: nothing, modifies string directly from within function using a pointer
*/
void removeNewLine(char* stringPtr, int size) {
	if (stringPtr[(size - 1)] == '\n') { // first check to see if there is a newline char \n at end of string
		stringPtr[size - 1] = '\0'; // if there is a newline char, replace it with a null char \0
	}
}

/*
	description: function returns true if valid integer (stringInput) was read to variable (var that integerPtr points to) and false otherwise
	parameters: constant pointer to a constant character input, pointer to a constant integer 
	return: true or false based on if the input was valid, valid = true, not valid = false
*/
bool scanInt(const char* const stringInput, int* const integerPtr) {

	char* end = NULL;
	errno = 0;
	long intTest = strtol(stringInput, &end, 10); // stops when hits non-integer character
	// if any of the following conditions return true (if any of the checks do not pass), then the if statement will pass over and continu to the else. 
	// the only way to enter the if statement and return true is if every single one of the conditions are met
	if (!(end == stringInput) && !('\0' != *end) && !((LONG_MIN == intTest || LONG_MAX == intTest) && ERANGE == errno)) {
		*integerPtr = (int)intTest;
		return true;
	}
	else {
		// default return value is false. Will only return true if non of the "if" conditions were returned  
		return false;
	}
}

// ================================================================================================

// linked list functions
// ================================================================================================

/*
description: prompts user for name of property to rent, if name matches property in list, will return pointer to that property struct
parameters: takes in the head pointer to the linked list, and the length of string
return: returns a POINTER to the property the user chose
*/
Property* choosePropToRent(Property** headPropPtr, int str_len) {	
	// initialize property pointers
	Property* currentPropPtr = *headPropPtr;
	Property* propertyPickedPtr = NULL;
	// display all existing properties for user to choose from
	displayProperties(*headPropPtr);

	// will loop until propertyPickedPtr is set to not NULL (a property was successfully picked)
	do {
		char userInput[STRING_LENGTH] = { "" };
		puts("\n\nEnter the name of the property you want to rent:");
		fgets(userInput, str_len, stdin);
		removeNewLine(userInput, strlen(userInput));

		// iterate through list to find property with specified name
		while (currentPropPtr != NULL && propertyPickedPtr == NULL) {
			// check for name match
			if (strcmpCaseIgnore(currentPropPtr->name, userInput, strlen(userInput)) == 0) {
				// assign return pointer to the address of the property with the name
				propertyPickedPtr = currentPropPtr;
			}
			else {
				// advance pointer to next prop if not match
				currentPropPtr = currentPropPtr->nextPtr;
			}
		}
		// if after iterating through the loop, the propertyPickPtr was not updated (there was no match), error and restart the loop
		if (propertyPickedPtr == NULL) {
			puts("\nError, the property you entered doesn't match and existing properties. Enter the property name again.");
			// reinit the pointer values for each loop iteration
			currentPropPtr = *headPropPtr;
			propertyPickedPtr = NULL;
		}
	} while (propertyPickedPtr == NULL);
	return propertyPickedPtr;
}

/*
description: takes in two strings and a length and compares the all-lowercase values of the strings
parameters: two constant strings and an integer string length
return: integer corresponding to how the two lowercase strings compare to each other
	0 -> the strings are equal
	< 0 -> second string is greater than first
	> 0 -> first string is greather than second
*/
int strcmpCaseIgnore(const char* string1, const char* string2, int string_length) {
	char holder1[STRING_LENGTH];
	char holder2[STRING_LENGTH];

	// copies strings from string1 and string2 to holder1 and holder2 so I don't modify the original values
	strncpy(holder1, string1, sizeof(holder1));
	strncpy(holder2, string2, sizeof(holder2));

	removeNewLine(holder1, strlen(holder1));
	removeNewLine(holder2, strlen(holder2));

	// for each element in the holder strings, change to lowercase
	for (int element = 0; element < string_length; element++) {
		holder1[element] = tolower(string1[element]);
		holder2[element] = tolower(string2[element]);
	}
	// compare the values in the all-lowercase strings
	return strcmp(holder1, holder2);; // returns (-1, 0, 1 etc)
}

/*
description: this is the looping logic for adding prop to the list. prompts user for choice (y or n) and name and age of prop. calls insertProp() to
	actually insert the prop into list
parameters: a double pointer that stores an address to a prop struct which is the head pointer of a linked list, and an integer string length
return: nothing
*/
void insertPropLoop(Property** headPropPtr, int str_len) {
	// init chars
	char userResponse[] = { " " };
	char noResponse[] = { "n" };

	do {
		// initialize and declare memory for new property "node"
		Property* propToInsert = malloc(sizeof(struct property)); // NOTE: I check if the memory was allocated successfully in the "insertProp()" function, not in this function
		initializeDefaultPropertyVals(propToInsert, MIN_RENTAL_NIGHTS, MAX_RENTAL_NIGHTS, MIN_RATE, MAX_RATE, DISCOUNT_MULTIPLIER);

		// set up property info and validate here...
		puts("\nSetting up property information...");
		setUpProperty(propToInsert, STRING_LENGTH, MIN_RENTAL_NIGHTS, MAX_RENTAL_NIGHTS, MIN_RATE, MAX_RATE);

		// call insert props passing head pointer, name, age and length to actually insert the prop into list
		insertProp(headPropPtr, propToInsert, str_len); // <- checks for successfull memory allocation in this function
		
		// ask user if they want to add another property
		displayProperties(*headPropPtr);
		puts("\n\nDo you want to add another property?\n");
		char response = validateYesNo();

		strncpy(userResponse, &response, strlen(userResponse));
		removeNewLine(userResponse, strlen(userResponse));
	} while ((strcmp(userResponse, noResponse)) != 0); // check that user entered "n" or something else
}

/*
description: this function prompts the user for yes or no and returns the lowercase value of their input
parameters: none
return: a single character of the user's choice
*/
char validateYesNo() {
	char validYesNo;

	do {
		puts("Please enter (y)es or (n)o:");
		validYesNo = getchar();
		while (getchar() != '\n'); // makes sure the user entered a character and didn't just hit enter

		// convert char to lowercase
		validYesNo = tolower(validYesNo);

	} while (validYesNo != 'y' && validYesNo != 'n'); // loops until user enters y or n (Y or N allowed)

	return  validYesNo;
}

/*
description: this function takes in the head pointer to linked list and determines where to add the prop based on the name and age provided
parameters: a double pointer that stores an address to a prop struct which is the head pointer of a linked list, string prop name, integer prop age and integer string length
return: nothing, updates values in linked list via pointers on calling stack
*/
void insertProp(Property** headPropPtr, Property* propToInsert, int str_len) {
	if (propToInsert != NULL) // checks that memory was allocated successfully
	{
		// initialize trailing prop pointer and current prop pointer
		Property* lastPropPtr = NULL;
		Property* currentPropPtr = *headPropPtr;

		// check that new name is less than current prop pointer name (alphabetical order check)
		while (currentPropPtr != NULL && strcmpCaseIgnore(currentPropPtr->name, propToInsert->name, strlen(currentPropPtr->name)) < 0) {
			// next two lines insert new prop before current prop
			lastPropPtr = currentPropPtr;
			currentPropPtr = currentPropPtr->nextPtr;
		}
		if (lastPropPtr == NULL) { // if first prop in list
			*headPropPtr = propToInsert; // assign head pointer to the new prop
		}
		else { // if last prop in list
			lastPropPtr->nextPtr = propToInsert; // assign last pointer to new prop
		}
		// advance pointer to next prop
		propToInsert->nextPtr = currentPropPtr;
	}
	else {
		printf("\nNo memory to create node for name \"%s\" and rate \"%.2f\"", propToInsert->name, propToInsert->rate);
	}
}

/*
description: takes in a head pointer to linked list and returns the values of all the props
parameters: single pointer to head prop struct in linked list
return: nothing
*/
void displayProperties(Property* headPropPtr) {
	// checks that there are props in list
	if (headPropPtr != NULL)
	{
		printf("%s", "\n\nThe list of properties is: ");
		// declares new prop pointer for iteration
		Property* currentPropPtr = headPropPtr;

		// loops until current prop pointer is NULL (reached end of list)
		while (currentPropPtr != NULL)
		{
			// display and go to next node
			displayRentalPropertyInfo(currentPropPtr);
			currentPropPtr = currentPropPtr->nextPtr;
		}
	}
	// there are no elements in list to begin with
	else
	{
		puts("List is empty");
	}
}

/*
function description: this function takes in a head pointer and iterates through the linked list deleting and deallocating
memory as it goes to completely wipe the list
parameters: a double pointer that stores an address to a prop struct which is the head pointer of a linked list
return: nothing
*/
void freeRemainingProperties(Property** headPropPtr) {
	// declare iterator pointers
	Property* currentPropPtr = *headPropPtr;
	Property* nextPropPtr = NULL;

	// will loop until currentPropPtr is NULL (reaches end of list)
	while (currentPropPtr != NULL)
	{
		// set the next pointer to the value stored in the current prop's nextPtr variable (move iteration along to next prop)
		nextPropPtr = currentPropPtr->nextPtr;
		// deallocate memory for current prop
		free(currentPropPtr);
		// bring current prop pointer back to speed by setting it equal to next pointer which is now pointing to the next prop in list
		currentPropPtr = nextPropPtr;
	}
	// set the address stored in the head pointer to NULL (delete any remaining links to list)
	*headPropPtr = NULL;
	printf("%s", "\n==============================\nAll properties have been deleted.\n==============================\n");
}

/*
description: writes name and age of each prop to a file
parameters: pointer to file address and pointer to address of head linked list prop
return: nothing
*/
void writePropsToFile(Property* headPropPtr, char* folderPath, const char* categories[RENTER_SURVEY_CATEGORIES]) {
	// checks that there are props in list
	if (headPropPtr != NULL)
	{
		// declares new prop pointer for iteration
		Property* currentPropPtr = headPropPtr;

		// loops until current prop pointer is NULL (reached end of list)
		while (currentPropPtr != NULL)
		{
			FILE* writePtr = NULL;
			char delimiter = '_'; // this is what to replace instances of charToReplace with
			char charToReplace = ' '; // will search file path for spaces (' ')
			// I split the file path unto three strings
			char specificFilePath[STRING_LENGTH] = { " " }; // the base folder
			char propertyNameFilePath[STRING_LENGTH] = { " " }; // the unique property file name
			char fileExtentionType[STRING_LENGTH] = { " " }; // and the file type (.txt)

			strncpy(specificFilePath, folderPath, strlen(folderPath));
			
			strncpy(propertyNameFilePath, currentPropPtr->name, strlen(currentPropPtr->name));
			// replaces all spaces with underscores
			replaceCharsInString(propertyNameFilePath, charToReplace, delimiter, strlen(propertyNameFilePath));
			
			strncpy(fileExtentionType, ".txt", strlen(".txt"));
			
			// concatenates the three strings into one master string containing the unique file path to each property
			strcat(specificFilePath, propertyNameFilePath);
			strcat(specificFilePath, fileExtentionType);

			if ((writePtr = fopen(specificFilePath, "w")) == NULL) {
				puts("\nFile could not be opened");
			}
			else {
				// boolean flag variable
				bool stop = false;

				// loops until end of file or boolean flag is caught
				while (!feof(writePtr) && !stop) {
					// checks that props exist in linked list
					if (headPropPtr != NULL)
					{
						// loops until reached end of linked list
						if (currentPropPtr != NULL)
						{
							// write information to file
							printNightsChargesToFile(currentPropPtr, writePtr);
							printCategoriesAndRatingsToFile(currentPropPtr, writePtr, categories, RENTER_SURVEY_CATEGORIES);
						}
						// set flag to true when the linked list iteration has completed
						stop = true;
					}
				}
				// close file
				fclose(writePtr);
			}
			currentPropPtr = currentPropPtr->nextPtr;
		}
	}
	// there are no elements in list to begin with
	else
	{
		puts("List is empty");
	}
}

/*
description: writes the rating categories and rating averages for each property to a file
parameters: takes the property to display information from, a file pointer, the category array, and the total num of categories
return: nothing, prints ouput to file
*/
void printCategoriesAndRatingsToFile(Property* propStrucPtr, FILE* filePtr, const char* categories[RENTER_SURVEY_CATEGORIES], size_t totalCategories) {
	//loop to display each category horizontally
	fprintf(filePtr, "%s", "\n\nRating Categories:\t");
	for (size_t surveyCategory = 0; surveyCategory < totalCategories; ++surveyCategory)
	{
		fprintf(filePtr, "\t%zu.%s\t", surveyCategory + 1, categories[surveyCategory]);
	}
	fputs("", filePtr); // start new line of output

	// this function prints the average of each category
	// iterate through categoryAverages array and display results
	fprintf(filePtr, "%s", "\nRating averages:");
	for (size_t category = 0; category < totalCategories; category++) {
		fprintf(filePtr, "\t\t\t%.2f", propStrucPtr->categoryAverages[category]);
	}
}

/*
description: writes the basic property information to a file
parameters: takes the property to display information from and the file pointer
return: nothing, prints ouput to file
*/
void printNightsChargesToFile(Property* propStrucPtr, FILE* filePtr) {
	fprintf(filePtr, "Rental Property Report");
	fprintf(filePtr, "\nName: %s", propStrucPtr->name);
	fprintf(filePtr, "\nLocation: %s", propStrucPtr->location);
	fprintf(filePtr, "\n\nRental Property Totals");
	fprintf(filePtr, "\nRenters\t\tNights\t\tCharge\n%d\t\t%d\t\t$%.0f", propStrucPtr->numOfRenters, propStrucPtr->totalNights, propStrucPtr->totalCharges);
}

/*
description: replaces all instances of a char with another char from a string
parameters: takes a string, a char to scan for in string, a char to replace instances of other char with, and length of string
return: nothing, modifies string directly from within function
*/
void replaceCharsInString(char* stringFilePathPtr, char charToScanFor, char replaceWithChar, size_t str_len) {
	for (int element = 0; element < str_len; element++) {
		char holder[STRING_LENGTH] = { stringFilePathPtr[element] };

		if ((strcmpCaseIgnore(holder, " ", 2) == 0)) {
			strchr(stringFilePathPtr, charToScanFor);
			stringFilePathPtr[element] = replaceWithChar;
		}
	}
}

//stringFilePathPtr[element] = '_';

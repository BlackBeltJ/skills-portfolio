/*
Joshua Edwards
CS 2060 003 AirUCCS Project Iteration 2
Made on Windows 10
This program creates a system for user to view information about rental properties and
then input the number of nights they want to rent the property for and receive an estimated
charge based on the number of nights and the discount rate. The owner of the property can
also input a special value to receive back a summary of the nights rented and the total
money made.
*/

#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
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
#define LOGIN_MAX_ATTEMPTS 2
#define SENTINAL_NEG1 -1
//rental property constant ranges
#define MIN_RENTAL_NIGHTS 1
#define MAX_RENTAL_NIGHTS 14
#define MIN_RATE 1
#define MAX_RATE 1000
#define DISCOUNT_MULTIPLIER 2
// folder path
#define FOLDERPATH "C:/Users/black/Desktop/UCCS-Code-Repo/CS2060ClassCode/Project iteration code/iter 3";

// Defining property structure
// can be declared using the 'Property" tag
typedef struct property {
	unsigned int minimumNights;
	unsigned int maximumNights;
	unsigned int interval1Nights;
	unsigned int interval2Nights;
	double minimumRate;
	double maximumRate;
	double rate;
	double discount;

	unsigned int numOfRenters;
	unsigned int totalNights;
	double totalCharges;

	int rentalSurvey[VACATION_RENTERS][RENTER_SURVEY_CATEGORIES];
	double categoryAverages[RENTER_SURVEY_CATEGORIES];

	char name[STRING_LENGTH];
	char location[STRING_LENGTH];
} Property;

// Prints the rental property information 
void displayRentalPropertyInfo(const Property* propStrucPtr, const unsigned int minNights, const unsigned int maxNights, const int discountMult);
// Calculates the charge based on the number of nights rented
double calculateCharges(unsigned int nights, unsigned int interval1Nights, unsigned int interval2Nights, double rate, double discount);
// Prints the name, locationm, number of nights, and the charge
void printNightsCharges(const Property* propStrucPtr);

// login function
bool isLoggedIn(const char* correctID, const char* correctPassword, const unsigned int allowedAttempts, const unsigned int STR_LEN);
// rental mode logic function
void rentalMode(Property* currentPropertyPtr, const int minRating, const unsigned int maxRating, size_t rows, size_t columns, const unsigned int discountMultiplier, const int sentinal, const char* correctID, const char* correctPassword, const int unsigned allowedAttempts, size_t STR_LEN, const char* categories[RENTER_SURVEY_CATEGORIES]);

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

// functions for property setup (structure)
void initializeDefaultPropertyVals(Property* prop, const unsigned int minNights, const unsigned int maxNights, const double minRate, const double maxRate);
void setUpProperty(Property* propertyPtr, int STR_SIZE, const unsigned int minNights, const unsigned int maxNights, const double minRate, const double maxRate);

// main function
int main(void) {
	// rating survey initializations
	const char* surveyCategories[RENTER_SURVEY_CATEGORIES] = { "Check-in Process", "Cleanliness", "Amenities" , "cat4"};
	// initialize property structure 
	Property property1;
	// initialize default values for property struct variables so they are not assigned to junk
	initializeDefaultPropertyVals(&property1, MIN_RENTAL_NIGHTS, MAX_RENTAL_NIGHTS, MIN_RATE, MAX_RATE);

	// initialize arrays with 0s
	// 1D array
	for (size_t i = 0; i < RENTER_SURVEY_CATEGORIES; i++) {
		property1.categoryAverages[i] = 0;
	}
	// 2D array
	for (size_t row = 0; row < VACATION_RENTERS; row++) {
		for (size_t column = 0; column < RENTER_SURVEY_CATEGORIES; column++) {
			property1.rentalSurvey[row][column] = 0;
		}
	}

	// main main logic and program flow
	// log the user in, return true if user logs in correctly, false otherwise
	bool loggedIn = isLoggedIn(CORRECT_ID, CORRECT_PASSCODE, LOGIN_MAX_ATTEMPTS, STRING_LENGTH);

	// if the user logged in, enter loop
	if (loggedIn) {
		puts("\nLogin successful\n");
		setUpProperty(&property1, STRING_LENGTH, MIN_RENTAL_NIGHTS, MAX_RENTAL_NIGHTS, MIN_RATE, MAX_RATE);
		rentalMode(&property1, MINRATING, MAXRATING, VACATION_RENTERS, RENTER_SURVEY_CATEGORIES, DISCOUNT_MULTIPLIER, SENTINAL_NEG1, CORRECT_ID, CORRECT_PASSCODE, LOGIN_MAX_ATTEMPTS, STRING_LENGTH, surveyCategories);
		// after rentalMode ends, check the total nights and total charges in the struct
		if (property1.totalNights == 0 || property1.totalCharges == 0) // this is for when there are no rentals and the owner wants a summary
		{
			puts("\nThere were no rentals...Exiting program");
		}
		else // there were charges, calculate and display averages
		{
			puts("\n==========================================================\n");
			printNightsCharges(&property1);
			printCategories(surveyCategories, RENTER_SURVEY_CATEGORIES);
			calculateCategoryAverages(&property1, VACATION_RENTERS, RENTER_SURVEY_CATEGORIES);
			displaySurveyAverages(&property1, RENTER_SURVEY_CATEGORIES);
			puts("\n==========================================================\n");
		}
	}
	puts("\n\nExiting AirUCCS.");
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
	char id[] = { "Default ID"};
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
void rentalMode(Property* currentPropertyPtr, const int minRating, const unsigned int maxRating, size_t rows, size_t columns, const unsigned int discountMultiplier, const int sentinal, const char* correctID, const char* correctPassword, const int unsigned allowedAttempts, size_t STR_LEN, const char* categories[RENTER_SURVEY_CATEGORIES]) {
	bool loggedOut = false;
	// main program loop
	do {
		displayRentalPropertyInfo(currentPropertyPtr, currentPropertyPtr->minimumNights, currentPropertyPtr->maximumNights, discountMultiplier);
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
	printf("%s", "\nRental Property Report");
	printf("Name: %s", propStrucPtr->name);
	printf("Location: %s", propStrucPtr->location);
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
void initializeDefaultPropertyVals(Property* prop, const unsigned int minNights, const unsigned int maxNights, const double minRate, const double maxRate) {
	prop->minimumNights = minNights;
	prop->maximumNights = maxNights;
	prop->minimumRate = minRate;
	prop->maximumRate = maxRate;
	prop->interval1Nights = 0;
	prop->interval2Nights = 0;
	prop->rate = 0;
	prop->discount = 0;
	prop->numOfRenters = 0;
	prop->totalCharges = 0;
	prop->totalNights = 0;
	prop->numOfRenters = 0;
}

/*
	description: prompts owner for property rental information, calls input validation functions and writes input to property structure values
	parameters: property structure, string size, const unsigned int min and max nights, const double min and max rates
	return: nothing
*/
void setUpProperty(Property* propertyPtr, int STR_SIZE, const unsigned int minNights, const unsigned int maxNights, const double minRate, const double maxRate) {
	puts("Set up rental property information");

	puts("Enter the number of nights until first discount: ");
	propertyPtr->interval1Nights = getValidInt(minNights, maxNights);
	puts("Enter the number of nights until second discount: ");
	propertyPtr->interval2Nights = getValidInt(propertyPtr->interval1Nights + 1, maxNights);

	puts("Enter the nightly rental rate: ");
	propertyPtr->rate = getValidInt(minRate, maxRate);

	puts("Enter the discount: ");
	propertyPtr->discount = getValidInt(minRate, propertyPtr->rate);

	puts("Enter the property name: ");
	fgets(propertyPtr->name, STR_SIZE, stdin); // dont need & because passing array which already holds address
	puts("Enter the property location: ");
	fgets(propertyPtr->location, STR_SIZE, stdin); // dont need & because passing array which already holds address
}

/*
	description: Prints the rental property information 
	parameters: constant property structure, constant unsigned int min and max nights, const int discount multiplier 
	return: nothing, console output
*/
void displayRentalPropertyInfo(const Property* propStrucPtr, const unsigned int minNights, const unsigned int maxNights, const int discountMult) {
	// %.0f is rounding to 0 decimal places, cuts down double to appear like an integer
	printf("\nName: %s", propStrucPtr->name);
	printf("Location: %s", propStrucPtr->location);
	printf("Rental Property can be rented for %d to %d nights.", minNights, maxNights);
	printf("\n$%.0f rate a night for the first %d nights", propStrucPtr->rate, propStrucPtr->interval1Nights);
	printf("\n$%.0f discount rate a night for nights %d to %d", propStrucPtr->discount, propStrucPtr->interval1Nights + 1, propStrucPtr->interval2Nights);
	printf("\n$%.0f discount rate a night for each remaining night over %d.", (propStrucPtr->discount * discountMult), propStrucPtr->interval2Nights);
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

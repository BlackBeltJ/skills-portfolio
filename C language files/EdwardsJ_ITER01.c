/*
Joshua Edwards
CS 2060 003 AirUCCS Project Iteration 1
Made on Windows 10
This program creates a system for user to view information about rental properties and
then input the number of nights they want to rent the property for and receive an estimated
charge based on the number of nights and the discount rate. The owner of the property can
also input a special value to receive back a summary of the nights rented and the total
money made.
*/

#include <stdio.h>
#include <stdbool.h>
#include <math.h>

//Prints the rental property information 
void printRentalPropertyInfo(unsigned int minNights, unsigned int maxNights, unsigned int
	interval1Nights, unsigned int interval2Nights, double rate, double discount);

//returns only valid input from the user based on the min and max values inclusively
int getValidInt(int min, int max, int sentinal);

//Calculates the charge based on the number of nights rented
double calculateCharges(unsigned int nights, unsigned int interval1Nights, unsigned int
	interval2Nights, double rate, double discount);

//Prints the number of nights, and the charge  if there were any rentals
//Use for printing vacationer charge or for property owner summary
void printNightsCharges(unsigned int nights, double charges);

// main function
int main(void) {
	// constants
	int const SENTINAL_NEG1 = -1;
	int unsigned const MIN_RENTAL_NIGHTS = 1;
	unsigned int const MAX_RENTAL_NIGHTS = 14;
	unsigned int const INTERVAL_1_NIGHTS = 3;
	unsigned int const INTERVAL_2_NIGHTS = 6;
	double const RENTAL_RATE = 400;
	double const DISCOUNT = 50;

	unsigned int userNightInput = 0; // NOTE TO SELF: change this to unsigned throughout program
	unsigned int totalNights = 0; // NOTE TO SELF: change this to unsigned throughout program
	double totalCharges = 0;

	// this loop controls the main flow of the program and will exit when the sentinal value is entered. 
	while (userNightInput != SENTINAL_NEG1) {
		printRentalPropertyInfo(MIN_RENTAL_NIGHTS, MAX_RENTAL_NIGHTS, INTERVAL_1_NIGHTS, INTERVAL_2_NIGHTS, RENTAL_RATE, DISCOUNT);
		userNightInput = getValidInt(MIN_RENTAL_NIGHTS, MAX_RENTAL_NIGHTS, SENTINAL_NEG1);
		if (userNightInput != SENTINAL_NEG1) // the purpose of this loop is to prevent the sentinal value from being written into the totalCharges and totalNights before the while loop comes back around and breaks
		{
			double rentalCharge = calculateCharges(userNightInput, INTERVAL_1_NIGHTS, INTERVAL_2_NIGHTS, RENTAL_RATE, DISCOUNT);
			totalCharges = totalCharges + rentalCharge;
			totalNights = totalNights + userNightInput;
		}
	} // this is for when there are no rentals and the owner wants a summary
	if (totalNights == 0 || totalCharges == 0)
	{
		puts("\nThere were no rentals...Exiting program");
	}
	else
	{
		printNightsCharges(totalNights, totalCharges);
	}
}

//Prints the rental property information 
void printRentalPropertyInfo(unsigned int minNights, unsigned int maxNights, unsigned int
	interval1Nights, unsigned int interval2Nights, double rate, double discount) {
	// %.2f is rounding to 2 decimal places
	printf("\n\nRental property can be rented for %d to %d nights", minNights, maxNights);
	printf("\n%.2f rate a night for the first %d nights", rate, interval1Nights);
	printf("\n%.2f discount rate a night for nights %d to %d", discount, (interval1Nights + 1), interval2Nights);
	printf("\n%.2f discount rate a night for each remaining night over %d.\n", (discount * 2), interval2Nights);
}

//returns only valid input from the user based on the min and max values inclusively
int getValidInt(int min, int max, int sentinal) {
	int userNightInput = 0;
	int scanfReturnValue = 0;

	bool isValidInput = false;
	puts("\nEnter the number of nights you want to rent the property: ");
	
	// algorithm to check valid input
	while (isValidInput == false)
	{
		// prompt user for nights
		scanfReturnValue = scanf("%d", &userNightInput); // %d is integer specifier, & symbol is address operator and writes directly back to userNightInput
		while ((getchar() != '\n')); // clear the input buffer 

		if (scanfReturnValue == 1)
		{
			// if scanf returns a 1, that means that data was successfully read (the user entered an integer)
			// check input against acceptable range
			if (userNightInput >= min && userNightInput <= max)
			{
				isValidInput = true;
			}
			else if (userNightInput == sentinal) {
				isValidInput = true;
			}
			else
			{
				printf("Error: Not within %d and %d. Please enter the value again:", min, max);
				isValidInput = false;
			}
		}
		else
		{
			// prompt the user for an integer again if they did not enter one initially
			puts("Error: Not an integer number. Please enter the value again:\n");
			isValidInput = false;
		}
	}
	return userNightInput; // this function will always return a valid integer 
}

//Calculates the charge based on the number of nights rented
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

//Prints the number of nights, and the charge  if there were any rentals
//Use for printing  vacationer charge or for property owner summary
void printNightsCharges(unsigned int nights, double charges) {
	puts("\nRental Property Owner Total Summary");
	printf("\nNights\tCharge\n%d\t$%.0f\n", nights, charges);
}



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#define R 6371
#define TO_RAD (3.1415926536 / 180)
#define C 40075
#define NUMBER_OF_CHANCES   6
#define MAXIMUM_COUNTRIES   300

typedef struct _country {
   char name[100];
   char name_lowercase[100]; //used for search
   double longi;
   double lati;
} country;

/**
 * \fn const char* getfield(char* line, int num)
 * \brief Reading a field in a csv file
 *
 * Function that allows to read a specific field in a line. This function will alter the line, so you may want to make a backup if you still want to work on it.
 *
 * \param line line to analyze
 * \param num number of the parameter to read (starts at 1)
 * \return the parameter
 */
const char* getfield(char* line, int num) // starts at 1
{
    const char* tok;
    for (tok = strtok(line, ",");
            tok && *tok;
            tok = strtok(NULL, ",\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}

/**
 * \fn void makeLowercase(char * source)
 * \brief Convert string to lowercase
 *
 * Converts a string to lowercase, basically applying tolower() to a whole string
 *
 * \param source the string to convert
 */
void makeLowercase(char * source){
    int i;
    for (i=0;i<strlen(source);++i)
        source[i]=tolower(source[i]);
}

/**
 * \fn int parseCSV(country *countries)
 * \brief Parse list of countries
 *
 * Takes a CSV with all the relevant data (countries' names, latitude, longitude) and populate a table with the data
 *
 * \param countries table of countries to fill
 * \return number of countries
 */
int parseCSV(country *countries){
	FILE * fichier = NULL;
	fichier = fopen("countries.csv","r");
    if (fichier != NULL){
        int i;
        char buf[150],buf2[100],backup[150];
        memset(buf,0,150);
        memset(buf2,0,100);
        memset(backup,0,150);
        i=0;
        while(fgets(buf,150,fichier)!=NULL){ //get countries one by one
            strncpy(backup,buf,150); // need to make a copy because getfield() alters strings
            strcpy(buf2,getfield(buf,3)); // gets longitude
            countries[i].longi=atof(buf2);
            strcpy(buf2,getfield(backup,2)); // gets latitude
            countries[i].lati=atof(buf2);
            strncpy(countries[i].name,buf,100); // name
            makeLowercase(buf); // also storing the name in lowercase for searching
            strncpy(countries[i].name_lowercase,buf,100);
            ++i;
        } //____while(fgets)
        fclose(fichier);
        return i;
	} //____if(fichier!=NULL)
	else{
        printf("Error reading, make sure you have the file \"countries.csv\" in the same folder");
        return 0;
    }
}

/**
 * \fn int searchCountry(country *countries, int totalNumber)
 * \brief Search a country
 *
 * Allows the user to search for a country by name, and select in a list.
 *
 * \param countries table of countries to fill
 * \param totalNumber number of countries
 * \return country selected by user, 0 if nothing found
 */
int searchCountry(country *countries, int totalNumber){

    printf("\n> Please type in your search prompt:\n");
    char search[50];
    *fgets(search, 50, stdin);
    search[strlen(search)-1]='\0'; //remove \n at the end
    makeLowercase(search); // we're comparing the result to the lowercase version of the country's name
    int j=1;
    int selections[MAXIMUM_COUNTRIES]; // list of matching names, using their id
    memset(selections,0,MAXIMUM_COUNTRIES*sizeof(int));
    for(int i=0;i<totalNumber;++i){
        if(strstr(countries[i].name_lowercase,search)!=NULL){ // storing all the id of the countries which name match
            selections[j]=i;
            printf("%d. %s\n",j,countries[i].name); // displaying for the user to make their choice among the matches
            ++j;
        }
    }
    if(j==1){ // no country found
        printf("No country found, please try again\n");
        return 0;
    }
    if(j==2){ // if only one choice, we've already found the desired country, no need to confirm
        return selections[j-1];
    }
    printf("%d. Search for another country\n\n",j); //it technically doesn't matter whether the user types this number or a bigger value, but looks better that way
    printf("Please type the number of your selection: ");
    *fgets(search, 50, stdin);
    if((atoi(search)>(MAXIMUM_COUNTRIES-1)) || atoi(search)<0) //security for absurd values
        return 0;
    return selections[atoi(search)]; // simply return country id associated to the number typed
}

/**
 * \fn double dist(double lat1, double long1, double lat2, double long2)
 * \brief Process distance
 *
 * Implementation of Haversine formula I found online (https://rosettacode.org/wiki/Haversine_formula#C)
 *
 * \return Distance between countries
 */
double dist(double lat1, double long1, double lat2, double long2)
{
	double dx, dy, dz;
	long1 -= long2;
	long1 *= TO_RAD, lat1 *= TO_RAD, lat2 *= TO_RAD;

	dz = sin(lat1) - sin(lat2);
	dx = cos(long1) * cos(lat1) - cos(lat2);
	dy = sin(long1) * cos(lat1);
	return asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * R;
}

/**
 * \fn double processDistance(country *countries,int userChoice,int randomChoice)
 * \brief Process distance
 *
 * Computes the distance between the country selected by the user and the country to guess, actually calls another function found online
 *
 * \param countries table of countries to fill
 * \param userChoice Country selected by user
 * \param randomChoice Country to guess
 * \return Distance between countries
 */
double processDistance(country *countries,int userChoice,int randomChoice){ // just calling another function I found online
    return dist(countries[userChoice].lati,countries[userChoice].longi,countries[randomChoice].lati,countries[randomChoice].longi);
}

/**
 * \fn processAngle(country *countries,int userChoice,int randomChoice)
 * \brief Process angle
 *
 * Computes the angle between the country selected by the user and the country to guess, displays a general direction (North, Northeast, etc.)
 *
 * \param countries table of countries to fill
 * \param userChoice Country selected by user
 * \param randomChoice Country to guess
 */
void processAngle(country *countries,int userChoice,int randomChoice){
    double angle;
    double midPointLat=countries[randomChoice].lati; // using a midway point for trigonometry things
    double midPointLongi=countries[userChoice].longi;
    // we need adjacent and opposite to calculate tangent, opposite is easy
    double opposite = ((midPointLat-countries[userChoice].lati)*C)/360;
    //adjacent is harder because longitude circles vary in length, we compute the circumference of said circle first:
    double tmpCircle=R*cos(midPointLongi)*2*M_PI;
    if(tmpCircle<0)
        tmpCircle=-tmpCircle;
    double adj=((countries[randomChoice].longi-midPointLongi)*tmpCircle)/360;
    angle = atan(opposite/adj)*180/M_PI;
    // end of math stuff. we have our angle, now we just display the general direction corresponding to some arbitrary intervals
    if(adj==0 && opposite>0) // shouldn't happen, but just in case
        printf("North");
    else if(adj==0 && opposite<0) // shouldn't happen, but just in case
        printf("South");
    else if(adj>0 && angle > 67.5)
        printf("North");
    else if(adj>0 && angle > 22.5 && angle <= 67.5)
        printf("North-East");
    else if(adj>0 && angle > -22.5 && angle <= 22.5)
        printf("East");
    else if(adj>0 && angle > -67.5 && angle <= -22.5)
        printf("South-East");
    else if(adj>0 && angle <= -67.5)
        printf("South");
    else if(adj<0 && angle > 67.5)
        printf("South");
    else if(adj<0 && angle > 22.5 && angle <= 67.5)
        printf("South-West");
    else if(adj<0 && angle > -22.5 && angle <= 22.5)
        printf("West");
    else if(adj<0 && angle > -67.5 && angle <= -22.5)
        printf("North-West");
    else if(adj<0 && angle <= -67.5)
        printf("North");
}

int main()
{
    country countries[300]; // master table of our countries
    int totalNumber = parseCSV(countries); //populating our table of countries
    int i=0,c=0; // i will keep track of the number of mistakes, c to get text with getchar
    int userChoice=0, randomChoice=0; // countries
    char keepPlaying = 'y'; // initializing it to enter the main loop
    srand(time(NULL));

    randomChoice=1+rand()%(totalNumber); // picking a random country for our game

    printf("Welcome to the Geo Guesser game.\n"
           "Try to guess the country using the distance from other countries.\n");

    while(keepPlaying=='y' || keepPlaying=='Y') { // main game loop
        while(i<NUMBER_OF_CHANCES){
            while(userChoice==0){ // country selection
                userChoice = searchCountry(countries, totalNumber);
            }
            printf("Your choice: %s\n", countries[userChoice].name); // confirming what the user typed
            if(userChoice==randomChoice){
                printf("Correct! Congratulations!\n");
                break; // user was right, exit loop
            }
            printf("Distance: %.0f km - Direction: ", processDistance(countries,userChoice,randomChoice)); // displaying our clues to the correct country
            processAngle(countries,userChoice,randomChoice);
            ++i;
            printf("\n%d chances remaining\n",NUMBER_OF_CHANCES-i);
            userChoice=0;
        } // ______while(i<NUMBER_OF_CHANCES)
        if (i==NUMBER_OF_CHANCES) // if loop was exited because user failed, display a losing message
            printf("You lost... the country was %s\n",countries[randomChoice].name);
        i=0;
        randomChoice=1+rand()%(totalNumber); // picking another country if user wants to play again
        userChoice=0; // important to reset
        printf("Do you want to play again? (y or n)\n");
        keepPlaying = getchar();
        while ((c = getchar()) != '\n' && c != EOF) { } //empty stdin buffer
    } // _____ main game loop

    return 0;
}

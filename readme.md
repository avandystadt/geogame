# Geo game

## Introduction 

This can be summarized as my own implementation of the game Worldle found online [at this address](https://worldle.teuteuf.fr)
I didn't come up with the concept, but was interested in making my own version in order to get unlimited play (as the online version only allows one country per day), and because I was curious about how this could be implemented. As I wanted to come up with my solution, I didn't check the original code (which can be found on Github), and the only indication I used was the theory of how distances between countries would be calculated. It is completely possible that my code would end up being similar to another implementation (barring language differences), as some concepts can only be done in specific ways (for example, distance calculation).

## Game

The concept of the game is as follows: the program will randomly select a country, without telling the user what it is. Then, the user has to guess what country was picked. Each time the user makes a guess, the program will display the distance between the user-selected country and the one picked by the program, as well as the cardinal direction. Of course, the first guess will always be completely random as there are absolutely no clues provided, but after this the user has to use the two clues to guess the country in 6 tries. 6 tries may not seem much, but it's actually completely possible for most countries if you're efficient with your picks, assuming you have solid knowledge of countries' locations.

The base game normally gives the shape of the country as a clue, but there's also a mode without the shape and that's the one I wanted to reproduce.

## Details

In order to process the direction and distance, the midpoints of the countries (using latitude and longitude notations) were used. This means that even if the countries share a border, the distance will not be equal to zero. 

The reasons were as follow: 
	First of all, it is much easier to compute data this way compared to taking into account all the borders of the countries. 
	Secondly, I am not even sure such a ressource giving you all the coordinates of borders from a country in exploitable format exists, while I was able to find a .csv file containing midpoints fairly easily. 
	Finally, it is actually better for guessing as each country has its own unique distance. For example, if you used all borders for your calculation, input France and got "0km distance, Southwest", this could equally mean Andorra and Spain, since both share a border with France and are southwest from it. Meanwhile, using midpoints will give you two different values for these two different countries, leaving no room for doubt (provided the user has a good appreciation of said distance). Enclaved countries such as Vatican City would also be a problem in themselves if we didn't use midpoints.

The program also allows the user to search for a country and doesn't expect them to enter the full name directly, since many countries have very long names. It was a basic implementation using the strstr() function in C. The subtlety is that I had to convert the country names and the search prompt in lowercase to make sure there were no issues with case. 
Haversine formula was used to compute distances (and indirectly, directions), since we have to take the curve of the Earth into account.
# Carpark

The brief for this assignment is shown below. As an extension, I added a feature to animate the cars leaving the carpark.

Write a program that reads in a car park file (specified on the command line),
and shows the ‘turns’ to solve it.

Vehicles are shown as a capital letter, gaps as a full-stop and
bollards as a hash symbol. Each cars may only lie in the grid vertically or horizontally, and
must be of at least length 2. Each vehicle must have a unique uppercase letter, the first of
which must be an ‘A’, the next one be ‘B’ and so on.

We wil use a brute-force algorithm for searching over all moves for a solution :

1. You will use an array (list) of structures, each one containg the data for one car park.
Note that you may choose to store the state of each car park, not as a 2D array, but
as something that is easier to manipulate, e.g. an array of vehicles, including their
position, orientation and whether they’ve exited or not. Each approach has pros and
cons.

2. Put the initial car park into the front of this list, f=0.

3. Consider the car park at the front of the list (index f).

4. For this (parent) car park, find the resulting (child) car parks which can be created from
all the possible vehicle moves. For each of these child car parks:
• If this car park is unique (i.e. it has not been seen before in the list), add it to the
end of the list.
• If it has been seen before (a duplicate) ignore it.
• If it is the ‘final’ car park, stop and print the solution.

5. Add one to f . If there are more car parks in the list, go to step 3.
To help with printing out the correct moves, when a solution has been found, each structure
in the list will need to contain (amongst other things) a car park and a record of its parent car
park, i.e. the car park that it was created from. Since you’re using an array, this could simply
be which index of the array was the parent.
The program reads the name of the car park definition file from the command line. If it finds a
successful solution, it prints out the number of car parks that would be printed in the solution
and nothing else, or else exactly the phrase ‘No Solution?” if none can be found (as might
be the case if it is impossible, or you simply run out of memory) :

Your program :
• Must use the algorithm detailed above (which is similar to a queue and therefore a
breadth-first search). Do not use the other algorithms possible (e.g. best-first, guided,
recursive etc.); the quality of your coding is being assessed against others taking the
same approach.
• Must not use dynamic arrays or linked lists. Since car parks cannot be any larger than
20×20, you can create car parks of this size, and only use a sub-part of them if the car
park required is smaller. The list of car parks can be a fixed (large) size.
• Should check for invalid car park definition files, and report in a graceful way if there
is a problem, aborting with exit(EXIT_FAILURE) if so.
• Should not print anything else out to screen after successfully completing the search,
except that which is shown above. Automated checking will be used during marking,
and therefore the ouput must be precise.
• Should call the function test() to perform any assertion testing etc.

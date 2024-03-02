# Hello there
This is an experiment i did on the one billion line challenge that was done earlier this year: [1BLC](https://github.com/gunnarmorling/1brc).

it was made for java but since Im more proficient in cpp, i chose to do it in there and learn more about optimization and  multithreading since i didnt work with it before.
i took the iterative route to solve this challenge in order to see the effect of each modification and see how far i can get it, the challenge consists of reading a billion line from a file and parse the data from there and print it into an output file as fast as possible while using only standard language library and no external code. 
The machine that i have at the moment consists of another set of challenges on top of the mentionned restrictions; as i run a intel Penitum E2220 2.40GHz, 3gb ddr2 ram 800hz and HDD harddrive.
seeing a finish time of a couple of minutes was out the door as the hardware limit makes it hard to go that fast.

## FIRST ITERATION
[the simplest way to go about it](1st%20iteration/main.cpp) .

a main thread that reads a line from the file then process it.
i made a map<string , statistc> to keep the station data in it
the statistic struct consist of a floats min, max, and average and sum all set to 0 which is the standard way of thinking about the problem.
each loop i check if its the first input of that station to put the value as min and max if not then check the min and max.
thinking about it you can see that added a check for each iteration and that being iterated over a billion time will pile up and lose performance especially when we know that the values of the temperature will be between [-99.9,99.9] which helps a bit.
that point will be adressed in a later iteration.

i got the whole thing done in 3504 sec (~58min) with 50mb of ram usage, which was expected from the hardware and the low usage of memory in the program itself 

## SECOND ITERATION

[**multithreading saga begins**](2nd%20try/main.cpp)

simple multithreading setup which i experimented with it, here i took the algoryth and divided it up into two threads: **Reader** and **Decoder** .
from their names you can conclude that the reader read the file line by line and put them in a queue that acts as a buffer between the two threads meanwhile the Decoder waits for information in the buffer copies it into its own queue then process the lines before waiting for new data in the buffer.
the main thread waits for the Reader and Decoder to finish before writing the map created into the output file.

in this solution i had to add booleans for control to see the data if its ready or done and a condition variable that locks the decoder thread until notified from the reader thread that it can read and lock the data without having data race conflict.

i adressed the min max problem that i mentioned in the mast section and that by initiating the min and max in the struct as 200 and -200 respectively which will ensure that any data will be higher and lower than both of them.
at the same time, there was lots of copying data from the map into the thread then back which lead me to get the pointer to the value in the map and access it directly while changing it, meaning there will less copying data.

although this solution was simple the speed of the reader was much faster than the decoder and the bufferkept being filled with more and more data before being processed which lead to my ram being filled and crashing the program as it reaches 2gb.
the solution to that was throttling the reader thread by putting it to sleep for 2 seconds each time the buffer was filled with 20 million line.

not the best but respectable 

the process takes 2770 sec (~46 min) approaximately a 20% increase in speed but in the otherside there was a hge increase in ram usage due to the buffer being filled faster than the processing speed of the decoder thread 1300mb of ram was the maximum and the whole program was going from 1100mb to 1300 and back in 20 sec interval in which we can conclude that was the time it took the decoder to finish its internal buffer and copy the external one.

## THIRD ITERATION
[** 2 threads 2 furious **](3rd%20try/main.cpp)

not much has changed from the previous iteration really while i added another thread since the one decoder thread couldnt keep up with the reader and made the reader sleep for 1 sec instead of 2 once the buffer is full.
this meant that in case the buffer was copied to the decoder thread itll take less time for the reader thread to catch on that and start the process again

in reality a small change in of itself but i was surprised by the impact it had on the processing and the speed of the procedure 
it took 1731sec (~29 min) a whole 50% increase from the first try and 36% from the second. ram usuage was the same as it was peaking around 1300mb but goes as low as 900mb.
that means that the data is being processed faster but the reader speed is still higher than the decoder.

## FOURTH ITERATION
[** I remembered the basics duh **](4th%20try/main.cpp)

in this iteration i added some safety measure and refined the control to avoid deadlock when using multiple threads which happend while testing and it was paingful to see it when it stopepd.

here i added 5 decoder threads which meant now i had 5 decoder threads, 1 reader, 1 thread as the main and the last thread was there by the IDE that i used for a total of 8 divided by the two cores that my super cpu have.

surprisingly i remember the basics in this iteration and instead of copying the buffer queue i used swap operation which swap the content by swaping the pointers of the queue this meant less time copying and more time reading and decoding the ressorces.

i noted that in this iteration tho it was faster the reader thread was taking less time reading and more time waiting for the buffer to be unlocked to put data in there as 5 decoder threads were demanding and there was always one waiting for a line to be read.
this meant that i could take off the throtle from the reader thread and having a one less check to do while reading.

tho i tried using more threads upward to 16 i concluded that it was taking longer since the reader thread was being locked after each line its read since there was always a thread waiting and the buffer was always empty.

this iteration didnt have the results i expected and was a bit disappointed while testibg it.
it took 1577sec(~26min) 55% increase from base try and 10% from the third iteration .unsurprisingly the ram usage peaked around 300mb which is expected as each queue took place in memory and the litle of what the buffe could be fiiled with

## WHAT'S NEXT

right now i might not continue as i lost intrest but i got some idea where i can improve even further if i get back to it
  -reading data by chunk of memory instead of line by line 
  -each decoder thread having its own map that it fills instead of the global map and then the main thread summing them all up and printing them out which lessen the locking of map each time its decoded
  -taking a look at the standard function as the data is predictable which means i can get rid of lots of checks there
  -having a better machine :)

anyway, thank you for reading about my experiment and i hope you enjoyed it!

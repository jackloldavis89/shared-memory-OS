# shared-memory-OS
Shared memory program for KSU OS class.

I used C++ to implement this programming assignment in a Linux Docker Dev Containter in VSCode, as stated in the requirements "You are required to use C/C++ in Linux/Unix." <br />

Both process share memory in the form of a character array called "str". <br />

The producer process "produces" by using str[1] and str[2] as the first and second item of the table. <br />
It produces by turning these values to 'i' for item. <br />

The consumer process "consumes" by checking str[1] and str[2] and if they are items (=='i'). <br />
It consumes them by setting them to '-'.<br />

str[0] is used for our semaphores and our turns. Depending on weather it is 'q' for quit, 'c' for consumer, 'p' for producer, or 'l' for last.<br />
Depending on this character different semaphores are waited and posted in either program. <br />
Semaphores are made using the semaphore.h library. <br />

The produce and comsume functions are used to make the threads for our processes while main checks the turn after starting the thread. <br />
Threads are made using the pthread.h library. <br />

Only the producer knows how many items it is going to make, this is a value between 1 - 100,000 using rand().<br />
Once all the items are consumed the producer tells how many was radomized in "Total needed: " and how many was produced in "Total produced: ", while the consumer tells how many were consumed each iteration.<br />
The last turn is used for special ending functionality to finialize the "Total consumed: " in the consumer process. <br /> 

I ran this in the dev container using g++ instead of gcc, as there were issues. But I would imagine on gcc would work on a non-buggy Linux environment.  <br /> 

Ran using these inputs in the terminal:<br />
g++ producer.cpp -pthread -lrt -o producer<br />
g++ consumer.cpp -pthread -lrt -o consumer<br />
./producer & ./consumer<br />

I hope this reaches the specifications of the assignment, Jack Davis

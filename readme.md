In this assignment, you will use a double-linked list to handle a tree structure with unknown number
of children. The file system of Unix/Linux is a natural application of this structure, where the root
of the tree represents the root of the system, leaves are files or empty directories, and each internal
node represents a directory in which files and subdirectories are stored as its children. Conceptually,
it is a tree of nodes with arbitrary numbers of children, but physically, it is a double linked list,
where we use a link for the first child and the other link for next sibling. (For more details, study
Class 5 handout and Section 4.1.)
1. Link to terminal servers and login your Linux account on any machine, Bur, Aspen, or Ash.
Make directory ~/IT279/Asg2/ on your Linux account for this assignment, where ~ denotes
your home directory. Note: don’t create IT279/Asg2 in your Desktop directory.
2. Use the following Linux command to create a text file LinuxDir.txt in your ~/IT279/Asg2,
which contains the file system under /home/ of our Linux server.
ls -R /home/ > ~/IT279/Asg2/LinuxDir.txt
You will see some permission denied messages on the screen, but these error message will
not be written into LinuxDir.txt. Since every student will have different results due to
permission, don’t use other’s LinuxDir.txt, otherwise you will not get the right results.
3. Check the contents of LinuxDir.txt, which contains all directories and their contents (for
example, under /home/ad.ilstu.edu/, there are all users on the system). If you have the
permission to read, the contents of the directories will be listed. Directories are listed as
follows (this is just an example, not exactly same as your LinuxDir.txt):
/home/ad.ilstu.edu/cli2/Public:
IT279
IT327
IT427
/home/ad.ilstu.edu/cli2/Public/IT279:
Asg1
Asg2
submit279.sh
/home/ad.ilstu.edu/cli2/Public/IT279/Asg1:
myArray.h
TestAsg1.cpp
/home/ad.ilstu.edu/cli2/Public/IT279/Asg2:
SampleDir.txt
Each directory starts with a line showing the full path of the directory ended by a colon. After
the first line, each line is an item (a file or a subdirectory) in the directory. The list of the
items in the directory is ended by an empty line. If there is another directory in the system
(not necessary the subdirectory of the previous one), it will be listed in the same format. For
example, /home/ad.ilstu.edu/cli2/Public/IT279 above is another directory.


4. Write a C++ program named find.cpp, which is a simple version of Linux command find.
This find.cpp will build an internal directory tree for your LinuxDir.txt stored in the current
directory and take a directory name from the command line and search/list its contents
in the directory tree. If such directory exists in LinuxDir.txt, find.cpp will list the contents
of the directory in a cascading format on the screen.
We will compile your program as follows: (the output executable program is find)
g++ -o find find.cpp
Suppose we want to find the contents of cli2/Public/IT279 in LinuxDir.txt, we will run
the program as follows:
./find cli2/Public/IT279
The program should print the following output in a cascading format, where there are 5 space
characters indentation from the parent directory for each subdirectory..
/home/ad.ilstu.edu/cli2/Public/IT279/
IT279
Asg1
myArray.h
TestAsg1.cpp
Asg2
SampleDir.txt
submit279.sh
If there are more than one director with the same name (e.g., there are many IT279/Asg2) in
different users, the program should list all of them. If no argument is provided to find, then
the entire LinuxDir.txt directory should be listed on the screen. If the directory does not
exist, you are free to handle this situation in your way, but your program should not crash.
5. Standard output to submit: Print the results of the following in your report for submission.
./find yourULID/IT279/
./find yourULID/Public/
Use your Linux account’s user id in yourULID above.
Prepare your programs in the Linux server and submission:
ˆ As the previous assignment, run bash
bash /home/ad.ilstu.edu/cli2/Public/IT279/submit279.sh 2 peekapoo
where 2 is the assignment number, and peekapoo is secret name of your choice. Always
run the script from my public directory for the newest version. Don’t compile and run your
programs from the directory where the script places them.
ˆ Report. Follow the same guideline from the previous assignment for submission, except: for
this assignment, you have to include the required output in your report as aforementioned,
but do not include the program code and the output in the report.

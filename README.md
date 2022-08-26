# Operating-System-Project

This was a project I worked on with 3 other students at West Virginia University:
* Kollin Labowski
* Kryzstof Kudlak
* Sohan Patel
* Soren Kowalski

The final product is an operating system called AmogOS, based on the Among Us video game. The system has the following features:
* Robust error-resistant command handling (see the user manual for a full list of commands and proper syntax
* Process control blocks which support processes in a Ready, Blocked, Suspended Ready, or Suspended Blocked state
* Allows processes to enter a Running state, where they are given control over the CPU for a particular time slice
* Certain system processes are essential to the functions of the OS and cannot be killed by the user (security checks in place)
* Users may create and delete multiple "User" processes, and change their state between the 4 states of the second bullet point
* An alarm process may be created which will give the user an alert message after the time they specify has passed
* Memory management is handled by the OS, which can allocate and free blocks of memory as needed
* Input/output is achieved using interrupts to be more efficient than using busy waiting

To run the program, make sure to clone the repository and run the file startSh

**Note that this project originally exists on a Bitbucket repo, so that is why none of the commits for the project are appearing on here**

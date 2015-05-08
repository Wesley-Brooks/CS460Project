# CS460_Project
Wesley Brooks wabrook2
Christian Schuetz cmschue2

Our project was to create a linux rootkit that sends a reverse shell once the module is installed.
Our rootkit hides itself as well as the directory it's in upon being installed. In order to run 
our rootkit hard code the attacker IP address and chosen port into the call to reverse_shell() at
the bottom of the main function in reverse_shell.c. Then run 'make'. Then in a separate terminal 
(or machine) setup a netcat listener on the chosen port with 'nc -l <PORT>'. Then on the victim
machine run 'sudo insmod myMod.ko'. A reverse shell with root access will be sent to the net cat
listener. Then to remove the rootkit run 'sudo rmmod myMod.ko'.

NOTE: The code to hide the rootkit from the system is currently commented out in the first two 
lines of rootkit_init() in rootkit.c. We did this in order to be able to remove our module from our 
machines. 


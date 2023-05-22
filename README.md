The UnoReverse program functions as a simple server that attacks another computer when they connect with the server by sending large amounts of data until it crashes.

*** note that this is strictly intended for educational puposes only ***


*	The program will firstly listen on port 22 (default ssh).

*	Secondly the server will create a new HTTP client for each new connection that is established on port 22.  Then connect to the ip-api.com api and write the ip address of the client in a seperate .txt file. 

*	After this, a get request will be sent to the api in order to retreive more information about the client from port 22. This information is then logged in the logs.txt file.

*	The fourth step consists of sending as much data as possible to the client on port 22 untill it crashes, after wich it will free the allocated memory from the HTTP client.

*	And finally the server will continue listening on port 22 and repeat the entire process.



This code is written during my first year of college and can be improved.
Bugs might occur so the use of this program is at your own risk.
The project was part of the course 'Networking' during the first year of Elektronics-ICT 2022-2023 in PXL-Digital.






Written by Aerts Xander on 09/05/2023.
Updated on 16/05/2023 and rewritten on 22/05/2023

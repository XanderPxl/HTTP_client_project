The HTTP_client project attemps to create a TCP server that functions as followed:


*	Firstly it will listen on port 22

*	It accepts multiple connections

*	It finds the IP address of the client and logs that address in a sepparate file

*	The TCP server will secondly start a new HTTP client for each new connection

*	Connect to the ip-api.com api to retrieve more data about the client

*	And lastly it will send as much data as possible to the client on port 22 untill it crashes


More functions might follow but this will be the goal for now.
This code is written during my first year of college so this code won't be perfect.
Note that this project is part of the 'netwerken' tasks from PXL-digital.






Written by Aerts Xander on 09/05/2023.
Updated by Aerts Xander on 16/05/2023.

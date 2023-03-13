
The CRYPTO module runs as a client/server model.
The server can only runs on the CM0+.
The client can run both the CM7 and the CM0+.

"ClientRunsOnCM7_0" folder contains examples that the client runs on the CM7.
(CM7 client <-> IPC <-> CM0+ server)
Just copy the files (main_cm7, main_cm0plus.c and a header if exists) and re-build project
then run the both CM7 and CM0+ project. Keep CM0+ freely run.
The CM7 is the client, you can get the result and debug on the CM7.
Run_XXX function (XXX is operation dependent) does CRYPTO client operations.
Plese refer the function in each main_cm7.c.
And also please refer main_cm0plus.c for how to start the server.
Note that main_cm0plus.c are same in each folder. So you need one re-build and
run the CM0+ only once if using "ClientRunsOnCM7_0" examples continuously.


Tizen and Backendless Integration Example
=========================================
This example demonstrates the usage of the Backendless REST API to persist data from a Tizen app in the Backendless object storage. 

A video with the demonstration of the app is available at: http://www.youtube.com/watch?v=E8R_MrAShq0

Before you run the app, make sure to login to your Backendless backend with the Developer console, select the app and click the Manage icon. Copy the Application ID and REST Secret Key from the console to src\Backendless.cpp.  The following two lines in the source code must be updated:

`const static wchar_t* BACKENDLESS_APPLICATION_ID = L"****** REPLACE THIS WITH YOUR APP ID ***********";`
`const static wchar_t* BACKENDLESS_SECRET_KEY = L"****** REPLACE THIS WITH YOUR REST SECREY KEY ***********";`

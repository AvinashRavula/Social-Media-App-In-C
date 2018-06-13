# Social-Media-App-In-C

This is one of my project which i did at MissionRnD
<br/><br>
<b><font size="20">User Documentation</font></b><br/>
* This is a Social Networking App where we can make a POST which will be visible to other users.<br>
* Post is like uploading a file of any type.<br>
* Any user can view/like/comment/update/delete/download a post.<br>
* User can send and recieve messages to/from the other users connected in a common network.<br>
* User has to signup/login to access the features of this app.<br>
<br>

<b><font size="20">Developer Documentation</font></b><br/>

This is an App which is built in C Language. This app contains 3(three) modules namely Client , Server , Database.<br>
A Client can make a request, get response to/from the server. Server has a request queue which holds all the new requests from the clients
and theres is ThreadPoolManager which contains a limited number of threads to process the client requests in the request queue concurrently.<br>
I have used a File System File of size 100MB to store all the data. <br>
To access the data efficiently from the filesystem, i have divided the filesystem into 3(three)parts:<br>
1. Starting 2(two) bytes of memory to store the database size.<br>
2. Next 2(two)MB for storing the whole database.<br>
3. Remaining Memory is to store any type of data which will be organized by the above two steps.<br>
<br>
I have used Sockets to make users to get connected and have a sync of the database.<br>
Sending and recieving of messages is also done through sockets only.<br>
I have designed my own database management system for this app which is similiar to <b>HBase</b>. 

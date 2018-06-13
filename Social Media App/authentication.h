

char * signup_wrapper()
{
	clear_screen();
	char *username = prompt("Enter the username:");
	char *fullname = prompt("Enter your fullname:");
	char *email = prompt("Enter your email id:");
	char *phonenum = prompt("Enter your phone number:");
	char *password = prompt("Enter new password:");
	if (create_user(username, fullname, email, phonenum, password)){
		start_server_socket_thread();
		printf("\nUser created succeffully");
	}
	else
		printf("\nFailed to create user.");
	//free(username);
	free(fullname);
	free(email);
	free(phonenum);
	return username;
}



char *login_wrapper(database *db)
{
	clear_screen();
	char *username = prompt("Enter the username:");
	char *password = prompt("Enter the password:");
	if (login(db, username, password)){
		start_server_socket_thread();
		printf("\nSuccesfully logged in.");
		return username;
	}
	else
		printf("\nLogin Failed.");

	return NULL;
}
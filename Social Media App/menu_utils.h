
void show_authentication_menu(database *);

void show_main_menu(database *db, char *username)
{
	clear_screen();
	printf("\n--Main Menu--");
	printf("\n1.Create User");
	printf("\n2.Remove User");
	printf("\n3.Get User");
	printf("\n4.Create Post");
	printf("\n5.Comment On Post");
	printf("\n6.View Comments");
	printf("\n7.Remove Comment");
	printf("\n8.Remove Post");
	printf("\n9.View Posts");
	printf("\n10.Download File from post");
	printf("\n11.Send a message");
	printf("\n12.Message Inbox");
	printf("\n13.Sync");
	printf("\n14.Exit");
	printf("\nEnter your choice:");
	int choice;
	scanf("%d", &choice);
	switch (choice)
	{
	case 1:
		printf("\nYour have been Logged out.\n");
		username = signup_wrapper();
		if (username == NULL)
			show_authentication_menu(db);
		break;
	case 2:
		delete_user(username);
		show_authentication_menu(db);
		return;
		break;
	case 3:
		get_user_wrapper(db, username);
		break;
	case 4:
		create_post_wrapper(db, username);
		break;
	case 5:
		comment_on_post(db, username);
		break;
	case 6:
		view_comments(db);
		break;
	case 7:
		delete_comment(db);
		break;
	case 8:
		delete_post(db);
		break;
	case 9:
		view_posts_wrapper(db, username);
		break;
	case 10:
		download_file_wrapper(db);
		break;
	case 11:
		send_message_to_user(db, username);
		break;
	case 12:
		message_inbox(db, username);
		break;
	case 13:
		sync_file_store();
		break;
	case 14:
		dump_db_to_filesystem(db);
		exit(0);
		break;
	}
	press_any_key();
	show_main_menu(db, username);
}

void show_authentication_menu(database *db)
{
	clear_screen();
	printf("\n--Authentication Menu--");
	printf("\n1.Login");
	printf("\n2.New User? SignUp");
	printf("\nEnter your choice:");
	int choice;
	fflush(stdin);
	scanf("%d", &choice);
	char *username = NULL;
	switch (choice)
	{
	case 1:
		username = login_wrapper(db);
		if (username != NULL)
			show_main_menu(db, username);
		break;
	case 2:
		username = signup_wrapper();
		if (username != NULL)
			show_main_menu(db, username);
		break;
	}
	press_any_key();
	show_authentication_menu(db);
}
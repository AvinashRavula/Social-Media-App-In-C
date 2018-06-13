
int get_free_position(database *db, long size, int *key)
{
	char *query = (char *)malloc(sizeof(char) * 100);
	for (int iter = 0; iter < 100; iter++)
	{
		sprintf(query, "get 'free_space','%d'", iter + 1);
		char *data = get_wrapper(db, query);
		if (data == NULL)
			return -1;
		//printf("\n%s", data);
		int count = 0;
		char **split = splitString(data, ",", &count);
		if (count == 2)
		{
			char **sub_split = splitString(split[0], "->", &count);
			long start = atoi(sub_split[1]);
			char **sub_split2 = splitString(split[1], "->", &count);
			long end = atoi(sub_split2[1]);
			if (size < (end - start)){
				/*free(split[0]);
				free(split[1]);
				free(sub_split[0]);
				free(sub_split[1]);
				free(sub_split2[0]);
				free(sub_split2[1]);
				free(split);
				free(sub_split);
				free(sub_split2);*/
				*key = iter + 1;
				return start;
			}
		
		}
		/*free(split[0]);
		free(split[1]);
		free(split);*/
		//printf("\n%s", data);
	}
	//free(query);
	return -1;
}

void update_free_space(int key, long size)
{
	char *query = (char *)malloc(sizeof(char) * 100);
	query[0] = '\0';
	sprintf(query, "put 'free_space','%d','info:from','%ld'", key, size);
	add_request_query(query);
}

int get_new_post_id(database *db)
{
	table *my_table = searchInDatabase(db, "post");
	if (my_table != NULL)
	{
		return my_table->row_count + 1;
	}
	return -1;
}


void get_user_wrapper(database *db, char *username)
{
	clear_screen();
	if (db == NULL || username == NULL)
		return;
	char *query = (char *)malloc(sizeof(char) * 100);
	sprintf(query, "get 'profile','%s'", username);
	char *details = get_wrapper(db, query);
	printf("\n%s", details);
	return;
}

void create_post_wrapper(database *db, char *username)
{
	clear_screen();
	if (db == NULL || username == NULL)
	{
		printf("\nError:");
		return;
	}
	
	char *filepath = prompt("Enter the filename along with the path:");
	if (filepath != NULL)
	{
		FILE *fptr = fopen(filepath, "rb");
		if (fptr != NULL)
		{
			fseek(fptr, 0L, SEEK_END);
			int size = ftell(fptr);
			fseek(fptr, 0, SEEK_SET);
			char *buff = (char *)malloc(sizeof(char) * size);
			fread(buff, size, 1, fptr);
			fclose(fptr);
			int key = 0;
			long file_position = get_free_position(db, size, &key);
			if (file_position > 0)
			{
				int new_post_id = get_new_post_id(db);
				if (new_post_id == -1)
				{
					printf("\nFailed to create the post.");
					return;
				}
				fptr = fopen(FILESYSTEM, "rb+");
				fseek(fptr, file_position, SEEK_SET);
				fwrite(buff, size, 1, fptr);
				fclose(fptr);
				free(buff);
				update_free_space(key, file_position + size);
				char **queries = (char **)malloc(sizeof(char *) * 5);
				for (int i = 0; i < 5; i++)
					queries[i] = (char *)malloc(sizeof(char) * 100);
				sprintf(queries[0], "put 'post','%d','info:username','%s'", new_post_id, username);
				sprintf(queries[1], "put 'post','%d','info:filename','%s'", new_post_id, filepath);
				sprintf(queries[2], "put 'post','%d','info:filesize','%ld'", new_post_id, size);
				sprintf(queries[3], "put 'post','%d','info:address','%ld'", new_post_id, file_position);
				sprintf(queries[4], "put 'post','%d','info:likes','%d'", new_post_id, 0);
				add_request_query(queries[0]);
				add_request_query(queries[1]);
				add_request_query(queries[2]);
				add_request_query(queries[3]);
				add_request_query(queries[4]);
			}
			else
				printf("\nNo Free Space to place the post.");
			//printf("\nfilepos = %ld", file_position);
			/*fptr = fopen("test.jpg", "wb");
			fwrite(buff, size, 1, fptr);
			fclose(fptr);*/
		}
		else
			printf("\nUnable to read the file.");
	}
	else
		printf("\mError file path");
}

void view_posts_wrapper(database *db, char *username)
{

	if (db == NULL || username == NULL)
		return;
	clear_screen();
	table *my_table = searchInDatabase(db, "post");
	if (my_table != NULL)
	{
		char *key = (char *)malloc(sizeof(char) * 5);
		printf("\n%10s %-50s", "POST ID", "FILENAME");
		for (int iter = 0; iter < my_table->row_count; iter++)
		{
			itoa(iter + 1, key, 10);
			record *rec = searchInTable(my_table, key);
			if (rec != NULL)
			{
				Stack *stck;
				for (int iter2 = 0; iter2 < rec->col_count; iter2++)
				{
					stck = rec->cols[iter2];
					if (strcmp(stck->col_family_name, "info") == 0 && strcmp(stck->col_name, "filename") == 0)
					{
						printf("\n%10d %-50s", iter + 1, stck->array[stck->version_number - 1]);
					}
				}
			}
		}
	}
}

















/*
void client_workings()
{
	
	char *put_query = (char *)malloc(sizeof(char) * 50);
	strcpy(put_query, "put 'profile','1','basic info:username','avinashravula'");
	char *get_query = (char *)malloc(sizeof(char) * 50);
	strcpy(get_query, "get 'profile','1'");
	while (true)
	{
		add_request_query(put_query);
		add_request_query(get_query);

		add_request_query(put_query);
		add_request_query(get_query);

		add_request_query(put_query);
		add_request_query(get_query);

		add_request_query(put_query);
		add_request_query(get_query);

		add_request_query(put_query);
		add_request_query(get_query);

		Sleep(2000);
	}
}


void startThreads(database *db)
{
	initialize_thread_pool();
	int no_of_queries = 0;
	//char **put_query = create_profile_random_query("1", &no_of_queries);
	char **get_query = (char **)malloc(sizeof(char *) * 5);
	for (int i = 1; i < 5; i++)
	{
		get_query[i] = (char *)malloc(sizeof(char) * 20);
		sprintf(get_query[i], "get 'profile','%d'", i);
	}
	/*for (int i = 0; i < no_of_queries; i++)
	{
		printf("\n%s", put_query[i]);
		}*//*
	int get_count = 4, put_count = 7;
	pthread_t threads[2];
	/*void *arr[5];
	arr[0] = (void *)db;
	arr[1] = (void *)put_query;
	arr[2] = (void *)put_count;
	void *arr2[5];
	arr2[0] = (void *)db;
	arr2[1] = (void *)get_query;
	arr[2] = (void *)get_count;*//*
	pthread_create(&threads[0], NULL, create_save_profile, (void *)db);
	//pthread_create(&threads[1], NULL, view_profile, (void *)db);
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);
	pthread_exit(NULL);
}*/


int get_new_msg_id(database *db)
{
	table *my_table = searchInDatabase(db, "message");
	if (my_table != NULL)
	{
		return my_table->row_count + 1;
	}
	else
		printf("Error : Unknown table %s", "message");
	return -1;
}


void send_message_to_user(database *db, char *username)
{
	char *msg = prompt("Enter the message to send:");
	/*char *resp = (char *)malloc(sizeof(char) * DEFAULT_BUFLEN + 50);
	char response[DEFAULT_BUFLEN];
	strcpy(response, send_request(msg, username));*/
	char *resp = send_request(msg, username);
	printf("\nresp = %s", resp);
	if (resp != NULL)
	{
		int id = get_new_msg_id(db);
		char *query = (char *)malloc(sizeof(char) * 100);
		sprintf(query, "put 'message','%d','info:message','%s'", id, msg);
		add_request_query(query);
		char *query1 = (char *)malloc(sizeof(char) * 100);
		sprintf(query1, "put 'message','%d','info:username','%s'", id, username);
		add_request_query(query1);
	}
	//free(msg);
	//free(resp);
}

void message_inbox(database *db, char * username)
{
	int id = 1;
	while (true){
		char *query = (char *)malloc(sizeof(char) * 100);
		char *query1 = (char *)malloc(sizeof(char) * 100);
		sprintf(query, "get 'message','%d',{COLUMN->'info:username'}",id);
		sprintf(query1, "get 'message','%d',{COLUMN->'info:message'}",id);
		char *uname = get_wrapper(db, query);
		char *msg = get_wrapper(db, query1);
		if (uname == NULL || msg == NULL)
			return;
		printf("\n%15s : %s", uname, msg);
		id++;
	}
}

void sync_file_store()
{
	clear_screen();
	char *port = prompt("Enter the port number:");
	char port_num[8];
	strcpy(port_num, port);
	free(port);
	send_request("sync", NULL);

}
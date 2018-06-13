


void initialize_thread_pool()
{
	for (int i = 0; i < NO_OF_THREADS; i++){
		thread_manager.thread_state[i] = 0;
		pthread_join(thread_manager.threads[i], NULL);
	}
}


int get_new_thread()
{
	while (true){
		for (int iter = 0; iter < NO_OF_THREADS; iter++)
		{
			if (thread_manager.thread_state[iter] == 0)
				return iter;
		}
	}
	return -1;
}

void *execute_query_on_thread(void *arg)
{
	if (arg == NULL)
	{
		printf("execute_query_on_thread : NULL");
		return NULL;
	}
	void **arr = (void **)arg;
	database  *db = (database *)arr[0];
	char *queries = (char *)arr[1];
	int *thread_index = (int *)arr[2];
	if (queries == NULL || db == NULL || thread_index == NULL)
	{
		printf("\nNULL inputs");
		return NULL;
	}
	if (strlen(queries) < 5)
	{
		printf("Error in query");
		return NULL;
	}
	printf("\nexecute qquery on thread : %s" , queries);
	///printf("\n\n%s\n\n", queries);
	pthread_mutex_lock(&lock);
	process_query(db, queries);
	pthread_mutex_unlock(&lock);
	//printf("\nThread executed");
	thread_manager.thread_state[*thread_index] = 0;
	return NULL;
}


void execute_queries(database *db, char **queries, int no_of_queries)
{

	for (int i = 0; i < no_of_queries; i++)
	{
		int thread_index = get_new_thread();
		if (thread_index != -1){
			void *arr[5];
			arr[0] = (void *)db;
			arr[1] = (void *)queries[i];
			//printf("\n----  %s", queries[i]);
			thread_manager.thread_state[thread_index] = 1;
			pthread_create(&thread_manager.threads[thread_index], NULL, execute_query_on_thread, (void*)arr);
		//	pthread_detach(thread_manager.threads[thread_index]);
			//printf("\n%u\n", thread_manager.threads[thread_index]);
			thread_manager.thread_state[thread_index] = 0;
			//free(queries[i]);
		}
		//free(queries);
	}
}


void add_request_query(char *query)
{
	if (query == NULL)
	{
		printf("\nadd_request_query : NULL");
		return;
	}
	else if (strlen(query) < 5)
	{
		printf("\nadd_request_query : Wrong query : %s",query);
		return;
	}
	//char *new_query = (char *)malloc(sizeof(char) * 100);
	//strcpy(new_query, query);

	enqueue(my_queue, query);
	//request_queue.push(query);
}


void *request_processor(void *arg)
{
	if (arg == NULL){
		printf("\nrequest processor : NULL");
	}
	//void **arr = (void **)arg;
	//database *db = (database *)arr[0];
	//pthread_rwlock_t *p = (pthread_rwlock_t *)arr[1];
	database *db = (database *)arg;
	/*if (pthread_rwlock_rdlock(p) != 0) {
		perror("reader_thread: pthread_rwlock_rdlock error");
		exit(__LINE__);
	}*/
	char *requested_query = NULL;
	while (true)
	{
		
		if (!isEmpty(my_queue))
		{
			
			int thread_index = get_new_thread();
			if (thread_index != -1){
				pthread_mutex_lock(&lock);
				requested_query = dequeue(my_queue);
				pthread_mutex_unlock(&lock);
				printf("\n - request queue : %s", requested_query);
				void *arr[5];
				arr[0] = (void *)db;
				arr[1] = (void *)requested_query;
				arr[2] = (void *)&thread_index;
				//printf("\n----  %s", requested_query);
				thread_manager.thread_state[thread_index] = 1;
				pthread_create(&thread_manager.threads[thread_index], NULL, execute_query_on_thread, (void*)arr);
				pthread_join(thread_manager.threads[thread_index], NULL);
				//	pthread_detach(thread_manager.threads[thread_index]);
				//printf("\n%u\n", thread_manager.threads[thread_index]);

				//free(queries[i]);
			}
			//request_queue.pop();
		}
		/*else
			printf("\nRequest Queue is empty.");*/
	}
}

void start_server_request_processor_thread(database *db)
{
	initialize_thread_pool();
	my_queue = createQueue(100);
	pthread_t thread;
	void *arr[5];
	arr[0] = (void *)db;
	//arr[1] = &lock_rw;
	pthread_create(&thread, NULL, request_processor, (void *)db);
	//printf("\nServer Request processor thread created successfully");
}




bool create_user(char *username, char *fullname, char *email, char *phonenum, char *password)
{
	if (username == NULL || fullname == NULL || email == NULL || phonenum == NULL)
		return false;
	char **queries = (char **)malloc(sizeof(char *) * 5);
	for (int iter = 0; iter < 5; iter++)
		queries[iter] = (char *)malloc(sizeof(char) * 100);
	sprintf(queries[0], "put 'profile','%s','basic info:username','%s'", username, username);
	sprintf(queries[1], "put 'profile','%s','basic info:fullname','%s'", username, fullname);
	sprintf(queries[2], "put 'profile','%s','contact info:email','%s'", username, email);
	sprintf(queries[3], "put 'profile','%s','contact info:phonenum','%s'", username, phonenum);
	sprintf(queries[4], "put 'profile','%s','security info:password','%s'", username, password);
	//char *recvBuf = (char *)malloc(sizeof(char) * DEFAULT_BUFLEN);
	/*for (int i = 0; i < 5; i++)
	{
		send_request("localhost", queries[i], recvBuf);
	}*/

	add_request_query(queries[0]);
	add_request_query(queries[1]);
	add_request_query(queries[2]);
	add_request_query(queries[3]);
	add_request_query(queries[4]);
	return true;
}


bool login(database *db,char *username, char *password)
{
	if (username == NULL || password == NULL)
		return false;
	char *query;
	query = (char *)malloc(sizeof(char) * 100);
	sprintf(query, "get 'profile','%s',{COLUMN->'security info:password'}", username);
	char *pwd = get_wrapper(db, query);
	printf("\n%s", pwd);
	if (pwd == NULL)
		return false;
	if (strcmp(password, pwd) == 0)
		return true;
	return false;
}

void dump_db_to_filesystem(database *db)
{
	process_query(db, "export");
	char filename[50];
	strcpy(filename, db->db_name);
	strcat(filename, ".nosql");
	FILE *fptr = fopen(filename, "rb");
	if (fptr != NULL)
	{
		fseek(fptr, 0, SEEK_END);
		int size = ftell(fptr);
		fseek(fptr, 0, SEEK_SET);
		char *buff = (char *)malloc(sizeof(char) * size);
		fread(buff, size, 1, fptr);
		FILE *fptr2 = fopen(FILESYSTEM, "rb+");
		if (fptr2 != NULL)
		{
			fwrite(&size, sizeof(int), 1, fptr2);
			fwrite(buff, size, 1, fptr2);
			fclose(fptr2);
		}
		fclose(fptr);
	}
}

void load_db_from_filesystem(database *db)
{
	FILE *fptr = fopen(FILESYSTEM, "rb+");
	if (fptr != NULL)
	{
		int size;
		fread(&size, sizeof(int), 1, fptr);
		//printf("\n%d", size);
		char *buff = (char *)malloc(sizeof(char) * size);
		fread(buff, size, 1, fptr);
		//printf("\n%s", buff);	
		fclose(fptr);
		char filename[50];
		strcpy(filename, db->db_name);
		strcat(filename, ".nosql");
		FILE *fptr = fopen(filename, "w");
		if (fptr != NULL)
		{
			fputs(buff, fptr);
			fclose(fptr);
			char *query = (char *)malloc(sizeof(char) * 20);
			query[0] = '\0';
			sprintf(query, "import %s.nosql", db->db_name);
			add_request_query(query);
		}
	}

}

void download_file_wrapper(database *db)
{
	clear_screen();
	char *postid = prompt("Enter the post id:");
	if (postid != NULL)
	{
		table *my_table = searchInDatabase(db, "post");
		if (my_table != NULL){
			record *rec = searchInTable(my_table, postid);
			if (rec != NULL)
			{
				char *ch_address = NULL, *ch_size = NULL,*ch_filename = NULL;
				for (int iter = 0; iter < rec->col_count; iter++)
				{
					Stack *stck = rec->cols[iter];
					if (strcmp(stck->col_name, "filename") == 0){
						ch_filename = stck->array[stck->version_number - 1];
					}
					else if (strcmp(stck->col_name, "address") == 0){
						ch_address = stck->array[stck->version_number - 1];
					}
					else if (strcmp(stck->col_name, "filesize") == 0){
						ch_size = stck->array[stck->version_number - 1];
					}
				}
				if (ch_address != NULL & ch_size != NULL)
				{
					long address = atol(ch_address);
					int size = atoi(ch_size);
					FILE *fptr = fopen(FILESYSTEM, "rb");
					if (fptr != NULL)
					{
						fseek(fptr, address, SEEK_SET); 
						char *buff = (char *)malloc(sizeof(char) *size);
						fread(buff, size, 1, fptr);
						FILE *fptr2 = fopen(ch_filename, "wb");
						if (fptr2 != NULL)
						{
							fwrite(buff, size, 1, fptr2);
							fclose(fptr2);
							printf("\nFile downloaded to local disk : %s", ch_filename);
						}
						fclose(fptr);
					}
				}
			}
		}
	}
}


void comment_on_post(database *db, char *username)
{
	clear_screen();
	char *postid = prompt("Enter the postid:");
	if (postid != NULL)
	{
		table *my_table = searchInDatabase(db, "post");
		if (my_table != NULL)
		{
			record *rec = searchInTable(my_table, postid);
			if (rec != NULL)
			{
				char *comment = prompt("Enter the comment:");
				if (comment != NULL)
				{
					char *query1 = (char *)malloc(sizeof(char) * 100);
					query1[0] = '\0';
					sprintf(query1, "put 'post','%s','basic info:comments',+'%s'", postid, comment);
					add_request_query(query1);
					printf("\nCommenting on the post...");
					printf("\nCommented.");
				}
			}
			
		}
	}
	else
		printf("\nNo Post Found with Id : %s", postid);
}

void view_comments(database *db)
{
	clear_screen();
	char *postid = prompt("Enter the post id:");
	if (postid != NULL)
	{
		char *query = (char *)malloc(sizeof(char) * 100);
		query[0] = '\0';
		sprintf(query, "get 'post','%s',{COLUMN->'basic info:comments'}", postid);
		char  *res = get_wrapper(db, query);
		if (res != NULL)
		{
			int len = 0;
			char **split = splitString(res, ";", &len);
			for (int iter = 0; iter < len; iter++)
				printf("\n%d  :  %s", iter+ 1, split[iter]);
		}
		free(query);
		
	}
}

void delete_comment(database *db)
{
	clear_screen();
	char *postid = prompt("Enter the postid : ");
	char *comment_no = prompt("Enter the comment number:");
	if (comment_no != NULL && postid != NULL)
	{
		char *query = (char *)malloc(sizeof(char) * 100);
		query[0] = '\0';
		sprintf(query, "get 'post','%s',{COLUMN->'basic info:comments'}", postid);
		char *al_coments = get_wrapper(db, query);
		if (al_coments != NULL)
		{
			int len = 0;
			int comment_num = atoi(comment_no);
			char **split = splitString(al_coments, ";", &len);
			char *updated_comment = (char *)malloc(sizeof(char) * 100);
			updated_comment[0] = '\0';
			bool flag = false;
			for (int iter = 0; iter < len; iter++)
			{
				if (flag)
					strcat(updated_comment, ",");
				if (iter + 1 != comment_num){
					strcat(updated_comment, split[iter]);
					flag = true;
				}
				
			}
			sprintf(query, "put 'post','%s','basic info:comments','%s'", postid, updated_comment);
			add_request_query(query);
		}
	}
}

void delete_post(database *db)
{
	clear_screen();
	char *postid = prompt("Enter the post id:");
	if (postid != NULL)
	{
		char *query12 = (char *)malloc(sizeof(char) * 100);
		query12[0] = '\0';
		sprintf(query12, "get 'post','%s',{COLUMN->'info:filesize'}", postid);
		char *filesize = get_wrapper(db, query12);
		if (filesize != NULL){
			char *query1 = (char *)malloc(sizeof(char) * 100);
			query1[0] = '\0';
			sprintf(query1, "get 'post','%s',{COLUMN->'info:address'}", postid);
			char *fileaddress = get_wrapper(db, query1);
			if (fileaddress != NULL){
				char *query2 = (char *)malloc(sizeof(char) * 100);
				query2[0] = '\0';
				sprintf(query2, "put 'free_space','%s','info:from','%s'}", postid, fileaddress);
				add_request_query(query2);
				char *query3 = (char *)malloc(sizeof(char) * 100);
				query3[0] = '\0';
				sprintf(query3, "put 'free_space','%s','info:to','%ld'}", postid, atoi(fileaddress) + atoi(filesize));
				add_request_query(query3);
				char *query = (char *)malloc(sizeof(char) * 100);
				query[0] = '\0';
				sprintf(query, "delete 'post','%s'", postid);
				add_request_query(query);
				printf("\nDeleting Post and its components.....");
				printf("\nDeleted");
			}
			else
				printf("\nFile address is null");
		}
		else
			printf("\nfile size is null");
	}
}

void delete_user(char *username)
{
	char *query = (char *)malloc(sizeof(char) * 100);
	query[0] = '\0';
	sprintf(query, "delete 'profile','%s'", username);
	add_request_query(query);
	printf("\nRemoving User...");
	printf("\nUser Removed...");
	getch();
}

char* process_request(database *db, char request[], int len, SOCKET ClientSocket)
{
	clear_screen();
	if (strcmp(request, "sync") == 0)
	{
		printf("\nSync Requested.");
		FILE *fptr = fopen(FILESYSTEM, "rb+");
		if (fptr != NULL)
		{
			char send_buf[DEFAULT_BUFLEN];
			int size;
			fread(&size, sizeof(int), 1, fptr);
			fseek(fptr, 0, SEEK_SET);
			fread(send_buf, sizeof(int), 1, fptr);
			//printf("\n%d", size);
			int i = 0;
			for (i = sizeof(int); i < size; i++){

				if (i == DEFAULT_BUFLEN - 1){
					send(ClientSocket, send_buf, i, 0);
					i = -1;
				}
				send_buf[i] = fgetc(fptr);
			}
			if (i > 0){
				int iSendResult = send(ClientSocket, send_buf, i, 0);
				if (iSendResult == -1) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					//	return NULL;
				}
			}
			//printf("\n%s", buff);	
			fclose(fptr);
			return send_buf;
		}
	}
	else
	{
		int col_index = find_char(request, ':');
		char username[50], message[DEFAULT_BUFLEN];
		strcpy(username, subString(request, 0, col_index - 1));
		strcpy(message, subString(request, col_index + 1, len));
		printf("\nUsername = %s", username);
		printf("\nMessage = %s", message);
		int msg_id = get_new_msg_id(db);
		char *query = (char *)malloc(sizeof(char) * 100);
		sprintf(query, "put 'message','%d','info:username','%s'", msg_id, username);
		add_request_query(query);
		char *query1 = (char *)malloc(sizeof(char) * 100);
		sprintf(query1, "put 'message','%d','info:username','%s'", msg_id, username);
		add_request_query(query1);
	}
	int iSendResult = send(ClientSocket, request, len, 0);
	if (iSendResult == -1) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		//	return NULL;
	}
	return NULL;
}
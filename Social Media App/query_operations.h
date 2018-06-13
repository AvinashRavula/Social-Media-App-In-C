
int get_command_index(char *query)
{
	int iter = 0;
	for (iter = 0; query[iter] != '\0' && query[iter] != ' '; iter++);
	char *command = sub_string(query, 0, iter - 1);
	for (int iter = 0; iter < command_len; iter++){
		if (strcmp(command, commands[iter]) == 0)
			return iter + 1;
	}
	return -1;
}

void create_table_wrapper(database *db, char *query)
{
	char *sub_query = sub_string(query, 7, strlen(query) - 1);
	int len = 0;
	char **split_query = splitString(sub_query, ",",&len);
	char *table_name = NULL, **col_families;
	col_families = (char **)malloc(sizeof(char *) * len);
	for (int iter = 0; iter < len; iter++)
	{
		if (iter == 0)
			table_name = sub_string(split_query[iter],1,strlen(split_query[iter]) - 2);
		else
			col_families[iter - 1] = sub_string(split_query[iter], 1, strlen(split_query[iter]) - 2);
	}
	table *new_table = create_table(table_name, col_families, len - 1);
	if (insert_into_database(db, new_table));
		/*printf("\nTable Created Successfully.");
	else
		printf("\nTable Creation Failed.");*/
}

void list_wrapper(database *db)
{
	list(db);
}

void disable_wrapper(database *db, char *query)
{
	char *table_name = sub_string(query, 8, strlen(query) - 2);
	if (disable(db, table_name))
		printf("\nDisabled");
	else
		printf("\n Failed to disable.");
}

void scan_wrapper(database *db, char *query)
{
	char *sub_query = sub_string(query, 5, strlen(query) - 1);
	int len = 0;
	char **split_query = splitString(sub_query, ",", &len);
	if (len != 3){
		printf("\nExpected 3 parameters.");
		return;
	}
	char *table_name, *start_id, *end_id;
	table_name = sub_string(split_query[0], 1, strlen(split_query[0]) - 1);
	start_id = sub_string(split_query[1], 1, strlen(split_query[1]) - 1);
	end_id = sub_string(split_query[2], 1, strlen(split_query[2]) - 1);
	scan(db, table_name,start_id,end_id);
}

void is_disabled_wrapper(database *db, char *query)
{
	char *table_name = sub_string(query, 13 ,strlen(query) - 2);
	is_disabled(db, table_name);
}

void enable_wrapper(database *db, char *query)
{
	char *table_name = sub_string(query, 8, strlen(query) - 2);
	if (disable(db, table_name))
		printf("\nEnabled");
	else
		printf("\n Failed to enable.");
}

void put_wrapper(database *db, char *query)
{
	char *sub_query = sub_string(query, 4, strlen(query) - 1);
	char *table_name,*value,*col_family_col_name,*row_name;
	int len = 0;
	char **split_query = splitString(sub_query, ",", &len);
	if (len != 4)
		return;
	table_name = sub_string(split_query[0], 1, strlen(split_query[0]) - 2);
	row_name = sub_string(split_query[1], 1, strlen(split_query[1]) - 2);
	col_family_col_name = sub_string(split_query[2], 1, strlen(split_query[2]) - 2);
	value = split_query[3];
	if (put(db, table_name, row_name, col_family_col_name, value));
		/*printf("\nRow Inserted.");
	else
		printf("\nFailed to insert.");*/
}

char * get_wrapper(database *db, char *query)
{
	char *sub_query = sub_string(query, 4, strlen(query) - 1);
	char *table_name, *row_name;
	int len = 0;
	char **split_query = splitString(sub_query, ",", &len);
	if (len == 2){
		table_name = sub_string(split_query[0], 1, strlen(split_query[0]) - 2);
		row_name = sub_string(split_query[1], 1, strlen(split_query[1]) - 2);
		return get(db, table_name, row_name, NULL, NULL);
	}
	else if (len == 3)
	{
		table_name = sub_string(split_query[0], 1, strlen(split_query[0]) - 2);
		row_name = sub_string(split_query[1], 1, strlen(split_query[1]) - 2);
		int count = 0;
		char **split = splitString(split_query[2], ":", &count);
		if (count == 2)
		{
			char *col_family = sub_string(split[0], 10, strlen(split[0]) - 1);
			char *col_name = sub_string(split[1], 0, strlen(split[1]) - 3);
			return get(db, table_name, row_name, col_family, col_name);
		}
	}
	return NULL;
}


void delete_wrapper(database *db, char *query)
{
	char *sub_query = sub_string(query, 7, strlen(query) - 1);
	char *table_name, *row_name;
	int len = 0;
	char **split_query = splitString(sub_query, ",", &len);
	if (len != 2)
		return;
	table_name = sub_string(split_query[0], 1, strlen(split_query[0]) - 2);
	row_name = sub_string(split_query[1], 1, strlen(split_query[1]) - 2);
	delte(db, table_name, row_name);
}


int count_wrapper(database *db, char *query)
{
	char *table_name = sub_string(query, 6, strlen(query) - 1);
	if (table_name != NULL)
	{
		table *my_table = searchInDatabase(db, table_name);
		if (my_table != NULL)
		{
			return my_table->row_count;
		}
		else
			printf("Error : Unknown table %s", table_name);
	}
	return -1;
}


void process_query(database *db, char *query)
{
	if (db == NULL || query == NULL)
		return;
	int command_index = get_command_index(query);
	switch (command_index)
	{
	case 1:
		create_table_wrapper(db,query);
		break;
	case 2:
		list_wrapper(db);
		break;
	case 3:
		disable_wrapper(db,query);
		break;
	case 4:
		scan_wrapper(db, query);
		break;
	case 5:
		is_disabled_wrapper(db, query);
		break;
	case 6:
		enable_wrapper(db, query);
		break;
	case 7:
		put_wrapper(db, query);
		break;
	case 8:
		get_wrapper(db, query);
		break;
	case 9:
		delete_wrapper(db, query);
		break;
	case 10:
		export_to_file(db);
		break;
	case 11:
		import_from_file(db, query);
		break;
	case 12:
		exit(0);
		break;
	case 13:
		count_wrapper(db, query);
		break;
	/*default:
		//printf("\nCommand not found.");
		break;*/
	}
}
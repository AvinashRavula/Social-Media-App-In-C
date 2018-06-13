void process_query(database *, char *);


void list(database *db)
{
	for (int iter = 0; iter < db->db_size; iter++)
		if (db->tables[iter] != NULL)
			printf("\n%s", db->tables[iter]->table_name);
}

bool disable(database *db, char *table_name)
{
	table *my_table = searchInDatabase(db, table_name);
	if (my_table != NULL)
		my_table->disabled = true;
	return my_table->disabled;
}


char * get(database *db, char *table_name, char *row_name, char *col_family, char *col_name)
{
	if (db == NULL || table_name == NULL || row_name == NULL)
		return NULL;
	table *my_table = searchInDatabase(db, table_name);
	if (my_table != NULL)
	{
		record *rec = searchInTable(my_table, row_name);
		if (rec != NULL)
		{
			Stack *stck = NULL;
			bool flag = false;
			char *res = (char *)malloc(sizeof(char) * 500);
			res[0] = '\0';
			for (int iter = 0; iter < rec->col_count; iter++)
			{
				stck = rec->cols[iter];
				if (col_family != NULL && col_name != NULL){
					if (strcmp(stck->col_name, col_name) == 0 & strcmp(col_family, stck->col_family_name) == 0)
					{
						return stck->array[stck->version_number - 1];
					}
				}
				else{
					if (flag)
						strcat(res, ",");
					char value[100];
					sprintf(value, "{%s:%s->%s}", stck->col_family_name, stck->col_name, stck->array[stck->version_number - 1]);
					strcat(res, value);
					flag = true;
				}
				//printf("\n%s : %s version = %d value = %s", stck->col_family_name, stck->col_name, stck->version_number, stck->array[stck->version_number-1]);
			}
			return flag ? res : NULL;
		}
		else
			printf("Error: Record not found with row id : %s", row_name);
	}
	else
		printf("Error : Unknown table %s", table_name);
	return NULL;
}

void print_records_in_range(database *db, table *my_table, char *start_id, char *end_id)
{
	if (my_table != NULL && start_id != NULL && end_id != NULL)
	{
		clear_screen();
		record *item;
		if (my_table->row_count < 1)
		{
			printErrorWithNullInputs();
			press_any_key();
			return;
		}
		int i = 0;
		int char_sum_of_start_id = sumOfChars(start_id);
		int char_sum_of_end_id = sumOfChars(end_id);
		for (i = 0; i<my_table->max_rows; i++) {
			if (my_table->rows[i] != NULL){
				item = my_table->rows[i];
				int char_sum_of_key = sumOfChars(item->key);
				if (char_sum_of_key >= char_sum_of_start_id && char_sum_of_key <= char_sum_of_end_id)
					get(db, my_table->table_name, item->key, NULL, NULL);
				item = my_table->rows[i]->next;
				while (item != NULL)
				{
					int char_sum_of_key = sumOfChars(item->key);
					if (char_sum_of_key >= char_sum_of_start_id && char_sum_of_key <= char_sum_of_end_id)
						get(db, my_table->table_name, item->key, NULL, NULL);
					item = item->next;
				}
			}
		}
	}
}

void scan(database *db, char *table_name, char *start_id, char *end_id)
{
	if (db == NULL || table_name == NULL || start_id == NULL || end_id == NULL)
		return;
	table *my_table = searchInDatabase(db, table_name);
	if (my_table != NULL)
	{
		if (my_table->disabled)
		{
			printf("\nError : %s is disabled.");
			return;
		}
		print_records_in_range(db, my_table, start_id, end_id);
	}
}

void is_disabled(database *db, char *table_name)
{
	table *my_table = searchInDatabase(db, table_name);
	if (my_table != NULL)
	{
		if (my_table->disabled)
			printf("\ntrue");
		else
			printf("\nfalse");
	}
	else
		printf("\nError : table not found");
}

bool enable(database *db, char *table_name)
{
	table *my_table = searchInDatabase(db, table_name);
	if (my_table != NULL)
		my_table->disabled = false;
	return !my_table->disabled;
}

bool put(database *db, char *table_name, char *row_name, char *col_family_col_name, char *value)
{
	table *my_table = searchInDatabase(db, table_name);
	if (my_table != NULL)
		insert_or_update_into_table(my_table, row_name, col_family_col_name, value);
	if (!commit(db))
		printf("\nFailed to commit.");
	return true;
}


void delte(database *db, char *table_name, char *row_name)
{
	if (db == NULL || table_name == NULL || row_name == NULL)
		return;
	table *my_table = searchInDatabase(db, table_name);
	if (my_table != NULL)
	{
		if (deleteInTable(my_table, row_name))
			printf("\nDeleted Successfully.");
		else
			printf("\nFailed to delete the record.");
	}
	else
		printf("Error : Unknown table %s", table_name);
}


void write_table_to_file(table *tb,FILE *fptr)
{
	char query[100];
	sprintf(query, "create '%s'", tb->table_name);
	for (int iter2 = 0; iter2 < tb->col_families_len; iter2++)
	{
		strcat(query, ",'");
		strcat(query, tb->col_families[iter2]);
		strcat(query, "'");
	}
	strcat(query, "\n");
	fprintf(fptr, query);
}

void write_records_to_file(table *tb, FILE *fptr)
{
	char query[100],temp[100];
	sprintf(query, "put '%s'", tb->table_name);
	strcpy(temp, query);
	record *rec = NULL;
	Stack *my_stack = NULL;
	for (int iter = 0; iter < tb->max_rows; iter++)
	{
		if (tb->rows[iter] != NULL)
		{
			strcpy(temp, query);
			strcat(temp, ",'");
			strcat(temp, tb->rows[iter]->key);
			strcat(temp, "'");
			rec = tb->rows[iter];
			char temp2[100];
			for (int iter2 = 0; iter2 < rec->col_count; iter2++)
			{
				strcpy(temp2, temp);
				my_stack = rec->cols[iter2];
				strcat(temp2, ",'");
				strcat(temp2, my_stack->col_family_name);
				strcat(temp2, ":");
				strcat(temp2, my_stack->col_name);
				strcat(temp2, "'");
				char temp3[100];
				for (int iter3 = 0; iter3 < my_stack->version_number; iter3++)
				{
					strcpy(temp3, temp2);
					strcat(temp3, ",'");
					strcat(temp3, my_stack->array[iter3]);
					strcat(temp3, "'\n");
					fprintf(fptr, temp3);
				}
			}
		}
	}
}

void export_to_file(database *db)
{
	char filename[50];
	strcpy(filename, db->db_name);
	strcat(filename, ".nosql");
	int i = 0;
	FILE *fptr;
	/*fptr = fopen(filename, "r");
	while (fptr != NULL)
	{
		strcpy(filename, db->db_name);
		char temp[20];
		sprintf(temp, "(%d).nosql", i);
		strcat(filename, temp);
		fptr = fopen(filename, "r");
		i++;
	}*/
	fptr = fopen(filename, "w");
	table *tb;
	char *query = (char *)malloc(sizeof(char) * 100);
	query[0] = '\0';
	for (int iter = 0; iter < db->db_size; iter++)
	{
		tb = db->tables[iter];
		if (tb != NULL)
		{
			write_table_to_file(tb, fptr);
			write_records_to_file(tb, fptr);
		}
	}
	//printf("\nExported to %s",filename);
	fclose(fptr);
}

void import_from_file(database *db, char *query)
{
	if (db == NULL || query == NULL)
		return;
	char *filename = sub_string(query, 7, strlen(query) - 1);
	FILE *fptr = fopen(filename, "r");
	if (fptr != NULL)
	{
		char query[200];
		while (!feof(fptr))
		{
			fgets(query, 200, fptr);
			char *my_query = sub_string(query, 0, strlen(query) - 3);
			process_query(db, my_query);
		}
		fclose(fptr);
	}

}
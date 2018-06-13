void insertIntoTransaction(char *, char *, char *);

void printErrorWithNullInputs()
{
	//printf("\nNo Records\n");
}

int sumOfChars(char *str)
{
	int len = strlen(str);
	int sum = 0;
	for (int i = 0; i < len; i++)
	{
		sum += str[i];
	}
	return sum;
}

// returns -1 if NULL input is given else returns the generated hashCode with key and size
int hashCode(char *key, int size) {
	if (key == NULL)
		return -1;
	return sumOfChars(key) % size;
}

char *generate_key(int num)
{
	char num_char[5];
	itoa(num, num_char, 10);
	int len = strlen(num_char);
	char *key = (char *)malloc(sizeof(char) * ((len * 2) + 1));
	key[0] = '\0';
	strcat(key, num_char);
	strcat(key, num_char);
	return key;
}


record * searchInTable(table *my_table, char  *key)
{
	if (my_table->row_count < 1 || key == NULL)
	{
		printErrorWithNullInputs();
		return NULL;
	}
	int hashIndex = hashCode(key, my_table->max_rows);
	if (hashIndex == -1)
	{
		//printErrorProcessingHashCode();
		return NULL;
	}
	record *item = my_table->rows[hashIndex];
	while (item != NULL) {
		if (strcmp(item->key, key) == 0)
			return item;
		item = item->next;
	}
	return NULL;
}

bool insert_values_into_record(record *rec, char *col_family_col_name, char *value, char *table_name,char *key)
{
	if (rec == NULL || col_family_col_name == NULL || value == NULL)
		return false;
	Stack *my_stack = NULL;
	int len = 0;
	char **split_col_family_col_name = splitString(col_family_col_name, ":", &len);
	if (len != 2)
		return false;
	
	my_stack = createStack(split_col_family_col_name[0], split_col_family_col_name[1], 100);
	push(my_stack, value);
	my_stack->version_number = 1;
	rec->cols[rec->col_count] = my_stack;
	rec->col_count++;
	rec->key = key;
	return true;
}


bool update_values_into_record(record *rec, char *col_family_col_name, char *value, char *table_name, char *key)
{
	if (rec == NULL || col_family_col_name == NULL || value == NULL)
		return false;
	Stack *my_stack = NULL;
	int len = 0;
	char **split_col_family_col_name = splitString(col_family_col_name, ":", &len);
	if (len != 2)
		return false;
	for (int iter = 0; iter < rec->col_count; iter++)
	{
		my_stack = rec->cols[iter];
		if (strcmp(my_stack->col_family_name, split_col_family_col_name[0]) == 0 && strcmp(my_stack->col_name, split_col_family_col_name[1]) == 0)
		{
			if (value[0] == '+')
			{
				char *append_value = (char *)malloc(sizeof(char) * (strlen(value) + strlen(my_stack->array[my_stack->version_number - 1]) + 2));
				append_value[0] = '\0';
				value = sub_string(value, 2, strlen(value) - 2);
				strcpy(append_value, my_stack->array[my_stack->version_number - 1]);
				strcat(append_value, ";");
				strcat(append_value, value);
				push(my_stack, append_value);
			}
			else
			{
				value = sub_string(value, 1, strlen(value) - 2);
				push(my_stack, value);
			}
			// creating transaction
			char *modified = (char *)malloc(sizeof(char) * 10);
			modified[0] = '\0';
			sprintf(modified, "%d=%d", iter, my_stack->version_number);
			insertIntoTransaction(table_name, key, modified);
			return true;
		}
	}
	strcat(col_family_col_name, ":");
	strcat(col_family_col_name, split_col_family_col_name[1]);
	value = sub_string(value, 1, strlen(value) - 2);
	insert_values_into_record(rec, col_family_col_name, value, table_name, key);
	return true;
}


void insert_or_update_into_table(table *my_table, char *row_name, char *col_family_col_name, char *value) {
	if (my_table == NULL || row_name == NULL || col_family_col_name == NULL || value == NULL)
		return;

	record *rec = searchInTable(my_table, row_name);
	if (rec == NULL){
		//Record not found.
		rec = create_record(my_table);
		if (value[0] == '+')
			value = sub_string(value, 2, strlen(value) - 2);
		else
			value = sub_string(value, 1, strlen(value) - 2);
		insert_values_into_record(rec, col_family_col_name, value, my_table->table_name, row_name);
	}
	else{
		//Record found.
		update_values_into_record(rec, col_family_col_name, value, my_table->table_name, row_name);
	}
	int hashIndex = hashCode(row_name, my_table->max_rows);
	if (hashIndex == -1)
	{
		//printErrorProcessingHashCode();
		return;
	}

	if (my_table->rows[hashIndex] != NULL) {
		record *tItem = my_table->rows[hashIndex];
		if (strcmp(tItem->key, row_name) == 0)
		{
			rec->next = my_table->rows[hashIndex]->next;
			my_table->rows[hashIndex] = rec;
			my_table->row_count += 1;
			return;
		}
		while (tItem->next != NULL)
		{
			tItem = tItem->next;
		}
		tItem->next = rec;
		my_table->row_count += 1;
		return;
	}

	my_table->rows[hashIndex] = rec;
	my_table->row_count += 1;
	return;
}


bool deleteInTable(table *my_table, char *key)
{
	if (my_table == NULL || key == NULL)
	{
		//printErrorWithNullInputs();
		return false;
	}
	int hashIndex = hashCode(key, my_table->max_rows);
	if (hashIndex == -1)
	{
		//printErrorProcessingHashCode();
		return false;
	}
	record *item = my_table->rows[hashIndex];
	record *prevItem = item;
	while (item->next != NULL) {
		if (strcmp(my_table->rows[hashIndex]->key, key) == 0) {
			record* temp = my_table->rows[hashIndex];
			prevItem->next = item->next;
			free(temp);
			return true;
		}
		prevItem = item;
		item = item->next;
	}
	my_table->rows[hashIndex] = NULL;
	free(item);
	return true;
}

table *searchInDatabase(database *db,char *name)
{
	if (db->table_count < 1 || name == NULL)
	{
		//printErrorWithNullInputs();
		return NULL;
	}
	int hashIndex = hashCode(name, db->db_size);
	if (hashIndex == -1)
	{
		//printErrorProcessingHashCode();
		return NULL;
	}
	table *item = db->tables[hashIndex];
	while (item != NULL) {
		if (strcmp(item->table_name, name) == 0)
			return item;
		item = item->next;
	}
	return NULL;

}

bool isDuplicateTable(database *db, table *t)
{
	table *temp_table = searchInDatabase(db, t->table_name);
	return temp_table != NULL;
}

bool insert_into_database(database *my_db, table *my_table) {
	if (my_db == NULL || my_table == NULL)
	{
		//printErrorWithNullInputs();
		return false;
	}

	if (isDuplicateTable(my_db,my_table))
		return false;

	int hashIndex = hashCode(my_table->table_name, my_db->db_size);
	if (hashIndex == -1)
	{
		//printErrorProcessingHashCode();
		return false;
	}

	if (my_db->tables[hashIndex] != NULL) {
		table *tItem = my_db->tables[hashIndex];
		if (strcmp(tItem->table_name, my_table->table_name) == 0)
		{
			my_db->tables[hashIndex] = my_table;
			my_db->table_count += 1;
			return true;
		}
		while (tItem->next != NULL)
		{
			tItem = tItem->next;
		}
		tItem->next = my_table;
		my_db->table_count += 1;
		return true;
	}

	my_db->tables[hashIndex] = my_table;
	my_db->table_count += 1;
	return true;
}

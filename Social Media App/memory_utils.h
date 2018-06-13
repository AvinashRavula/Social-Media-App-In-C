
table *create_table(char *table_name, char **col_families, int col_families_len)
{
	table *root = (table *)malloc(sizeof(table));
	root->table_name = table_name;
	root->rows = (record **)malloc(sizeof(record *) * 100);
	root->row_count = 0;
	root->max_rows = 100;
	root->col_families = col_families;
	root->col_families_len = col_families_len;
	root->disabled = false;
	for (int iter = 0; iter < 100; iter++)
		root->rows[iter] = NULL;
	return root;
}

void reinitialize_table(table *my_table)
{
	int size = my_table->max_rows + 100;
	my_table->rows = (record **)realloc(my_table->rows, size);
	my_table->max_rows = size;
}

void reinitialize_record(record *rec)
{
	int old_size = rec->max_cols;
	rec->cols = (Stack **)realloc(rec->cols, old_size + 100);
	rec->max_cols = old_size + 100;
	for (int iter = old_size; iter < rec->max_cols; iter++)
		rec->cols[iter] = NULL;
}

record *create_record(table *my_table)
{
	record *root = (record *)malloc(sizeof(record));
	root->cols = (Stack **)malloc(sizeof(Stack *) * 100);
	root->max_cols = 100;
	root->col_count = 0;
	root->key = (char *)malloc(sizeof(char) * 50);
	for (int iter = 0; iter < 100; iter++)
		root->cols[iter] = NULL;
	root->next = NULL;
	return root;
}


void reinitialize_database(database *db)
{
	int old_size = db->db_size;
	db->db_size = db->db_size + DB_SIZE;
	db->tables = (table **)realloc(db->tables, db->db_size);
	for (int iter = old_size; iter < db->db_size; iter++)
		db->tables[iter] = NULL;
}

database *create_database(char *name)
{
	database *my_db = (database*)malloc(sizeof(database));
	my_db->db_name = (char *)malloc(sizeof(char) * DB_NAME_LEN);
	strcpy(my_db->db_name, name);
	my_db->db_size = DB_SIZE;
	my_db->tables = (table **)malloc(sizeof(table) * my_db->db_size);
	my_db->table_count = 0;
	for (int iter = 0; iter < DB_SIZE; iter++)
	{
		my_db->tables[iter] = NULL;
	}
	return my_db;
}



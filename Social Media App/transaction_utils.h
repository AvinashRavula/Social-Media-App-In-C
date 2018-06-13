

void initialize_transaction_holder(int size)
{
	trans_holder.trans = (transaction **)malloc(sizeof(transaction *) * size);
	trans_holder.transaction_count = 0;
	trans_holder.transaction_limit = size;
}

void reinitialize_transaction_holder()
{
	trans_holder.transaction_limit = trans_holder.transaction_limit + trans_holder.transaction_limit;
	trans_holder.trans = (transaction **)realloc(trans_holder.trans, trans_holder.transaction_limit);
}

void insertIntoTransaction(char *table_name, char *key, char *modified_cols)
{
	transaction *new_transaction = (transaction *)malloc(sizeof(transaction));
	new_transaction->key = key;
	new_transaction->table_name = table_name;
	new_transaction->modified_cols= modified_cols;
	if (trans_holder.transaction_count == trans_holder.transaction_limit)
		reinitialize_transaction_holder();
	trans_holder.trans[trans_holder.transaction_count] = new_transaction;
	trans_holder.transaction_count += 1;
}

void reset_transaction_record()
{
	for (int iter = 0; iter < trans_holder.transaction_count; iter++)
	{
		free(trans_holder.trans[iter]);
		trans_holder.trans[iter] = NULL;
	}
	trans_holder.transaction_count = 0;
}

void revert_commit(record *rec, int *modified_cols, int len)
{
	for (int iter = 0; iter < len; iter++){
		rec->cols[modified_cols[iter]]->version_number -= 1;
		pop(rec->cols[modified_cols[iter]]);
	}
}

bool commit(database *db)
{
	for (int iter = 0; iter < trans_holder.transaction_count; iter++)
	{
		char *states = trans_holder.trans[iter]->modified_cols;
		char *table_name = trans_holder.trans[iter]->table_name;
		char *key = trans_holder.trans[iter]->key;
		table *my_table = searchInDatabase(db, table_name);
		if (my_table != NULL)
		{
			record *rec = searchInTable(my_table, key);
			if (rec != NULL)
			{
				int count = 0;
				char **splitted = splitString(states, ",", &count);
				int *modified_indexes = (int *)malloc(sizeof(int) * count);
				int modified_index = 0;
				for (int iter2 = 0; iter2 < count; iter2++)
				{
					int count2 = 0;
					char **split = splitString(splitted[iter2], "=", &count2);
					int index = atoi(split[0]);
					int prev_version_number = atoi(split[1]);
					if (rec->cols[index]->version_number == prev_version_number)
					{
						rec->cols[index]->version_number += 1;
						modified_indexes[modified_index++] = index;
					}
					else{
						revert_commit(rec, modified_indexes, modified_index);
						reset_transaction_record();
						return false;
					}
				}
			}
		}
	}
	reset_transaction_record();
	return true;
}
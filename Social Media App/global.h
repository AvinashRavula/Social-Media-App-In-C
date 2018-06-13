
struct Stack
{
	int top;
	int version_number;
	char *col_name;
	char *col_family_name;
	unsigned capacity;
	char **array;
};

// A structure to represent a queue
struct Queue
{
	int front, rear, size;
	unsigned capacity;
	char** array;
};

struct record{
	char *key;
	struct Stack **cols;
	int col_count;
	int max_cols;
	record *next;
};


struct table{
	char *table_name;
	record **rows;
	int row_count;
	int max_rows;
	char **col_families;
	int col_families_len;
	bool disabled;
	table *next;
};

struct database{
	char *db_name;
	table **tables;
	int table_count;
	int db_size;
};

struct transaction{
	char *table_name;
	char *key;
	char *modified_cols;
};

struct transaction_holder{
	transaction **trans;
	int transaction_count;
	int transaction_limit;
}trans_holder;

struct thread_pool{
	pthread_t threads[NO_OF_THREADS];
	int thread_state[NO_OF_THREADS];
}thread_manager;

queue<char *> request_queue;
Queue *my_queue;

char *put_queries[80] = {
	"put 'profile','1','basic info:username','avinashravula'",
	"put 'profile','1','basic info:fullname','Avinash Ravula'",
	"put 'profile','1','basic info:gender','male'",
	"put 'profile','1','contact info:email','avinashravula1@gmail.com'"
};

char *get_queries[20] = {
	"get 'profile','1'",
	"get 'profile','2'",
	"get 'profile','3'"
};

char commands[][20] = { "create", "list", "disable", "scan", "is_disabled", "enable", "put", "get", "delete", "export", "import", "exit", "count"};
int command_len = 13;
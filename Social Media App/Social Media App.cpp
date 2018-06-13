// Social Media App.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<stdlib.h>
#include<conio.h>
#include<string.h>
#include<stdio.h>
#include <time.h>
#include <pthread.h>
//#include<Windows.h>
#include <queue>

using namespace std;
#pragma warning(disable:4996)

#define FILESYSTEM "filesystem.bin"
#define DATABASE_MAX_SIZE 1048576

#define VERSION_LIMIT 100
#define COL_NAME_LEN 50
#define COL_VALUE_LEN 200
#define DB_NAME_LEN 50
#define DB_SIZE 100

#define DEFAULT_BUFLEN 512
#define NO_OF_THREADS 4

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t req_proc_lock = PTHREAD_MUTEX_INITIALIZER;
//pthread_rwlock_t lock_rw = PTHREAD_RWLOCK_INITIALIZER;
#include "global.h"
#include "stack_utils.h"
#include "string_utils.h"
#include "queue.h"
#include "utils.h"
#include "memory_utils.h"

#include "hash.h"
#include "transaction_utils.h"

#include "command_utils.h"
#include "query_operations.h"
#include "server.h"

#include "socket_client.h"
#include "client.h"

#include "socket_server.h"


#include "authentication.h"
#include"menu_utils.h"


int _tmain(int argc, _TCHAR* argv[])
{
	database *my_db = create_database("my_db");
	initialize_transaction_holder(100);

	//Default factors to be implemented to run this app.
	process_query(my_db, "create 'profile','basic info','contact info','other info','social info'");
	process_query(my_db, "create 'free_space','info'");
	process_query(my_db, "create 'post','info'");
	process_query(my_db, "create 'message', 'info'");
	process_query(my_db, "put 'free_space','1','info:from','1048576'");
	process_query(my_db, "put 'free_space','1','info:to','1073741824'");

	start_server_request_processor_thread(my_db);
	load_db_from_filesystem(my_db);
	show_authentication_menu(my_db);
	getch();
	return 0;
}


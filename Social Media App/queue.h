
// function to create a queue of given capacity. 
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity)
{
	struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
	queue->capacity = capacity;
	queue->front = queue->size = 0;
	queue->rear = capacity - 1;  // This is important, see the enqueue
	queue->array = (char **)malloc(queue->capacity * sizeof(char *) * capacity);
	return queue;
}

// Queue is full when size becomes equal to the capacity 
int isFull(struct Queue* queue)
{
	return (queue->size == queue->capacity);
}

// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{
	return (queue->size == 0);
}

// Function to add an item to the queue.  
// It changes rear and size
void enqueue(struct Queue* queue, char *item)
{
	if (isFull(queue))
		return;
	queue->rear = (queue->rear + 1) % queue->capacity;
	queue->array[queue->rear] = (char *)malloc(sizeof(char) * 100);
	strcpy(queue->array[queue->rear],item);
	queue->size = queue->size + 1;
	//printf("%d enqueued to queue\n", item);
}

// Function to remove an item from queue. 
// It changes front and size
char * dequeue(struct Queue* queue)
{
	if (isEmpty(queue))
		return NULL;
	char * item = queue->array[queue->front];
	queue->front = (queue->front + 1) % queue->capacity;
	queue->size = queue->size - 1;
	return item;
}

// Function to get front of queue
char * front(struct Queue* queue)
{
	if (isEmpty(queue))
		return NULL;
	return queue->array[queue->front];
}

// Function to get rear of queue
char * rear(struct Queue* queue)
{
	if (isEmpty(queue))
		return NULL;
	return queue->array[queue->rear];
}
char **splitString(char *input, const char *delimter, int *count)
{
	char **words = (char **)malloc(sizeof(char *) * 20);
	for (int i = 0; i < 20; i++)
		words[i] = (char *)malloc(sizeof(char) * 20);
	char *token = strtok(input, delimter);
	int i = 0;
	while (token != NULL)
	{
		sprintf(words[i++], "%s", token);
		token = strtok(NULL, delimter);
	}
	*count = i;
	return words;
}


void trim(char *str)
{
	bool space_flag = false;
	char output[100];
	int output_index = 0, start = 0, end = 0;
	for (start = 0; str[start] != '\0'; start++)
	{
		if (!iswspace(str[start]))
			break;
	}
	int len = strlen(str);
	for (end = len - 1; end >= 0; end--)
	{
		if (!iswspace(str[end]) && str[end] != '\n')
			break;
	}

	for (start; start <= end; start++)
	{
		str[output_index++] = str[start];
	}
	str[output_index] = '\0';
}

char *sub_string(char *str, int start_index, int end_index)
{
	if (end_index >= start_index){
		char *sub_str = (char *)malloc(sizeof(char) * (end_index - start_index) + 2);
		int sub_str_index = 0;
		for (start_index; start_index <= end_index; start_index++)
		{
			sub_str[sub_str_index++] = str[start_index];
		}
		sub_str[sub_str_index] = '\0';
		return sub_str;
	}
	return NULL;
}

char *subString(char str[], int low, int high){
	char new_str[100];
	sprintf(new_str, "%.*s", high - low + 1, (str + low));
	return new_str;
}

int find_char(char *input, char ch)
{
	if (input != NULL){
		for (int i = 0; input[i] != '\0'; i++)
		{
			if (input[i] == ch)
				return i;
		}
	}
	return -1;
}
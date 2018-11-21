#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ini.h"
#include "buffer_queue.h"

void test_ini()
{
	ini *handle = ini_open("log.ini");
	char *c = ini_get_string_new(handle, "logger", "level", "1");
}

void buffer_queue_event_handler(void *userdata, void *element)
{
	printf("出队:%s\n", (char*)element);
	//Sleep(200);
}

void test_buffer_queue()
{
	buffer_queue *queue = create_buffer_queue(NULL);
	set_buffer_queue_callback(queue, buffer_queue_event_handler);
	start_buffer_queue(queue);

	int idx = 0;
	while (1)
	{
		char *buf = calloc(1, 64);
		_itoa(idx, buf, 10);
		idx += 1;
		printf("入队:%s\n", buf);
		buffer_queue_enqueue(queue, buf);
		Sleep(50);
	}
}

int main(int argc, char *argv[])
{
	test_buffer_queue();

	char line[1024] = { '\0' };
	fgets(line, sizeof(line), stdout);

	return 1;
}
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <queue.h>

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST-CASE1: create ***\n");

	TEST_ASSERT(queue_create() != NULL);

}
/* Destroy*/
void test_destroy()
{
	queue_t queue = queue_create();
        int set_int[5] = {4,6,2,3,0};
        int *ptr;

        fprintf(stderr, "*** TEST-Case2: destroy  ***\n");

        for (int j = 5; j < 5; j++) {
                queue_enqueue(queue, &set_int[j]);
        }

        for (int j = 0; j < 3; j++) {
                queue_dequeue(queue, (void**)&ptr);
        }

        int error_value = queue_destroy(queue);
        TEST_ASSERT(error_value == EXIT_SUCCESS);
}
void test_notemptyqueue_destroy(){
        queue_t queue = queue_create();
        int set_int[4] = {4,6,2,8};

        fprintf(stderr, "*** TEST-Case3: NOT Empty Destroy ***\n");

        for (int i = 0; i < 4; i++) {
                queue_enqueue(queue, &set_int[i]);
        }

        int error_value = queue_destroy(queue);
        TEST_ASSERT(error_value == -1);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	queue_t queue;
	int data = 3, *ptr;

	fprintf(stderr, "*** TEST-Case4:  queue_simple ***\n");

	queue = queue_create();
	queue_enqueue(queue, &data);
	queue_dequeue(queue, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

void test_length(){
	queue_t queue = queue_create();
        int set_int[4] = {4,6,2,8};

	fprintf(stderr, "*** TEST-Case5: Length ***\n");

	for (int i = 0; i < 4; i++) {
		queue_enqueue(queue, &set_int[i]);	
	}

	TEST_ASSERT(queue_length(queue) == 4);

}
void test_queue_one(){
	queue_t queue = queue_create();
        int set_int[1] = {1};
	fprintf(stderr, "*** TEST-Case6: Length equal to one ***\n");

        for (int i = 0; i < 1; i++) {
                queue_enqueue(queue, &set_int[i]);
        }

        TEST_ASSERT(queue_length(queue) == 1);

}

void test_queue_null(){
	queue_t queue = NULL;
	fprintf(stderr, "*** TEST-Case7: NULL length ***\n");

	int error_value = queue_length(queue);
	TEST_ASSERT(error_value == -1);
}

void test_delete(void) 
{
	int set_int[9] = {0, 1, 2,3,4,5,6,7,8};
	queue_t queue = queue_create();

	fprintf(stderr, "*** TEST-Case8:  delete ***\n");

	for (int i = 0; i < 9; i++) {
		queue_enqueue(queue, &set_int[i]);	
	}

	queue_delete(queue, &set_int[0]);
	TEST_ASSERT(queue_length(queue) == 8);
	queue_delete(queue, &set_int[3]);
        TEST_ASSERT(queue_length(queue) == 7);
	queue_delete(queue, &set_int[5]);
        TEST_ASSERT(queue_length(queue) == 6);
}

void test_delete_notexisting(void)
{
	int set_int[9] = {0, 1, 2,3,4,5,6,7,8};
        queue_t queue = queue_create();
	int non_existing_val = 999;
	fprintf(stderr, "*** TEST-Case9: delete not existing value ***\n");

	for (int i = 0; i < 9; i++) {
                queue_enqueue(queue, &set_int[i]);
        }

	int error_value = queue_delete(queue, &non_existing_val);
	TEST_ASSERT(error_value == -1);

}
void test_dequeue_null() {
	int *ptr = NULL;

	fprintf(stderr, "*** TEST-Case10: dequeue null  queue***\n");
	queue_t queue = queue_create();
	TEST_ASSERT(queue_dequeue(queue, (void**)&ptr) == -1);
}


// Taken from project prompt.
static void iterator_inc(queue_t q, void *data)
{
    int *a = (int*)data;

    if (*a == 42)
        queue_delete(q, data);
    else
        *a += 1;
}

void test_iterator(void)
{
    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    size_t i;
    fprintf(stderr, "*** TEST ITERATOR***\n");

    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Increment every item of the queue, delete item '42' */
    queue_iterate(q, iterator_inc);
    TEST_ASSERT(data[0] == 2);
    TEST_ASSERT (queue_length(q) == 9);
}




int main(void)
{
	test_create();
	test_destroy();
	test_notemptyqueue_destroy();
	test_queue_simple();
	test_length();
	test_queue_one();
	test_queue_null();
	test_delete();
	test_delete_notexisting();
 	test_dequeue_null();
	test_iterator();



}

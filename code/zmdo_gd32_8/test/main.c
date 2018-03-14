#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void tled_task(void *p);

int main(void) {
	BaseType_t xReturn = xTaskCreate(tled_task, (const char*)"tled_task", 300, NULL, 4, NULL);
	if (xReturn != pdPASS) {
		xReturn = pdPASS;
	}
	//xTaskCreate(tled_task2, (const char*)"tled_task2", 300, NULL, 4, NULL);
	/* Start the scheduler. */
	vTaskStartScheduler();
	return 0;
}

void tled_task(void *p) {
	while(1) {
        vTaskDelay(5000/portTICK_RATE_MS);
		//static uint8_t dr = 0;
		//dr = ~dr;
		//TM1650.show_led(&TM1650.tm1650_n,9,dr);
	}
}

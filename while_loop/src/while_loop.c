/*
 ============================================================================
 Name        : while_loop.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

//#include <stdio.h>
//#include <stdlib.h>

//
//int main(void) {
//	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
//	return EXIT_SUCCESS;
//}

//print the 10 numbers

//
//#include <stdio.h>
//#include <stdint.h>
//void wait_for_user_input(void);
//
//int main(void){
//
//	uint8_t num = 1;
//	while(num <= 10){
//		printf("%u\n",num);
//		num++;
//	}
//
//}
//
//void wait_for_user_input(void){
//	getchar();
//	}


//#include <stdio.h>
//#include <stdint.h>
//void main(void){
//	int32_t start_num, end_num;
//	uint32_t even;
//
//	printf("Enter starting and ending numbers (Give space between two num's): ");
//	scanf("%d %d",&start_num,&end_num);
//
//	if(end_num < start_num){
//		printf("")
//	}
//}
//

#include <stdio.h>
#include <stdint.h>
int main(void){

	int height;
	printf("Enter the height of pyramid: ");
	fflush(stdout);
	scanf("%d",&height);

	if(height > 0){
			for(uint32_t i = 1;i<=(uint32_t)height;i++){
		//		printf("%d\n",i);
				for(uint32_t j = i;j>0 ;j--){
					printf("%u  ",j);
				}
				printf("\n");

			}
	}
	printf("Enter any key to exit.");
	getchar();
	getchar();

	return 0;
}



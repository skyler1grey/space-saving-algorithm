/*
 * test.c
 *
 *  Created on: Mar 30, 2018
 *      Author: root
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "space.h"

#define MAXLINELEN  5

int main(int argc, char *argv[])
{
	space *sp=space_new(5);
	for(int i=0;i<10;i++)
	{
		int temp_num=rand()%101;
		printf("temp_num:%d\n",temp_num);
		space_insert(sp,&i,sizeof(int),temp_num);
		print_mlist(sp);
	}
	printf("****************test round2**************\n");
	for(int i=0;i<10;i++)
	{
		int temp_num=rand()%101;
		printf("temp_num:%d\n",temp_num);
		space_insert(sp,&i,sizeof(int),temp_num);
		print_mlist(sp);
	}

	printf("%llu",sp->mlist->value);
	return 0;
}

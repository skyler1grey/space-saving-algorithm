/*
 * space.c
 *
 *  Created on: Mar 29, 2018
 *      Author: root
 */



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#include "space.h"



void node_delete(node *now)
{
	node *left=now->front_node;
	node *right=now->back_node;
	if(left==NULL)
	{
		if(right==NULL)
		{
			free(now);
			return;
		}
		else
		{
			right->front_node=NULL;
			free(now);
			return;
		}
	}
	else
	{
		if(right==NULL)
		{
			left->back_node=NULL;
			free(now);
			return;
		}
		else
		{
			left->back_node=right;
			right->front_node=left;
			free(now);
			return;
		}
	}
};

space *space_new(int size)
{
	space *sp=malloc(sizeof(space));
	sp->msize=0;
	sp->hsize=size;
	sp->mlist=NULL;
	sp->htable=ght_create(size);
	return sp;
};

void space_free(space *sp)
{
    void *pv, *pk;
    ght_iterator_t iterator;
    for(pv = (bucket *)ght_first(sp->htable, &iterator, &pk); pv;
            pv=(bucket *) ght_next(sp->htable, &iterator, &pk)) {
    	bucket *temp=(bucket *)pv;
    	free(temp->id);
    	free(pv);
    }
    ght_finalize(sp->htable);
    node *first_node=sp->mlist;
    for(;first_node->back_node;first_node=first_node->back_node)
    {
    	free(first_node);
    }
    return;
};

node* node_create(void)
{
	node *new_node=malloc(sizeof(node));
	new_node->back_node=NULL;
	new_node->first_child=NULL;
	new_node->front_node=NULL;
	new_node->value=0;
	return new_node;
};

node *space_getnode(space *sp,unsigned long long count,node *start_node)
{
	if(start_node==NULL)
	{
		start_node=node_create();
		start_node->value=count;
		sp->mlist=start_node;
		return start_node;
	}
	for(;start_node->back_node!=NULL&&start_node->value<count;
			start_node=start_node->back_node);
	if(start_node->back_node==NULL)
	{
		if(start_node->value==count)
			return start_node;
		if(start_node->value>count)
		{
			node* new_node=node_create();
			if(sp->mlist==start_node)
			{
				sp->mlist=new_node;
			}
			new_node->front_node=start_node->front_node;
			new_node->back_node=start_node;
			if(start_node->front_node==NULL)
			{
				start_node->front_node=new_node;
			}
			else
			{
				start_node->front_node->back_node=new_node;
				start_node->front_node=new_node;
			}
			new_node->value=count;
			return new_node;
		}
		else
		{
			node *new_node=node_create();
			start_node->back_node=new_node;
			new_node->back_node=NULL;
			new_node->front_node=start_node;
			new_node->value=count;
			return new_node;
		}
	}
	else
	{
		if(start_node->value==count)
			return start_node;
		else
		{
			node* new_node=node_create();
			if(sp->mlist==start_node)
			{
				sp->mlist=new_node;
			}
			new_node->front_node=start_node->front_node;
			new_node->back_node=start_node;
			if(start_node->front_node==NULL)
			{
				start_node->front_node=new_node;
			}
			else
			{
				start_node->front_node->back_node=new_node;
				start_node->front_node=new_node;
			}
			new_node->value=count;
			return new_node;
		}
	}
};

void insert_bucket_to_node(node *now_node,bucket *now_bucket)
{
	bucket *temp_bucket=now_node->first_child;
	if(temp_bucket==NULL)
	{
		now_node->first_child=now_bucket;
		now_bucket->father=now_node;
		now_bucket->next_bucket=now_bucket;
		return;
	}
	else
	{
		for(;temp_bucket->next_bucket!=now_node->first_child
		;temp_bucket=temp_bucket->next_bucket);
		temp_bucket->next_bucket=now_bucket;
		now_bucket->next_bucket=now_node->first_child;
		now_bucket->father=now_node;
		return;
	}
	return;
};

void remove_bucket_from_node(space *sp,node *now_node,bucket *now_bucket)
{
	if(now_bucket==now_node->first_child)
	{
		if(now_bucket->next_bucket==now_bucket)
		{
			if(sp->mlist==now_node)
			{
				sp->mlist=now_node->back_node;
			}
			node_delete(now_node);
			return;
		}
		else
		{
			now_node->first_child=now_bucket->next_bucket;
			bucket *temp_bucket;
			for(temp_bucket=now_node->first_child;temp_bucket->next_bucket!=now_bucket;
					temp_bucket=temp_bucket->next_bucket);
			temp_bucket->next_bucket=now_bucket->next_bucket;
			return;
		}
	}
	else
	{
		bucket *temp_bucket;
		for(temp_bucket=now_node->first_child;temp_bucket->next_bucket!=now_bucket;
				temp_bucket=temp_bucket->next_bucket);
		temp_bucket->next_bucket=now_bucket->next_bucket;
		return;
	}
	return;
}

bucket *bucket_create(space *sp,void *v,size_t vlen)
{
	bucket *new_bucket=malloc(sizeof(bucket));
	new_bucket->father=NULL;
	new_bucket->id=calloc(1,vlen);
	memcpy(new_bucket->id,v,vlen);
	new_bucket->error=0;
	new_bucket->next_bucket=NULL;
	ght_insert(sp->htable,new_bucket,vlen,v);
	return new_bucket;
};


void space_insert(space * sp,void *v, int vlen, unsigned long long count)
{
	bucket* now_bucket=ght_get(sp->htable,vlen,v);
	if(now_bucket==NULL)
	{
		bucket *new_bucket=bucket_create(sp,v,vlen);
		if(sp->msize<sp->hsize)
		{
			node *now_node=sp->mlist;
			now_node=space_getnode(sp,count,now_node);
			insert_bucket_to_node(now_node,new_bucket);
			sp->msize++;
			return;
		}
		else
		{
			node *min_node=sp->mlist;
			if(min_node==NULL)
			{
				fprintf(stderr,"mlist is NULL\n");
			}
			bucket *min_bucket=min_node->first_child;
			min_bucket=find_min_bucket(min_bucket);
			unsigned long long min_num=min_node->value;
			printf("min_num:%llu\n",min_node->value);
			count+=min_num;
			new_bucket->error=min_num;
			bucket *remove_bucket=ght_remove(sp->htable,vlen,min_bucket->id);
			remove_bucket_from_node(sp,min_node,min_bucket);
			if(remove_bucket==NULL)
			{
				fprintf(stderr,"can't find this bucket\n");
				return;
			}
			if(remove_bucket!=min_bucket)
			{
				fprintf(stderr,"buckets do not match\n");
				return;
			}
			node *now_node=sp->mlist;
			now_node=space_getnode(sp,count,now_node);
			insert_bucket_to_node(now_node,new_bucket);
			free(min_bucket->id);
			free(min_bucket);
			return;
		}
	}
	else
	{
		count+=now_bucket->father->value;
		remove_bucket_from_node(sp,now_bucket->father,now_bucket);
		node *now_node=sp->mlist;
		now_node=space_getnode(sp,count,now_node);
		insert_bucket_to_node(now_node,now_bucket);
	}
	return;
};

bucket *find_min_bucket(bucket *start_bucket)
{
	bucket *cur_bucket=start_bucket->next_bucket;
	bucket *min_bucket=start_bucket;
	unsigned long long temp_error=start_bucket->error;
	for(;cur_bucket->next_bucket!=start_bucket;cur_bucket=cur_bucket->next_bucket)
	{
		if(cur_bucket->error>temp_error)
		{
			min_bucket=cur_bucket;
			temp_error=cur_bucket->error;
		}
	}
	return min_bucket;
}

void print_mlist(space *sp)
{
	node *start_node=sp->mlist;
	for(;start_node!=NULL;start_node=start_node->back_node)
	{
		printf("%llu->",start_node->value);
	}
	printf("\n");
	return;
}


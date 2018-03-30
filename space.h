/*
 * space.h
 *
 *  Created on: Mar 29, 2018
 *      Author: root
 */
/*  sketch element */

#include <stddef.h>
#include <ght_hash_table.h>

typedef struct bucket_{
    unsigned long long error;
    void *id;
    struct bucket_ * next_bucket;
    struct noded *father;
} bucket;

/* monitored list element */
typedef struct noded {
	struct noded *front_node;
	struct noded *back_node;
    unsigned long long value;
    bucket *first_child;
} node;

typedef struct {
    int size;			/* number of elements to monitor -- maximum size of mlist */

    ght_hash_table_t *htable;
    int hsize;			/* size of hash table */

    node *mlist;
    int msize;			/* current size of list */
} space;


void node_delete(node *now);
space *space_new(int size);
void space_free(space *sp);
node* node_create(void);
node *space_getnode(space *sp,unsigned long long count,node *start_node);
void insert_bucket_to_node(node *now_node,bucket *now_bucket);
void remove_bucket_from_node(node *now_node,bucket *now_bucket);
bucket *bucket_create(char *v,size_t vlen);
void space_insert(space * sp,char *v, size_t vlen, unsigned long long count);

/*
 * File: offspring.c
 * @author: Mike Cao 
 * 3/21/19
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trimit.h"
#define MAX_LENGTH 1024 

/*****************************************************************************/

// NTree_S implementation for N-ary tree structure
typedef struct NTree {
	char * name;    //Name of the person
	struct NTree * * children; //collection of children
	size_t child_count;  //Current number of children
	size_t capacity;    //Capacity of children collection
} NTree;              //NTree is an alias for the struct

//NTree_p is the "posize_ter to N-ary tree struct" type.
typedef struct NTree * NTree_p;

// node struct for queue
typedef struct node {
	void * tree;
	struct node * next;
	size_t priority;
} node;

// queue struct
typedef struct queue {
	node * back;
	node * front;
	size_t size;
} queue;

/// makes a queue by initializing it and returning it
queue* make_queue() {
	//allocate space
  	queue *queue = malloc(sizeof(queue));
  	//Initialize all values to empty 
  	queue->front = queue->back = NULL;
  	queue->size = 0;
  	//return the queue
  	return queue;
}

/// enqueues trees to the queue
/// @param queue the queue to enqueue to
/// @param tree the tree to enqueue to the queue
/// @param priotity 
void enqueue(queue* queue, void* tree, size_t priority) {
	// allocate it memory
  	node *QNode = malloc(sizeof(node));
	// set the tree to the node
  	QNode->tree = tree;
	// set the priority
  	QNode->priority = priority;
	// set the next to NULL
  	QNode->next = NULL;
	// check if the queue has anything in it...
  	if(queue->size > 0) {
		// compare the priorities and add the node to the queue
		// accordingly
    		if(queue->back->priority <= QNode->priority) {
     			queue->back->next = QNode;
      			queue->back = QNode;
    		} else if(queue->front->priority > QNode->priority) { 
      			QNode->next = queue->front;
      			queue->front = QNode;
    		} else {
			// check every single node's priority by starting fron
			// the front
      			node * temp = queue->front;
      			while(temp != NULL) {
        			if(temp->next->priority > QNode->priority) {
         				QNode->next = temp->next;
          				temp->next = QNode;
          				break;
        			}
			// go to the next node
        		temp = temp->next;
      			}
    		}
  	} else {
		// set both the front and back to the node if it's the first
		// to be enqueued and there's nothing else in the queue
    		queue->front = QNode;
    		queue->back = QNode;
  	}
	// increment size by one
  	queue->size++;
}

/// dequeues the queue
/// @param queue the queue to dequeue from
/// returns the pointer to the dequeued tree
void* dequeue(queue* queue) {
	// point the node to the front
  	node *QNode = queue->front;
	// check if there is anything in the queue
  	if(queue->size > 0) {
		// point the front to the next node
    		queue->front = queue->front->next;
		// if there's only one node in the queue, set the front and 
		// back to NULL
    		if(queue->size == 1) {
      			queue->back = NULL;
      			queue->front = NULL;
    		}
		// decrement the size
    		queue->size--;
    		void * tree = QNode->tree;
    		//delete the node
    		free(QNode);
    		//return the data
    		return tree;
  	} else { 
		// return NULL since there's nothing to dequeue...
	     	return NULL;
  	}
}

/// frees the queue
/// @param queue the queue to free
void free_queue(queue* queue) {
	// dequeue everything in the queue, which will free the nodes in the
	// process....
  	while(queue->size > 0) 
    		dequeue(queue);
	// finally, free the queue itself
  	free(queue);
}

/// queues the tree
/// @param tree to queue
/// @param queue to enter the tree into
/// @param priotity which will be lower for descendants
void queueT(NTree *tree, queue * queue, size_t priority) {
  	if(tree == NULL)
    		return;
	// enqueues the root
  	enqueue(queue, tree, priority);
	// enqueues the children and their children and so on...
  	for(size_t i = 0; i < tree->child_count; i++)
   		queueT(tree->children[i],queue,priority + 1);
}

/*****************************************************************************/

/// finds a person in the tree and returns their tree
/// @param tree to search
/// @param name to search for
NTree* find(NTree * tree, char * name){
  	NTree *temp = NULL;
	// make a queue to dequeue and search for the names
  	queue *queue = make_queue();
	// queue the tree
  	queueT(tree,queue,0);
	size_t size = queue->size;
	// for every node in the queue...
  	for(size_t i = 0; i < size; i++) {
		// dequeue and check the name
    		NTree *ptr = dequeue(queue);
    		if(!strcmp(ptr->name, name)) {
      			temp = ptr;
      			break;
    		}
  	}
  	// free the queue
  	free_queue(queue);
	// return the tree
  	return temp;
}

/// add inserts a child_name an NTree
/// @param tree to add to
/// @param parent_name to find
/// @child_name to add
NTree * add(NTree * tree, char * parent_name, char * child_name){
	 //check if tree is empty
 	if(!tree) {
    		char * pName = malloc(strlen(parent_name) + 1);
    		strcpy(pName, parent_name);
    		//If tree is empty, initialize tree with parent as the root
    		tree = malloc(sizeof(NTree));
    		//Set root's name to parent's name
    		tree->name = pName;
    		//set root to childless
    		tree->children = NULL;
    		tree->child_count = 0;
    		//root can hold no children
    		tree->capacity = 0;
 	}
	// if the child_name is provided
	if(child_name != NULL) {
		// find the parent
  		NTree * parent = find(tree, parent_name);
 		 // Check if parent was found
  		if(parent != NULL) {
    			for(size_t i = 0; i < parent->child_count; i++) {
      			//check all of the children
      				if(!strcmp(child_name,
					parent->children[i]->name)) {
					//child's name is found
        				return tree;      							}
    			}
			char *cName = malloc(strlen(child_name) + 1);
			strcpy(cName,child_name);
			// increase the child_count of the parent
			parent->child_count++;
			//reallocate memory to account for new child
			parent->children = realloc(parent->children,
					(sizeof(NTree)*(parent->child_count)));
			parent->capacity++;
			NTree * child = malloc(sizeof(NTree));
			// initialize the child tree variables...
			if(child != NULL) {
				child->name = cName;
				child->children = NULL;
				child->child_count = 0;
				child->capacity = 0;
			}				
			// set the child to one of the parent's children
			parent->children[parent->child_count - 1] = child;
		// if the name doesn't match....
		// check if the child is the root and if so make the parent
		// the root and the previous root its child
  		} else if(!strcmp(tree->name, child_name)) {
    			NTree * parent = malloc(sizeof(NTree));
    			if(parent != NULL) {
     				 //copy name size_to new pointer
      				char * pName = malloc(strlen(parent_name)+1);
      				strcpy(pName, parent_name);
      				parent->name = pName;
      				//add root to list of children
      				parent->child_count = 1;
      				parent->children = malloc(
					(sizeof(tree) * (parent->child_count)));
      				parent->children[0] = tree;
      				parent->capacity = 1;
      				return parent;
    			}	
  		} else {
			// if all those tests were failed....
    			fprintf(stderr,
				"Unable to add %s, %s not found in tree.\n",
					child_name, parent_name);
    			free(parent);
  		}
 	}
	// return the whole tree
 	return tree;
}

/// prints the tree
/// @param tree the tree to print
void print_tree(NTree * tree) {
	// make the queue and queue the tree to print in BFS order
  	queue *queue = make_queue();
  	queueT(tree, queue, 0);
	// check if there's anything in the queue
  	while(queue->size > 0) {
		// start dequeuing
		NTree *parent = dequeue(queue);
		// print statement format for the parent of the subtree
		printf("%s had ", parent->name);	
		if(parent->child_count > 0) {
			// loop through the children and print their names
			// in different formats according to their index
			for(size_t i = 0; i < parent->child_count; i++) {
				if(parent->child_count == 1) {
					printf("%s.\n",
						parent->children[i]->name);
				} else if(i == parent->child_count - 1) {
					printf(" and %s.\n",
						parent->children[i]->name);
				} else if(i == 0) {
					printf("%s",parent->children[i]->name);
				} else {
					printf(", %s",
						parent->children[i]->name);
				}
			}
		} else {
			// print this if the child_count is 0
			printf("no offspring.\n");
		}
  	}
	// free the queue since it won't be used anymore
  	free_queue(queue);
}

/// prints the subtree of the parent
/// @param parent the tree of the parent to print
void print_person(NTree * parent){
	// printf format
  	printf("%s had ",parent->name);
	// check if they have children
  	if(parent->child_count > 0) {
		// loop through each children and print their names in 
		// different formats accourding to their index
    		for(size_t i=0; i< parent->child_count; i++) { 
      			if(parent->child_count == 1) {
        			printf("%s. \n", parent->children[i]->name);
      			} else if(i == parent->child_count-1){
        			printf(" and %s.\n",parent->children[i]->name);
			} else if(i == 0) {
        			printf("%s", parent->children[i]->name);
      			} else {        
				printf(", %s", parent->children[i]->name);
      			}

    		}
  	} else {
		// print this if they don't have children
    		printf("no offspring.\n");
  	}
}

/// calculates the size
/// @param tree to calculate the size of
size_t tree_size(NTree * tree){
	// queues the whole tree and count the number of nodes in the queue
  	queue *queue = make_queue();
  	queueT(tree, queue, 0);
	size_t size = queue->size;
  	free_queue(queue);
	// return the size
  	return size;
}

/// calculates the height
/// @param tree to calculate the height of
size_t tree_height(NTree * tree){
  	if(tree == NULL)
    		return 0;
	// make the queue to queue the tree
	queue *queue = make_queue();
  	queueT(tree, queue, 0);
	// priority is their level in the tree (e.g. root = 1 grandchildren = 3)
	size_t height = queue->back->priority;
	// free the queue
  	free_queue(queue);
	// return the height
  	return height + 1;
}

/// free the individual node
/// @param QNode the node to free
void free_node(NTree * QNode){
	// free the name and children
  	free(QNode->name);
  	free(QNode->children);
}

/// free the tree
/// @param tree the tree to free
void free_tree(NTree * tree){
	// queue the tree
  	queue *queue = make_queue();
    	queueT(tree, queue, 0);
	// dequeue everything while freeing them one by one
	while(queue->size > 0) {
		NTree *QNode = dequeue(queue);
		free_node(QNode);
		free(QNode);
  	}
	// finally, free the queue
 	free_queue(queue);
}

/// print information on available commands. 
void help() {
        printf("User Commands for offspring:\n");
        printf("add parent-name, child-name # find parent and add child.\n");
        printf("find name     # search and print name and children if name is found.\n");
        printf("print [name]  # breadth first traversal of offspring from name.\n");
        printf("size [name]   # count members in the [sub]tree.\n");
        printf("height [name] # return the height of [sub]tree.\n");
        printf("init          # delete current tree and restart with an empty tree.\n");
        printf("help          # print this information.\n");
        printf("quit          # delete current tree and end program.\n");
}

/// Main function
/// prompts the user if no file is given until they exit by specifying quit
/// @param argc number of arguments
/// @param argv array of arguments
int main(size_t argc, char **argv){
	NTree * tree = NULL;
	// read from file if the proper arguments is inputted
	if(argc == 2) {
		FILE * fp;
                fp = fopen(argv[1],"r");
                char finput[MAX_LENGTH];
                // read file line by line
                while(fscanf(fp, "%100[^\n]\n", finput) != EOF) {
                        char * parent;
                        parent = strtok(finput,",");
                        if(parent != NULL) {
                                tree = add(tree,parent, NULL);
                                char *child = strtok(NULL, ",");
                                while(child) {
                                          // add children to parent
                                          strcpy(child,trim(child));
                                          // add the child
                                          tree = add
						  (tree,parent,child);
                                          child = strtok(NULL,",");
                                }
                        }
                        //Read next line
                }
                //Close file
                fclose(fp);
  	} else if(argc > 2) {
		fprintf(stderr, "Unknown arguments!\n");
		return EXIT_FAILURE;
	}
	// infinite loop to keep prompting the user
  	while(1) {
    		printf("offspring> ");
		char input[MAX_LENGTH];
		fgets(input, MAX_LENGTH, stdin);
		char tInput[MAX_LENGTH];
		strcpy(tInput , strtok(input," ,"));
		//strcpy(tInput , trim(tInput));
		memmove(tInput, trim(tInput), strlen(tInput));
		if(!strcmp(tInput,"add")) { 
			char * parent = strtok(NULL,",");
			if(parent != NULL) {
				// get rid of overlap using memmove
				memmove(parent, trim(parent), strlen(parent));
				//strcpy(parent , trim(parent));
				//child argument
				char * child = strtok(NULL,",");
				if(child != NULL) {
					memmove(child, trim(child), strlen(
						child));
					//strcpy(child , trim(child));
					tree = add(tree,parent,child);
				} else {
					fprintf(stderr,
						"add parent_name child_name\n");
				}
			} else {
				fprintf(stderr,"add parent_name child_name\n");
			}
		}
		//Check if tInput is print
		if(!strcmp(tInput,"print")) {
			//Check if optional argument is there
			char * name;
			name = strtok(NULL, ",");
			if(name != NULL) {
				strcpy(name, trim(name));
				if(find(tree,name)) {
					print_tree(find(tree,name));
				} else {
					fprintf(stderr, 
						"Unable to find %s in tree.\n", 
						name);
				}
			} else {
				print_tree(tree);
			}
		}
		//Check if tInput is find
		if(!strcmp(tInput,"find")) { 
			//Check for arguments
			char * name;
			name = strtok(NULL, ",");
			strcpy(name, trim(name));
			if(find(tree,name)) {//name in tree
				print_person(find(tree,name));
			} else {
				fprintf(stderr, 
					"Unable to find %s in tree.\n", name);
			}
	      
		}		
   		// Check if tInput is size
    		if(!strcmp(tInput,"size")) {
      			//print the size of the tree
      			char * name = strtok(NULL, ",");
      			if(name != NULL) {
				strcpy(name,trim(name));
        			if(find(tree,name)) {
          				printf("size: %ld\n",
						tree_size(find(tree,name)));
        			} else {
          				fprintf(stderr, 
						"Unable to find %s in tree.\n", 
						name);
       			 	}			
      			} else {
        			printf("size: %ld\n",tree_size(tree));
      			}
    		}				
    		//Check if tInput is height
    		if(!strcmp(tInput,"height")) {
			char * name = strtok(NULL, ",");
			if(name != NULL) {
				strcpy(name,trim(name));
				if(find(tree,name)) {
					printf("height: %ld\n",
						tree_height(find(tree,name)));
				} else {
					fprintf(stderr, 
						"Unable to find %s in tree.\n", 
						name);
				}
			} else {
				printf("height: %ld\n",tree_height(tree));
			}
    		}
		//Check if tInput is quit
		if(!strcmp(tInput,"quit")) {
			//Destroy the tree 
			free_tree(tree);
			printf("\n");
			return EXIT_SUCCESS;
		}
		// check if tInput is init
    		if(!strcmp(tInput,"init")) {
      			//Destroy the tree 
      			free_tree(tree);
      			tree = NULL;
    		}
		//Check if tInput is help
    		if(!strcmp(tInput, "help")) {
	    		//print information of valid commands
			help(); 
    		}
  	}
}

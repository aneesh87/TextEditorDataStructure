#include <stdio.h>
#include <stdlib.h>

//This is my comment

typedef int key_t;
typedef char object_t;
typedef struct tr_n_t {key_t      key; 
                struct tr_n_t   *left;
                struct tr_n_t  *right;
                int height;
               /* possibly additional information */ } tree_node_t;

typedef struct tr_n_t text_t;
#define BLOCKSIZE 256

tree_node_t *currentblock = NULL;
int size_left;
tree_node_t *free_list = NULL;
int nodes_taken = 0;
int nodes_returned = 0;

tree_node_t *get_node()
{ tree_node_t *tmp;
  nodes_taken += 1;
  if( free_list != NULL )
  {  tmp = free_list;
     free_list = free_list -> right;
  }
  else
  {  if( currentblock == NULL || size_left == 0)
     {  currentblock = 
                (tree_node_t *) malloc( BLOCKSIZE * sizeof(tree_node_t) );
        size_left = BLOCKSIZE;
     }
     tmp = currentblock++;
     size_left -= 1;
  }
  return( tmp );
}

void return_node(tree_node_t *node)
{  node->right = free_list;
   free_list = node;
   nodes_returned +=1;
}

tree_node_t *create_tree(void)
{  tree_node_t *tmp_node;
   tmp_node = get_node();
   tmp_node->left = NULL;
   return( tmp_node );
}

object_t *find_iterative(tree_node_t *tree, key_t query_key)
{  tree_node_t *tmp_node;
   if( tree->left == NULL )
     return(NULL);
   else
   {  tmp_node = tree;
      while( tmp_node->right != NULL )
      {   if( query_key < tmp_node->key )
               tmp_node = tmp_node->left;
          else
               tmp_node = tmp_node->right;
      }
      if( tmp_node->key == query_key )
         return( (object_t *) tmp_node->left );
      else
         return( NULL );
   }
}

object_t *find_recursive(tree_node_t *tree, key_t query_key)
{  if( tree->left == NULL || 
       (tree->right == NULL && tree->key != query_key ) )
      return(NULL);
   else if (tree->right == NULL && tree->key == query_key )
      return( (object_t *) tree->left );     
   else
   {  if( query_key < tree->key )
         return( find_recursive(tree->left, query_key) );
      else
         return( find_recursive(tree->right, query_key) );
   }
}

void insert(tree_node_t *tree, key_t new_key, object_t *new_object)
{  
    tree_node_t *tmp_node;
    tmp_node = tree;
    while( tmp_node->right != NULL ) {   
          tmp_node->key = tmp_node->key + 1;
          if( new_key <= tmp_node->left->key ) {
               tmp_node = tmp_node->left;
          } else {               
               new_key = new_key - tmp_node->left->key;
               tmp_node = tmp_node->right;
          }
    }
    /* found the candidate leaf. Test whether key distinct */ 
    /* key is distinct, now perform the insert */ 
    tree_node_t *old_leaf, *new_leaf;
    old_leaf = get_node();
    old_leaf->left = tmp_node->left; 
    old_leaf->key = tmp_node->key;
    old_leaf->right  = NULL;
    new_leaf = get_node();
    new_leaf->left = (tree_node_t *) new_object; 
    new_leaf->key = 1;
    new_leaf->right  = NULL; 
    tmp_node->left  = new_leaf;
    tmp_node->right = old_leaf;
    tmp_node->key  = 2; 
}

object_t *_delete(tree_node_t *tree, key_t delete_key)
{  tree_node_t *tmp_node, *upper_node, *other_node;
   object_t *deleted_object;
   if( tree->left == NULL )
      return( NULL );
   else if( tree->right == NULL )
   {  if(  tree->key == delete_key )
      {  deleted_object = (object_t *) tree->left;
         tree->left = NULL;
         return( deleted_object );
      }
      else
         return( NULL );
   }
   else
   {  tmp_node = tree;
      while( tmp_node->right != NULL )
      {   upper_node = tmp_node;
          if( delete_key < tmp_node->key )
          {  tmp_node   = upper_node->left; 
             other_node = upper_node->right;
          } 
          else
          {  tmp_node   = upper_node->right; 
             other_node = upper_node->left;
          } 
      }
      if( tmp_node->key != delete_key )
         return( NULL );
      else
      {  upper_node->key   = other_node->key;
         upper_node->left  = other_node->left;
         upper_node->right = other_node->right;
         deleted_object = (object_t *) tmp_node->left;
         return_node( tmp_node );
         return_node( other_node );
         return( deleted_object );
      }
   }
}

void remove_tree(tree_node_t *tree)
{  tree_node_t *current_node, *tmp;
   if( tree->left == NULL )
      return_node( tree );
   else
   {  current_node = tree;
      while(current_node->right != NULL )
      {  if( current_node->left->right == NULL )
         {  return_node( current_node->left );
            tmp = current_node->right;
            return_node( current_node );
            current_node = tmp;
         }
         else
         {  tmp = current_node->left;
            current_node->left = tmp->right;
            tmp->right = current_node; 
            current_node = tmp;
         }
      }
      return_node( current_node );
   }
}

tree_node_t *interval_find(tree_node_t *tree, key_t a, key_t b)
{  tree_node_t *tr_node;
   tree_node_t *node_stack[200]; int stack_p = 0;
   tree_node_t *result_list, *tmp, *tmp2;
   result_list = NULL;
   node_stack[stack_p++] = tree;
   while( stack_p > 0 )
   {  tr_node = node_stack[--stack_p];
      if( tr_node->right == NULL )
      {  /* reached leaf, now test */
	 if( a <= tr_node->key && tr_node->key < b )
         {  tmp = get_node();        /* leaf key in interval */
            tmp->key  = tr_node->key; /* copy to output list */  
	    tmp->left = tr_node->left;   
            tmp->right = result_list;
            result_list = tmp;
         }
      } /* not leaf, might have to follow down */
      else if ( b <= tr_node->key ) /* entire interval left */
         node_stack[stack_p++] = tr_node->left;
      else if ( tr_node->key <= a ) /* entire interval right*/
         node_stack[stack_p++] = tr_node->right;
      else   /* node key in interval, follow left and right */
      {  node_stack[stack_p++] = tr_node->left;
         node_stack[stack_p++] = tr_node->right;
      }
   }
   return( result_list );
}

void check_tree( tree_node_t *tr, int depth, int lower, int upper )
{  if( tr->left == NULL )
   {  printf("Tree Empty\n"); return; }
   if( tr->key < lower || tr->key >= upper )
         printf("Wrong Key Order \n");
   if( tr->right == NULL )
   {  if( *( (int *) tr->left) == 10*tr->key + 2 )
         printf("%d(%d)  ", tr->key, depth );
      else
         printf("Wrong Object \n");
   }
   else
   {  check_tree(tr->left, depth+1, lower, tr->key ); 
      check_tree(tr->right, depth+1, tr->key, upper ); 
   }
}

text_t * create_text();
void insert_line( text_t *txt, int index, char * new_line);
char * get_line( text_t *txt, int index);

int main()
{  
   
   tree_node_t *searchtree;
   char nextop;
   searchtree = create_text();
   printf("Made Tree\n");
   printf("In the following, the key n is associated wth the objecct 10n+2\n");
   while( (nextop = getchar())!= 'q' )
   { if( nextop == 'i' )
     { int inskey, success;
       char *strobj;
       strobj = (char *) malloc(sizeof(char) * 100);
       scanf(" %d", &inskey);
       fseek(stdin,0,SEEK_END);
       scanf ("%[^\n]%*c", strobj);
       insert_line( searchtree, inskey, strobj );
       printf("  insert line successful, key = %d, object value = %s, \n",
        	  inskey, strobj);
     }  
     if(nextop == 'f' ) { 
        int findkey;
        char *findobj;
        scanf(" %d", &findkey);
        findobj = get_line( searchtree, findkey);
        if(findobj == NULL) {
           printf("  get line failed, for key %d\n", findkey);
        } else {
           printf("  get line successful, found object %s\n", findobj);
        }
     }
     if( nextop == 'v' )
     { int a, b;  tree_node_t *results, *tmp;
       scanf(" %d %d", &a, &b);
       results = interval_find( searchtree, a, b );
       if( results == NULL )
          printf("  no keys found in the interval [%d,%d[\n", a, b);
       else
       {  printf("  the following keys found in the interval [%d,%d[\n", a, b);
          while( results != NULL )
	  {  printf("(%d,%d) ", results->key, *((int *) results->left) );
             tmp = results;
	     results = results->right;
             return_node( tmp );
          }
          printf("\n");
       }
     }
     if( nextop == 'd' )
     { int delkey;
       char *delobj;
       scanf(" %d", &delkey);
       delobj = _delete( searchtree, delkey);
       if( delobj == NULL )
         printf("  delete failed for key %d\n", delkey);
       else
         printf("  delete successful, deleted object %s for key %d\n", delobj, delkey);
     }
     if( nextop == '?' )
     {  printf("  Checking tree\n"); 
        check_tree(searchtree,0,-1000,1000);
        printf("\n");
        if( searchtree->left != NULL )
 	   printf("key in root is %d\n",	 searchtree->key );
        printf("  Finished Checking tree\n"); 
     }
   }

   remove_tree( searchtree );
   printf("Removed tree.\n");
   printf("Total number of nodes taken %d, total number of nodes returned %d\n",
	  nodes_taken, nodes_returned );
   return(0);
}

// the main functions

text_t * create_text() {
  text_t * new_text;
  new_text = get_node();
  new_text->left = ( text_t *) ("\0");
  new_text->key = 1;
  new_text->right = NULL;
  return ( new_text );
}
int length_text( text_t *txt) {
  /* returns the number of lines of the current text. */
   return (txt->key - 1); 
}

char * get_line( text_t *txt, int index) {
  /* gets the line of number index, if such a line exists, and
    returns NULL else.
  */
  printf("root key is %d\n",txt->key);
  if (txt->key == 1 || index >= txt->key) {
     return NULL;
  }
  text_t *temp = txt;
  // later test and change to temp->key == 1 !!!!!!!!!!!!!!!! change to for ??
  while (temp->right != NULL){
      if (index > temp->left->key) {
          index = index - temp->left->key;
          temp = temp->right;
      } else {
          temp = temp->left;
     }
  }
  return (char *) temp->left;
}

void append_line( text_t *txt, char * new_line) {
  /* appends new line as new last line. */
  insert(txt, txt->key, new_line);

}

char * set_line( text_t *txt, int index, char * new_line) {
  /* sets the line of number index, if such
    a line exists, to new line, and returns a pointer to the previous line of that number. If no line of
    that number exists, it does not change the structure and returns NULL.
  */
    if (txt->key == 1 || index >= txt->key) {
     return NULL;
  }
  text_t *temp = txt;
  // later test and change to temp->key == 1 !!!!!!!!!!!!!!!! change to for ??
  while (temp->right != NULL){
      if (index > temp->left->key) {
          index = index - temp->left->key;
          temp = temp->right;
      } else {
          temp = temp->left;
     }
  }
  char * old  = (char *) temp->left;
  temp->left = (text_t *) new_line;
  return old;
}

void insert_line( text_t *txt, int index, char * new_line) {
  /* inserts the line before the line of
     number index, if such a line exists, to new line, renumbering all lines after that line. If no such
    line exists, it appends new line as new last line.
  */
    if (index >= txt->key) {
        append_line(txt, new_line);
    } else {
        insert(txt, index, new_line);
    }
}

char * delete_line( text_t *txt, int index) {
  /* deletes the line of number index, renumbering all
    lines after that line, and returns a pointer to the deleted line.
  */
  return NULL;
}

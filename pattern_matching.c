
#include "pattern_matching.h"
#include <stdio.h>
#include<string.h>

void rec_destroy(pm_state_t* state);//Assignment of the recursive function of the destroy
//-------------pm_init----------------
//Initializes the fsm parameters ,Returns 0 successfully and -1 in failure
int pm_init(pm_t * t)
{
t->newstate = 1;
pm_state_t* s0 = (pm_state_t*)malloc(sizeof(pm_state_t));
t->zerostate = s0;
if (s0 == NULL)
   return -1;
s0->id = 0;
s0->fail = NULL;
s0->depth = 0;
s0->output = NULL;
s0->_transitions = NULL;
return 0;
}
//--------------pm_goto_set-----------------
//Set a transition arrow from this from_state, via a symbol, to a to_state
//Auxiliary function to be used for functions:pm_addstring, pm_makeFSM, pm_fsm_search, pm_destroy functions.
int pm_goto_set(pm_state_t* from_state, unsigned char symbol, pm_state_t* to_state)
{
if (from_state->_transitions == NULL)
{
from_state->_transitions = malloc(sizeof(slist_t));
if (from_state->_transitions == NULL)
    return -1;
slist_init(from_state->_transitions);
}
pm_labeled_edge_t* edge = malloc(sizeof(pm_labeled_edge_t));//Add the new rib
if (edge == NULL)
    return -1;
edge->label = symbol;
edge->state = to_state;
if (slist_append(from_state->_transitions, edge) == -1)
    return -1;
return 0;
}
//-----------------pm_goto_get--------------------
// Returns the transition state.  If no such state exists, returns NULL.
////Auxiliary function to be used for functions:pm_addstring, pm_makeFSM, pm_fsm_search, pm_destroy functions.
pm_state_t* pm_goto_get(pm_state_t* state, unsigned char symbol)
{
if (state->_transitions == NULL)
   return NULL;
slist_node_t* iterator = slist_head(state->_transitions);
pm_labeled_edge_t* edge = slist_data(iterator);//Find the right rib
while (iterator != NULL)
{
if (edge->label == symbol)
   return edge->state;
iterator = slist_next(iterator);
if (iterator != NULL)
    edge = slist_data(iterator);
}
return NULL;
}
//--------------pm_addstring-------------
//* Adds a new string to the fsm, given that the string is of length n.Returns 0 on success, -1 on failure.
int pm_addstring(pm_t* t, unsigned char* c, size_t n)
{
pm_state_t* iterator = t->zerostate;
pm_state_t* temp;
pm_state_t* new_state;
for (int i = 0; i < n; i++)//Passing on a word to add it to the vending machine
{
temp = pm_goto_get(iterator, c[i]);
if (temp == NULL && t->newstate < PM_CHARACTERS)//If they did not find a rib with the signal in the vending machine and there are still 256 situations
{
   new_state = (pm_state_t*)malloc(sizeof(pm_state_t));
   if (new_state == NULL)
      return -1;
   new_state->depth = iterator->depth + 1;
   new_state->fail = NULL;
   new_state->id = t->newstate;
   t->newstate = (t->newstate) + 1;
   new_state->output = NULL;
   new_state->_transitions = NULL;
   pm_goto_set(iterator, c[i], new_state);
   printf("Allocating state %d\n", new_state->id);
   printf("%d -> %c -> %d\n", iterator->id, c[i], new_state->id);
   iterator = new_state;
}
else
  iterator = temp;
}
iterator->output = (slist_t*)malloc(sizeof(slist_t));
if (iterator->output == NULL)
   return -1;
slist_init(iterator->output);
slist_append(iterator->output, c);
}

//----------pm_makeFSM-----------------------
int pm_makeFSM(pm_t* pmt)
{
//chack validation
if (pmt == NULL ){
  return -1;
}
if( pmt->zerostate == NULL)
  return -1;
if (pmt->zerostate->_transitions==NULL){
  return -1;  
}
slist_t* queue = (slist_t*)malloc(sizeof(slist_t));//create a queue(by a list)
if (queue == NULL)
  return -1;
slist_init(queue);
pm_labeled_edge_t* ed;
pm_state_t * state;
pm_state_t* r;
pm_state_t* fail_temp;
//Putting the zerostate's _transitions into the queue:
slist_node_t* temp = slist_head(pmt->zerostate->_transitions);//a pointer to the transitions list of the zerostate
while (temp != NULL) {
ed = (pm_labeled_edge_t*)slist_data(temp);//get the data of the node
ed->state->fail = pmt->zerostate;//Updating the fail to be the zerostate
slist_append(queue, (void*)ed->state);//adding him to the queue
temp = slist_next(temp);
}
while (queue->size > 0) {
  r = (pm_state_t*)(slist_pop_first(queue));//take the state from the queue
  if (r->_transitions != NULL) {
    temp = slist_head(r->_transitions);
    while (temp != NULL) {
      ed = (pm_labeled_edge_t*)(slist_data(temp));
      state = r->fail;//create a pointer state that be the fail of his "father"
        while ((fail_temp = pm_goto_get(state, ed->label)) == NULL) {//if there is no edge with the char from this state
//if the state is the zerostate and there is no edge with the char from him(Avoid endless loop)
          if (state->id == 0)
            {
            fail_temp = pmt->zerostate;
            break;
            }
          state = state->fail;
          }
//Updating the currentState fail to be the NextState of the state with the requested char
//Connect the output list of the currentState and the nextState
        if (fail_temp->output != NULL) {
          if (ed->state->output == NULL)
          {
            ed->state->output = (slist_t*)malloc(sizeof(slist_t));
            if (ed->state->output == NULL)
              return -1;
            slist_init(ed->state->output);
          }
          slist_append_list(ed->state->output, fail_temp->output);
        }
        slist_append(queue, (void*)ed->state);
        ed->state->fail = fail_temp;
        printf("Setting f(%d) = %d\n", ed->state->id, (ed->state->fail->id));
        temp = slist_next(temp);
      } 
    }
    else {
      if (r->fail->output != NULL) {
        if (r->output == NULL) {
          r->output = (slist_t*)malloc(sizeof(slist_t));
          if (r->output == NULL)
            return -1;
          slist_init(r->output);
          slist_append_list(r->output, r->fail->output);
        }

      }
    }
}
free(queue);
queue = NULL;
return 0;
}
//----------pm_fsm_search---------------------
//Search for matches in a string of size n in the FSM. if there are no matches return empty list
slist_t* pm_fsm_search(pm_state_t* s0, unsigned char* string, size_t n)
{
int i = 0;
pm_state_t* state_now, * from_get;
slist_node_t* temp;
if (s0 == NULL || string == NULL)//check if the user dosen't call to the pm_makeFSM func
  return NULL;
if (s0->_transitions == NULL) //if the automat is empty
  return NULL;
if (strlen(string) != n)
  return NULL;
slist_t* matched_list = (slist_t*)malloc(sizeof(slist_t)); //create a new list from a match type
if (matched_list == NULL)
  return NULL;
slist_init(matched_list);
state_now = s0;
while (i < n)  //Passing as long as we didn't finish the word
{
 //Passing as long as we did not find the right side
  while((from_get=pm_goto_get(state_now,string[i]))==NULL && i<n)
  {
    if (state_now->fail != NULL) {
      state_now = state_now->fail;
      continue;
    }
    i++;
  }
  state_now = from_get;
  if(state_now!=NULL)
  {
  if (state_now->output != NULL)

  {
    temp = slist_head(state_now->output);
    while (temp != NULL && i < n)//If it has output then it goes on the list as long as it's over
    {
      pm_match_t* now = (pm_match_t*)malloc(sizeof(pm_match_t));
      if (now == NULL)
        return NULL;
      now->pattern = ((char*)slist_data(temp));
      now->fstate = state_now;
      now->end_pos = i;
      now->start_pos = (i - strlen(now->pattern) + 1);
      slist_append(matched_list, now);
      printf("Pattern: %s, start at: %d, ends at: %d, last state = %d\n", now->pattern, now->start_pos, now->end_pos, now->fstate->id);
      temp = slist_next(temp);
    }
  }
  }
  i++;
}
return matched_list;//Returns the list of matches
}
//--------------pm_destroy---------------
//A function that calls a recursive function
void pm_destroy(pm_t* pm)
{
if (pm == NULL)
  return;
rec_destroy(pm->zerostate);//Call to function
free(pm);
return;
}
//--------------rec_destroy----------------
//Recursive function that frees the mallocs
void rec_destroy(pm_state_t* state)
{
slist_node_t* temp;
if (state->_transitions == NULL)//Conditions of stopping if he no longer has boys-ribs coming out of him
{
  if (state->output != NULL) {//The list of outputs
    slist_destroy(state->output, SLIST_LEAVE_DATA);
    free(state->output);
  }
free(state);//free the state
state = NULL;
return;
}
temp = slist_head(state->_transitions);
while (temp != NULL)//Moves on the ribs list
{
  rec_destroy(((pm_labeled_edge_t*)(temp->data))->state);//Recursive function reading
  temp = slist_next(temp);
}
if (state->_transitions != NULL)//If it is already over all the ribs and now his ribs list is empty
{
  slist_destroy(state->_transitions, SLIST_FREE_DATA);
  free(state->_transitions);
  state->_transitions = NULL;
}
rec_destroy(state);//Recursive function reading
return;
}
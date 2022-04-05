#include "sdb.h"
#include <isa.h>


#define NR_WP 33

//typedef struct watchpoint {
//  int NO;
//  struct watchpoint *next;
//
//  /* TODO: Add more members if necessary */
//
//  uint32_t val;
//  char expr_str[32];
//} WP;
//
//static WP wp_pool[NR_WP] = {};
//static WP *head = NULL, *free_ = NULL;
//typedef struct watchpoint {
//  int NO;
//  struct watchpoint *next;
//
//  /* TODO: Add more members if necessary */
//
//  uint32_t val;
//  char expr_str[32];
//} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;


void init_wp_pool() {
  int i;
  for (i = 1; i < NR_WP; i ++) {//NO==0, checking blank node
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = &wp_pool[1];
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp(){
    if(free_==NULL){printf("watchpoints exceeds!\n");exit(0);}    
    
    WP *temp;
    WP *p;
    temp = free_;
    free_ = free_->next;
    temp->next=NULL;
    p=head;
    if(p==NULL){
        head = temp;p=head;
    }else{//add new node at the tail of the list
        while(p->next!=NULL) p=p->next;
            p->next = temp;
    }
   // head = temp;
    return temp;
}

void free_wp(WP *wp){
      
    WP* temp=head;
    WP* start=free_;

    if(start==NULL) {free_ = wp; start = free_;}
    else{
        while(start->next!=NULL) start=start->next;
        start->next=wp;
    }//add the node at the tail of the list

   if(head==NULL) assert(0);

   if(head->NO==wp->NO){
        head=head->next;//the delete node at the HEAD of the head list 
   }else{
        while(temp->next!=NULL&&temp->next->NO!=wp->NO) temp=temp->next;
        if(temp->next==NULL&&temp->NO==wp->NO) printf("no possible!\n");
        else if(temp->next!=NULL&&temp->next->NO==wp->NO) temp->next=temp->next->next;//delete the NO node
        else{ printf("There is no #%d in watchpoints pool!\n",wp->NO); }
   }

   wp->next=NULL;
   wp->val=0;
   wp->expr_str[0]='\0';
   wp->NO=0;//invalid node

//            
//    if(start->NO==wp->NO){
//       temp=free_;
//       free_=start;
//       free_->next=temp;//add free node at the head of list
//       head=start->next;        
//    }else{
//        for(;start->next!=NULL;start=start->next){
//        if(start->next->NO == wp->NO){
//            temp = free_;//add node to free_
//            free_=start->next;
//            free_->next = temp;
//
//            start->next = start->next->next;//remove node 
//        }
//        }
//  
//
//    }
//
 }

bool check_wp(){
    bool success=true;
    bool key=true;
    WP *p=head;
    uint32_t value;
    while(p!=NULL){
        value=expr(p->expr_str,&success);
        if(success==false) {printf("wrong expr\n");assert(1);}
        else{
            if(value!=p->val){
               // if(p->NO!=0){//
                    printf("Hit watchpoint at #%d at 0x%08x\n",p->NO,cpu.pc);
               //     p=p->next;
               //     continue;
               // }
        	    printf ("Watchpoint %d: %s\n",p->NO,p->expr_str);
				printf ("Old value = %d 0x%08x\n",p->val,p->val);
				printf ("New value = %d 0x%08x\n",value,value);
				printf ("\n");
				p->val = value;
				key=false;
            }
            
      }
     p=p->next;
    }
//	Log("in checking watchpoints!\n");

	return key;
  
}


void delete_wp(int num){
	WP* p;
	p=&wp_pool[num];
	free_wp(p);

}

void info_w(){
	WP* p=head;
	while(p!=NULL){
		printf("Watchpoint #%d: %s=%d  0x%08x\n",p->NO, p->expr_str, p->val,p->val);
        p=p->next;
	}
	
}

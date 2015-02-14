/* links.h: double-link support */
#define double_link(structure,head,tail,fail_msg) {               \
	structure *newstr;                                            \
	newstr= (structure *) malloc(sizeof(structure));              \
	outofmem((void *) newstr,"%s\n",fail_msg);                    \
	if(tail) (tail)->nxt= newstr;                                 \
	else     head       = newstr;                                 \
	newstr->prv= tail;                                            \
	newstr->nxt= (structure *) NULL;                              \
	tail       = newstr;                                          \
	}
#define delete_double_link(structure,str,head,tail) {             \
	structure *old= str;                                          \
	if(old->prv) old->prv->nxt= old->nxt;                         \
	else         head         = old->nxt;                         \
	if(old->nxt) old->nxt->prv= old->prv;                         \
	else         tail         = old->prv;                         \
	free((char *) old);                                           \
	}

/* stralloc: allocates new memory for and copies a string into the new mem */
#define stralloc(ptr,string,fail_msg) {                             \
	ptr= (char *) calloc((size_t) strlen(string) + 1,sizeof(char)); \
	outofmem(ptr,"%s: <%s>\n",fail_msg,string);                     \
	strcpy(ptr,string);                                             \
	}

#ifndef _H_MAILBOX
#define _H_MAILBOX

#include <stdint.h>

typedef int mailbox_t;

void mailbox_init ( );

mailbox_t mailbox_create ( uint32_t capacity );
void mailbox_destroy ( mailbox_t mbox );

int mailbox_recv ( mailbox_t mbox );
int mailbox_send ( mailbox_t mbox, int msg );


#endif

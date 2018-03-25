#include "mailbox.h"
#include "semaphore.h"
#include "memory.h"
#include "arm.h"

#define MAILBOX_NB 8

enum
{
    MAILBOX_FREE,
    MAILBOX_USED,
};

struct mailbox_s
{
    int state;

    uint32_t count;
    uint32_t first;
    uint32_t capacity;

    sem_t recv_sem;
    sem_t send_sem;

    int * data;
};

static struct mailbox_s mailboxes [ MAILBOX_NB ];

void mailbox_init ( )
{
    for ( int i = 0 ; i < MAILBOX_NB ; ++i )
    {
        mailboxes [ i ].state = MAILBOX_FREE;
        mailboxes [ i ].count = 0;
        mailboxes [ i ].first = 0;
        mailboxes [ i ].capacity = 0;
        mailboxes [ i ].data = 0;
    }
}

mailbox_t mailbox_create ( uint32_t capacity )
{
    if ( capacity == 0 )
    {
        return -1;
    }

    uint32_t irqmask = irq_disable ( );

    for ( int i = 0 ; i < MAILBOX_NB ; ++i )
    {
        if ( mailboxes [ i ].state != MAILBOX_FREE )
        {
            continue;
        }

        sem_t recv, send;
        if ( ( recv = sem_create ( 0 ) ) < 0 )
        {
            irq_restore ( irqmask );
            return -1;
        }

        if ( ( send = sem_create ( capacity ) ) < 0 )
        {
            sem_destroy ( recv );
            irq_restore ( irqmask );
            return -1;
        }

        int * data = memory_allocate ( capacity * sizeof ( int ) );
        if ( ! data )
        {
            sem_destroy ( recv );
            sem_destroy ( send );
            irq_restore ( irqmask );
            return -1;
        }

        struct mailbox_s * pmbox = & ( mailboxes [ i ] );

        pmbox -> state = MAILBOX_USED;
        pmbox -> count = 0;
        pmbox -> first = 0;
        pmbox -> capacity = capacity;

        pmbox -> recv_sem = recv;
        pmbox -> send_sem = send;

        pmbox -> data = data;

        irq_restore ( irqmask );
        return i;
    }

    irq_restore ( irqmask );
    return -1;
}

void mailbox_destroy ( mailbox_t mbox )
{
    // Bound check
    if ( mbox < 0 || mbox >= MAILBOX_NB )
    {
        return;
    }

    uint32_t irqmask = irq_disable ( );

    struct mailbox_s * pmbox = & ( mailboxes [ mbox ] );

    // Nothing to do
    if ( pmbox -> state == MAILBOX_FREE )
    {
        irq_restore ( irqmask );
        return;
    }

    // Destroy mailbox
    memory_deallocate ( pmbox -> data );
    sem_destroy ( pmbox -> recv_sem );
    sem_destroy ( pmbox -> send_sem );

    pmbox -> count = 0;
    pmbox -> first = 0;
    pmbox -> capacity = 0;
    pmbox -> data = 0;
    pmbox -> state = MAILBOX_FREE;

    irq_restore ( irqmask );
}

int mailbox_recv ( mailbox_t mbox )
{
    // Bound check
    if ( mbox < 0 || mbox >= MAILBOX_NB )
    {
        return -1;
    }

    uint32_t irqmask = irq_disable ( );

    struct mailbox_s * pmbox = & ( mailboxes [ mbox ] );

    if ( pmbox -> state != MAILBOX_USED )
    {
        irq_restore ( irqmask );
        return -1;
    }

    wait ( pmbox -> recv_sem );

    // Recheck whether mailbox still exists
    if ( pmbox -> state != MAILBOX_USED )
    {
        irq_restore ( irqmask );
        return -1;
    }

    // Retrieve message and update mailbox
    int msg = pmbox -> data [ pmbox -> first ];
    pmbox -> first = ( pmbox -> first + 1 ) % pmbox -> capacity;
    pmbox -> count--;

    // Signal the freed space to the sender
    signal ( pmbox -> send_sem );

    irq_restore ( irqmask );
    return msg;
}

int mailbox_send ( mailbox_t mbox, int msg )
{
    // Bound check
    if ( mbox < 0 || mbox >= MAILBOX_NB )
    {
        return -1;
    }

    uint32_t irqmask = irq_disable ( );

    struct mailbox_s * pmbox = & ( mailboxes [ mbox ] );

    if ( pmbox -> state != MAILBOX_USED )
    {
        irq_restore ( irqmask );
        return -1;
    }

    wait ( pmbox -> send_sem );

    // Recheck whether mailbox still exists
    if ( pmbox -> state != MAILBOX_USED )
    {
        irq_restore ( irqmask );
        return -1;
    }

    pmbox -> data [ ( pmbox -> first + pmbox -> count ) % pmbox -> capacity ] = msg;
    pmbox -> count++;

    // Signal the new message to the receiver
    signal ( pmbox -> recv_sem );

    irq_restore ( irqmask );
    return 0;
}

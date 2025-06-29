.globl outb

/*
outb - send a byte to an I/O port
stack: 8(%esp) the data byte
       4(%esp) the I/O port
       %esp    return address
*/
outb:
    movb 8(%esp), %al   // move the data to be sent into the al register
    movw 4(%esp), %dx   // move the address of the I/O port into the dx register
    out %al, %dx        // send the data to the I/O port
    ret                 // return to the calling function

Bridget Rassell v00804180

README :)

make to compile.

usage:
./rdpr receiver_ip receiver_port receiver_file_name
./rdps sender_ip sender_port receiver_ip receiver_port sender_file_name

deisgn:

rdpr:


The receiver listens for then syn, and will not continue until a syn is received.
Upon receiving the syn, rdpr enters a loop where it listens for incoming data, fin or rst packets. Rdpr tests to make sure that the packets recieved are unique, and if they are unique rdpr inserts the packets into a linked list until all packets have been received.

upon receiving the fin packet. rdpr iterates through the linked list to make sure all seq no from 1-fin-seqno have been received. If the tests pass, rdpr writes to a file specified by receiver_file_name.

Select is used with a timeout of 6 seconds before writing to the file.

rdps:

rdps runs on 2 threads. a thread for sending packets and a thread for receiving packets.

once a packet is sent, it is inserted into a linked list.

once an ack packet is received, rdps removes that packet from the list. Once the list is empty, rdps knows that teh reciever has recieved all the data packets and can then timeout to close the connection.

I wrote a function that resends packets that are still in the linked list. this function sleeps for a multiple of the RTT then iterates through the entire list to resend the packets.



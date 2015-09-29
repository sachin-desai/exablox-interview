# Sachin Desai
# Exablox Interview project
# Copyright 2015
# https://github.com/sachin-desai/exablox-interview.git

Q1. THE WEIGHT OF DATA
--------------------------------------------------------------------------------
Assume you are going to transfer a file from one computer to another on floppy
disks, that can only hold 1024 bytes of data per floppy (and a little bit of
room for metadata, perhaps a barcode on the label). You have an unlimited number
of these floppy disks, but no network connection - that is, there is no way to
transmit data between the computers or share data between them, apart from the
floppies.

Using files to simulate floppies, write a program, or programs, that:
1) -- Given a file of arbitrary size, write a function that can split the file
into N floppy disks and then write the function to reassemble the file from N
floppy disks. The first disk is guaranteed to arrive intact first, but every
other disk may arrive in any order, and there may be some extraneous disks from
some other file(s) mixed in which got mis-delivered.  By the end of the process,
the recipient must verify that it has an identical file to the sender to a very
high level of certainty, perhaps using a cryptographic integrity check or
similar mechanism.
2) -- Assume the floppy disks are very heavy, and you want to minimize shipping
costs.  On the upside, the recipient may already have the file, or a substantially
similar file.  Get the new file to the recipient while sending only the minimal
number of floppy disks each direction, even if some floppies get reordered, lost
or corrupted en route (but again, the first disk is guaranteed to arrive first,
intact).

A1. PROPOSAL
--------------------------------------------------------------------------------
1) -- A block based solution has been chosen to tackle this requirements
scenario. A given file of N bytes, is broken up into (N/datasize) blocks, or
floppies. Within the block, eight bytes represent the index, for a given
floppy. This allows for a total of 2^64 floppies per file transfered. To meet
the data integrity requirement, each block is encrypted using SHA2, along with a
shared secret key between sender and receiver. To ensure the cryptographic hash
value is not corrupted inflight, a CRC-32 calculation is appended. Each floppy
would have information in the following format:

id:     8 bytes
data: 980 bytes
sha2:  32 bytes
crc:    4 bytes

The first floppy, guaranteed to  be intact, contains the file length, index
value, SHA2 checksum of the entire file, and CRC-32 signature.

The receiver, upon receiving the first floppy, checks the SHA2 key to see if the
file already exists. In the event of a mismatch, the file is assembled from the
out-of-order floppies. In the event of a checksum mismatch, or a missing index,
the receiver is unable to reconstruct the file, indicating a breach or
corruption.

2) -- To minimize the number of floppies mailed, a data dedupe scheme, based on
Merkle hash tree is proposed. This minimizes the the costly operation of
shipping heavy floppies, until requested by the receipient. However, the tree
would aggregate floppies in groups of 1MB to reduce height, and minimize costly
hash operations.

Q2. CREATE A QUESTION
--------------------------------------------------------------------------------
Compose a programming challenge question, along with its answer, that you would
like to have been asked.  This should be a question where the answer exhibits
some aspect of your skills better than the other questions and answers.

A2. PROPOSAL
--------------------------------------------------------------------------------
The secret key shared between sender and reciever can be easily compromised in
transit, or post receipt. What mechanism would you utilize to ensure security
and prevent a man in the middle attacks?

Would the nature of file updates (static vs. dynamic) affect your choice of
Merkle tree implementation? Would a Bloom filter be a better fit?

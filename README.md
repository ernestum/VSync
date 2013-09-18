ValueSync
=========

An Arduino library to easily synchronize integer values between arduinos (via the Serial interface eg. using XBee)


Installation:
-------------

Just clone this repository into you libraries folder and you are done!


Tip for debugging:
------------------

To make two arduinos that are connected to you linux machine via USB talk to each other just type:

`dd if=/dev/tty<ALICE> bs=1 of=/dev/tty<BOB> bs=1`

(Adapt the <ALICE> and <BOB> part according to the adresses on your computer. Alice is the sending arduino while Bob is the receiving end)

If this does not work right away try `screen /dev/tty<ALICE>` and exit it with ctrl+a, k, y. Then try the above line again.

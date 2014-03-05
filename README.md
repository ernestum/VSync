VSync Library
=========

This is the VSync library for the arduino platform. It can 'magically' synchronize variables on your Arduino with variables on other Arduinos or in your Processing sketch.

Use cases:
----------
For example if you read the heading off a digital compass and store it in a variable
on your Arduino, you can sync that variable with a Processing sketch. In the Processing
sketch you can use it to visualize the digital compass reading.

Maybe you have many settings (color of a RGB LED; angle of a servo motor; parameters
for a PID algorithm ...), that often need to be changed during the runtime of your Arduino.
Simply synchronize the position of a sliders in your Processing sketch
with the arduino to transfer transfer a parameter.

Technical details:
------------------

(boring, don't read if you just want to use the library)
    
VSync uses Serial to synchronize int variables, which means that it will work through your
USB cable as well as with your XBee. 
It also means it can only do integers and will block any other (e.g. debugging) uses of
the Serial interface.
    
The library uses a very simple human readable protocol that is optimized for minimal 
data traffic (e.g. only when a value changes it is sent over the line).
The protocol is package based with two kinds of packages:
  * Full-Sync-Package: contains the values of all variables
  * Diff-Sync-Package: contains the values only of variables that changed
  
The Full-Sync-Package looks like this: A<val1>|<val1>|<val3># if you are syncing three
variables. E.g if the variables have the values 4, -8, 555 the package looks like:

A|4|-8|555#

The A at the beginning stands for all values, the # is the end-charakter.
The Diff-Sync-Package looks like this: 0|4|2|555# if the first and the last value
changed to 4 and 555. If only few (1 or 2) variables changed a Diff-Sync-Package is sent;
if lots of variables changed it is more efficient to send a Full-Sync-Package, because
the id's of the variables do not need to be sent.

Installation:
-------------

Just clone this repository into you libraries folder and you are done!


Tip for debugging:
------------------

To make two arduinos that are connected to you linux machine via USB talk to each other just type:

`dd if=/dev/tty<ALICE> bs=1 of=/dev/tty<BOB> bs=1`

(Adapt the <ALICE> and <BOB> part according to the adresses on your computer. Alice is the sending arduino while Bob is the receiving end)

If this does not work right away try `screen /dev/tty<ALICE>` and exit it with ctrl+a, k, y. Then try the above line again.

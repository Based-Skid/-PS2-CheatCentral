www.CheatersLounge.org
www.Cheaters-Lounge.net

Yeah, pre-beta release...

Supposed to have something important to say here...

This is PRE-BETA Release, MEANING HALF OF THE FEATURES ARE EITHER BUGGY OR NOT IMPLEMENTED!
Well, for what is implemented, enjoy =D!

You'll need to create a folder on your memory card called "CheatersLounge" in order for
the IP config to be able to save.

Update 0.1a: 1.3 Engine implemented into RemotePS2.
Update 0.1d: RemotePS2 Control Panel fully functional.
             RemotePS2 Archive Manager fully functional.
             RemotePS2 Server Archives fully functional.

Note: I take no credit for the IRX modules implemented in the application.
I take full credit for my work (the applications themselves).

~ Gtlcpimp


The current implemented command structure 1.3 engine (included in RemotePS2):

__ 0 - 8 Bit Constant Write
__ 1 - 16 Bit Constant Write
__ 2 - 32 Bit Constant Write
__ 3 - Increment / Decrement
__ 4 - 32-bit Multiple Address Write
__ 5 - Copy Bytes
__ 6 - Pointer Write
__ 7 - 32 Bit Boolean
__ 8 - Find and Replace
__ 9 - Execute Data
__ A - (not implemented)
__ B - (not implemented)
__ C - (not implemented)
__ D - Boolean: Half (Aka: Joker)
__ E - Line Specific Boolean: Half (Aka: Line Specific Joker)
__ F - Extended Command Structure
__ F0 - IOP 8 Bit Constant Write
__ F1 - IOP 16 Bit Constant Write
__ F2 - IOP 32 Bit Constant Write
__ F3 - (not implemented)
__ F4 - (not implemented)
__ F5 - (not implemented)
__ F6 - (not implemented)
__ F7 - (not implemented)
__ F8 - (not implemented)
__ F9 - (not implemented)
__ FA - (not implemented)
__ FB - (not implemented)
__ FC - (not implemented)
__ FD - (not implemented)
__ FE - (not implemented)
__ FF - (not implemented)


The information on the command structure:


-== Code Command Information ==-

0, 1, 2, F0, F1, F2


-- Constant Writes (0, 1, 2, F0, F1, F2) --

Code Setup (0, 1, 2): CAAAAAAA DDDDDDDD
Code Setup (F0, F1, F2): CCAAAAAA DDDDDDDD
C - Command
A - Address
D - Data

Description:
If not jokered, these codes are constantly writing the given data to the specified address.

-------------------------------------

3

-- Increment / Decrement --

8 Bit Increment
300000nn aaaaaaaa

8 Bit Decrement
301000nn aaaaaaaa

16 Bit Increment
3020nnnn aaaaaaaa

16 Bit Decrement
3030nnnn aaaaaaaa

32 Bit Increment
30400000 aaaaaaaa
nnnnnnnn 00000000

32 Bit Decrement
30500000 aaaaaaaa
nnnnnnnn 00000000

a = Address
n = Amount To Increase / Decrease

Description:
Increments or decrements the value contained at the address by the amount of data given every time this code is applied by the cheat engine.

-------------------------------------

4

-- 32 Bit Multiple Address Write --

4aaaaaaa wwwwssss
dddddddd nnnnnnnn

a = Starting Address
w = Number Of Lines To Write
s = Number Of Lines To Skip
d = Data To Write
n = Ammount To Increase Given Data By

Description:
Writes the given data to the address specified. It also writes the same data to as many lines specified. The data can be increased for each line, and the number of lines to write can be spaced out rather than being consecutive.

-------------------------------------

5

-- Copy Bytes --

5fffffff nnnnnnnn
tttttttt 00000000

f = Address To Copy From
t = Address To Copy To
n = Number Of Bytes To Copy

Description:
Copies the given number of bytes from the first address, and writes those bytes to the second address.

-------------------------------------


6

-- Pointer Write --


Pointer start
6aaaaaaa nnnnnnnn
0600iiii *n
0C00iiii
dddddddd
FFFFFFFF (Padding if needed, used to prevent null address + data lines)

Example 1 (No Padding Needed):
60100000 00000003
06000001 06000001
06000001 02000004
12345678 00000000

Example 2 (Padding Needed):
60100000 00000001
06000001 02000004
00000000 FFFFFFFF (Storing null data will cause us to NEED padding to prevent the double null)

a = Starting pointer address
n = Number of points
c = Write type (8 / 16 / 32 Bit): 1 - Byte 2 - Half 3 - Word
d = Data to write

Description:
Reads the address from the given address, and writes the given data to that address. If more than one pointer are needed, then it will keep reading the address contained at the current address it has loaded for as many pointers specified (n) and write the data to the final address.

-------------------------------------

7

-- 32 Bit Boolean --

7aaaaaaa dddddddd

a = Address to read
d = Data to compare

Description:
Compares the 32 bit data located at the address to the provided 32 bit data in the code. If the values are not equal, the code chunk assigned to this command will not be executed. If the values are equal, then they will.

-------------------------------------

8

-- Find and Replace --

8 Bit Search
8aaaaaaa 000000ff
000000rr 0000iiii

16 Bit Search
8aaaaaaa 0000ffff
0000rrrr 0010iiii

32 Bit Search
8aaaaaaa ffffffff
rrrrrrrr 0020iiii

a = Address to start
f = Data to find
r = Data to replace with
i = Search count

Description:
Searches for the first data, and replaces any matches with the second data. The search count is to limit how much it will search.

-------------------------------------

9

-- Execute Data --

9fffffff aaaaaaaa

f = Address of function (also called a sub routine)
a = Argument sent to function (sent in register a0)

Description:
In basic terms, best method to hook routines without having to find a hook in the game.

-------------------------------------

E

-- Line Specific Boolean Half --

Eaaaaaaa nnnidddd

a = Address to read
n = Number of lines assigned to boolean
i = Check if equal, or if not equal (0 = equal, 1 = not equal)
d = Data to check


Description:

Checks if the 16 bit data at the address is equal (if i is 0) or not equal (if i is 1) to the 16 bit data given. If the check returns True, then the number of code lines assigned to the boolean will be executed. If the check returns False, then the number of code lines will be skipped.

===========================

Created by: Gtlcpimp

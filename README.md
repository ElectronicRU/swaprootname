swaprootname - root window name swapper
=======================================
Use
	swaprootname "New name"
to persistently change root window name, or
	echo "New name1\nNew name2\n..." | swaprootname
to temporarily set root window name(s) and revert it to original one on EOF.

Building:
	make

Installation:
	make install # as root


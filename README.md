pstree
======

pstree displays currently running processes in a tree structure. User can provide a root pid for the tree, by default the root pid is 0.

Program was developed and tested on an Ubuntu 12.04 system.



make-tree
=========

Test file used to create process trees. Required arguments are --width <width> and --depth <depth>. Each process with have width children and the total depth of the process tree is defined by depth.

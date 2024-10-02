#!/bin/bash

# 1
cd ~/Desktop/

mkdir test

chomd 660 test

ls -l 


# 2
#groups <username > or id -nG <username>
groups amir

chown root:sudo test # give the file with user: root and gorup: test

# 3
#‫‪ chmod‬‬ 4664 file.txt

give the others read only
groups and user have read and write 
also give the suid for the user (let user start some thing and contiue by the root)

** more about suid:This means when a file is executed, it will run with the permissions of the
 group that owns the file rather than the permissions of the user who executed it.
 For directories, this means files created in the directory will inherit the group ownership of the directory.

# 4
find -empty -exec rm -f {} \; 

# 5
when you go in less mode you can use / and ? for search a patterns so you can use /pattern or ? pattern
/ is forward search and ? is backward search

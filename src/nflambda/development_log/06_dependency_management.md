# Goal: Dependency Management

## Problem Statement

- Maintain a document in the nflambda folder to track the dependencies of the files and modules, ensure that changing of one file/module does not break the others

## Step

1. Create a document called 00_dependency_management.md in the nflambda folder
Repeat if necessary
2. List all the files and modules in the file at the first section 
   Definitions:
    - Module is defined as a folder with a CMakeLists.txt file
    - File is defined as a .c or .h file
   Steps:
    - create a python script that lists all the files and modules
    - the script should output the results to the 00_dependency_management.md file which overwrites a section (do not remove other sections)
3. List the dependencies as a graph, or a list of edges 
done Repeat
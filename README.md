# 354H-FinalProj
CS 354H Computer Graphics Final Project --> https://www.cs.utexas.edu/~graphics/s24/cs354h/final/overview/

[ Y ] Implement Catmull-Clark.\
[ Y ] Implement Loop subdivision.\
[ Y ] Add extensions to handle extraordinary vertices and sharp creases.\
[ Y ] Build a tool that shows the limit surface.\
[ Y ] Add functionality to view intermediate subdivision levels.

To compile, use the following commands:

make clean

make

NOTE: This assignment has been tested on Ubuntu 22.04. If you want to run this on a Mac, run the 'make smoothing-apple' command instead of 'make'. 


To run, execute the following command:

./smoothing

Select the file you want to run subdivision on and the subdivision algorithm you want to use. Set the slider value and click 'Process'. 
If you want to see the limit surface, click the 'Limit surface' button. 

## Summary
### GUI
Added file browser, drop down, process button, slider and about menu
### Render window
A separate executable, triggered from within control flow
### Loop Subdivision
Needs input file as trimeshes
### Catmull Clark
Needs input file as quadfaces
### Limit surfaces
We have precomputed the limit surfaces, upon pressing the 'limit surface' button the program picks the limit surface file corrresponding to your input file. You can also recompute the limit surface if you wish by setting the slider value to 6,5 or 4, depending on how many vertices/faces your input file has and the limits of your system. 
## Sharp creases
For catmull clark, a seperate 'Catmull Clark Sharp Creases' option should be selected to handle sharp creases. Your input quad file should have a 's' after every sharp vertex (corresponding to an endpoint of a sharp edge) and a 'n' after every non sharp vertex.

For Loop, sharp vertices are specified with the lines 'cv' followed by the index of the sharp vertex. No seperate option has to be selected. 
## Extra credit survey
We have both submitted the online course instructor survey.

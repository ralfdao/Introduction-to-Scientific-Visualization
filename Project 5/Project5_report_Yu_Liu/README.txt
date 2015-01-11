The codes have been tested under Win7+VS2010.
 
The graphic card I am using is ATI Radeon HD 3400 Series.

Solution file ("Project5.sln" is the main one) and project files (.vcproj) are included to build the projects.

The datasets are passed to the applications through command line in the following orders.

"threeplanes.exe" "full path of the delta wing geometry dataset" "full path of the velocity vector dataset"
"streamlines.exe" "full path of the delta wing geometry dataset" "full path of the velocity vector dataset"
"streamtubes.exe" "full path of the delta wing geometry dataset" "full path of the velocity vector dataset"
"streamsurface.exe" "full path of the delta wing geometry dataset" "full path of the velocity vector dataset"

"combined.exe" "full path of the delta wing geometry dataset" "full path of the velocity vector dataset" "full path of the vorticity magnitude dataset"
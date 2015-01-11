The codes have been tested under Win7+VS2010.
 
The graphic card I am using is NVIDIA NVS 5400M. 

Solution files (.sln) and project files (.vcproj) are included to build the projects.

If Visual Studio cannot find the vtk head files and lib files, reset the include directory and library directory to your local ones.

The datasets are passed to the applications through command line in the following order.

"HeightMap.exe" "full file path of bathymetry vtk file"

"EarthMap.exe" "full file path of bathymetry vtk file" "full file path of texture image file" "full file path of sphere vtk file"

"SeaRise.exe" "full file path of bathymetry vtk file" "full file path of texture image file" "full file path of sphere vtk file"

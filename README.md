# bscor: Beam Scaffolded Origami Routing

## INTRODUCTION

The program *bscor* is a pipeline of executables for routing triangulated polyhedal meshes and relaxing a physical cylinderical model of the mesh so that the mesh can be assembled from DNA at the nanoscale. The source code for the physical relaxation is found in the repository https://github.com/gardell/scaffold-routing-rectification.
	
## INSTALLATION
	Copy the parent directory to your desired location. To access the binaries and the batch file from other locations, you 
	can pad your BSCoR location to the path environment variable.
	
## USAGE
	`bscor.bat model_ply_file [scale]`
	
	The intended usecase of bscor is as follows: 
	1) The user builds a triangulated polyhedral mesh in a 3d modelling software such as Maya. Maya is nevertheless needed to import the DNA model using vHelix.
	2) The resulting model is exported in a standard format (e.g. vrml, obj, stl) which can then be converted into PLY (e.g. using meshconv).  
	3) The user then opens a command window and goes to the bscor installation directory
	4) The user then runs the command `bscor.bat octahedron.ply`, where octahedron.ply is the filename of target PLY model.
	   Optionally, the user can specify a scale parameter as in 'bscor.bat octahedron.ply 10', for scaling by 10. 
	5) If the execution was successful, bscor provides an inputfile.rpoly output.
	6) If the execution was unsuccesful, the user can troubleshoot the problem from the error messages output by bscor.
	7) The resulting rpoly can be imported into maya in with the vhelix plugin for Maya. vhelix can be downloaded and installed from vhelix.net.
	8) In vhelix, further processing of the DNA strands composing the target polyhedra can be done. Desired sequences can be applied to the DNA strands in vhelix. 
	
	The coordinates in the PLY file are assumed to be in nanometer. Models exported from maya may have small dimensions in this unit. That is,
	the models may not be feasible to implement using DNA. BSCoR provides an optional argument to set the scale of the model. For instance,
	to scale the octahedron model by 2 for rendering in DNA, execute 'bscor.bat octahedron.ply 2' from the command line.
	
## LIST OF FILES

The following files must be present in the installation:
* bscor.bat
* ply_to_dimacs.exe
* postman_tour.exe
* embed_planar.exe
* Atrail_search.exe
* Atrail_verify.exe
* scaffold-routing-rectification.exe
* PhysX3_x64.dll
* PhysX3Common_x64.dll
* octahedron.ply

Authors information
--------------------

Antonio J. Rueda
ajrueda@ujaen.es

Jose M. Noguera
jnoguera@ujaen.es
http://noguerar.wordpress.com

University of Jaen (Spain). 

Paper details
--------------

June 11st, 2013.

Title: *"A flooding algorithm for extracting drainage networks from unprocessed digital elevation models"*

Journal: *"Computers & Geosciences"*

Folders
--------

* ./src/ Contains the source code in C++.
* ./win32/ Contains two projects (.vcprof for Microsoft Visual Studio and .pri for Qt Developer) to compile the application.
* ./dataset_301.hgt A small DEM (size 301x301 cells) for testing purposes. Extracted from the NASA SRTM 2.1 (http://dds.cr.usgs.gov/srtm/).

Additional information
-----------------------

Qt (http://qt-project.org/) is recommended but not required to compile the source code. Without Qt, the output drainage can only be produced in PLY format (which can be visualized with MeshLab). With Qt, all standard image formats are supported.

Once compiled, just run the program from a text terminal to check out the input parameters.

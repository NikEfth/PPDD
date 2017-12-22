# Sino_viewer (v0.1)
A sinogram viewer for interfiles. 

This is a simple (so far) sinogram viewer, based on QT5 supporting interfiles. The [STIR toolkit](https://github.com/UCL/STIR) 
is the backend for the projection data and [QWT](http://qwt.sourceforge.net/) is used for the plotting. 

![screenshot from 2017-12-22 19-04-00](https://user-images.githubusercontent.com/8995070/34312027-062d41b6-e759-11e7-909a-fddcad1e96c6.png)

## Warning (22/12/17) 
The CMake file is still a bit of a mess, so please be sure the you have installed STIR and QWT before hand. 

## Installation
### Linux systems
#### Dependencies
1. Download and compile the git version of STIR. This stable version at sourceforge has not been tested. 
2. Either install on a global location (e.g. `/usr/local`) or define your custome location in `$PATH`. 
3. Download and install QWT **>v.6.xx**, preferably in a global location (some distributions provide packages for this).

*These are the only two dependencies. All other in the CMAKELISTS.txt will be dropped. In one of the next commits. *
#### Sino_viewer
1. Clone the repository [Sino_viewer](https://github.com/NikEfth/sino_viewer).
2. **warning** for the time being, if you have not installed STIR on a global location you have to edit the CMAKELISTS.txt. 
Replace the STIR path from the current to the one on your system. 
3. If QWT is global it will be detected automatically. 

4. Create a new folder `sino_viewer.build` outside the main folder of the project. 
5. Go to `sino_viewer.build` and run `ccmake [SOURCE_PATH]`.
6. `make all`
7. (optionally) `make install`



### Other systems
No idea, really! 

### How to use
#### Load sinogram
1. Use the `File :: Open sinogram` option and select a Interfile header file (.hs). *As STIR framework expands in the future, any supported file types (that is stored as ProjData) will be, probably automatically, supported*.
*No questions will be asked, all data will come from the header file.*
2. A pop-up window will appear to allow to select between sinogram and viewgram view mode. Once selected this cannot change. The control widgets will be updated to the appropriate sizes.
3. Different number of viewports are supported. 
4. Colormaps can be selected. 
![screenshot from 2017-12-22 19-03-31](https://user-images.githubusercontent.com/8995070/34312028-064be008-e759-11e7-82bf-0e7eca3188f5.png)

#### Save a sinogram
Any sinogram can be saved either as image file (PNG) or as interfile (.hv & .v).
By right click on the sinogram, a context menu will appear. 

![screenshot from 2017-12-22 19-04-18](https://user-images.githubusercontent.com/8995070/34312026-0607aa0a-e759-11e7-9716-889a0a7d22f3.png)



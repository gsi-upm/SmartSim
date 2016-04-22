# SmartSim
SmartSim project by GSI-UPM

## Introduction

This is a Graphical User Interface for an evacuation plans simulator. It makes use of UbikSim and SmartBody to fully perform the simulation and the interface.

## Installation

SmartSim provides a connection between UbikSim and SmartBody. To install it we have to install the other two programs.

### Installing SmartBody

SmartBody has some package depencies. To install these packages we can execute in the terminal

>  sudo apt-get install cmake g++ libxerces-c3-dev libgl1-mesa-dev libglu1-mesa-dev xutils-dev libxi-dev freeglut3-dev libglew-dev libxft-dev libapr1-dev libaprutil1-dev libcppunit-dev liblapack-dev libblas-dev libf2c2-dev build-essential mono-devel mono-xbuild python-dev libopenal-dev libsndfile-dev libalut-dev ncurses-dev fltk1.3-dev libtool libzzip-dev libxaw7-dev libxxf86vm-dev libxrandr-dev libfreeimage-dev nvidia-cg-toolkit libois-dev libogre-1.9-dev protobuf-compiler libboost-all-dev

After this, we build the SmartBody libs by running the linuxlibsbuild.sh script. This script compiles and installs the SmartBody dependencies.

SmartBody installation is performed using cmake. The creation of a build folder is advised.

To create the build folder and run the installation, the following commands must be run in a terminal.

> 
```  
mkdir buildfolder
sudo cmake ..
sudo make install

### Installing UbikSim

To install UbikSim we need a running Apache Tomcat 7, Java 1.7 and Java 3d. This project can be installed both as an Eclipse or Netbeans project.

The project is installed in the Tomcat server. The base path to UbikSim is APACHEPATH/UbikSimMOSI-AGIL-Server/

### Installing SmartSim

In order to install SmartSim, the SmartSim modules and the scene must be copied to the smartbody directories.

To install the scene, the scene folder must be moved to smartbody/data

The scripts in script folder must be moved to smartbody/data/scripts

## Configuring SmartSim

To configure SmartSim, a configuration file must be provided. 'SmartSimSettings.cfg' must be the name of the file. The file must be placed in the smartbody/bin folder.

This file is used to configure several SmartSim options such as the number of agents displayed or the path to the UbikSim server.

An example of this file with all the available options can be found in the config folder.  

## Running SmartSim

To run any of the SmartSim simulator we must run first UbikSim and SmartBody. Once the UbikSim simulation is started we can run with SmartBody any of the SmartSim use cases. The use cases files are

>
```
SmartSimOneCharacter.py
SmartSimLeader.py
SmartSimSimulator.py
SmartSimBehaviors.py
SmartSimCharacterTypes.py
```

## Use cases

We have defined some use cases in order to take advantage of the SmartSim possibilities.

- The SmartSimOneCharacter.py simulation sets a simulation with only an agent.

- The SmartSimLeader.py sets a simulation with groups of characters following the leader of each group.

- The SmartSimSimulator.py sets a simulation with many agents with their own path.

- The SmartSimBehavior.py adds emotions to agents of the multi-agent simulation.

- The SmartSimCharacterTypes.py extends the variety of agents displayed.

## Controlling the simulation

In all of the aforementioned simulations, the user can stop or play the simulation as he wishes. To play the simulation, using in the command window the command

> play()

will start the simulation. It can be stopped using

> stop()

## Agents creation

In the multi-agents simulations, agents can be added to the simulation. The command to create them is

> createAgent('name', 'x coordinate of his position', 'y coordinate of his position')

Additionally, in the character types simulator is possible to change the type of agent to create using

> createAgent('name', 'x coordinate of his position', 'y coordinate of his position', 'agent type')

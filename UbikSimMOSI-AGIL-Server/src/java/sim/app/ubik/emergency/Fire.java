/*
* 
* 
* This file is part of UbikSim Web Service. UbikSim Web Service is a UbikSim library. 
* 
* UbikSim Web Service has been developed by members of the research Group on 
* Intelligent Systems [GSI] (Grupo de Sistemas Inteligentes), 
* acknowledged group by the  Technical University of Madrid [UPM] 
* (Universidad Politécnica de Madrid) 
* 
* Authors:
* Emilio Serrano
* Carlos A. Iglesias
* 
* Contact: 
* http://www.gsi.dit.upm.es/;
* 
* 
* 
* UbikSim Web Service, as UbikSim, is free software: 
* you can redistribute it and/or modify it under the terms of the GNU 
* General Public License as published by the Free Software Foundation, 
* either version 3 of the License, or (at your option) any later version. 
*
* 
* UbikSim Web Service is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with UbikSim Web Service. If not, see <http://www.gnu.org/licenses/>
 */


package sim.app.ubik.emergency;

import java.util.List;
import sim.app.ubik.Ubik;
import sim.app.ubik.behaviors.PositionTools;
import sim.app.ubik.people.Person;
import sim.app.ubik.representation.Representation;
import sim.engine.SimState;
import sim.engine.Steppable;
import sim.engine.Stoppable;
import sim.util.Int2D;
import sim.util.MutableInt2D;
import ubik3d.model.HomePieceOfFurniture;



public class Fire implements Steppable, Stoppable {
    protected Ubik ubik;
    protected int steps=0;
    protected Int2D initialPosition;
    protected HomePieceOfFurniture fireFurniture;
    protected int advanceOfFirePerStep=1;
    protected int startingSizeOfFire=10;
    
    
    
    
    public Fire(Ubik ubik){
        this.ubik=ubik;
        insertInRandomPosition();
        register();

    }
    
    public Fire(Ubik ubik, Int2D position){
        this.ubik=ubik;
        initialPosition =position;
        register();

    }
/**
 * Method with the actions to be performed by fire in each step.
 * @param ss 
 */
    public void step(SimState ss) {      
        spreadFire();
        steps++; 
    }
    
    /**
     * Method called to check if the person is touching fire
     * @param p
     * @return 
     */
    
    public boolean touching(Int2D p) {      
      return PositionTools.isNeighboring(p.x, p.y, initialPosition.x, initialPosition.y,steps*advanceOfFirePerStep + startingSizeOfFire );
      
    }
    
    /**
     * Get room of the fire
     * @return 
     */
    public Int2D getInitialPos(){
        return initialPosition;
    }
    

 /**
  * Register the fire in the schedule (to make the simulation call the step method in fire for each step).
  */
    
    public void register() {        
        ubik.schedule.scheduleRepeating(this, 1);
    }
     
    public void stop() {
        throw new UnsupportedOperationException("Not supported yet.");
    }
/**
 * Calculate a random position in the map for the fire.
 */
    private void insertInRandomPosition() {
        List<Person> people = ubik.getBuilding().getFloor(0).getPersonHandler().getPersons();
        initialPosition = PositionTools.getRandomPositionInRandomRoom(people.get(0));//getting a random position for the fire                
    }
    
    
    
    /**
     * Insert a fire 3d object in the display. It has to be called if there is GUI
     * Look out the path of the file with the 3D object and the texture
     */
    public void insertInDisplay(){
          try{
            //object 3d imported, it must be in the same folder than the second parameter (inside src and build)
            fireFurniture = Representation.createModel("Fire", Fire.class, "Fire.dae");//includes a 3D object in the display
            MutableInt2D point3D = PositionTools.pointMasonToPoint3D(ubik.getCellSize(),initialPosition.x, initialPosition.y);//the positions in the 3d display are not the same than in the MASON grid
            fireFurniture.setHeight(100);//changing the size of the flame            
            fireFurniture.setWidth(startingSizeOfFire*ubik.getCellSize());
            fireFurniture.setDepth(startingSizeOfFire*ubik.getCellSize());
            fireFurniture.setX(point3D.x);
            fireFurniture.setY(point3D.y);
            ubik.getBuilding().getFloor(0).getHome().addPieceOfFurniture(fireFurniture);
            //JOptionPane.showMessageDialog(null, "There is a fire in position " + initialPosition.x + "," +  initialPosition.y);
        }
        catch(Exception ex){            
            ex.printStackTrace();//"Exception inserting the fire in the display"
        }    
    }
    
    
    private void spreadFire(){ 
        if(fireFurniture!=null){
            ubik.getBuilding().getFloor(0).getHome().deletePieceOfFurniture(fireFurniture);
            fireFurniture.setWidth(fireFurniture.getWidth()+advanceOfFirePerStep*10);
            fireFurniture.setDepth(fireFurniture.getDepth()+advanceOfFirePerStep*10);
            ubik.getBuilding().getFloor(0).getHome().addPieceOfFurniture(fireFurniture);           
        }
    }
    
}

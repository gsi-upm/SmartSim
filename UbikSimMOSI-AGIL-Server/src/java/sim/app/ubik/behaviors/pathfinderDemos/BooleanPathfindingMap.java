
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

package sim.app.ubik.behaviors.pathfinderDemos;

import org.newdawn.slick.util.pathfinding.PathFindingContext;
import org.newdawn.slick.util.pathfinding.TileBasedMap;
import sim.app.ubik.behaviors.PositionTools;
import sim.app.ubik.building.OfficeFloor;
import sim.app.ubik.building.SpaceArea;
import sim.app.ubik.building.connectionSpace.Door;
import sim.app.ubik.building.rooms.Room;
import sim.app.ubik.people.Person;

/**
 * Modificación de versión oficial para no considerar el "Vacio" como espacio válido para andar.
 *  Por defecto no es obstaculo y si lo es :     if(!of.isCorridor(i, j) && !of.isInDoor(i, j) && of.getRoom(i, j)==null)  
 * @author Emilio Serrano, Ph.d.; Universidad Politécnica de Madrid.
 */
public class BooleanPathfindingMap implements TileBasedMap {


    private Person p;
    private int percetionRange;    
    /**
     * Map is static, one for all instances since blocked obstacles are shared
     */
    private static boolean[][] bmap=null;//true if blocked, false if not
    private static int WIDTH;
    private static int HEIGHT;
    private String mapPath;
    
    /**Map depends on person floor and percetion*/
    public BooleanPathfindingMap(Person p, int percetionRange){
       this.p=p;
       this.percetionRange= percetionRange;
       if(bmap==null || !p.getUbik().getPathScenario().equals(mapPath)){
           mapPath=p.getUbik().getPathScenario();
           generateBmap();
       }
       
    }

    
    /**
     * Considered if its wall of if there is  a person in its in perception range (so calling it when finding a mobile object, generates a path considering this)
     * @param ctx
     * @param x
     * @param y
     * @return 
     */
    @Override
    public boolean blocked(PathFindingContext ctx, int x, int y) {
       if(x>=WIDTH ||x<0 ||y<0 || y>= HEIGHT) return true;
       if(bmap[x][y]) return true;
       if(PositionTools.getDistance(p.getPosition().x, p.getPosition().y, x, y)<percetionRange){
           if(PositionTools.getPerson(p, x, y)!=null) return true;
        }
       return false;
    }

    @Override
    public float getCost(PathFindingContext ctx, int x, int y) {
        return 1.0f;
    }

    @Override
    public int getHeightInTiles() {
        return HEIGHT;
    }

    @Override
    public int getWidthInTiles() {
        return WIDTH;
    }

    @Override
    public void pathFinderVisited(int x, int y) {}

    
    /**
     * generates the boolean map of obstacles: true if obstacle.
     * Called only once, map shared for all agents
     */
    private void generateBmap() {        
        HEIGHT = p.getUbik().getBuilding().getFloor(p.getFloor()).getSpaceAreaHandler().getGrid().getHeight();
        WIDTH=  p.getUbik().getBuilding().getFloor(p.getFloor()).getSpaceAreaHandler().getGrid().getWidth();
        bmap= new boolean[WIDTH][HEIGHT];
        for (int i = 0; i < WIDTH ; i++) {
            for (int j = 0; j < HEIGHT; j++) {
               bmap[i][j]=false;               
               OfficeFloor of = p.getUbik().getBuilding().getFloor(p.getFloor());
               bmap[i][j]=false;
               if(!of.isCorridor(i, j) && !of.isInDoor(i, j) && of.getRoom(i, j)==null)  bmap[i][j]=true; //it is an obstacle
        
            }
            
        }
    }



    public static boolean[][] getBmap() {
        return bmap;
    }
    
    public  boolean isBlocked(Person p, int x, int y) {
       if(x>=WIDTH ||x<0 ||y<0 || y>= HEIGHT) return true;
       if(bmap[x][y]) return true;
       if(PositionTools.getPerson(p, x, y)!=null) return true;  
       return false;
    }

}


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
package sim.app.ubik.people;

import com.hp.hpl.jena.sparql.lib.org.json.JSONArray;
import com.hp.hpl.jena.sparql.lib.org.json.JSONException;
import com.hp.hpl.jena.sparql.lib.org.json.JSONObject;
import java.util.logging.Level;
import java.util.logging.Logger;
import org.apache.catalina.tribes.util.Arrays;
import org.newdawn.slick.util.pathfinding.AStarPathFinder;
import org.newdawn.slick.util.pathfinding.Path;
import sim.app.ubik.Ubik;
import sim.app.ubik.behaviors.PositionTools;
import sim.app.ubik.behaviors.pathfinderDemos.Pathfinder;
import sim.app.ubik.behaviors.pathfinderDemos.PathfinderThread;
import sim.app.ubik.building.rooms.Room;
import sim.app.ubik.emergency.Fire;
import sim.engine.SimState;
import sim.util.Int2D;
import ubik3d.model.HomePieceOfFurniture;
import ubiksimdist.ServletToSimulator;

/**
 * Worker person example: uses the pathfinding to reach a random exit (assumed
 * rooms with names "YARD1" to "YATD3"). This goal can be changed online. You
 * can use it in any environment exits named as described, or change the exits
 * name at this class according to your environment. Moreover, the environment
 * has to include an object Worker. It has been tested with mapExample.ubiksim.
 * Agents get out after reaching a goal
 *
 * @author Emilio Serrano, Ph.d.; Universidad Politécnica de Madrid.
 */
public class Worker extends Person {

    private static final Logger LOG = Logger.getLogger(Worker.class.getName());
    private static String globalGoal = null; //goal for all agents     
    private static final String exits[] = {"YARD1", "YARD2", "YARD3"}; //list of poosible goals
    private static int loadOfExits[]={0,0,0}; //users going to each exit
    private String room;
    private boolean stopped = false;

    public void setStopped(boolean stopped) {
        this.stopped = stopped;
    }

    public String getRoom() {
        return room;
    }

    private String localGoal = null;//local goal if no global is fixed
    private Pathfinder pf;

    public Worker(int floor, HomePieceOfFurniture person3DModel, Ubik ubik) {
        super(floor, person3DModel, ubik);

    }

    public void step(SimState state) {

        super.step(state);

        //room inspector
        Room r = PositionTools.getRoom(this);
        if (r == null) {
            room = null;
        } else {
            room = r.getName();
        }

        if (stopped) {
            return; //skip movement
        }

        //go to the yard
        if (pf == null) {//generate pathfidner and goals in first step                       
            pf = new PathfinderThread(this);
            int exitIndex=state.random.nextInt(exits.length);
            this.localGoal = exits[exitIndex];//random exit          
            Worker.loadOfExits[exitIndex]++; //increase load of exit
            pf.setGoalAndGeneratePath(PositionTools.getRoom(this, localGoal).getCenter());

        }

        if (globalGoal != null && !globalGoal.equals(localGoal)) {//if globalGoal fixed and it does not match the current goal
            pf.setGoalAndGeneratePath(PositionTools.getRoom(this, globalGoal).getCenter());
            localGoal = globalGoal;//this allow not entering this condition again if global goal is not rechanged
        }

        if (pf.isInGoal()) {
            this.stop();//stop agent and make it get out of the simulation       
            PositionTools.getOutOfSpace(this);
            LOG.info(name + " has leave the building using " + localGoal);
            return;
        }

        pf.step(state);//take steps to the goal (step can regenerate paths) 

    }

    public static void setGlobalGoal(String globalGoal) {
        Worker.globalGoal = globalGoal;

    }

    /**
     * Calcula los caminos a las salidas y los devuelve en JSON. Usa Pathfinder
     * y no PathfinderThread para bloquearse hasta que se termine. Por defecto,
     * se consideran personas que haya dentro de la percepción del usuario (ver
     * clase Pathfinder) y obstaculos fijos en la ruta. Pero no se han incluido
     * emergencias u otros valores. Se llama "obtain" y no get porque esta es
     * una clase implementando un agente, y el método get se interpreta como
     * inspector. Así que de llamarse getJSONPaths el resultado es que se
     * calcularía, al menos, cada vez que seleccionaras un agente para mostrarlo
     * en la consola.
     *
     * @return
     */
    public JSONObject obtainJSONPathsToExits(boolean extended) {
        JSONObject obj = new JSONObject();
        for (int i = 0; i < exits.length; i++) {
            try {
                JSONObject obj2 = new JSONObject();
                JSONArray ja= obtainJSONPath(exits[i]);
                obj2.put("distance", ja.length()); 
                Int2D posExit= PositionTools.getRoom(this, exits[i]).getCenter();
                obj2.put("position", "(" + posExit.x + "," + posExit.y + ")");
                Fire fire= ServletToSimulator.getInstance().getFire();
    
                if(fire!=null) obj2.put("distanceToEmergency", PositionTools.getDistance(posExit.x, posExit.y, fire.getInitialPos().x, fire.getInitialPos().y));
                obj2.put("loadOfExit", loadOfExits[i]);
                if(extended) obj2.put("steps", ja);
                obj.put(exits[i], obj2);
            } catch (JSONException ex) {
                Logger.getLogger(Worker.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
        return obj;

    }

    /**
     * Get step by step a path to a room, returned a JSONArray. Just for extended ouputs
     * @param p Person
     * @param room room destiny
     */
    private JSONArray obtainJSONPath(String room) {
        pf = new Pathfinder(this);
        JSONArray pathStepsList = new JSONArray();
        if (pf.setGoalAndGeneratePath(PositionTools.getRoom(this, room).getCenter())) {
            Path p = pf.getPath();
            int plength = p.getLength();
            for (int j = 0; j < plength; j++) {
                pathStepsList.put("(" + p.getStep(j).getX() + "," + p.getStep(j).getY() + ")");
            }
        }
        return pathStepsList;
    }

    /**
     * Get the goal stored by the agent
     */
    public JSONArray getGoalPath() {
        Path p = pf.getPath();
        JSONArray pathStepsList = new JSONArray();
        int plength = p.getLength();
        for (int j = 0; j < plength; j++) {
            pathStepsList.put("(" + p.getStep(j).getX() + "," + p.getStep(j).getY() + ")");
        }

        return pathStepsList;
    }
    
    /**
     * Goal of the person
     */
    public String getGoal(){
        return localGoal;
        
    }

}

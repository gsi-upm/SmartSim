package ubiksimdist;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.JsonElement;
import com.google.gson.JsonParser;
import com.hp.hpl.jena.sparql.lib.org.json.JSONArray;
import com.hp.hpl.jena.sparql.lib.org.json.JSONException;
import com.hp.hpl.jena.sparql.lib.org.json.JSONObject;
import java.awt.Color;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.SwingUtilities;
import static sim.app.ubik.UbikSimWithUI.getLocoIcon;
import sim.app.ubik.behaviors.PositionTools;
import sim.app.ubik.behaviors.pathfinderDemos.BooleanPathfindingMap;
import sim.app.ubik.building.rooms.Room;
import sim.app.ubik.emergency.Fire;
import sim.app.ubik.people.Person;
import sim.app.ubik.people.PersonHandler;
import sim.app.ubik.people.Worker;
import sim.display.Console;
import sim.util.MutableInt2D;
import ubiksimdist.*;


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

/**
 *
 * Comunicación servlet - simulador. Clase singleton para ser llamada en cada
 * petición asegurando "threadsafety"
 *
 * @author Emilio Serrano, Ph.d.; eserrano@gsi.dit.upm.es
 */
public class ServletToSimulator {

    private static sim.display.Console c;
    private static ETSITSimGUI sim;
    private static PersonHandler ph;
    private static final Logger LOG = Logger.getLogger(ServletToSimulator.class.getName());

    private static BooleanPathfindingMap pmap;
    private static Fire fire;
    
    private static ServletToSimulator INSTANCE = null;

    private ServletToSimulator() {
    }

    ;
   
    private synchronized static void createInstance() {
        if (INSTANCE == null) {
            INSTANCE = new ServletToSimulator();
            sim = ETSITSimGUI.initialize();
            c = ETSITSimGUI.getConsole();

        }
    }

    public static ServletToSimulator getInstance() {
        if (INSTANCE == null) {
            createInstance();
        }
        return INSTANCE;
    }

    /**
     * Este metodo lleva los comandos de control de la simulación, stop play y
     * pause. Funciona igual que en MASON. Se añade "frames"
     *
     * @param m: stop, play or pause
     */
    public String control(String m) {
        // mostrar mensaje del subproceso de ejecución despachador de eventos

        //comandos basico consola
        if (m.toLowerCase().equals("stop")) {
            c.pressStop();
            return ("Simulation stopped.");
        }
        if (m.toLowerCase().equals("play")) {
            c.pressPlay();
            return ("Simulation played, time step " + (sim.state.schedule.getSteps()-1));
        }
        if (m.toLowerCase().equals("pause")) {
            c.pressPause();
            ph = sim.getSim().getBuilding().getFloor(0).getPersonHandler();
            pmap = new BooleanPathfindingMap(ph.getPersons().get(0), 10);
            return ("Simulation paused, time step " + (sim.state.schedule.getSteps()-1));
        }
        if (m.toLowerCase().equals("frames")) {
            c.showAllFrames();
            return ("Simulation frames have been shown in the server side.");
        }
        return null;
    }

    /**
     * Este método lleva los comandos de posición de la simulación: people, map,
     * emergency, roomName
     */
    public String position(String m) {

        if (ph == null) {
            return "Set up the simulation with control=pause before getting positions.";
        }
        if (m.toLowerCase().equals("people")) {
            return (getPeoplePositions());
        }
        if (m.toLowerCase().equals("goals")) {
            return (getPeopleGoals());
        }
        
        if (m.toLowerCase().equals("map")) {
            return (getMap());
        }

        if (m.toLowerCase().equals("emergency")) {
            return (setFire());
        }
        if (m.toLowerCase().startsWith("(") && m.toLowerCase().endsWith(")")) {
            return (addOrMoveUser(m));
        }
        
        //goal de un usuario                
        Person p = ph.getPersonByName(m);        
        if(p!=null){
            String goal= ((Worker) p).getGoal();
            if(goal==null) return "Valid agent name, but the agent doesn't have a goal set.";
            else return ((Worker) p).getGoal();
        }
        
        
        return null;

    }

    private String getPeoplePositions() {

        List<Person> lp = ph.getPersons();
        JSONObject objPerson = new JSONObject();

        try {
            //cargar json
            for (Person p : lp) {

                JSONObject objAux = new JSONObject();
                objAux.put("positionX", p.getPosition().x);
                objAux.put("positionY", p.getPosition().y);
                Room r = PositionTools.getRoom(p);//puede devolver null si no está en habitación, i.e. entre dos habitaciones
                if (r != null) {
                    objAux.put("room", r.getName());
                } else {
                    objAux.put("room", "null");
                }
                objPerson.put(p.getName(), objAux);
            }

       
            return prettyJSON(objPerson.toString());
            //cargar json            

        } catch (JSONException ex) {
            Logger.getLogger(ServletToSimulator.class.getName()).log(Level.SEVERE, null, ex);
        }
        return "";

    }
    
    
     private String getPeopleGoals() {

        List<Person> lp = ph.getPersons();
        JSONObject objPerson = new JSONObject();

        try {
            //cargar json
            for (Person p : lp) {

                JSONObject objAux = new JSONObject();
                objAux.put("positionX", p.getPosition().x);
                objAux.put("positionY", p.getPosition().y);
                Room r = PositionTools.getRoom(p);//puede devolver null si no está en habitación, i.e. entre dos habitaciones
                if (r != null) {
                    objAux.put("room", r.getName());
                } else {
                    objAux.put("room", "null");
                }
                objAux.put("goal", ((Worker) p).getGoal() );
                objAux.put("goalPath", ((Worker) p).getGoalPath() );
                objPerson.put(p.getName(), objAux);                
            }

       
            return prettyJSON(objPerson.toString());
            //cargar json            

        } catch (JSONException ex) {
            Logger.getLogger(ServletToSimulator.class.getName()).log(Level.SEVERE, null, ex);
        }
        return "";

    }


    private String prettyJSON(String json) {
        Gson gson = new GsonBuilder().setPrettyPrinting().create();
        JsonParser jp = new JsonParser();
        JsonElement je = jp.parse(json.toString());
        String prettyJsonString = gson.toJson(je);
        return prettyJsonString;

    }

    private String getMap() {
        JSONObject objMap = new JSONObject();
        boolean map[][] = pmap.getBmap();
        try {
            objMap.put("width", pmap.getWidthInTiles());
            objMap.put("height", pmap.getHeightInTiles());
            JSONArray obstacleList = new JSONArray();
            for (int j = 0; j < map[0].length; j++) {
                for (int i = 0; i < map.length; i++) {
                    if (map[i][j]) {//obstacle there
                        obstacleList.put("(" + i + "," + j + ")");
                    }
                }
            }
            objMap.put("obstacles", obstacleList);
            return prettyJSON(objMap.toString());

        } catch (JSONException ex) {
            Logger.getLogger(ServletToSimulator.class.getName()).log(Level.SEVERE, null, ex);
        }
        return null;
    }

    /**
     * Recibe triple (nombre usuario, x, y) para crearlo si no existe y
     * posicionarlo si es así. Opcionalmente se puede pasar reduced/extended después de la y
     *
     * @param m
     * @return
     */
    private String addOrMoveUser(String m) {
        m = m.substring(1, m.length() - 1);
        List<String> triple = Arrays.asList(m.split(","));

        //load parameters
        String userName = triple.get(0);
        int x;
        int y;
        try {
            x = Integer.parseInt(triple.get(1));
            y = Integer.parseInt(triple.get(2));
        } catch (NumberFormatException nfe) {
            return "Non numeric coordinates given.";
        }

        //creating or getting person
        Worker pAdded;
        if (ph.getPersonByName(userName) == null) {
            ph.addPersons(1, true, ph.getPersons().get(0));
            pAdded = (Worker) ph.getPersons().get(ph.getPersons().size() - 1);
            pAdded.setStopped(true);//evitar movimiento automático como resto de trabajadores.
            pAdded.setName(userName);
        } else {
            pAdded = (Worker) ph.getPersonByName(userName);
        }

        //moving if no person or 
        if (pmap.isBlocked(pAdded, x, y)) {
            return "Coordinates are out of range, an obstacle, or other user is there.";
        }
        PositionTools.getOutOfSpace(pAdded);
        PositionTools.putInSpace(pAdded, x, y);
        pAdded.setColor(Color.blue); 
        
        if(triple.size()>=4 && triple.get(3).equals("reduced")){
            return "User added or moved.";
        }     
        
        JSONObject output;
        //paths, extended or not 
        if(triple.size()>=4 && triple.get(3).equals("extended")){
            output = pAdded.obtainJSONPathsToExits(true);
        }
        else{
            output = pAdded.obtainJSONPathsToExits(false);
        }
                
        return prettyJSON(output.toString());
        
    }

    private String setFire() {
        if(fire!=null) return "There is already a fire set in " + "(" + fire.getInitialPos().x + "," + fire.getInitialPos().y + ")";
        else{
            fire = new Fire(sim.getETSITSim());
            fire.insertInDisplay();
            return  "Emergency in " + "(" + fire.getInitialPos().x + "," + fire.getInitialPos().y + ")";
        }
     
    }
    
    public Fire getFire(){
        return fire;
    }

}

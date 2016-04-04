
package ubiksimdist;

import javax.servlet.ServletContext;
import sim.app.ubik.Ubik;
import sim.app.ubik.UbikSimWithUI;
import static sim.app.ubik.UbikSimWithUI.getLocoIcon;
import sim.app.ubik.behaviors.Automaton;
import sim.app.ubik.people.PersonHandler;
import sim.app.ubik.utils.Configuration;
import sim.app.ubik.utils.ElementsHandler;
import sim.display.Console;
import sim.display.Controller;



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
public class ETSITSimGUI extends UbikSimWithUI {

    private static ETSITSim ETSITSim;

 
    private static final String PATHBASE="C:\\Users\\Winston\\Documents\\UbikSimMOSI-AGIL-Server v2\\UbikSimMOSI-AGIL-Server\\build\\web\\WEB-INF\\";
    private static final String ENVIRONMENTPATH="C:\\Users\\Winston\\Documents\\UbikSimMOSI-AGIL-Server v2\\UbikSimMOSI-AGIL-Server\\build\\web\\WEB-INF\\environments\\EtsitEdificioBPlantaBaja.ubiksim";
    private static Console c;
    
    
   /**
 * @author Emilio Serrano, Ph.d.; Universidad Politécnica de Madrid.
 */ 
    public ETSITSimGUI(Ubik ubik) {
        super(ubik);
    }

    /**
     * Method called after pressing pause (the building variables are
     * instantiated) but before executing simulation. Any JFrame can be
     * registered to be shown in the display menu
     */
    @Override
    public void start() {
        super.start();
  
        Automaton.setEcho(false);
        //add more people
        
        
        PersonHandler ph = ETSITSim.getBuilding().getFloor(0).getPersonHandler();
        ph.addPersons(100, true, ph.getPersons().get(0));
        ph.changeNameOfAgents("a");


    }

    /**
     * Method to finish the simulation
     */
    @Override
    public void finish() {
        super.finish();
  
    }

    @Override
    public void init(final Controller c) {
        super.init(c);

    }

    /**
     * Executing simulation with GUI, it delegates to SimExample, simulation
     */
    public static ETSITSimGUI initialize() {
        //simExample = new SimExample(System.currentTimeMillis());
        Configuration.setPATHBASE(PATHBASE);   
        ElementsHandler.setPATHBASE(PATHBASE);
        
        
        ETSITSim = new ETSITSim();      
        
      
                
        ETSITSim.setPathScenario(ENVIRONMENTPATH);
        
        
        ETSITSimGUI vid = new ETSITSimGUI(ETSITSim);
        c = new Console(vid);
        c.setIncrementSeedOnStop(true);
        c.setVisible(true);
        c.setSize(500, 650);
        c.setIconImage(getLocoIcon().getImage());
        return vid;
    }

    public static ETSITSim getSim() {
        return ETSITSim;
    }

    public static Console getConsole() {
        return c;
    }
  
         public static ETSITSim getETSITSim() {
        return ETSITSim;
    }
    
        public static void main(String []args) {
           ETSITSimGUI.initialize();
        }

    
    

}

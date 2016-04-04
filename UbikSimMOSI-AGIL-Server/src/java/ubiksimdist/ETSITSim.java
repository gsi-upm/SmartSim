package ubiksimdist;


import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;
import sim.app.ubik.Ubik;
import sim.app.ubik.behaviors.Automaton;
import sim.app.ubik.people.PersonHandler;
import sim.app.ubik.people.Worker;
import sim.app.ubik.utils.Configuration;


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
public class ETSITSim extends Ubik {
    
 
    
    static int maxTimeForExecution=1500;
     
     private int globalTargetDemo1=1;//fix global exit for 

    public void setGlobalTargetDemo1(int val) {       
        this.globalTargetDemo1 = val;
        String[]  s= {"YARD1", "YARD2", "YARD3"};
        Worker.setGlobalGoal(s[val]);
    }

    public int getGlobalTargetDemo1() {
        return globalTargetDemo1;
    }
    
     public Object domGlobalTargetDemo1() { return new String[]  {"YARD1", "YARD2", "YARD3"}; }



     /**    
    * Object with information about execution and, if needed,
      * to finish the execution
      */     

  
    
    /**
     * Passing a random seed
     * @param seed 
     */
    public ETSITSim(long seed)   {
        super(seed);
        
    }
    
      /**
     * Passing a random seed and time to make EscapeMonitorAgent to finish simulation
     * This time must be less than maxTimeForExecution
     * @param seed 
     */
    public ETSITSim(long seed, int timeForSim)   {
        super(seed);

        
    }
    

    /**
     * Using seed from config.pros file
     */
     public ETSITSim() {         
         
           super();
       
           setSeed(getSeedFromFile());       
          
              
    }
     
     /**
      * 
     * Adding things before running simulation.   
     * Method called after pressing pause (the building variables are instantiated) but before executing simulation.
 
      */
   public void start() {               
        super.start();      


        
        

   }
    
   
   /**
 * Default execution without GUI. It executed the simulation for maxTimeForExecution steps.
 * @param args 
 */
    public static void main(String []args) {
       
       SimExample state = new SimExample(System.currentTimeMillis());
      
       state.start();
        do{
                if (!state.schedule.step(state)) break;
        }while(state.schedule.getSteps() < maxTimeForExecution);//
        state.finish();     
      
     
    }
    
 
      


}

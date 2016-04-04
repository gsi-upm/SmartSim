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
package ubiksimdist;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.apache.commons.lang3.StringEscapeUtils;



/**
 * @author Emilio Serrano, Ph.d.; Universidad Politécnica de Madrid.
 */
public class UbikSimServlet extends HttpServlet {

    // <editor-fold defaultstate="collapsed" desc="HttpServlet methods. Click on the + sign on the left to edit the code.">
    /**
     * Handles the HTTP <code>GET</code> method.
     *
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        //processRequest(request, response);

        //getting the simulator instance with singleton
        ServletToSimulator sts = ServletToSimulator.getInstance();

        //comprobar si se pide sólo json o web de salida 
        boolean showWeb = false;
        String output = request.getParameter("output");
        if (output != null) {
            showWeb = true;
        }

        //control orders:          
        String controlParameter = request.getParameter("control");
        if (controlParameter != null) {//hay un parámetro para control
            String controlOutput = sts.control(controlParameter); //se llama para su ejecución
            treatParameterOutput(response, controlOutput, "control", controlParameter, showWeb);
            return;
        }

        //position orders        
        String positionParameter = request.getParameter("position");
        if (positionParameter != null) {
            String positionResponse = sts.position(positionParameter);
            treatParameterOutput(response, positionResponse, "position", positionParameter, showWeb);
            return;
        }

        if (request.getParameterNames().hasMoreElements()) {
            response.sendError(HttpServletResponse.SC_BAD_REQUEST, "Parameter not recognized.");
        }

    }

    /**
     * Handles the HTTP <code>POST</code> method.
     *
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        doGet(request,response);//mismo comportamiento que get
        
    }

    /**
     * Returns a short description of the servlet.
     *
     * @return a String containing servlet description
     */
    @Override
    public String getServletInfo() {
        return "UbikSim Web Service";
    }// </editor-fold>

    /**
     * Processes requests for both HTTP <code>GET</code> and <code>POST</code>
     * methods.
     *
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    protected void processRequest(HttpServletRequest request, HttpServletResponse response)
            throws ServletException, IOException {
        /* 
         response.setContentType("text/html;charset=UTF-8");
         try (PrintWriter out = response.getWriter()) {
         
         out.println("<!DOCTYPE html>");
         out.println("<html>");
         out.println("<head>");
         out.println("<title>Servlet UbikSimServlet</title>");
         out.println("</head>");
         out.println("<body>");
         out.println("<h1>Servlet UbikSimServlet at " + request.getContextPath() + "</h1>");
         out.println("</body>");
         out.println("</html>");
         }*/
    }

    /**
     * Trata el valor dado para el simulador y genera una respuesta
     *
     * @param response, variable servlet para respuesta
     * @param output, salida dada por el simulador para imprimir (json, o web).
     * @param parameterName, nombre del parámetro: position o control
     * @param parameterValue, valor dado en la URL
     * @param showWeb, salida como WEB o como JSON
     */
    private void treatParameterOutput(HttpServletResponse response, String output, String parameterName, String parameterValue, boolean showWeb) {
        if (output == null) {
            try {
                response.sendError(HttpServletResponse.SC_NOT_IMPLEMENTED, "No " + parameterName + " action for: " + parameterValue);
            } catch (IOException ex) {
                Logger.getLogger(UbikSimServlet.class.getName()).log(Level.SEVERE, null, ex);
            }
        } else {
            if (!showWeb) {
                try {
                    PrintWriter out = response.getWriter();
                    response.setContentType("application/json");
                    out.println(output);
                    out.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            } else {
                printWeb(response, output);
            }

        }

    }

    /**
     * Genera una respuesta en formato web, en su llamada ya se ha comprobado
     * que hay respuesta y que la acción se ejecutó en el simulador.
     *
     * @param response
     * @param output
     */
    private void printWeb(HttpServletResponse response, String output) {
    response.setContentType("text/html");
        String web = "";
        try {
            
            PrintWriter out = response.getWriter();
            BufferedReader in = new BufferedReader(new FileReader(getServletContext().getRealPath("ubiksim.html")));
            while ((web = in.readLine()) != null) {                
               // web += web;
                  out.println(web);
                  if(web.endsWith("<!--SERVLET OUTPUT-->")){//add pantel with output
                    out.println("<div class=\"panel panel-primary\">");
                    out.println("<div class=\"panel-heading\">");
                    out.println("<h3 class=\"panel-title\">Action output:</h3>");
                    out.println("</div>");
                    out.println("<div class=\"panel-body\">");
                    out.println(StringEscapeUtils.unescapeHtml4(output));
                    out.println("</div>");
                    out.println("</div>");                                            
                  }                                                             
            }
            in.close();
            out.close();
        } catch (IOException ex) {
            Logger.getLogger(UbikSimServlet.class.getName()).log(Level.SEVERE, null, ex);
        }
       
      
    }

}

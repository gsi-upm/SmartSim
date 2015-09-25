
#include "vhcl.h"
#include "vhcl_socket.h"

#include "SBDebuggerServer.h"

#include <stdio.h>

#ifndef SB_NO_VHMSG
#include "vhmsg-tt.h"
#endif

#include <sb/SBScene.h>
#include <sb/SBCharacter.h>
#include <sb/SBSkeleton.h>
#include <sb/SBAnimationStateManager.h>
#include <sb/SBAnimationState.h>
#include <sb/SBAnimationTransition.h>

using std::string;
using std::vector;


SBDebuggerServer::SBDebuggerServer()
{
#ifndef SB_NO_VHMSG
   m_sbmFriendlyName = "sbm";
   m_connectResult = false;
   m_updateFrequencyS = 0;
   m_lastUpdate = m_timer.GetTime();
   m_scene = NULL;
   m_rendererIsRightHanded = true;
#endif
}


SBDebuggerServer::~SBDebuggerServer()
{
}


static const int NETWORK_PORT_TCP = 15104;

vhcl::socket_t m_sockTCP;
vector< vhcl::socket_t > m_sockConnectionsTCP;


void SBDebuggerServer::Init()
{
#ifndef SB_NO_VHMSG

#if __FLASHPLAYER__
	return;
#endif

   bool ret = vhcl::SocketStartup();
   if (!ret)
   {
      LOG("SocketStartup() failed for Debugger Server\n");
   }


   m_hostname = vhcl::SocketGetHostname();


   m_sockTCP = vhcl::SocketOpenTcp();
#ifdef WIN32
   if (m_sockTCP == NULL)
#else   
   if (m_sockTCP == -1)
#endif
   {
      LOG( "SocketOpenTcp() failed for Debugger Server. \n" );
      vhcl::SocketShutdown();
      return;
   }


   //ret = SocketSetReuseAddress(m_sockTCP, true);


   int portToTry = NETWORK_PORT_TCP;
   int portMax = NETWORK_PORT_TCP + 100;

   while (portToTry < portMax)
   {
      ret = vhcl::SocketBind(m_sockTCP, portToTry);
      if (ret)
      {
         break;
      }

      LOG( "SocketBind() failed for Debugger Server. Trying next port up.\n" );
      portToTry++;
   }

   if (portToTry >= portMax)
   {
      printf( "SocketBind() failed for Debugger Server\n" );
      vhcl::SocketClose(m_sockTCP);
      m_sockTCP = 0;
      vhcl::SocketShutdown();
      return;
   }


   m_port = portToTry;

   m_fullId = vhcl::Format("%s:%d:%s", m_hostname.c_str(), m_port, m_sbmFriendlyName.c_str());


   vhcl::SocketSetBlocking(m_sockTCP, false);

   vhcl::SocketListen(m_sockTCP);

#endif
   //return true;
}

void SBDebuggerServer::Close()
{
#ifndef SB_NO_VHMSG

#if __FLASHPLAYER__
	return;
#endif

   if ( m_sockTCP )
   {
      vhcl::SocketClose(m_sockTCP);
      m_sockTCP = 0;
   }

   vhcl::SocketShutdown();
#endif
}

void SBDebuggerServer::SetID(const std::string & id)
{
#ifndef SB_NO_VHMSG
   m_sbmFriendlyName = id;
   m_fullId = vhcl::Format("%s:%d:%s", m_hostname.c_str(), m_port, m_sbmFriendlyName.c_str());
#endif
}

const std::string& SBDebuggerServer::GetID()
{
	return m_fullId;
}

void SBDebuggerServer::Update()
{
#ifndef SB_NO_VHMSG

#if __FLASHPLAYER__
	return;
#endif

   if (m_updateFrequencyS > 0)
   {
      double currentTime = m_timer.GetTime();
      if (currentTime > m_lastUpdate + m_updateFrequencyS)
      {
         m_lastUpdate = currentTime;

         // I have to send pawn, camera, and character updates all on one SocketSend
         // call otherwise there is a massive delay on the receiving end
         bool sentCamUpdate = false;
         bool sentPawnUpdates = false;
         if (m_scene)
         {
            const std::vector<std::string>& charNames = m_scene->getCharacterNames();
			string msg = "";
            for (size_t i = 0; i < charNames.size(); i++)
            {
				SmartBody::SBCharacter * c = m_scene->getCharacter(charNames[i]);

               size_t numBones = c->getSkeleton()->getNumJoints();

               msg += vhcl::Format("sbmdebugger %s update", m_fullId.c_str());
               msg += vhcl::Format(" character %s bones %d\n", c->getName().c_str(), numBones);

               for (int j = 0; j < c->getSkeleton()->getNumJoints(); j++)
               {
                  SmartBody::SBJoint * joint = c->getSkeleton()->getJoint(j);

                  // beware of temporaries
                  float posx = joint->getPosition().x;
                  float posy = joint->getPosition().y;
                  float posz = joint->getPosition().z;
                  float rotx = joint->getQuaternion().x;
                  float roty = joint->getQuaternion().y;
                  float rotz = joint->getQuaternion().z;
                  float rotw = joint->getQuaternion().w;				  

                  msg += vhcl::Format("  %s pos %.3f %.3f %.3f rot %.3f %.3f %.3f %.3f\n", joint->getName().c_str(), posx, posy, posz, rotx, roty, rotz, rotw);
               }

               msg += ";";
			}

               if (!sentCamUpdate)
               {
                  // camera update
                  msg += vhcl::Format("sbmdebugger %s update camera\n", m_fullId.c_str());

				  msg += vhcl::Format("pos %.3f %.3f %.3f\n", m_cameraPos.x, m_cameraPos.y, m_cameraPos.z);
                  msg += vhcl::Format("rot %.3f %.3f %.3f %.3f\n", m_cameraRot.x, m_cameraRot.y, m_cameraRot.z, m_cameraRot.w);
				  msg += vhcl::Format("persp %.3f %.3f %.3f %.3f\n", m_cameraFovY, m_cameraAspect, m_cameraZNear, m_cameraZFar);
				  msg += vhcl::Format("lookat %.3f %.3f %.3f\n", m_cameraLookAt.x, m_cameraLookAt.y, m_cameraLookAt.z);
                  msg += ";";
                  sentCamUpdate = true;
               }

               if (!sentPawnUpdates)
               {
                  // sbmdebugger <sbmid> update pawn <name> pos <x y z> rot <x y z w> geom <s> size <s> 
                  const std::vector<std::string>& pawnNames = m_scene->getPawnNames();
                  for (size_t i = 0; i < pawnNames.size(); i++)
                  {
                     SmartBody::SBPawn* p = m_scene->getPawn(pawnNames[i]);
                     msg += vhcl::Format("sbmdebugger %s update pawn %s", m_fullId.c_str(), p->getName().c_str());
                     SrVec pos = p->getPosition();
                     SrQuat rot = p->getOrientation();
                     
                     msg += vhcl::Format(" pos %.3f %.3f %.3f rot %.3f %.3f %.3f %.3f", 
                        pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, rot.w); 

                     SBGeomObject* geom = p->getGeomObject();
                     if (geom)
                        msg += vhcl::Format(" geom %s size %.3f", geom->geomType().c_str(), geom->getGeomSize().x);    
                     else 
                        msg += vhcl::Format(" geom %s size %.3f", "sphere", 10.0f);
                                        
                     msg += ";";
                  }
                  sentPawnUpdates = true;
               }

               for ( size_t i = 0; i < m_sockConnectionsTCP.size(); i++ )
               {
                  //static int c = 0;
                  //LOG("TCP Send %d - %d\n", c++, i);
                  vhcl::SocketSend(m_sockConnectionsTCP[ i ], msg);
               }
            //}
         }
      }
   }





   if (vhcl::SocketIsDataPending(m_sockTCP))
   {
      vhcl::socket_t socket = vhcl::SocketAccept(m_sockTCP);
      if (socket)
      {
         vhcl::SocketSetBlocking(socket, false);
         m_sockConnectionsTCP.push_back(socket);
      }
   }


   for ( size_t i = 0; i < m_sockConnectionsTCP.size(); i++ )
   {
      vhcl::socket_t s = m_sockConnectionsTCP[i];

      bool tcpDataPending;
      tcpDataPending = vhcl::SocketIsDataPending(s);

      std::string overflowData = "";
      while ( tcpDataPending )
      {
         tcpDataPending = 0;

         char str[ 1000 ];
         memset( str, 0, sizeof( char ) * 1000 );

         int bytesReceived = vhcl::SocketReceive(s, str, sizeof( str ) - 1);
         if ( bytesReceived > 0 )
         {
            string recvStr = overflowData + str;

            vector< string > tokens;
            vhcl::Tokenize( recvStr, tokens, ";" );

            for ( int t = 0; t < (int)tokens.size(); t++ )
            {
               vector< string > msgTokens;
               vhcl::Tokenize( tokens[ t ], msgTokens, "|" );

               if ( msgTokens.size() > 0 )
               {
                  if ( msgTokens[ 0 ] == "TestMessage" )
                  {
                     if ( msgTokens.size() > 4 )
                     {
                        string s = msgTokens[ 1 ];
                     }
                  }
               }
            }
         }
         else if (bytesReceived < 0)
         {
            m_sockConnectionsTCP.erase(m_sockConnectionsTCP.begin() + i);
            tcpDataPending = false;
            continue;
         }

         tcpDataPending = vhcl::SocketIsDataPending(s);
      }
   }
#endif
}


void SBDebuggerServer::GenerateInitHierarchyMsg(SmartBody::SBJoint * root, string & msg, int tab)
{
#ifndef SB_NO_VHMSG

   string name = root->getName();
   float posx = root->offset().x;
   float posy = root->offset().y;
   float posz = root->offset().z;
   const SrQuat & q = root->quat()->prerot();   

   msg += string().assign(tab, ' ');
   msg += vhcl::Format("{ %s pos %f %f %f prerot %f %f %f %f\n", name.c_str(), posx, posy, posz, q.x, q.y, q.z, q.w);

   for (int i = 0; i < root->getNumChildren(); i++)
   {
      GenerateInitHierarchyMsg(root->getChild(i), msg, tab + 2);
   }

   msg += string().assign(tab, ' ');
   msg += vhcl::Format("}\n");
#endif
}


void SBDebuggerServer::ProcessVHMsgs(const char * op, const char * args)
{
#ifndef SB_NO_VHMSG

   string message = string(op) + " " + string(args);
   vector<string> split;
   vhcl::Tokenize(message, split, " \t\n");

   if (split.size() > 0)
   {
      if (split[0] == "sbmdebugger")
      {
         //LOG("SBDebuggerServer::ProcessVHMsgs() - %s", message.c_str());

         if (split.size() > 1)
         {
            if (split[1] == m_fullId)
            {
               if (split.size() > 2)
               {
                  if (split[2] == "connect")
                  {
                     m_connectResult = true;

                     vhmsg::ttu_notify1(vhcl::Format("sbmdebugger %s connect_success", m_fullId.c_str()).c_str());
                  }
                  else if (split[2] == "disconnect")
                  {
                     m_sockConnectionsTCP.clear();

                     m_updateFrequencyS = 0;
                     m_connectResult = false;
                  }
                  else if (split[2] == "send_init")
                  {
                     if (m_scene != NULL)
                     {
						 std::string message = vhcl::Format("sbmdebugger %s init scene\n", m_fullId.c_str());	
						 //LOG("before scene save");
						 std::string initScript = m_scene->save(true); // save for remote connection	 
						 //LOG("initScript = %s",initScript.c_str());
						 message += initScript;
						 //LOG("initScript size = %d",initScript.size());
						 //FILE* fp = fopen("e:/sceneServer.py","wt");
						 //fprintf(fp,"%s",initScript.c_str());
						 //fclose(fp);
						 vhmsg::ttu_notify1(message.c_str());

#if 0
						 std::vector<string> skeletonNames = m_scene->getSkeletonNames();
						for (size_t i = 0; i < skeletonNames.size(); ++i)
						{
							SmartBody::SBSkeleton* skeleton = m_scene->getSkeleton(skeletonNames[i]);
							if (!skeleton)
							{
								LOG("Cannot find skeleton %s.", skeletonNames[i].c_str());
							}
							else
							{
								std::string msg = vhcl::Format("sbmdebugger %s init skeleton %s ", m_fullId.c_str(), skeleton->skfilename().c_str());
								msg += skeleton->saveToString();
								vhmsg::ttu_notify1(msg.c_str());	

							}
						}						

                        const std::vector<string>& charNames = m_scene->getCharacterNames();
                        for (size_t i = 0; i < charNames.size(); i++)
                        {
                           SmartBody::SBCharacter * c = m_scene->getCharacter(charNames[i]);

                           size_t numBones = c->getSkeleton()->getNumJoints();

						   // SbmMonitor character format
                           string msg = vhcl::Format("sbmdebugger %s init", m_fullId.c_str());
                           msg += vhcl::Format(" character %s bones %d\n", c->getName().c_str(), numBones);
						   SmartBody::SBJoint * root = c->getSkeleton()->getJoint(0);
                           GenerateInitHierarchyMsg(root, msg, 4);
						   vhmsg::ttu_notify1(msg.c_str());

						  // this is used for smartbody-monitor (use character-skeleton to not conflict with sbm-monitor)
						   msg = vhcl::Format("sbmdebugger %s init", m_fullId.c_str());
						   msg += vhcl::Format(" character-skeleton %s %s\n", c->getName().c_str(), c->getSkeleton()->getName().c_str());
						   vhmsg::ttu_notify1(msg.c_str());

						   // render direction information
                           msg = vhcl::Format("sbmdebugger %s init", m_fullId.c_str());
						   msg += vhcl::Format(" renderer right_handed %d\n", m_rendererIsRightHanded ? 1 : 0);
                           vhmsg::ttu_notify1(msg.c_str());
                        }

                        const std::vector<std::string>& pawnNames = m_scene->getPawnNames();
                        for (size_t i = 0; i < pawnNames.size(); i++)
                        {
                           SmartBody::SBPawn* p = m_scene->getPawn(pawnNames[i]);
                           string msg = vhcl::Format("sbmdebugger %s init pawn %s", m_fullId.c_str(), p->getName().c_str());
                           SrVec pos = p->getPosition();
                           SrQuat rot = p->getOrientation();
                           msg += vhcl::Format(" pos %.3f %.3f %.3f rot %.3f %.3f %.3f %.3f geom %s size %.3f", 
                              pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, rot.w, "sphere", 10.0f);

                           vhmsg::ttu_notify1(msg.c_str());
                        }

						const std::vector<std::string>& faceDefNames = m_scene->getFaceDefinitionNames();
						for (size_t i = 0; i < faceDefNames.size(); i++)
						{
							SmartBody::SBFaceDefinition* faceDefinition = m_scene->getFaceDefinition(faceDefNames[i]);
							std::string msg = vhcl::Format("sbmdebugger %s init face_definition %s", m_fullId.c_str(), faceDefNames[i].c_str());
							msg += faceDefinition->saveToString();

							vhmsg::ttu_notify1(msg.c_str());
						}

						for (size_t i = 0; i < charNames.size(); i++)
						{
							SmartBody::SBCharacter * c = m_scene->getCharacter(charNames[i]);
							string msg = vhcl::Format("sbmdebugger %s init", m_fullId.c_str());
							SmartBody::SBFaceDefinition* faceDef = c->getFaceDefinition();
							if (faceDef)
							{
								msg += vhcl::Format(" character-face_definition %s %s\n", c->getName().c_str(), faceDef->getName().c_str());
								vhmsg::ttu_notify1(msg.c_str());
							}
						}
						
						SmartBody::SBAnimationBlendManager* blendManager = m_scene->getBlendManager();
						const std::vector<std::string>& blendNames = blendManager->getBlendNames();
						for (size_t i = 0; i < blendNames.size(); i++)
						{
							SmartBody::SBAnimationBlend* blend = blendManager->getBlend(blendNames[i]);
							std::string msg = vhcl::Format("sbmdebugger %s init blend %s", m_fullId.c_str(), blendNames[i].c_str());
							msg += blend->saveToString();

							vhmsg::ttu_notify1(msg.c_str());
						}

						for (int i = 0; i < blendManager->getNumTransitions(); ++i)
						{
							SmartBody::SBAnimationTransition* transition = blendManager->getTransitionByIndex(i);
							std::string msg = vhcl::Format("sbmdebugger %s init transition ", m_fullId.c_str());
							msg += transition->saveToString();

							vhmsg::ttu_notify1(msg.c_str());
						}
#endif
                     }
                  }
                  else if (split[2] == "start_update")
                  {
                     if (split.size() > 3)
                     {
                        m_updateFrequencyS = vhcl::ToDouble(split[3]);
                     }
                  }
                  else if (split[2] == "end_update")
                  {
                     m_updateFrequencyS = 0;
                  }
               }
            }
            else if (split[1] == "queryids")
            {
               LOG("SBDebuggerServer::ProcessVHMsgs() - queryids");
               vhmsg::ttu_notify1(vhcl::Format("sbmdebugger %s id", m_fullId.c_str()).c_str());
            }
         }
      }
   }
#endif
}

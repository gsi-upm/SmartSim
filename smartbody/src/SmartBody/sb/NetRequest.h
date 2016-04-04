#ifndef NETREQUEST_H_
#define NETREQUEST_H_

#include <sb/SBTypes.h>
#include <stdio.h>

using std::vector;
using std::string;

class NetRequest
{
public:
   typedef bool (*RequestCallback)(void* caller, NetRequest* req);
   enum RequestId
   {
      Get_Viseme_Names,
      Get_AU_Side,
      Get_Seq_Asset_Paths,
      Get_ME_Asset_Paths,
      Get_Audio_Asset_Paths,
      Get_Mesh_Asset_Paths,
      Get_Script_Names,
      Get_Service_Names,
      Get_Skeleton_Names,
      Get_BoneMap_Names,
      Get_EventHandler_Names,
      Get_Motion_Names,
      Get_Channel_Names,
      Get_Controller_Names,
      Get_Character_Height,
      Get_Scene_Scale,
	  Send_Python_Command,
	  Get_Face_Definition,
   };

   NetRequest(RequestId _rid, RequestCallback _cb, void* _callbackOwner = NULL) { rid = _rid; cb = _cb; callbackOwner = _callbackOwner; }
   virtual ~NetRequest() {}

   SBAPI bool DataAvailable()
   {
      return false;
   }

   SBAPI bool ProcessRequest(vector<string>& _args)
   {
      if (!cb)
      {
         //printf("Can't Process NetRequest \'%d\' because it doesn't have a callback", rid);
         return false;
      }

      args = _args;
      return cb(callbackOwner, this);
   }

   template<typename DataType>
   DataType getCaller()
   {
      return static_cast<DataType>(callbackOwner);
   }

   SBAPI RequestId Rid() { return rid; }
   SBAPI vector<string> Args() { return args; }

   SBAPI string ArgsAsString()
   {
      string ret = "";

      for (unsigned int i = 0; i < args.size(); i++)
      {
         ret += args[i];
         ret.insert(ret.length(), " ");
      }

      // get rid of ending space
      ret.erase(ret.end() - 1);

      return ret;
   }

private: 
   RequestId rid;
   void* callbackOwner;
   RequestCallback cb;
   vector<string> args;
};



class NetRequestManager
{
public:
   NetRequestManager() {}
   virtual ~NetRequestManager() 
   {
      for (unsigned int i = 0; i < m_requests.size(); i++)
      {
         delete m_requests[i];
         m_requests.erase(m_requests.begin());
         i--;
      }
   }

   void CreateNetRequest(NetRequest::RequestId rid, NetRequest::RequestCallback cb, void* callbackOwner)
   {
      AddNetRequest(new NetRequest(rid, cb, callbackOwner));
   }

   void CreateNetRequest(int rid, NetRequest::RequestCallback cb, void* callbackOwner)
   {
      CreateNetRequest((NetRequest::RequestId)rid, cb, callbackOwner);
   }

   void ProcessRequest(NetRequest::RequestId rid, vector<string>& args)
   {
      for (unsigned int i = 0; i < m_requests.size(); i++)
      {
         if (m_requests[i]->Rid() == rid)
         {
            ProcessRequest(m_requests[i], args);
            m_requests.erase(m_requests.begin() + i);
            return;
         }
      }

      printf("Can't Process NetRequest \'%d\' because it doesn't exist", rid);
   }

   //void ProcessRequests()
   //{
   //   for (unsigned int i = 0; i < m_requests.size(); i++)
   //   {
   //      if (m_requests[i]->DataAvailable())
   //      {
   //         m_requests[i]->ProcessRequest();
   //         delete m_requests[i];
   //         m_requests.erase(m_requests.begin() + i);
   //         i--;
   //      }
   //   }
   //}

   void RemoveRequest(NetRequest::RequestId rid)
   {
      for (unsigned int i = 0; i < m_requests.size(); i++)
      {
         if (m_requests[i]->Rid() == rid)
         {
            delete m_requests[i];
            m_requests.erase(m_requests.begin() + i);
            return;
         }
      }
   }

private:
   vector<NetRequest*> m_requests;

   void ProcessRequest(NetRequest* req, vector<string>& args)
   {
      req->ProcessRequest(args);
      delete req;
   }

   void AddNetRequest(NetRequest* req)
   {
      m_requests.push_back(req);
   }
};


#endif

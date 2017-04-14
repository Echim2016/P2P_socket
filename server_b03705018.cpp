#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <pthread.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>

#define MAX 500

using namespace std;
/* b03705018 Hsu Yi-Chin */


/* Some function about SSL */
void InitializeSSL() { 
  SSL_load_error_strings();
  SSL_library_init();
  OpenSSL_add_all_algorithms();
};
void DestroySSL() {
  ERR_free_strings();
  EVP_cleanup();

};
void ShutdownSSL(SSL * ssl) {
  SSL_shutdown(ssl);
  SSL_free(ssl);
};

/* Data structure of user information */
struct user {
  int user_index;
  string user_name;
  char* user_ip;
  string user_port;
  int user_account;
  struct user * next_user;
};


/* Data structure of online user */
class onlineList {

private:
  struct user* head;
  int onlineListLength;

public:

  /* Constructor */
  onlineList(){
    head = new user;
    head->user_index = -1;
    head->user_name = "";
    onlineListLength = 0;
  }

  /* Insert a new user to online list */
  void insertOnlineUser (int index, string name, char* ip, string port, int account){

    struct user* newUser = new user;
    newUser->user_index = index;
    newUser->user_name = name;
    newUser->user_ip = ip;
    newUser->user_port = port;
    newUser->next_user = head;
    newUser->user_account = account;
    head=newUser;
    onlineListLength++;

  }

  void updateUserAccount(string name, int newAccount){

    struct user * temp;
    int updateAccount;
    //struct user * lastUser;
    if (head->user_name==name)
    {
      updateAccount = head->user_account + newAccount;
      head->user_account = updateAccount;

    }

    else{
      temp = head->next_user;
      //lastUser = head;
      for (int j = 0; j < onlineListLength-1; j++)
      {
        if (name==temp->user_name)
        {
          //lastUser->next_user = temp->next_user;
          updateAccount = temp->user_account + newAccount;
          temp->user_account = updateAccount;
          break;
        }

        else{
          //lastUser = temp;
          temp = temp->next_user;
        }
      }
    }

  }

  /* Delete a given user from data structure */
  void deleteOnlineUser (int index){

    struct user * temp;
    struct user * lastUser;
    if (head->user_index==index)
    {
      temp = head;
      head = head->next_user;
      delete head;
    }

    else{
      temp = head->next_user;
      lastUser = head;
      for (int j = 0; j < onlineListLength-1; j++)
      {
        if (index==temp->user_index)
        {
          lastUser->next_user = temp->next_user;
          delete temp;
          break;
        }

        else{
          lastUser = temp;
          temp = temp->next_user;
        }
      }
    }
    onlineListLength--;

  }

  /* Show the user's account balance, nubmer of online user and online list */
  char* displayList (string balance) {
    char userInfo[9999] ="";
    char userInfoReverse[9999]="";
    char* list;
    struct user* temp = head;
    char disNum[100]="";

    strcat(disNum, "Account Balance: ");
    strcat(disNum, balance.c_str());
    strcat(disNum, "\n");

    string numUser = to_string(onlineListLength);
    strcat(disNum, "Number of users: ");
    strcat(disNum, numUser.c_str());
    strcat(disNum, "\n");
    strcat(userInfo, disNum);


    for (int m = 0; m < onlineListLength; m++)
    {
      char userInfo1[9999] ="";
      strcat(userInfo1, (temp->user_name).c_str());
      strcat(userInfo1, "#");
      strcat(userInfo1, temp->user_ip);
      strcat(userInfo1, "#");
      strcat(userInfo1, (temp->user_port).c_str());
      strcat(userInfo1, "\n");

      strcat(userInfo1, userInfoReverse);
      strcpy(userInfoReverse, userInfo1);

      temp=temp->next_user;
    }
    strcat(userInfo, userInfoReverse);
    list=userInfo;
    return list;

  }

  /* Return the latest number of online users*/
  int getOnlineUserNum(){
    return onlineListLength;
  }

  /* Return the given user's port number */
  string getPort(string name){

    struct user * temp;
    //struct user * lastUser;
    if (head->user_name==name)
    {
        return head->user_port;
    }

    else{
      temp = head->next_user;
      //lastUser = head;
      for (int j = 0; j < onlineListLength-1; j++)
      {
        if (name==temp->user_name)
        {
          //lastUser->next_user = temp->next_user;
          return temp->user_port;
          break;
        }

        else{
          //lastUser = temp;
          temp = temp->next_user;
        }
      }
    }
    return "NULL";
  }

  /* Return the given user's IP */
  char* getIP (string name){

    struct user * temp;
    //struct user * lastUser;
    if (head->user_name==name)
    {
        return head->user_ip;
    }

    else{
      temp = head->next_user;
      //lastUser = head;
      for (int j = 0; j < onlineListLength-1; j++)
      {
        if (name==temp->user_name)
        {
          //lastUser->next_user = temp->next_user;
          return temp->user_ip;
          break;
        }

        else{
          //lastUser = temp;
          temp = temp->next_user;
        }
      }
    }
    return 0;
  }
};

/* Data structure to store the registered user */
class userList{

private:
  string userNames[MAX];
  int userAccount [MAX];
  int listLength;

public:

  /* Constructor */
  userList(){
    listLength=0;
  }

  /* Store a new user name and new account balance */
  void insertUser(string name, int account){
    userNames[listLength]=name;
    userAccount[listLength]=account;
    listLength++;
  }

  /* Check whether a given user name is registered in the user list */
  bool searchUser(string searchName){
    bool found = false;
    for (int i = 0; i < listLength; i++)
    {
      if (userNames[i]==searchName)
      {
        found = true;
        break;
      }
    }
    return found;
  }

  void updateUserAccount0(string tName, int newAccount){

    int t = 0;
    for (int i = 0; i < listLength; i++)
    {
      if (userNames[i] == tName)
      {
        t = i;
        break;
      }
    }
    userAccount[t]=newAccount+userAccount[t];
  }


  /* Return the number of registered users */
  int getUserNum(){
    return listLength;
  }

  /* Return a given user's account */
  int getUserAccount(string tName){

    int t = 0;
    for (int i = 0; i < listLength; i++)
    {
      if (userNames[i] == tName)
      {
        t = i;
        break;
      }
    }
    return userAccount[t];
  }


};

/* Build online user list and user list */
onlineList OL;
userList UL;
char buffer[1024];
struct sockaddr_in caddr[1000];

void * services(void *arg){

  int clientID = *((int *) arg);
  struct sockaddr_in clientAddr = caddr[clientID];
  string logName;
  string logPort;
  bool login=0;
  bool check=0;


  /* Build SSL */
  SSL_CTX *sslctx;
  SSL *cSSL;

  InitializeSSL();

  sslctx = SSL_CTX_new( TLSv1_server_method());
  SSL_CTX_set_options(sslctx, SSL_OP_SINGLE_DH_USE);
  int use_cert = SSL_CTX_use_certificate_file(sslctx, "mycert.pem" , SSL_FILETYPE_PEM);
  if (use_cert<=0)
  {
    ERR_print_errors_fp(stderr);
    abort();
  }
  int use_prv = SSL_CTX_use_PrivateKey_file(sslctx, "mykey.pem", SSL_FILETYPE_PEM);
  if (use_prv<=0)
  {
    ERR_print_errors_fp(stderr);
    abort();
  }
  if (!SSL_CTX_check_private_key(sslctx))
  {
    fprintf(stderr,"Private key does not match the public certificate.\n");
    abort();
  }

  cSSL = SSL_new(sslctx);
  SSL_set_fd(cSSL, clientID);
  int ssl_err = SSL_accept(cSSL);
  if(ssl_err <= 0)
  {
      ShutdownSSL(cSSL);
  }

  SSL_write(cSSL, "Success Connection!", strlen("Success Connection!"));  


    while(login==0 && check==0) {

        login=1;
        memset(buffer, 0, 1024);
        SSL_read(cSSL, buffer,1024);
        string message(buffer);
        
        /* Register */
        if (strstr(buffer,"REGISTER#"))
        {
          string inst_1_OK = "100 OK\n";
          string inst_1_NO = "210 FAIL\n";
          size_t accountPos = (message.substr(9)).find("#");
          string regName = message.substr(strlen("REGISTER#"),accountPos);
          int nameLength = regName.length();
          string regAccount = message.substr(strlen("REGISTER#")+nameLength+1);
          int accountBalance = atoi(regAccount.c_str());
          if (UL.searchUser(regName)==false && UL.getUserNum()<=MAX)
          {
            /* Insert the new user */
            UL.insertUser(regName, accountBalance);
            SSL_write(cSSL,inst_1_OK.c_str(), inst_1_OK.length());
            cout<<message<<endl;
            login=0;
          }
          else{
            /* Failed to register */
            SSL_write(cSSL,inst_1_NO.c_str(), inst_1_NO.length());
            login=0;

          }
        }

        /* Update the payer and payee's account balance */
        else if (strstr(buffer,"TRANSFER#"))
        {
          string inst_up = "\n-----Account updated-----\n";
          string inst_nup = "-----Fail to update your account-----\n";
          size_t accountPos = (message.substr(9)).find("#");

          string payeeName = message.substr(strlen("REGISTER#"),accountPos);
          int nameLength = payeeName.length();

          size_t accountPos2 = (message.substr(10+nameLength)).find("#");

          string tranAccount = message.substr(strlen("REGISTER#")+nameLength+1,accountPos2);
          int tranBalance = atoi(tranAccount.c_str());
          int accountLength = tranAccount.length();

          string payerName = message.substr(11+nameLength+accountLength);

          if (UL.searchUser(payeeName)==true)
          {
            /* Update user's account balance */
            UL.updateUserAccount0(payeeName, tranBalance);
            UL.updateUserAccount0(payerName, (-1*tranBalance) );
            OL.updateUserAccount(payeeName, tranBalance );
            OL.updateUserAccount(payerName, (-1*tranBalance));

            SSL_write(cSSL,inst_up.c_str(), inst_up.length());
            cout<<message<<endl;
            login=0;
          }
          else{
            /* Failed to register */
            SSL_write(cSSL,inst_nup.c_str(), inst_nup.length());
            login=0;

          }
        }

        /* Display current online list and account balance */
        else if (strstr(buffer,"LIST#"))
        {
          string lName = message.substr(5);
          char* displayList2;
          int iniAccount2 = UL.getUserAccount(lName);
          string tempBalance2 = to_string(iniAccount2);
          displayList2 = OL.displayList(tempBalance2);

          SSL_write(cSSL,displayList2,strlen(displayList2));
          cout<<"List from user<"<<logName<<">"<<endl;
          
          login=0;
        }

        /* Exit */
        else if (strstr(buffer,"EXIT"))
        {
          check=0;
          SSL_write(cSSL,"Bye~\n", strlen("Bye~\n"));
          OL.deleteOnlineUser(clientID);
          cout<<"Exit from user<"<<logName<<">"<<endl;
          close(clientID);
          SSL_CTX_free(sslctx);
          pthread_exit(0);
        }

        else if (strstr(buffer, "FIND#"))
        {
          string inst_notf = "230 User not found\n";
          string findName = message.substr(strlen("FIND#"));
          if (UL.searchUser(findName)==true )
          {
            /* Insert the new user */

            char* tIP = OL.getIP(findName);
            string tPort = OL.getPort(findName);

            char findReply[1024];
            strcat(findReply,tIP);
            strcat(findReply, "#");
            strcat(findReply,tPort.c_str());


            // SSL_write(cSSL,findReply, strlen(findReply));
            SSL_write(cSSL,findReply, strlen(findReply));

            cout<<message<<endl;
            login=0;
          }
          else{
            /* Failed to register */
            SSL_write(cSSL,inst_notf.c_str(), inst_notf.length());

            login=0;

          }
        }

        /* User want to login*/
        else if (message.find("#")!=string::npos)
        {
          /* Get the user's name and port */
          char logIP[20]; 
          char nullIP[20] ;
          strcpy(nullIP, "0.0.0.0");       
           
          size_t namePos = message.find("#");
          logName = message.substr(0,namePos);
          logPort = message.substr(namePos+1);
          string inst_2_NO = "220 AUTH_FAIL";

          /* Check for unexpected IP error */
          if (strcmp(inet_ntoa(clientAddr.sin_addr),nullIP))
          {
              strcpy(logIP, inet_ntoa(clientAddr.sin_addr));
          }
          else{
              strcpy(logIP, "127.0.0.1");
          }

          if (UL.searchUser(logName)==false)
          {
            /* Failed to login if user is not in the user list */
            SSL_write(cSSL,inst_2_NO.c_str(), inst_2_NO.length());
            login=0;

          }
          else{
            /* Login and update the online list */
            int iniAccount = UL.getUserAccount(logName);
            OL.insertOnlineUser(clientID, logName, logIP, logPort, iniAccount);
            
            /* Send the latest information to client */
            char* displayList;
            string tempBalance  = to_string(iniAccount);
            displayList = OL.displayList(tempBalance);
            SSL_write(cSSL,displayList,strlen(displayList));
            cout<<message<<endl;

            while(check==0){
              check=1;
              /* Get the user's next instruction */
              memset(buffer, 0, 1024);
              SSL_read(cSSL, buffer, 1024);
              string message2 (buffer);

              if (message2=="List")
              {
                /* Display the latest online list */
                iniAccount = UL.getUserAccount(logName);
                tempBalance = to_string(iniAccount);
                displayList = OL.displayList(tempBalance);

                SSL_write(cSSL,displayList,strlen(displayList));
                login=0;
                check=0;
                cout<<message2<<" from user<"<<logName<<">"<<endl;
              }

              if (message2=="Exit")
              {
                /* Exit and lose the thread */
                check=0;
                SSL_write(cSSL,"Bye~\n", strlen("Bye~\n"));
                OL.deleteOnlineUser(clientID);
                cout<<message2<<" from user<"<<logName<<">"<<endl;
                break;
              }

              if (message2=="Back")
              {
                /* Back to homepage */
                check=0;
                login=0;
                break;
              }
            }
          }
        }


        /* Prevent unpredictable crashed */
        if (check==1||login==1)
        {
          cout<<"User <"<<logName<<"> has dropped out\n";
          OL.deleteOnlineUser(clientID);
          close(clientID);
          pthread_exit(0);
        }
    }
    close(clientID);
    SSL_CTX_free(sslctx);
    pthread_exit(0);
}


int main(int argc, char* argv[]){
  int port_Num = atoi(argv[1]);
  pthread_t threads[MAX];

  /* Error detection when there is a syntax error */
  if (argc<2)
   {
     cout<<"Syntax error.\n";
     exit(0);
   } 
  struct sockaddr_in serverAddr, clientAddr;

  /* Create the socket */
  int socket_ser = socket(PF_INET, SOCK_STREAM, 0);

  /* Error detection when there is a setup error */
  if (socket_ser<0)
  {
    cout<<"Setup error.\n";
    exit(0);
  }

  /* Set the connection using the port number and IP given by user */
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port_Num);
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  memset(serverAddr.sin_zero, 0, sizeof (serverAddr));  

  /* Connecting */
  int bindCon=bind(socket_ser,(struct sockaddr *) &serverAddr, sizeof(serverAddr));
  /* Error detection when connection failed */
  if (bindCon<0)
  {
    cout<<"Binding error.\n";
    exit(0);
  }
  /* Start listening */
  listen(socket_ser,50);

  while(1){

    int cli_socket = accept (socket_ser, (struct sockaddr *) &clientAddr, (socklen_t *)&(clientAddr));
    if (cli_socket<0)
    {
      cout<<"Accepting error. \n";
      exit(0);
    }

    /* Record the client */
    caddr[cli_socket] = clientAddr;

    /* Create threads for client */
    pthread_create(&threads[cli_socket], 0, services, &cli_socket);
 
  }
  return 0;
}

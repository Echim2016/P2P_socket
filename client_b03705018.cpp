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
#include <cctype>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>


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

/* Check that whether the string is integer */
bool IsDigit (string str){

  bool id=true;
  for(unsigned int i=0; i<str.length(); i++)
  {
    while(!isdigit(str[i]))
    {
      id=false;
      break;
    }

 }
  return id;
}


/* Main function */
int main(int argc, char* argv[]){

  int port_Num = atoi(argv[2]);
  char* IP = argv[1];

  /* Error detection when there is a syntax error */
  if (argc<3)
   {
     cout<<"Syntax error.\n";
   } 

  /* Error detection when there is a host error */
  if (gethostbyname(argv[1])==NULL)
  {
    cout<<"Host error.\n";
  } 

  char buffer[1024];
  char c_msg[1024];

  struct sockaddr_in serverAddr;

  /* Create the socket */
  int socket_cli = socket(PF_INET, SOCK_STREAM, 0);

  /* Error detection when there is a setup error */
  if (socket_cli<0)
  {
    cout<<"Setup error.\n";
    exit(0);
  }

  /* Set the connection using the port number and IP given by user */
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port_Num);
  //serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_addr.s_addr = inet_addr(IP);
  memset(serverAddr.sin_zero, 0, sizeof (serverAddr));  

  /* Connecting */
  int Con=connect(socket_cli, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  /* Error detection when connection failed */
  if (Con<0)
  {
    cout<<"Connect error.\n";
    exit(0);
  }

  /* Build SSL */
  SSL_CTX *sslctx;
  SSL * cSSL;
  InitializeSSL();

  sslctx = SSL_CTX_new( TLSv1_client_method());
  SSL_CTX_set_options(sslctx, SSL_OP_NO_SSLv2);

  cSSL = SSL_new(sslctx);
  SSL_set_fd(cSSL, socket_cli);
  int ssl_err = SSL_connect(cSSL);
  if (ssl_err<1)
  {
    ShutdownSSL(cSSL);
  }

  /* Read welcome message from server */
  memset(buffer, 0, 1024);
  SSL_read(cSSL, buffer,1024);
  cout<< buffer <<endl;

  /* Decalre some variables */
  string entry;
  string userName;
  string userAccount;
  string loginName;
  string loginPort;

  string payerName;
  string payeeName;
  string tranferAmount;
  string payeeIP;
  int payeePort;

  string t_msg;

  bool login = 0;
  bool transferLogin=0;


  /* Some functions when user connects to the server */
    while(login == 0 && Con>=0) {

        cout << "Enter your instructions!\n --------------------------------------\n";
        cout<<"1 for Register\n2 for Login\n3 for Waiting for account transferring\n4 for Tranfer money\n5 for List\n";
        cout<<"E for Exit\n ";
        cout<<"--------------------------------------\n";
        cout<<"Your instruction: ";
        cin >> entry;

        /* Register */
        if (entry=="1")
        {
          cout<<"Enter the name you want to register: ";
          cin>> userName;
          cout<<"Enter the inital account balance: ";
          cin>> userAccount;

          while (!IsDigit(userAccount)){
          cout<<"Enter the inital account balance: ";
          cin>> userAccount;
          }

          string inst_1 = "REGISTER#" + userName + "#" + userAccount;
          SSL_write(cSSL,inst_1.c_str(), inst_1.length());
          memset (buffer,0,1024);
          SSL_read(cSSL, buffer,1024);
          cout<< buffer <<endl;
        }
        /* Login */

        if (entry=="2")
        {
          cout<<"Enter your name: ";
          cin>> loginName;
          cout<<"Enter the port number (which is > 1023 ): ";
          cin>>loginPort;

          while(!IsDigit(loginPort)||atoi(loginPort.c_str())<=1023){
            cout<<"Enter the port number (which is > 1023 ): ";
            cin>>loginPort;
          }

          cout<<"--------------------------------------\n";
          string inst_2 = loginName + "#" + loginPort ;
          SSL_write(cSSL,inst_2.c_str(), inst_2.length());
          memset (buffer,0,1024);
          SSL_read(cSSL, buffer,1024);
          cout<< buffer <<endl;

          transferLogin=1;
          bool out=0;
          if (buffer[0]=='2')
          {
            out=1;
          }

            while (out==0)
            {
                cout<<"Enter the number of actions you want to take.\n";
                cout<<"1 to ask for the latest list, 7 for back to homepage, 8 to Exit: ";
                
                cin>>entry;

                /* Show the online list */
                if (entry == "1")
                {
                  string inst_l = "List";
                  SSL_write(cSSL,inst_l.c_str(), inst_l.length());
                  memset (buffer,0,1024);
                  SSL_read(cSSL, buffer,1024); 
                  cout<< buffer <<endl;
                }
                /* Exit */
                if (entry == "8")
                { 
 
                  string inst_8 = "Exit";
                  SSL_write(cSSL,inst_8.c_str(), inst_8.length());
                  memset (buffer,0,1024);
                  SSL_read(cSSL, buffer,1024); 
                  string rsp(buffer);
                  cout<< rsp <<endl;
                  out=1;
                  login=1;
                }

                /* Back to homepage */
                if (entry== "7")
                {
                  string inst_7 ="Back";
                  SSL_write(cSSL, inst_7.c_str(), inst_7.length());
                  out=1;

                }
             }
        }

        /* Wait for transfering */
        if (entry=="3")
        {

          if (transferLogin==0)
          {
            cout<<"Please login your account first. \n";
          }

          else{
              cout<<"---Waiting for account transferring---\n";

              struct sockaddr_in w_serverAddr; 
              struct sockaddr_in w_clientAddr;
              int wait_c;
              int wait_c_sockfd = socket(PF_INET, SOCK_STREAM, 0);
              if (wait_c_sockfd <0 )
              {
                cout<<"Setup error.\n";
                exit(0);
              }
              int localPort1 = atoi(loginPort.c_str());
             
              w_serverAddr.sin_family = AF_INET;
              w_serverAddr.sin_port = htons(localPort1);
              w_serverAddr.sin_addr.s_addr = INADDR_ANY;
              memset(w_serverAddr.sin_zero, 0, sizeof (w_serverAddr));  

              int bindCon=bind(wait_c_sockfd,(struct sockaddr *) &w_serverAddr, sizeof(w_serverAddr));
              /* Error detection when connection failed */
              if (bindCon<0)
              {
                cout<<"Binding error.\n";
                exit(0);
              }

              listen (wait_c_sockfd, 50);

              int w_cli_socket = accept (wait_c_sockfd, (struct sockaddr *) &w_clientAddr, (socklen_t *)&(w_clientAddr)); 
              if (w_cli_socket<0)
              {
                cout<<"Accepting error. \n";
                exit(0);
              }
              cout<<"A payer has connected! \n";

              /* Build SSL between two clients */
              SSL_CTX *wsslctx;
              SSL *wcSSL;

              InitializeSSL();

              wsslctx = SSL_CTX_new( TLSv1_server_method());
              SSL_CTX_set_options(sslctx, SSL_OP_SINGLE_DH_USE);
              int use_cert2 = SSL_CTX_use_certificate_file(wsslctx, "mycert.pem" , SSL_FILETYPE_PEM);
              if (use_cert2<=0)
              {
                ERR_print_errors_fp(stderr);
                abort();
              }
              int use_prv2 = SSL_CTX_use_PrivateKey_file(wsslctx, "mykey.pem", SSL_FILETYPE_PEM);
              if (use_prv2<=0)
              {
                ERR_print_errors_fp(stderr);
                abort();
              }
              if (!SSL_CTX_check_private_key(wsslctx))
              {
                fprintf(stderr,"Private key does not match the public certificate.\n");
                abort();
              }

              wcSSL = SSL_new(wsslctx);
              SSL_set_fd(wcSSL, w_cli_socket);
              int wssl_err = SSL_accept(wcSSL);
              if(wssl_err <= 0)
              {
                  ShutdownSSL(wcSSL);
                  cout<<"connection error!\n";
              }

              memset(c_msg,0,1024);
              SSL_read(wcSSL, c_msg, 1024);
              string cmsg (c_msg);
              string hashTransfer = "TRANSFER#";
              memset(buffer,0,1024);
              strcat(buffer, hashTransfer.c_str());         
              strcat(buffer, c_msg);

              /* Send transfer message to server */
              SSL_write(cSSL, buffer, strlen(buffer));
              
              memset(buffer, 0, 1024);
              /* Get server's reply */
              SSL_read(cSSL, buffer,1024);           
              /* Send server's message to payer */
              SSL_write(wcSSL, buffer, strlen(buffer));
              cout<<buffer<<endl;

          }
          
        }

        /* Transfer money to someone */
        if (entry=="4")
        {
          if (transferLogin==0)
          {
            cout<<"Please login your account first. \n";
          }

          else{

              bool gate = 0;
              bool gate2 = 0;
              while (gate==0)
              {
                  cout<<"Please enter the payee's name: ";
                  cin>>payeeName;

                  while (gate2==0){
                  cout<<"Please enter the transfer amount: ";
                  cin>>tranferAmount;
	                  if (atoi(tranferAmount.c_str()) > atoi(userAccount.c_str()))
	                  {
	                  	cout<<"You can't afford the payment. Please enter again!\n";
	                  }
	                  else{
	                  	gate2=1;
	                  }
	              }

                  /* Get the payee's IP and port from server */
                  string askPayee = "FIND#"+payeeName;
                  SSL_write(cSSL, askPayee.c_str(), askPayee.length());
                  memset(buffer,0,1024);
                  SSL_read(cSSL, buffer, 1024); 

                  /* Error detection for server reply */
                  if (buffer[0]=='2')
                  {
                     cout<<buffer<<endl;
                  }
                  else{
                     gate=1;
                  }

              }

              /* Get payee's IP and port number from server reply */
              string serverReply (buffer);
              string tempPort;

              size_t IPPos = serverReply.find("#");
              payeeIP = serverReply.substr(0,IPPos);
              tempPort = serverReply.substr(IPPos+1);
              payeePort = atoi(tempPort.c_str());

            
              /* Build socket */
              struct sockaddr_in serverAddr2;

              int socket_cli2 = socket(PF_INET, SOCK_STREAM, 0);

              /* Error detection when there is a setup error */
              if (socket_cli2<0)
              {
                cout<<"Setup error.\n";
                exit(0);
              }
              const char* tempIP = payeeIP.c_str();

              /* Set the connection using the port number and IP given by user */
              serverAddr2.sin_family = AF_INET;
              serverAddr2.sin_port = htons(payeePort);
              serverAddr2.sin_addr.s_addr = inet_addr(tempIP);
              memset(serverAddr2.sin_zero, 0, sizeof (serverAddr2));  

              /* Connecting */
              int Con2=connect(socket_cli2, (struct sockaddr *) &serverAddr2, sizeof(serverAddr2));

              /* Error detection when connection failed */
              if (Con2<0)
              {
                cout<<"Connect error.\n";
                exit(0);
              }
              cout<<"successful connect to payee!!!\n";

              /* Build SSL between two clients */
              SSL_CTX *wsslctx;
              SSL * wcSSL;
              InitializeSSL();

              wsslctx = SSL_CTX_new( TLSv1_client_method());
              SSL_CTX_set_options(wsslctx, SSL_OP_NO_SSLv2);

              wcSSL = SSL_new(wsslctx);
              SSL_set_fd(wcSSL, socket_cli2);

              int wssl_err = SSL_connect(wcSSL);
              if (wssl_err<1)
              {
                cout<<"connection error!\n";
                ShutdownSSL(wcSSL);
              }


              /* Send message to payee */
              t_msg = payeeName+"#"+tranferAmount+"#"+loginName;
              SSL_write(wcSSL, t_msg.c_str(), t_msg.length());
              memset(buffer,0,1024);

              /* Receive messsage from payee */
              // recv(socket_cli2,buffer,1024,0);
              SSL_read(wcSSL, buffer,1024);
              cout<<buffer<<endl;

            }


        }

        /* List */
        if (entry=="5")
        {
            string inst_li = "LIST#"+loginName;
            SSL_write(cSSL,inst_li.c_str(), inst_li.length());
            memset (buffer,0,1024);
            SSL_read(cSSL, buffer,1024); 
            cout<< endl << buffer <<endl;
        }

        /* Exit */
        if (entry=="E")
        {
            string inst_e = "EXIT";
            SSL_write(cSSL,inst_e.c_str(), inst_e.length());
            memset (buffer,0,1024);
            SSL_read(cSSL, buffer,1024); 
            string rsp(buffer);
            cout<< rsp <<endl;
            login=1;
        }

   }

  /* Close the socket */
  SSL_CTX_free(sslctx);
  close(socket_cli);
  return 0;
}



#include<winsock2.h>//winsock2的头文件
#include <unistd.h>
#include<iostream>
#include<queue>
using  namespace std;


#pragma comment(lib, "ws2_32.lib")

#define ip_server "192.168.43.158"
#define ip_client "192.168.43.158"

DWORD WINAPI ThreadFun(LPVOID lpThreadParameter);

queue<pair<string,string>> rev;

int  main()
{

    //加载winsock2的环境
    WSADATA  wd;
    WSAStartup(MAKEWORD(2, 2), &wd);
    SOCKET  s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


    //2.链接服务器
    sockaddr_in   addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = inet_addr(ip_server);

    int len = sizeof(sockaddr_in);
    connect(s, (SOCKADDR*)&addr, len) ;

    
    //3接收服务端的消息
    char buf[100] = { 0 };
    recv(s, buf, 100, 0);
    cout <<  buf << endl;

    HANDLE hThread = CreateThread(NULL, 0, ThreadFun, (LPVOID)s, 0, NULL);


    while(1){
        Sleep(500);

        while(!rev.empty()){
            cout<<"IP地址："<<rev.front().second<<" 对您私聊说：";
            cout<<rev.front().first<<endl;
            rev.pop();
            Sleep(100);
        }


        char buf[100] = {0};
        cout << "请输入聊天内容:";
        gets(buf);
        send(s, buf, 100, 0); 
        
        if(buf[0]=='@'){
            cout<<"请输入你要私聊对象的IP地址：";
            gets(buf);
            send(s, buf, 100, 0); 
            cout << "请输入私聊内容:";
            gets(buf);
            send(s, buf, 100, 0);

        }


    }

    closesocket(s);
    WSACleanup();
    return 0;
}


DWORD WINAPI ThreadFun(LPVOID lpThreadParameter){
        SOCKET s= (SOCKET)lpThreadParameter;
        int ret;
        char rev_buf[100],ip[100];

        while(1){
            ret = recv(s, rev_buf, 100, 0);
            if(ret==SOCKET_ERROR || ret==0){
                break;
            }
            ret = recv(s, ip, 100, 0);
            if(ret==SOCKET_ERROR || ret==0){
                break;
            }
            
            rev.push({rev_buf,ip});
        }      
        return 0;
        
}
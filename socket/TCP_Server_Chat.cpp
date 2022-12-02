#include <winsock2.h> 
#include <iostream>
#include<set>
#pragma comment(lib, "ws2_32.lib")
using namespace std;


DWORD WINAPI ThreadFun(LPVOID lpThreadParameter);

struct Soc
{
    SOCKET s;
    sockaddr_in s_in;
    bool operator <(const Soc& A) const { 
		return s < A.s;
	}
};

set<Soc> st;

int main()
{
    cout<<"�������������..."<<endl;
    cout<<"�����������ϣ�"<<endl;
    WSADATA wd;
    WSAStartup(MAKEWORD(2,2), &wd);

    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);



    sockaddr_in addr;
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    addr.sin_addr.s_addr = inet_addr("192.168.43.158");

    int len = sizeof(sockaddr_in);
    bind(s, (SOCKADDR*)&addr, len);



    listen(s,5);



    while(true)
    {
        sockaddr_in addrClient;
        len = sizeof(sockaddr_in);

        SOCKET c = accept(s, (SOCKADDR*)&addrClient, &len);

        Soc tmp = {c,addrClient};
        st.insert(tmp);


        HANDLE hThread = CreateThread(NULL, 0, ThreadFun, (LPVOID)c, 0, NULL);
        CloseHandle(hThread); 

    }

    closesocket(s);
    WSACleanup();

    return 0;
}

DWORD WINAPI ThreadFun(LPVOID lpThreadParameter)
{

    SOCKET c= (SOCKET)lpThreadParameter;
    
    char ip_self[100];
     for(auto x:st){
        if(x.s == c){
            strcpy(ip_self,inet_ntoa(x.s_in.sin_addr));
            break;
        }
    }

    cout << "��ӭ" << ip_self << " ���������ң�\t" ;
    cout<<"��ǰ����������"<<st.size()<<endl;

    char buf[100] = {0};
    char ip[100]={0};

    sprintf(buf,"��ӭ %s ���������ң� ",ip_self);
    send(c,buf, 100, 0);

    int ret;
    while (1) 
    {
        ret = recv(c, buf, 100, 0);
        if(ret==SOCKET_ERROR || ret==0)
            break;

        if(buf[0]=='@'){

            cout<<"��ַ�б�:"<<endl;
            for(auto x:st){
                cout<<inet_ntoa(x.s_in.sin_addr)<<endl;
            }

            ret = recv(c, ip, 100, 0);
            if(ret==SOCKET_ERROR || ret==0)
                break;

            bool flag= false;

            SOCKET des;
            for(auto x:st){
                if( strcmp(inet_ntoa(x.s_in.sin_addr),ip) == 0 ){
                    flag=true;
                    des=x.s;
                    break;
                }
            }

            ret = recv(c, buf, 100, 0);
            if(ret==SOCKET_ERROR || ret==0)
                break;

           

            if(flag){
                send(des,buf,100,0);
                send(des,ip_self,100,0);
            }

            else{
                cout<<"��ַ����"<<endl;
            }
        }
        else{
            cout << ip_self << "˵��" << buf << endl;
        }
    };

    cout << ip_self << "�뿪�������ң�\n";
    for(auto x:st){
        if(x.s == c){
            st.erase(x);
            break;
        }
    }
    return 0;
}
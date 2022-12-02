#include<iostream>
#include <ctime>
#include<graphics.h>
#include<conio.h>
#include<windows.h>
#define endl "\n"
using namespace std;
const int N = 20;          //地图 大小  在100以内时效果较好
const int N1 = N * N;
const int b = 30;             //图形大小
const int dx[4] = { -1,1,0,0 }, dy[4] = { 0,0,-1,1 };
const double reward = 1e10;

class Natu_env {
private:
    int wind_v[N][N];   //风考虑2维
    int wind_d[N][N];     //  0,1,2,3  上北下南左西右东
    int temperature;
public:
    void init_nat() {
        srand(time(NULL));
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                int t = rand() % 2;
                if (t) {
                    wind_v[i][j] = rand() % 100;
                    wind_d[i][j] = rand() % 4;
                }
                else
                    wind_v[i][j] = 0;
            }
        }
    }

    void trans_wind(double  r[][4]) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (wind_v[i][j]) {
                    int x = i * N + j;
                    int k = wind_d[i][j];
                    if (i + dx[k] >= 0 && i + dx[k] < N && j + dy[k] >= 0 && j + dy[k] < N
                        && r[x][k] == -100)
                        r[x][k] =  -(100-wind_v[i][j]);
                }
            }
        }
    }

    void print_wind() {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                cout << wind_v[i][j] << '-' << wind_d[i][j] << "   ";
            }
            cout << endl;
        }
    }
};

class  Obstacle {
private:
    int  obs[N][N];     //数字大小代表高度，三维时
    bool flag[N][N];   //用于检查地图的可达性
    bool legal = false;
 
public:
    void dfs(int x, int y) {
        if (x == N - 1 && y == N - 1) {
            legal = true;
            return;
        }

        flag[x][y] = 1;
        for (int i = 0; i < 4; i++) {
            if (x + dx[i] >= 0 && x + dx[i] < N && y + dy[i] >= 0 && y + dy[i] < N 
                && !flag[x + dx[i]][y + dy[i]] && obs[x+dx[i]] [y+dy[i]]==0) {
                dfs(x + dx[i], y + dy[i]);
            }
       }
    }
    int get_obs(int i, int j) {
        return obs[i][j];
    }

    void init_obs() {
        while (!legal) {
            srand(time(NULL));
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    int t = rand() % 100;
                    if (t < 70)
                        obs[i][j] = 0;
                    else
                        obs[i][j] = t;
                }
            }
            obs[0][0] = obs[N - 1][N - 1] = 0;
            memset(flag, 0, sizeof(flag));
            dfs(0, 0);
        }
    }

    void print_obs() {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                cout << obs[i][j] << "    ";
            }
            cout << endl;
        }
    }

    void trans_obs(double  r[][4]) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {

                int x = i * N + j;
                for (int k = 0; k < 4; k++) {
                    if (i + dx[k] >= 0 && i + dx[k] < N && j + dy[k] >= 0 && j + dy[k] < N
                        && obs[i + dx[k]][j + dy[k]] == 0) {
                        r[x][k] = -100;
                    }
                    else
                        r[x][k] = -reward;
                }

            }
        }

        if (obs[N - 1][N - 2] == 0)
            r[N1 - 2][3] = reward;
        if (obs[N - 2][N - 1] == 0)
            r[N1 - 1 - N][1] = reward;
    }
};

class Env :public Natu_env, public Obstacle {       //总环境  继承
public:
    Env() {                  //构造函数
        init_obs();
        init_nat();

        initgraph(N * b, N * b);
        setlinecolor(DARKGRAY);
        for (int i = 1; i < N; i++) {
            line(i * b, 0, i * b, N * b);
            line(0, i * b, N * b, i * b);
        }
        setfillcolor(WHITE);
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (get_obs(i, j) == 0) {
                    fillrectangle(j * b, i * b, j * b + b, i * b + b);
                }
            }
        }
        setfillcolor(YELLOW);
        fillrectangle(N * b - b, N * b - b, N * b, N * b);
        setfillcolor(RED);
        solidcircle(b / 2, b / 2, b / 4);
        _getch();
    }
};

class UAV {
private:
    int v_level, v_vertical;
    int max_v_l, max_v_v, acceleration;
    int min_h = 70, max_h = 100, radius = 1;

public:

    double  q[N1][4] = { 0 };
    double  r[N1][4];
    double gamma = 0.9;
    int episode = 4 * N + 200;

    int target_state = N1 - 1;
    int start_state; 
    int current_state;
    int current_action;
    int next_state;
    int visit[N1];

    double next_state_max_q;
    double current_action_point;
    double M(double * p){
        double  ma = -reward;
        for (int i = 0; i < 4; i++)
            ma = max(ma, p[i]);
        return ma;
    }

    void print_r() {
        cout << endl;
        for (int i = 0; i < N1; i++) {
            cout << i << "###   ";
            for (int j = 0; j < 4; j++) {
                cout << r[i][j] << ' ';
            }
            cout << endl;
        }
    }

    void print_q() {
        cout << endl;
        cout << "Q值表更新过程为" << endl;
        for (int i = 0; i < N1; i++) {
            cout << i << "###   ";
            for (int j = 0; j < 4; j++) {
                cout << q[i][j] << ' ';
            }
            cout << endl;
        }
    }

    void Q_learing() {
        

        for (int i = 0; i < episode; i++) {
            //memset(visit, 0, sizeof(visit));
            srand(time(NULL));
            start_state = 0;

          
            current_state = start_state;
            int epsilon = 105 - i * 100 / episode;
            
            
            setfillcolor(YELLOW);
            solidcircle(b / 2 + N * b - b, b / 2 + N * b - b, b / 4);
         
            setfillcolor(RED);
            solidcircle(b / 2, b / 2, b / 4);

            while (current_state != target_state) {
               
                int t = rand() % 100;
                if (t > epsilon  && q[current_state][0] && q[current_state][1] && q[current_state][2] && q[current_state][3] ) {    
                   
                    double ma = -reward;
                    for (int i = 0; i < 4; i++)
                        if (q[current_state][i] > ma) {
                            current_action = i;
                            ma = q[current_state][i];
                        }

                }
                else
                    current_action = rand() % 4;

                current_action_point = r[current_state][current_action];


                if (current_action_point == -reward)
                    q[current_state][current_action] = current_action_point;

                else
                {   

					int c = current_action;


					next_state = current_state + dx[c] * N + dy[c];

					int x = current_state / N, y = current_state % N;

					//Sleep(0.05);

					setfillcolor(WHITE);
					solidcircle(b / 2 + y * b, b / 2 + x * b, b / 4);
					setfillcolor(RED);
					solidcircle(b / 2 + y * b + dy[c] * b, b / 2 + x * b + dx[c] * b, b / 4);

					next_state_max_q = M(q[next_state]);

					// 当前动作的经验总得分 = 当前动作得分 + γ X 执行该动作后的下一个状态的最大的经验得分
					// 即：积累经验 = 动作执行后的即时奖励 + 下一状态根据现有学习经验中最有价值的选择 X 折扣率
					q[current_state][current_action] = current_action_point + gamma * next_state_max_q;
					current_state = next_state;
                    
                }
            }

        }

    }

    void route_planning(Env e) {       //Q-learning
        e.trans_obs(r);
        e.trans_wind(r);


        Q_learing();
        
        setfillcolor(YELLOW);
        solidcircle(b / 2 + N * b - b, b / 2 + N * b - b, b / 4);
        setfillcolor(RED);
        solidcircle(b / 2, b / 2, b / 4);



        int current_state = 0;
        while (current_state != target_state) {
            double ma = -reward;
            for (int i = 0; i < 4; i++) {
                if (q[current_state][i] > ma) {
                    current_action = i;
                    ma = q[current_state][i];
                }
            }

            int c = current_action;
            int x = current_state / N, y = current_state % N;
            Sleep(100);
            setfillcolor(RED);
            solidcircle(b / 2 + y * b + dy[c] * b, b / 2 + x * b + dx[c] * b, b / 4);
            current_state += dx[current_action] * N + dy[current_action];


            /*
            int x = current_state / N, y = current_state % N;
            cout << "从： " << x<<","<<y << "  走到了:  ";

            current_state += dx[current_action] * N + dy[current_action];

            x = current_state / N, y = current_state % N;
            cout << x << "," << y << endl;
            */

        }

        setfillcolor(RED);
        solidcircle(b / 2 + N * b - b, b / 2 + N * b - b, b / 4);
        
        _getch();

    }

};
int main() {
 
    Env env;
    UAV uav;
    uav.route_planning(env);

    env.print_obs();
    //env.print_wind();
    //uav.print_r();
    //uav.print_q();
    return 0;
}



/*

* 




*/

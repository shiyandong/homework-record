#include<iostream>
using namespace std;
class Chess {               
	int S[10];             //存储棋盘
	int Pos = 0;		   //用于记录算法选择的位置
public:

	Chess() {
		init();
	}
	void init() {
		for (int i = 0; i < 10; i++)
			S[i] = 0;
	}
	void output() {
		cout << "---------" << endl;
		for (int i = 1; i < 10; i++) {
			cout << S[i] << ' ';
			if (i % 3 == 0)
				cout << endl;
		}
		cout << "---------" << endl;

	}
	bool input() {
		int x, y;
		cin >> x >> y;
		int pos = x * 3 - 3 + y;
		if (S[pos] != 0) {
			cout << "error!" << endl;
			return false;
		}
		else {
			S[pos] = 1;
			return true;
		}
	}
	void random() {
		int v[10],cnt = 0;
		for (int i = 1; i < 10; i++)
			if (S[i] == 0)
				v[cnt++] = i;
		int pos = rand() % cnt;
		S[v[pos]] = 2;
	}
	int grade(int* s) {               // 对当前状态打分，机器方胜利为1，人类反胜利为-1
		if (is_game_end(s,true) == 2)
			return 1;
		if (is_game_end(s,true) == 1)
			return -1;

		return 0;
	}

	int dfs(int *s,int x,int deg) {        // deg为搜索深度，仅当为0时修改Pos
		
		int ss[10];                       //复制棋盘
		for (int i = 1; i < 10; i++)
			ss[i] = s[i];

		for (int i = 1; i < 10; i++) {
			if (ss[i] == 0) {
				ss[i] = x + 1;
				if (is_game_end(ss, true) == x + 1) {        //是否就差一步
					if (deg == 0 && x == 1)
						Pos = i;
					if (x == 1) return 1;
					else return -1;
				}
				ss[i] = 0;
			}
		}
		



		
		int cnt = 0;
		for (int i = 1; i < 10; i++)
			if (s[i])
				cnt++;
		if (cnt == 9)
			return 0;                        //如果棋盘满了，返回0

		int gra;
		if (x == 1)               //根据当前谁先手，成绩grade初始值不同
			gra = -1e6;
		else
			gra = 1e6;

		for (int i = 1; i < 10; i++) {
			if (ss[i] == 0) {
				ss[i] = x + 1;
				int tmp = dfs(ss, x ^ 1, deg + 1);
				if (x == 1) {
					if (tmp > gra) {                //机器方取最大
						gra = tmp; 
						if(deg==0)	Pos = i;
					}
					if (gra == 1) {                  //剪枝
						return gra;
					}
						
				}
				else {
					gra = min(gra, tmp);           //人类方取最小
					if (gra == -1) {                 //剪枝
						return gra;
					}
				
				}
				
				ss[i] = 0;
			}
		}

		return gra;

	}

	void alpha_beta() {
		dfs(S, 1, 0);
		S[Pos] = 2;
	}
	
	int is_game_end(int* a,bool flag) {            //判断游戏是否结束
		int s[10];
		if (flag)
			for (int i = 1; i < 10; i++)
				s[i] = a[i];
		else
			for (int i = 1; i < 10; i++)
				s[i] = S[i];

		bool f1 = false, f2 = false;
		for (int i = 1; i <= 3; i++) {
			int be = (i - 1) * 3 + 1;
			if (s[be] == 1 && s[be + 1] == 1 && s[be + 2] == 1 
				|| s[i] == 1 && s[i + 3] == 1 && s[i + 6] == 1 )
				f1 = true;
		}
		if (s[1] == 1 && s[5] == 1 && s[9] == 1 
			|| s[3] == 1 && s[5] == 1 && s[7] == 1 )
			f1 = true;



		for (int i = 1; i <= 3; i++) {
			int be = (i - 1) * 3 + 1;
			if ( s[be] == 2 && s[be + 1] == 2 && s[be + 2] == 2
				|| s[i] == 2 && s[i + 3] == 2 && s[i + 6] == 2)
				f2 = true;
		}
		if (s[1] == 2 && s[5] == 2 && s[9] == 2
			|| s[3] == 2 && s[5] == 2 && s[7] == 2)
			f2 = true;
		
		if (f1)	return 1;
		if (f2) return 2;
		return 0;
	}
};


void solve() {
	Chess chess;

	while (true) {                //交互逻辑
		int cnt = 0;
		chess.output();
		while (true) {
			while (!chess.input());
			chess.output();
			if (chess.is_game_end(NULL, false) == 1) {
				cout << "You win!" << endl;
				break;
			}
			cnt++;
			if (cnt == 9) {
				cout << "Draw!" << endl;
				break;
			}
			//chess.random();
			chess.alpha_beta();
			chess.output();
			if (chess.is_game_end(NULL, false) == 2) {
				cout << "You loss!" << endl;
				break;
			}
			cnt++;
		}
		cout << "again?\t\t y/n" << endl;
		
		char ch;
		cin >> ch;
		if (ch == 'n')
			break;
		chess.init();
	}

}

int main() {

	solve();
	return 0;
}
/*


*/
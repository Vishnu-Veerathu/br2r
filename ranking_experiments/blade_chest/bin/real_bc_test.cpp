#include <bits/stdc++.h>

using namespace std;

//--------------------------------------------------------------------------------------------

bool beats(vector < vector < double > > &P, int arm1, int arm2){
    return P[arm1][arm2] > P[arm2][arm1];
}

//--------------------------------------------------------------------------------------------

// beats function when arms is a permutation of the arms and P follows the permutation order as well. 
bool perm_beats(vector < vector < double > > &P, vector <int> &arms, int idx1, int idx2){
    int arm1 = arms[idx1];
    int arm2 = arms[idx2];
    return beats(P,arm1,arm2);
}
//-------------------------------------------------------------------------------------------

// Find upsets for ranking w.r.t. P
int upsets(vector<int> &ranking, vector<vector<double>> &P, bool print = false)
{
    int upsets = 0;
    int n = ranking.size();
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < i; j++)
        {
        	if(print)
        		cout << P[ranking[i]][ranking[j]] << " " << P[ranking[j]][ranking[i]] << endl;
            if (P[ranking[i]][ranking[j]] > P[ranking[j]][ranking[i]])
            {
                upsets++;
            }
        }
    }
    return upsets;
}
//--------------------------------------------------------------------------------------------

// Min Upsets from u, v and P, upsets wrt P_true
int min_upsets(vector <double> u, vector <double> v, vector <vector <double> > P, vector <vector <double> > P_true){
    set <pair < double, int> > s;
    int n = u.size();
    for(int i = 0;i < u.size();i++){
        s.insert({atan2(v[i],u[i]),i});
    }

    vector <int> ranking;
    for(auto e: s){
        ranking.push_back(e.second);
    }

    int mini = n*n;
    vector<int> best_ranking;

    for(int i = 0;i < n;i++){
        int ranking_eval = upsets(ranking,P);
        if(ranking_eval < mini){
            mini = ranking_eval;
            best_ranking = ranking;
        }
        ranking.push_back(ranking[0]);
        ranking.erase(ranking.begin()); 
    }
    return upsets(best_ranking,P_true);
}

//-------------------------------------------------------------------------------------------

vector<int> calc_borda_ranking(vector <int> &arms, vector<vector<double > > &P){
    set < pair <double,int> > s;
    int n = arms.size();
    for(int i = 0;i < n;i++){
        int score = 0;
        for(int j = 0;j < n;j++){
            score += P[arms[i]][arms[j]] - P[arms[j]][arms[i]];
        }
        s.insert({-score,arms[i]});
    }

    vector <int> ranking;
    for(auto itr: s){
        ranking.push_back(itr.second);
    }
    return ranking;
}

//-------------------------------------------------------------------------------------------

int calc_borda_upsets(vector < vector < double > > &P, vector < vector < double> > &P_true){
    int n = P.size();
    vector <int> arms;
    for(int i = 0;i < n;i++){
        arms.push_back(i);
    }
    vector <int> best_ranking = calc_borda_ranking(arms,P);
    // for(auto e : P_true){
    // 	for(auto e1: e){
    // 		cout << e1 << " ";
    // 	}
    // 	cout << endl;
    // }
    return upsets(best_ranking,P_true);
}

//-------------------------------------------------------------------------------------------

// Finds copeland ranking.
vector<int> calc_copeland_ranking(vector <int> &arms,vector < vector < double > > &P){
    set < pair <int,int> > s;
    int n = arms.size();
    // cout << "Creating Set" << endl;
    for(int i = 0;i < n;i++){
        int beats = 0;
        for(int j = 0;j < n;j++){
            if(perm_beats(P,arms,i,j)){
                beats++;
            }
        }
        // cout << beats << " " << arms[i] << endl;
        s.insert({-beats,arms[i]});
    }

    vector <int> ranking;
    for(auto itr: s){
        ranking.push_back(itr.second);
    }

    return ranking;
}

//-------------------------------------------------------------------------------------------

// Find number of copeland upsets for ranking generated by P w.r.t. P_true.
int calc_copeland_upsets(vector < vector < double > > &P,vector < vector <double > > &P_true){
    int n = P.size();
    vector <int> arms;
    for(int i = 0;i < n;i++){
        arms.push_back(i);
    }
    vector <int> best_ranking = calc_copeland_ranking(arms,P);
    // for(auto e: best_ranking){
    // 	cout << e << " ";
    // }
    // cout << endl;
    return upsets(best_ranking,P_true);
}

//-------------------------------------------------------------------------------------------

// Finds ranking assuming Rank 2 model. Makes additional recursive calls/considerations since all inputs may not by of Rank 2.
vector<int> optimal_estimated_ranking(vector <int> arms, vector < vector < double > > P, bool use_copeland = false){
    // vector <double> s;
    int n = arms.size();

    if(n <= 1){
        vector <int> best_ranking = arms;
        return best_ranking;
    }

    // Find a cycle
    int a;
    int b;
    int c;
    bool cycle = false;

    for(a = 0;a < n;a++){
        for(b = 0;b < n;b++){
            for(c=0;c<n;c++){
                if(a == b or b== c or c == a){
                    continue;
                }
                if(perm_beats(P,arms,a,b) && perm_beats(P,arms,b,c) && perm_beats(P,arms,c,a)){
                    cycle = true;
                    goto skip;
                }
            }
        }
    }
    skip:

    if (!cycle || n <= 3){
        // cout << "Default to Copeland" << endl;
        return calc_copeland_ranking(arms, P);
    }
    
    vector <int> s_a,s_b,s_c,s_lead,s_behind;
    for(int i = 0;i < n;i++){
        int arm = arms[i];
        bool beats_a = perm_beats(P,arms,i,a);
        bool beats_b = perm_beats(P,arms,i,b);
        bool beats_c = perm_beats(P,arms,i,c);

        if(beats_a && beats_b && beats_c){
            s_lead.push_back(arm);
        }
        else if(!beats_a && !beats_b && !beats_c){
            s_behind.push_back(arm);
        }
        else if(beats_a && !beats_b){
            s_c.push_back(arm);
        }
        else if(beats_b && !beats_c){
            s_a.push_back(arm);
        }
        else if(beats_c && !beats_a){
            s_b.push_back(arm);
        }   
    }

    vector<int> ss_a = optimal_estimated_ranking(s_a,P);
    vector<int> ss_b = optimal_estimated_ranking(s_b,P);
    vector<int> ss_c = optimal_estimated_ranking(s_c,P);
    vector<int> ss_lead = optimal_estimated_ranking(s_lead,P);
    vector<int> ss_behind = optimal_estimated_ranking(s_behind,P);

    vector <int> ranking;
    for(int i = 0;i < ss_a.size();i++){
        ranking.push_back(ss_a[i]);
    }
    for(int i = 0;i < ss_b.size();i++){
        ranking.push_back(ss_b[i]);
    }
    for(int i = 0;i < ss_c.size();i++){
        ranking.push_back(ss_c[i]);
    }

    int mini = n*n;
    vector<int> best_ranking;
    int rsize = ranking.size();
    for(int i = 0;i < rsize;i++){
        int ranking_eval = upsets(ranking,P);
        if(ranking_eval < mini){
            mini = ranking_eval;
            best_ranking = ranking;
        }
        ranking.push_back(ranking[0]);
        ranking.erase(ranking.begin()); 
    }

    vector <int> full_best_ranking;
    for(int i = 0;i < ss_lead.size();i++){
        full_best_ranking.push_back(ss_lead[i]);
    }
    for(int i = 0;i < rsize;i++){
        full_best_ranking.push_back(best_ranking[i]);
    }
    for(int i = 0;i < ss_behind.size();i++){
        full_best_ranking.push_back(ss_behind[i]);
    }

    int final_upsets = upsets(full_best_ranking, P);
    vector <int> copeland_ranking = calc_copeland_ranking(arms, P);
    int copleand_upsets = upsets(copeland_ranking, P);
    if(use_copeland && copleand_upsets < final_upsets){
        // cout << "Copeland Better for size = " << arms.size() << endl;
        return copeland_ranking;
    }

    return full_best_ranking;
}

//-------------------------------------------------------------------------------------------

// Number of upsets of P when ranked according to optimal Rank 2 method w.r.t. P_true.
int optimal_estimated_upsets(vector < vector < double > > &P,vector < vector <double > > &P_true, bool use_copeland = false){
    int n = P.size();
    vector <int> arms;
    for(int i = 0;i < n;i++){
        arms.push_back(i);
    }
    vector <int> best_ranking = optimal_estimated_ranking(arms,P,use_copeland);
    return upsets(best_ranking,P_true);
}

//-------------------------------------------------------------------------------------------

double dp(vector<double> &a, vector<double> &b){
	double sum = 0;
	for(int i = 0;i < a.size();i++){
		sum += a[i]*b[i];
	}
	return sum;
}

int main(int argc, char** argv){
	int n;
	cin >> n;
	// cout << argc << endl;
	// return 0;
	string s(argv[1]);
	// cout << s << endl;
	// return 0;
	int d = stoi(s);
	// return 0;
	vector < vector <double> > hvecs(n,vector<double>(d));
	vector < vector <double> > tvecs(n,vector<double>(d));

	for(int i = 0;i < n;i++){
		for(int j = 0;j < d;j++){
			cin >> hvecs[i][j];
		}
	}

	for(int i = 0;i < n;i++){
		for(int j = 0;j < d;j++){
			cin >> tvecs[i][j];
		}
	}

	vector <double> r(n,0);
	for(int i = 0;i < n;i++){
		cin >> r[i];
	}

   vector < vector < double > > P(n,vector <double>(n,0));
   for(int i =0;i < n;i++){
   	for(int j = 0;j < n;j++){
   	 P[i][j] = dp(hvecs[i],tvecs[j]) - dp(hvecs[j],tvecs[i]) + r[i] + r[j];
   	}
   }

   // open a file in read mode.
   ifstream infile; 
   infile.open("../create_data/true_data.txt");
   vector < vector < double > > P_test(n,vector <double>(n,0));
   for(int i = 0;i < n;i++){
    for(int j = 0;j < n;j++){
        infile >> P_test[i][j];
    }
   }
 //   int a,b,winner;
 //   // double ex_pred;
 //   int upsets = 0;
 //   int inputs = 0;
 //   while(infile >> a){
 //   	inputs++;
 //   	infile >> b;
 //   	infile >> winner;
 //   	// cout << a << " " << b << " " << winner << endl;
 //   	if(a == winner){
 //   		P_test[a][b] += 1;
 //   	} else if(b == winner){
 //   		P_test[b][a] += 1;
 //   	}
 //   }

	// int nonz = 0;   
	// for(int i = 0;i < n;i++){
	// 	for(int j = 0;j < n;j++){
	// 		if(P_test[i][j]){
	// 			nonz++;
	// 		}
	// 	}
	// }
   	// cout << endl;
   // cout <<  inputs << endl << n << endl << nonz << endl;
   // cout << calc_borda_upsets(P,P_test);
	
   cout << calc_copeland_upsets(P,P_test) << " " << calc_borda_upsets(P,P_test) << " " << optimal_estimated_upsets(P,P_test) << " " << optimal_estimated_upsets(P,P_test,true) << endl;
   return 0;
}
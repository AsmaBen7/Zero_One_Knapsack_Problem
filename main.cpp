#include<iostream>
#include<fstream>
#include<vector>
#include<random>

using namespace std;

#define buffer_size 65536

//利用隨機設備（std::random_device)再mod2產生亂數bits
//參考自
//https://blog.gtwang.org/programming/cpp-random-number-generator-and-probability-distribution-tutorial/
int get_string_bit(){
  random_device rd;
  return rd()%2;
}
//利用隨機設備（std::random_device)再mod item size 決定要改變哪個bit
int get_change_position(int a){
  random_device rd;
  return rd()%a;
}

int main(){

//宣告用來儲存dataset的資料結構
//vector使用參考自
//https://mropengate.blogspot.com/2015/07/cc-vector-stl.html
  int capacity;//背包容量(重量)
  int total_weight=0;//candidate的總重量
  int profit_temp=0;//儲存暫時最好的解(總profit)
  int optimal_profit=0;//已給定最好的解的總profit

  vector<int> object_profits;//物品價值
  vector<int> object_weights;//物品重量

  //物品是否放入背包，0不放；1放
  vector<int> object_slect_current_state;//candidate之bit vector
  vector<int> object_slect_ans;//儲存暫時最好的解之bit vector(欲輸出的答案)
  vector<int> object_slect_optimal_ans;//最佳解之bit vector(用來對照)

  //檔案處理參考自
  //http://www2.lssh.tp.edu.tw/~jing/teach/note/c/c-file.htm
  //宣告fstream物件
  fstream file_c;
  fstream file_p;
  fstream file_w;
  fstream file_s;

  char buffer[buffer_size];


  //開檔
  file_c.open("dataset/p08_c.txt", ios::in);
  file_p.open("dataset/p08_p.txt", ios::in);
  file_w.open("dataset/p08_w.txt", ios::in);
  file_s.open("dataset/p08_s.txt", ios::in);

  int index=0;
  if(!file_c || !file_p || !file_w)
    cout<<"檔案無法開啟！\n";

  //將dataset資料存入變數中
  else{
    do{
      file_p.getline(buffer, sizeof(buffer));
      object_profits.push_back(stoi(buffer));

      file_w.getline(buffer, sizeof(buffer));
      object_weights.push_back(stoi(buffer));

      file_s.getline(buffer, sizeof(buffer));
      object_slect_optimal_ans.push_back(stoi(buffer));


      //背包總容量重量
      if(index==0){
        file_c.getline(buffer, sizeof(buffer));
        capacity=stoi(buffer);
        cout <<endl<< "capacity:" << capacity<<endl
             << "index"<<"\t"<<"weights"<<"\t"<<"profits"<<"\t"<<"selecte"<<" "<<"optima"<<endl;
      }
      index++;
    }while(!file_p.eof()&&!file_w.eof());
  }

  file_c.close();
  file_p.close();
  file_w.close();
  file_s.close();

  //物品數量
  int item_size=object_weights.size();


  for(int i=0; i<item_size; i++){

    //亂數產生initial state(candidate)
    object_slect_current_state.push_back(get_string_bit());

    //暫時最好的解 <- 亂數產生initial state(candidate)
    object_slect_ans.push_back(object_slect_current_state[i]);

    //initial state(candidate)的總重量及總價值
    total_weight+=object_slect_current_state[i]*object_weights[i];
    profit_temp+=object_slect_current_state[i]*object_profits[i];

    //已給定最好的解的總價值(之後用來break while loop)
    optimal_profit+=object_slect_optimal_ans[i]*object_profits[i];
  }

  int h_1, h_2;//heuristic function, h_1存上一個，h_2存當前這一個
  int check=-1;//判斷使得決定要改變哪個bit時，不會跟前一個同樣位置
  int position;//改變bit位置的index
  int index_alg=0;//iteration的index
  int max_profit=0;//目前找到最好的解的總價值
  int iteration_limit=pow (2, item_size);

  //initial state 的heuristic function
  if(total_weight>capacity){
    h_1=-1;
  }
  else{
    h_1=profit_temp;
    max_profit=h_1;
  }

  //iteration開始
  do{
    //選cadidate，避免跟前一次一樣
    for(;;){
      position=get_change_position(item_size);
      if(position!=check){
        check=position;
        break;
      }
    }

    //將所改變的bit，其改變後的weight及profits做調整
    if(object_slect_current_state[position]==1){
      total_weight -= object_weights[position];
      profit_temp -= object_profits[position];
      object_slect_current_state[position]=0;
    }
    else{
      total_weight += object_weights[position];
      profit_temp += object_profits[position];
      object_slect_current_state[position]=1;
    }

    //heuristic function
    if(total_weight>capacity){
      h_2=-1;
    }
    else{
      h_2=profit_temp;
      //保留optima
      if(h_2>max_profit){
        max_profit=h_2;

        //將此state存進最好的state(bit array)
        for(int j=0; j<object_slect_ans.size(); j++){
          if(object_slect_ans[j]!=object_slect_current_state[j])
          object_slect_ans[j]=object_slect_current_state[j];
        }

        //輸出每一次最佳解改變時的狀態
        cout<<position<<"\t"<<"\t"<<max_profit<<"\t"<<object_slect_ans[position]<<endl;

        h_1=h_2;//交換

        //找到符合以給定的最佳解之總價值，break while loop
        if(max_profit==optimal_profit)
            break;
      }
    }

  index_alg++;
  }while(index_alg<iteration_limit);

  cout<<endl;


  total_weight=0;
  max_profit=0;

  //開始輸出
  for(int i=0; i<object_slect_ans.size(); i++){
    total_weight+=object_slect_ans[i]*object_weights[i];
    max_profit+=object_slect_ans[i]*object_profits[i];

    cout << i <<"\t"
    << object_weights[i] <<"\t"
    << object_profits[i] <<"\t"
    << object_slect_ans[i] <<"\t"
    << object_slect_optimal_ans[i]<<endl;
  }
  cout<<"capacity of knapsack: "<<"\t"<<capacity<<endl;
  cout<<"weight of candidate: "<<"\t"<<total_weight<<endl;
  cout<<"profit of candidate: "<<"\t"<<max_profit<<endl;
  cout<<"profit of optimal solution: "<<"\t"<<optimal_profit<<endl;
  cout<<"accuracy:"<<"\t"<<((double)(max_profit)*100/(double)optimal_profit)<<"%"<<endl;
  cout<<"iteration taken"<<"\t"<<index_alg<<endl;
  cout<<"iteration limit"<<"\t"<<iteration_limit<<endl;
  cout<<"rate of iteration taken:"<<"\t"<<((double)(index_alg)*100/(double)iteration_limit)<<"%"<<endl;

return 0;
}

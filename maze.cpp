#include <iostream>
using namespace std;

typedef struct stack{ //儲存岔路/走過路的位置
    int x;
    int y;
    int previous_x;
    int previous_y;
    int path_count;
    bool directions[8]; // 順序：上 下 左 右 右上 右下 左上 左下 ; 1為有路 0無路
    struct stack *below;
}Stack;

class maze{
    int maze_len, maze_width;
    bool **maze_map;
    int current_x, current_y; //程式現在座標
    bool current_directions[8]; // 順序：上 下 左 右 右上 右下 左上 左下 ; 1為有路 0無路
    int previous_x, previous_y; //上個位置，用於避免返回原路
    int end_x, end_y; //終點座標
    Stack *crossroad_top; //記岔路
    Stack *passed_path_top; //記走過的路
    
    public:
        bool no_crossroad; //若為1，則無岔路可返回
        int path_count;

        bool at_end(){ //若到達終點，則回傳1
            if((current_x==end_x)&&(current_y==end_y)){
                return 1;
            }
            return 0;
        }

        void get_maze_size(){
            while(1){ //get maze length
                cout << "Enter maze length: ";
                cin >> maze_len;

                if(maze_len>0){
                    break;
                }
                else{
                    cout << "[ERROR] maze length must be bigger than 0." << endl;
                }
            }
            
            while(1){ //get maze width
                cout << "Enter maze width: ";
                cin >> maze_width;

                if(maze_width>0){
                    break;
                }
                else{
                    cout << "[ERROR] maze width must be bigger than 0." << endl;
                }
            }
        }

        void create_maze(){
            maze_map = new bool *[maze_len];
            cout << "Enter " << maze_width << " data for each row." << endl;

            for(int i=0; i<maze_len; i++){
                cout << "row [" << i << "]: ";
                maze_map[i] = new bool[maze_width];
                for(int j=0; j<maze_width; j++){
                    cin >> maze_map[i][j];
                }
            }

            //設定終點座標
            end_x = maze_len-1;
            end_y = maze_width-1;
            current_x = 0;
            current_y = 0;

            no_crossroad = 1;
            crossroad_top = NULL;
            
            //將起點加入passed_path_top
            Stack *passed_path = new Stack;
            passed_path->x = 0;
            passed_path->y = 0;
            passed_path_top = passed_path;

            cout << "Maze created." << endl;
        }

        void find_road(){ //找出可走的方向，回傳方向數，並將可走方向存於current_directions
            path_count = 0;
            //若此為已走過的岔路口，則可走方向已存在crossroad_top
            if(crossroad_top && (current_x==crossroad_top->x)&&(current_y==crossroad_top->y)){
                for(int i=0; i<8; i++){
                    current_directions[i] = crossroad_top->directions[i];
                }
                path_count = crossroad_top->path_count;

                //若岔路的路皆已走過，則將此岔路從stack中刪除
                if(crossroad_top->path_count){
                    Stack *current_crossroad = crossroad_top;
                    if(crossroad_top->below){
                        crossroad_top = crossroad_top->below;
                    }
                    else{
                        crossroad_top = NULL;
                        no_crossroad = 1;
                    }
                    delete current_crossroad;
                }
            }
            else{ //若未走過，則找出可走的路
                //判斷方向：下 右下 左下
                if(current_y!=end_y){ 
                    if((current_y+1!=previous_y) && !maze_map[current_x][current_y+1]){ //下
                        path_count++;
                        current_directions[1] = 1;
                    }
                    else{
                        current_directions[1] = 0;
                    }
                    if((current_x!=end_x) && !((current_x+1==previous_x) && (current_y+1!=previous_y)) && !maze_map[current_x+1][current_y+1]){ //右下
                        path_count++;
                        current_directions[5] = 1;
                    }
                    else{
                        current_directions[5] = 0;
                    }
                    if((current_x!=0) && !((current_x-1==previous_x) && (current_y+1==previous_y)) && !maze_map[current_x-1][current_y+1]){ //左下
                        path_count++;
                        current_directions[7] = 1;
                    }
                    else{
                        current_directions[7] = 0;
                    }
                }
                else{
                    current_directions[1] = 0;
                    current_directions[5] = 0;
                    current_directions[7] = 0;
                }
            
                //判斷方向：右 左
                if((current_x!=end_x) && (current_x+1!=previous_x) && !maze_map[current_x+1][current_y]){ //右
                    path_count++;
                    current_directions[3] = 1;
                }
                else{
                    current_directions[3] = 0;
                }
                if((current_x!=0) && (current_x-1!=previous_x) && !maze_map[current_x-1][current_y]){ //左
                    path_count++;
                    current_directions[2] = 1;
                }
                else{
                    current_directions[2] = 0;
                }
            
                //判斷方向：上 右上 左上
                if(current_y!=0){
                    if((current_y-1!=previous_y) && !maze_map[current_x][current_y-1]){ //上
                        path_count++;
                        current_directions[0] = 1;
                    }
                    else{
                        current_directions[0] = 0;
                    }
                    if((current_x!=end_x) && !((current_x+1==previous_x) && (current_y-1==previous_y)) && !maze_map[current_x+1][current_y-1]){ //右上
                        path_count++;
                        current_directions[4] = 1;
                    }
                    else{
                        current_directions[4] = 0;
                    }
                    if((current_x!=0) && !((current_x-1==previous_x) && (current_y-1==previous_y)) && !maze_map[current_x-1][current_y-1]){ //左上
                        path_count++;
                        current_directions[6] = 1;
                    }
                    else{
                        current_directions[6] = 0;
                    }
                }
                else{
                    current_directions[0] = 0;
                    current_directions[4] = 0;
                    current_directions[6] = 0;
                }

                //若為岔路，則將此位置存於stack
                if(path_count > 1){
                    Stack *new_crossroad = new Stack;
                    new_crossroad->x = current_x;
                    new_crossroad->y = current_y;
                    new_crossroad->previous_x = previous_x;
                    new_crossroad->previous_y = previous_y;
                    new_crossroad->path_count = path_count;
                    for(int i=0; i<8; i++){
                        new_crossroad->directions[i] = current_directions[i];
                    }

                    new_crossroad->below = crossroad_top;
                    crossroad_top = new_crossroad;
                    no_crossroad = 0;
                }
            }
        }

        void go_back(){ //回到上一個岔路
            current_x = crossroad_top->x;
            current_y = crossroad_top->y;
            previous_x = crossroad_top->previous_x;
            previous_y = crossroad_top->previous_y;
        
            //將通往死路的路從passed_path中移除
            Stack *previous_path;
            while(!((passed_path_top->x==current_x)&&(passed_path_top->y==current_y))){
                previous_path = passed_path_top;
                passed_path_top = passed_path_top->below;
                delete previous_path;
            }
        }

        void move_ahead(){
            //移動
            previous_x = current_x;
            previous_y = current_y;
            if(current_directions[1]){ //下
                current_y++;
                if(path_count>1){ //若為岔路
                    crossroad_top->directions[1] = 0;
                    crossroad_top->path_count--;
                }
            }
            else if(current_directions[5]){ //右下
                current_x++;
                current_y++;
                if(path_count>1){ //若為岔路
                    crossroad_top->directions[5] = 0;
                    crossroad_top->path_count--;
                }
            }
            else if(current_directions[3]){ //右
                current_x++;
                if(path_count>1){ //若為岔路
                    crossroad_top->directions[3] = 0;
                    crossroad_top->path_count--;
                }
            }
            else if(current_directions[7]){ //左下
                current_x--;
                current_y++;
                if(path_count>1){ //若為岔路
                    crossroad_top->directions[7] = 0;
                    crossroad_top->path_count--;
                }
            }
            else if(current_directions[4]){ //右上
                current_x++;
                current_y--;
                if(path_count>1){ //若為岔路
                    crossroad_top->directions[4] = 0;
                    crossroad_top->path_count--;
                }
            }
            else if(current_directions[2]){ //左
                current_x--;
                if(path_count>1){ //若為岔路
                    crossroad_top->directions[2] = 0;
                    crossroad_top->path_count--;
                }
            }
            else if(current_directions[0]){ //上
                current_y--;
                if(path_count>1){ //若為岔路
                    crossroad_top->directions[0] = 0;
                    crossroad_top->path_count--;
                }
            }
            else if(current_directions[6]){ //左上
                current_x--;
                current_y--;
                if(path_count>1){ //若為岔路
                    crossroad_top->directions[6] = 0;
                    crossroad_top->path_count--;
                }
            }

            //將此新位置存入passed_path中
            Stack *new_passed_path = new Stack;
            new_passed_path->x = current_x;
            new_passed_path->y = current_y;
            new_passed_path->below = passed_path_top;
            passed_path_top = new_passed_path;
        }

        void print_solution(){
            cout << "Maze solution: (1=road, 0=wall)" << endl;
            Stack *previous_path;

            //建立一陣列，存取通往終點路徑的各座標
            bool **maze_solution = new bool *[maze_len];
            for(int i=0; i<maze_len; i++){
                maze_solution[i] = new bool[maze_width];
                for(int j=0; j<maze_width; j++){
                    maze_solution[i][j] = 0;
                }
            }

            //將通往終點路徑的各座標存入陣列中
            while(passed_path_top){
                maze_solution[passed_path_top->x][passed_path_top->y] = 1;
                previous_path = passed_path_top;
                passed_path_top = passed_path_top->below;
                delete previous_path;
            }

            //輸出陣列，使使用者知道迷宮解答
            for(int i=0; i<maze_width; i++){
                for(int j=0; j<maze_len; j++){
                    cout << maze_solution[j][i] << ' ';
                }
                cout << endl;
            }
        }

        void print_current(){//testing
            cout << "(" << current_x << ", " << current_y << ")  ";
        }
};

int main(){
    int path_count;
    maze Maze;
    Maze.get_maze_size();
    Maze.create_maze(); //建立迷宮, 並使使用者輸入迷宮樣式

    while(!Maze.at_end()){ //若到達終點：break loop
        Maze.find_road();
        if(Maze.path_count==0){ //若遇死路
            if(Maze.no_crossroad){ //若已經沒有可走的路，則程式結束
                cout << "No solution to this maze." << endl;
                break;
            }
            else{
                //回到上一個岔路（stack top）
                Maze.go_back();
            }           
        }
        else{
            Maze.move_ahead();
        }
        cout << endl;//testing
    }

    if(Maze.at_end()){ //若在終點
        Maze.print_solution(); //告訴使用者往哪裡走
    }
}
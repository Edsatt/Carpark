#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<assert.h>

#define MAX 20
#define ALPHA 26
#define STORE_SIZE 100000

struct car{
  char name;
  int sz, r, c;
  bool exit, orientation; 
};
typedef struct car car;

struct carpark{
  char cp2d[MAX][MAX];
  car cars[ALPHA];
  int x, y, n, f;
};
typedef struct carpark carpark;

struct cp_store{
  carpark store[STORE_SIZE];
  int moves;
};
typedef struct cp_store cp_store;

bool make_carpark(carpark* cp, FILE* file);
bool string_check(carpark* cp, char* str);
void initialise_cp_store(carpark* cp, cp_store* s);
void initialise_carpark(carpark* cp);
bool carpark_check(carpark* cp);
void car_count(carpark* cp);
void car_orientation(carpark* cp);
bool exit_check(carpark* cp);
bool car_size(carpark* cp);
bool consec_check(carpark* cp);
bool car_find(carpark* cp, int car_num);
bool carpark_empty(carpark* cp);
bool solve_carpark(carpark* cp, carpark* temp_cp, cp_store* s, bool print);
void move(carpark* cp, carpark* temp_cp, cp_store* s, int reference);
bool store_carpark(carpark* temp_cp, cp_store* s);
bool carpark_unique(carpark* temp_cp, carpark* cp_store_cp);
void copy_carpark(carpark* cp, carpark* temp_cp);
void turns(cp_store* s, int previous, int end, bool print);
bool car_coordinates(carpark* cp, int car_num);
bool move_car_left(carpark* cp, carpark* temp_cp, int car_num);
bool move_car_right(carpark* cp, carpark* temp_cp, int car_num);
bool move_car_up(carpark* cp, carpark* temp_cp, int car_num);
bool move_car_down(carpark* cp, carpark* temp_cp, int car_num);
void exit_horizontal(carpark* cp, carpark* temp_cp, int car_num);
void exit_vertical(carpark* cp, carpark* temp_cp, int car_num);
void str2cp(carpark* cp, char* str);
void cp2str(carpark* cp, char* str);
void print_carpark(carpark* cp);
void test(void);


int main(int argc, char* argv[])
{
  test();
  carpark cp;
  carpark temp_cp;
  static cp_store s;
  bool print=false;

  initialise_carpark(&cp);
  initialise_carpark(&temp_cp);
  initialise_cp_store(&cp, &s);
  
  if(argc<2){
    fprintf(stderr, "No file entered\n");
    exit(EXIT_FAILURE);
  }
  if(argc>2){ 
    if(strcmp(argv[2], "-show")==0){
      print=true;
    }
  }
  FILE* file = fopen(argv[1], "r");
  if(!file){
    fprintf(stderr, "No file with name %s found in directory\n", argv[1]);
    exit(EXIT_FAILURE);
  }
  if(!make_carpark(&cp, file) || !carpark_check(&cp)){
    fprintf(stderr, "Carpark in %s is invalid\n", argv[1]);
    exit(EXIT_FAILURE);
  }
  if(!solve_carpark(&cp, &temp_cp, &s, print)){
    fprintf(stderr, "No solution to %s\n", argv[1]);
    exit(EXIT_FAILURE);
  }
  printf("%i Moves\n", s.moves);
  fclose(file);
return 0;
}


bool make_carpark(carpark* cp, FILE* file)
{  
  char str[MAX*MAX];
  char row[MAX];
  fgets(row, MAX, file);
  sscanf(row, "%ix%i", &cp->y, &cp->x);

  for(int r=0; r<cp->y; r++){
    fgets(str, cp->x+2, file);
    sscanf(str, "%s", row);
    for(int c=0; c<cp->x; c++){
      if(!string_check(cp, str)){
        return false;
      }
      cp->cp2d[r][c] = row[c];
    }
  }
  return true;
}


bool string_check(carpark* cp, char* str)
{
  for(int i=0; i<cp->x; i++){
    if((str[i]<'A' || str[i]>'Z') && str[i]!='#' && str[i]!='.'){
      return false;
    }
  }
  return true;
}


void initialise_cp_store(carpark* cp, cp_store* s)
{
  for(int i=0; i<STORE_SIZE; i++){
    copy_carpark(cp, &s->store[i]);
  }
}


void initialise_carpark(carpark* cp)
{
  for(int r=0; r<MAX; r++){
    for(int c=0; c<MAX; c++){
      cp->cp2d[r][c]='0';
      cp->x=0;
      cp->y=0;
      cp->n=0;
      cp->f=0;
      for(int i=0; i<cp->n; i++){
        cp->cars[i].name='0';
        cp->cars[i].sz=0;
        cp->cars[i].c=0;
        cp->cars[i].r=0;
        cp->cars[i].orientation=false;
        cp->cars[i].exit=false;
      }
    }
  }
}
 

bool carpark_check(carpark* cp)
{
  car_count(cp);
  car_orientation(cp);
  if(exit_check(cp) && !carpark_empty(cp) && car_size(cp) && consec_check(cp)){
    return true;
  }
  return false;
}


void car_count(carpark* cp)
{
  int cnt='.';
  for(int r=1; r<cp->y-1; r++){
    for(int c=1; c<cp->x-1; c++){
      if(cp->cp2d[r][c]>cnt){
        cnt=cp->cp2d[r][c];
      }
    }
  }
  if(cnt>='A' && cnt<='Z'){  
    cp->n=(cnt-'A'+1);
  }
}
    

void car_orientation(carpark* cp)
{
  int cnt=0;
  while(cnt<cp->n){
    for(int r=0; r<cp->y; r++){
      for(int c=0; c<cp->x; c++){
        if((cp->cp2d[r][c]==('A'+cnt))&&(cp->cp2d[r][c+1]==('A'+cnt))){
          cp->cars[cnt].orientation=false;
        }
        if((cp->cp2d[r][c]==('A'+cnt))&&(cp->cp2d[r+1][c]==('A'+cnt))){
          cp->cars[cnt].orientation=true;
        }
      }
    }
    cp->cars[cnt].name=('A'+cnt);
    cp->cars[cnt].exit=false;
    cnt++;
  }      
}


bool exit_check(carpark* cp)
{
  int cnt=0;
  for(int r=0; r<cp->y; r++){
    for(int c=0; c<cp->x; c++){
      if(cp->cp2d[r][c]=='#'){
        cnt++;
      }
    }
  }
  if(cnt==2*(cp->y+cp->x-2)){
    return false;
  }
  return true;
}


bool car_size(carpark* cp)
{
  int cnt=0, size=1, total_count=0;
  while(cnt<cp->n){
    for(int r=1; r<cp->y-1; r++){
      for(int c=1; c<cp->x-1; c++){
        if(cp->cp2d[r][c]==(cp->cars[cnt].name)){
            total_count++;
          }
        if((cp->cp2d[r][c]==(cp->cars[cnt].name))&&(cp->cp2d[r][c+1]==(cp->cars[cnt].name))){
            size++;
        }
        else if((cp->cp2d[r][c]==(cp->cars[cnt].name))&&(cp->cp2d[r+1][c]==(cp->cars[cnt].name))){
            size++;
        }   
      }
    }
    if((size!=total_count) || (size<2)){
      return false;
    }
    cp->cars[cnt].sz=size, cnt++, size=1, total_count=0;
  }
  return true;    
}


bool consec_check(carpark* cp)
{
  for(int i=0; i<cp->n; i++){
    if(!car_find(cp, i)){
      return false;
    }
  }
  return true;
}


bool car_find(carpark* cp, int car_num)
{
  for(int r=1; r<cp->y-1; r++){
    for(int c=1; c<cp->x-1; c++){
      if(cp->cp2d[r][c]==cp->cars[car_num].name){
        return true;
      }
    }
  }
  return false;
}


bool carpark_empty(carpark* cp)
{
  for(int r=1; r<cp->y-1; r++){
    for(int c=1; c<cp->x-1; c++){
      if(cp->cp2d[r][c]!='.'){
        return false;
      }
    }
  }
  return true;
}


bool solve_carpark(carpark* cp, carpark* temp_cp, cp_store* s, bool print)
{
  int iterations=0;
  if(carpark_check(cp)){
    cp->f=0;
    store_carpark(cp, s);
    for(int f=0; !carpark_empty(&s->store[f]); f++){
      move(&s->store[f], temp_cp, s, f);
      iterations++;
    }
    if(iterations>1 && s->store[iterations].f==0){
      return false;
    }
    else{
      turns(s, s->store[iterations].f, iterations, print);
      return true;
    }
  }
  return false;
}


void move(carpark* cp, carpark* temp_cp, cp_store* s, int reference)
{
  copy_carpark(cp, temp_cp);
  int i=0;
  while(i<cp->n){
    if(cp->cars[i].exit==true){
        i++;
    }
    else{
      if(cp->cars[i].orientation==true){
        move_car_up(cp, temp_cp, i);
        exit_vertical(cp, temp_cp, i);
        temp_cp->f=reference;
        store_carpark(temp_cp, s);
        copy_carpark(cp, temp_cp);

        move_car_down(cp, temp_cp, i);
        exit_vertical(cp, temp_cp, i);
        temp_cp->f=reference;
        store_carpark(temp_cp, s);
        copy_carpark(cp, temp_cp);
      }
      else{
        move_car_left(cp, temp_cp, i);
        exit_horizontal(cp, temp_cp, i);
        temp_cp->f=reference;
        store_carpark(temp_cp, s);
        copy_carpark(cp, temp_cp);

        move_car_right(cp, temp_cp, i);
        exit_horizontal(cp, temp_cp, i);
        temp_cp->f=reference;
        store_carpark(temp_cp, s);
        copy_carpark(cp, temp_cp);
      }
      i++;
    }
  }     
}


bool store_carpark(carpark* temp_cp, cp_store* s)
{
  for(int i=0; i<STORE_SIZE; i++){
    if(!carpark_unique(temp_cp, &s->store[i])){
      return false;
    }
  }
  for(int j=0; j<STORE_SIZE; j++){
    if(s->store[j].cp2d[0][0]=='0'){
      copy_carpark(temp_cp, &s->store[j]);        
      return true;
    }
  }
  return false;
}


bool carpark_unique(carpark* temp_cp, carpark* cp_store_cp)
{
  for(int r=0; r<temp_cp->y; r++){
    for(int c=0; c<temp_cp->x; c++){
      if(temp_cp->cp2d[r][c]!=cp_store_cp->cp2d[r][c]){
        return true;
      }
    }
  }
  return false;
}


void copy_carpark(carpark* cp, carpark* cp_copy)
{
  for(int r=0; r<MAX; r++){
    for(int c=0; c<MAX; c++){
      cp_copy->cp2d[r][c]=cp->cp2d[r][c];
      cp_copy->x=cp->x;
      cp_copy->y=cp->y;
      cp_copy->n=cp->n;
      cp_copy->f=cp->f;
      for(int i=0; i<cp->n; i++){
        cp_copy->cars[i].name=cp->cars[i].name;
        cp_copy->cars[i].sz=cp->cars[i].sz;
        cp_copy->cars[i].orientation=cp->cars[i].orientation;
        cp_copy->cars[i].exit=cp->cars[i].exit;
      }
    }
  }
}


void turns(cp_store* s, int previous, int end, bool print)
{
  int solution[MAX*MAX]={0};
  s->moves=1;
  for(int i=0; previous!=0; i++){
    solution[i+1]=previous;    
    previous=s->store[previous].f;
    s->moves++;
  }
  if(print==true){
    solution[0]=end;
    for(int i=s->moves; i>=0; i--){
      print_carpark(&s->store[solution[i]]);   
    }
  }
}


bool car_coordinates(carpark* cp, int car_num)
{
    for(int r=0; r<cp->y; r++){
      for(int c=0; c<cp->x; c++){
        if(cp->cp2d[r][c]==cp->cars[car_num].name){
          cp->cars[car_num].r=r;
          cp->cars[car_num].c=c;
          return true;
      }   
    }
  }
  return false;
}
 

bool move_car_left(carpark* cp, carpark* temp_cp, int car_num)
{
  car_coordinates(cp, car_num);
  int r=cp->cars[car_num].r;
  int c=cp->cars[car_num].c;
  if((cp->cp2d[r][c]==(cp->cars[car_num].name))&&(cp->cp2d[r][c-1]=='.')){
    temp_cp->cp2d[r][c-1]=cp->cars[car_num].name;
    temp_cp->cp2d[r][(c-1)+cp->cars[car_num].sz]='.';
    return true;
  }
  return false;
}


bool move_car_right(carpark* cp, carpark* temp_cp, int car_num)
{
  car_coordinates(cp, car_num);
  int r=cp->cars[car_num].r;
  int c=cp->cars[car_num].c;
  int sz=cp->cars[car_num].sz;
  if((cp->cp2d[r][c]==(cp->cars[car_num].name))&&(cp->cp2d[r][c+sz]=='.')){
    temp_cp->cp2d[r][c+sz]=cp->cars[car_num].name;
    temp_cp->cp2d[r][c]='.';
    return true;
  }
  return false;
}


bool move_car_up(carpark* cp, carpark* temp_cp, int car_num)
{
  car_coordinates(cp, car_num);
  int r=cp->cars[car_num].r;
  int c=cp->cars[car_num].c;
  if((cp->cp2d[r][c]==(cp->cars[car_num].name))&&(cp->cp2d[r-1][c]=='.')){
    temp_cp->cp2d[r-1][c]=cp->cars[car_num].name;
    temp_cp->cp2d[(r-1)+cp->cars[car_num].sz][c]='.';
    return true;
  }
  return false;
}


bool move_car_down(carpark* cp, carpark* temp_cp, int car_num)
{
  car_coordinates(cp, car_num);
  int r=cp->cars[car_num].r;
  int c=cp->cars[car_num].c;
  int sz=cp->cars[car_num].sz;
  if((cp->cp2d[r][c]==(cp->cars[car_num].name))&&(cp->cp2d[r+sz][c]=='.')){
    temp_cp->cp2d[r+sz][c]=cp->cars[car_num].name;
    temp_cp->cp2d[r][c]='.';
    return true;
  }
  return false;
}


void exit_horizontal(carpark* cp, carpark* temp_cp, int car_num)
{
  car_coordinates(temp_cp, car_num);
  int r=temp_cp->cars[car_num].r;
  int c=temp_cp->cars[car_num].c;
  int sz=cp->cars[car_num].sz;
  char name=cp->cars[car_num].name;
  if(((temp_cp->cp2d[r][c]==name) && (c==0)) || ((temp_cp->cp2d[r][(c-1)+sz]==name) && ((c-1)+sz==cp->x-1))){
    for(int y=0; y<cp->y; y++){
      for(int x=0; x<cp->x; x++){
        if(temp_cp->cp2d[y][x]==name){
          temp_cp->cp2d[y][x]='.';
        }
      }
    }
    cp->cars[car_num].exit=true;
  }    
}


void exit_vertical(carpark* cp, carpark* temp_cp, int car_num)
{
  car_coordinates(temp_cp, car_num);
  int r=temp_cp->cars[car_num].r;
  int c=temp_cp->cars[car_num].c;
  int sz=cp->cars[car_num].sz;
  char name=cp->cars[car_num].name;
  if(((temp_cp->cp2d[r][c]==name) && (r==0)) || ((temp_cp->cp2d[r-1+sz][c]==name) && ((r-1)+sz==cp->y-1))){
    for(int y=0; y<cp->y; y++){
      for(int x=0; x<cp->x; x++){
        if(temp_cp->cp2d[y][x]==cp->cars[car_num].name){
          temp_cp->cp2d[y][x]='.';
        }
      }
    }
    cp->cars[car_num].exit=true;
  }
}


void str2cp(carpark* cp, char* str)
{
  int x=0;
  for(int r=0; r<cp->y; r++){
    for(int c=0; c<cp->x; c++){
      cp->cp2d[r][c]=str[x];
      x++;
    }
  }
}


void cp2str(carpark* cp, char* str)
{
  int x=0;
  for(int r=0; r<cp->y; r++){
    for(int c=0; c<cp->x; c++){
      str[x]=cp->cp2d[r][c];
      x++;
    }
  }
  for(int i=0; i<=cp->x*cp->y; i++){
    if(i==(cp->x*cp->y)){
      str[i]='\0';
    }
  }
}


void print_carpark(carpark* cp)
{
  for(int r=0; r<cp->y; r++){
    for(int c=0; c<cp->x; c++){
      printf("%c", cp->cp2d[r][c]);
    }
    printf("\n");
  }
  printf("\n");
}


void test(void)
{
  carpark cp;
  carpark temp_cp;
  static cp_store s;
  char str[MAX*MAX];
  char copy_str[MAX*MAX];
  
  initialise_carpark(&cp);
  initialise_carpark(&temp_cp);

  //tests that cp_store initialises correctly
  initialise_cp_store(&cp, &s);
  assert(s.store[0].cp2d[0][0]=='0');
  assert(s.store[50000].cp2d[0][0]=='0');

  //tests string_check function
  cp.x=6, cp.y=6;
  assert(string_check(&cp, "#.ABZ."));
  assert(!string_check(&cp, "@#.ABZ"));
  assert(!string_check(&cp, "      "));

  //tests car count function
  cp.x=6, cp.y=6;
  str2cp(&cp, "#.####.AAAB##CCCB##DDDB##EEEB#######");
  car_count(&cp);
  assert(cp.n==5);
  cp.x=6, cp.y=6;
  str2cp(&cp, "#.####.AAA.##....##....##....#######");
  car_count(&cp);
  assert(cp.n==1);

  //tests car_orientation function
  cp.x=6, cp.y=6;
  str2cp(&cp, "#.####..AA.##..B.##..B.##..B.#######");
  car_count(&cp);
  car_orientation(&cp);
  assert(cp.cars[0].orientation==false);
  assert(cp.cars[0].name=='A');
  assert(cp.cars[0].exit==false);
  assert(cp.cars[1].orientation==true);
  assert(cp.cars[1].name=='B');
  assert(cp.cars[1].exit==false);

  //tests exit_check function
  cp.x=6, cp.y=6;
  str2cp(&cp, "#.####.AAA.##B...##B...##B...#######");
  assert(exit_check(&cp));
  str2cp(&cp, "#######AAA.##B...##B...##B...#######");
  assert(!exit_check(&cp));

  //tests car size function with every cell of grid
  cp.x=6, cp.y=6;
  str2cp(&cp, "#.####.AAAB##CCCB##DDDB##EEEB#######");
  car_count(&cp);
  car_orientation(&cp);
  assert(car_size(&cp));
  assert(cp.cars[0].sz==3);
  assert(cp.cars[1].sz==4);
  assert(cp.cars[2].sz==3);
  assert(cp.cars[3].sz==3);
  assert(cp.cars[4].sz==3);

  //tests car size function with an invalid car shape
  cp.x=6, cp.y=6;
  str2cp(&cp, "#.####.AAA.##.A..##.BBB##....#######");
  car_count(&cp);
  car_orientation(&cp);
  assert(!car_size(&cp));

  //tests car size function with multiple versions of a car
  cp.x=6, cp.y=6;
  str2cp(&cp, "#.####..AA.##....##.BBB##..AA#######");
  car_count(&cp);
  car_orientation(&cp);
  assert(!car_size(&cp));

  //tests car size function with a car smaller than 2 characters
  cp.x=6, cp.y=6;
  str2cp(&cp, "#.####..A..##....##.BBB##..AA#######");
  car_count(&cp);
  car_orientation(&cp);
  assert(!car_size(&cp));

  //tests consec_check function
  cp.x=6, cp.y=6;
  str2cp(&cp, "#.####.....##....##.BBB##..AA#######");
  car_count(&cp);
  car_orientation(&cp);
  assert(consec_check(&cp));
  str2cp(&cp, "#.####.....##....##.BBB##....#######");
  car_count(&cp);
  car_orientation(&cp);
  assert(!consec_check(&cp));
  str2cp(&cp, "#.####.AAA.##....##CCC.##....#######");
  car_count(&cp);
  car_orientation(&cp);
  assert(!consec_check(&cp));
  
  //tests car_find function
  cp.x=6, cp.y=6;
  str2cp(&cp, "#.####.....##....##.BBB##..AA#######");
  car_count(&cp);
  car_orientation(&cp);
  assert(car_find(&cp, 0));
  assert(car_find(&cp, 1));
  str2cp(&cp, "#.####.....##....##.BBB##....#######");
  car_count(&cp);
  car_orientation(&cp);
  assert(!car_find(&cp, 0));

  //tests carpark empty function
  cp.x=6, cp.y=6;
  str2cp(&cp, "#######....##....##....##....#######");
  assert(carpark_empty(&cp));
  cp.x=6, cp.y=6;
  str2cp(&cp, "#######..A.##..A.##..A.##.BB.#######");
  assert(!carpark_empty(&cp));
  
  //tests store_carpark function
  cp.x=6, cp.y=6;
  str2cp(&cp, "#.####.AAA.##B...##B...##B...#######");
  store_carpark(&cp, &s);
  cp.x=6, cp.y=6;
  str2cp(&cp, "#.####.AAA.##B...##B...##B...#######");
  store_carpark(&cp, &s);
  cp.x=6, cp.y=6;
  str2cp(&cp, "#.####.AA..##B...##B...##B...#######");
  store_carpark(&cp, &s);
  cp2str(&s.store[0], str);
  assert(strcmp(str, "#.####.AAA.##B...##B...##B...#######")==0);
  cp2str(&s.store[1], str);
  assert(strcmp(str, "#.####.AA..##B...##B...##B...#######")==0);

  //tests carpark_unique function
  cp.x=6, cp.y=6;
  temp_cp.x=6, temp_cp.y=6;
  str2cp(&cp, "#.####..AA.##..B.##..B.##..B.#######");
  str2cp(&temp_cp, "#.####..AA.##..B.##..B.##..B.#######");
  assert(!carpark_unique(&cp, &temp_cp));
  temp_cp.x=6, temp_cp.y=6;
  str2cp(&temp_cp, "#.####.AA..##..B.##..B.##..B.#######");
  assert(carpark_unique(&cp, &temp_cp));
 
  //tests copy_carpark function
  cp.x=6, cp.y=6;
  str2cp(&cp, "#.####.AAA.##B...##B...##B...#######");
  copy_carpark(&cp, &temp_cp);
  cp2str(&cp, str);
  cp2str(&temp_cp, copy_str);
  assert(strcmp(str, copy_str)==0);

  //tests car_coordinates function
  cp.x=6, cp.y=6;
  str2cp(&cp, "#.####..AA.##B...##B...##B...#######");
  car_count(&cp);
  assert(car_coordinates(&cp, 0));
  assert(cp.cars[0].r==1);
  assert(cp.cars[0].c==2);
  assert(car_coordinates(&cp, 1));
  assert(cp.cars[1].r==2);
  assert(cp.cars[1].c==1);
  str2cp(&cp, "#.####.....##B...##B...##B...#######");
  car_count(&cp);
  assert(!car_coordinates(&cp, 0));
  assert(car_coordinates(&cp, 1));
  assert(cp.cars[1].r==2);
  assert(cp.cars[1].c==1);

  //tests move_car_left function
  cp.x=6, cp.y=6;
  str2cp(&cp, "#######.AA.##.BB.##.CC.##.DD.#######");
  car_count(&cp);
  car_size(&cp);
  copy_carpark(&cp, &temp_cp);
  move_car_left(&cp, &temp_cp, 0);
  move_car_left(&cp, &temp_cp, 1);
  move_car_left(&cp, &temp_cp, 2);
  move_car_left(&cp, &temp_cp, 3);
  cp2str(&temp_cp, str);
  assert(strcmp(str, "#######AA..##BB..##CC..##DD..#######")==0);

  //tests move_car_right function
  cp.x=6, cp.y=6;
  str2cp(&cp, "#######.AA.##.BB.##.CC.##.DD.#######");
  car_count(&cp);
  car_size(&cp);
  copy_carpark(&cp, &temp_cp);
  move_car_right(&cp, &temp_cp, 0);
  move_car_right(&cp, &temp_cp, 1);
  move_car_right(&cp, &temp_cp, 2);
  move_car_right(&cp, &temp_cp, 3);
  cp2str(&temp_cp, str);
  assert(strcmp(str, "#######..AA##..BB##..CC##..DD#######")==0);

  //tests move_car_up function
  cp.x=6, cp.y=6;
  str2cp(&cp, "#######....##ABCD##ABCD##....#######");
  car_count(&cp);
  car_size(&cp);
  copy_carpark(&cp, &temp_cp);
  move_car_up(&cp, &temp_cp, 0);
  move_car_up(&cp, &temp_cp, 1);
  move_car_up(&cp, &temp_cp, 2);
  move_car_up(&cp, &temp_cp, 3);
  cp2str(&temp_cp, str);
  assert(strcmp(str, "#######ABCD##ABCD##....##....#######")==0);

  //tests move_car_down function
  cp.x=6, cp.y=6;
  str2cp(&cp, "#######....##ABCD##ABCD##....#######");
  car_count(&cp);
  car_size(&cp);
  copy_carpark(&cp, &temp_cp);
  move_car_down(&cp, &temp_cp, 0);
  move_car_down(&cp, &temp_cp, 1);
  move_car_down(&cp, &temp_cp, 2);
  move_car_down(&cp, &temp_cp, 3);
  cp2str(&temp_cp, str);
  assert(strcmp(str, "#######....##....##ABCD##ABCD#######")==0);

  //tests exit_horizontal function
  cp.x=6, cp.y=6;
  str2cp(&cp, "#.####.AAA.##.BBB.#....##....#######");
  temp_cp.x=6, temp_cp.y=6;
  car_count(&cp);
  car_size(&cp);
  copy_carpark(&cp, &temp_cp);
  str2cp(&temp_cp, "#.####AAA..##..BBB#....##....#######");
  exit_horizontal(&cp, &temp_cp, 0);
  exit_horizontal(&cp, &temp_cp, 1);
  cp2str(&temp_cp, str);
  assert(strcmp(str, "#.####.....##.....#....##....#######")==0); 

  //tests exit_vertical function
  cp.x=6, cp.y=6;
  str2cp(&cp, "#.#####A...##A..B##A..B##...B#####.#");
  temp_cp.x=6, temp_cp.y=6;
  car_count(&cp);
  car_size(&cp);
  copy_carpark(&cp, &temp_cp);
  str2cp(&temp_cp, "#A#####A...##A...##...B##...B#####B#");
  exit_vertical(&cp, &temp_cp, 0);
  exit_vertical(&cp, &temp_cp, 1);
  cp2str(&temp_cp, str);
  assert(strcmp(str, "#.#####....##....##....##....#####.#")==0);


  //4x4_1c_1t
  initialise_carpark(&cp);
  initialise_cp_store(&cp, &s);
  cp.x=4, cp.y=4;
  str2cp(&cp, "#####AA.#..#####");
  assert(solve_carpark(&cp, &temp_cp, &s, false));
  assert(s.moves==1);
  
  //6x6_2c_3t
  initialise_carpark(&cp);
  initialise_cp_store(&cp, &s);
  cp.x=6, cp.y=6;
  str2cp(&cp, "#.####.AAA.##B...##B...##B...#######");
  assert(solve_carpark(&cp, &temp_cp, &s, false));
  assert(s.moves==3);

  //7x7_4c_11t
  initialise_carpark(&cp);
  initialise_cp_store(&cp, &s);
  cp.x=7, cp.y=7;
  str2cp(&cp, "#.######BBBC..#A..C.##A....##A....#.ADDDD#####.##");
  assert(solve_carpark(&cp, &temp_cp, &s, false));
  assert(s.moves==11);

  //no cars
  initialise_carpark(&cp);
  initialise_cp_store(&cp, &s);
  cp.x=6, cp.y=6;
  str2cp(&cp, "#######....##....##....##....#######");
  assert(!solve_carpark(&cp, &temp_cp, &s, false));

  //wrong shape
  initialise_carpark(&cp);
  initialise_cp_store(&cp, &s);
  cp.x=6, cp.y=6;
  str2cp(&cp, "#######A..B##A..B##AA.B##....#.#####");
  assert(!solve_carpark(&cp, &temp_cp, &s, false));
}
  














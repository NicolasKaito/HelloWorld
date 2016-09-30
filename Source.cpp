#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;
// VERSION WORKING WITH git

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
 
 
 struct Dist_impact {
     int dist;
     int impact; // 0 means the object pointed won't affect the priority
     
     Dist_impact()
     {
        dist = 0;
        impact = 0;
     }
     
     void setDI(int distf, int impactf)
     {
         dist = distf;
         impact = impactf;
     }
     
     Dist_impact getDI()
     {
      Dist_impact result;
      result.dist = dist;
      result.impact = impact;
      return result;
     }
 };
 
const int simpleBox = 0;
const int rangeBox = 1;
const int bombBox = 2;
const int stone = 3;
const int empty = 4;
const int rangeItem = 5;
const int bombItem = 6;
const int bomb = 7;

struct Case {
     int priority;
     int type;  //  0 = box  1 = box item range  2 = box item extra Bomb   3 = stone 4 = empty  5 = item range  6 = item bomb  7 = bomb
     int cooldown;  // time before a case would really appear on the map
     int reachable;  // 0 = false  1 = True
     int danger;  // 0 = non danger   1 = dangerous
     Dist_impact matrixObjects [4];  // indice 0 = left, 1 = up, 2 = right, 3 = down
     int matrixDanger [4];   // effect on danger on this case when bomb put
     
     Case()
     {
         priority = 0;
         type = 0;
         cooldown = 0;
         reachable = 0;
         danger = 0;
         matrixObjects[0] = Dist_impact();
         matrixObjects[1] = Dist_impact();
         matrixObjects[2] = Dist_impact();
         matrixObjects[3] = Dist_impact();
         matrixDanger[0] = 0;
         matrixDanger[1] = 0;
         matrixDanger[2] = 0;
         matrixDanger[3] = 0;
     }
 };
 
typedef Case TabPrio [13][11];

struct Coord {
    int x;
    int y;
    
    Coord()
    {
     x = 0;
     y = 0;
    }
    
     void setCoord(int xx, int yy)
    {
        x = xx;
        y = yy;
    }
};

struct Map {
    int myId;
    int height;
    int width;
    int myBombRange;
    int itemCounterRange;
    int itemCounterBomb;
    int targetType; // 0 = item  1 = place to put a bomb
    Coord myPos;
    Coord hisPos;
    Coord targetPos;   
    TabPrio priorityTable;
    
    Map()
    {
        myId = 0;
        height = 11;
        width = 13;
        myBombRange = 3;
        itemCounterRange = 3;
        itemCounterBomb = 3;   
        targetType = 1;
        myPos = Coord();
        hisPos = Coord();
        targetPos = Coord();
        TabPrio priorityTable = {{Case()}};
    } // end constructor
};
 
 
void initializeMap(Map *map, int myId, int height, int width)
{
    (*map).myId = myId;
    (*map).height = height;
    (*map).width = width;
    if (myId == 0)
    {
        (*map).myPos.setCoord(0, 0);
    } else {
        (*map).myPos.setCoord(width-1, height-1);
    }
    (*map).targetPos.setCoord(6, 5);
}


int caseReachable(Map *map, int x, int y)
{
    int curReachable = 0;
    TabPrio *tableOfPriority = &((*map).priorityTable);
    
    if ( ((*tableOfPriority)[x][y].type < empty) || ((*tableOfPriority)[x][y].cooldown > 0) ) 
    {
        return 0;
    } else {
        // look around
        if ( x > 0) curReachable += (*tableOfPriority)[x-1][y].reachable;
        if ( x < 12 ) curReachable += (*tableOfPriority)[x+1][y].reachable;
        if ( y > 0 ) curReachable += (*tableOfPriority)[x][y-1].reachable;
        if ( y < 10 ) curReachable += (*tableOfPriority)[x][y+1].reachable;
        if (curReachable > 0)
        {
            return 1;
        } else {
            return 0;
        }
    } // end if
} // end caseReachable

 

void actualizeTableReachable(Map *map)
{
    int offsetX;
    int offsetY;
    TabPrio *tableOfPriority = &((*map).priorityTable);
    // resetTableReachable(&(*tableOfPriority));
    // corners are reachable
    if ((*map).myId == 0)
    {
        (*tableOfPriority)[0][0].reachable = 1;
        offsetX = 0;
        offsetY = 0;
    } else {
        (*tableOfPriority)[12][10].reachable = 1;
        offsetX = -12;
        offsetY = -10;
    }
    
    for  (int j = offsetY; j < (11+offsetY); j++)
    {
        for (int i = offsetX; i < (13+offsetX); i++)
        {   
            if ( (*tableOfPriority)[abs(i)][abs(j)].reachable != 1 )
            {
                (*tableOfPriority)[abs(i)][abs(j)].reachable = caseReachable(&(*map), abs(i), abs(j)); 
            }
        } // end for
    } // end for
    
    for  (int j = -10-offsetY; j < offsetY; j++)
    {
        for (int i = -12-offsetX; i < offsetX; i++)
        {   
            if ( (*tableOfPriority)[abs(i)][abs(j)].reachable != 1 )
            {
                (*tableOfPriority)[abs(i)][abs(j)].reachable = caseReachable(&(*map), abs(i), abs(j)); 
            }
        } // end for
    } // end for
} // end actualizeTableReachable


int calculateCasePriority(Map *map, int x, int y )
{
    // cerr << "Calculating priority of case : " << x << " , " << y << endl;
    
    int casePriority = 0;
    Case *pCurCase = &((*map).priorityTable[x][y]);
    int curType = (*pCurCase).type;
    if ( (curType < empty) || ((*pCurCase).reachable == 0) || ((*pCurCase).cooldown > 4) ) // case is a box or a Stone so can't go there or an item still in the box who will explose
    {
        return 0;
    } else {  // case is either empty or an item
        for (int i = 0; i < 4; i++)
        {
            Dist_impact curDI = (*pCurCase).matrixObjects[i];
            if ( (curDI.dist > 0) && (curDI.dist < (*map).myBombRange) && (curDI.impact == 1) )
            {    
                casePriority = casePriority + 1;
            }
        } // end for
        if ( curType == rangeItem ) // item range out of box
        {
            return max(casePriority, (*map).itemCounterRange);
        } else if ( curType == bombItem) // item bomb out of box
        {
            return max(casePriority, (*map).itemCounterBomb);
        } else {  // empty case or 
            //cerr << "case prio : " << casePriority << endl;
            return casePriority;
        } // end if
    }// end if
} // end calculateCasePriority


void computePriority(Map *map)
{
    actualizeTableReachable(&(*map));
    for (int j = 0; j < (*map).height; j++) 
    {
        for(int i = 0; i < (*map).width; i++) 
        {   
            if ( (*map).priorityTable[i][j].reachable == 1 )
            {
                (*map).priorityTable[i][j].priority = calculateCasePriority(&(*map), i, j);
            }
        } // end for
    } // end for    
} // end computePriority

 void decreaseCooldown(Map *map)
 {
    TabPrio *tableOfPriority = &((*map).priorityTable);
    for  (int j = 0; j < (*map).height; j++)
    {
        for (int i = 0; i < (*map).width; i++)
        {
            if ( (*tableOfPriority)[i][j].cooldown > 0 )
            {
                (*tableOfPriority)[i][j].cooldown -= 1;
                if ( ((*tableOfPriority)[i][j].type == bomb) && ((*tableOfPriority)[i][j].cooldown == 0) )
                {
                    (*tableOfPriority)[i][j].type = empty;
                }
                
            }
        } // end for
    } // end for
 } // end decreaseCooldown 
 
 
 

void actualiseMatrixProxObjects(Map *map, int x, int y)
{
    int curImpact = 0;
    TabPrio *tableOfPriority = &((*map).priorityTable);
    
    if ( (*tableOfPriority)[x][y].type < stone)  // box 
    { 
        curImpact = 1;
    }
    
    // Left 
    int i = 1;
    if (x > 0)
    {
        while ( ((x-i) > 0) && ((*tableOfPriority)[x-i][y].type == empty) && ((*tableOfPriority)[x-i][y].cooldown == 0) )
        {      
            (*tableOfPriority)[x-i][y].matrixObjects[2].setDI(i, curImpact); // Right dist_impact actualized
            i++;
        } // end while
        (*tableOfPriority)[x-i][y].matrixObjects[2].setDI(i, curImpact);
    } // end if 
    
    // Up
    i = 1;
    if (y > 0)
    {
        while ( ((y-i) > 0) && ((*tableOfPriority)[x][y-i].type == empty) && ((*tableOfPriority)[x][y-i].cooldown == 0) )
        {
            (*tableOfPriority)[x][y-i].matrixObjects[3].setDI(i, curImpact); 
            i++;
        } // end while
        (*tableOfPriority)[x][y-i].matrixObjects[3].setDI(i, curImpact);  
    } // end if
    
    // Right
    i = 1;
    if (x < ((*map).width)-1)
    {
        while ( ((x+i) < 12) && ((*tableOfPriority)[x+i][y].type == empty) && ((*tableOfPriority)[x+i][y].cooldown == 0) )
        {
            (*tableOfPriority)[x+i][y].matrixObjects[0].setDI(i, curImpact);  
            i++;
        } // end while
        (*tableOfPriority)[x+i][y].matrixObjects[0].setDI(i, curImpact); 
    } // end if
    
    // Down
    i = 1;
    if (y < ((*map).height)-1)
    {
        while ( ((y+i) < 10) && ((*tableOfPriority)[x][y+i].type == empty) && ((*tableOfPriority)[x][y+i].cooldown == 0) )
        {
            (*tableOfPriority)[x][y+i].matrixObjects[1].setDI(i, curImpact); 
            i++;
        } // end while
        (*tableOfPriority)[x][y+i].matrixObjects[1].setDI(i, curImpact);
    } // end if   
} // end actualiseMatrixProxObjects


 void resetMatrixObjects(Map *map)
 {
    for (int j = 0; j < (*map).height; j++) 
    {
        for(int i = 0; i < (*map).width; i++) 
        {
            (*map).priorityTable[i][j].matrixObjects[0].setDI(0, 0);
            (*map).priorityTable[i][j].matrixObjects[1].setDI(0, 0);
            (*map).priorityTable[i][j].matrixObjects[2].setDI(0, 0);
            (*map).priorityTable[i][j].matrixObjects[3].setDI(0, 0);
        }
    }
 } // end reset
 
 
void actualizeTablePriority(Map *map)
{
    bool mapEmpty = true;
    resetMatrixObjects(&(*map));
    for (int j = 0; j < (*map).height; j++) 
    {
        for(int i = 0; i < (*map).width; i++) 
        {
            // if object, then actualize prioritytable
            if ( (*map).priorityTable[i][j].type != empty ) 
            {
                actualiseMatrixProxObjects(&(*map), i, j);
                if ( ((*map).priorityTable[i][j].type != stone) && ((*map).priorityTable[i][j].type != bomb) )
                {
                    mapEmpty = false;
                }
            } // end if
        } // end for
    } // end for
    // the matrix is filled with the distances to the next object in the 4 directions of each case
    computePriority(&(*map));
    if (mapEmpty)
    {
        (*map).priorityTable[(*map).hisPos.x][(*map).hisPos.y].priority = 10;  // considerate enemi as a box
        cerr << endl;
        cerr << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
        cerr << "@@ FRENZY MODE, WAZAAAAAAAAAAAAAAAAAAAA @@" << endl;
        cerr << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl << endl;
    }

} // end actualizeTablePriority


void setDangerTable(Map *map, int x, int y, int bombRange)
 {
     TabPrio *tableOfPriority = &((*map).priorityTable);
     Case *pCurCase = &((*tableOfPriority)[x][y]);
     
     int leftRange = (*pCurCase).matrixObjects[0].dist;
     int upRange = (*pCurCase).matrixObjects[1].dist;
     int rightRange = (*pCurCase).matrixObjects[2].dist;
     int downRange = (*pCurCase).matrixObjects[3].dist;
     if ( leftRange == 0 ) 
     {
         leftRange = bombRange;
     } else {
         leftRange = min(bombRange, leftRange);
     }
     if ( upRange == 0 ) 
     {
         upRange = bombRange;
     } else {
         upRange = min(bombRange, upRange);
     }
     if ( rightRange == 0 ) 
     {
         rightRange = bombRange;
     } else {
         rightRange = min(bombRange, rightRange);
     }
     if ( downRange == 0 ) 
     {
         downRange = bombRange;
     } else {
         downRange = min(bombRange, downRange);
     }
     (*pCurCase).matrixDanger[0] = leftRange;
     (*pCurCase).matrixDanger[1] = upRange;
     (*pCurCase).matrixDanger[2] = rightRange;
     (*pCurCase).matrixDanger[3] = downRange;
         
         
     for (int i = 0; i < leftRange; i++)
     {
        if ( (x-i) >= 0 ) (*tableOfPriority)[x-i][y].danger += 1;
     }
     for (int i = 0; i < rightRange; i++)
     {
        if ( (x+i) < 13 ) (*tableOfPriority)[x+i][y].danger += 1;
     }
     for (int i = 0; i < upRange; i++)
     {
        if ( (y-i) >= 0 ) (*tableOfPriority)[x][y-i].danger += 1;
     }
     for (int i = 0; i < downRange; i++)
     {
        if ( (y+i) < 11 ) (*tableOfPriority)[x][y+i].danger += 1;
     }
 } // end setDangerTable
 
  void cleanDangerTable(Map *map, int x, int y)
 {
     TabPrio *tableOfPriority = &((*map).priorityTable);
     Case *pCurCase = &((*tableOfPriority)[x][y]);
     
     int leftRange = (*pCurCase).matrixDanger[0];
     int upRange = (*pCurCase).matrixDanger[1];
     int rightRange = (*pCurCase).matrixDanger[2];
     int downRange = (*pCurCase).matrixDanger[3];
     
     for (int i = 0; i < leftRange; i++)
     {
        if ( ((x-i) >= 0) && ((*tableOfPriority)[x-i][y].danger > 0) ) (*tableOfPriority)[x-i][y].danger -= 1;
     }
     for (int i = 0; i < rightRange; i++)
     {
        if ( ((x+i) < 13) && ((*tableOfPriority)[x+i][y].danger > 0) ) (*tableOfPriority)[x+i][y].danger -= 1;
     }
     for (int i = 0; i < upRange; i++)
     {
        if ( ((y-i) >= 0) && ((*tableOfPriority)[x][y-i].danger > 0) ) (*tableOfPriority)[x][y-i].danger -= 1;
     }
     for (int i = 0; i < downRange; i++)
     {
        if ( ((y+i) < 11) && ((*tableOfPriority)[x][y+i].danger > 0) ) (*tableOfPriority)[x][y+i].danger -= 1;
     }
 } // end cleanDangerTable
 


void objectDestroyedEffect(Map *map, int x, int y)
{
    cerr << "Object destroyed effect called on : " << x << " , " << y << endl;
    Case *pCurCase = &((*map).priorityTable[x][y]);
    
    if ( (*pCurCase).type != empty ) // security check, objectDestroyedEffect should always be called upon an object
    {
        if ( (*pCurCase).type == stone )
        {
            //cerr << "can't explose stone" << endl;
        } else if ( (*pCurCase).type < stone )
        {
            //cerr << "BOX EXPLODED" << endl;
            (*pCurCase).cooldown = 8;
            (*pCurCase).type += 4;
        } else if ( (*pCurCase).type == bomb ) 
        {
            cleanDangerTable(&(*map), x, y);
            (*pCurCase).type = empty;
        } else {
            //cerr << " ITEM EXPLODED !" << endl;
            (*pCurCase).type = empty;
        } // end if
        
    } else {
        cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
        cerr << "!!!!!!!!!!!!! ObjectDestroyed called on an empty cell !!!!!!!!!!!!!!!!" << endl;
        cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    } // end if  
}// end objectDestroyedEffect

 
 void bombEffect(Map *map, int x, int y, int bombRange)
 {
    // cerr << "Bomb effect called on : " << x << " , " << y << endl;
    Case *pCurCase = &((*map).priorityTable[x][y]);
    int left = (*pCurCase).matrixObjects[0].dist;
    int up = (*pCurCase).matrixObjects[1].dist;
    int right = (*pCurCase).matrixObjects[2].dist;
    int down = (*pCurCase).matrixObjects[3].dist;

    // left
    if ( left > 0 && (left < bombRange ) )
    {
        objectDestroyedEffect(&(*map), x-left, y);
    }
    
    // up
    if ( (up > 0) && (up < bombRange ) ) // an object up from the bomb will be destroyed
    {
        objectDestroyedEffect(&(*map), x, y-up);
    }
    
    // right
    if ( right > 0 && (right < bombRange ) )
    {
        objectDestroyedEffect(&(*map), x+right, y);
    }
    
    // down
    if ( down > 0 && (down < bombRange ) )
    {
        objectDestroyedEffect(&(*map), x, y+down);
    }    
 } // end bombEffect
 
 
 
 
 // find closest case reachable and safe
 void dodge(Map *map, int x, int y)
 {
     bool safe = false;
     int scope = 1;
     TabPrio *tableOfPriority = &((*map).priorityTable);
     (*map).targetType = 0; // doging means target is not to put a bomb on
     while (!safe)
     {
        for (int i = -scope; i <= scope; i++)
        {
            if ( ((x+i) >= 0) && ((x+i) < 13) && ((y+scope) < 11) && ((*tableOfPriority)[x+i][y+scope].danger == 0) && ((*tableOfPriority)[x+i][y+scope].reachable == 1) )
            {
                safe = true;
                (*map).targetPos.setCoord(x+i, y+scope);
                return;
            }
            if ( ((x+i) >= 0) && ((x+i) < 13) && ((y-scope) >= 0) && ((*tableOfPriority)[x+i][y-scope].danger == 0) && ((*tableOfPriority)[x+i][y-scope].reachable == 1) )
            {
                safe = true;
                (*map).targetPos.setCoord(x+i, y-scope);
                return;
            }
            if ( ((y+i) >= 0) && ((y+i) < 11) && ((x+scope) < 13) && ((*tableOfPriority)[x+scope][y+i].danger == 0) && ((*tableOfPriority)[x+scope][y+i].reachable == 1) )
            {
                safe = true;
                (*map).targetPos.setCoord(x+scope, y+i);
                return;
            }
            if ( ((y+i) >= 0) && ((y+i) < 11) && ((x-scope) >= 0) && ((*tableOfPriority)[x-scope][y+i].danger == 0) && ((*tableOfPriority)[x-scope][y+i].reachable == 1) )
            {
                safe = true;
                (*map).targetPos.setCoord(x-scope, y+i);
                return;
            }
        } // end for
        scope++;
     } // end while
 } // end dodge
 
void checkDangerPath(Map *map, int x, int y)
{
    if ( (*map).priorityTable[x][y].danger > 0 )
    {
        cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
        cerr << "!!!!!!!!   I AM IN DANGER  !!!!!!!!" << endl;
        cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
        dodge(&(*map), x, y);
    } else {
        if ( ((x-1) > 0) && ((*map).priorityTable[x-1][y].danger > 0) ) 
        {
            cerr << " There is danger around " << endl;
            (*map).targetPos.setCoord(x, y);  // do not move
            (*map).targetType = 0;
        }
        if ( ((y-1) > 0) && ((*map).priorityTable[x][y-1].danger > 0) ) 
        {
            (*map).targetPos.setCoord(x, y);
            (*map).targetType = 0;
        }
        if ( ((x+1) < (*map).width) && ((*map).priorityTable[x+1][y].danger > 0) ) 
        {
            (*map).targetPos.setCoord(x, y);
            (*map).targetType = 0;
        }
        if ( ((y+1) < (*map).height) && ((*map).priorityTable[x][y+1].danger > 0) ) 
        {
            (*map).targetPos.setCoord(x, y);
            (*map).targetType = 0;
        }
    }
} // end checkDangerPath


 
 void determinateTarget(Map *map)
 {
    int cur_prio_dist = 0;
    int maxPrio_dist = 0;
    int distance = 0;
    int ponderPrio = 100;
    int ponderDist = 26;
    
    int curX = 0;
    int curY = 0;
    
    for  (int j = 0; j < (*map).height; j++)
    {
        for (int i = 0; i < (*map).width; i++)
        {
            cur_prio_dist = ((*map).priorityTable[i][j].priority);
            if ( cur_prio_dist > 0 )
            {
                cur_prio_dist += 10;
                distance = abs(((*map).myPos.x)-i) + abs (((*map).myPos.y)-j);
                cur_prio_dist = ((cur_prio_dist * ponderPrio) - (distance * ponderDist));
                if (cur_prio_dist > maxPrio_dist )
                {
                    maxPrio_dist = cur_prio_dist;
                    curX = i;
                    curY = j;
                } // end if
            } // end if
        } // end for
    } // end for
    (*map).targetPos.setCoord(curX, curY);
    if ( (*map).priorityTable[(*map).targetPos.x][(*map).targetPos.y].type == empty )
    {
        (*map).targetType = 1;
    } else {  // targeting an item
        (*map).targetType = 0;
    }
    cerr << "New Target acquired : " << (*map).targetPos.x << " , " << (*map).targetPos.y << endl;
    /*
    if ( (*map).priorityTable[((*map).targetPos).x][((*map).targetPos).y].type > empty )
    {
        cerr << "///////// Targeting an item ! /////////" <<endl;   
    }
    */
 }// end determinateTarget
 
 
int main()
{
    string command = "MOVE";
    bool start = true;
    
    int width;
    int height;
    int myId;
    cin >> width >> height >> myId; cin.ignore();
    //cerr << "width : " << width << " height : " << height << " myId : " << myId << endl;

    Map map = Map();
    initializeMap(&map, myId, height, width);
    
    // game loop
    while (1) 
    {       
        for (int i = 0; i < height; i++) 
        {
            string row;
            getline(cin, row);          
            // create the table of priority
            if (start == true) 
            { 
                for(int j = 0; j < 13; j++) 
                {
                    if (row[j] == '.')
                    {
                        map.priorityTable[j][i].type = empty;
                    } else if (row[j] == 'X')
                    {
                        map.priorityTable[j][i].type = stone;
                    } else {
                        map.priorityTable[j][i].type = row[j] - '0';
                    } // end if
                } // end for
            } // end if
        } // end for
          
        
        
        if (start) 
        {       
            start = false;
            actualizeTablePriority(&map); 
            determinateTarget(&map);
        } 
        
         
           
        ///////////////////////////////////////
        cerr << endl << "///// type table ////////" << endl;
        for  (int j = 0; j < 11; j++)
        {
            for (int i = 0; i < 13; i++)
            {
                cerr << map.priorityTable[i][j].type;
            }
            cerr << endl;
        } // end for
        cerr << "/////////////////////" << endl << endl;
        /////////////////////////////////////////
        
        
        
        ///////////////////////////////////////
        // test display priorities
        cerr << "///// priority table ////////" << endl;
        for  (int j = 0; j < 11; j++)
        {
            for (int i = 0; i < 13; i++)
            {
                cerr << map.priorityTable[i][j].priority;
            }
            cerr << endl;
        } // end for
        cerr << "/////////////////////" << endl << endl;
        /////////////////////////////////////////
        
            
        /*
        ////////////////////////////////////////
        // test display matrix
        cerr << "///////// matrix objects ////////////" << endl;
        for  (int j = 0; j < 11; j++)
        {
            for (int i = 0; i < 13; i++)
            {
                cerr << map.priorityTable[i][j].matrixObjects[0].dist << "-" ;
                cerr << map.priorityTable[i][j].matrixObjects[1].dist << "-" ;
                cerr << map.priorityTable[i][j].matrixObjects[2].dist << "-" ;
                cerr << map.priorityTable[i][j].matrixObjects[3].dist << "|" ;
            }
            cerr << endl;
        } // end for
        cerr << endl;
        /////////////////////////////////////////
        */
        
        
        
        
        /*
        ///////////////////////////////////////
        // test display danger
        cerr << "///// DANGER TABLE ////////" << endl;
        for  (int j = 0; j < 11; j++)
        {
            for (int i = 0; i < 13; i++)
            {
                cerr << map.priorityTable[i][j].danger;
            }
            cerr << endl;
        } // end for
        cerr << "/////////////////////" << endl << endl;
        /////////////////////////////////////////
        */
        
        /*
        ///////////////////////////////////////
        // test display reachable
        cerr << "///// reachable table ////////" << endl;
        for  (int j = 0; j < 11; j++)
        {
            for (int i = 0; i < 13; i++)
            {
                cerr << map.priorityTable[i][j].reachable;
            }
            cerr << endl;
        } // end for
        cerr << "/////////////////////" << endl << endl;
        /////////////////////////////////////////
        */
        
  
        int entities;
        cin >> entities; cin.ignore();
        
        for (int i = 0; i < entities; i++) 
        {
            int entityType;
            int owner;
            int x;
            int y;
            int param1;
            int param2;
            cin >> entityType >> owner >> x >> y >> param1 >> param2; cin.ignore();
            
             // bomb
            if ( (entityType == 1) ) 
            {
                if ( param1 == 8 ) // bomb has been put
                {
                    map.priorityTable[x][y].type = bomb;
                    map.priorityTable[x][y].cooldown = 8;
                    bombEffect(&map, x, y, param2);
                    setDangerTable(&map, x, y, param2);
                    determinateTarget(&map);
                } else if ( (param1 == 7) ) // bomb explode in X turn => DANGER
                {
                    //setDangerTable(&map, x, y, param2);
                } else if ( (param1 == 1) ) // bomb exploded
                {
                    //cerr << endl << "HIS BOMB EXPLODED !" << endl << endl;
                    //map.priorityTable[x][y].type = empty;
                    objectDestroyedEffect(&map, x, y);
                }               
            } // end if bomb
  
            // enemy champion
            if ((entityType == 0) && (owner != myId)) 
            {   
                map.hisPos.setCoord(x, y);
                if ( (map.priorityTable[x][y].type == rangeItem) || (map.priorityTable[x][y].type == bombItem) )
                {
                    //cerr << "ENEMY PICKED UP AN ITEM !" << endl;
                    objectDestroyedEffect(&map, x, y);
                }
            } // end if
                
            // my character
            if ((entityType == 0) && (owner == myId)) 
            {
                map.myPos.setCoord(x, y);
                
                //checkDangerPath(&priorityTable, x, y, &posTargetX, &posTargetY);
                
                if (map.priorityTable[x][y].type == rangeItem) 
                {
                    cerr << "I got an ITEM RANGE !" << endl;
                    map.myBombRange = param2;
                    map.itemCounterRange = 2;
                    objectDestroyedEffect(&map, x, y);
                    //determinateTarget(&map);
                }
                
                if (map.priorityTable[x][y].type == bombItem)
                {
                    cerr << "I got an ITEM BOMB !" << endl; 
                    map.itemCounterBomb = 1;
                    objectDestroyedEffect(&map, x, y);
                    //determinateTarget(&map);
                }
                //cerr << "my pos : " << x << " , " << y << endl;
                //cerr << "Target pos : " << posTargetX << " , " << posTargetY << endl;
                
                // Target reached => move or bomb
                if ( (x == map.targetPos.x) && (y == map.targetPos.y) )
                {
                    if (map.targetType == 1) 
                    {
                        command = "BOMB";
                    }  else { // arrived on item
                        command = "MOVE";
                    }
                } else {
                    command = "MOVE";   
                } // end if
                
                decreaseCooldown(&map);
                actualizeTablePriority(&map); 
                determinateTarget(&map); 
                checkDangerPath(&map, x, y);
            } // end if
        } // end for
           
        
        
        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cout << command << " " << map.targetPos.x << " " << map.targetPos.y << " Raizyl" << endl;
    } // end while
    
}
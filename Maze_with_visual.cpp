#include <iostream>
#include <cstdlib>
#include <ctime>
#include <Windows.h>
#include <conio.h>
#include <string>
#include <stack>
#include <iomanip>

using namespace std;

//			
//	<!    Size Of Maze    !>
#define SIZE 51     //			<!   only non even number    !>

const int WALL = -1;        

const int BLANK = -2;		         

//			<!    Structure For Every Cell In Maze    !>
struct Cell {
	bool visited;
	bool top_wall;
	bool bot_wall;
	bool left_wall;
	bool right_wall;
	int display;
};

//Structurs for printing fast  
struct Coord_2D
{
	int x = 0;
	int y = 0;
};

size_t coords_to_index(const Coord_2D& coords, const Coord_2D& dimensions)
{
	return coords.y * dimensions.x + coords.x;
}


//			<!    Declaration For Change Text Color Function    !>
void SetColor(int text, int backGround);

//			<!    Wave Algorithm For Finding the Exit in the Maze    !>
bool path_finder_struct(int startX, int startY, int finishX, int finishY, Cell Level[SIZE][SIZE], int *&pointX, int *&pointY, int &len);

//			<!    SetUp Structure   !>
void Initialize(Cell Level[][SIZE]);

//			<!    Draw Maze    !>
void Draw(Cell Level[][SIZE]);

//			<!    Draw Maze with the Path    !>
void Redraw(Cell Level[][SIZE], int len, int *pointX, int *pointY, int start_x, int start_y, int finish_x, int finish_y);

//			<!    Maze Generator    !> 
void GenerateMaze(Cell Level[][SIZE], string arr, Coord_2D dimensions);

void Clear();

int main() {
	srand(time(NULL));

	Coord_2D dimensions = { SIZE + 1, SIZE };
	string arr(dimensions.x * dimensions.y, (char)219);
	for (int i = 0; i < SIZE; i++) {
		arr[coords_to_index({ SIZE, i }, dimensions)] = '\n';
	}
	
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE); // Color
	system("Color 70");	 //Setting Color for Console background and text

	int start_x,	//Start j Coordinate 
		start_y,	//Start i Coordinate 
		finish_x,	//Finish j Coordinate
		finish_y,	//Finish i Coordinate
		len;		//Length of Path

	Cell Level[SIZE][SIZE];								//	Initialize Structure SIZE x SIZE 
	
	cout << "Press any key to start." << endl;
	_getch();

	Initialize(Level);			//	Calling Function "Initialize"
	GenerateMaze(Level, arr, dimensions);		//  Calling Function "GenerateMaze"
	Draw(Level);				//	Calling Function "Draw"

	int *pointX = new int[SIZE * SIZE];		// Array keeps shortest way j
	int *pointY = new int[SIZE * SIZE];		// Array keeps shortest way i

	cout << "Enter Start i: ";
	cin >> start_x;
	cout << "Enter Start j: ";
	cin >> start_y;
	cout << "Enter Finish i: ";
	cin >> finish_x;
	cout << "Enter Finish j: ";
	cin >> finish_y;

	path_finder_struct(start_y, start_x, finish_y, finish_x, Level, pointX, pointY, len);	//Calling Function "path_finder_struct"
	Redraw(Level, len, pointX, pointY, start_x, start_y, finish_x, finish_y);	//Calling Function "Redraw"
	
	_getch();
	delete[] pointX;
	delete[] pointY;
}


void SetColor(int text, int backGround) {
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdOut, (WORD)((backGround << 4) | text));
}

bool path_finder_struct(int startX, int startY, int finishX, int finishY, Cell Level[SIZE][SIZE], int *&pointX, int *&pointY, int &len) {
	int directX[4] = { 1, 0, -1, 0 };   // offsets corresponding to cell neighbors
	int directY[4] = { 0, 1, 0, -1 };   // Right, Bottom, Left and Top
	int wavecounter;			// Declaring a variable to store the value of the wave
	bool stop;					// Declare a variable to stop the loop

	if (Level[startY][startX].display == WALL || Level[finishY][finishX].display == WALL) {		// If Start or Finish is a Wall, then the Function breaks
		return false;
	}

	//		<!   Wave propagation    !>
	wavecounter = 0;
	Level[startY][startX].display = 0;            // Starting cell marked 0
	do {
		stop = true;               // Assume that all free cells are already labeled
		for (int y = 0; y < SIZE; y++) {
			for (int x = 0; x < SIZE; x++) {
				if (Level[y][x].display == wavecounter)                   // The cell (x, y) is marked with the number d
				{
					for (int k = 0; k < 4; k++)                    // Pass on all unlabeled neighbors
					{
						int valueY = y + directY[k], valueX = x + directX[k];
						if (valueY >= 0 && valueY < SIZE && valueX >= 0 && valueX < SIZE &&
							Level[valueY][valueX].display == BLANK)
						{
							stop = false;						// Unlabeled cells found
							Level[valueY][valueX].display = wavecounter + 1;      // Spread wave
						}
					}
				}
			}
		}
		wavecounter++;
	} while (!stop && Level[finishY][finishX].display == BLANK);

	if (Level[finishY][finishX].display == BLANK) {		// If the path is not found, exit the function.
		return false;
	}

	// Recovery path
	len = Level[finishY][finishX].display;            // The shortest path length from (startY, startX) to (finishY, finishX)
	int x, y;
	x = finishX;
	y = finishY;
	wavecounter = len;
	while (wavecounter > 0)
	{
		pointX[wavecounter] = x;
		pointY[wavecounter] = y;                   // Write the cell (x, y) in the path
		wavecounter--;
		for (int k = 0; k < 4; k++)
		{
			int iy = y + directY[k], ix = x + directX[k];
			if (iy >= 0 && iy < SIZE && ix >= 0 && ix < SIZE &&
				Level[iy][ix].display == wavecounter)
			{
				x = x + directX[k];
				y = y + directY[k];           // Go to the cell, which is 1 closer to the start
				break;
			}
		}
	}
	pointX[0] = startX;
	pointY[0] = startY;                    // Now pointX [0..len] and pointY [0..len] are the coordinates of the path cells
	return true;
}

void Initialize(Cell Level[][SIZE]) {
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			Level[i][j].display = -1;
			Level[i][j].visited = false;
			Level[i][j].top_wall = true;
			Level[i][j].bot_wall = true;			//    <!	Initialize walls with standard parameters	!>
			Level[i][j].left_wall = true;
			Level[i][j].right_wall = true;
		}
	}
	for (int i = 1; i < SIZE - 1; i++) {
		for (int j = 1; j < SIZE - 1; j++) {
			Level[1][j].top_wall = false;
			Level[SIZE - 2][j].bot_wall = false;			//    <!    Initialize passes with standard parameters.    !>
			Level[i][1].left_wall = false;
			Level[i][SIZE - 2].right_wall = false;
		}
	}
}

void Draw(Cell Level[][SIZE]) {
	for (int i = 0; i < SIZE; i++) {
		cout << setw(2) << i;
	}
	cout << endl;
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			if (Level[i][j].display == -1) {		
				SetColor(0, 7);
				cout << (char)219 << (char)219;
				SetColor(0, 7);
			}
			else {
				cout << setw(2) << " ";			
			}
		}
		cout << i << endl;
	}
}

void Redraw(Cell Level[][SIZE], int len, int *pointX, int *pointY, int start_x, int start_y, int finish_x, int finish_y) {
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			bool flag = false;
			for (int z = 0; z < len; z++) {
				if (pointX[z] == j && pointY[z] == i) {		// Find the shortest way
					flag = true;
				}
			}
			if (i == start_x && j == start_y) {		// If start - set the letter S
				SetColor(5, 7);
				cout << setw(3) << "S";
				SetColor(0, 7);
			}
			else if (i == finish_x && j == finish_y) {		// If the finish - put the letter F
				SetColor(5, 7);
				cout << setw(3) << "F";
				SetColor(0, 7);
			}
			else if (Level[i][j].display == WALL) {		// If the wall - display the character (char)219
				SetColor(0, 7);
				cout << (char)219 << (char)219 << (char)219;
				SetColor(0, 7);
			}
			else if (Level[i][j].display == BLANK) {		// If there is a BLANK, then output a space
				cout << setw(3) << " ";
			}
			else if (flag) {		// If this is the shortest path to a certain point, write the value in red
				SetColor(4, 7);
				cout << setw(3) << Level[i][j].display;
				SetColor(0, 7);
			}
			else if (!flag) {		// If this is a branch, write the value in green
				SetColor(0, 7);
				cout << setw(3) << Level[i][j].display;
				SetColor(0, 7);
			}
		}
		cout << endl;
	}
}

void GenerateMaze(Cell Level[][SIZE], string arr, Coord_2D dimensions) {
	
	int random;
	int randomX = ((2 * rand()) + 1) % (SIZE - 1);				//    <!    Declare variables for random values    !>
	int randomY = ((2 * rand()) + 1) % (SIZE - 1);

	int visitedCells = 1;										//    <!    Declare the number of common and visited cells    !>
	int totalCells = ((SIZE - 1) / 2) * ((SIZE - 1) / 2);

	stack<int> back_trackX;										//    <!    Declare vectors for storing X and Y for further return.    !>
	stack<int> back_trackY;

	Level[randomY][randomX].visited = true;
	arr[coords_to_index({ randomX, randomY }, dimensions)] = ' ';
	bool flag = false;
	while (visitedCells < totalCells) {
		if (((Level[randomY - 2][randomX].visited == false) && (Level[randomY][randomX].top_wall == true) && (Level[randomY - 2][randomX].bot_wall == true)) ||
			((Level[randomY + 2][randomX].visited == false) && (Level[randomY][randomX].bot_wall == true) && (Level[randomY + 2][randomX].top_wall == true)) ||
			((Level[randomY][randomX - 2].visited == false) && (Level[randomY][randomX].left_wall == true) && (Level[randomY][randomX - 2].right_wall == true)) ||
			((Level[randomY][randomX + 2].visited == false) && (Level[randomY][randomX].right_wall == true) && (Level[randomY][randomX + 2].left_wall == true))) 
		{
			/*
				If:
					1. Level value with a random value of Y - 2 and a random value of X - not visited and are the bottom wall
					And the value of the level with a random value of Y and a random value of X - is the upper wall
				Or
					2. Level value with a random value Y + 2 and a random value X - not visited and are the upper wall
					And the level value with a random value of Y and a random value of X - is the bottom wall
				Or
					3. Level value with a random value of Y and a random value of X - 2 - not visited and are the right wall
					And the value of the level with a random value of Y and a random value of X - is the left wall
				Or
					4. Level value with a random value of Y and a random value of X + 2 - not visited and are the left wall
					And the value of the level with a random value of Y and a random value of X - is the right wall
				That:
			*/
			random = (rand() % 4) + 1;		// Generate random value from 1 to 4

			// Проверяем условия: 
			if ((random == 1) && (randomY > 1)) {
				if (Level[randomY - 2][randomX].visited == false) {

					Level[randomY - 1][randomX].display = -2;
					arr[coords_to_index({ randomX, randomY - 1 }, dimensions)] = ' ';
					Level[randomY - 1][randomX].visited = true;
					Level[randomY][randomX].top_wall = false;

					back_trackX.push(randomX);		 // Save X for Backtrack
					back_trackY.push(randomY);		 // Save Y for Backtrack

					randomY -= 2;
					arr[coords_to_index({ randomX, randomY }, dimensions)] = ' ';
					Level[randomY][randomX].visited = true;
					Level[randomY][randomX].display = -2;
					Level[randomY][randomX].bot_wall = false;
					visitedCells++;
					flag = true;
				}
				else {
					continue;
				}
			}
			else if ((random == 2) && (randomY < SIZE - 2)) {
				if (Level[randomY + 2][randomX].visited == false) {
					Level[randomY + 1][randomX].display = -2;
					arr[coords_to_index({ randomX, randomY + 1 }, dimensions)] = ' ';
					Level[randomY + 1][randomX].visited = true;
					Level[randomY][randomX].bot_wall = false;

					back_trackX.push(randomX);		 // Save X for Backtrack
					back_trackY.push(randomY);		 // Save Y for Backtrack

					randomY += 2;
					arr[coords_to_index({ randomX, randomY }, dimensions)] = ' ';
					Level[randomY][randomX].visited = true;
					Level[randomY][randomX].display = -2;
					Level[randomY][randomX].top_wall = false;
					visitedCells++;
					flag = true;
				}
				else {
					continue;
				}
			}
			else if ((random == 3) && (randomX > 1)) {
				if (Level[randomY][randomX - 2].visited == false) {
					Level[randomY][randomX - 1].display = -2;
					arr[coords_to_index({ randomX - 1, randomY }, dimensions)] = ' ';
					Level[randomY][randomX - 1].visited = true;
					Level[randomY][randomX].left_wall= false;

					back_trackX.push(randomX);		 // Save X for Backtrack
					back_trackY.push(randomY);		 // Save Y for Backtrack

					randomX -= 2;
					arr[coords_to_index({ randomX, randomY }, dimensions)] = ' ';
					Level[randomY][randomX].visited = true;
					Level[randomY][randomX].display = -2;
					Level[randomY][randomX].right_wall = false;
					visitedCells++;
					flag = true;
				}
				else {
					continue;
				}
			}
			else if ((random == 4) && (randomX < SIZE - 2)) {
				if (Level[randomY][randomX + 2].visited == false) {
					arr[coords_to_index({ randomX + 1, randomY }, dimensions)] = ' ';
					Level[randomY][randomX + 1].display = -2;
					Level[randomY][randomX + 1].visited = true;
					Level[randomY][randomX].right_wall = false;

					back_trackX.push(randomX);		 // Save X for Backtrack
					back_trackY.push(randomY);		 // Save Y for Backtrack

					randomX += 2;
					arr[coords_to_index({ randomX, randomY }, dimensions)] = ' ';
					Level[randomY][randomX].visited = true;
					Level[randomY][randomX].display = -2;
					Level[randomY][randomX].left_wall = false;
					visitedCells++;
					flag = true;
				}
				else {
					continue;
				}
			}
		}
		else  {
			randomX = back_trackX.top();
			back_trackX.pop();
			randomY = back_trackY.top();
			back_trackY.pop();
			flag = false;
		}
		if (flag) {
			cout << arr << endl;
			Clear();
			Sleep(20);
		}
	}
}

void Clear()
{
	HANDLE hOut;
	COORD Position;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	Position.X = 0;
	Position.Y = 0;
	SetConsoleCursorPosition(hOut, Position);
}

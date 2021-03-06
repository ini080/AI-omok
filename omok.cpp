#include "omok.h"

int    COMOK::arrBoard[SIZE + 2][SIZE + 2] = {LINE,};
int    COMOK::lx = 0;
int    COMOK::ly = 0;
int    COMOK::x = CENTER;
int    COMOK::y = CENTER;
time_t COMOK::t1 = clock() - 500;
time_t COMOK::t2 = clock();
bool   COMOK::undoflag = 0;

COMOK::COMOK()
{
}

//바둑이 시작될 때 초기화 
void COMOK::initOmok()
{
	drawBoard();
	initBoard();
	undoflag = 0;
	t2 = clock();
	// 1초에 한번씩 그리는데 시차를 두기 위해서
	// 결과적으로 500msec마다 커서처럼 깜박거리게 된다. 
	t1 = clock() - 500;
	pDraw()->infoKey();
}

//커서를 이동하기전 원래 바둑판을 복원한다. 
void COMOK::setBoard(int x, int y)
{
	pDraw()->printData(x, y, arrBoard[y][x]);
	// 커서가 이동중일땐 커서가 깜박잊 않게 한다.
	// 멈췄을때 얼마 지나지 않아 깜박이도록 시간 조절  
	t2 = clock() - 400;
	t1 = clock() - 900; 
}

// 커서를 대신해 커서처럼 보이게 한다.
void COMOK::cursorView(int x, int y)
{
	// 커서를 1초단위로 그려준다. 
	if(clock() - t1 >= 1000)
	{
		pDraw()->printData(x, y, CURSOR);
		t1 = clock();
	}
	// 커서 위치의 바둑판을 1초 단위로 그려준다. 
	if(clock() - t2 >= 1000)
	{
		pDraw()->printData(x, y, arrBoard[y][x]);
		t2 = clock();
	}
}

// 한수 물리기 위한함수 
int COMOK::undo()
{
	if(!undoflag) return NOTUNDO;

	pDraw()->printData(lx, ly, LINE);
	arrBoard[ly][lx] = LINE;
	undoflag = false;
	
	return CHANGE;
} 

void COMOK::undoSet()
{
	lx = x;
	ly = y;
	undoflag = true;
}

// 바둑판을 그린다. 
void COMOK::drawBoard()
{
	pDraw()->printNum();
	for(int i = 1; i <= SIZE; i++)
		for(int j = 1; j <= SIZE; j++)
			pDraw()->printData(j, i, LINE);
} 

// 돌이 놓이면 배열에 흑백돌을 저장한다.
// 처음 흑돌을 중앙에 한수 놓고 시작한다. 
void COMOK::initBoard()
{
	for(int i = 0; i < SIZE + 2; i++)
		for(int j = 0; j <= SIZE + 2; j++)
			arrBoard[i][j] = LINE;
	arrBoard[CENTER][CENTER] = BLACK_STONE;
} 

// 바둑알 놓을 자리를 체크한다. 
bool COMOK::isOccupy()
{
	return arrBoard[y][x] != LINE;
}

// 바둑알이 착점 되면 보드에 바둑알을 그리고
// 보드 배열에 저장한다.  
void COMOK::saveBoard(int stone)
{
	pDraw()->printData(x, y, stone);
	pDraw()->stonePoint(x, y);
	arrBoard[y][x] = stone;
	undoSet();
}

// Game class에서 받아온 좌표를 저장한다. 
void COMOK::setXY(int ax, int ay)
{
	x = ax;
	y = ay;
}

// 착점을 위한함수
// 착점이 가능한지 체크하고
// 착점이 되면 저장을 한다.
// 다음으로 오목인지 검사하여 결과를 리턴한다. 
int COMOK::placement(int ax, int ay, int nStone)
{
	setXY(ax, ay);
	if(isOccupy()) return OCCUPIED;
	
	int returnValue = CHANGE;
	orule.SetBoard(arrBoard);
	if(orule.IsFive(x, y, nStone) || orule.IsSix(x, y, nStone)) returnValue = FIVEMOK;
	saveBoard(nStone);
	
	return returnValue;
}

CBLACKSTONE::CBLACKSTONE()
{
}

int CBLACKSTONE::placement(int ax, int ay, int nStone)
{
	int returnValue = CHANGE;

	orule.SetBoard(arrBoard);
	ai.play(x, y, nStone, &orule);
	if(orule.IsFive(x, y, nStone) || orule.IsSix(x, y, nStone)) returnValue = FIVEMOK;
	saveBoard(nStone);
	
	return returnValue;
}

//
// Created by RenYi on 2017/6/17.
//

#include "virtualboard.h"

VirtualBoard::VirtualBoard(ItemType AIColor) : ChessBoard() {
    if (AIColor == BLACK) {
        comPointType = BLACK;
        humPointType = WHITE;
    } else {
        comPointType = WHITE;
        humPointType = BLACK;
    }
    blackScores.resize(ChessBoardWidth);
    for (int i = 0; i < ChessBoardWidth; i++) {
        this->blackScores[i].resize(ChessBoardWidth);
    }
    whiteScores.resize(ChessBoardWidth);
    for (int i = 0; i < ChessBoardWidth; i++) {
        this->whiteScores[i].resize(ChessBoardWidth);
    }
}

VirtualBoard::VirtualBoard(const VirtualBoard &board) : ChessBoard(board) {
    comPointType = board.comPointType;
    humPointType = board.humPointType;
    this->blackScores.resize(ChessBoardWidth);
    this->whiteScores.resize(ChessBoardWidth);
    for (int i = 0; i < ChessBoardWidth; i++) {
        this->blackScores[i].resize(ChessBoardWidth);
        this->whiteScores[i].resize(ChessBoardWidth);
        for (int j = 0; j < ChessBoardWidth; j++) {
            blackScores[i][j] = board.getScores(BLACK)[i][j];
            whiteScores[i][j] = board.getScores(WHITE)[i][j];
        }
    }

}

const vector<vector<Score>> &VirtualBoard::getScores(ItemType itemType) const {
    return itemType == BLACK ? blackScores : whiteScores;
}

void VirtualBoard::set(const ChessItem &next) {
    ChessBoard::set(next);
    if (next.player == BLACK) {
        whiteScores[next.cx][next.cy].setTotal(0);
        for (int dirI = 0; dirI < 8; dirI++)
            blackScores[next.cx][next.cy].setDirScore(dirI % 4,
                                                      evalScoreInOneDir(ChessItem(next.cx, next.cy, BLACK), dirI));
    }
    if (next.player == WHITE) {
        blackScores[next.cx][next.cy].setTotal(0);
        for (int dirI = 0; dirI < 8; dirI++)
            whiteScores[next.cx][next.cy].setDirScore(dirI % 4,
                                                      evalScoreInOneDir(ChessItem(next.cx, next.cy, WHITE), dirI));
    }
    for (int dirI = 0; dirI < 8; dirI++) { //dir
        int stopFlag[2] = {0}; // break if find three empty points in one direction
        for (int j = 1; j <= 8; j++) {
            int dirX = DIR[dirI].first, dirY = DIR[dirI].second;
            int x = next.cx + dirX * j, y = next.cy + dirY * j;
            if (stopFlag[BLACK] < 2) {
                if (!isBlock(ChessItem(x, y, BLACK))) { // black or empty
                    blackScores[x][y].setDirScore(dirI % 4, evalScoreInOneDir(ChessItem(x, y, BLACK), dirI));
                    if (isEmpty(ChessItem(x, y))) { // empty
                        stopFlag[BLACK]++;
                    }
                } else {
                    stopFlag[BLACK] = 2;
                }
            }
            if (stopFlag[WHITE] < 2) {
                if (!isBlock(ChessItem(x, y, WHITE))) { // white or empty
                    whiteScores[x][y].setDirScore(dirI % 4, evalScoreInOneDir(ChessItem(x, y, WHITE), dirI));
                    if (isEmpty(ChessItem(x, y))) { // empty
                        stopFlag[WHITE]++;
                    }
                } else {
                    stopFlag[WHITE] = 2;
                }
            }
        }
    }
}

int VirtualBoard::calcTypeScore(int totalItems, int firstEmpty, int blockType) {
    if (totalItems >= 10)
        return FIVE;
    if (firstEmpty == 0) {
        if (totalItems >= 5) {
            return FIVE*2; // XOOOOOX
        }
        if (blockType == 0) {
            switch (totalItems) {
                case 2:
                    return TWO*1.5; // OO
                case 3:
                    return THREE*1.5; // OOO
                case 4:
                    return FOUR*1.5; // OOOO
                default:
                    return 0;
            }
        } else if (blockType == 1) {
            switch (totalItems) {
                case 2:
                    return BLOCKED_TWO*1.5; // XOO
                case 3:
                    return BLOCKED_THREE*1.5; // XOOO
                case 4:
                    return BLOCKED_FOUR*1.5; // XOOOO
                default:
                    return 0;
            }
        }
    } else if (firstEmpty == 2) {
        if (totalItems >= 7) {
            return FIVE; // XOEOOOOOX
        }
        if (blockType == 0) {
            switch (totalItems) {
                case 4:
                    return THREE; // OEOO
                case 5:
                    return BLOCKED_FOUR; // OEOOO
                default:
                    return 0;
            }
        } else if (blockType == 1) {
            switch (totalItems) {
                case 4:
                    return BLOCKED_THREE; // XOEOO
                case 5:
                    return BLOCKED_FOUR; // XOEOOO
                case 6:
                    return FOUR/2; // XOEOOOO
                default:
                    return 0;
            }
        } else if (blockType == -1) {
            switch (totalItems) {
                case 4:
                    return BLOCKED_THREE; //  OEOOX
                case 5:
                    return BLOCKED_FOUR; //  OEOOOX
                case 6:
                    return FOUR/2; //  OEOOOOX
                default:
                    return 0;
            }
        } else if (blockType == 2) {
            switch (totalItems) {
                case 5:
                    return BLOCKED_FOUR; // XOEOOOX
                case 6:
                    return BLOCKED_FOUR; // XOEOOOOX
                default:
                    return 0;
            }
        }
    } else if (firstEmpty == 3) {
        if (totalItems >= 8) {
            return FIVE; // XOOEOOOOOX
        }
        if (blockType == 0) {
            switch (totalItems) {
                case 5:
                    return BLOCKED_FOUR; //OOEOO
                case 6:
                    return BLOCKED_FOUR; //OOEOOO
                case 7:
                    return FOUR/4; //OOEOOOO
                default:
                    return 0;
            }
        } else if (blockType == 1) {
            switch (totalItems) {
                case 5:
                    return BLOCKED_FOUR; // XOOEOO
                case 6:
                    return BLOCKED_FOUR; // XOOEOOO
                case 7:
                    return FOUR/4; // XOOEOOOO
                default:
                    return 0;
            }
        } else if (blockType == -1) {
            return BLOCKED_FOUR;
        } else if (blockType == 2) {
            switch (totalItems) {
                case 5:
                    return BLOCKED_FOUR;// XOOEOOX
                case 6:
                    return BLOCKED_FOUR;// XOOEOOOX
                case 7:
                    return BLOCKED_FOUR;// XOOEOOOOX
                default:
                    return 0;
            }
        }
    } else if (firstEmpty == 4) {
        if (totalItems >= 9) {
            return FIVE; // XOOOEOOOOOX
        }
        if (blockType == 0) {
            switch (totalItems) {
                case 7:
                    return BLOCKED_THREE; // OOOEOOO
                case 8:
                    return FOUR/4; //OOOEOOOO
                default:
                    return 0;
            }
        } else if (blockType == 1) {
            switch (totalItems) {
                case 7:
                    return BLOCKED_THREE; // XOOOEOOO
                case 8:
                    return FOUR/4; //XOOOEOOOO
                default:
                    return 0;
            }
        } else if (blockType == -1) {
            switch (totalItems) {
                case 7:
                    return BLOCKED_THREE; // OOOEOOOX
                case 8:
                    return BLOCKED_FOUR; //OOOEOOOOX
                default:
                    return 0;
            }
        } else if (blockType == 2) {
            switch (totalItems) {
                case 7:
                    return BLOCKED_THREE; // OOOEOOO
                case 8:
                    return FOUR/4; //OOOEOOOO
                default:
                    return 0;
            }
        }
    } else if (firstEmpty == 5) {
        if (totalItems >= 10) {
            return FIVE; // OOOOEOOOOO
        }
        if (blockType == 0) {
            return FOUR/4; //OOOOEOOOO
        } else if (blockType == 1 || blockType == -1) {
            return FOUR/4; //XOOOOEOOOO
        } else if (blockType == 2) {
            return BLOCKED_FOUR; //XOOOOEOOOOX
        }
    }
    return 0;
}

int VirtualBoard::evalScoreInOneDir(ChessItem item, int updateDir) {
    int dirX[2] = {DIR[updateDir % 4].first, DIR[updateDir % 4 + 4].first};
    int dirY[2] = {DIR[updateDir % 4].second, DIR[updateDir % 4 + 4].second};
    bool stop[2] = {0, 0};
    int count[2] = {0, 0};
    int firstEmpty[2] = {0, 0};
    int block[2] = {0, 0};
    for (int step = 1; !stop[0] || !stop[1]; step++) {
        for (int i = 0; i <= 1; i++) {
            if (stop[i]) {
                continue;
            }
            int x = item.cx + step * dirX[i], y = item.cy + step * dirY[i];
            if (isBlock(ChessItem(x, y, item.player))) {
                block[i] = step;
                stop[i] = true;
                continue;
            } else if (isEmpty(ChessItem(x, y))) {
                if (firstEmpty[i]) {
                    stop[i] = true;
                    continue;
                } else {
                    firstEmpty[i] = step;
                }
            }
            count[i] = step;
        }
    }
    for (int i = 0; i <= 1; i++) {
        if (count[i] == firstEmpty[i]) {
            count[i]--;
            firstEmpty[i] = 0;
        }
        if (block[i] == 0 || block[i] > count[i] + 1) {
            block[i] = 0;
        } else {
            block[i] = 1;
        }
    }

    int score = 0;
    for (int i = 0; i <= 1; i++) {
        int totalPoints, emptyPos, blockNum;
        if (firstEmpty[i]) {
            totalPoints = count[1 - i] + firstEmpty[i];
            emptyPos = firstEmpty[1 - i] > 0 ? count[1 - i] - firstEmpty[1 - i] + 1 : 0;
            blockNum = -block[1 - i]; // the block item is in the opposite direction of  the empty item
        } else {
            totalPoints = count[1 - i] + count[i] + 1;
            emptyPos = firstEmpty[1 - i] > 0 ? count[1 - i] - firstEmpty[1 - i] + 1 : 0;
//                blockNum = block[i] + block[1 - i];
            if (block[i] + block[1 - i] == 2) {
                blockNum = 2;
            } else if (block[i] + block[1 - i] == 1) {
                if (emptyPos > 0 && block[i]) {
                    blockNum = -1;
                } else {
                    blockNum = 1;
                }
            } else {
                blockNum = 0;
            }
        }
        if (emptyPos > (totalPoints + 1) / 2) {
            emptyPos = totalPoints - emptyPos + 1;
            if (block[i] == 1 || block[i] == -1) {
                block[i] = -block[i];
            }
        }
        int thisScore = calcTypeScore(totalPoints, emptyPos, blockNum);
        score = max(score, thisScore);
    }
    if(!score){
        for (int i = 0; i <=1; i++) {
            if(isItem(ChessItem(item.cx+dirX[i],item.cx+dirY[i]))){
                score++;
            }
        }
    }
}

int VirtualBoard::evalGlobalScore(ItemType itemType) const {
    int maxScore[2] = {0};
    int maxi,maxj;
    for (int i = 0; i < ChessBoardWidth; i++) {
        for (int j = 0; j < ChessBoardWidth; j++) {
            if (!isEmpty(ChessItem(i, j))) {
                if (getScores(currentChessBoard[i][j])[i][j].getTotal() >
                    maxScore[currentChessBoard[i][j]]) {
                    maxScore[currentChessBoard[i][j]] =
                            getScores(currentChessBoard[i][j])[i][j].getTotal();
                    if(itemType==BLACK){
                        maxi=i;
                        maxj=j;
                    }
                }
            }
//            else {
//                if (getScores(itemType)[i][j].getTotal() >
//                    maxScore[itemType]) {
//                    maxScore[itemType] = getScores(itemType)[i][j].getTotal();
//                }
//            }
        }
    }
    return 1.2*maxScore[itemType] - maxScore[reverseItemType(itemType)];
}

ItemType VirtualBoard::getComPointType() const {
    return comPointType;
}

ItemType VirtualBoard::getHumPointType() const {
    return humPointType;
}

void VirtualBoard::print(bool showScores ) {
    cout << "   ";
    for (int j = 0; j < ChessBoardWidth; j++) {
        cout << j << " ";
        if (j < 10) {
            cout << " ";
        }
    }
    cout << endl;
    for (int i = 0; i < ChessBoardWidth; i++) {
        cout << i << " ";
        if (i < 10) {
            cout << " ";
        }
        for (int j = 0; j < ChessBoardWidth; j++) {
            if (getChessBoard()[i][j] == -1) {
                cout << "   ";
            }
            if (getChessBoard()[i][j] == 0) {
                cout << "H  ";
            }
            if (getChessBoard()[i][j] == 1) {
                cout << "C  ";
            }
        }
        cout << endl;
    }

    if (showScores) {
        for (int i = 0; i < ChessBoardWidth; i++) {
            for (int j = 0; j < ChessBoardWidth; j++) {
                cout << "|" << i << "," << j << ":" << getScores(BLACK)[i][j].getTotal() << ","
                     << getScores(WHITE)[i][j].getTotal() << "| ";
            }
            cout << endl;
        }
        cout << evalGlobalScore(BLACK)<< " "<< evalGlobalScore(WHITE) << endl;
    }
}

void Score::setDirScore(const int &dir, const int &score) {
    int delta = score - dirScore[dir];
    dirScore[dir] = score;
    total += delta;
    if (total <= BLOCKED_FOUR && total >= THREE * 2) { //double three
        convertTotal = FOUR*0.8;
    } else if (total < FOUR/4 && total >= BLOCKED_FOUR) {
        if (total >= BLOCKED_FOUR && total < (BLOCKED_FOUR + THREE)) {
            convertTotal = THREE;
        } else if (total >= BLOCKED_FOUR + THREE && total < BLOCKED_FOUR * 2) {
            convertTotal = FOUR;
        } else if (total >= BLOCKED_FOUR * 2) {
            convertTotal = FOUR * 2;
        }
    } else {
        convertTotal = total;
    }
}

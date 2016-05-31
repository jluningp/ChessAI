import numpy
import copy
import tkinter
from tkinter import PhotoImage
import os
import random
import time
import queue
import threading
import subprocess
from subprocess import Popen, PIPE

class MoveArray:
    def __init__(self):
        self.moveList = [None, None, None, None, None, None]
        self.boardList = [None, None, None, None, None, None, None, None, None, None]
        self.initialSetupMoved = numpy.zeros((8,8))
        self.nextLoc = 0
        self.nextBoard = 0
        self.moveCount = 0
    
    def getLast(self):
        return self.moveList[(self.nextLoc - 1) % 6]
        
    def setNext(self, move):
        self.moveList[self.nextLoc] = move
        self.nextLoc = (self.nextLoc + 1) % 6
        
    def setNextBoard(self, board):
        self.boardList[self.nextBoard] = board
        self.nextBoard = (self.nextBoard + 1) % 10
        self.moveCount+=1
        
    def boardEQ(self, board1, board2):
        if(board1 == None or board2 == None):
            return False
        for x in range(0,8):
            for y in range(0,8):
                    if(board1[(y,x)] != board2[(y,x)]):
                        return False
        return True
    
    def sameTwoBoards(self, board):
        second = False
        third = False
        for bd in self.boardList:
            eq = self.boardEQ(board, bd)
            if(second and eq):
                third = True
                second = False
            elif(third and eq):
                return True
            elif(eq):
                second = True
        return False
    
    def threeSame(self):
        for board in self.boardList:
            if(self.sameTwoBoards(board)):
                return True
        return False
        
    def setMoved(self, location):
        self.initialSetupMoved[location] = 1
    
    def wasMoved(self, location):
        return self.initialSetupMoved[location] == 1

class ChessBoard:
    def __init__(self):
        self.board = numpy.zeros((8, 8)) 
        self.whiteTurn = True
        self.inPlay = True
        self.moveMemory = MoveArray()
        #white king, white rook, black king, black rook
        self.moved = [False, False, False, False, False, False]
        self.pieceLookup = {"White Pawn": 11, 
        "White Knight": 12, 
        "White Bishop": 13,
        "White Rook": 14,
        "White Queen": 15,
        "White King": 16,
        "Black Pawn": 21, 
        "Black Knight": 22, 
        "Black Bishop": 23,
        "Black Rook": 24,
        "Black Queen": 25,
        "Black King": 26}

    def toString(self):
        s = ""
        for i in range(0,8):
            for j in range(0,8):
                if(self.board[(i,j)] == 0):
                    s += "00"
                else:
                    s += str(int(self.board[(i,j)]))
        if(self.moveMemory.getLast() == None):
            s += "0000"
        else:
            ((startY, startX), (endY, endX), stuff) = self.moveMemory.getLast()
            s += str(startY) + str(startX) + str(endY) + str(endX)
        if(self.whiteTurn):
            s += "1"
        else:
            s += "0"
        for x in range(0,6):
            if(self.moved[x]):
                s += "1"
            else:
                s += "0"
        return s

    def getCAI(self):
        args = "./getAI " + self.toString()
        p = Popen(args, stdout=PIPE, stderr=PIPE, shell=True)
        stdout, stderr = p.communicate()
        moveString = stdout.decode("utf-8")
        nums = moveString.split()
        return ((int(nums[0]), int(nums[1])), (int(nums[2]), int(nums[3])))


    def getCLegalMove(self, startLocation, endLocation):
        (startY, startX) = startLocation
        (endY, endX) = endLocation
        args = "./getAI " + self.toString() + str(startY) + str(startX) + str(endY) + str(endX)
        print(args)
        p = Popen(args, stdout=PIPE, stderr=PIPE, shell=True)
        stdout, stderr = p.communicate()
        moveString = stdout.decode("utf-8")
        print(moveString)
        return (moveString == "1")
    
    def getPieceName(self, num):
        for str in self.pieceLookup:
            if self.pieceLookup[str] == num:
                return str
        return ""
        
    def pieceValue(self, num):
        pieces = {0: 0,
        11: 1,
        12: 3,
        13: 3,
        14: 5,
        15: 9,
        16: 40}
        (white, black) = self.centerFill()
        if(white + black > 4):
            pieces[12] = 4
            pieces[13] = 2
        if(white + black < 3):
            pieces[12] = 2
            pieces[13] = 4
        if(num > 20):
            return pieces[num - 10]
        else:
            return pieces[num]

    def setBoard(self):
        self.board[(0, 0)] = 24
        self.board[(0, 1)] = 22
        self.board[(0, 2)] = 23
        self.board[(0, 3)] = 25
        self.board[(0, 4)] = 26
        self.board[(0, 5)] = 23
        self.board[(0, 6)] = 22
        self.board[(0, 7)] = 24
        self.board[(1, 0)] = 21
        self.board[(1, 1)] = 21
        self.board[(1, 2)] = 21
        self.board[(1, 3)] = 21
        self.board[(1, 4)] = 21
        self.board[(1, 5)] = 21
        self.board[(1, 6)] = 21
        self.board[(1, 7)] = 21 
              
        self.board[(7, 0)] = 14
        self.board[(7, 1)] = 12
        self.board[(7, 2)] = 13
        self.board[(7, 3)] = 15
        self.board[(7, 4)] = 16
        self.board[(7, 5)] = 13
        self.board[(7, 6)] = 12
        self.board[(7, 7)] = 14
        self.board[(6, 0)] = 11
        self.board[(6, 1)] = 11
        self.board[(6, 2)] = 11
        self.board[(6, 3)] = 11
        self.board[(6, 4)] = 11
        self.board[(6, 5)] = 11
        self.board[(6, 6)] = 11
        self.board[(6, 7)] = 11
        self.inPlay = True
        self.whiteTurn = True
        
    def getPiece(self, location):
        return self.board[location]
    
    def getPieceMoves(self, location):
        moves = []
        for x in range(4,12):
            for y in range(4,12):
                if(self.checkLegalMove(location, (y % 8,x % 8))):
                    moves.append((location, (y % 8,x % 8)))
        return moves
    
    def legalMoves(self):
        white = self.whiteTurn
        legalMoves = []
        for x in range(4,12):
            for y in range(4,12):
                piece = self.getPiece((y % 8,x % 8))
                if((piece > 20 and not white) or 
                (piece < 20 and piece != 0 and white)):
                    legalMoves += self.getPieceMoves((y % 8,x % 8))
        return legalMoves

    def getAIMoves(self):
        legalMoves = self.legalMoves()
        aiMoves = []
        count = 0 #for my own curiosity
        for move in legalMoves:
            white = self.whiteTurn
            (start, end) = move
            (startY, startX) = start
            (endY, endX) = end
            piece = self.getPiece(start)
            takePiece = self.getPiece(end)
            ignoreKing = (piece == 26 or piece == 16 and takePiece == 0 
                        and not self.checkCastle(start, end))
            #isEndGame = opponents queen and two non-pawn pieces are off the board
            gPawn = ((piece == 11 or piece == 21) and startX == 6 and not 
                        self.moveMemory.wasMoved(start) and takePiece == 0)
            edgeKnight = ((piece == 12 or piece == 22) and takePiece == 0 and 
                            (endY == 7 or endY == 0) and not self.moveMemory.wasMoved(start))
            if(not (ignoreKing or gPawn or edgeKnight)):   
                aiMoves.append(move)
            else:
                count+=1
        #print(count)
        return aiMoves
    
    
    def checkLegalMove(self, startLocation, endLocation):
        piece = self.getPiece(startLocation)
        legalMove = False
        if(piece == 11 or piece == 21):
            legalMove = self.checkPawnMove(startLocation, endLocation)
        elif (piece == 13 or piece == 23):
            legalMove = self.checkBishopMove(startLocation, endLocation)
        elif (piece == 14 or piece == 24):
            legalMove = self.checkRookMove(startLocation, endLocation)
        elif (piece == 15 or piece == 25):
            legalMove = self.checkQueenMove(startLocation, endLocation)
        elif (piece == 12 or piece == 22):
            legalMove = self.checkKnightMove(startLocation, endLocation)
        elif (piece == 16 or piece == 26):
            legalMove = self.checkKingMove(startLocation, endLocation)
        return (legalMove and not self.causesSelfCheck(startLocation, endLocation)
        and (self.whiteTurn) == (self.isWhite(self.getPiece(startLocation))))
    
    def naiveAI(self):
        moves = self.legalMoves()
        num = random.randint(0, len(moves) - 1)
        (startLocation, endLocation) = moves[num]
        self.makeMove(startLocation, endLocation)
        
    def materialAI(self):
        #move = self.alphabeta(3, self, (None, None))
        #print(move.num)
        #(startLocation, endLocation) = move.move
        (startLocation, endLocation) = self.getCAI()
        self.makeMove(startLocation, endLocation)
    
    def checkMate(self):
        if(not len(self.legalMoves()) > 0):
            if(self.inCheck(self.whiteTurn)):
                return 2
            else:
                return 1
        else:
            return 0
    
    def makeMove(self, startLocation, endLocation):
        if(self.inPlay):
            if self.checkLegalMove(startLocation, endLocation):
                if(self.checkCastle(startLocation, endLocation)):
                    white = self.isWhite(self.getPiece(startLocation))
                    if(white):
                        if(endLocation == (7,6)):
                            self.board[(7,4)] = 0
                            self.board[(7,6)] = 16
                            self.board[(7,7)] = 0
                            self.board[(7,5)] = 14
                        else:
                            self.board[(7,4)] = 0
                            self.board[(7,2)] = 16
                            self.board[(7,0)] = 0
                            self.board[(7,3)] = 14
                    else:
                        if(endLocation == (0,6)):
                            self.board[(0,4)] = 0
                            self.board[(0,6)] = 26
                            self.board[(0,7)] = 0
                            self.board[(0,5)] = 24
                        else:
                            self.board[(0,4)] = 0
                            self.board[(0,2)] = 26
                            self.board[(0,0)] = 0
                            self.board[(0,3)] = 24
                elif(self.enPassant(startLocation, endLocation)):
                    self.board[endLocation] = self.board[startLocation]
                    self.board[startLocation] = 0
                    (endY, endX) = endLocation
                    if(self.whiteTurn):
                        self.board[(endY + 1, endX)] = 0
                    else:
                        self.board[(endY - 1, endX)] = 0
                else:
                    self.board[endLocation] = self.board[startLocation]
                    self.board[startLocation] = 0
                    self.checkQueen(endLocation)
                self.moveMemory.setNext((startLocation, endLocation, self.getPiece(endLocation)))
                self.moveMemory.setMoved(startLocation)
                self.moveMemory.setNextBoard(copy.deepcopy(self.board))
                if(self.moveMemory.threeSame()):
                    self.inPlay = False
                    print("Threefold  Repetition")
                self.checkKingRook(endLocation)
                self.switchTurn()
                if(self.checkMate() == 2):
                    self.inPlay = False
                    print("Checkmate!")
                elif(self.checkMate() == 1):
                    self.inPlay = False
                    print("Stalemate!")
                return True
            else:
                print( "Illegal Move", self.getPieceName(self.getPiece(startLocation)))
                return False
        else:
            return False
    
    # REQUIRES: piece > 0
    def isWhite(self, piece):
        return piece < 20 and piece != 0
    
    def checkCastle(self, startLocation, endLocation):
        piece = self.getPiece(startLocation)
        if(piece == 16 and startLocation == (7,4) and endLocation == (7,6)):
            return True
        elif(piece == 16 and startLocation == (7,4) and endLocation == (7,2)):
            return True
        elif(piece == 26 and startLocation == (0,4) and endLocation == (0,6)):
            return True
        elif(piece == 26 and startLocation == (0,4) and endLocation == (0,2)):
            return True
        else:
            return False
    
    def checkQueen(self, location):
        (y,x) = location
        piece = self.getPiece(location)
        if(piece == 11 and y == 0):
            self.board[location] = 15
        elif(piece == 21 and y == 7):
            self.board[location] = 25
    
    def checkKingRook(self, location):
        piece = self.getPiece(location)
        if(piece == 16):
            self.moved[0] = True
        elif(piece == 26):
            self.moved[2] = True
        elif(piece == 14 and location == (7,7)):
            self.moved[1] = True
        elif(piece == 24 and location == (0,7)):
            self.moved[3] = True
        elif(piece == 14 and location == (7,0)):
            self.moved[4] = True
        elif(piece == 24 and location == (0,0)):
            self.moved[5] = True
    
    # REQUIRES: piece is a pawn
    def checkPawnMove(self, startLocation, endLocation):
        white = self.isWhite(self.getPiece(startLocation))
        (startY, startX) = startLocation
        (endY, endX) = endLocation
        if(endY < 0 or endY >= 8 or endX < 0 or endX >= 8):
            return False
        emptySquare = self.getPiece(endLocation) == 0
        takeWhite = self.isWhite(self.getPiece(endLocation))
        legalTake = takeWhite != white and not emptySquare
        taking = (legalTake and abs(startX - endX) == 1 and
        ((white and startY - endY == 1) or (not white and endY - startY == 1)))
        moving = (emptySquare and startX == endX and 
        ((white and startY - endY == 1) or (not white and endY - startY == 1)))
        firstMove = (emptySquare and startX == endX and 
        ((white and startY - endY == 2 and startY == 6 and 
        self.getPiece((startY - 1, startX)) == 0) or 
        (not white and endY - startY == 2 and startY == 1 and
        self.getPiece((startY + 1, startX)) == 0)))
        enPassant = self.enPassant(startLocation, endLocation)
        return taking or moving or firstMove or enPassant
        
    def enPassant(self, startLocation, endLocation):
        if(self.moveMemory.getLast() == None):
            return False
        (start, end, piece) = self.moveMemory.getLast()
        if((self.whiteTurn and piece != 21) or 
        (not self.whiteTurn and piece != 11)):
            return False
        (startPY, startPX) = start
        (endPY, endPX) = end
        (startY, startX) = startLocation
        (endY, endX) = endLocation
        sameX = startPX == endPX and startPX == endX and abs(startX - endX) == 1
        correctY = (abs(startPY - endPY) == 2 and (abs(startPY - endY) == 1 and 
        abs(endPY - endY) == 1) and abs(startY - endY) == 1)
        return sameX and correctY
        
    def checkBishopClean(self, startLocation, endLocation):
        white = self.isWhite(self.getPiece(startLocation))
        (startY, startX) = startLocation
        (endY, endX) = endLocation
        clear = True
        x = endX
        y = endY
        plusX = False
        plusY = False
        if(startX > endX):
            plusX = True
            x+=1
        else: x-=1
        if(startY > endY):
            plusY = True
            y+=1
        else: y-=1
        while(x != startX and y != startY):
            if(self.getPiece((y, x)) != 0):
                return False
            if(plusX): x+=1
            else: x-=1
            if(plusY): y+=1
            else: y-=1
        return True

    
    def checkBishopMove(self, startLocation, endLocation):
        white = self.isWhite(self.getPiece(startLocation))
        (startY, startX) = startLocation
        (endY, endX) = endLocation
        if(endY < 0 or endY >= 8 or endX < 0 or endX >= 8):
            return False
        emptySquare = self.getPiece(endLocation) == 0
        takeWhite = self.isWhite(self.getPiece(endLocation))
        legalTake = emptySquare or takeWhite != white
        moveVal = (startX - endX) + (startY - endY)  
        otherMoveVal = (startX - endX) - (startY - endY)
        return (legalTake and (moveVal == 0 or otherMoveVal == 0) and 
        (startX != endX and startY != endY) and self.checkBishopClean(startLocation, endLocation))

    def checkRookClean(self, startLocation, endLocation):
        (startY, startX) = startLocation
        (endY, endX) = endLocation
        x = endX
        y = endY
        moveX = startY == endY
        negative = (startY - endY) + (startX - endX) <= 0
        if(moveX):
            if(negative): x-=1
            else: x+=1
        else:
            if(negative): y-=1
            else: y+=1
        while(x != startX or y != startY):
            if(self.getPiece((y, x)) != 0):
                return False
            if(moveX):
                if(negative): x-=1
                else: x+=1
            else:
                if(negative): y-=1
                else: y+=1
        return True
        
    def checkRookMove(self, startLocation, endLocation):
        white = self.isWhite(self.getPiece(startLocation))
        (startY, startX) = startLocation
        (endY, endX) = endLocation
        if(endY < 0 or endY >= 8 or endX < 0 or endX >= 8):
            return False
        emptySquare = self.getPiece(endLocation) == 0
        takeWhite = self.isWhite(self.getPiece(endLocation))
        legalTake = emptySquare or takeWhite != white
        moveVal = startX == endX or startY == endY
        return (legalTake and moveVal and startLocation != endLocation and 
        self.checkRookClean(startLocation, endLocation))

    def checkQueenMove(self, startLocation, endLocation):
        return (self.checkRookMove(startLocation, endLocation) or 
        self.checkBishopMove(startLocation, endLocation))
        
    def checkKnightMove(self, startLocation, endLocation):
        white = self.isWhite(self.getPiece(startLocation))
        (startY, startX) = startLocation
        (endY, endX) = endLocation
        if(endY < 0 or endY >= 8 or endX < 0 or endX >= 8):
            return False
        emptySquare = self.getPiece(endLocation) == 0
        takeWhite = self.isWhite(self.getPiece(endLocation))
        legalTake = emptySquare or takeWhite != white
        if(not legalTake):
            return False
        if(abs(endX - startX) == 1):
            return abs(endY - startY) == 2
        elif(abs(endY - startY) == 1):
            return abs(endX - startX) == 2
        else: 
            return False
            
    def checkKingMove(self, startLocation, endLocation):
        white = self.isWhite(self.getPiece(startLocation))
        (startY, startX) = startLocation
        (endY, endX) = endLocation
        #updates for castling rule
        if(white):
            if(startX == 4 and startY == 7 and endX == 6 and endY == 7 and not self.moved[0] and not self.moved[1] and self.getPiece((7,5)) == 0 and self.getPiece((7,6)) == 0 
            and not self.causesSelfCheck((7,4),(7,5))):
                    return True
            elif(startX == 4 and startY == 7 and endX == 2 and endY == 7 and 
            not self.moved[0] and not self.moved[4] and self.getPiece((7,3)) == 0 and self.getPiece((7,2)) == 0 and self.getPiece((7,1)) == 0 and not self.causesSelfCheck((7,4),(7,3))):
                return True
        else:
            if(startX == 4 and startY == 0 and endX == 6 and endY == 0 and not self.moved[2] and not self.moved[3] and self.getPiece((0,5)) == 0 and self.getPiece((0,6)) == 0 
            and not self.causesSelfCheck((0,4),(0,5))):
                return True
            elif(startX == 4 and startY == 0 and endX == 2 and endY == 0 and 
            not self.moved[2] and not self.moved[5] and self.getPiece((0,3)) == 0 and self.getPiece((0,2)) == 0 and self.getPiece((0,1)) == 0 and not self.causesSelfCheck((0,4),(0,3))):
                return True
        if(endY < 0 or endY >= 8 or endX < 0 or endX >= 8):
            return False
        emptySquare = self.getPiece(endLocation) == 0
        takeWhite = self.isWhite(self.getPiece(endLocation))
        legalTake = emptySquare or takeWhite != white
        if(not legalTake):
            return False
        if(abs(startX - endX) == 1):
            return abs(startY - endY) <= 1
        elif(abs(startY - endY) == 1):
            return abs(startX - endX) <= 1
        return False

    def getKing(self, white):
        for x in range(0,8):
            for y in range(0,8):
                if((self.getPiece((y,x)) == 16 and white) or 
                (self.getPiece((y,x)) == 26 and not white)):
                    return (y,x)
        return None

    def switchTurn(self):
        self.whiteTurn = not self.whiteTurn

    def inCheck(self, white):
        kingPosition = self.getKing(white)
        if(kingPosition == None):
            return True
        self.switchTurn()
        for x in range(0,8):
            for y in range(0,8):
                piece = self.getPiece((y,x))
                if((piece > 20 and white) or 
                (piece < 20 and piece != 0 and not white)):
                    if(self.checkLegalMove((y,x), kingPosition)):
                        self.switchTurn()
                        return True
        self.switchTurn()
        return False
        
    #REQUIRES: is a legal move other than causing check
    def causesSelfCheck(self, startLocation, endLocation):
        white = self.isWhite(self.getPiece(startLocation))
        original = self.getPiece(endLocation)
        self.board[endLocation] = self.board[startLocation]
        self.board[startLocation] = 0
        selfCheck = self.inCheck(white)
        self.board[startLocation] = self.board[endLocation]
        self.board[endLocation] = original
        return selfCheck


    def minimax(self, depth, board):
        maxie = board.whiteTurn
        if(depth == 0):
            return Branch(None, self.estimate(board, moveList))
        moveList = board.legalMoves()
        estimates = []
        if(len(moveList) == 0):
            return self.estimate(board, moveList)
        for move in moveList:
            (startLoc, endLoc) = move
            newBoard = copy.deepcopy(board)
            newBoard.makeAIMove(startLoc, endLoc)
            branch = self.minimax(depth - 1, newBoard)
            branch.move = move
            estimates.append(branch)
        if(maxie):
            return self.getMax(estimates)
        else:
            return self.getMin(estimates)
    
    def getMax(self, branches):
        max = None
        for branch in branches:
            if(max == None):
                max = branch
            elif(branch.estimate > max.estimate):
                max = branch
        return max
        
    def getMin(self, branches):
        max = None
        for branch in branches:
            if(max == None):
                max = branch
            elif(branch.estimate < max.estimate):
                max = branch
        return max
        
    def alphabeta(self, depth, board, range):
        (alpha, beta) = range
        maxie = board.whiteTurn
        if(depth == 0):
            return Branch(None, self.estimate(board, None), 0)
        estimates = []
        moveList = board.getAIMoves()
        if(len(moveList) == 0):
            moveList = board.legalMoves()
            if(len(moveList) == 0):
                return Branch(None, self.estimate(board, moveList), 0)
        for move in moveList:
            (startLoc, endLoc) = move
            print(str(startLoc) + str(endLoc) + " | ", end="")
            newBoard = copy.deepcopy(board)
            newBoard.makeAIMove(startLoc, endLoc)
            branch = self.alphabeta(depth - 1, newBoard, (copy.deepcopy(alpha), copy.deepcopy(beta)))
            branch.move = move
            branch.num = len(moveList)
            if(maxie):
                if(alpha == None):
                    alpha = branch
                elif(not beta == None and beta.estimate <= branch.estimate):
                    return branch
                elif(alpha.estimate < branch.estimate):
                    alpha = branch
            else:
                if(beta == None):
                    beta = branch
                elif(not alpha == None and alpha.estimate >= branch.estimate):
                    return branch
                elif(beta.estimate > branch.estimate):
                    beta = branch
        print("")
        if(maxie):
            return alpha
        else:
            return beta
            
    def makeAIMove(self, startLocation, endLocation):
        if(self.checkCastle(startLocation, endLocation)):
            white = self.isWhite(self.getPiece(startLocation))
            if(white):
                if(endLocation == (7,6)):
                    self.board[(7,4)] = 0
                    self.board[(7,6)] = 16
                    self.board[(7,7)] = 0
                    self.board[(7,5)] = 14
                else:
                    self.board[(7,4)] = 0
                    self.board[(7,2)] = 16
                    self.board[(7,0)] = 0
                    self.board[(7,3)] = 14
            else:
                if(endLocation == (0,6)):
                    self.board[(0,4)] = 0
                    self.board[(0,6)] = 26
                    self.board[(0,7)] = 0
                    self.board[(0,5)] = 24
                else:
                    self.board[(0,4)] = 0
                    self.board[(0,2)] = 26
                    self.board[(0,0)] = 0
                    self.board[(0,3)] = 24
        elif(self.enPassant(startLocation, endLocation)):
            self.board[endLocation] = self.board[startLocation]
            self.board[startLocation] = 0
            (endY, endX) = endLocation
            if(self.whiteTurn):
                self.board[(endY + 1, endX)] = 0
            else:
                self.board[(endY - 1, endX)] = 0
        else:
            self.board[endLocation] = self.board[startLocation]
            self.board[startLocation] = 0
            self.checkQueen(endLocation)
        self.moveMemory.setNext((startLocation, endLocation, self.getPiece(endLocation)))
        self.moveMemory.setMoved(startLocation)
        self.moveMemory.setNextBoard(copy.deepcopy(self.board))
        self.checkKingRook(endLocation)
        self.switchTurn()
    
    def centerFill(self):
        centralWhitePawns = 0
        centralBlackPawns = 0
        for y in range(3,4):
            for x in range(2,5):
                if(self.getPiece((y,x)) == 11):
                    centralWhitePawns+=1
                if(self.getPiece((y,x)) == 21):
                    centralBlackPawns+=1
        return (centralWhitePawns, centralBlackPawns)
    
    def estimate(self, board, moveList):
        maxie = 0
        minnie = 0
        (centralWhitePawns, centralBlackPawns) = board.centerFill()
        maxie+=centralWhitePawns / 2
        minnie+=centralBlackPawns / 2
        
        if(moveList != None and len(moveList) == 0):
            if(board.whiteTurn):
                if(board.inCheck(True)):
                    return -1000
                else:
                    return -50
            else:
                if(board.inCheck(False)):
                    return 1000
                else:
                    return 50
                
        for x in range(0,8):
            for y in range(0,8):
                piece = board.getPiece((y,x))
                if(piece > 20):
                    minnie += self.pieceValue(piece)
                else:
                    maxie += self.pieceValue(piece)
        return maxie - minnie
        
class Branch():
    def __init__(self, move, estimate, num):
        self.move = move
        self.estimate = estimate
        self.num = num

# ***INTERFACE***
# NOTE: ALL FUNCTIONS EDIT BOARD - IF YOU DONT WANT TO CHANGE ACTUAL GAME, DEEPCOPY WHOLE OBJECT
# setBoard() - sets the board to start a game
# makeMove(startPosition, endPosition) - moves a piece from startPostion to endPosition and returns true if 
#                                        the move is legal, and returns false otherwise
# legalMoves() - returns a list of all current legal moves
# inCheck(white) - returns true if white == true and white is in check 
#                               or if white == false and black is in check
# checkLegalMove(startPosition, endPosition) - checks if a move from startPosition to endPosition is legal
# naiveAI() - makes random moves for black
# materialAI() - alphabeta AI with serach depth 3 that looks mainly at material advantage moves for black
# ***************

class ClickRemember():
    def __init__(self):
        self.firstClick = True
        self.fromCoords = None
        self.AI = False
        self.t0 = 0

    def square_click(self, board, top, event):
        if(board.inPlay):
            square = (event.widget.grid_info()['row'], event.widget.grid_info()['column'])
            (r,c) = square
            if(self.firstClick):
                self.fromCoords = square
                self.firstClick = False
                event.widget.configure(bg=coloring(r,c)[2])
    
            else:
                self.firstClick = True
                moveMade = board.makeMove(self.fromCoords, square)
                label(board, top, self)
                self.fromCoords = None
                if(moveMade and board.inPlay):
                    self.t0 = time.time()
                    self.queue = queue.Queue()
                    ThreadedTask(self.queue, board, top, self).start()
                    top.after(10, lambda: self.process_queue(top))
                    
    def process_queue(self, top):
        try:
            msg = self.queue.get(0)
            print(msg)
        except queue.Empty:
            top.after(10, lambda: self.process_queue(top))

class ThreadedTask(threading.Thread):
    def __init__(self, queue, board, top, clicks):
        threading.Thread.__init__(self)
        self.queue = queue
        self.board = board
        self.top = top
        self.clicks = clicks
    def run(self):
        self.board.materialAI()
        label(self.board, self.top, self.clicks)
        t1 = time.time()
        self.queue.put(t1 - self.clicks.t0)

def coloring(row, col):
    if((row + col) % 2 == 0): return ['white', 'black', '#cce6ff']	
    else: return ['black', 'white', '#00284d']

def label(board, top, clicks):
    for r in range(0, 8):
        for c in range(0, 8):
            txt = coloring(r, c)[1]
            back = coloring(r,c)[0]
            pieceName = (board.getPieceName(board.getPiece((r,c))))
            if(pieceName != ""):
                #imageName = os.path.abspath("Documents\\Python Scripts\\chessAI\\" + ("".join(pieceName.split())).lower() + ".gif")
                imageName = ("".join(pieceName.split())).lower() + ".gif"
                img = PhotoImage(file=imageName)
                nextLabel = tkinter.Label(top, fg=txt, bg=back, text=" \n\n\n ",
                borderwidth=1, width=58, height=70, image=img)
                nextLabel.image = img
            else:
                nextLabel = tkinter.Label(top, fg=txt, bg=back,  text=pieceName,
                borderwidth=1, width=7, height=5 )
            nextLabel.grid(row=r,column=c)
            nextLabel.bind("<Button-1>", lambda event: clicks.square_click(board, top, event))

def graphics():
    board = ChessBoard()
    board.setBoard()
    top = tkinter.Tk()
    clicks = ClickRemember()
    #board.materialAI();
    label(board, top, clicks)
    top.mainloop()

graphics()


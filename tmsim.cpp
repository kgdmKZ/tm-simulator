#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

/* make an int into a vector of chars representing its reverse binary 
 * form up to and including its most significant bit
 * ex. binVector(6) produces a vector containing (in order) '0', '1', '1' 
 */
std::vector<char> binVector(unsigned int x) {
	int bit;
	vector<char> res;
	while (x != 0) {
		bit = x & 1;
		res.push_back(to_string(bit).at(0));
		x = x >> 1;
	}
	if (res.size() == 0) {
		res.push_back('0');
	}
	return res;
}

//convert a vector of '0' and '1' chars to a corresponding int
unsigned int vecToInt(vector<char> vec) {
	if (vec.size() > 32)
		vec.resize(32);
	reverse(begin(vec), end(vec));
	string istr(vec.begin(), vec.end());
	size_t *nptr = NULL;
	return (unsigned int)std::stoul(istr, nptr, 2);
}

//write to the tape at an index, even if the tape hasn't reached that size before
void writeSafe(vector<char> &tape, char write, int idx) {
	if (idx >= (int)tape.size())
		tape.resize(idx + 1, 'B');
	tape[idx] = write;
}

/* The functions below that return ints are representative of states in the 
 * simulated Turing machine. A negative return value indicates a transition
 * to some other state to the caller, and all return values represent the next
 * cell to move to on the tape by its index (or the negative of its index)
 */

//decrement the first input x by 1
int takeOneX(vector<char> &tape, int idx) {
	if (tape[idx] == '0') {
		tape[idx] = '1';
		return idx + 1;
	}
	if (tape[idx] == '1') {
		tape[idx] = '0';
	}
	return -idx;
}

//currently in the first input's cells, keep going until reaching a blank
int addOneX(vector<char> &tape, int idx) {
	if (tape[idx] == 'B')
		return -idx - 1;
	return idx + 1;
}

//add 1 to the integer represented in the cells that originally hold the second input y
int addOneY(vector<char> &tape, int idx) {
	if (tape[idx] == '1') {
		tape[idx] = '0';
		return idx + 1;
	}
	if (tape[idx] == '0') {
		tape[idx] = '1';
		return -idx + 1;
	}
	//Blank case - expand the vector that represents the tape
	tape[idx] = '1';
	writeSafe(tape, 'B', idx+1);
	return -idx + 1;
}

//go left until a blank, then go left once more and transition
int getNextY(vector<char> &tape, int idx) {
	if (tape[idx] == 'B')
		return -idx + 1;
	return idx - 1;
}

//go left until a blank, then go right one time and transition
int getNextX(vector<char> &tape, int idx) {
	if (tape[idx] == 'B')
		return -idx - 1;
	return idx - 1;
}

/* given a pre-formed input tape, perform an addition and return the index of
 * the blank ('B') char that precedes the output
 */
int addSim(vector<char> &tape, string &trace, bool detailed=true) {
	int cur_idx = 1;
	if (detailed)
		trace += "State: Start\nCurrent index: 0\n";

	while (1) {
		//subtract 1 from x by traversing it from the left and changing bit values
		while (cur_idx > 0) {
			if (detailed)
				trace += string(tape.begin(), tape.end()) + "\n\nState: TakeOneX\nCurrent index: " + (to_string(cur_idx)) + "\n";
			cur_idx = takeOneX(tape, cur_idx);
		}
		cur_idx = -cur_idx;
		
		//Addition is finished in this case, since ending at a blank means all bits in place of the input are now 0
		if (tape[cur_idx] == 'B') {
			//x was subtracted from itself and added to the section of tape cells that contained y
			if (detailed) {
				trace += string(tape.begin(), tape.end()) + "\n\nState: Halt\nCurrent index: " + (to_string(cur_idx)) + "\n" + string(tape.begin(), tape.end());
				tape.erase(tape.begin(), tape.begin() + cur_idx);
			}
			return cur_idx;
		}
		
		//get to the single blank cell that separated the 2 inputs, then go right once to y
		while (cur_idx > 0) {
			if (detailed)
				trace += string(tape.begin(), tape.end()) + "\n\nState: AddOneX\nCurrent index: " + (to_string(cur_idx)) + "\n";
			cur_idx = addOneX(tape, cur_idx);
		}
		cur_idx = -cur_idx;
		
		//add the one subtracted from x to y
		while (cur_idx > 0) {
			if (detailed)
				trace += string(tape.begin(), tape.end()) + "\n\nState: AddOneY\nCurrent index: " + (to_string(cur_idx)) + "\n";
			cur_idx = addOneY(tape, cur_idx);
		}
		cur_idx = -cur_idx;
		
		//now go back to the separating blank and then left one more time to reach the x cells
		while (cur_idx > 0) {
			if (detailed)
				trace += string(tape.begin(), tape.end()) + "\n\nState: GetNextY\nCurrent index: " + (to_string(cur_idx)) + "\n";
			cur_idx = getNextY(tape, cur_idx);
		}
		cur_idx = -cur_idx;
		
		//go to the first cell for x to start the next iteration
		while (cur_idx >= 0) {
			if (detailed)
				trace += string(tape.begin(), tape.end()) + "\n\nState: GetNextX\nCurrent index: " + (to_string(cur_idx)) + "\n";
			cur_idx = getNextX(tape, cur_idx);
		}
		cur_idx = -cur_idx;
	}
}

//initialize a tape with 2 arguments x,y for add, with three blanks 
void initTape(int x, int y, vector<char> &tape) {
	//Bx...By...B
	vector<char> vec_x = binVector(x);
	vector<char> vec_y = binVector(y);
	tape.push_back('B');
	tape.insert(tape.end(), vec_x.begin(), vec_x.end());
	tape.push_back('B');
	tape.insert(tape.end(), vec_y.begin(), vec_y.end());
	tape.push_back('B');
}

//perform an addition in a simulated TM given the int args
vector<char> add(int x, int y, string &trace) {
	string str_x = to_string(x);
	string str_y = to_string(y);
	vector<char> tape;
	initTape(x, y, tape);
	trace = "Trace for " + str_x + " + " + str_y + "\n\n";
	addSim(tape, trace);
	return tape;
}

//given a tape of the form BxB, where x is the result, interpret it as an int
int interpretTapeRes(vector<char> tape) {
	tape.erase(tape.begin());
	tape.pop_back();
	return vecToInt(tape);
}

//make trace information string for one step in the exponentiation TM simulation
string traceStepExp(string state_name, int mult_idx, int exp_idx, vector<char> exp_tape, vector<char> mult_tape) {
	string step = "State: " + state_name + "\nCurrent Mult Index: " + (to_string(mult_idx)) +
		"\nCurrent Exp Index: " + (to_string(exp_idx)) + "\nCurrent Mult Tape: " +
		(string(mult_tape.begin(), mult_tape.end())) + "\nCurrent Exp Tape: " +
		(string(exp_tape.begin(), exp_tape.end())) + "\n\n";
	return step;
}

/* copy chars from read tape to write tape in order from the given indices in
 * each, up to and including the next blank (left to right)
 */
void copyUntilBlank(vector<char> &tape_read, vector<char> &tape_write, int &idx_r, int &idx_w) {
	while (tape_read[idx_r] != 'B') {
		if (idx_w >= (int)tape_write.size())
			tape_write.resize(idx_w + 1, 'B');
		tape_write[idx_w] = tape_read[idx_r];
		idx_r++;
		idx_w++;
	}
	if (idx_w >= (int)tape_write.size())
		tape_write.resize(idx_w+1, 'B');
	
	tape_write[idx_w] = 'B';
	idx_w--;
	idx_r--;
}

/* for add tape, moves from the last char before the last blank to the first
 * char after the first blank
 */
void moveToInputStart(vector<char> &tape, int &idx) {
	while (idx > 0)
		idx = getNextY(tape, idx);
	idx = -idx;
	while (idx >= 0)
		idx = getNextX(tape, idx);
	idx = -idx;
}

//see moveToInputStart (this tape stores 3 blank-separated values instead of 2)
void moveToMultStart(vector<char> &tape, int &idx) {
	while (idx > 0)
		idx = getNextY(tape, idx);
	idx = -idx;
	while (idx > 0)
		idx = getNextY(tape, idx);
	idx = -idx;
	while (idx >= 0)
		idx = getNextX(tape, idx);
	idx = -idx;
}

//make a string of information about the current state for the trace
string traceStep(string state_name, int add_idx, int mult_idx, vector<char> mult_tape, vector<char> add_tape) {
	string step = "State: " + state_name + "\nCurrent Add Index: " + (to_string(add_idx)) + 
		"\nCurrent Mult Index: " + (to_string(mult_idx)) + "\nCurrent Add Tape: " + 
		(string(add_tape.begin(), add_tape.end())) + "\nCurrent Mult Tape: " + 
		(string(mult_tape.begin(), mult_tape.end())) + "\n\n";
	return step;
}

//simulate a multiplication on a given mult tape and a blank add tape
int multSim(vector<char> &add_tape, vector<char> &tape, string &trace, bool detailed = true) {
	//move to the second cell after the blank in each tape to start
	int mult_idx = 1;
	int add_idx = 1;
	//multiply by repeated addition of x copies of y
	while (1) {
		//decrement x by 1
		if (detailed) {
			trace += traceStep("TakeOneInX", add_idx, mult_idx, tape, add_tape);
		}
		while (mult_idx > 0) {
			mult_idx = takeOneX(tape, mult_idx);
		}
		mult_idx = -mult_idx;
		if (tape[mult_idx] == 'B') {
			// when x=0 on the mult tape, the multiplication is finished
			//TakeOneInX moves right when the current cell has a blank and transitions
			mult_idx++;
			if (detailed)
				trace += traceStep("MoveToOutputFromY", add_idx, mult_idx, tape, add_tape);
			//position tape head at blank right before first bit of output
			while (mult_idx > 0)
				mult_idx = addOneX(tape, mult_idx);
			mult_idx = -mult_idx-1;
			if (detailed) {
				trace += traceStep("Halt", add_idx, mult_idx, tape, add_tape);
				tape.erase(tape.begin(), tape.begin() + mult_idx);
			}
			//return index of first cell of output
			return mult_idx+1;
		}
		if (detailed)
			trace += traceStep("MoveToYFromX", add_idx, mult_idx, tape, add_tape);
		//move to the separator blank between x and y, then move right one last time
		while (mult_idx > 0) {
			mult_idx = addOneX(tape, mult_idx);
		}

		//position mult tape head at first bit of y
		mult_idx = -mult_idx;
		if (detailed)
			trace += traceStep("WriteFirstAddArg", add_idx, mult_idx, tape, add_tape);
		//write y from the mult tape in the first arg position in the add tape
		copyUntilBlank(tape, add_tape, mult_idx, add_idx);
		//want the tape heads to be on the right side of their respective blanks
		mult_idx += 2;
		add_idx += 2;
		if (detailed)
			trace += traceStep("WriteSecondAddArg", add_idx, mult_idx, tape, add_tape);
		//make the result so far from the mult tape the second arg on the add tape
		copyUntilBlank(tape, add_tape, mult_idx, add_idx);

		if (detailed)
			trace += traceStep("MoveToBeginAdd", add_idx, mult_idx, tape, add_tape);
		//send tape head in add tape to first bit of its first input after copying result
		moveToInputStart(add_tape, add_idx);

		if (detailed)
			trace += traceStep("Add*", add_idx, mult_idx, tape, add_tape);
		//compute the sum whose input args are on the add tape, and increment position to first bit of answer sum
		add_idx = addSim(add_tape, trace, false) + 1;

		if (detailed)
			trace += traceStep("MoveToOutputStartFromEnd", add_idx, mult_idx, tape, add_tape);
		//position mult tape head at the first cell of where the result goes from the end cell
		while (mult_idx >= 0)
			mult_idx = getNextX(tape, mult_idx);

		//mult's tape head at first bit of old result
		mult_idx = -mult_idx;

		if (detailed)
			trace += traceStep("WriteSumBack", add_idx, mult_idx, tape, add_tape);
		//write new sum from the add tape over old one on the mult tape
		copyUntilBlank(add_tape, tape, add_idx, mult_idx);

		if (detailed)
			trace += traceStep("FirstCellFromEndAddTape*", add_idx, mult_idx, tape, add_tape);
		//move tape heads back to initial positions for beginning of next iteration
		moveToInputStart(add_tape, add_idx);
		if (detailed)
			trace += traceStep("FirstCellFromEndMultTape*", add_idx, mult_idx, tape, add_tape);
		moveToMultStart(tape, mult_idx);
	}
}

//multiply 2 input ints x and y by inputting them to a simulated 2-tape TM
vector<char> mult(int x, int y, string &trace, bool detailed=true) {
	vector<char> tape;
	vector<char> add_tape;
	initTape(x, y, tape);
	//add another value to the mult tape representing the output (0 for now)
	tape.push_back('0');
	tape.push_back('B');
	string str_x = to_string(x);
	string str_y = to_string(y);
	if (detailed)
		trace = "Abbreviated Trace for " + str_x + " x " + str_y + "\n\n";
	
	//add tape initially blank
	add_tape.push_back('B');
	if (detailed)
		trace += "State: Start\nCurrent Add Index: 0\nCurrent Mult Index: 0\nInitial Add Tape: " + (string(add_tape.begin(), add_tape.end())) + "\nInitial Mult Tape: " + (string(tape.begin(), tape.end())) + "\n\n";

	multSim(add_tape, tape, trace);
	return tape;
}

//simulate an exponentiation TM given input tapes
int expSim(vector<char> &mult_tape, vector<char> &tape, string &trace) {
	int mult_idx = 1;
	int exp_idx = 1;

	//only mult will use this tape
	vector<char> add_tape;
	add_tape.push_back('B');

	while (1) {
		trace += traceStepExp("MoveToYFromXForDecrement", mult_idx, exp_idx, tape, mult_tape);
		//move from x's position on the exp tape to y's leftmost cell so that it can be decremented
		while (exp_idx > 0)
			exp_idx = addOneX(tape, exp_idx);
		exp_idx = -exp_idx;

		//decrement y
		trace += traceStepExp("DecrementY", mult_idx, exp_idx, tape, mult_tape);
		while (exp_idx > 0) {
			exp_idx = takeOneX(tape, exp_idx);
		}
		
		//tape is at position of the tape where a 1 was changed to a 0, or a blank if there wasn't one in y
		exp_idx = -exp_idx;
		

		//if a blank was reached in DecrementY, then the result cells have the final output
		if (tape[exp_idx] == 'B') {
			//DecrementY transitions to halt state and moves one right upon encountering a blank
			exp_idx++;
			trace += traceStepExp("Halt", mult_idx, exp_idx, tape, mult_tape);
			tape.erase(tape.begin(), tape.begin() + (exp_idx - 1));
			//return the position of the last bit of the second input before changing the tape
			return exp_idx - 2;
		}
		
		//otherwise, go to the leftmost cell of the section of tape that stores the result so far
		trace += traceStepExp("MoveToResFromY", mult_idx, exp_idx, tape, mult_tape);
		while (exp_idx > 0)
			exp_idx = addOneX(tape, exp_idx);
		exp_idx = -exp_idx;

		//Write the result as the first arg on the multiplication tape, plus a blank afterward
		trace += traceStepExp("WriteResAsMultArg", mult_idx, exp_idx, tape, mult_tape);
		copyUntilBlank(tape, mult_tape, exp_idx, mult_idx);
		
		//Next arg on mult tape will start after the new blank, not before it
		mult_idx += 2;

		//Now x will be the second arg on the mult tape. Go left to the rightmost bit of y first...
		trace += traceStepExp("MoveToYForXWrite", mult_idx, exp_idx, tape, mult_tape);
		while (exp_idx > 0)
			exp_idx = getNextY(tape, exp_idx);
		exp_idx = -exp_idx;

		//...and then go left to the rightmost bit of x, and then...
		trace += traceStepExp("MoveToXForXWrite", mult_idx, exp_idx, tape, mult_tape);
		while (exp_idx > 0)
			exp_idx = getNextY(tape, exp_idx);
		exp_idx = -exp_idx;

		//finally go left to the first cell of x to begin write to mult tape
		trace += traceStepExp("MoveToXBeginFromXEndForXWrite", mult_idx, exp_idx, tape, mult_tape);
		while (exp_idx >= 0)
			exp_idx = getNextX(tape, exp_idx);
		exp_idx = -exp_idx;

		//write x to the mult tape at the second arg position
		trace += traceStepExp("WriteXAsMultArg", mult_idx, exp_idx, tape, mult_tape);
		copyUntilBlank(tape, mult_tape, exp_idx, mult_idx);
		
		/* WriteXAsMultArg should move exp tape head right once to leftmost bit of y from blank that causes transition
		 * instead of left once as in copyUntilBlank function default
		 */
		exp_idx += 2;

		//WriteXAsMultArg should similarly move mult tape head to cell after the blank
		mult_idx += 2;
		
		//now write the initial result cell to the mult tape, move right, and transition
		trace += traceStepExp("InitMultRes", mult_idx, exp_idx, tape, mult_tape);
		writeSafe(mult_tape, '0', mult_idx);
		mult_idx++;
		
		//write a blank (since the mult tape is reused, some unwanted bit characters could be here) and move left
		trace += traceStepExp("MakeLastBlankInMult", mult_idx, exp_idx, tape, mult_tape);
		writeSafe(mult_tape, 'B', mult_idx);
		mult_idx--;

		/* this state moves left for non-blank characters, and moves left and transitions for blanks,
		 * to reach the rightmost cell of the second input on the mult tape
		 */
		trace += traceStepExp("MultMoveToYAfterInitResWrite", mult_idx, exp_idx, tape, mult_tape);
		while (mult_idx > 0)
			mult_idx = getNextY(mult_tape, mult_idx);
		mult_idx = -mult_idx;

		//now go back to the last bit of the first input on the mult tape...
		trace += traceStepExp("ToFirstInputEndInMultAfterXWrite", mult_idx, exp_idx, tape, mult_tape);
		while (mult_idx > 0)
			mult_idx = getNextY(mult_tape, mult_idx);
		mult_idx = -mult_idx;

		//and finally go to the first bit of the first input to start the multiplication
		trace += traceStepExp("ToFirstInputBeginInMultAfterArgWrites", mult_idx, exp_idx, tape, mult_tape);
		while (mult_idx >= 0)
			mult_idx = getNextX(mult_tape, mult_idx);
		mult_idx = -mult_idx;

		//Perform the multiplication (treat the entire process and all its state transitions as a black box in trace)
		trace += traceStepExp("Mult*", mult_idx, exp_idx, tape, mult_tape);
		mult_idx = multSim(add_tape, mult_tape, trace, false);

		//Now to copy the product, need to move exp tape's head first from its current position, at the leftmost cell of y,
		//to the leftmost result cell
		trace += traceStepExp("ToExpResultForUpdate", mult_idx, exp_idx, tape, mult_tape);
		while (exp_idx > 0)
			exp_idx = addOneX(tape, exp_idx);
		exp_idx = -exp_idx;

		//Now that the tape heads are positioned correctly, write the product to the result cells on the exp tape
		trace += traceStepExp("WriteProduct", mult_idx, exp_idx, tape, mult_tape);
		copyUntilBlank(mult_tape, tape, mult_idx, exp_idx);

		/* All that's left is to move to the first cell of the first arg to start the next iteration in each tape
		 * As in the multSim, this abbreviates 3 states within one (the combined result is going left until 3 blanks
		 * have been seen, then positioning the tape head 1 position right of the 3rd blank
		 */
		trace += traceStepExp("FirstCellFromEndExpTape*", mult_idx, exp_idx, tape, mult_tape);
		moveToMultStart(tape, exp_idx);

		trace += traceStepExp("FirstCellFromEndMultTape*", mult_idx, exp_idx, tape, mult_tape);
		moveToMultStart(mult_tape, mult_idx);
	}
}

//simulate exponentiation x^y in a Turing machine for inputs x and y
vector<char> exp(int x, int y, string &trace) {
	vector<char> tape;
	vector<char> mult_tape;
	
	//exp tape stores inputs x and y, and a result (which starts as 1)
	initTape(x, y, tape);
	tape.push_back('1');
	tape.push_back('B');

	//mult_tape is entirely blank initially
	mult_tape.push_back('B');

	string str_x = to_string(x);
	string str_y = to_string(y);

	/* only mult tape and exp tape will appear in trace - all modifications and accesses to the add are lost
	* in the abbreviated trace format since they're now less "interesting"
	*/
	trace = "Abbreviated Trace for " + str_x + " ^ " + str_y + " (x to the yth power)\n\n";
	trace += "State: Start\nCurrent Mult Index: 0\nCurrent Exp Index: 0\nInitial Mult Tape: " + (string(mult_tape.begin(), mult_tape.end())) + "\nInitial Exp Tape: " + (string(tape.begin(), tape.end())) + "\n\n";
	
	expSim(mult_tape, tape, trace);
	return tape;
}

int main(int argc, char *argv[])
{   
	if (argc != 4)
		return 0;
	string trace;
	ofstream outfile;
	int x = atoi(argv[2]);
	int y = atoi(argv[3]);
	string operation = argv[1];
	string res;
	string filename;
	vector<char> tape;
	string str_x = to_string(x);
	string str_y = to_string(y);

	if (operation == "-add") {
		tape = add(x, y, trace);
		filename = "add_" + str_x + "_" + str_y;
	}
	else if (operation == "-mult") {
		tape = mult(x, y, trace);
		filename = "mult_" + str_x + "_" + str_y;
	}
	else if (operation == "-exp") {
		tape = exp(x, y, trace);
		filename = "exp_" + str_x + "_" + str_y;
	}
	else
		return 0;
	
	outfile.open(filename);
	res = to_string(interpretTapeRes(tape));
	outfile << trace + "\n\nInterpreted result of this computation: " + res;
	std::cout << "Created trace file \'" + filename + "\'\nResult: " + res + "\n";
	return 0;
}


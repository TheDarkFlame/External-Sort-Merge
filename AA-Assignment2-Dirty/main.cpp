#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <chrono>

using namespace std;
using namespace std::chrono;
typedef vector<pair<int, int>> pairvector;
int MAX_READS = 50000;//global variable, dealing with 50k reads should keep the max number of entries below 1MB
int FILE_SIZE = -1;//ignore this param
int ELEMENT_COUNT = 0;//total element count
bool read_in(int pairs_to_read, pairvector & output, ifstream & file){
	pair<int, int>temp;
	int j;
	for (int i = 0; i < pairs_to_read; i++){
		j = i+1;
		file >> temp.first;
		file >> temp.second;
		if(file.good())
			output.push_back(temp);
		else return false;//if file.good()=false return false, we are at EOF.
	}
	return true;//if file.good()=true for all entries return true, we have not met EOF
}

void write_out(pairvector & input, string filename){
	ofstream file(filename, fstream::app);
	pair<int, int>temp;
	while (!input.empty()){//loop until input is empty
		temp = input.back();
		file << temp.first << " ";
		file << temp.second << endl;
		input.pop_back();
	}
}

bool custom_compare_function(pair<int, int>arg1, pair<int, int>arg2){//returns true if arg1>arg2
	if (arg1.first == arg2.first)
		return arg1.second < arg2.second;
	else return arg1.first > arg2.first;
}

string filename(int level, int filenumber){
	//filename in the format of vvnnnn vv=level nnnn=number within the level
	stringstream filename;
	filename.width(2);
	filename.fill('0');
	filename << level;
	filename.width(4);
	filename.fill('0');
	filename << filenumber;
	filename << ".txt";
	return filename.str();
}

void push_until_empty(pairvector &input1, pairvector &input2, pairvector &output){//merges until one input empties
	//takes in 2 vectors with smallest element at back, and outputs with smallest element at back
	if (input1.empty())
		return;
	if (input2.empty())
		return;
	
	pair<int, int>pair1;
	pair<int, int>pair2;
	pair1 = input1.back();
	pair2 = input2.back();

	while (true){		//insert lowest element
		//insert pair2
		if (custom_compare_function(pair1, pair2)){//if pair1>pair2
			output.push_back(pair2);
			input2.pop_back();
			if (input2.empty()){
				reverse(output.begin(), output.end());
				return;//if input2 is empty, return it so we know it is empty
			}
			else
				pair2 = input2.back();//else re-assign pair2 to back of input2
		}
		//insert pair1
		else{
			output.push_back(pair1);
			input1.pop_back();
			if (input1.empty()){
				reverse(output.begin(), output.end());
				return;//if input1 is empty, return it so we know it is empty
			}
			else
				pair1 = input1.back();//else re-assign pair1 to back of input1
		}
	}
}

void myMerge(string filename1,string filename2,string filename_output){
	ifstream file1(filename1);
	ifstream file2(filename2);
	pairvector input_vector_1;
	pairvector input_vector_2;
	pairvector output_vector;
	pairvector empty_vector;//empty array always assigned to the empty array, used for comparison
	bool file1good;
	bool file2good;
	file1good = read_in(MAX_READS / 4, input_vector_1, file1);//read in, and store the file.good() bool
	reverse(input_vector_1.begin(), input_vector_1.end());//get lowest number to back
	file2good = read_in(MAX_READS / 4, input_vector_2, file2);//read in, and store the file.good() bool
	reverse(input_vector_2.begin(), input_vector_2.end());//get lowest number to back
	//if file1good=true, we know we did not reach EOF for file1, etc for file2
	
	while (true){
		push_until_empty(input_vector_1, input_vector_2, output_vector);//merge into output until an input empties
		write_out(output_vector, filename_output);//write out the output_vector
		//handle vector1 empty
		if (input_vector_1.empty()){//push until empty, test if vector1
			if (file1good == false){
				//vector1 is empty, and file 1 is empty, so write the entirety of vector2 then the rest of file2
				do{
					write_out(input_vector_2, filename_output);//write the remainder of vector2
					file2good = read_in(MAX_READS / 4, input_vector_2, file2);//read in remainder of file2 into vector2
					reverse(input_vector_2.begin(), input_vector_2.end());//get lowest number to back
				} while (file2good == true);
				return;//we are done with these files, exit the merge
			}
			else{
				//vector1 is empty, but file1 still has items, so populate vector1 with them
				file1good = read_in(MAX_READS / 4, input_vector_1, file1);
				reverse(input_vector_1.begin(), input_vector_1.end());//get lowest number to back
			}
		}
		//handle vector2 empty
		else{//if is implied that vector2 is empty
			if (file2good == false){
				//vector2 is empty, and file 2 is empty, so write the entirety of vector1 then the rest of file1
				do{
					write_out(input_vector_1, filename_output);//write the remainder of vector2
					file1good = read_in(MAX_READS / 4, input_vector_1, file1);//read in remainder of file2 into vector2
					reverse(input_vector_1.begin(), input_vector_1.end());//get lowest number to back
				} while (file1good == true);
				return;//we are done with these files, exit the merge
			}
			else{
				//vector2 is empty, but file2 still has items, so populate vector2 with them
				file2good = read_in(MAX_READS / 4, input_vector_2, file2);
				reverse(input_vector_2.begin(), input_vector_2.end());//get lowest number to back
			}
		}
	}
}

bool within_limits(int total_file_count){//this function controls reading in only parts of a input file
	if (FILE_SIZE == -1)//if filesize=-1 we read the entire file
		return true;
	else if ((total_file_count+1)*MAX_READS < FILE_SIZE)//if (maxreads+1)*total_file_count< filesize, next read will be OK
		return true;
	else return false;
}

int main(int argc, char* argv[]){
	//http://www.cplusplus.com/articles/DEN36Up4/
	if (argc > 3) {
		// Tell the user how to run the program
		std::cerr << "Usage: " << argv[0] << " <INPUTS TO BE READ FROM FILE> <MAXIMUM ENTRIES IN MEMORY>" << std::endl;
		return 1;
	}
	if (argc == 3){
		FILE_SIZE = strtol(argv[1], NULL, 10);
		MAX_READS = strtol(argv[2], NULL, 10);
	}

	//http://stackoverflow.com/questions/22387586/measuring-execution-time-of-a-function-in-c
	high_resolution_clock::time_point tick = high_resolution_clock::now();//measure time before process
	pairvector working_vector;
	int total_file_count = 0;
	int level = 0;
	bool next_state=true;//have we reached EOF in the current read?
	ifstream file("input.txt");
	while (within_limits(total_file_count)&&next_state){//if 1st condition is false, second will not be executed, we rely on this
		next_state = read_in(MAX_READS, working_vector, file);
		sort(working_vector.begin(), working_vector.end(), custom_compare_function);
		ELEMENT_COUNT += working_vector.size();//keep track of number of elements we are working with for statistics
		write_out(working_vector, filename(level, total_file_count++));//level 0, with filesegmentation
		//enable limit to number of file reads
	}
	if (!(within_limits(total_file_count))){
		//if we just exited the above while loop due to the filecount being exceeded, do a read of whatever is left of make up the difference
		read_in(FILE_SIZE - total_file_count*MAX_READS, working_vector, file);
		sort(working_vector.begin(), working_vector.end(), custom_compare_function);
		ELEMENT_COUNT += working_vector.size();//keep track of number of elements we are working with for statistics
		write_out(working_vector, filename(level, total_file_count++));//level 0, with filesegmentation
	}
	level++;

	//begin sorting the individual files
	while (total_file_count > 1){
		int j = 0;
		for (int i = 0; i < total_file_count - (total_file_count % 2); i += 2){//go up to last set of files
			
			//get 2 files from previous level, and write to 1 file from current level
			myMerge(filename(level - 1, i), filename(level - 1, i + 1), filename(level, j++));
			//perform file cleanup
			remove(filename(level - 1, i).c_str());
			remove(filename(level - 1, i + 1).c_str());
		}
		//modify filecount and level
		if (total_file_count % 2){							//if odd number of files
			//move the odd(last) file up a level
			rename(filename(level - 1, total_file_count - 1).c_str(), filename(level, j).c_str());//totalfilecount=end of level-1, j=end of level
			total_file_count = total_file_count / 2 + 1;	//filecount=half+1
		}
		else												//even number of files
			total_file_count /= 2;							//filecount=half
		level++;
	}

	//correctly name output
	remove("output.txt");
	rename(filename(level - 1, 0).c_str(), "output.txt");
	high_resolution_clock::time_point tock = high_resolution_clock::now();//measure time before process
	long long duration = duration_cast<milliseconds>(tock - tick).count();
	cout << ELEMENT_COUNT<< "," << duration << endl;
	return 0;
}
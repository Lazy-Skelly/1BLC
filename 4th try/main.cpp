#include <fstream>
#include <string>
#include <map>
#include <math.h>
#include <thread>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <chrono>
#include <iostream>

struct statistic{
	int number =0;
	float max =-200;
	float min =200;
	float sum =0;
};

std::condition_variable condition;
std::mutex mtx;
std::queue<std::string> buffer;
std::map<std::string, statistic> stations;
bool done = false;
bool ready = false;

void Reader(){
	std::string line;
	std::fstream file("data/measurements.txt");
	if(file.is_open()){
		while(getline(file,line)){
			ready = false;
			std::lock_guard<std::mutex> lock(mtx);
			buffer.push(line);
			ready = true;
			condition.notify_one();
		}
	}
	done = true;
	condition.notify_all();
}

void Decoder(){
	std::string line;
	std::string name;
	statistic* temp;
	std::queue<std::string> inside;
	float now;
	while(!done){
		if(!ready){
			std::unique_lock<std::mutex> lock(mtx);
			condition.wait(lock);
			inside.swap(buffer);
			
		}else{
			std::unique_lock<std::mutex> lock(mtx);
			inside.swap(buffer);
		}
		ready = false;
		while(!inside.empty()){
			line = inside.front();
			inside.pop();
			int i = line.find(";");
			name = line.substr(0,i);  		     		
       		temp = &stations[name];
       		temp->number++;
       		now = stof(line.substr(i+1));
       		temp->sum += now;
       		
			if(temp->min > now){
				temp->min = now;
			}else if(temp->max < now){
				temp->max = now;
			}
		}		
	}
}


int main(int argc, char** argv) {

	
	statistic temp;
	float now;
    std::string line;
    std::string name;
    
    std::thread R(Reader);
    
     std::vector<std::thread> threads;

	for (int i=1; i<=5; ++i)
    threads.push_back(std::thread(Decoder));
    
    R.join();
    
    for (auto& th : threads) th.join();
    
    std::fstream file;
    file.open("output.txt"); 
    file << "{"; 
    for (const auto& [name, st] : stations){
        file << name << "=" << round(st.min*10)/10<< "/" << round(st.sum / st.number *10)/10 << "/" << round(st.max *10)/10 <<",";
	}
	file << "}";
	file.close();
    
	
	return 0;
}

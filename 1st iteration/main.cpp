#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <math.h>

struct statistic{
	int number =0;
	float max =0;
	float min =0;
	float sum =0;
};


int main(int argc, char** argv) {

	std::map<std::string, statistic> stations;
	
	statistic temp;
	float now;
    std::string line;
    std::string name;
    std::fstream file("../data/measurements.txt");
    if (file.is_open()) {
       	while(getline(file, line)){
       		int i = line.find(";");
			name = line.substr(0,i);  		     		
       		temp = stations[name];
       		temp.number++;
       		now = stof(line.substr(i+1));
       		temp.sum += now;
       		if(temp.number == 1){
       			temp.min = now;
			  	temp.max = now;
			}else{
				if(temp.min > now){
					temp.min = now;
				}else if(temp.max < now){
					temp.max = now;
				}
			} 
			stations[name] = temp;  		
		}
        file.close();
    }
    
    file.open("output.txt"); 
    file << "{"; 
    for (const auto& [name, st] : stations){
        file << name << "=" << round(st.min*10)/10<< "/" << round(st.sum / st.number *10)/10 << "/" << round(st.max *10)/10 <<",";
	}
	file << "}";
	file.close();	
	
	return 0;
}

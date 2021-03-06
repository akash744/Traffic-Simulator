#include "TrafficModel.h"
#include<iostream>

TrafficModel::TrafficModel() { }
TrafficModel::~TrafficModel(){ 
  for (int i = 0; i < platoons.size(); i++)
	{
		Car* c = platoons[i].get_tail();
		while (c != NULL)
		{
			Car* traversal = c->get_prev();
			delete c;
			c = traversal;
		}
	}

}

void TrafficModel::set_commands(vector<string> commands)
{
	this->commands = commands;
}

/* A helper function. 
 * Given a specific id as the input, it searches the lane change command stored in
 * "commands". Returns the value of the turn-light in the command for this car id.
 * 0 = no light, 1 = left light, 2 = right light.
 */
int TrafficModel::get_lane_change_command(int id)
{
	int count = commands.size();
	for (int i = 0; i < count; i++){
		size_t found = commands[i].find(',');
		string iter_id = commands[i].substr(0, found);
		string light = commands[i].substr(found+1, commands[i].size()-1);
		stringstream ss1(iter_id);
		int id_value;
		ss1 >> id_value;
		if (id_value == id){
			stringstream ss2(light);
			int light_value;
			ss2 >> light_value;
			return light_value;
		}
	}
	return 0;
}

/*
 * The function that updates the vehicle positions and states.
 */
void TrafficModel::update()
{
  int lane_count = platoons.size();
  vector<Car*> errors_vector;

  for(unsigned int i = 0; i < lane_count; i++){

    Car* cars = platoons[i].get_tail();
    Car* copy_car = NULL;

    if (cars != NULL)
		{
			copy_car = cars->get_prev();
		}

    while(cars != NULL){

      if(this->prevent_right_update_twice(cars, errors_vector) == 0){
        if(this->get_lane_change_command(cars->get_id()) == 0){
          this->move_car_forward(cars);               // Moves Car Forward
        }
        else if(this->get_lane_change_command(cars->get_id()) == 1){
          this->change_lane_left(cars, i);            // Moves Car Left
        }
        else if(this->get_lane_change_command(cars->get_id()) == 2){
          this->change_lane_right(cars, i);           // Moves Car Right
          errors_vector.push_back(cars);
        }
      }
      
      cars = copy_car;
			if (copy_car != NULL)
			{
				copy_car = copy_car->get_prev(); 
			}
    }
  }
}

// Helper Function: Moves Car Forward
void TrafficModel::move_car_forward(Car* c){

  Car* space_in_front = c->get_next();
  int curr_position = c->get_position();

  if(space_in_front == NULL){
    curr_position += 1;
    c->set_position(curr_position);
  }
  else if (space_in_front->get_position() - c->get_position() != 1) 
	{
		curr_position += 1;
    c->set_position(curr_position);
	}
}

//Helper Function: Check free space returns 0 if true, 1 if false
int TrafficModel::check_free_space(Car* c, int current_position, int lane){
  
  int traversal_position = 0;
  Car* temp_head_node = platoons[lane].get_head();

  while(temp_head_node != NULL){

    traversal_position = temp_head_node->get_position();
    if(traversal_position == current_position){
      return 0;
    }
    
    temp_head_node = temp_head_node->get_next();
  }
  return 1;
}

//Helper Function: Change to Left Lane
void TrafficModel::change_lane_left(Car* c, int current_lane){

  Car* copy_car = c;

  if(current_lane != 0){

    int current_position = c->get_position();
    int new_lane = current_lane - 1;
    int lane_change_allowed = this->check_free_space(c, current_position,new_lane);

    if(lane_change_allowed == 1){
      platoons[current_lane].remove(c);
      platoons[new_lane].insert(copy_car); 
    }
    else{
      this->move_car_forward(c);
    }

  }
  else{
    this->move_car_forward(c);
  }
}

//Helper Function: Change to Right Lane
void TrafficModel::change_lane_right(Car* c, int current_lane){

  Car* copy_car = c;

  if(current_lane != platoons.size() - 1){

    int current_position = c->get_position();
    int new_lane = current_lane + 1;
    bool lane_change_allowed = this->check_free_space(c, current_position,new_lane);

    if(lane_change_allowed == 1){
      platoons[current_lane].remove(c);
      platoons[current_lane + 1].insert(copy_car);
    }
    else{
      this->move_car_forward(c);
    }

  }
  else{
    this->move_car_forward(c);
  }
}

//Helper Function: To prevent right lane change twice returns 1 if true, 0 if false
int TrafficModel::prevent_right_update_twice(Car* c, vector<Car*> errors_vector)
{
	for (int i = 0; i < errors_vector.size(); i++)
	{
		if (c == errors_vector[i])
		{
			return 1;
		}
	}
	return 0;
}


/*
 * Initialization based on the input information
 */
void TrafficModel::initialize(vector<string> info)
{
	int lane_count = info.size();
	for (int i = 0; i < lane_count; i++){
		Platoon p = Platoon(info[i]);
		platoons.push_back(p);
	}
}

//
// IMPORTANT: DO NOT CHANGE THE FUNCTIONS BELOW THIS LINE
//

// Returns all the vehicle states in the system
vector<string> TrafficModel::get_system_state()
{
	vector<string> output;
	int size = platoons.size();
	for (int i = 0; i < size; i++){
		// get the last vehicle in the platoon
		Car* temp = platoons[i].get_tail();
		string s = "";
		ostringstream out;
		while (temp != NULL){
			out << ";(" << temp->get_id() << "," << i << "," << temp->get_position() << \
					 "," << get_lane_change_command(temp->get_id()) << ")";
			temp = temp->get_prev();
		}

		output.push_back(out.str());
	}
	return output;
}

//Get the state of cars as a 3D vector representing car data in lane/pos
vector<vector<vector<int>>> TrafficModel::get_car_state(){

	vector<string> state = get_system_state();
	vector<vector<vector<int>>> cars;
	string remainingCars;
	string newCar;

	//Parse state into vector of car data
	for (unsigned int i = 0; i < state.size(); i++){
		vector<vector<int>> carRow;
		remainingCars = state[i];
		remainingCars.push_back(';');

		//Parse string of entire lane into individual car data
		while (remainingCars.size() > 1) {
			remainingCars = remainingCars.substr(1);
			size_t pos = remainingCars.find(";");
			newCar = remainingCars.substr(1,pos-2);

			carRow.push_back(parse_car_data(newCar));

			if(pos!=string::npos){
				remainingCars = remainingCars.substr(pos);
			} else {
				remainingCars = "";
			}
		}
		cars.push_back(carRow);
	}
	return cars;
}

//Parse string in form (id,lane,pos,turn) into vector of ints
vector<int> TrafficModel::parse_car_data (string& car){
	vector<int> carData;
	string delimiter = ",";
	size_t last = 0;
	size_t next = 0;
	int index = 0;

	while ((next = car.find(delimiter, last)) != string::npos) {
		carData.push_back(stoi(car.substr(last, next-last)));
		last = next + 1;
		index++;
	}
	carData.push_back(stoi(car.substr(last)));
	return carData;
}
